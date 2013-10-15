#include "stdafx.h"
#include "defineitem.h"

#if __VER >= 19
#include "defineItemGrade.h"
#include "defineItemType.h"
#endif//__VER >= 19

#include "defineText.h"
#include "AppDefine.h"
#include "WndUpgradeBase.h"
#include "DPClient.h"
extern	CDPClient	g_DPlay;

extern	CGuildMng	g_GuildMng;



/****************************************************
  WndId : APP_UPGRADE_BASE - 
  CtrlId : WIDC_TABCTRL1 - TabCtrl
****************************************************/
CWndUpgradeBase::CWndUpgradeBase()
{
	SetPutRegInfo( FALSE );

	memset( &m_pItemElem, 0, sizeof(CItemBase*)*MAX_UPGRADE );
	memset( &m_Rect, 0, sizeof(CRect)*MAX_UPGRADE );

	m_nCost = 0;
	m_dwReqItem[0] = 0;
	m_dwReqItem[1] = 0;

	m_nCount[0] = 0;
	m_nCount[1] = 0;

	m_nMaxCount = 0;
}

CWndUpgradeBase::~CWndUpgradeBase()
{
}

void CWndUpgradeBase::OnDraw( C2DRender* p2DRender )
{
	for( int i=0; i<MAX_UPGRADE; i++ )
	{
		if( m_pItemElem[i] && m_pItemElem[i]->GetTexture() )
		{
			m_pItemElem[i]->GetTexture()->Render( p2DRender, m_Rect[i].TopLeft(), 255 );

			if( m_pItemElem[i]->m_nItemNum > 1 )
			{
				TCHAR szTemp[32];
				_stprintf( szTemp, "%d", m_pItemElem[i]->GetExtra() );
				CSize size	= p2DRender->m_pFont->GetTextExtent( szTemp );
				p2DRender->TextOut( m_Rect[i].right-11,  m_Rect[i].bottom-11 , szTemp, 0xff1010ff );
			}
			//*
			CRect hitrect = m_Rect[i];
			CPoint point = GetMousePoint();
			if( m_Rect[i].PtInRect( point ) )
			{
				CPoint point2 = point;
				ClientToScreen( &point2 );
				ClientToScreen( &hitrect );
				
				g_WndMng.PutToolTip_Item( (CItemBase*)m_pItemElem[i], point2, &hitrect );
			}
			/**/
		}
	}
}

void CWndUpgradeBase::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate(); 

	CWndInventory* pWndInventory = (CWndInventory*)g_WndMng.CreateApplet( APP_INVENTORY );
	
	CRect rcInventory	= pWndInventory->GetWindowRect( TRUE );
	CRect rcVendor = GetWindowRect( TRUE );
	CPoint ptInventory	= rcInventory.TopLeft();
	CPoint point;
	if( ptInventory.x > m_pWndRoot->GetWndRect().Width() / 2 )
		point	= ptInventory - CPoint( rcVendor.Width(), 0 );
	else
		point	= ptInventory + CPoint( rcInventory.Width(), 0 );

	Move( point );

	CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );
#if __VER < 12 // __MOD_TUTORIAL		
	if( pWndWorld ) 
	{
		pWndWorld->m_pWndGuideSystem->SendGuideMessage(APP_TEST);
	}
#endif
	LPWNDCTRL pCustom = NULL;
	pCustom = GetWndCtrl( WIDC_CUSTOM3 );
	m_Rect[0] = pCustom->rect;
	pCustom = GetWndCtrl( WIDC_CUSTOM4 );
	m_Rect[1] = pCustom->rect;
	pCustom = GetWndCtrl( WIDC_CUSTOM1 );
	m_Rect[2] = pCustom->rect;
	pCustom = GetWndCtrl( WIDC_CUSTOM2 );
	m_Rect[3] = pCustom->rect;
	pCustom = GetWndCtrl( WIDC_CUSTOM5 );
	m_Rect[4] = pCustom->rect;
	pCustom = GetWndCtrl( WIDC_CUSTOM6 );
	m_Rect[5] = pCustom->rect;

	m_nCount[0] = 0;
	m_nCount[1] = 0;

	m_nMaxCount = 0;
} 

BOOL CWndUpgradeBase::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_TEST, 0, 0, pWndParent );
}

BOOL CWndUpgradeBase::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}

void CWndUpgradeBase::OnSize( UINT nType, int cx, int cy )
{
	CWndNeuz::OnSize( nType, cx, cy );
}

void CWndUpgradeBase::OnLButtonUp( UINT nFlags, CPoint point ) 
{

}

void CWndUpgradeBase::OnLButtonDown( UINT nFlags, CPoint point ) 
{

}
BOOL CWndUpgradeBase::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	if( g_pPlayer == NULL )
		return FALSE;

	int nNeedRes[3][10] = {
		{ 10, 20, 40,  70, 110, 160, 220, 290, 370, 460 },
		{ 20, 40, 80, 140, 220, 320, 440, 580, 740, 920 },
		{ 2000, 4000, 6000, 8000, 10000, 12000, 14000, 16000, 18000, 20000 }
	};

	float fSucessPersent[5][10] = {
		{ 80.0f, 50.0f, 20.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f, 0.0f },
		{  0.0f, 70.0f, 50.0f, 20.0f, 10.0f,  0.0f,  0.0f,  0.0f,  0.0f, 0.0f },
		{  0.0f,  0.0f,  0.0f, 60.0f, 40.0f, 20.0f, 10.0f,  0.0f,  0.0f, 0.0f },
		{  0.0f,  0.0f,  0.0f,  0.0f,  0.0f, 60.0f, 40.0f, 20.0f, 10.0f, 0.0f },
		{  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f, 50.0f, 25.0f, 5.0f }
	};

	CWndBase* pWndFrame =  pShortcut->m_pFromWnd->GetFrameWnd();

	// 아이템이 인벤토리에서 왔는가?
	if( pShortcut->m_dwShortcut == SHORTCUT_ITEM && pWndFrame->GetWndId() == APP_INVENTORY )
	{
		if( g_pPlayer->m_Inventory.IsEquip( pShortcut->m_dwId ) )
		{
			g_WndMng.PutString( prj.GetText(TID_GAME_EQUIPPUT), NULL, prj.GetTextColor(TID_GAME_EQUIPPUT) );
			SetForbid( TRUE );
			return FALSE;
		}

		static int*  pAbilityOption = NULL;

		CItemElem* pItemElem;
		pItemElem  = (CItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );

		if(pItemElem == NULL)
			return FALSE;
		
		// 아이템( 방어구, 무기구 )
		if( PtInRect(&m_Rect[0], point) )
		{
			if( m_pItemElem[0] )
				return FALSE;

//			if( pItemElem->GetProp()->nLog >=2 )
//			{
//				g_WndMng.OpenMessageBox( prj.GetText(TID_UPGRADE_ERROR_NOUNICK), MB_OK, this );
//				return FALSE;
//			}
			
			if( pItemElem->m_nResistSMItemId != 0 ) // 상용화 아이템 적용중이면 불가능
			{
				g_WndMng.OpenMessageBox( prj.GetText(TID_GAME_NOTUPGRADE), MB_OK, this );
				return FALSE;
			}
		
			// 방어구나 무기류가 아니면 제련불가능
			if( pItemElem->GetProp()->dwItemKind2 == IK2_ARMOR ||
				pItemElem->GetProp()->dwItemKind2 == IK2_WEAPON_MAGIC ||
				pItemElem->GetProp()->dwItemKind2 == IK2_WEAPON_DIRECT ||
				pItemElem->GetProp()->dwItemKind2 == IK2_ARMORETC )
			{
				pItemElem->SetExtra( 1 );
				m_pItemElem[0]	= pItemElem;
			}
		}
		else
		// 아이템( 카드, 주사위 )
		if( PtInRect(&m_Rect[1], point) )
		{
			if( m_pItemElem[1] )
				return FALSE;

			// 재련할 아이템이 없는경우 리턴
			if( m_pItemElem[0] == NULL )
			{
				g_WndMng.OpenMessageBox( prj.GetText(TID_UPGRADE_ERROR_ITEMFIRST), MB_OK, this );
				return FALSE;
			}
			
			// 카드, 주사위 아이템이 아니면 리턴
		#if __VER >= 8 //__Y_NEW_ENCHANT
			if( pItemElem->GetProp()->dwItemKind3 != IK3_ELECARD && pItemElem->GetProp()->dwItemKind3 != IK3_ENCHANT )
		#else //__Y_NEW_ENCHANT			
			if( pItemElem->GetProp()->dwItemKind3 != IK3_ELECARD && pItemElem->GetProp()->dwItemKind3 != IK3_DICE )
		#endif //__Y_NEW_ENCHANT
				return FALSE;

			if( pItemElem->GetProp()->dwItemKind3 == IK3_ELECARD  )
			{
				if( ( m_pItemElem[0]->GetProp()->dwItemKind3 != IK3_SUIT && 
					m_pItemElem[0]->GetProp()->dwItemKind2 != IK2_WEAPON_MAGIC &&
					m_pItemElem[0]->GetProp()->dwItemKind2 != IK2_WEAPON_DIRECT )
					)
				{
					g_WndMng.OpenMessageBox( prj.GetText(TID_UPGRADE_ERROR_NOELEUPGRADE), MB_OK, this );
					return FALSE;
				}
			}
				
	
			// 카드이고, 제련할 아이템의 속성이 카드의 속성과 다를경우 리턴
			if( pItemElem->GetProp()->dwItemKind3 == IK3_ELECARD && m_pItemElem[0]->m_bItemResist != SAI79::NO_PROP && m_pItemElem[0]->m_bItemResist != pItemElem->GetProp()->eItemType )
			{
				g_WndMng.OpenMessageBox( prj.GetText(TID_UPGRADE_ERROR_TWOELEMENT), MB_OK, this );
				return FALSE;
			}

			if( pItemElem->GetProp()->dwItemKind3 == IK3_ELECARD )
			{
				pAbilityOption = &(m_pItemElem[0]->m_nResistAbilityOption);
			}

		#if __VER >= 8 //__Y_NEW_ENCHANT
			if( pItemElem->GetProp()->dwItemKind3 == IK3_ENCHANT )
		#else //__Y_NEW_ENCHANT
			if( pItemElem->GetProp()->dwItemKind3 == IK3_DICE )
		#endif //__Y_NEW_ENCHANT
			{
				//*
				// 속성레벨이 10 이상이면 제련 불가능
				if( m_pItemElem[0]->GetAbilityOption() >= 10 )
				{
					CString str;
					str.Format( prj.GetText(TID_UPGRADE_ERROR_MAXLEVEL), 10 );
					g_WndMng.OpenMessageBox( str, MB_OK, this );
					return FALSE;
				}
				/**/
				

				pAbilityOption = m_pItemElem[0]->GetAbilityOptionPtr();
			}
			
			
			if( fSucessPersent[pItemElem->GetProp()->dwItemLV-1][*pAbilityOption] <= 0.0f )
			{
				g_WndMng.OpenMessageBox( prj.GetText(TID_UPGRADE_ERROR_WRONGUPLEVEL), MB_OK, this );
				return FALSE;
			}

			// 제련시 필요 보조석 설정(카드인경우와 주사위인경우)
			m_dwReqItem[0] = 0;
			m_dwReqItem[1] = 0;
			
			if( pItemElem->GetProp()->dwItemKind3 == IK3_ELECARD )
			{
				switch( pItemElem->GetProp()->eItemType )
				{
					case SAI79::FIRE:
						m_dwReqItem[0]  = II_GEN_MAT_SUP_ERONS;
						m_dwReqItem[1]  = II_GEN_MAT_SUP_GURU;
						break;
					case SAI79::WATER:
						m_dwReqItem[0]  = II_GEN_MAT_SUP_KRASEC;
						m_dwReqItem[1]  = II_GEN_MAT_SUP_GURU;
						break;
					case SAI79::EARTH:
						m_dwReqItem[0]  = II_GEN_MAT_SUP_MINERAL;
						m_dwReqItem[1]  = II_GEN_MAT_SUP_GURU;
						break;
					case SAI79::ELECTRICITY:
						m_dwReqItem[0]  = II_GEN_MAT_SUP_MINERAL;
						m_dwReqItem[1]  = II_GEN_MAT_SUP_KRASEC;
						break;
					case SAI79::WIND:
						m_dwReqItem[0]  = II_GEN_MAT_SUP_ERONS;
						m_dwReqItem[1]  = II_GEN_MAT_SUP_KRASEC;
						break;
				}
			}
			else
			if( pItemElem->GetProp()->dwItemKind3 == IK3_DICE )
			{
				m_dwReqItem[0]  = II_GEN_MAT_SUP_MINERAL;
				m_dwReqItem[1]  = II_GEN_MAT_SUP_ERONS;
			}
			
			pItemElem->SetExtra( 1 );
			m_pItemElem[1]	= pItemElem;
			
			//필요한 보조석 갯수 표시
			CString str;
			CWndStatic* pWndStatic;
			pWndStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC6);
			str.Format( prj.GetText(TID_UPGRADE_SUPPORTM), nNeedRes[0][*pAbilityOption] );
			pWndStatic->SetTitle(str);
			
			pWndStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC4);
			str.Format( prj.GetText(TID_UPGRADE_SUPPORTM), nNeedRes[0][*pAbilityOption] );
			pWndStatic->SetTitle(str);

			//필요한 보조석 이름 표시
			pWndStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC10);
			str.Format( "%s", prj.GetItemProp(m_dwReqItem[0])->szName );
			pWndStatic->SetTitle(str);

			pWndStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC11);
			str.Format( "%s", prj.GetItemProp(m_dwReqItem[1])->szName );
			pWndStatic->SetTitle(str);
			


			// 필요 금액 설정
			pWndStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC5);
			str.Format( prj.GetText(TID_UPGRADE_COST), nNeedRes[2][*pAbilityOption] );
			pWndStatic->SetTitle(str);

			m_nCount[0] = nNeedRes[0][*pAbilityOption];
			m_nCount[1] = nNeedRes[0][*pAbilityOption];

			m_nMaxCount = nNeedRes[0][*pAbilityOption];
			
			// 성공확률 표시
			/*
			float fResult = fSucessPersent[m_pItemElem[1]->GetProp()->dwItemLV-1][m_pItemElem[0]->m_nAbilityOption];
			pWndStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC7);
			str.Format( prj.GetText(TID_UPGRADE_SUCCESSRATE), fResult );
			pWndStatic->SetTitle(str);
			*/
		}
		else
		// 보조석
		if( PtInRect(&m_Rect[2], point) )
		{
			if( m_pItemElem[1] == NULL )
				return FALSE;

			if( m_pItemElem[2] || !pAbilityOption )
				return FALSE;

			if( pItemElem->GetProp()->dwItemKind3 == IK3_SUPSTONE )
			{
				if( m_dwReqItem[0] != pItemElem->m_dwItemId )
				{
					g_WndMng.OpenMessageBox( prj.GetText(TID_UPGRADE_ERROR_WRONGSUPITEM), MB_OK, this );
					//g_WndMng.OpenMessageBox( "필요한 보조석이 아닙니다.", MB_OK, this );
					return FALSE;
				}

				if( m_pItemElem[4] )
				{
					if(pItemElem->m_nItemNum < m_nCount[0])
					{
						g_WndMng.OpenMessageBox( prj.GetText(TID_UPGRADE_ERROR_NOSUPSTON), MB_OK, this );
						return FALSE;
					}
				}
				
				if( pItemElem->m_nItemNum < m_nCount[0] )
				{
					pItemElem->SetExtra( pItemElem->m_nItemNum );	
					m_nCount[0] -= pItemElem->m_nItemNum;
				}
				else
				{
					pItemElem->SetExtra( m_nCount[0] );	
					m_nCount[0] = 0;
				}

				m_pItemElem[2]	= pItemElem;
			}
		}
		else
		if( PtInRect(&m_Rect[4], point) )
		{
			if( m_pItemElem[1] == NULL )
				return FALSE;

			if( m_pItemElem[4] || !pAbilityOption )
				return FALSE;
			
			if( pItemElem->GetProp()->dwItemKind3 == IK3_SUPSTONE )
			{
				if( m_dwReqItem[0] != pItemElem->m_dwItemId )
				{
					g_WndMng.OpenMessageBox( prj.GetText(TID_UPGRADE_ERROR_WRONGSUPITEM), MB_OK, this );
					//g_WndMng.OpenMessageBox( "필요한 보조석이 아닙니다.", MB_OK, this );
					return FALSE;
				}

				if( m_pItemElem[2] )
				{
					if(pItemElem->m_nItemNum < m_nCount[0])
					{
						g_WndMng.OpenMessageBox( prj.GetText(TID_UPGRADE_ERROR_NOSUPSTON), MB_OK, this );
						return FALSE;
					}
				}

				if( pItemElem->m_nItemNum < m_nCount[0] )
				{
					pItemElem->SetExtra( pItemElem->m_nItemNum );	
					m_nCount[0] -= pItemElem->m_nItemNum;
				}
				else
				{
					pItemElem->SetExtra( m_nCount[0] );	
					m_nCount[0] = 0;
				}
				
				m_pItemElem[4]	= pItemElem;
			}
		}
		else
		if( PtInRect(&m_Rect[3], point) )
		{
			if( m_pItemElem[1] == NULL )
				return FALSE;

			if( m_pItemElem[3] || !pAbilityOption )
				return FALSE;

			if( pItemElem->GetProp()->dwItemKind3 == IK3_SUPSTONE )
			{
				if( m_dwReqItem[1] != pItemElem->m_dwItemId )
				{
					g_WndMng.OpenMessageBox( prj.GetText(TID_UPGRADE_ERROR_WRONGSUPITEM), MB_OK, this );
					return FALSE;
				}

				if( m_pItemElem[5] )
				{
					if(pItemElem->m_nItemNum < m_nCount[1])
					{
						g_WndMng.OpenMessageBox( prj.GetText(TID_UPGRADE_ERROR_NOSUPSTON), MB_OK, this );
						return FALSE;
					}
				}

				if( pItemElem->m_nItemNum < m_nCount[1] )
				{
					pItemElem->SetExtra( pItemElem->m_nItemNum );	
					m_nCount[1] -= pItemElem->m_nItemNum;
				}
				else
				{
					pItemElem->SetExtra( m_nCount[1] );	
					m_nCount[1] = 0;
				}
				
				m_pItemElem[3]	= pItemElem;
			}
		}
		else
		if( PtInRect(&m_Rect[5], point) )
		{
			if( m_pItemElem[1] == NULL )
				return FALSE;

			if( m_pItemElem[5] || !pAbilityOption  )
				return FALSE;
			
			if( pItemElem->GetProp()->dwItemKind3 == IK3_SUPSTONE )
			{
				if( m_dwReqItem[1] != pItemElem->m_dwItemId )
				{
					g_WndMng.OpenMessageBox( prj.GetText(TID_UPGRADE_ERROR_WRONGSUPITEM), MB_OK, this );
					return FALSE;
				}

				if( m_pItemElem[3] )
				{
					if(pItemElem->m_nItemNum < m_nCount[1])
					{
						g_WndMng.OpenMessageBox( prj.GetText(TID_UPGRADE_ERROR_NOSUPSTON), MB_OK, this );
						return FALSE;
					}
				}

				if( pItemElem->m_nItemNum < m_nCount[1] )
				{
					pItemElem->SetExtra( pItemElem->m_nItemNum );	
					m_nCount[1] -= pItemElem->m_nItemNum;
				}
				else
				{
					pItemElem->SetExtra( m_nCount[1] );	
					m_nCount[1] = 0;
				}
				
				m_pItemElem[5]	= pItemElem;
			}
		}
	}			
	else
		SetForbid( FALSE );

	return FALSE;
}

BOOL CWndUpgradeBase::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch( nID )
	{
		case WIDC_OK:
			{
				DWORD dwCount[2];
				DWORD dwobjId[6];
				DWORD ItemCount[6];

				dwCount[0] = 0;
				dwCount[1] = 0;

				if( m_pItemElem[0] == NULL || m_pItemElem[1] == NULL )
					break;

				dwobjId[0] = m_pItemElem[0]->m_dwObjId;
				dwobjId[1] = m_pItemElem[1]->m_dwObjId;
				
				if( m_pItemElem[2] == NULL ) 
				{
					ItemCount[2] = 0;
					dwobjId  [2] = NULL_ID;
				}
				else
				{
					ItemCount[2] = m_pItemElem[2]->GetExtra();	
					dwobjId  [2] = m_pItemElem[2]->m_dwObjId;
					dwCount  [0]+= m_pItemElem[2]->GetExtra();
				}
				if( m_pItemElem[4] == NULL ) 
				{
					ItemCount[4] = 0;
					dwobjId  [4] = NULL_ID;
				}
				else
				{
					ItemCount[4] = m_pItemElem[4]->GetExtra();	
					dwobjId  [4] = m_pItemElem[4]->m_dwObjId;
					dwCount  [0]+= m_pItemElem[4]->GetExtra();
				}
				if( m_pItemElem[3] == NULL ) 
				{
					ItemCount[3] = 0;
					dwobjId  [3] = NULL_ID;
				}
				else
				{
					ItemCount[3] = m_pItemElem[3]->GetExtra();	
					dwobjId  [3] = m_pItemElem[3]->m_dwObjId;
					dwCount  [1]+= m_pItemElem[3]->GetExtra();	
				}
				if( m_pItemElem[5] == NULL ) 
				{
					ItemCount[5] = 0;
					dwobjId  [5] = NULL_ID;
				}
				else
				{
					ItemCount[5] = m_pItemElem[5]->GetExtra();	
					dwobjId  [5] = m_pItemElem[5]->m_dwObjId;
					dwCount  [1]+= m_pItemElem[5]->GetExtra();
				}

				if( (int)( dwCount[0] ) < m_nMaxCount || (int)( dwCount[1] ) < m_nMaxCount )
				{
					g_WndMng.OpenMessageBox( prj.GetText(TID_UPGRADE_ERROR_NOSUPSTON), MB_OK, this );
					return FALSE;
				}

				g_DPlay.SendUpgradeBase( dwobjId[0], 
									 	dwobjId[1], 
										dwobjId[2], ItemCount[2],
										dwobjId[3], ItemCount[3],
										dwobjId[4], ItemCount[4],
										dwobjId[5], ItemCount[5] );
				Destroy();
			}
			break;
		case WIDC_CANCEL:
			{
				Destroy();
			}
			break;
	};

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

void CWndUpgradeBase::OnDestroyChildWnd( CWndBase* pWndChild )
{
}

void CWndUpgradeBase::OnDestroy( void )
{
	for( int i=0; i<MAX_UPGRADE; i++ )
	{
		if( m_pItemElem[i] )
			m_pItemElem[i]->SetExtra( 0 );
	}
}


void CWndUpgradeBase::OnRButtonUp( UINT nFlags, CPoint point )
{
	if( PtInRect(&m_Rect[0], point) )
	{
		for( int i=0; i<MAX_UPGRADE; i++ )
		{
			if( m_pItemElem[i] )
			{
				m_pItemElem[i]->SetExtra( 0 );
				m_pItemElem[i] = NULL;
			}
		}
		m_nCount[0] = m_nCount[1] = 0;
		m_dwReqItem[0] = 0;
		m_dwReqItem[1] = 0;

		CWndStatic* pWndStatic;
		pWndStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC6);
		pWndStatic->SetTitle("");
		pWndStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC4);
		pWndStatic->SetTitle("");
		pWndStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC10);
		pWndStatic->SetTitle("");
		pWndStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC11);
		pWndStatic->SetTitle("");
		pWndStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC5);
		pWndStatic->SetTitle("");
	}
	else
	if( PtInRect(&m_Rect[1], point) )
	{
		for( int i=1; i<MAX_UPGRADE; i++ )
		{
			if( m_pItemElem[i] )
			{
				m_pItemElem[i]->SetExtra( 0 );
				m_pItemElem[i] = NULL;
			}
		}
		m_nCount[0] = m_nCount[1] = 0;
		m_dwReqItem[0] = 0;
		m_dwReqItem[1] = 0;
		
		CWndStatic* pWndStatic;
		pWndStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC6);
		pWndStatic->SetTitle("");
		pWndStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC4);
		pWndStatic->SetTitle("");
		pWndStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC10);
		pWndStatic->SetTitle("");
		pWndStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC11);
		pWndStatic->SetTitle("");
		pWndStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC5);
		pWndStatic->SetTitle("");
	}
	else
	if( PtInRect(&m_Rect[2], point) && m_pItemElem[2] )
	{
		m_nCount[0] += m_pItemElem[2]->GetExtra();
		m_pItemElem[2]->SetExtra( 0 );
		m_pItemElem[2] = NULL;
	}
	else
	if( PtInRect(&m_Rect[3], point) && m_pItemElem[3]  )
	{
		m_nCount[1] += m_pItemElem[3]->GetExtra();
		m_pItemElem[3]->SetExtra( 0 );
		m_pItemElem[3] = NULL;
	}
	else
	if( PtInRect(&m_Rect[4], point) && m_pItemElem[4]  )
	{
		m_nCount[0] += m_pItemElem[4]->GetExtra();
		m_pItemElem[4]->SetExtra( 0 );
		m_pItemElem[4] = NULL;
	}
	else
	if( PtInRect(&m_Rect[5], point) && m_pItemElem[5]  )
	{
		m_nCount[1] += m_pItemElem[5]->GetExtra();
		m_pItemElem[5]->SetExtra( 0 );
		m_pItemElem[5] = NULL;
	}
		
		
}



