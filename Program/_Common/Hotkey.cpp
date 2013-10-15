// Neuz.cpp : Defines the entry point for the application.
//
#include "stdafx.h"
#include "Hotkey.h"


CHotkey::CHotkey()
{
  ZeroMemory( g_bKeyTable, 256 );
}
CHotkey::~CHotkey()
{
}
void CHotkey::SetDefaultHotkey( int nMax )
{
	// 기본 단축키 세팅 
	for( int i = 0; i < nMax; i++ )
	{
		LPHOTKEY lpHotkey = &m_HotkeyTable[i];
		lpHotkey->m_bShift   = 0;
		lpHotkey->m_bControl = 0;
		lpHotkey->m_nHotkey  = 0;
		lpHotkey->m_bPressed = 0;
		switch(i)
		{
		case FUNC_USE1       : lpHotkey->m_nHotkey = VK_F1; break;
		case FUNC_USE2       : lpHotkey->m_nHotkey = VK_F2; break;
		case FUNC_USE3       : lpHotkey->m_nHotkey = VK_F3; break;
		case FUNC_USE4       : lpHotkey->m_nHotkey = VK_F4; break;
		case FUNC_USE5       : lpHotkey->m_nHotkey = VK_F5; break;
		case FUNC_USE6       : lpHotkey->m_nHotkey = VK_F6; break;
		case FUNC_USE7       : lpHotkey->m_nHotkey = VK_F7; break;

		case FUNC_GUILD      : lpHotkey->m_nHotkey = 'G'; break;
		case FUNC_COMBAT     : lpHotkey->m_nHotkey = 'C'; break;
		case FUNC_EQUIPMENT  : lpHotkey->m_nHotkey = 'E'; break;
		case FUNC_KNOWLEDGE  : lpHotkey->m_nHotkey = 'K'; break;
		case FUNC_KNOWSUB    : lpHotkey->m_nHotkey = 'U'; break;
		case FUNC_STATUS     : lpHotkey->m_nHotkey = 'S'; break;
		case FUNC_INVENTORY  : lpHotkey->m_nHotkey = 'I'; break;
		case FUNC_ATTACKPC   : lpHotkey->m_nHotkey = 'P'; break;
		case FUNC_CHATLOG    : lpHotkey->m_nHotkey = 'Z'; break;
		case FUNC_VIEWOBJECT : lpHotkey->m_nHotkey = 'V'; break;
		case FUNC_CLEARMSG   : lpHotkey->m_nHotkey = 192; break;
		case FUNC_LOOKHIDDEN : lpHotkey->m_nHotkey = 'L'; break;

		case FUNC_STOP       : lpHotkey->m_nHotkey = VK_SPACE ; break;
		case FUNC_CHAT       : lpHotkey->m_nHotkey = VK_RETURN; break;
		case FUNC_KNOWLEFT   : lpHotkey->m_nHotkey = VK_LEFT  ; break;
		case FUNC_KNOWRIGHT  : lpHotkey->m_nHotkey = VK_RIGHT ; break;
		case FUNC_KNOWTOP    : lpHotkey->m_nHotkey = VK_UP    ; break;
		case FUNC_OLDKNOW    : lpHotkey->m_nHotkey = VK_BACK  ; break;
		case FUNC_EXIT       : lpHotkey->m_nHotkey = VK_ESCAPE; break;
		case FUNC_SCREENCAP  : lpHotkey->m_nHotkey = '0'   ; break;
		case FUNC_SLOT1      : lpHotkey->m_nHotkey = '1'    ; break;
		case FUNC_SLOT2      : lpHotkey->m_nHotkey = '2'    ; break;
		case FUNC_SLOT3      : lpHotkey->m_nHotkey = '3'    ; break;
		case FUNC_HELP       : lpHotkey->m_nHotkey = '4'    ; break;
		case FUNC_VIEWPARTY  : lpHotkey->m_nHotkey = VK_TAB   ; break;
		default              : lpHotkey->m_nHotkey = 0        ; break;

		}
	}
}
BOOL CHotkey::IsAlreadyHotkey(int nChar,BOOL bShift,BOOL bControl)
{
	for(int i = 0; i < HOTKEY_MAX; i++)
	{
		LPHOTKEY lpHotkey = &m_HotkeyTable[i];
		if(lpHotkey->m_nHotkey == nChar && lpHotkey->m_bShift == bShift && lpHotkey->m_bControl == bControl) 
			return TRUE;
	}
	return FALSE;
}
LPHOTKEY CHotkey::GetHotkey(int nFunc)
{
	return &m_HotkeyTable[nFunc];
}
LPHOTKEY CHotkey::GetHotkey(int nChar,BOOL bShift,BOOL bControl)
{
	for(int i = 0; i < HOTKEY_MAX; i++)
	{
		LPHOTKEY lpHotkey = &m_HotkeyTable[i];
		if(lpHotkey->m_nHotkey == nChar && lpHotkey->m_bShift == bShift && lpHotkey->m_bControl == bControl) 
			return lpHotkey;
	}
	return NULL;
}
int CHotkey::GetHotkeyType(int nChar,BOOL bShift,BOOL bControl)
{
	for(int i = 0; i < HOTKEY_MAX; i++)
	{
		LPHOTKEY lpHotkey = &m_HotkeyTable[i];
		if(lpHotkey->m_nHotkey == nChar && lpHotkey->m_bShift == bShift && lpHotkey->m_bControl == bControl) 
		{
			ItemProp* pItemProp = prj.GetItemProp(i);
			//if(pItemProp && pItemProp->m_nObjType == IT_KNOWLEDGE)
			//	return 2;
			return 1;
		}
	}
	return 0;
}
BOOL CHotkey::RemoveHotkey(int nChar,BOOL bShift,BOOL bControl)
{
	for(int i = 0; i < HOTKEY_MAX; i++)
	{
		LPHOTKEY lpHotkey = &m_HotkeyTable[i];
		if(lpHotkey->m_nHotkey == nChar && lpHotkey->m_bShift == bShift && lpHotkey->m_bControl == bControl) 
		{
			lpHotkey->m_bShift   = 0;
			lpHotkey->m_bControl = 0;
			lpHotkey->m_nHotkey  = 0;
			lpHotkey->m_bPressed = 0;
			return TRUE;
		}
	}
	return FALSE;
}
// nFunc = 기능 또는 지식 
// nChar = 키 또는 가상키
BOOL CHotkey::AddHotkey(int nFunc,int nChar,BOOL bShift,BOOL bControl)
{
	ItemProp* pItemProp = prj.GetItemProp(nFunc);
	LPHOTKEY lpHotkey = &m_HotkeyTable[nFunc];
	int nHotkeyType;
	if((nHotkeyType = GetHotkeyType(nChar,bShift,bControl)))
	{
		if(nHotkeyType == 2) // 지식 핫키 
			RemoveHotkey(nChar,bShift,bControl);
		else
		{
			//g_clientMessage.PutMessage(g_pMemDC, MKHIGHRGB(30, 16, 0),"단축키가 이미 지정되어있어 이전 명령을 수행합니다.");
			return FALSE;
		}
	}
	lpHotkey->m_nHotkey  = nChar   ;
	lpHotkey->m_bControl = bControl;
	lpHotkey->m_bShift   = bShift  ;
	lpHotkey->m_bPressed = FALSE   ;
	/*
	CString str;
	if(pItemProp && pItemProp->m_nObjType == 1 )//IT_KNOWLEDGE)
	{
		if(g_pWndMgr->m_pWndKnowledge)
			g_pWndMgr->m_pWndKnowledge->SetSubKnowListToolTip();
		str.Format("%s 지식 단축키가 정의되었습니다.",pItemProp->m_strName,nChar);
		//g_pPlayer->SetNewbieStatus(NEWBIE_HOTKEY);
	}
	else
		str.Format("단축키가 정의되었습니다.");
//	g_clientMessage.PutMessage(g_pMemDC, MKHIGHRGB(30, 16, 0), str);
	//SetConfig();
	*/
	return TRUE;
}
void CHotkey::ExecuteFunc(int nFunc)
{
	BOOL bIMEMode = 0;//g_pMugenMenu->IsChatOn() || g_pMugenMenu->m_BaseMenu[MENU_PULLITEMNO]->m_nOn || CWndEditCtrl::GetWndEditCtrl();
	BOOL GMFlag   = 0;//(g_pPlayer && g_pPlayer->IsSuperAccount()) ? TRUE : FALSE;

	//CWndKnowledge* pWndKnowledge = NULL;//(CWndKnowledge*)g_pWndMgr->GetWndBase(201);
	//CWndKnowledgeSub* pWndKnowledgeSub = (CWndKnowledgeSub*)g_pWndMgr->GetWndBase(202);

	int nSlotKey = -1;

	if(bIMEMode == TRUE)
	{
		if(nFunc == FUNC_CHAT)
		{
			//if(CWndEditCtrl::GetWndEditCtrl() == NULL)
			//	if(g_pMugenMenu->IsChatOn() && g_pMugenMenu->m_bChattingFixed == FALSE)
			//		g_pMugenMenu->DeActChatMenu();
		}
	}
	else
	{
		//if(g_pFieldWnd && g_pPlayer)
		{
			// 필드 상태에서 단축키 처리 
			switch(nFunc)
			{
			case FUNC_USE1: nSlotKey = 0; break;
			case FUNC_USE2: nSlotKey = 1; break;
			case FUNC_USE3: nSlotKey = 2; break;
			case FUNC_USE4: nSlotKey = 3; break;
			case FUNC_USE5: nSlotKey = 4; break;
			case FUNC_USE6: nSlotKey = 5; break;
			case FUNC_USE7: nSlotKey = 6; break;

			case FUNC_GUILD: 
				break;
			case FUNC_COMBAT: 
				break;
			case FUNC_EQUIPMENT: 
				break;
			case FUNC_KNOWLEDGE: 
				break;
			case FUNC_KNOWSUB: 
				break;
			case FUNC_STATUS: 
				break;
			case FUNC_INVENTORY: 
				break;
			case FUNC_ATTACKPC: 
				break;
			case FUNC_CHATLOG:
				break;
			case FUNC_LOOKHIDDEN:
				break;
			case FUNC_VIEWOBJECT:
				break;
			case FUNC_CLEARMSG:
				break;
			case FUNC_STOP: 
				break;
			case FUNC_CHAT: 
				break;
			case FUNC_KNOWLEFT: 
				break;
			case FUNC_KNOWRIGHT: 
				break;
			case FUNC_KNOWTOP: 
				break;
			case FUNC_OLDKNOW: 
				break;
			case FUNC_EXIT: 
				break;
			case FUNC_SLOT1: 
				break;
			case FUNC_SLOT2: 
				break;
			case FUNC_SLOT3: 
				break;
			case FUNC_VIEWPARTY: 
				break;
			default: 
				break;
			}
		}
		// 타이틀 또는 필드가 열린 상태에서 모두 처리 
		switch(nFunc)
		{
			case FUNC_HELP: 
				//g_pWndMgr->SendMessage(WM_COMMAND,1000);
				break;
			case FUNC_SCREENCAP: 
				//CaptureScreen(); 
				break;
		}
	}
	/*
	if(nSlotKey != -1 && g_pPlayer->IsLive())
	{
		CItem *pItem;
		pItem = g_pPlayer->m_QuickSlot.GetItem(g_pMugenMenu->m_nShowQuickItemMode*7 + nSlotKey);
		if( pItem )
		{
			CCmdFactor cmdFactor;
			cmdFactor.SetObjId(pItem->GetId());
			g_pPlayer->UseItem(cmdFactor);	
		}
	}
	*/
}
//////////////////////////////////////////////////////////
// 
// 단축키 관련
//
void CHotkey::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	/*
	g_bPressControl = (GetAsyncKeyState(VK_CONTROL) & 0x8000) ? TRUE : FALSE;
	g_bPressShift   = (GetAsyncKeyState(VK_SHIFT  ) & 0x8000) ? TRUE : FALSE;
	g_bPressAlt     = (GetAsyncKeyState(VK_MENU   ) & 0x8000) ? TRUE : FALSE;

	if(nChar == VK_CONTROL)	g_bPressControl = TRUE;
	if(nChar == VK_SHIFT  )	g_bPressShift   = TRUE;
	if(nChar == VK_MENU   )	g_bPressAlt     = TRUE;

	BOOL bIMEMode = g_pMugenMenu->IsChatOn() || g_pMugenMenu->m_BaseMenu[MENU_PULLITEMNO]->m_nOn || CWndEditCtrl::GetWndEditCtrl();
	CWndKnowledge* pWndKnowledge = (CWndKnowledge*)g_pWndMgr->GetWndBase(WID_KNOWLEDGE);
	CWndKnowledgeSub* pWndKnowledgeSub = (CWndKnowledgeSub*)g_pWndMgr->GetWndBase(WID_KNOWLEDGESUB);

	int nKnowIdxList = -1;
	if(bIMEMode == FALSE)
	{
		// 지식 단축키 처리 : 인터페이스에서 지식 항목에 하이라이트가 되어 있나?
		if(pWndKnowledge && pWndKnowledge->m_nShowKnowMode != -1)
		{
			for(int i = 0; i < 20; i++)
			{
				if(pWndKnowledge->m_wndButtonKnowSub[i] && pWndKnowledge->m_wndButtonKnowSub[i]->IsHighLight())
				{
					if(nChar != VK_SHIFT && nChar != VK_CONTROL)
					{
						nKnowIdxList = pWndKnowledge->m_nKnowIdxList[i];
						break;
					}
				}
			}
		}
		if(pWndKnowledgeSub && pWndKnowledgeSub->m_nShowKnowMode != -1)
		{
			for(int i = 0; i < 20; i++)
			{
				if(pWndKnowledgeSub->m_wndButtonKnowSub[i] && pWndKnowledgeSub->m_wndButtonKnowSub[i]->IsHighLight())
				{
					if(nChar != VK_SHIFT && nChar != VK_CONTROL)
					{
						nKnowIdxList = pWndKnowledgeSub->m_nKnowIdxList[i];
						break;
					}
				}
			}
		}
		// 지식 단축키 등록 
		if(nKnowIdxList != -1)
		{
			if(AddHotkey(nKnowIdxList,nChar,g_bPressShift,g_bPressControl))
				PlayEffect(SND_CLICKMENUICON);
			else
				nKnowIdxList = -1;
		}
	}
		if(nKnowIdxList == -1)
		{
			for(int i = 0; i < HOTKEY_MAX; i++)
			{
				LPHOTKEY lpHotkey = &m_HotkeyTable[i];
				if(lpHotkey->m_bPressed == FALSE && lpHotkey->m_nHotkey == nChar && lpHotkey->m_bShift == g_bPressShift && lpHotkey->m_bControl == g_bPressControl) //lpHotkey->m_bAlt == g_bPressAlt
				{
					lpHotkey->m_bPressed = TRUE;
					ExecuteFunc(i);
				}
			}
		}

	if( !g_pMugenInput->IsLockBut(nChar) )
	{
		g_pMugenInput->m_nBut[nChar]=PRESS;
		g_pMugenInput->LockBut(nChar);
	}
	*/
	//CFrameWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}
