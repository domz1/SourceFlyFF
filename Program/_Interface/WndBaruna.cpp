#include "WndBaruna.h"
#include "stdafx.h"
#include "resData.h"
#include "DPClient.h"
#include "WndManager.h"
#include "defineText.h"

extern CDPClient g_DPlay;

#ifdef __BARUNA_V18
/***************************************
*
*	APP_BARUNA_ELEMENT_SMELT
*
****************************************/

CWndBarunaElement::CWndBarunaElement()
{
	m_nDelay = 2; //Sekunden die der Balken braucht
	m_fGaugeRate = 0;

	for( int i=0;i<4;i++ )
		m_pItemElem[i] = NULL;

	m_nSmeltWidth = -1;
	m_pVBSmeltGauge = NULL;
	m_bVBSmeltGauge = FALSE;
	m_bStart = FALSE;
	m_dwStartTime = m_dwEndTime = 0xffffffff;

}

CWndBarunaElement::~CWndBarunaElement()
{
	DeleteDeviceObjects();
}

HRESULT CWndBarunaElement::RestoreDeviceObjects()
{
	CWndBase::RestoreDeviceObjects();

	m_pApp->m_pd3dDevice->CreateVertexBuffer( sizeof( TEXTUREVERTEX2 ) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pVBSmeltGauge, NULL );
	m_nSmeltWidth = -1;
	m_texGauEmptyNormal.SetInvalidate(m_pApp->m_pd3dDevice); 

	return S_OK;
}

HRESULT CWndBarunaElement::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
	SAFE_RELEASE( m_pVBSmeltGauge );
	m_texGauEmptyNormal.Invalidate(); 
	return S_OK;
}

HRESULT CWndBarunaElement::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	return InvalidateDeviceObjects();
}

BOOL CWndBarunaElement::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	CRect rect;
	CItemElem* pItemElem;
	pItemElem = (CItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );

	if(g_pPlayer->IsUsing(pItemElem))
	{
		g_WndMng.PutString( "Item wird bereits verwendet" );
	}
	else
	{
		if( pItemElem->GetProp()->IsBaruna() && pItemElem->m_nResistAbilityOption < 20 )	
		{
			if( /*m_nCtrlId[0].PtInRect( point ) &&*/ m_pItemElem[0] == NULL )
			{
				if( pItemElem->m_bItemResist > 0 && m_pItemElem[1] && m_pItemElem[1]->GetProp()->eItemType != pItemElem->m_bItemResist )
					return FALSE;

				m_pItemElem[0] = pItemElem;
				m_pItemElem[0]->SetExtra(m_pItemElem[0]->GetExtra()+1);
			}
		}
		else if( pItemElem->GetProp()->dwItemKind3 == IK3_ELEORB )
		{
			if( /*m_nCtrlId[1].PtInRect( point ) &&*/ m_pItemElem[1] == NULL )
			{
				if( m_pItemElem[0] != NULL && m_pItemElem[0]->m_bItemResist > 0 && pItemElem->GetProp()->eItemType != m_pItemElem[0]->m_bItemResist )
					return FALSE;

				m_pItemElem[1] = pItemElem;
				m_pItemElem[1]->SetExtra(m_pItemElem[1]->m_nItemNum);
			}
		}
		else if( pItemElem->GetProp()->dwItemKind3 == IK3_BARUNA_ELE_PROPTECTION || pItemElem->GetProp()->dwItemKind3 == IK3_BARUNA_ELE_PROPTECTION_KEEP )
		{
			if( /*m_nCtrlId[2].PtInRect( point ) &&*/ m_pItemElem[2] == NULL )
			{
				m_pItemElem[2] = pItemElem;
				m_pItemElem[2]->SetExtra(m_pItemElem[2]->m_nItemNum);
			}
		}
		else if( pItemElem->GetProp()->dwItemKind3 == IK3_BARUNA_ELE_PROP_INCREASE )
		{
			if( /*m_nCtrlId[3].PtInRect( point ) &&*/ m_pItemElem[3] == NULL )
			{
				m_pItemElem[3] = pItemElem;
				m_pItemElem[3]->SetExtra(m_pItemElem[3]->m_nItemNum);
			}
		}
	}
	if( m_pItemElem[0] != NULL && m_pItemElem[1] != NULL )
	{
		CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BT_OK);
		pButton->EnableWindow(TRUE);
	}
	return TRUE;
}

BOOL CWndBarunaElement::Initialize( CWndBase* pWndParent, DWORD nType )
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_BARUNA_ELEMENT_SMELT, 0, CPoint( 0, 0 ), pWndParent );
}

BOOL CWndBarunaElement::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	if( nID == WIDC_BT_OK )
	{
		if( m_bStart )
			return TRUE;

		m_bStart = TRUE;
		m_dwStartTime = g_tmCurrent;
		m_dwEndTime = g_tmCurrent + SEC(m_nDelay);

	}
	else if( nID == WIDC_BT_CANCEL )
	{
		if( m_bStart )
		{
			m_bStart = FALSE;
		}
		else
			Destroy();
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

BOOL CWndBarunaElement::Process()
{
	if(m_bStart && m_pItemElem[0] != NULL)
	{
		m_fGaugeRate = static_cast<float>(g_tmCurrent - m_dwStartTime) / static_cast<float>(m_dwEndTime - m_dwStartTime);
		
		if(m_dwEndTime < g_tmCurrent )
		{
			m_bStart = FALSE;
			m_dwStartTime = 0xffffffff;
			m_dwEndTime = 0xffffffff;

			// Send to Server...
			if( m_pItemElem[2] == NULL && m_pItemElem[0]->GetExtra() > 0 )
			{
/*				if( g_WndMng.GetWndBase( APP_SMELT_SAFETY_CONFIRM ) )
				{
					if( g_WndMng.m_pWndBarunaEnchantConfirm )
					{
						g_WndMng.m_pWndBarunaEnchantConfirm->Destroy();
						SAFE_DELETE( g_WndMng.m_pWndBarunaEnchantConfirm );
					}
					if( g_WndMng.m_pWndSmeltSafetyConfirm )
					{
						g_WndMng.m_pWndSmeltSafetyConfirm->Destroy();
						SAFE_DELETE( g_WndMng.m_pWndSmeltSafetyConfirm )
					}
				}
				g_WndMng.m_pWndBarunaEnchantConfirm = new CWndBarunaEnchantConfirm( CWndBarunaEnchantConfirm::WND_ERROR3 );
				g_WndMng.m_pWndBarunaEnchantConfirm->SetWndMode( m_pItemElem );
				g_WndMng.m_pWndBarunaEnchantConfirm->Initialize();*/

				m_pItemElem[1]->SetExtra( m_pItemElem[1]->GetExtra()-1 );
				if( m_pItemElem[2] != NULL )
					m_pItemElem[2]->SetExtra( m_pItemElem[2]->GetExtra()-1 );
				if( m_pItemElem[3] != NULL )
					m_pItemElem[3]->SetExtra( m_pItemElem[3]->GetExtra()-1 );
			}
			else if( m_pItemElem[0]->GetExtra() > 0 )
			{
				g_DPlay.SendBarunaElement( m_pItemElem );

				m_pItemElem[1]->SetExtra( m_pItemElem[1]->GetExtra()-1 );
				if( m_pItemElem[2] != NULL )
					m_pItemElem[2]->SetExtra( m_pItemElem[2]->GetExtra()-1 );
				if( m_pItemElem[3] != NULL )
					m_pItemElem[3]->SetExtra( m_pItemElem[3]->GetExtra()-1 );
				if( m_pItemElem[0]->GetExtra() > 0 && m_pItemElem[1]->GetExtra() > 0 && m_pItemElem[0]->m_nResistAbilityOption < 10 )
				{
					SetSwitch();
				}
			}
			
			else if( m_pItemElem[0]->GetExtra() <= 0 || m_pItemElem[1]->GetExtra() <= 0 )
			{
				Destroy();
			}
		}
	}
	else
	{
		m_dwStartTime = 0xffffffff;
		m_dwEndTime = 0xffffffff;
	}

	return TRUE;
}

void CWndBarunaElement::OnDraw( C2DRender* p2DRender )
{
////////////////////////////////
	if( m_bStart )
	{
		static CRect rectStaticTemp;
		LPWNDCTRL lpStatic = GetWndCtrl( WIDC_PC_DELAY );
		rectStaticTemp.TopLeft().y = lpStatic->rect.top+2;
		rectStaticTemp.TopLeft().x = lpStatic->rect.left+2;
		rectStaticTemp.BottomRight().y = lpStatic->rect.bottom;
		int nGaugeWidth(lpStatic->rect.left + static_cast<int>(static_cast<float>(lpStatic->rect.right - lpStatic->rect.left) * m_fGaugeRate));
		nGaugeWidth = nGaugeWidth < lpStatic->rect.right ? nGaugeWidth : lpStatic->rect.right;
		rectStaticTemp.BottomRight().x = nGaugeWidth;
		assert(m_pVBSmeltGauge != NULL);
		m_pTheme->RenderGauge(p2DRender, &rectStaticTemp, 0xffffffff, m_pVBSmeltGauge, &m_texGauEmptyNormal);
	}
/////////////////////////////////////

	for( int i=0;i<4;i++ )
	{
		if( m_pItemElem[i] != NULL )
		{
			m_pItemElem[i]->GetTexture()->Render( p2DRender, m_nCtrlId[i].TopLeft(), 255 );

			if( m_pItemElem[i]->m_nItemNum > 1 )
			{
				TCHAR szTemp[32];
				_stprintf( szTemp, "%d", m_pItemElem[i]->GetExtra() );
				CSize size	= p2DRender->m_pFont->GetTextExtent( szTemp );
				p2DRender->TextOut( m_nCtrlId[i].right-11,  m_nCtrlId[i].bottom-11 , szTemp, 0xff1010ff );
			}

			CRect hitrect = m_nCtrlId[i];
			CPoint point = GetMousePoint();
			if( m_nCtrlId[i].PtInRect( point ) )
			{
				CPoint point2 = point;
				ClientToScreen( &point2 );
				ClientToScreen( &hitrect );
				
				g_WndMng.PutToolTip_Item( (CItemBase*)m_pItemElem[i], point2, &hitrect );
			}
		}
	}
}


void CWndBarunaElement::OnInitialUpdate()
{ 
	CWndNeuz::OnInitialUpdate();
	
	RestoreDeviceObjects();

	m_texGauEmptyNormal.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "ColoGauge01.bmp" ), 0xffff00ff, TRUE );

	CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BT_OK);
	pButton->EnableWindow(FALSE);

	CWndCustom* pWndCustom = (CWndCustom*)GetDlgItem(WIDC_PC_DELAY);
	pWndCustom->SetTitle( "" );

	m_nCtrlId[0] = GetWndCtrl( WIDC_PC_BARUNA )->rect;
	m_nCtrlId[1] = GetWndCtrl( WIDC_PC_ORB )->rect;
	m_nCtrlId[2] = GetWndCtrl( WIDC_PC_GRACE )->rect;
	m_nCtrlId[3] = GetWndCtrl( WIDC_PC_REJU )->rect;

	MoveParentCenter();
} 

void CWndBarunaElement::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	for(int i=0; i<4; i++)
	{
		if(m_nCtrlId[i].PtInRect( point ))
		{
			if(m_pItemElem[i])
			{
				m_pItemElem[i]->SetExtra(0);
				m_pItemElem[i] = NULL;
				CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BT_OK);
				pButton->EnableWindow(FALSE);
			}
		}
	}
}

void CWndBarunaElement::OnDestroy( void )
{
	for(int i=0; i<4; i++)
	{
		if(m_pItemElem[i] != NULL)
			m_pItemElem[i]->SetExtra(0);
	}
}

/***************************************
*
*	APP_BARUNA_WAKEUP
*
****************************************/

CWndBarunaWakeUp::CWndBarunaWakeUp()
{
	m_nDelay = 2; //Sekunden die der Balken braucht
	m_fGaugeRate = 0;

	m_pItemElem = NULL;

	m_nSmeltWidth = -1;
	m_pVBSmeltGauge = NULL;
	m_bVBSmeltGauge = FALSE;
	m_bStart = FALSE;
	m_dwStartTime = m_dwEndTime = 0xffffffff;

}

CWndBarunaWakeUp::~CWndBarunaWakeUp()
{
	DeleteDeviceObjects();
}

HRESULT CWndBarunaWakeUp::RestoreDeviceObjects()
{
	CWndBase::RestoreDeviceObjects();

	m_pApp->m_pd3dDevice->CreateVertexBuffer( sizeof( TEXTUREVERTEX2 ) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pVBSmeltGauge, NULL );
	m_nSmeltWidth = -1;
	m_texGauEmptyNormal.SetInvalidate(m_pApp->m_pd3dDevice); 

	return S_OK;
}

HRESULT CWndBarunaWakeUp::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
	SAFE_RELEASE( m_pVBSmeltGauge );
	m_texGauEmptyNormal.Invalidate(); 
	return S_OK;
}

HRESULT CWndBarunaWakeUp::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	return InvalidateDeviceObjects();
}

BOOL CWndBarunaWakeUp::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	CRect rect;
	CItemElem* pItemElem;
	pItemElem = (CItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );

	if(g_pPlayer->IsUsing(pItemElem))
	{
		g_WndMng.PutString( "Item wird bereits verwendet" );
	}
	else
	{
		if( pItemElem->GetProp()->IsBaruna() && pItemElem->GetRandomOpt() == NULL )	
		{
			if( /*m_nCtrlId[0].PtInRect( point ) &&*/ m_pItemElem == NULL )
			{
				m_pItemElem = pItemElem;
				m_pItemElem->SetExtra(m_pItemElem->GetExtra()+1);
			}
		}
	}
	if( m_pItemElem != NULL )
	{
		CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BT_OK);
		pButton->EnableWindow(TRUE);
	}
	return TRUE;
}

BOOL CWndBarunaWakeUp::Initialize( CWndBase* pWndParent, DWORD nType )
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_BARUNA_WAKEUP, 0, CPoint( 0, 0 ), pWndParent );
}

BOOL CWndBarunaWakeUp::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	if( nID == WIDC_BT_OK )
	{
		if( m_bStart )
			return TRUE;

		m_bStart = TRUE;
		m_dwStartTime = g_tmCurrent;
		m_dwEndTime = g_tmCurrent + SEC(m_nDelay);
	}
	else if( nID == WIDC_BT_CANCEL )
	{
		if( m_bStart )
		{
			m_bStart = FALSE;
		}
		else
			Destroy();
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

BOOL CWndBarunaWakeUp::Process()
{
	if(m_bStart && m_pItemElem != NULL)
	{
		m_fGaugeRate = static_cast<float>(g_tmCurrent - m_dwStartTime) / static_cast<float>(m_dwEndTime - m_dwStartTime);
		
		if(m_dwEndTime < g_tmCurrent)
		{
			m_bStart = FALSE;
			m_dwStartTime = 0xffffffff;
			m_dwEndTime = 0xffffffff;

			// Send to Server...
			g_DPlay.SendBarunaAwake( m_pItemElem );
			Destroy();
		}
	}
	else
	{
		m_dwStartTime = 0xffffffff;
		m_dwEndTime = 0xffffffff;
	}

	return TRUE;
}

void CWndBarunaWakeUp::OnDraw( C2DRender* p2DRender )
{
	CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem(WIDC_ST_PENYA01);
	pWndStatic->SetTitle("");
	pWndStatic = (CWndStatic*)GetDlgItem(WIDC_ST_PENYA);
	pWndStatic->SetTitle("");
////////////////////////////////
	if( m_bStart )
	{
		static CRect rectStaticTemp;
		LPWNDCTRL lpStatic = GetWndCtrl( WIDC_PC_DELAY );
		rectStaticTemp.TopLeft().y = lpStatic->rect.top+2;
		rectStaticTemp.TopLeft().x = lpStatic->rect.left+2;
		rectStaticTemp.BottomRight().y = lpStatic->rect.bottom;
		int nGaugeWidth(lpStatic->rect.left + static_cast<int>(static_cast<float>(lpStatic->rect.right - lpStatic->rect.left) * m_fGaugeRate));
		nGaugeWidth = nGaugeWidth < lpStatic->rect.right ? nGaugeWidth : lpStatic->rect.right;
		rectStaticTemp.BottomRight().x = nGaugeWidth;
		assert(m_pVBSmeltGauge != NULL);
		m_pTheme->RenderGauge(p2DRender, &rectStaticTemp, 0xffffffff, m_pVBSmeltGauge, &m_texGauEmptyNormal);
	}
/////////////////////////////////////

	if( m_pItemElem != NULL )
	{
		m_pItemElem->GetTexture()->Render( p2DRender, m_nCtrlId.TopLeft(), 255 );

		if( m_pItemElem->m_nItemNum > 1 )
		{
			TCHAR szTemp[32];
			_stprintf( szTemp, "%d", m_pItemElem->GetExtra() );
			CSize size	= p2DRender->m_pFont->GetTextExtent( szTemp );
			p2DRender->TextOut( m_nCtrlId.right-11,  m_nCtrlId.bottom-11 , szTemp, 0xff1010ff );
		}

		CRect hitrect = m_nCtrlId;
		CPoint point = GetMousePoint();
		if( m_nCtrlId.PtInRect( point ) )
		{
			CPoint point2 = point;
			ClientToScreen( &point2 );
			ClientToScreen( &hitrect );
			
			g_WndMng.PutToolTip_Item( (CItemBase*)m_pItemElem, point2, &hitrect );
		}
	}

}


void CWndBarunaWakeUp::OnInitialUpdate()
{ 
	CWndNeuz::OnInitialUpdate();
	
	RestoreDeviceObjects();

	m_texGauEmptyNormal.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "ColoGauge01.bmp" ), 0xffff00ff, TRUE );

	CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BT_OK);
	pButton->EnableWindow(FALSE);

	CWndCustom* pWndCustom = (CWndCustom*)GetDlgItem(WIDC_PC_DELAY);
	pWndCustom->SetTitle( "" );

	m_nCtrlId = GetWndCtrl( WIDC_PC_BARUNA02 )->rect;

	MoveParentCenter();
} 

void CWndBarunaWakeUp::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	if(m_nCtrlId.PtInRect( point ))
	{
		if(m_pItemElem)
		{
			m_pItemElem->SetExtra(0);
			m_pItemElem = NULL;
			CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BT_OK);
			pButton->EnableWindow(FALSE);
		}
	}
}

void CWndBarunaWakeUp::OnDestroy( void )
{
	if(m_pItemElem != NULL)
		m_pItemElem->SetExtra(0);
}

/***************************************
*
*	APP_BARUNA_WAKEUPCANCEL
*
****************************************/

CWndBarunaWakeUpCancel::CWndBarunaWakeUpCancel()
{
	m_nDelay = 2; //Sekunden die der Balken braucht
	m_fGaugeRate = 0;

	for( int i=0;i<2;i++ )
		m_pItemElem[i] = NULL;

	m_nSmeltWidth = -1;
	m_pVBSmeltGauge = NULL;
	m_bVBSmeltGauge = FALSE;
	m_bStart = FALSE;
	m_dwStartTime = m_dwEndTime = 0xffffffff;

}

CWndBarunaWakeUpCancel::~CWndBarunaWakeUpCancel()
{
	DeleteDeviceObjects();
}

HRESULT CWndBarunaWakeUpCancel::RestoreDeviceObjects()
{
	CWndBase::RestoreDeviceObjects();

	m_pApp->m_pd3dDevice->CreateVertexBuffer( sizeof( TEXTUREVERTEX2 ) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pVBSmeltGauge, NULL );
	m_nSmeltWidth = -1;
	m_texGauEmptyNormal.SetInvalidate(m_pApp->m_pd3dDevice); 

	return S_OK;
}

HRESULT CWndBarunaWakeUpCancel::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
	SAFE_RELEASE( m_pVBSmeltGauge );
	m_texGauEmptyNormal.Invalidate(); 
	return S_OK;
}

HRESULT CWndBarunaWakeUpCancel::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	return InvalidateDeviceObjects();
}

BOOL CWndBarunaWakeUpCancel::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	CRect rect;
	CItemElem* pItemElem;
	pItemElem = (CItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );

	if(g_pPlayer->IsUsing(pItemElem))
	{
		g_WndMng.PutString( "Item wird bereits verwendet" );
	}
	else
	{
		if( pItemElem->GetProp()->IsBaruna() && pItemElem->GetRandomOptItemId() )	
		{
			if( /*m_nCtrlId[0].PtInRect( point ) &&*/ m_pItemElem[0] == NULL )
			{
				m_pItemElem[0] = pItemElem;
				m_pItemElem[0]->SetExtra(m_pItemElem[0]->GetExtra()+1);
			}
		}
		else if( pItemElem->GetProp()->dwItemKind3 == IK3_BARUNA_RANDOMOPTION_INIT )
		{
			if( /*m_nCtrlId[0].PtInRect( point ) &&*/ m_pItemElem[1] == NULL )
			{
				m_pItemElem[1] = pItemElem;
				m_pItemElem[1]->SetExtra(m_pItemElem[1]->GetExtra()+1);
			}
		}
	}
	if( m_pItemElem[0] != NULL && m_pItemElem[1] != NULL )
	{
		CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BT_OK);
		pButton->EnableWindow(TRUE);
	}
	return TRUE;
}

BOOL CWndBarunaWakeUpCancel::Initialize( CWndBase* pWndParent, DWORD nType )
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_BARUNA_WAKEUP_CANCEL, 0, CPoint( 0, 0 ), pWndParent );
}

BOOL CWndBarunaWakeUpCancel::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	if( nID == WIDC_BT_OK )
	{
		if( m_bStart )
			return TRUE;

		m_bStart = TRUE;
		m_dwStartTime = g_tmCurrent;
		m_dwEndTime = g_tmCurrent + SEC(m_nDelay);
	}
	else if( nID == WIDC_BT_CANCEL )
	{
		if( m_bStart )
		{
			m_bStart = FALSE;
		}
		else
			Destroy();
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

BOOL CWndBarunaWakeUpCancel::Process()
{
	if(m_bStart && m_pItemElem[0] != NULL)
	{
		m_fGaugeRate = static_cast<float>(g_tmCurrent - m_dwStartTime) / static_cast<float>(m_dwEndTime - m_dwStartTime);
		
		if(m_dwEndTime < g_tmCurrent)
		{
			m_bStart = FALSE;
			m_dwStartTime = 0xffffffff;
			m_dwEndTime = 0xffffffff;

			// Send to Server...
			g_DPlay.SendBarunaAwakeCancel( m_pItemElem );
			Destroy();
		}
	}
	else
	{
		m_dwStartTime = 0xffffffff;
		m_dwEndTime = 0xffffffff;
	}

	return TRUE;
}

void CWndBarunaWakeUpCancel::OnDraw( C2DRender* p2DRender )
{
////////////////////////////////
	if( m_bStart )
	{
		static CRect rectStaticTemp;
		LPWNDCTRL lpStatic = GetWndCtrl( WIDC_PC_DELAY );
		rectStaticTemp.TopLeft().y = lpStatic->rect.top+2;
		rectStaticTemp.TopLeft().x = lpStatic->rect.left+2;
		rectStaticTemp.BottomRight().y = lpStatic->rect.bottom;
		int nGaugeWidth(lpStatic->rect.left + static_cast<int>(static_cast<float>(lpStatic->rect.right - lpStatic->rect.left) * m_fGaugeRate));
		nGaugeWidth = nGaugeWidth < lpStatic->rect.right ? nGaugeWidth : lpStatic->rect.right;
		rectStaticTemp.BottomRight().x = nGaugeWidth;
		assert(m_pVBSmeltGauge != NULL);
		m_pTheme->RenderGauge(p2DRender, &rectStaticTemp, 0xffffffff, m_pVBSmeltGauge, &m_texGauEmptyNormal);
	}
/////////////////////////////////////
	for( int i=0;i<2;i++ )
	{
		if( m_pItemElem[i] != NULL )
		{
			m_pItemElem[i]->GetTexture()->Render( p2DRender, m_nCtrlId[i].TopLeft(), 255 );

			if( m_pItemElem[i]->m_nItemNum > 1 )
			{
				TCHAR szTemp[32];
				_stprintf( szTemp, "%d", m_pItemElem[i]->GetExtra() );
				CSize size	= p2DRender->m_pFont->GetTextExtent( szTemp );
				p2DRender->TextOut( m_nCtrlId[i].right-11,  m_nCtrlId[i].bottom-11 , szTemp, 0xff1010ff );
			}

			CRect hitrect = m_nCtrlId[i];
			CPoint point = GetMousePoint();
			if( m_nCtrlId[i].PtInRect( point ) )
			{
				CPoint point2 = point;
				ClientToScreen( &point2 );
				ClientToScreen( &hitrect );
				
				g_WndMng.PutToolTip_Item( (CItemBase*)m_pItemElem[i], point2, &hitrect );
			}
		}
	}
}


void CWndBarunaWakeUpCancel::OnInitialUpdate()
{ 
	CWndNeuz::OnInitialUpdate();
	
	RestoreDeviceObjects();

	m_texGauEmptyNormal.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "ColoGauge01.bmp" ), 0xffff00ff, TRUE );

	CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BT_OK);
	pButton->EnableWindow(FALSE);

	CWndCustom* pWndCustom = (CWndCustom*)GetDlgItem(WIDC_PC_DELAY);
	pWndCustom->SetTitle( "" );

	m_nCtrlId[0] = GetWndCtrl( WIDC_PC_BARUNA )->rect;
	m_nCtrlId[1] = GetWndCtrl( WIDC_PC_OBLIVION )->rect;

	MoveParentCenter();
} 

void CWndBarunaWakeUpCancel::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	for( int i=0;i<2;i++ )
	{
		if(m_nCtrlId[i].PtInRect( point ))
		{
			if(m_pItemElem[i])
			{
				m_pItemElem[i]->SetExtra(0);
				m_pItemElem[i] = NULL;
				CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BT_OK);
				pButton->EnableWindow(FALSE);
			}
		}
	}
}

void CWndBarunaWakeUpCancel::OnDestroy( void )
{
	for(int i=0;i<2;i++)
	{
		if(m_pItemElem[i] != NULL)
			m_pItemElem[i]->SetExtra(0);
	}
}

#endif
