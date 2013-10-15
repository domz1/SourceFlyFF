#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndManager.h"
#include "WndSummonAngel.h"

#include "dpclient.h"
extern CDPClient	g_DPlay;


#if __VER >= 8 //__CSC_VER8_5

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndSummonAngel
////////////////////////////////////////////////////////////////////////////////////////////////////////////
CWndSummonAngel::CWndSummonAngel() 
{
	m_nSelecCtrl = -1;
	m_nOrichalcum = 0;
	m_nMoonstone = 0;
	m_WhiteAngelRate = 0.0f;
	m_RedAngelRate = 0.0f;
	m_BlueAngelRate = 0.0f;
	m_GreenAngelRate = 0.0f;
	m_pText = NULL;
	m_pStatic[0] = NULL;
	m_pStatic[1] = NULL;
	m_pStatic[2] = NULL;
	m_nitemcount = 0;
	m_nowStarting = FALSE;
//	m_isCreateSuccess = FALSE;
}

CWndSummonAngel::~CWndSummonAngel() 
{ 
	DeleteDeviceObjects();
} 

void CWndSummonAngel::OnDestroy()
{
	for(int i=0; i<MAX_MATDIE; i++)
	{
		if(m_MatDie[i].pItemElem != NULL)
		{
			if( !g_pPlayer->m_vtInfo.IsTrading( m_MatDie[i].pItemElem ) )
				m_MatDie[i].pItemElem->SetExtra(0);
		}
	}
	if(m_pWndInventory != NULL)
		m_pWndInventory->m_wndItemCtrl.SetDieFlag(FALSE);
}

void CWndSummonAngel::SerializeRegInfo( CAr& ar, DWORD& dwVersion )
{
	CWndNeuz::SerializeRegInfo( ar, dwVersion );
}

void CWndSummonAngel::OnDraw( C2DRender* p2DRender ) 
{ 
	//Render Icon
	ItemProp* pItemProp;
	CTexture* pTexture;
	
	pItemProp = prj.GetItemProp( II_GEN_MAT_ORICHALCUM01 );
	if(pItemProp != NULL)
	{
		pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );
		if(pTexture != NULL)
			pTexture->Render( p2DRender, CPoint( 30, 218 ) );
	}

	pItemProp = prj.GetItemProp( II_GEN_MAT_MOONSTONE );
	if(pItemProp != NULL)
	{
		pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );
		if(pTexture != NULL)
			pTexture->Render( p2DRender, CPoint( 30, 306 ) );
	}
	
	CPoint point = GetMousePoint();
	int testnum = HitTest( point );
	if( testnum != -1)
	{
		if( CWndBase::m_GlobalShortcut.m_dwData )
		{
			m_nSelecCtrl = testnum;
			CRect rect;
			rect = m_MatDie[m_nSelecCtrl].wndCtrl->rect;
			p2DRender->RenderFillRect( rect, 0x60ffff00 );
		}
	}
	else
		m_nSelecCtrl = -1;
	
	for(int i=0; i<MAX_MATDIE; i++)
	{
		if(m_MatDie[i].isUse)
		{
			if(m_MatDie[i].pItemElem)
			{
				pItemProp = prj.GetItemProp( m_MatDie[i].pItemElem->GetProp()->dwID );
				if(pItemProp != NULL)
				{
					pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );
					if(pTexture != NULL)
						pTexture->Render( p2DRender, CPoint( m_MatDie[i].wndCtrl->rect.left, m_MatDie[i].wndCtrl->rect.top ) );
				}
			}
		}
	}

	// '%'
	int y = 206;
	int nNext = 24;
	DWORD dwColor = 0xff2e70a9;
	CD3DFont* pOldFont = p2DRender->GetFont();
	p2DRender->SetFont( CWndBase::m_Theme.m_pFontText );
	p2DRender->TextOut( 415, y, "%", dwColor); y += nNext;
	p2DRender->TextOut( 415, y, "%", dwColor); y += nNext;
	p2DRender->TextOut( 415, y, "%", dwColor); y += nNext;
	p2DRender->SetFont( pOldFont );
} 

void CWndSummonAngel::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	//Ctrl Initialize.
	m_pText = (CWndText *)GetDlgItem( WIDC_TEXT1 );

	m_pStatic[0] = (CWndStatic *)GetDlgItem( WIDC_STATIC_RED );
	m_pStatic[1] = (CWndStatic *)GetDlgItem( WIDC_STATIC_BLUE );
	m_pStatic[2] = (CWndStatic *)GetDlgItem( WIDC_STATIC_GREEN );
	SummonRateRefresh();
	
	int StaticID[20] = {WIDC_STATIC11, WIDC_STATIC12, WIDC_STATIC13, WIDC_STATIC14, WIDC_STATIC15, WIDC_STATIC16,
						WIDC_STATIC17, WIDC_STATIC18, WIDC_STATIC19, WIDC_STATIC20, WIDC_STATIC21, WIDC_STATIC22,
						WIDC_STATIC23, WIDC_STATIC24, WIDC_STATIC25, WIDC_STATIC26, WIDC_STATIC27, WIDC_STATIC28,
						WIDC_STATIC29, WIDC_STATIC30};

	for(int i=0; i<MAX_MATDIE; i++)
	{
		m_MatDie[i].wndCtrl = GetWndCtrl( StaticID[i] );
		m_MatDie[i].staticNum = StaticID[i];
		m_MatDie[i].isUse = FALSE;
		m_MatDie[i].pItemElem = NULL;
		m_ItemInfo[i].extracount = 0;
		m_ItemInfo[i].itemid = -1;
	}
	
	//Text Setting
	CWndStatic* pStatic = (CWndStatic *)GetDlgItem( WIDC_STATIC3 );
	pStatic->SetTitle(prj.GetText(TID_GAME_REDANGEL_RATE));
	pStatic = (CWndStatic *)GetDlgItem( WIDC_STATIC4 );
	pStatic->SetTitle(prj.GetText(TID_GAME_BLUEANGEL_RATE));
	pStatic = (CWndStatic *)GetDlgItem( WIDC_STATIC5 );
	pStatic->SetTitle(prj.GetText(TID_GAME_GREENANGEL_RATE));

	pStatic = (CWndStatic *)GetDlgItem( WIDC_STATIC6 );
	pStatic->SetTitle(prj.GetText(TID_GAME_WHITEANGEL_INFO1));
	pStatic = (CWndStatic *)GetDlgItem( WIDC_STATIC1 );
	pStatic->SetTitle(prj.GetText(TID_GAME_WHITEANGEL_INFO2));
	pStatic = (CWndStatic *)GetDlgItem( WIDC_STATIC7 );
	pStatic->SetTitle(prj.GetText(TID_GAME_WHITEANGEL_INFO3));

	//France Button Image
	CWndButton* pButton = (CWndButton*)GetDlgItem( WIDC_START );
	if(::GetLanguage() == LANG_FRE)
		pButton->SetTexture(g_Neuz.m_pd3dDevice, MakePath( "Theme\\", ::GetLanguage(), _T( "ButtNormal00.tga" ) ), TRUE);

	//Window Position
	m_pWndInventory = (CWndInventory*)GetWndBase( APP_INVENTORY );
	CRect rectInventory;
	if(m_pWndInventory != NULL)
	{
		m_pWndInventory->m_wndItemCtrl.SetDieFlag(TRUE);
		rectInventory = m_pWndInventory->GetWindowRect( TRUE );
	}
	CPoint ptInventory = rectInventory.TopLeft();
	CPoint ptMove;
	
	CRect rect = GetWindowRect( TRUE );
	
	if( ptInventory.x > rect.Width() / 2 )
		ptMove = ptInventory - CPoint( rect.Width(), 0 );
	else
		ptMove = ptInventory + CPoint( rectInventory.Width(), 0 );
	
	Move( ptMove );
}

void CWndSummonAngel::SetQuestText( CHAR* szChar )
{
	if(m_pText != NULL)
	{
		m_pText->m_string.AddParsingString( szChar );
		m_pText->ResetString();	
	}
}

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndSummonAngel::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_SUMMON_ANGEL, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndSummonAngel::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndSummonAngel::OnSize( UINT nType, int cx, int cy )
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 

void CWndSummonAngel::OnMouseMove( UINT nFlags, CPoint point )
{
}

void CWndSummonAngel::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 

void CWndSummonAngel::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
}

void CWndSummonAngel::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	int choicenum = HitTest( point );
	if(choicenum > -1 && m_MatDie[choicenum].isUse)
	{
		m_MatDie[choicenum].isUse = FALSE;

		if(choicenum >= 0 && choicenum < 10)
			m_nOrichalcum--;
		else if(choicenum > 9 && choicenum < 20)
			m_nMoonstone--;

		m_MatDie[choicenum].pItemElem->SetExtra(m_MatDie[choicenum].pItemElem->GetExtra()-1);
		SummonRateRefresh();
	}
}

void CWndSummonAngel::OnRButtonDblClk( UINT nFlags, CPoint point )
{
}

BOOL CWndSummonAngel::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	int isValid = ITEM_INVALID;
	CItemElem* pItemElem  = (CItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
	if( m_nSelecCtrl > -1 && !m_MatDie[m_nSelecCtrl].isUse)
	{
		if( m_nSelecCtrl < 10 && (pItemElem->GetProp()->dwID == II_GEN_MAT_ORICHALCUM01 || pItemElem->GetProp()->dwID == II_GEN_MAT_ORICHALCUM01_1))
		{
			if( pItemElem->m_nItemNum > pItemElem->GetExtra() )
				isValid = ITEM_VALID;
			else
				isValid = ITEM_MAX_OVERFLOW;
		}
		else if( m_nSelecCtrl > 9 && m_nSelecCtrl < 20 && (pItemElem->GetProp()->dwID == II_GEN_MAT_MOONSTONE || pItemElem->GetProp()->dwID == II_GEN_MAT_MOONSTONE_1))
		{
			if( pItemElem->m_nItemNum > pItemElem->GetExtra() )
				isValid = ITEM_VALID;
			else
				isValid = ITEM_MAX_OVERFLOW;
		}

	}	
	switch (isValid)
	{
		case ITEM_VALID:
			{
				SetDie(pItemElem);
				break;					
			}
		case ITEM_MAX_OVERFLOW:
			{
				// Max overflow Error Msg.
				g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_SUMMONANGEL_ERROR1 ) );
				break;
			}
		case ITEM_INVALID:
			{
				// Invalid Item Error Msg.
				break;
			}
	}
	return TRUE;
}

void CWndSummonAngel::SetDie(CItemElem* pItemElem)
{
	int ptcount, extslotcount, usableitemnum, insertnum, i;
	int slotnum[10];
	ptcount = 0;
	extslotcount = 0;
	usableitemnum = 0;
	insertnum = 0;

	if(pItemElem != NULL)
	{
		if(m_nSelecCtrl > -1)
		{
			if(m_nSelecCtrl >=0 && m_nSelecCtrl < 10)
				ptcount = 0;
			else if(m_nSelecCtrl > 9 && m_nSelecCtrl < 20)
				ptcount = 10;

			if(g_WndMng.m_pWndWorld->m_bShiftPushed)
			{
				int maxcount = ptcount + 10;
				for(i=ptcount; i<maxcount; i++)
				{
					if(!m_MatDie[i].isUse)
					{
						slotnum[extslotcount] = i;
						extslotcount++;
					}
				}
				usableitemnum = pItemElem->m_nItemNum - pItemElem->GetExtra();
				
				if(extslotcount > 0 && usableitemnum > 0)
				{
					if(extslotcount > usableitemnum) //남은 Slot과 사용가능한 Item갯 수를 파악하여 한번에 넣을 갯 수를 지정.
						insertnum = usableitemnum;
					else
						insertnum = extslotcount;

					for(i=0; i<insertnum; i++)
					{
						m_MatDie[slotnum[i]].isUse = TRUE;
						m_MatDie[slotnum[i]].pItemElem = pItemElem;
					}
					pItemElem->SetExtra(pItemElem->GetExtra()+insertnum);
				}
			}
			else
			{
				insertnum++;
				m_MatDie[m_nSelecCtrl].isUse = TRUE;
				m_MatDie[m_nSelecCtrl].pItemElem = pItemElem;
				pItemElem->SetExtra(pItemElem->GetExtra()+insertnum);	
			}

			switch(ptcount)
			{
			case 0:
				m_nOrichalcum += insertnum;
				break;
			case 10:
				m_nMoonstone += insertnum;
				break;
			}
		}
		else //Inventory Dbl Clk...
		{
			int nSelect = -1;
			int count = 0;
			BOOL stopcheck = FALSE;
			if(pItemElem->GetProp()->dwID == II_GEN_MAT_ORICHALCUM01 || pItemElem->GetProp()->dwID == II_GEN_MAT_ORICHALCUM01_1)
				ptcount = 0;
			else if(pItemElem->GetProp()->dwID == II_GEN_MAT_MOONSTONE || pItemElem->GetProp()->dwID == II_GEN_MAT_MOONSTONE_1)
				ptcount = 10;

			count += ptcount;
			while(!stopcheck && count < ptcount + 10)
			{
				if(!m_MatDie[count].isUse)
				{
					stopcheck = TRUE;
					nSelect = count;
				}
				count++;					
			}
			
			if(nSelect > -1)
			{
				m_MatDie[nSelect].isUse = TRUE;
				m_MatDie[nSelect].pItemElem = pItemElem;
				pItemElem->SetExtra(pItemElem->GetExtra()+1);
				if(ptcount == 0)
					m_nOrichalcum++;
				else if(ptcount == 10)
					m_nMoonstone++;
			}
		} //Inventory Dbl Clk...
		
		SummonRateRefresh();		
	}
}

void CWndSummonAngel::ReFreshAll(BOOL extracheck)
{
	for(int i=0; i<MAX_MATDIE; i++)
	{
		m_MatDie[i].isUse = FALSE;
		if(m_MatDie[i].pItemElem != NULL)
		{
			if(extracheck)
			{
				m_MatDie[i].pItemElem->SetExtra(0);
				m_MatDie[i].pItemElem = NULL;
			}
			else
				m_MatDie[i].pItemElem = NULL;
		}
	}

	for(i=0; i<m_nitemcount; i++)
	{
		m_ItemInfo[i].itemid = -1;
		m_ItemInfo[i].extracount = 0;
	}
	m_nOrichalcum = 0;
	m_nMoonstone = 0;
	m_nitemcount = 0;
	m_nowStarting = FALSE;
//	m_isCreateSuccess = FALSE;
	SummonRateRefresh();
}

BOOL CWndSummonAngel::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_RESET )
		ReFreshAll(TRUE);
	else if( nID == WIDC_START )
	{
		if(	m_nOrichalcum + m_nMoonstone < 3 )
		{
			g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_SUMMONANGEL_ERROR2 ) );
		}
		else
		{	
			if(!m_nowStarting)
			{
				m_nowStarting = TRUE;
				m_nitemcount = 0;
				for(int i=0; i<MAX_MATDIE; i++)
				{
					if(m_MatDie[i].isUse)
					{
						if(m_MatDie[i].pItemElem != NULL && m_MatDie[i].pItemElem->GetExtra() > 0)
						{
							BOOL equalflag = FALSE;

							for(int j=0; j<m_nitemcount; j++)
							{
								if(m_ItemInfo[j].itemid == m_MatDie[i].pItemElem->m_dwObjId)
								{
									m_ItemInfo[j].extracount++;
									equalflag = TRUE;
								}
							}

							if(!equalflag)
							{
								m_ItemInfo[m_nitemcount].itemid = m_MatDie[i].pItemElem->m_dwObjId;
								m_ItemInfo[m_nitemcount].extracount++;
								m_nitemcount++;
							}
						}
					}		
				}
				CString sendstr;
				
				for(i=0; i<m_nitemcount; i++)
				{
					CString tempstr;
					tempstr.Format("%dD%dDD", m_ItemInfo[i].itemid, m_ItemInfo[i].extracount);
					sendstr = sendstr + tempstr;
				}
				//Angel Create
				g_DPlay.SendCreateAngel(sendstr);
				ReFreshAll(TRUE);
			}
		}
	}
	else if( nID == WIDC_CANCEL )
		Destroy();
	
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

int CWndSummonAngel::HitTest( CPoint point )
{
	int rtn_val = -1;
	CRect rect;
	for(int i=0; i<MAX_MATDIE; i++)
	{
		rect = m_MatDie[i].wndCtrl->rect;
		if( rect.PtInRect( point ) )
		{
			rtn_val = i;
			i = MAX_MATDIE;
		}
	}
	return rtn_val;
}

void CWndSummonAngel::SummonRateRefresh()
{
	m_GreenAngelRate = (m_nOrichalcum * 1.0f) + (m_nMoonstone * 1.0f);
	m_WhiteAngelRate = m_GreenAngelRate / 10.0f;
	m_BlueAngelRate = m_GreenAngelRate * 2.0f;
	m_RedAngelRate = 100.0f - ( m_GreenAngelRate + m_BlueAngelRate );

	if(m_pStatic[0] && m_pStatic[1] && m_pStatic[2])
	{
		CString tempString;
		tempString.Format("%0.1f", m_RedAngelRate);
		m_pStatic[0]->SetTitle(tempString);
		tempString.Format("%0.1f", m_BlueAngelRate);
		m_pStatic[1]->SetTitle(tempString);
		tempString.Format("%0.1f", m_GreenAngelRate);
		m_pStatic[2]->SetTitle(tempString);
	}
}

/*
void CWndSummonAngel::CreateAngelIs(BOOL isSuccess, char* createAngel)
{
//	m_isCreateSuccess = isSuccess;
	m_CreateAngel.Format("%s", createAngel);
	SummonAngel();
}

void CWndSummonAngel::SummonAngel()
{
	CString strCommand;

	if(m_isCreateSuccess && m_CreateAngel.GetLength() != 0)
	{
		if(m_CreateAngel.Compare("WHITEANGEL") == 0)
		{
			strCommand.Format( "/ci %s", "\"화이트엔젤\"" );
			g_DPlay.SendChat( strCommand );
		}
		else if(m_CreateAngel.Compare("GREENANGEL") == 0)
		{
			strCommand.Format( "/ci %s", "\"그린엔젤\"" );
			g_DPlay.SendChat( strCommand );
		}
		else if(m_CreateAngel.Compare("BLUEANGEL") == 0)
		{
			strCommand.Format( "/ci %s", "\"블루엔젤\"" );
			g_DPlay.SendChat( strCommand );
		}
		else if(m_CreateAngel.Compare("REDANGEL") == 0)
		{
			strCommand.Format( "/ci %s", "\"레드엔젤\"" );
			g_DPlay.SendChat( strCommand );
		}

		ReFreshAll(TRUE);
	}
}
*/
#endif //__CSC_VER8_5
