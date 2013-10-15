#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndQuest.h"
#include "party.h"
#include "dpclient.h"
#include "definequest.h"
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
#include "WorldMap.h"
#endif // __IMPROVE_QUEST_INTERFACE
#ifdef __IMPROVE_MAP_SYSTEM
#include "WndMapEx.h"
#endif // __IMPROVE_MAP_SYSTEM
extern	CDPClient	g_DPlay;

#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
CTreeInformationManager g_QuestTreeInfoManager;
#endif // __IMPROVE_QUEST_INTERFACE


BOOL CWndRemoveQuest::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	CRect rect = m_pWndRoot->MakeCenterRect( 250, 130 );
	Create( "", MB_OKCANCEL, rect, APP_MESSAGEBOX );//dwWndId );

	m_wndText.SetString( prj.GetText(TID_GAME_QUEST_DELCONFIRM) );
	m_wndText.ResetString();
	if( g_WndMng.m_pWndWorld && g_WndMng.m_pWndWorld->GetMouseMode() == 1 )	// FPS모드일때
	{
		g_WndMng.m_pWndWorld->SetMouseMode( 0 );
	}
	return CWndMessageBox::Initialize( pWndParent, dwWndId );
}
BOOL CWndRemoveQuest::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	if( message == WNM_CLICKED  )
	{
		switch(nID)
		{
			case IDCANCEL:   
				Destroy(); 
				break;
			case IDOK:       
			{
				LPQUEST lpQuest = g_pPlayer->FindQuest( m_nRemoveQuestId );
				if( lpQuest )
					g_DPlay.SendRemoveQuest( m_nRemoveQuestId );
				Destroy(); 
			}
			break;
		}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}


CWndQuest::CWndQuest() 
{ 
	m_idSelQuest = -1;
}     
CWndQuest::~CWndQuest() 
{ 
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	if( g_WndMng.m_pWndQuestDetail )
		SAFE_DELETE( g_WndMng.m_pWndQuestDetail );
#endif // __IMPROVE_QUEST_INTERFACE
} 
void CWndQuest::OnDraw( C2DRender* p2DRender ) 
{ 
	//CWndText* pText4 = (CWndText*)GetDlgItem( WIDC_TEXT4 );
	//CRect rect = pText4->GetWindowRect( TRUE );
	//p2DRender->RenderTexture( rect.TopLeft(), &m_texChar, 255, 0.3, 0.3 );
} 
void CWndQuest::SerializeRegInfo( CAr& ar, DWORD& dwVersion )
{
	CWndNeuz::SerializeRegInfo( ar, dwVersion );
	if( ar.IsLoading() )
	{
		if( dwVersion == 0 )
		{
		}
		else
		{
			int nNum;
			WORD nQuest;
			ar >> m_idSelQuest;
			ar >> nNum;
			for( int i = 0; i < nNum; i++ )
			{
				ar >> nQuest;
				m_aOpenTree.Add( nQuest );
			}
			TreeOpen();
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
			RemoveQuest();
#else // __IMPROVE_QUEST_INTERFACE
			UpdateText();
#endif // __IMPROVE_QUEST_INTERFACE
		}
	}
	else
	{
		dwVersion = 1;
		ar << m_idSelQuest;
		ar << m_aOpenTree.GetSize();
		for( int i = 0; i < m_aOpenTree.GetSize(); i++ )
			ar << m_aOpenTree.GetAt( i );
	}
}
void CWndQuest::TreeOpen() 
{
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	CWndTabCtrl* pWndTabCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_QUEST_LIST_TABCTRL );
	assert( pWndTabCtrl );
	LPWTCITEM lpItem = pWndTabCtrl->GetSelectedTab();
	assert( lpItem );
	CWndQuestTreeCtrl* pTreeCtrl = ( CWndQuestTreeCtrl* )lpItem->pWndBase;
	assert( pTreeCtrl );
#else // __IMPROVE_QUEST_INTERFACE
	CWndTreeCtrl* pTreeCtrl = (CWndTreeCtrl*)GetDlgItem( WIDC_TREECTRL1 );
#endif // __IMPROVE_QUEST_INTERFACE
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	OpenTreeArray( pTreeCtrl->GetRootElem()->m_ptrArray );
#else // __IMPROVE_QUEST_INTERFACE
	LPTREEELEM lpRoot = pTreeCtrl->GetRootElem();
	int nPos = 0;
	LPTREEELEM lpElem = NULL;
	do {
		lpElem = pTreeCtrl->GetNextElem( lpRoot, nPos );
		if( lpElem )
		{
			for( int i = 0; i < m_aOpenTree.GetSize(); i++ )
			{
				if( m_aOpenTree.GetAt( i ) == lpElem->m_dwData )
				{
					lpElem->m_bOpen = TRUE;
					break;
				}
			}
		}
	} while( lpElem );
	pTreeCtrl->SetCurSel( m_idSelQuest );
#endif // __IMPROVE_QUEST_INTERFACE
}
void CWndQuest::Update( int nNewQuestId ) 
{
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	CWndTabCtrl* pWndTabCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_QUEST_LIST_TABCTRL );
	assert( pWndTabCtrl );
	LPWTCITEM lpItem = NULL;
	CWndQuestTreeCtrl* pTreeCtrl = NULL;
	for( int k = 0; k < pWndTabCtrl->GetSize(); ++k )
	{
		lpItem = pWndTabCtrl->GetTabItem( k );
		assert( lpItem );
		pTreeCtrl = ( CWndQuestTreeCtrl* )lpItem->pWndBase;
		assert( pTreeCtrl );
		pTreeCtrl->DeleteAllItems();
	}
	lpItem = pWndTabCtrl->GetSelectedTab();
	assert( lpItem );
	pTreeCtrl = ( CWndQuestTreeCtrl* )lpItem->pWndBase;
	assert( pTreeCtrl );
#else // __IMPROVE_QUEST_INTERFACE
	CWndTreeCtrl* pTreeCtrl = (CWndTreeCtrl*)GetDlgItem( WIDC_TREECTRL1 );
#endif // __IMPROVE_QUEST_INTERFACE

	LPQUEST lpQuest = NULL;

	CString strQuest;

	//pTreeCtrl->GetText( pTreeCtrl->GetCurSel(), strQuest );

	//pTreeCtrl->m_dwColor = 0xffffffff;//adeilson 0xff000000
#if __VER < 15 // __IMPROVE_QUEST_INTERFACE
	pTreeCtrl->DeleteAllItems();
#endif // __IMPROVE_QUEST_INTERFACE

#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	CDWordArray aOldHeadData;
	if( CTreeInformationManager::m_eQuestListGroup != CTreeInformationManager::COMPLETE_QUEST_LIST )
	{
		for( int i = 0; i < g_pPlayer->m_nQuestSize; ++i )
		{
			LPQUEST lpQuest = &g_pPlayer->m_aQuest[ i ];
			if( lpQuest->m_wId == 0xffff )
				continue;
			InsertQuestItem( lpQuest->m_wId, aOldHeadData, FALSE );
		}
	}
	if( CTreeInformationManager::m_eQuestListGroup != CTreeInformationManager::CURRENT_QUEST_LIST )
	{
		for( int i = 0; i < g_pPlayer->m_nCompleteQuestSize; ++i )
		{
			WORD wQuest = g_pPlayer->m_aCompleteQuest[ i ];
			if( wQuest == 0xffff )
				continue;
			InsertQuestItem( wQuest, aOldHeadData, TRUE, nNewQuestId );
		}
	}
#else // __IMPROVE_QUEST_INTERFACE
	QuestProp * pQuestProp;
	//CPtrArray* paQuestArray;
	
	CUIntArray* paHeadQuest;
	CPtrArray aOrderQuest;
	// 해드 퀘스트를 만든다. 
	int i;
	for( i = 0; i < g_pPlayer->m_nCompleteQuestSize; i++ )
	{
		WORD wQuest = g_pPlayer->m_aCompleteQuest[ i ];
		if( wQuest != 0xffff )
		{
			pQuestProp = prj.m_aPropQuest.GetAt( wQuest );
			if( pQuestProp )
			{
				for( int j = 0; j < aOrderQuest.GetSize(); j++ )
				{
					paHeadQuest = (CUIntArray*) aOrderQuest.GetAt( j );
					// 같은게 발견되면 뒤를 이어 추가 : 추가된건 일반 퀘스트 
					if( paHeadQuest->GetAt( 0 ) == pQuestProp->m_nHeadQuest ) 
					{
						paHeadQuest->Add( wQuest );
						break;
					}
				}
				// 발견 못했으면 해드 퀘스트를 처음 추가 
				if( j == aOrderQuest.GetSize() )
				{
					paHeadQuest = new CUIntArray;
					paHeadQuest->Add( pQuestProp->m_nHeadQuest );
					paHeadQuest->Add( wQuest );
					aOrderQuest.Add( paHeadQuest );
				}
			}
			else
			{
				CString string;
				string.Format( "CWndQuest::Update 1 : Quest %d의 pQuestProp NULL이다.", wQuest );
				ADDERRORMSG( string );
			}
		}
	}
	for( i = 0; i < g_pPlayer->m_nQuestSize; i++ )
	{
		lpQuest = &g_pPlayer->m_aQuest[ i ];
		if( lpQuest->m_wId != 0xffff )
		{
			pQuestProp = prj.m_aPropQuest.GetAt( lpQuest->m_wId );
			if( pQuestProp )
			{
				for( int j = 0; j < aOrderQuest.GetSize(); j++ )
				{
					paHeadQuest = (CUIntArray*) aOrderQuest.GetAt( j );
					// 같은게 발견되면 뒤를 이어 추가 : 추가된건 일반 퀘스트 
					if( paHeadQuest->GetAt( 0 ) == pQuestProp->m_nHeadQuest ) 
					{
						paHeadQuest->Add( lpQuest->m_wId );
						break;
					}
				}
				// 발견 못했으면 해드 퀘스트를 처음 추가 
				if( j == aOrderQuest.GetSize() )
				{
					paHeadQuest = new CUIntArray;
					paHeadQuest->Add( pQuestProp->m_nHeadQuest );
					paHeadQuest->Add( lpQuest->m_wId );
					aOrderQuest.Add( paHeadQuest );
				}
			}
			else
			{
				CString string;
				string.Format( "CWndQuest::Update 2 : Quest %d의 pQuestProp NULL이다.", lpQuest->m_wId );
				ADDERRORMSG( string );
			}
		}
	}

	// 퀘스트를 list에 넣는다.
	QUEST questTemp;
	BOOL bCompleteQuest;
	for( i = 0; i < aOrderQuest.GetSize(); i++ )
	{
		paHeadQuest = (CUIntArray*)aOrderQuest.GetAt( i );
		LPTREEELEM lpTreeElem = NULL;
		for( int j = 0; j < paHeadQuest->GetSize(); j++ )
		{
			int nQuestId = paHeadQuest->GetAt( j );
			QuestProp * pQuestProp = prj.m_aPropQuest.GetAt( nQuestId );
			if( pQuestProp )
			{
				lpQuest = g_pPlayer->GetQuest( nQuestId );
				bCompleteQuest = FALSE;
				if( lpQuest == NULL )
				{
					if( g_pPlayer->IsCompleteQuest( nQuestId ) )
					{
						g_pPlayer->MakeCompleteQuest( nQuestId, &questTemp );
						lpQuest = &questTemp;
						bCompleteQuest = TRUE;
					}
				}
				// 실제 퀘스트 
				CString string;
				if( lpQuest )
				{
					CString strState;
					if( g_Option.m_bOperator || g_pPlayer->IsAuthHigher( AUTH_GAMEMASTER ) )
						strState.Format( "(%d,%d)", lpQuest->m_nState, lpQuest->m_wId );
					string.Format( "%s%s", pQuestProp->m_szTitle, strState );
					if( bCompleteQuest )
						pTreeCtrl->InsertItem( lpTreeElem, string, nQuestId )->m_dwColor = 0xffc0c0c0;
					else
						pTreeCtrl->InsertItem( lpTreeElem, string, nQuestId );

				}
				// 해드 퀘스트 
				else
				{
					string.Format( "%s", pQuestProp->m_szTitle );
					lpTreeElem = pTreeCtrl->InsertItem( NULL, string, nQuestId );
					// 새 퀘스트의 헤드랑 나랑 같으면 트리 자동 오픈 
					if( nNewQuestId != -1 && prj.m_aPropQuest.GetAt( nNewQuestId )->m_nHeadQuest == nQuestId )
					{
						lpTreeElem->m_bOpen = TRUE;
						m_aOpenTree.Add( nQuestId );
					}
				}
			}
		}
		safe_delete( paHeadQuest );
	}
#endif // __IMPROVE_QUEST_INTERFACE
	if( nNewQuestId != -1 )
		m_idSelQuest = nNewQuestId;
	TreeOpen();
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	RemoveQuest();
#else // __IMPROVE_QUEST_INTERFACE
	UpdateText();
#endif // __IMPROVE_QUEST_INTERFACE
}

CString CWndQuest::MakeQuestString( CString& string, BOOL bCond ) 
{
	CString strResult;
	if( bCond )
		strResult = "#s#cffa0a0a0" + string + "#ns#nc\n";
	else
		strResult = string + "\n";
	return strResult;
}

#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
void CWndQuest::RemoveQuest( void )
{
	CWndTabCtrl* pWndTabCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_QUEST_LIST_TABCTRL );
	assert( pWndTabCtrl );
	LPWTCITEM lpItem = pWndTabCtrl->GetSelectedTab();
	assert( lpItem );
	CWndQuestTreeCtrl* pTreeCtrl = ( CWndQuestTreeCtrl* )lpItem->pWndBase;
	assert( pTreeCtrl );
	CWndButton* pWndRemove = (CWndButton*)GetDlgItem( WIDC_REMOVE );
	assert( pWndRemove );
	LPQUEST lpQuest = NULL;
	LPTREEELEM lpTreeElem = pTreeCtrl->GetCurSel();
	int nQuest = -1;
	if( lpTreeElem )
	{
		nQuest = lpTreeElem->m_dwData ;
		lpQuest = g_pPlayer->FindQuest( nQuest );
	}
	BOOL bComplete = nQuest != -1 && g_pPlayer->IsCompleteQuest( nQuest ); 
	if( lpQuest || bComplete )
	{
		CWndDialog* pWndDialog = (CWndDialog*)g_WndMng.GetWndBase( APP_DIALOG_EX );
		QuestProp* pQuestProp = prj.m_aPropQuest.GetAt( nQuest );
		// 운영자는 언제나 퀘스트를 지울 수 있다
		if( g_pPlayer->IsAuthHigher( AUTH_GAMEMASTER ) && g_Option.m_bOperator )
			pWndRemove->EnableWindow( TRUE );
		else if( bComplete || lpQuest->m_nState == QS_END || pWndDialog )
			pWndRemove->EnableWindow( FALSE );
		else
		{
			if( pQuestProp->m_bNoRemove )
				pWndRemove->EnableWindow( FALSE );
			else
				pWndRemove->EnableWindow( TRUE );
		}
	}
	else
		pWndRemove->EnableWindow( FALSE );
}
#else // __IMPROVE_QUEST_INTERFACE
#if __VER >= 13 // __QUEST_HELPER
void CWndQuest::UpdateText(BOOL bClick)
#else //__QUEST_HELPER
void CWndQuest::UpdateText()
#endif //__QUEST_HELPER
{
	QuestProp* pQuestProp; 
	CWndTreeCtrl* pTreeCtrl = (CWndTreeCtrl*)GetDlgItem( WIDC_TREECTRL1 );
	CWndButton* pWndRemove = (CWndButton*)GetDlgItem( WIDC_REMOVE );
	CWndText* pTextDesc   = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	CWndText* pTextCond   = (CWndText*)GetDlgItem( WIDC_TEXT2 );
	CWndText* pTextReward  = (CWndText*)GetDlgItem( WIDC_TEXT3 );
	LPQUEST lpQuest = NULL;
	LPTREEELEM lpTreeElem = pTreeCtrl->GetCurSel();
	int nQuest = -1;
	if( lpTreeElem && lpTreeElem->m_lpParent )
	{
		nQuest = lpTreeElem->m_dwData ;
		lpQuest = g_pPlayer->FindQuest( nQuest );
	}
	BOOL bComplete = nQuest != -1 && g_pPlayer->IsCompleteQuest( nQuest ); 

	if( lpQuest || bComplete ) 
	{
#ifdef __CSC_VER12_3
		CWndDialog* pWndDialog = (CWndDialog*)g_WndMng.GetWndBase( APP_DIALOG_EX );
#else //__CSC_VER12_3
		CWndDialog* pWndDialog = (CWndDialog*)g_WndMng.GetWndBase( APP_DIALOG );
#endif //__CSC_VER12_3
		pQuestProp = prj.m_aPropQuest.GetAt( nQuest );
		// 영자는 언제나 퀘스트를 지울 수 있다.
		if( g_pPlayer->IsAuthHigher( AUTH_GAMEMASTER ) && g_Option.m_bOperator )
			pWndRemove->EnableWindow( TRUE );
		else
		if( bComplete || lpQuest->m_nState == QS_END || pWndDialog )
		{
			pWndRemove->EnableWindow( FALSE );
		}
		else
		{
			if( pQuestProp->m_bNoRemove )
				pWndRemove->EnableWindow( FALSE );
			else
				pWndRemove->EnableWindow( TRUE );
		}

		// 디스크립션만 출력
		if( bComplete )
		{
			for( int i = QS_END; i >= 0; i-- )
			{
				QuestState* pQuestState = pQuestProp->m_questState[ i ];
				if( pQuestState )
				{
					pTextDesc->SetString( pQuestState->m_szDesc, 0xffffffff );//ADEILSON 0xff000000
					break;
				}
			}
		}
		else
		{
			QuestState* pQuestState = pQuestProp->m_questState[ lpQuest->m_nState ];
			pTextDesc->SetString( pQuestState->m_szDesc, 0xffffffff );//ADEILSON 0xff000000
		}
		//////////////////////////////////////////////////////////////////////////////////
		// 퀘스트 종료 조건 
		//////////////////////////////////////////////////////////////////////////////////
		CString strTemp;
		CString strCond;
		CString strComplete;

#ifdef __BS_PUTNAME_QUESTARROW
		CWndWorld* pWndWorld = g_WndMng.m_pWndWorld;
		if(pWndWorld)
			pWndWorld->m_strDestName = "";
#endif

		if( bComplete )
			pTextCond->SetString( GETTEXT( TID_QUEST_COMPLETED  ), 0xff404040 );
		else
		{
			for( int i = 0; i < 14; i++ )
			{
				if( pQuestProp->m_questState[ i ] == NULL )
					break;
			}
			if( lpQuest->m_nState == ( i - 1 ) || lpQuest->m_nState == 14 )
			{
				int nNum;
				if( pQuestProp->m_nEndCondLimitTime )
				{
					if( lpQuest->m_wTime )
					{
						strTemp.Format( GETTEXT( TID_QUEST_LIMIT_TIME ), lpQuest->m_wTime & 0x7fff );
						strCond += strTemp + "\n";
					}
					else
					{
						strTemp.Format( GETTEXT( TID_QUEST_LIMIT_TIMEOUT ) );
						strCond += "#cffff0000" + strTemp + "#ns#nc\n";
					}
				}
				for( int i = 0 ; i < MAX_QUEST_COND_KILL; i++ )//2
				{
					if( pQuestProp->m_nEndCondKillNPCIdx[ i ] )
					{
						MoverProp* pMoverProp = prj.GetMoverProp( pQuestProp->m_nEndCondKillNPCIdx[ i ] );
						nNum = lpQuest->m_nKillNPCNum[ i ];
						if( nNum > pQuestProp->m_nEndCondKillNPCNum[ i ] )
							nNum = pQuestProp->m_nEndCondKillNPCNum[ i ];
						strTemp.Format( GETTEXT( TID_QUEST_KILL_NPC ), pMoverProp->szName, lpQuest->m_nKillNPCNum[ i ], pQuestProp->m_nEndCondKillNPCNum[ i ] );

						if( nNum == pQuestProp->m_nEndCondKillNPCNum[ i ] )
							strCond += "#s#cffa0a0a0" + strTemp + "#ns#nc\n";
						else 
							strCond += strTemp + "\n";
					}
				}
				for( i = 0; i < pQuestProp->m_nEndCondItemNum; i++ )
				{
					QuestPropItem* pEndCondItem = &pQuestProp->m_paEndCondItem[ i ];

					if( pEndCondItem->m_nSex == -1 || pEndCondItem->m_nSex == g_pPlayer->GetSex() )
					{
						if( pEndCondItem->m_nType == 0 )
						{
							if( pEndCondItem->m_nJobOrItem == -1 || pEndCondItem->m_nJobOrItem == g_pPlayer->GetJob() )
							{
								if( pEndCondItem->m_nItemIdx )
								{
									ItemProp* pItemProp = prj.GetItemProp( pEndCondItem->m_nItemIdx );
									nNum = g_pPlayer->GetItemNum( pEndCondItem->m_nItemIdx );
									strTemp.Format( GETTEXT( TID_QUEST_ITEM ), pItemProp->szName, nNum, pEndCondItem->m_nItemNum );

									if( nNum >= pEndCondItem->m_nItemNum )
										strCond += "#s#cffa0a0a0" + strTemp + "#ns#nc\n";
									else 
										strCond += strTemp + "\n";
								}
							}
						}
						else
						if( pEndCondItem->m_nType == 1 )
						{
							if( pEndCondItem->m_nJobOrItem == -1 || g_pPlayer->GetItemNum( pEndCondItem->m_nJobOrItem ) )
							{
								if( pEndCondItem->m_nItemIdx )
								{
									ItemProp* pItemProp = prj.GetItemProp( pEndCondItem->m_nItemIdx );
									nNum = g_pPlayer->GetItemNum( pEndCondItem->m_nItemIdx );
									strTemp.Format( GETTEXT( TID_QUEST_ITEM ), pItemProp->szName, nNum, pEndCondItem->m_nItemNum );

									if( nNum >= pEndCondItem->m_nItemNum )
										strCond += "#s#cffa0a0a0" + strTemp + "#ns#nc\n";
									else 
										strCond += strTemp + "\n";
								}

							}
						}
					}
				}
				if( pQuestProp->m_dwEndCondPatrolWorld )
				{
					strTemp.Format( GETTEXT( TID_QUEST_PATROL ), pQuestProp->m_szPatrolZoneName );
					if( lpQuest->m_bPatrol )
						strCond += "#s#cffa0a0a0" + strTemp + "#ns#nc\n";
					else
						strCond += strTemp + "\n";
				}
				if( pQuestProp->m_nEndCondDisguiseMoverIndex )
				{
					if( pQuestProp->m_nEndCondDisguiseMoverIndex == -1 )
					{
						strTemp.Format( GETTEXT( TID_QUEST_NODISQUISE ) );
						if( g_pPlayer->IsDisguise() == FALSE )
							strCond += "#s#cffa0a0a0" + strTemp + "#ns#nc\n";
						else
							strCond += strTemp + "\n";
					}
					else
					{
						MoverProp* pMoverProp = prj.GetMoverProp( pQuestProp->m_nEndCondDisguiseMoverIndex );
						strTemp.Format( GETTEXT( TID_QUEST_DISQUISE ), pMoverProp->szName );
						if( g_pPlayer->IsDisguise() )
							strCond += "#s#cffa0a0a0" + strTemp + "#ns#nc\n";
						else
							strCond += strTemp + "\n";
					}
				}
				if( pQuestProp->m_nEndCondSkillIdx ) 
				{
					ItemProp* pSkillProp = prj.GetSkillProp( pQuestProp->m_nEndCondSkillIdx );
					LPSKILL lpSkill = g_pPlayer->GetSkill( pQuestProp->m_nEndCondSkillIdx );
					if( g_pPlayer->CheckSkill( pQuestProp->m_nEndCondSkillIdx ) && lpSkill )
					{
						strTemp.Format( GETTEXT( TID_QUEST_SKILL_LVL ), pSkillProp->szName, lpSkill->dwLevel, pQuestProp->m_nEndCondSkillLvl );
						if( lpSkill->dwLevel >= pQuestProp->m_nEndCondSkillLvl )
							strCond += "#s#cffa0a0a0" + strTemp + "#ns#nc\n";
						else
							strCond += strTemp + "\n";
					}
					else
					{
						strTemp.Format( GETTEXT( TID_QUEST_SKILL_LVL ), pSkillProp->szName, 0, pQuestProp->m_nEndCondSkillLvl );
						strCond += strTemp + "\n";
					}
				}
#if __VER < 8 // __S8_PK
				if( pQuestProp->m_nEndCondKarmaPoint ) 
				{
					if( pQuestProp->m_nEndCondKarmaComp == 0 )
					{
						strTemp.Format( GETTEXT( TID_QUEST_KARMA_POINT ), g_pPlayer->GetKarma(),"=",pQuestProp->m_nEndCondKarmaPoint );
						if( g_pPlayer->GetKarma() == pQuestProp->m_nEndCondKarmaPoint )
							strCond += "#s#cffa0a0a0" + strTemp + "#ns#nc\n";
						else
							strCond += strTemp + "\n";
					}
					else
					if( pQuestProp->m_nEndCondKarmaComp == -1 )
					{
						strTemp.Format( GETTEXT( TID_QUEST_KARMA_POINT ), g_pPlayer->GetKarma(),"<=",pQuestProp->m_nEndCondKarmaPoint );
						if( g_pPlayer->GetKarma() <= pQuestProp->m_nEndCondKarmaPoint )
							strCond += "#s#cffa0a0a0" + strTemp + "#ns#nc\n";
						else
							strCond += strTemp + "\n";
					}
					else
					if( pQuestProp->m_nEndCondKarmaComp == 1)
					{
						strTemp.Format( GETTEXT( TID_QUEST_KARMA_POINT ), g_pPlayer->GetKarma(),">=",pQuestProp->m_nEndCondKarmaPoint );
						if( g_pPlayer->GetKarma() >= pQuestProp->m_nEndCondKarmaPoint )
							strCond += "#s#cffa0a0a0" + strTemp + "#ns#nc\n";
						else
							strCond += strTemp + "\n";
					}
				}
#endif // __VER < 8 // __S8_PK
				// 파티 체크 
				if( pQuestProp->m_nEndCondParty != 0 ) 
				{
					BOOL bLeader = FALSE;
					BOOL bParty  = g_Party.IsMember( g_pPlayer->m_idPlayer );
					int nSize = g_Party.GetSizeofMember();

					if( pQuestProp->m_nEndCondParty == 1 ) // 싱글이어야함 
					{
						strTemp.Format( GETTEXT( TID_QUEST_PARTY_SOLO ) );
						strCond += MakeQuestString( strTemp, !bParty );
					}
					else
					if( pQuestProp->m_nEndCondParty == 2 ) // 파티여야함
					{
						int nTextId = 0;
						BOOL bResult = FALSE;

						if( pQuestProp->m_nEndCondPartyNum == 0 ) 
						{
							if( pQuestProp->m_nEndCondPartyLeader == 0 ) // 파티 여부 (0 == 0 )
								nTextId = TID_QUEST_PARTY;
							else // 파티, 맴버 여부 (0 == 0 ) 
							if( pQuestProp->m_nEndCondPartyLeader == 1 )
								nTextId = TID_QUEST_PARTY_MEMBER;
							else // 파티, 리더 여부 (0 == 0 ) 
							if( pQuestProp->m_nEndCondPartyLeader == 2 )
								nTextId = TID_QUEST_PARTY_LEADER;

							strTemp.Format( GETTEXT( nTextId ) );

							if( pQuestProp->m_nEndCondPartyLeader == 0 ) // 파티 여부 
								bResult = TRUE;
							else // 파티, 맴버 여부 
							if( ( pQuestProp->m_nEndCondPartyLeader - 1 ) == bLeader )
								bResult = TRUE;
						}
						else
						{
							if( pQuestProp->m_nEndCondPartyLeader == 0 ) // 파티 여부 (0 == 0 )
								nTextId = TID_QUEST_PARTY_NUM;
							else // 파티, 맴버 여부 (0 == 0 ) 
							if( pQuestProp->m_nEndCondPartyLeader == 1 )
								nTextId = TID_QUEST_PARTY_MEMBER_NUM;
							else // 파티, 리더 여부 (0 == 0 ) 
							if( pQuestProp->m_nEndCondPartyLeader == 2 )
								nTextId = TID_QUEST_PARTY_LEADER_NUM;

							if( pQuestProp->m_nEndCondPartyNumComp == 0 ) 
							{
								strTemp.Format( GETTEXT( nTextId ), nSize,"=",pQuestProp->m_nEndCondPartyNum );
								if( nSize == pQuestProp->m_nEndCondPartyNum )
									bResult = TRUE;
							}
							else
							if( pQuestProp->m_nEndCondPartyNumComp == -1 )
							{
								strTemp.Format( GETTEXT( nTextId ), nSize,"<=",pQuestProp->m_nEndCondPartyNum );
								if( nSize <= pQuestProp->m_nEndCondPartyNum )
									bResult = TRUE;
							}
							else
							if( pQuestProp->m_nEndCondPartyNumComp == 1)
							{
								strTemp.Format( GETTEXT( nTextId ), nSize,">=",pQuestProp->m_nEndCondPartyNum );
								if( nSize >= pQuestProp->m_nEndCondPartyNum )
									bResult = TRUE;
							}
						}
						strCond += MakeQuestString( strTemp, bResult && bParty );
					}
				}
				// 길드 체크 
				if( pQuestProp->m_nEndCondGuild != 0 ) 
				{
					BOOL bLeader = FALSE;
					BOOL bGuild  = g_Party.IsMember( g_pPlayer->m_idPlayer );
					int nSize = g_Party.GetSizeofMember();



					if( pQuestProp->m_nEndCondGuild == 1 ) // 길드가 아니어야됨 
					{
						strTemp.Format( GETTEXT( TID_QUEST_GUILD_SOLO ) );
						strCond += MakeQuestString( strTemp, !bGuild );
					}
					else
					if( pQuestProp->m_nEndCondGuild == 2 ) // 길드여야됨 
					{
						int nTextId = 0;
						BOOL bResult = FALSE;

						if( pQuestProp->m_nEndCondGuildNum == 0 ) 
						{
							if( pQuestProp->m_nEndCondGuildLeader == 0 ) // 파티 여부 (0 == 0 )
								nTextId = TID_QUEST_GUILD;
							else // 파티, 맴버 여부 (0 == 0 ) 
							if( pQuestProp->m_nEndCondGuildLeader == 1 )
								nTextId = TID_QUEST_GUILD_MEMBER;
							else // 파티, 리더 여부 (0 == 0 ) 
							if( pQuestProp->m_nEndCondGuildLeader == 2 )
								nTextId = TID_QUEST_GUILD_LEADER;

							strTemp.Format( GETTEXT( nTextId ) );

							if( pQuestProp->m_nEndCondGuildLeader == 0 ) // 길드 여부 
								bResult = TRUE;
							else // 길드, 리더 여부 
							if( ( pQuestProp->m_nEndCondGuildLeader - 1 ) == bLeader )
								bResult = TRUE;
						}
						else
						{
							if( pQuestProp->m_nEndCondGuildLeader == 0 ) // 길드 여부 (0 == 0 )
								nTextId = TID_QUEST_GUILD_NUM;
							else // 길드, 맴버 여부 (0 == 0 ) 
							if( pQuestProp->m_nEndCondGuildLeader == 1 )
								nTextId = TID_QUEST_GUILD_MEMBER_NUM;
							else // 길드, 리더 여부 (0 == 0 ) 
							if( pQuestProp->m_nEndCondGuildLeader == 2 )
								nTextId = TID_QUEST_GUILD_LEADER_NUM;

							if( pQuestProp->m_nEndCondGuildNumComp == 0 ) 
							{
								strTemp.Format( GETTEXT( nTextId ), nSize,"=",pQuestProp->m_nEndCondGuildNum );
								if( nSize == pQuestProp->m_nEndCondGuildNum )
									bResult = TRUE;
							}
							else
							if( pQuestProp->m_nEndCondGuildNumComp == -1 )
							{
								strTemp.Format( GETTEXT( nTextId ), nSize,"<=",pQuestProp->m_nEndCondGuildNum );
								if( nSize <= pQuestProp->m_nEndCondGuildNum )
									bResult = TRUE;
							}
							else
							if( pQuestProp->m_nEndCondGuildNumComp == 1)
							{
								strTemp.Format( GETTEXT( nTextId ), nSize,">=",pQuestProp->m_nEndCondGuildNum );
								if( nSize >= pQuestProp->m_nEndCondGuildNum )
									bResult = TRUE;
							}
						}
						strCond += MakeQuestString( strTemp, bResult && bGuild );
					}
				}
				//////////////////////////////////////////////////////
#if __VER >= 8 // __S8_PK
				CString strEndCondOneItem;
				CString strEndCondOneItemComplete;
				for( i = 0; i < pQuestProp->m_nEndCondOneItemNum; i++ )
				{
					QuestPropItem* pEndCondOneItem = &pQuestProp->m_paEndCondOneItem[ i ];
					
					if( pEndCondOneItem->m_nSex == -1 || pEndCondOneItem->m_nSex == g_pPlayer->GetSex() )
					{
						if( pEndCondOneItem->m_nType == 0 )
						{
							if( pEndCondOneItem->m_nJobOrItem == -1 || pEndCondOneItem->m_nJobOrItem == g_pPlayer->GetJob() )
							{
								if( pEndCondOneItem->m_nItemIdx )
								{
									ItemProp* pItemProp = prj.GetItemProp( pEndCondOneItem->m_nItemIdx );
									nNum = g_pPlayer->GetItemNum( pEndCondOneItem->m_nItemIdx );
									if( nNum >= pEndCondOneItem->m_nItemNum )
									{
										strEndCondOneItemComplete = "#s#cffa0a0a0";
										break;
									}
								}
							}
						}
						else
						if( pEndCondOneItem->m_nType == 1 )
						{
							if( pEndCondOneItem->m_nJobOrItem == -1 || g_pPlayer->GetItemNum( pEndCondOneItem->m_nJobOrItem ) )
							{
								if( pEndCondOneItem->m_nItemIdx )
								{
									ItemProp* pItemProp = prj.GetItemProp( pEndCondOneItem->m_nItemIdx );
									nNum = g_pPlayer->GetItemNum( pEndCondOneItem->m_nItemIdx );									
									if( nNum >= pEndCondOneItem->m_nItemNum )
									{
										strEndCondOneItemComplete = "#s#cffa0a0a0";
										break;
									}
								}
								
							}
						}
					}
				}
				for( i = 0; i < pQuestProp->m_nEndCondOneItemNum; i++ )
				{
					QuestPropItem* pEndCondOneItem = &pQuestProp->m_paEndCondOneItem[ i ];
					
					if( pEndCondOneItem->m_nSex == -1 || pEndCondOneItem->m_nSex == g_pPlayer->GetSex() )
					{
						if( pEndCondOneItem->m_nType == 0 )
						{
							if( pEndCondOneItem->m_nJobOrItem == -1 || pEndCondOneItem->m_nJobOrItem == g_pPlayer->GetJob() )
							{
								if( pEndCondOneItem->m_nItemIdx )
								{
									ItemProp* pItemProp = prj.GetItemProp( pEndCondOneItem->m_nItemIdx );
									nNum = g_pPlayer->GetItemNum( pEndCondOneItem->m_nItemIdx );
									strTemp.Format( GETTEXT( TID_QUEST_ITEM ), pItemProp->szName, nNum, pEndCondOneItem->m_nItemNum );
									
									strEndCondOneItem += strEndCondOneItemComplete + "-" + strTemp + "#ns#nc\n";
								}
							}
						}
						else
						if( pEndCondOneItem->m_nType == 1 )
						{
							if( pEndCondOneItem->m_nJobOrItem == -1 || g_pPlayer->GetItemNum( pEndCondOneItem->m_nJobOrItem ) )
							{
								if( pEndCondOneItem->m_nItemIdx )
								{
									ItemProp* pItemProp = prj.GetItemProp( pEndCondOneItem->m_nItemIdx );
									nNum = g_pPlayer->GetItemNum( pEndCondOneItem->m_nItemIdx );
									strTemp.Format( GETTEXT( TID_QUEST_ITEM ), pItemProp->szName, nNum, pEndCondOneItem->m_nItemNum );
									
									strEndCondOneItem += strEndCondOneItemComplete + "-" + strTemp + "#ns#nc\n";								
								}
								
							}
						}
					}
				}
				if( 0 < strEndCondOneItem.GetLength() )
				{
					strTemp.Format( GETTEXT( TID_QUEST_COND_ITEM ) );
					strCond += strEndCondOneItemComplete + strTemp + "#ns#nc\n";
					strCond += strEndCondOneItem;
				}
#else // __VER >= 8 // __S8_PK
				if( pQuestProp->m_nEndCondChaotic ) 
				{
					if( pQuestProp->m_nEndCondChaotic == 1 )
					{
						strTemp.Format( GETTEXT( TID_QUEST_CAOTIC1 ) );
						if( g_pPlayer->IsChaotic() )
							strCond += "#s#cffa0a0a0" + strTemp + "#ns#nc\n";
						else
							strCond += strTemp + "\n";
					}
					else
					if( pQuestProp->m_nEndCondChaotic == 2 )
					{
						strTemp.Format( GETTEXT( TID_QUEST_CAOTIC2 ) );
						if( g_pPlayer->IsChaotic() == FALSE )
							strCond += "#s#cffa0a0a0" + strTemp + "#ns#nc\n";
						else
							strCond += strTemp + "\n";
					}
				}
#endif // __VER < 8 // __S8_PK
				if( pQuestProp->m_szEndCondDlgCharKey[ 0 ] )
				{
					LPCHARACTER lpCharacter = prj.GetCharacter( pQuestProp->m_szEndCondDlgCharKey );
					strTemp.Format( GETTEXT( TID_QUEST_DIALOG ), lpCharacter->m_strName );
					if( lpQuest->m_bDialog )
						strCond += "#s#cffa0a0a0" + strTemp + "#ns#nc\n";
					else
						strCond += strTemp + "\n";
				}
				if( pQuestProp->m_szEndCondCharacter[ 0 ] ) 
				{
					LPCHARACTER lpCharacter = prj.GetCharacter( pQuestProp->m_szEndCondCharacter );
					strTemp.Format( GETTEXT( TID_QUEST_DESTINATION ), lpCharacter->m_strName );
					strCond += strTemp + "\n";
#if __VER >= 13 // __QUEST_HELPER
					if(bClick) //클릭해서 업데이트 되는 경우에만 좌표를 요청한다.
					{
						CWndWorld* pWndWorld = g_WndMng.m_pWndWorld;
						if(pWndWorld)
						{
							CWorld* pWorld = g_WorldMng();
							if(pWorld)
								pWorld->SetObjFocus(NULL);

							pWndWorld->m_bSetQuestNPCDest = FALSE;
							pWndWorld->m_pNextTargetObj = NULL;
#ifdef __BS_PUTNAME_QUESTARROW
							pWndWorld->m_strDestName = lpCharacter->m_strName;
#endif
						}

						if(pQuestProp->m_nHeadQuest != 1992)
							g_DPlay.SendReqNPCPos(lpCharacter->m_szKey);
					}
#endif //__QUEST_HELPER
				}
				else
				if( pQuestProp->m_lpszEndCondMultiCharacter ) 
				{
					for( int i = 0; i < 10; i++ )
					{
						CHAR* lpszChar = &pQuestProp->m_lpszEndCondMultiCharacter[ i * 64 ];
						if( lpszChar[ 0 ] )
						{
							LPCHARACTER lpCharacter = prj.GetCharacter( lpszChar );
							if( lpCharacter )
							{
								for( int j = 0; j < lpCharacter->m_anDstQuestItem.GetSize(); j++ )
								{
									if( g_pPlayer->GetItemNum( lpCharacter->m_anDstQuestItem.GetAt( j ) ) )
									{
										strTemp.Format( GETTEXT( TID_QUEST_DESTINATION ), lpCharacter->m_strName );
										strCond += strTemp + "\n";
										i = 10;
										break;
									}
								}
							}
						}
					}
				}
			}
			pTextCond->SetString( strCond, 0xff404040 );
		}

		//////////////////////////////////////////////////////////////////////////////////
		// 보상 아이템 목록
		//////////////////////////////////////////////////////////////////////////////////
		CString strReward;

		for( int i = 0; i < pQuestProp->m_nEndRewardItemNum; i++ )
		{
			QuestPropItem* pEndRewardItem = &pQuestProp->m_paEndRewardItem[ i ]; 

			if( pEndRewardItem->m_nSex == -1 || pEndRewardItem->m_nSex == g_pPlayer->GetSex() )

			{
				if( pEndRewardItem->m_nType == 0 )
				{
					if( pEndRewardItem->m_nJobOrItem == -1 || pEndRewardItem->m_nJobOrItem == g_pPlayer->GetJob() )
					{

						if( pEndRewardItem->m_nItemIdx )
						{
							ItemProp* pItemProp = prj.GetItemProp( pEndRewardItem->m_nItemIdx );
							if( pQuestProp->m_bEndRewardHide == FALSE || bComplete == TRUE )
								strTemp.Format( GETTEXT( TID_QUEST_ITEM1 ), pItemProp->szName, pEndRewardItem->m_nItemNum );
							else
								strTemp.Format( GETTEXT( TID_QUEST_ITEM2 ) );
							strReward += strTemp + "\n";
						}

					}
				}
				else
				if( pEndRewardItem->m_nType == 1 )
				{
					if( pEndRewardItem->m_nJobOrItem == -1 || g_pPlayer->GetItemNum( pEndRewardItem->m_nJobOrItem ) ) 
					{
						if( pEndRewardItem->m_nItemIdx )
						{
							ItemProp* pItemProp = prj.GetItemProp( pEndRewardItem->m_nItemIdx );
							if( pQuestProp->m_bEndRewardHide == FALSE || bComplete == TRUE )
								strTemp.Format( GETTEXT( TID_QUEST_ITEM1 ), pItemProp->szName, pEndRewardItem->m_nItemNum );
							else
								strTemp.Format( GETTEXT( TID_QUEST_ITEM2 ) );
							strReward += strTemp + "\n";
						}
					}
				}
			}
		}
		if( pQuestProp->m_nEndRewardGoldMin )
		{
			if( pQuestProp->m_bEndRewardHide == FALSE || bComplete == TRUE )
			{
				if( ( pQuestProp->m_nEndRewardGoldMax - pQuestProp->m_nEndRewardGoldMin ) == 0 )
					strTemp.Format( GETTEXT( TID_QUEST_GOLD1 ), pQuestProp->m_nEndRewardGoldMin );
				else
					strTemp.Format( GETTEXT( TID_QUEST_GOLD2 ), pQuestProp->m_nEndRewardGoldMin, pQuestProp->m_nEndRewardGoldMax );
			}
			else
				strTemp.Format( GETTEXT( TID_QUEST_GOLD3 ) );
			strReward += strTemp + "\n";
		}
		if( pQuestProp->m_nEndRewardExpMin )
		{
			strTemp.Format( GETTEXT( TID_QUEST_EXP1 ) );
			strReward += strTemp + "\n";
		}
		if( pQuestProp->m_nEndRewardSkillPoint )
		{
			strTemp.Format( GETTEXT( TID_QUEST_SKILLPOINT ) );
			strReward += strTemp + "\n";
		}
#if __VER >= 8 // __S8_PK
		if( pQuestProp->m_nEndRewardPKValueMin )
		{
			strTemp.Format( GETTEXT( TID_QUEST_PK_REWARD_PKVALUE ) );
			strReward += strTemp + "\n";
		}
#else // __VER >= 8 // __S8_PK
		if( pQuestProp->m_nEndRewardKarmaStyle )
		{
			if( pQuestProp->m_bEndRewardHide == FALSE || bComplete == TRUE )
			{
				if( pQuestProp->m_nEndRewardKarmaStyle == 1 )
					strTemp.Format( GETTEXT( TID_QUEST_MODIFY_KARMA1 ), pQuestProp->m_nEndRewardKarmaPoint );
				else
					strTemp.Format( GETTEXT( TID_QUEST_ADD_KARMA1 ), pQuestProp->m_nEndRewardKarmaPoint );
			}
			else
			{
				if( pQuestProp->m_nEndRewardKarmaStyle == 1 )
					strTemp.Format( GETTEXT( TID_QUEST_MODIFY_KARMA2 ) );
				else
					strTemp.Format( GETTEXT( TID_QUEST_ADD_KARMA2 ) );
			}
			strReward += strTemp + "\n";
		}
#endif // __VER < 8 // __S8_PK
		pTextReward->SetString( strReward );
	}
	else
	{
		pWndRemove->EnableWindow( FALSE );
		pTextDesc->SetString( "" );
		pTextCond->SetString( "" );
		pTextReward->SetString( "" );
	}
	pTextDesc->ResetString();
	pTextCond->ResetString();
	pTextReward->ResetString();

}
#endif // __IMPROVE_QUEST_INTERFACE
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
void CWndQuest::ControlOpenTree( const LPTREEELEM lpTreeElem )
{
	if( lpTreeElem->m_bOpen == TRUE )
		m_aOpenTree.Add( (WORD)( lpTreeElem->m_dwData ) );
	else
	{
		for( int i = 0; i < m_aOpenTree.GetSize(); ++i )
		{
			DWORD dwOpenTreeID = ( DWORD )m_aOpenTree.GetAt( i );
			if( lpTreeElem->m_dwData == dwOpenTreeID )
			{
				m_aOpenTree.RemoveAt( i );
				break;
			}
		}
	}
}
#endif // __IMPROVE_QUEST_INTERFACE
void CWndQuest::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	CWndTabCtrl* pWndTabCtrl = ( CWndTabCtrl* )GetDlgItem( WIDC_QUEST_LIST_TABCTRL );
	assert( pWndTabCtrl );

	pWndTabCtrl->SetTabTitleAlign( CWndTabCtrl::ALIGN_CENTER );

	m_WndScenario.Create( WBS_CHILD | WBS_NODRAWFRAME | WBS_VSCROLL, GetClientRect(), pWndTabCtrl, 11 );
	m_WndScenario.SetMemberCheckingMode( TRUE );
	m_WndScenario.SetMaxCheckNumber( MAX_CHECK_NUMBER );

	m_WndNormal.Create( WBS_CHILD | WBS_NODRAWFRAME | WBS_VSCROLL, GetClientRect(), pWndTabCtrl, 12 );
	m_WndNormal.SetMemberCheckingMode( TRUE );
	m_WndNormal.SetMaxCheckNumber( MAX_CHECK_NUMBER );

	m_WndRequest.Create( WBS_CHILD | WBS_NODRAWFRAME | WBS_VSCROLL, GetClientRect(), pWndTabCtrl, 13 );
	m_WndRequest.SetMemberCheckingMode( TRUE );
	m_WndRequest.SetMaxCheckNumber( MAX_CHECK_NUMBER );

	m_WndEvent.Create( WBS_CHILD | WBS_NODRAWFRAME | WBS_VSCROLL, GetClientRect(), pWndTabCtrl, 14 );
	m_WndEvent.SetMemberCheckingMode( TRUE );
	m_WndEvent.SetMaxCheckNumber( MAX_CHECK_NUMBER );

	QuestProp* pQuestProp = NULL;
	WTCITEM tabTabItem;
	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;
	pQuestProp = prj.m_aPropQuest.GetAt( QUEST_KIND_SCENARIO );
	tabTabItem.pszText = pQuestProp ? pQuestProp->m_szTitle : _T( "" );
	tabTabItem.pWndBase = &m_WndScenario;
	pWndTabCtrl->InsertItem( 0, &tabTabItem );

	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;
	pQuestProp = prj.m_aPropQuest.GetAt( QUEST_KIND_NORMAL );
	tabTabItem.pszText = pQuestProp ? pQuestProp->m_szTitle : _T( "" );
	tabTabItem.pWndBase = &m_WndNormal;
	pWndTabCtrl->InsertItem( 1, &tabTabItem );

	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;
	pQuestProp = prj.m_aPropQuest.GetAt( QUEST_KIND_REQUEST );
	tabTabItem.pszText = pQuestProp ? pQuestProp->m_szTitle : _T( "" );
	tabTabItem.pWndBase = &m_WndRequest;
	pWndTabCtrl->InsertItem( 2, &tabTabItem );

	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;
	pQuestProp = prj.m_aPropQuest.GetAt( QUEST_KIND_EVENT );
	tabTabItem.pszText = pQuestProp ? pQuestProp->m_szTitle : _T( "" );
	tabTabItem.pWndBase = &m_WndEvent;
	pWndTabCtrl->InsertItem( 3, &tabTabItem );

	pWndTabCtrl->SetCurSel( CTreeInformationManager::m_nSelectedTabNumber );

	CWndComboBox* pWndComboBox = ( CWndComboBox* )GetDlgItem( WIDC_COMBOBOX_QUEST_LIST );
	if( pWndComboBox )
	{
		pWndComboBox->AddWndStyle( EBS_READONLY );
		pWndComboBox->AddString( prj.GetText( TID_GAME_QUEST_ALL_LIST ) );
		pWndComboBox->AddString( prj.GetText( TID_GAME_QUEST_CURRENT_LIST ) );
		pWndComboBox->AddString( prj.GetText( TID_GAME_QUEST_COMPLETE_LIST ) );
		pWndComboBox->SetCurSel( CTreeInformationManager::m_eQuestListGroup );
	}
#endif // __IMPROVE_QUEST_INTERFACE

	Update();

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndQuest::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_QUEST_EX_LIST, 0, CPoint( 0, 0 ), pWndParent );
#else // __IMPROVE_QUEST_INTERFACE
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_QUEST, 0, CPoint( 0, 0 ), pWndParent );
#endif // __IMPROVE_QUEST_INTERFACE
} 

BOOL CWndQuest::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
void CWndQuest::SetWndRect( CRect rectWnd, BOOL bOnSize )
{
	CWndNeuz::SetWndRect( rectWnd, bOnSize );

	CWndQuestDetail* pWndQuestDetail = g_WndMng.m_pWndQuestDetail;
	if( pWndQuestDetail )
	{
		CRect rtCurrentWndQuestDetail = pWndQuestDetail->GetWndRect();
		CRect rtChangedWndQuestDetail;
		int nRevisedLeft = rectWnd.right - 2;
		pWndQuestDetail->Move( nRevisedLeft, rectWnd.top );
	}
}
#endif // __IMPROVE_QUEST_INTERFACE
void CWndQuest::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndQuest::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndQuest::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 

BOOL CWndQuest::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	if( message != WNM_SELCHANGE && message != WNM_DBLCLK && message != WNM_SELCANCEL && message != WNM_CLICKED )
		return FALSE;

	CWndTabCtrl* pWndTabCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_QUEST_LIST_TABCTRL );
	assert( pWndTabCtrl );
	CTreeInformationManager::m_nSelectedTabNumber = pWndTabCtrl->GetCurSel();
	LPWTCITEM lpItem = pWndTabCtrl->GetSelectedTab();
	assert( lpItem );
	CWndQuestTreeCtrl* pTreeCtrl = ( CWndQuestTreeCtrl* )lpItem->pWndBase;
	assert( pTreeCtrl );
	LPTREEELEM lpTreeElem = pTreeCtrl->GetCurSel();
	int nQuestID = 0;
	if( lpTreeElem )
	{
		m_idSelQuest = lpTreeElem->m_dwData;
		if( lpTreeElem->m_ptrArray.GetSize() == 0 )
			nQuestID = lpTreeElem->m_dwData;
	}

	switch( message )
	{
	case WNM_SELCHANGE:
		{
			switch( nID )
			{
			case WIDC_COMBOBOX_QUEST_LIST:
			case WIDC_QUEST_LIST_TABCTRL:
				{
					if( g_WndMng.m_pWndQuestDetail )
						SAFE_DELETE( g_WndMng.m_pWndQuestDetail )
						CWndComboBox* pWndComboBox = ( CWndComboBox* )GetDlgItem( WIDC_COMBOBOX_QUEST_LIST );
					if( pWndComboBox )
					{
						CTreeInformationManager::m_eQuestListGroup = static_cast< CTreeInformationManager::QuestListGroup >( pWndComboBox->GetCurSel() );
						Update();
					}
					break;
				}
			}
			break;
		}
	case WNM_DBLCLK:
		{
			if( lpTreeElem && lpTreeElem->m_ptrArray.GetSize() > 0 )
				ControlOpenTree( lpTreeElem );
			break;
		}
	case WNM_SELCANCEL:
	case WNM_CLICKED:
		{
			switch( nID )
			{
			case WIDC_REMOVE:
				{
					if( lpTreeElem && lpTreeElem->m_ptrArray.GetSize() == 0 )
					{
						if( g_WndMng.m_pWndQuestDetail )
							SAFE_DELETE( g_WndMng.m_pWndQuestDetail )
						CWndRemoveQuest* pWndRemoveQuest = new CWndRemoveQuest;
						pWndRemoveQuest->m_nRemoveQuestId = nQuestID;
						g_WndMng.OpenCustomBox( NULL, pWndRemoveQuest );
					}
					break;
				}
			case WIDC_CLOSE:
				{
					nID = WTBID_CLOSE;
					break;
				}
			case WIDC_BUTTON_CLOSE:
				{
					Destroy();
					break;
				}
			default:
				{
					if( lpTreeElem && lpTreeElem->m_ptrArray.GetSize() == 0 )
					{
						if( g_WndMng.m_pWndQuestDetail && nQuestID == g_WndMng.m_pWndQuestDetail->GetQuestID() )
							SAFE_DELETE( g_WndMng.m_pWndQuestDetail )
						else
						{
							SAFE_DELETE( g_WndMng.m_pWndQuestDetail )
								g_WndMng.m_pWndQuestDetail = new CWndQuestDetail( nQuestID );
							CWndQuestDetail* pWndQuestDetail = g_WndMng.m_pWndQuestDetail;
							pWndQuestDetail->Initialize( NULL );
							if( pWndQuestDetail )
								pWndQuestDetail->UpdateQuestText( TRUE );
							CWndQuestQuickInfo* pWndQuestQuickInfo = g_WndMng.m_pWndQuestQuickInfo;
							if( pWndQuestQuickInfo )
								pWndQuestQuickInfo->SetUpdateTextSwitch( TRUE );
						}
					}
					RemoveQuest();
				}
			}
			break;
		}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
#else // __IMPROVE_QUEST_INTERFACE
	int nId = 0;
	CWndTreeCtrl* pTreeCtrl = (CWndTreeCtrl*)GetDlgItem( WIDC_TREECTRL1 );
	LPTREEELEM lpTreeElem = pTreeCtrl->GetCurSel();
	if( lpTreeElem && lpTreeElem->m_lpParent )
		nId = lpTreeElem->m_dwData;
	if( lpTreeElem )
		m_idSelQuest = lpTreeElem->m_dwData;

	if( ( message == WNM_CLICKED || message == WNM_SELCANCEL ) ) //&& (nID == 100000 || nID == 1000001 || nID == 1000002 ) )
	{
#if __VER >= 13 // __QUEST_HELPER
		UpdateText(TRUE);
#else //__QUEST_HELPER
		UpdateText();
#endif //__QUEST_HELPER
		LPTREEELEM lpRoot = pTreeCtrl->GetRootElem();
		int nPos = 0;
		LPTREEELEM lpElem = NULL;
		m_aOpenTree.RemoveAll();
		do {
			lpElem = pTreeCtrl->GetNextElem( lpRoot, nPos );
			if( lpElem && lpElem->m_bOpen )
				m_aOpenTree.Add( lpElem->m_dwData );
		} while( lpElem );
	}

	if( message == WNM_CLICKED )
	{
		switch(nID)
		{
		case WIDC_REMOVE: // cancel
			{
				CWndRemoveQuest* pWndRemoveQuest = new CWndRemoveQuest;
				pWndRemoveQuest->m_nRemoveQuestId = nId;
				g_WndMng.OpenCustomBox( NULL, pWndRemoveQuest );
			}
			break;
		case WIDC_CLOSE:
			nID = WTBID_CLOSE;
			break;
		}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
#endif // __IMPROVE_QUEST_INTERFACE
} 

#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
CWndQuestTreeCtrl* CWndQuest::GetQuestTreeSelf( const DWORD dwQuestID )
{
	DWORD dwRootHeadQuestID = GetRootHeadQuest( dwQuestID );
	switch( dwRootHeadQuestID )
	{
	case QUEST_KIND_SCENARIO: // 시나리오 퀘스트
		{
			return &m_WndScenario;
		}
	case QUEST_KIND_NORMAL: // 일반 퀘스트
		{
			return &m_WndNormal;
		}
	case QUEST_KIND_REQUEST: // 의뢰소 퀘스트
		{
			return &m_WndRequest;
		}
	case QUEST_KIND_EVENT: // 이벤트 퀘스트
		{
			return &m_WndEvent;
		}
	default:
		{
			return &m_WndNormal;
		}
	}
}

void CWndQuest::AddOpenTree( CWordArray& raOpenTree, CPtrArray& rPtrArray )
{
	for( int i = 0; i < rPtrArray.GetSize(); ++i )
	{
		LPTREEELEM lpTreeElem = ( LPTREEELEM )rPtrArray.GetAt( i );
		if( lpTreeElem == NULL )
			return;

		if( lpTreeElem->m_ptrArray.GetSize() > 0 )
		{
			if( lpTreeElem->m_bOpen == TRUE )
				raOpenTree.Add( (WORD)( lpTreeElem->m_dwData ) );
			AddOpenTree( raOpenTree, lpTreeElem->m_ptrArray );
		}
	}
}

void CWndQuest::OpenTreeArray( CPtrArray& rPtrArray, BOOL bOpen )
{
	for( int i = 0; i < rPtrArray.GetSize(); ++i )
	{
		LPTREEELEM lpTreeElem = ( LPTREEELEM )rPtrArray.GetAt( i );
		if( lpTreeElem == NULL )
			return;

		if( lpTreeElem->m_ptrArray.GetSize() > 0 )
		{
			for( int i = 0; i < m_aOpenTree.GetSize(); ++i )
			{
				DWORD dwOpenTree = ( DWORD )m_aOpenTree.GetAt( i );
				if( lpTreeElem->m_dwData == dwOpenTree )
				{
					lpTreeElem->m_bOpen = bOpen;
					break;
				}
			}
			OpenTreeArray( lpTreeElem->m_ptrArray, bOpen );
		}
	}
}

void CWndQuest::InsertQuestItem( const DWORD dwQuestID, CDWordArray& raOldHeadQuestID, const BOOL bCompleteQuest, const int nNewQuestId )
{
	static CWndQuestTreeCtrl* pQuestTreeCtrl = NULL;

	QuestProp* pQuestProp = prj.m_aPropQuest.GetAt( dwQuestID );
 	if( pQuestProp == NULL )
		return;
	if( pQuestProp->m_nHeadQuest > 0 )
	{
		DWORD dwNowHeadQuestID = pQuestProp->m_nHeadQuest;
		CString strQuestTitle = pQuestProp->m_szTitle;
		if( dwNowHeadQuestID > 0 )
		{
			DWORD dwOldHeadQuestID = FindOldHeadQuest( raOldHeadQuestID, dwNowHeadQuestID );
			if( dwOldHeadQuestID == 0 )
				InsertQuestItem( dwNowHeadQuestID, raOldHeadQuestID, bCompleteQuest );
			else
				pQuestTreeCtrl = GetQuestTreeSelf( dwOldHeadQuestID );
			assert( pQuestTreeCtrl );
			LPTREEELEM lpTreeElem = NULL;
			if( dwNowHeadQuestID != QUEST_KIND_SCENARIO && 
				dwNowHeadQuestID != QUEST_KIND_NORMAL && 
				dwNowHeadQuestID != QUEST_KIND_REQUEST && 
				dwNowHeadQuestID != QUEST_KIND_EVENT )
				lpTreeElem = pQuestTreeCtrl->FindTreeElem( dwNowHeadQuestID );

			CString strFullQuestTitle = _T( "" );
			QUEST oCompleteQuest;
			LPQUEST lpQuest = g_pPlayer->GetQuest( dwQuestID );
			if( lpQuest == NULL )
			{
				if( g_pPlayer->IsCompleteQuest( dwQuestID ) )
				{
					g_pPlayer->MakeCompleteQuest( dwQuestID, &oCompleteQuest );
					lpQuest = &oCompleteQuest;
				}
			}
			if( lpQuest )
			{
				CString strState = _T( "" );
				if( g_Option.m_bOperator || g_pPlayer->IsAuthHigher( AUTH_GAMEMASTER ) )
					strState.Format( "(%d, %d)", lpQuest->m_nState, lpQuest->m_wId );
				strFullQuestTitle.Format( "%s%s", strQuestTitle, strState );
				if( bCompleteQuest )
					pQuestTreeCtrl->InsertItem( lpTreeElem, strFullQuestTitle, dwQuestID, TRUE )->m_dwColor = 0xffc0c0c0;
				else
				{
					if( g_QuestTreeInfoManager.GetTreeInformation( dwQuestID ) == NULL )
						g_QuestTreeInfoManager.InsertTreeInformation( dwQuestID, TRUE );
					pQuestTreeCtrl->InsertItem( lpTreeElem, strFullQuestTitle, dwQuestID, FALSE, IsCheckedQuestID( dwQuestID ) );
				}
			}
			else
			{
				strFullQuestTitle.Format( "%s", strQuestTitle );
				LPTREEELEM pFolderTreeElem = pQuestTreeCtrl->InsertItem( lpTreeElem, strFullQuestTitle, dwQuestID );
				if( nNewQuestId != -1 && prj.m_aPropQuest.GetAt( nNewQuestId )->m_nHeadQuest == dwQuestID )
				{
					pFolderTreeElem->m_bOpen = TRUE;
					m_aOpenTree.Add( (WORD)( dwQuestID ) );
				}
			}
			raOldHeadQuestID.Add( dwQuestID );
		}
	}
	else
	{
		pQuestTreeCtrl = GetQuestTreeSelf( dwQuestID );
		raOldHeadQuestID.Add( dwQuestID );
	}
}

DWORD CWndQuest::FindOldHeadQuest( const CDWordArray& raOldHeadQuestID, const DWORD dwNowHeadQuestID ) const
{
	for( int i = 0; i < raOldHeadQuestID.GetSize(); ++i )
	{
		DWORD dwOldHeadQuestID = raOldHeadQuestID.GetAt( i );
		if( dwNowHeadQuestID == dwOldHeadQuestID )
			return dwOldHeadQuestID;
	}
	return 0;
}

BOOL CWndQuest::IsCheckedQuestID( DWORD dwQuestID )
{
	for( int i = 0; i < g_pPlayer->m_nCheckedQuestSize; ++i )
	{
		DWORD dwCheckedQuestID = g_pPlayer->m_aCheckedQuest[ i ];
		if( dwCheckedQuestID == dwQuestID )
			return TRUE;
	}
	return FALSE;
}

CWndQuestTreeCtrl::CWndQuestTreeCtrl( void )
{
}

CWndQuestTreeCtrl::~CWndQuestTreeCtrl( void )
{
}

BOOL CWndQuestTreeCtrl::OnChildNotify( UINT nCode, UINT nID, LRESULT* pLResult )
{
	switch( nID )
	{
	case WIDC_CHECK:
		{
			for( int i = 0; i < GetTreeItemArray()->GetSize(); ++i )
			{
				LPTREEITEM pTreeItem = ( LPTREEITEM )GetTreeItemArray()->GetAt( i );
				if( GetMemberCheckingMode() == TRUE )
				{
					LPTREEELEM pTreeElem = pTreeItem->m_lpTreeElem;
					CWndButton* pWndCheckBox = pTreeElem->m_pWndCheckBox;
					DWORD dwQuestID = pTreeElem->m_dwData;
					if( pWndCheckBox && ( CWndButton* )pLResult == pWndCheckBox )
					{
						if( pWndCheckBox->GetCheck() == TRUE )
						{
							if( g_pPlayer->m_nCheckedQuestSize >= GetMaxCheckNumber() )
							{
								pWndCheckBox->SetCheck( FALSE );
								g_WndMng.PutString( prj.GetText( TID_GAME_ERROR_DONT_CHECK ), NULL, prj.GetTextColor( TID_GAME_ERROR_DONT_CHECK ) );
								break;
							}

							if( g_QuestTreeInfoManager.GetTreeInformation( dwQuestID ) == NULL )
								g_QuestTreeInfoManager.InsertTreeInformation( dwQuestID, TRUE );
							g_DPlay.SendCheckedQuestId( dwQuestID, TRUE );
						}
						else
						{
							g_QuestTreeInfoManager.DeleteTreeInformation( dwQuestID );
							g_DPlay.SendCheckedQuestId( dwQuestID, FALSE );
						}
					}
				}
			}
			break;
		}
	}
	return TRUE;
}

void CWndQuestTreeCtrl::OnLButtonDown( UINT nFlags, CPoint point )
{
	LPTREEITEM pTreeItem = NULL;
	for( int i = 0; i < GetTreeItemArray()->GetSize(); ++i )
	{
		pTreeItem = ( LPTREEITEM )GetTreeItemArray()->GetAt( i );
		if( GetMemberCheckingMode() == TRUE )
		{
			CWndButton* pWndCheckBox = pTreeItem->m_lpTreeElem->m_pWndCheckBox;
			LPTREEELEM pParentTreeElem = pTreeItem->m_lpTreeElem->m_lpParent;
			if( pWndCheckBox && pParentTreeElem && CheckParentTreeBeOpened( pParentTreeElem ) == FALSE )
			{
				if( pWndCheckBox->IsWindowEnabled() == TRUE )
					pWndCheckBox->EnableWindow( FALSE );
				if( pWndCheckBox->IsVisible() == TRUE )
					pWndCheckBox->SetVisible( FALSE );
			}
		}
		CRect rect = pTreeItem->m_rect;
		SIZE size = m_pTexButtOpen->m_size;
		rect.left -= GetCategoryTextSpace();
		rect.SetRect( rect.left, rect.top, rect.left + size.cx, rect.top + size.cy );
		if( rect.PtInRect( point ) )
		{
			LPTREEELEM lpTreeElem = pTreeItem->m_lpTreeElem;
			lpTreeElem->m_bOpen = !lpTreeElem->m_bOpen;
			SetFocusElem( lpTreeElem );

			CWndBase* pWndParent = m_pParentWnd;
			while( pWndParent->GetStyle() & WBS_CHILD )
				pWndParent = pWndParent->GetParentWnd();
			CWndQuest* pWndQuest = ( CWndQuest* )pWndParent;
			pWndQuest->ControlOpenTree( lpTreeElem );
		}
	}
}
#endif // __IMPROVE_QUEST_INTERFACE
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
//-----------------------------------------------------------------------------
CWndQConditionTreeCtrl::CWndQConditionTreeCtrl( void )
{
}
//-----------------------------------------------------------------------------
CWndQConditionTreeCtrl::~CWndQConditionTreeCtrl( void )
{
}
//-----------------------------------------------------------------------------
void CWndQConditionTreeCtrl::OnLButtonDown( UINT nFlags, CPoint point )
{
	LPTREEITEM pTreeItem = NULL;
	for( int i = 0; i < GetTreeItemArray()->GetSize(); ++i )
	{
		pTreeItem = ( LPTREEITEM )GetTreeItemArray()->GetAt( i );
		if( GetMemberCheckingMode() == TRUE )
		{
			CWndButton* pWndCheckBox = pTreeItem->m_lpTreeElem->m_pWndCheckBox;
			LPTREEELEM pParentTreeElem = pTreeItem->m_lpTreeElem->m_lpParent;
			if( pWndCheckBox && pParentTreeElem && pParentTreeElem->m_bOpen == FALSE )
			{
				if( pWndCheckBox->IsWindowEnabled() == TRUE )
					pWndCheckBox->EnableWindow( FALSE );
				if( pWndCheckBox->IsVisible() == TRUE )
					pWndCheckBox->SetVisible( FALSE );
			}
		}

		LPTREEELEM pTreeElem = pTreeItem->m_lpTreeElem;
		if( pTreeItem->m_rect.PtInRect( point ) )
		{
			DWORD dwGoalTextID = SetQuestDestinationInformation( ( ( CWndQuestDetail* )m_pParentWnd )->GetQuestID(), pTreeElem->m_dwData );
			ProcessQuestDestinationWorldMap( dwGoalTextID );
		}
	}
}
//-----------------------------------------------------------------------------
CWndQuestDetail::CWndQuestDetail( DWORD dwQuestID ) : 
m_dwQuestID( dwQuestID )
{
}
//-----------------------------------------------------------------------------
CWndQuestDetail::~CWndQuestDetail( void )
{
}
//-----------------------------------------------------------------------------
BOOL CWndQuestDetail::Initialize( CWndBase* pWndParent, DWORD nType )
{
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_QUEST_EX_DETAIL, 0, CPoint( 0, 0 ), pWndParent );
}
//-----------------------------------------------------------------------------
void CWndQuestDetail::OnInitialUpdate( void )
{
	CWndNeuz::OnInitialUpdate();

	LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_CUSTOM_CONDITION );
	lpWndCtrl->dwWndStyle |= WBS_VSCROLL;

	m_WndQConditionTreeCtrl.Create( lpWndCtrl->dwWndStyle, lpWndCtrl->rect, this, lpWndCtrl->dwWndId );
	m_WndQConditionTreeCtrl.SetCategoryTextSpace( 0 );

	CWndQuest* pWndQuest = ( CWndQuest* )g_WndMng.GetDlgItem( APP_QUEST_EX_LIST );
	if( pWndQuest )
	{
		CRect rtWndQuest = pWndQuest->GetWndRect();
		int nRevisedLeft = rtWndQuest.right - 2;
		Move( nRevisedLeft, rtWndQuest.top );
	}
	else
		MoveParentCenter();
}
//-----------------------------------------------------------------------------
BOOL CWndQuestDetail::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}
//-----------------------------------------------------------------------------
void CWndQuestDetail::SetWndRect( CRect rectWnd, BOOL bOnSize )
{
	CWndQuest* pWndQuest = ( CWndQuest* )g_WndMng.GetDlgItem( APP_QUEST_EX_LIST );
	if( pWndQuest )
		DelWndStyle( WBS_MOVE );
	else
		AddWndStyle( WBS_MOVE );
	CWndNeuz::SetWndRect( rectWnd, bOnSize );
}
//-----------------------------------------------------------------------------
void CWndQuestDetail::PaintFrame( C2DRender* p2DRender )
{
	CRect rect = GetWindowRect();
	if( m_pTexture )
	{
		RenderWnd();
		if( IsWndStyle( WBS_CAPTION ) )
		{
#ifdef __WINDOWS_V19
			CPoint newpoint;
			CSize sizeTitle = p2DRender->m_pFont->GetTextExtent( m_strTitle );
			CRect rect	= GetWindowRect();
			newpoint = (rect.Width() - sizeTitle.cx) / 2;

			CD3DFont* pOldFont = p2DRender->GetFont();
			p2DRender->SetFont( CWndBase::m_Theme.m_pFontWndNewTitle );
            p2DRender->TextOut( newpoint.x , 8, m_strTitle, 0xffffffff );
			p2DRender->SetFont( pOldFont );
#else
			CD3DFont* pOldFont = p2DRender->GetFont();
			p2DRender->SetFont( CWndBase::m_Theme.m_pFontWndTitle );
			p2DRender->TextOut( 10, 4, m_strTitle, m_dwColor );
			p2DRender->SetFont( pOldFont );
#endif//__WINDOWS_V19
		}
	}
	else if( m_strTexture.IsEmpty() )
	{
		m_pTheme->RenderWndBaseFrame( p2DRender, &rect );
		if( IsWndStyle( WBS_CAPTION ) )
		{
			rect.bottom = 21;
			m_pTheme->RenderWndBaseTitleBar( p2DRender, &rect, m_strTitle, m_dwColor );
		}
	}
}
//-----------------------------------------------------------------------------
void CWndQuestDetail::UpdateQuestText( BOOL bClick )
{
	if( m_dwQuestID == -1 )
		return;

	CWndText* pTextDesc = ( CWndText* )GetDlgItem( WIDC_TEXT_CONTENTS );
	CWndQConditionTreeCtrl* pWndQuestConditionTree = ( CWndQConditionTreeCtrl* )GetDlgItem( WIDC_CUSTOM_CONDITION );
	pWndQuestConditionTree->DeleteAllItems();
	CWndText* pTextReward = (CWndText*)GetDlgItem( WIDC_TEXT_REWARD );
	pTextReward->m_dwColor = 0xffffffff;

	LPQUEST lpQuest = g_pPlayer->FindQuest( m_dwQuestID );
	BOOL bComplete = m_dwQuestID != -1 && g_pPlayer->IsCompleteQuest( m_dwQuestID );

	if( lpQuest || bComplete )
		UpdateQuestDetailText( m_dwQuestID, lpQuest, bComplete, bClick );
	else
	{
		pTextDesc->SetString( _T( "" ) );
		pWndQuestConditionTree->DeleteAllItems();
		pTextReward->SetString( _T( "" ) );
	}
	pTextDesc->ResetString();
	pTextReward->ResetString();
}
//-----------------------------------------------------------------------------
void CWndQuestDetail::UpdateQuestDetailText( DWORD dwQuestID, LPQUEST lpQuest, BOOL bComplete, BOOL bClick )
{
	CWndText* pTextDesc = ( CWndText* )GetDlgItem( WIDC_TEXT_CONTENTS );
	CWndQConditionTreeCtrl* pWndQuestConditionTree = ( CWndQConditionTreeCtrl* )GetDlgItem( WIDC_CUSTOM_CONDITION );
	pWndQuestConditionTree->DeleteAllItems();
	CWndText* pTextReward = ( CWndText* )GetDlgItem( WIDC_TEXT_REWARD );
	pTextReward->m_dwColor = 0xffffffff;

	QuestProp* pQuestProp = prj.m_aPropQuest.GetAt( dwQuestID );

	// 디스크립션만 출력
	if( bComplete )
	{
		for( int i = QS_END; i >= 0; --i )
		{
			QuestState* pQuestState = pQuestProp->m_questState[ i ];
			if( pQuestState )
			{
				pTextDesc->SetString( pQuestState->m_szDesc, 0xffffffff );//ADEILSON 0xff000000
				break;
			}
		}
	}
	else
	{
		QuestState* pQuestState = pQuestProp->m_questState[ lpQuest->m_nState ];
		pTextDesc->SetString( pQuestState->m_szDesc, 0xffffffff );//ADEILSON 0xff000000
	}

	//////////////////////////////////////////////////////////////////////////////////
	// 퀘스트 종료 조건
	//////////////////////////////////////////////////////////////////////////////////
	CString strTemp = _T( "" );
#ifdef __BS_PUTNAME_QUESTARROW
	CWndWorld* pWndWorld = g_WndMng.m_pWndWorld;
	if( pWndWorld )
		pWndWorld->m_strDestName = _T( "" );
#endif // __BS_PUTNAME_QUESTARROW
	if( bComplete )
		pWndQuestConditionTree->InsertItem( NULL, GETTEXT( TID_QUEST_COMPLETED ), 0 );
	else
		MakeQuestConditionItems( dwQuestID, pWndQuestConditionTree, bClick );

	//////////////////////////////////////////////////////////////////////////////////
	// 보상 아이템 목록
	//////////////////////////////////////////////////////////////////////////////////
	CString strReward = _T( "" );
	for( int i = 0; i < pQuestProp->m_nEndRewardItemNum; i++ )
	{
		QuestPropItem* pEndRewardItem = &pQuestProp->m_paEndRewardItem[ i ];
		if( pEndRewardItem->m_nSex == -1 || pEndRewardItem->m_nSex == g_pPlayer->GetSex() )
		{
			if( pEndRewardItem->m_nType == 0 )
			{
				if( pEndRewardItem->m_nJobOrItem == -1 || pEndRewardItem->m_nJobOrItem == g_pPlayer->GetJob() )
				{
					if( pEndRewardItem->m_nItemIdx )
					{
						ItemProp* pItemProp = prj.GetItemProp( pEndRewardItem->m_nItemIdx );
						if( pQuestProp->m_bEndRewardHide == FALSE || bComplete == TRUE )
							strTemp.Format( GETTEXT( TID_QUEST_ITEM1 ), pItemProp->szName, pEndRewardItem->m_nItemNum );
						else
							strTemp.Format( GETTEXT( TID_QUEST_ITEM2 ) );
						strReward += strTemp + "\n";
					}
				}
			}
			else if( pEndRewardItem->m_nType == 1 )
			{
				if( pEndRewardItem->m_nJobOrItem == -1 || g_pPlayer->GetItemNum( pEndRewardItem->m_nJobOrItem ) ) 
				{
					if( pEndRewardItem->m_nItemIdx )
					{
						ItemProp* pItemProp = prj.GetItemProp( pEndRewardItem->m_nItemIdx );
						if( pQuestProp->m_bEndRewardHide == FALSE || bComplete == TRUE )
							strTemp.Format( GETTEXT( TID_QUEST_ITEM1 ), pItemProp->szName, pEndRewardItem->m_nItemNum );
						else
							strTemp.Format( GETTEXT( TID_QUEST_ITEM2 ) );
						strReward += strTemp + "\n";
					}
				}
			}
		}
	}
	if( pQuestProp->m_nEndRewardGoldMin )
	{
		if( pQuestProp->m_bEndRewardHide == FALSE || bComplete == TRUE )
		{
			if( ( pQuestProp->m_nEndRewardGoldMax - pQuestProp->m_nEndRewardGoldMin ) == 0 )
				strTemp.Format( GETTEXT( TID_QUEST_GOLD1 ), pQuestProp->m_nEndRewardGoldMin );
			else
				strTemp.Format( GETTEXT( TID_QUEST_GOLD2 ), pQuestProp->m_nEndRewardGoldMin, pQuestProp->m_nEndRewardGoldMax );
		}
		else
			strTemp.Format( GETTEXT( TID_QUEST_GOLD3 ) );
		strReward += strTemp + "\n";
	}
	if( pQuestProp->m_nEndRewardExpMin )
	{
		strTemp.Format( GETTEXT( TID_QUEST_EXP1 ) );
		strReward += strTemp + "\n";
	}
	if( pQuestProp->m_nEndRewardSkillPoint )
	{
		strTemp.Format( GETTEXT( TID_QUEST_SKILLPOINT ) );
		strReward += strTemp + "\n";
	}
#if __VER >= 8 // __S8_PK
	if( pQuestProp->m_nEndRewardPKValueMin )
	{
		strTemp.Format( GETTEXT( TID_QUEST_PK_REWARD_PKVALUE ) );
		strReward += strTemp + "\n";
	}
#else // __VER >= 8 // __S8_PK
	if( pQuestProp->m_nEndRewardKarmaStyle )
	{
		if( pQuestProp->m_bEndRewardHide == FALSE || bComplete == TRUE )
		{
			if( pQuestProp->m_nEndRewardKarmaStyle == 1 )
				strTemp.Format( GETTEXT( TID_QUEST_MODIFY_KARMA1 ), pQuestProp->m_nEndRewardKarmaPoint );
			else
				strTemp.Format( GETTEXT( TID_QUEST_ADD_KARMA1 ), pQuestProp->m_nEndRewardKarmaPoint );
		}
		else
		{
			if( pQuestProp->m_nEndRewardKarmaStyle == 1 )
				strTemp.Format( GETTEXT( TID_QUEST_MODIFY_KARMA2 ) );
			else
				strTemp.Format( GETTEXT( TID_QUEST_ADD_KARMA2 ) );
		}
		strReward += strTemp + "\n";
	}
#endif // __VER < 8 // __S8_PK
	pTextReward->SetString( strReward );
}
//-----------------------------------------------------------------------------
void CWndQuestDetail::SetQuestID( DWORD dwQuestID )
{
	m_dwQuestID = dwQuestID;
}
//-----------------------------------------------------------------------------
DWORD CWndQuestDetail::GetQuestID( void ) const
{
	return m_dwQuestID;
}
//-----------------------------------------------------------------------------
void MakeQuestConditionItems( DWORD dwQuestID, CWndTreeCtrl* pWndTreeCtrl, BOOL bClick, LPTREEELEM lpTreeElem, DWORD dwStartColor, DWORD dwEndColor, DWORD dwSelectColor )
{
	CString strTemp = _T( "" );
	LPQUEST lpQuest = g_pPlayer->FindQuest( dwQuestID );
	QuestProp* pQuestProp = prj.m_aPropQuest.GetAt( dwQuestID );

	for( int i = 0; i < 14; i++ )
	{
		if( pQuestProp->m_questState[ i ] == NULL )
			break;
	}
	if( lpQuest->m_nState == ( i - 1 ) || lpQuest->m_nState == 14 )
	{
		int nNum;
		if( pQuestProp->m_nEndCondLimitTime )
		{
			if( lpQuest->m_wTime )
			{
				strTemp.Format( GETTEXT( TID_QUEST_LIMIT_TIME ), lpQuest->m_wTime & 0x7fff );
				pWndTreeCtrl->InsertItem( lpTreeElem, strTemp, 0, TRUE, FALSE, dwEndColor, dwSelectColor );
			}
			else
			{
				strTemp.Format( GETTEXT( TID_QUEST_LIMIT_TIMEOUT ) );
				pWndTreeCtrl->InsertItem( lpTreeElem, "#cffff0000" + strTemp + "#ns#nc", 0, TRUE, FALSE, dwStartColor, dwSelectColor );
			}
		}
		for( int i = 0 ; i < MAX_QUEST_COND_KILL; i++ )
		{
			if( pQuestProp->m_nEndCondKillNPCIdx[ i ] )
			{
				MoverProp* pMoverProp = prj.GetMoverProp( pQuestProp->m_nEndCondKillNPCIdx[ i ] );
				nNum = lpQuest->m_nKillNPCNum[ i ];
				if( nNum > pQuestProp->m_nEndCondKillNPCNum[ i ] )
					nNum = pQuestProp->m_nEndCondKillNPCNum[ i ];
				strTemp.Format( GETTEXT( TID_QUEST_KILL_NPC ), pMoverProp->szName, lpQuest->m_nKillNPCNum[ i ], pQuestProp->m_nEndCondKillNPCNum[ i ] );

				DWORD dwCompleteColor = MakeTextColor( dwStartColor, dwEndColor, lpQuest->m_nKillNPCNum[ i ], pQuestProp->m_nEndCondKillNPCNum[ i ] );
				pWndTreeCtrl->InsertItem( lpTreeElem, MakeString( strTemp, nNum == pQuestProp->m_nEndCondKillNPCNum[ i ] ), pQuestProp->m_KillNPCGoalData[ i ].m_dwGoalIndex, TRUE, FALSE, dwCompleteColor, dwSelectColor );
			}
		}
		for( i = 0; i < pQuestProp->m_nEndCondItemNum; i++ )
		{
			QuestPropItem* pEndCondItem = &pQuestProp->m_paEndCondItem[ i ];

			if( pEndCondItem->m_nSex == -1 || pEndCondItem->m_nSex == g_pPlayer->GetSex() )
			{
				if( pEndCondItem->m_nType == 0 )
				{
					if( pEndCondItem->m_nJobOrItem == -1 || pEndCondItem->m_nJobOrItem == g_pPlayer->GetJob() )
					{
						if( pEndCondItem->m_nItemIdx )
						{
							ItemProp* pItemProp = prj.GetItemProp( pEndCondItem->m_nItemIdx );
							nNum = g_pPlayer->GetItemNum( pEndCondItem->m_nItemIdx );
							strTemp.Format( GETTEXT( TID_QUEST_ITEM ), pItemProp->szName, nNum, pEndCondItem->m_nItemNum );

							DWORD dwCompleteColor = MakeTextColor( dwStartColor, dwEndColor, nNum, pEndCondItem->m_nItemNum );
							pWndTreeCtrl->InsertItem( lpTreeElem, MakeString( strTemp, nNum >= pEndCondItem->m_nItemNum ), pEndCondItem->m_ItemGoalData.m_dwGoalIndex, TRUE, FALSE, dwCompleteColor, dwSelectColor );
						}
					}
				}
				else if( pEndCondItem->m_nType == 1 )
				{
					if( pEndCondItem->m_nJobOrItem == -1 || g_pPlayer->GetItemNum( pEndCondItem->m_nJobOrItem ) )
					{
						if( pEndCondItem->m_nItemIdx )
						{
							ItemProp* pItemProp = prj.GetItemProp( pEndCondItem->m_nItemIdx );
							nNum = g_pPlayer->GetItemNum( pEndCondItem->m_nItemIdx );
							strTemp.Format( GETTEXT( TID_QUEST_ITEM ), pItemProp->szName, nNum, pEndCondItem->m_nItemNum );

							DWORD dwCompleteColor = MakeTextColor( dwStartColor, dwEndColor, nNum, pEndCondItem->m_nItemNum );
							pWndTreeCtrl->InsertItem( lpTreeElem, MakeString( strTemp, nNum >= pEndCondItem->m_nItemNum ), pEndCondItem->m_ItemGoalData.m_dwGoalIndex, TRUE, FALSE, dwCompleteColor, dwSelectColor );
						}
					}
				}
			}
		}
		if( pQuestProp->m_dwEndCondPatrolWorld )
		{
			strTemp.Format( GETTEXT( TID_QUEST_PATROL ), prj.GetPatrolDestination( pQuestProp->m_dwPatrolDestinationID ) );
			DWORD dwCompleteColor = 0;
			if( lpQuest->m_bPatrol )
			{
				strTemp += _T( " " );
				strTemp += prj.GetText( TID_GAME_QUEST_COMPLETE_TEXT );
				dwCompleteColor = dwEndColor;
			}
			else
				dwCompleteColor = dwStartColor;
			pWndTreeCtrl->InsertItem( lpTreeElem, MakeString( strTemp, lpQuest->m_bPatrol ), pQuestProp->m_PatrolWorldGoalData.m_dwGoalIndex, TRUE, FALSE, dwCompleteColor, dwSelectColor );
		}
		if( pQuestProp->m_nEndCondDisguiseMoverIndex )
		{
			if( pQuestProp->m_nEndCondDisguiseMoverIndex == -1 )
			{
				strTemp.Format( GETTEXT( TID_QUEST_NODISQUISE ) );
				DWORD dwCompleteColor = 0;
				if( g_pPlayer->IsDisguise() == FALSE )
					dwCompleteColor = dwEndColor;
				else
					dwCompleteColor = dwStartColor;
				pWndTreeCtrl->InsertItem( lpTreeElem, MakeString( strTemp, g_pPlayer->IsDisguise() == FALSE ), 0, TRUE, FALSE, dwCompleteColor, dwSelectColor );
			}
			else
			{
				MoverProp* pMoverProp = prj.GetMoverProp( pQuestProp->m_nEndCondDisguiseMoverIndex );
				strTemp.Format( GETTEXT( TID_QUEST_DISQUISE ), pMoverProp->szName );
				DWORD dwCompleteColor = 0;
				if( g_pPlayer->IsDisguise() )
					dwCompleteColor = dwEndColor;
				else
					dwCompleteColor = dwStartColor;
				pWndTreeCtrl->InsertItem( lpTreeElem, MakeString( strTemp, g_pPlayer->IsDisguise() ), 0, TRUE, FALSE, dwCompleteColor, dwSelectColor );
			}
		}
		if( pQuestProp->m_nEndCondSkillIdx )
		{
			ItemProp* pSkillProp = prj.GetSkillProp( pQuestProp->m_nEndCondSkillIdx );
			LPSKILL lpSkill = g_pPlayer->GetSkill( pQuestProp->m_nEndCondSkillIdx );
			if( g_pPlayer->CheckSkill( pQuestProp->m_nEndCondSkillIdx ) && lpSkill )
			{
				strTemp.Format( GETTEXT( TID_QUEST_SKILL_LVL ), pSkillProp->szName, lpSkill->dwLevel, pQuestProp->m_nEndCondSkillLvl );
				DWORD dwCompleteColor = MakeTextColor( dwStartColor, dwEndColor, lpSkill->dwLevel, pQuestProp->m_nEndCondSkillLvl );
				pWndTreeCtrl->InsertItem( lpTreeElem, MakeString( strTemp, (int)( lpSkill->dwLevel ) >= pQuestProp->m_nEndCondSkillLvl ), 0, TRUE, FALSE, dwCompleteColor, dwSelectColor );
			}
			else
			{
				strTemp.Format( GETTEXT( TID_QUEST_SKILL_LVL ), pSkillProp->szName, 0, pQuestProp->m_nEndCondSkillLvl );
				DWORD dwCompleteColor = MakeTextColor( dwStartColor, dwEndColor, 0, pQuestProp->m_nEndCondSkillLvl );
				pWndTreeCtrl->InsertItem( lpTreeElem, strTemp, 0, TRUE, FALSE, dwCompleteColor, dwSelectColor );
			}
		}
#if __VER < 8 // __S8_PK
		if( pQuestProp->m_nEndCondKarmaPoint ) 
		{
			if( pQuestProp->m_nEndCondKarmaComp == 0 )
			{
				strTemp.Format( GETTEXT( TID_QUEST_KARMA_POINT ), g_pPlayer->GetKarma(),"=",pQuestProp->m_nEndCondKarmaPoint );
				DWORD dwCompleteColor = MakeTextColor( dwStartColor, dwEndColor, g_pPlayer->GetKarma(), pQuestProp->m_nEndCondKarmaPoint );
				pWndTreeCtrl->InsertItem( lpTreeElem, MakeString( strTemp, g_pPlayer->GetKarma() == pQuestProp->m_nEndCondKarmaPoint ), 0, TRUE, FALSE, dwCompleteColor, dwSelectColor );
			}
			else if( pQuestProp->m_nEndCondKarmaComp == -1 )
			{
				strTemp.Format( GETTEXT( TID_QUEST_KARMA_POINT ), g_pPlayer->GetKarma(),"<=",pQuestProp->m_nEndCondKarmaPoint );
				DWORD dwCompleteColor = MakeTextColor( dwStartColor, dwEndColor, g_pPlayer->GetKarma(), pQuestProp->m_nEndCondKarmaPoint );
				pWndTreeCtrl->InsertItem( lpTreeElem, MakeString( strTemp, g_pPlayer->GetKarma() <= pQuestProp->m_nEndCondKarmaPoint ), 0, TRUE, FALSE, dwCompleteColor, dwSelectColor );
			}
			else if( pQuestProp->m_nEndCondKarmaComp == 1)
			{
				strTemp.Format( GETTEXT( TID_QUEST_KARMA_POINT ), g_pPlayer->GetKarma(),">=",pQuestProp->m_nEndCondKarmaPoint );
				DWORD dwCompleteColor = MakeTextColor( dwStartColor, dwEndColor, g_pPlayer->GetKarma(), pQuestProp->m_nEndCondKarmaPoint );
				pWndTreeCtrl->InsertItem( lpTreeElem, MakeString( strTemp, g_pPlayer->GetKarma() >= pQuestProp->m_nEndCondKarmaPoint ), 0, TRUE, FALSE, dwCompleteColor, dwSelectColor );
			}
		}
#endif // __VER < 8 // __S8_PK
		// 파티 체크
		if( pQuestProp->m_nEndCondParty != 0 )
		{
			BOOL bLeader = FALSE;
			BOOL bParty  = g_Party.IsMember( g_pPlayer->m_idPlayer );
			int nSize = g_Party.GetSizeofMember();

			if( pQuestProp->m_nEndCondParty == 1 ) // 싱글이어야 함
			{
				strTemp.Format( GETTEXT( TID_QUEST_PARTY_SOLO ) );
				DWORD dwCompleteColor = 0;
				if( !bParty )
					dwCompleteColor = dwEndColor;
				else
					dwCompleteColor = dwStartColor;
				pWndTreeCtrl->InsertItem( lpTreeElem, MakeString( strTemp, !bParty ), 0, TRUE, FALSE, dwCompleteColor, dwSelectColor );
			}
			else if( pQuestProp->m_nEndCondParty == 2 ) // 파티여야 함
			{
				int nTextId = 0;
				BOOL bResult = FALSE;

				if( pQuestProp->m_nEndCondPartyNum == 0 )
				{
					if( pQuestProp->m_nEndCondPartyLeader == 0 ) // 파티 여부 (0 == 0 )
						nTextId = TID_QUEST_PARTY;
					else if( pQuestProp->m_nEndCondPartyLeader == 1 ) // 파티, 맴버 여부 (0 == 0 )
						nTextId = TID_QUEST_PARTY_MEMBER;
					else if( pQuestProp->m_nEndCondPartyLeader == 2 ) // 파티, 리더 여부 (0 == 0 )
						nTextId = TID_QUEST_PARTY_LEADER;

					strTemp.Format( GETTEXT( nTextId ) );

					if( pQuestProp->m_nEndCondPartyLeader == 0 ) // 파티 여부 
						bResult = TRUE;
					else if( ( pQuestProp->m_nEndCondPartyLeader - 1 ) == bLeader ) // 파티, 맴버 여부
						bResult = TRUE;
				}
				else
				{
					if( pQuestProp->m_nEndCondPartyLeader == 0 ) // 파티 여부 (0 == 0 )
						nTextId = TID_QUEST_PARTY_NUM;
					else if( pQuestProp->m_nEndCondPartyLeader == 1 ) // 파티, 맴버 여부 (0 == 0 )
						nTextId = TID_QUEST_PARTY_MEMBER_NUM;
					else if( pQuestProp->m_nEndCondPartyLeader == 2 ) // 파티, 리더 여부 (0 == 0 )
						nTextId = TID_QUEST_PARTY_LEADER_NUM;

					if( pQuestProp->m_nEndCondPartyNumComp == 0 ) 
					{
						strTemp.Format( GETTEXT( nTextId ), nSize,"=",pQuestProp->m_nEndCondPartyNum );
						if( nSize == pQuestProp->m_nEndCondPartyNum )
							bResult = TRUE;
					}
					else if( pQuestProp->m_nEndCondPartyNumComp == -1 )
					{
						strTemp.Format( GETTEXT( nTextId ), nSize,"<=",pQuestProp->m_nEndCondPartyNum );
						if( nSize <= pQuestProp->m_nEndCondPartyNum )
							bResult = TRUE;
					}
					else if( pQuestProp->m_nEndCondPartyNumComp == 1)
					{
						strTemp.Format( GETTEXT( nTextId ), nSize,">=",pQuestProp->m_nEndCondPartyNum );
						if( nSize >= pQuestProp->m_nEndCondPartyNum )
							bResult = TRUE;
					}
				}
				DWORD dwCompleteColor = 0;
				if( bResult && bParty )
					dwCompleteColor = dwEndColor;
				else
					dwCompleteColor = dwStartColor;
				pWndTreeCtrl->InsertItem( lpTreeElem, MakeString( strTemp, bResult && bParty ), 0, TRUE, FALSE, dwCompleteColor, dwSelectColor );
			}
		}
		// 길드 체크
		if( pQuestProp->m_nEndCondGuild != 0 ) 
		{
			BOOL bLeader = FALSE;
			BOOL bGuild  = g_Party.IsMember( g_pPlayer->m_idPlayer );
			int nSize = g_Party.GetSizeofMember();

			if( pQuestProp->m_nEndCondGuild == 1 ) // 길드가 아니어야 됨
			{
				strTemp.Format( GETTEXT( TID_QUEST_GUILD_SOLO ) );
				DWORD dwCompleteColor = 0;
				if( !bGuild )
					dwCompleteColor = dwEndColor;
				else
					dwCompleteColor = dwStartColor;
				pWndTreeCtrl->InsertItem( lpTreeElem, MakeString( strTemp, !bGuild ), 0, TRUE, FALSE, dwCompleteColor, dwSelectColor );
			}
			else if( pQuestProp->m_nEndCondGuild == 2 ) // 길드여야 됨
			{
				int nTextId = 0;
				BOOL bResult = FALSE;

				if( pQuestProp->m_nEndCondGuildNum == 0 ) 
				{
					if( pQuestProp->m_nEndCondGuildLeader == 0 ) // 파티 여부 (0 == 0 )
						nTextId = TID_QUEST_GUILD;
					else if( pQuestProp->m_nEndCondGuildLeader == 1 ) // 파티, 맴버 여부 (0 == 0 )
						nTextId = TID_QUEST_GUILD_MEMBER;
					else if( pQuestProp->m_nEndCondGuildLeader == 2 ) // 파티, 리더 여부 (0 == 0 )
						nTextId = TID_QUEST_GUILD_LEADER;

					strTemp.Format( GETTEXT( nTextId ) );

					if( pQuestProp->m_nEndCondGuildLeader == 0 ) // 길드 여부
						bResult = TRUE;
					else if( ( pQuestProp->m_nEndCondGuildLeader - 1 ) == bLeader ) // 길드, 리더 여부
						bResult = TRUE;
				}
				else
				{
					if( pQuestProp->m_nEndCondGuildLeader == 0 ) // 길드 여부 (0 == 0 )
						nTextId = TID_QUEST_GUILD_NUM;
					else if( pQuestProp->m_nEndCondGuildLeader == 1 ) // 길드, 맴버 여부 (0 == 0 )
						nTextId = TID_QUEST_GUILD_MEMBER_NUM;
					else if( pQuestProp->m_nEndCondGuildLeader == 2 ) // 길드, 리더 여부 (0 == 0 )
						nTextId = TID_QUEST_GUILD_LEADER_NUM;

					if( pQuestProp->m_nEndCondGuildNumComp == 0 )
					{
						strTemp.Format( GETTEXT( nTextId ), nSize,"=",pQuestProp->m_nEndCondGuildNum );
						if( nSize == pQuestProp->m_nEndCondGuildNum )
							bResult = TRUE;
					}
					else if( pQuestProp->m_nEndCondGuildNumComp == -1 )
					{
						strTemp.Format( GETTEXT( nTextId ), nSize,"<=",pQuestProp->m_nEndCondGuildNum );
						if( nSize <= pQuestProp->m_nEndCondGuildNum )
							bResult = TRUE;
					}
					else if( pQuestProp->m_nEndCondGuildNumComp == 1)
					{
						strTemp.Format( GETTEXT( nTextId ), nSize,">=",pQuestProp->m_nEndCondGuildNum );
						if( nSize >= pQuestProp->m_nEndCondGuildNum )
							bResult = TRUE;
					}
				}
				DWORD dwCompleteColor = 0;
				if( bResult && bGuild )
					dwCompleteColor = dwEndColor;
				else
					dwCompleteColor = dwStartColor;
				pWndTreeCtrl->InsertItem( lpTreeElem, MakeString( strTemp, bResult && bGuild ), 0, TRUE, FALSE, dwCompleteColor, dwSelectColor );
			}
		}
		//////////////////////////////////////////////////////
#if __VER >= 8 // __S8_PK
		CString strEndCondOneItem;
		CString strEndCondOneItemComplete;
		for( i = 0; i < pQuestProp->m_nEndCondOneItemNum; i++ )
		{
			QuestPropItem* pEndCondOneItem = &pQuestProp->m_paEndCondOneItem[ i ];

			if( pEndCondOneItem->m_nSex == -1 || pEndCondOneItem->m_nSex == g_pPlayer->GetSex() )
			{
				if( pEndCondOneItem->m_nType == 0 )
				{
					if( pEndCondOneItem->m_nJobOrItem == -1 || pEndCondOneItem->m_nJobOrItem == g_pPlayer->GetJob() )
					{
						if( pEndCondOneItem->m_nItemIdx )
						{
							ItemProp* pItemProp = prj.GetItemProp( pEndCondOneItem->m_nItemIdx );
							nNum = g_pPlayer->GetItemNum( pEndCondOneItem->m_nItemIdx );
							if( nNum >= pEndCondOneItem->m_nItemNum )
							{
								strEndCondOneItemComplete = "#s#cffa0a0a0";
								break;
							}
						}
					}
				}
				else if( pEndCondOneItem->m_nType == 1 )
				{
					if( pEndCondOneItem->m_nJobOrItem == -1 || g_pPlayer->GetItemNum( pEndCondOneItem->m_nJobOrItem ) )
					{
						if( pEndCondOneItem->m_nItemIdx )
						{
							ItemProp* pItemProp = prj.GetItemProp( pEndCondOneItem->m_nItemIdx );
							nNum = g_pPlayer->GetItemNum( pEndCondOneItem->m_nItemIdx );
							if( nNum >= pEndCondOneItem->m_nItemNum )
							{
								strEndCondOneItemComplete = "#s#cffa0a0a0";
								break;
							}
						}
					}
				}
			}
		}
		for( i = 0; i < pQuestProp->m_nEndCondOneItemNum; i++ )
		{
			QuestPropItem* pEndCondOneItem = &pQuestProp->m_paEndCondOneItem[ i ];

			if( pEndCondOneItem->m_nSex == -1 || pEndCondOneItem->m_nSex == g_pPlayer->GetSex() )
			{
				if( pEndCondOneItem->m_nType == 0 )
				{
					if( pEndCondOneItem->m_nJobOrItem == -1 || pEndCondOneItem->m_nJobOrItem == g_pPlayer->GetJob() )
					{
						if( pEndCondOneItem->m_nItemIdx )
						{
							ItemProp* pItemProp = prj.GetItemProp( pEndCondOneItem->m_nItemIdx );
							nNum = g_pPlayer->GetItemNum( pEndCondOneItem->m_nItemIdx );
							strTemp.Format( GETTEXT( TID_QUEST_ITEM ), pItemProp->szName, nNum, pEndCondOneItem->m_nItemNum );

							strEndCondOneItem += strEndCondOneItemComplete + "-" + strTemp + "#ns#nc";
						}
					}
				}
				else if( pEndCondOneItem->m_nType == 1 )
				{
					if( pEndCondOneItem->m_nJobOrItem == -1 || g_pPlayer->GetItemNum( pEndCondOneItem->m_nJobOrItem ) )
					{
						if( pEndCondOneItem->m_nItemIdx )
						{
							ItemProp* pItemProp = prj.GetItemProp( pEndCondOneItem->m_nItemIdx );
							nNum = g_pPlayer->GetItemNum( pEndCondOneItem->m_nItemIdx );
							strTemp.Format( GETTEXT( TID_QUEST_ITEM ), pItemProp->szName, nNum, pEndCondOneItem->m_nItemNum );

							strEndCondOneItem += strEndCondOneItemComplete + "-" + strTemp + "#ns#nc";
						}
					}
				}
			}
		}
		if( 0 < strEndCondOneItem.GetLength() )
		{
			strTemp.Format( GETTEXT( TID_QUEST_COND_ITEM ) );
			pWndTreeCtrl->InsertItem( lpTreeElem, strEndCondOneItemComplete + strTemp + "#ns#nc" + strEndCondOneItem, 0, TRUE, FALSE, dwEndColor, dwSelectColor );
		}
#else // __VER >= 8 // __S8_PK
		if( pQuestProp->m_nEndCondChaotic )
		{
			if( pQuestProp->m_nEndCondChaotic == 1 )
			{
				strTemp.Format( GETTEXT( TID_QUEST_CAOTIC1 ) );
				pWndTreeCtrl->InsertItem( lpTreeElem, MakeString( strTemp, g_pPlayer->IsChaotic() ), 0, TRUE, FALSE, dwStartColor, dwSelectColor );
			}
			else if( pQuestProp->m_nEndCondChaotic == 2 )
			{
				strTemp.Format( GETTEXT( TID_QUEST_CAOTIC2 ) );
				pWndTreeCtrl->InsertItem( lpTreeElem, MakeString( strTemp, g_pPlayer->IsChaotic() == FALSE ), 0, TRUE, FALSE, dwEndColor, dwSelectColor );
			}
		}
#endif // __VER < 8 // __S8_PK
		if( pQuestProp->m_szEndCondDlgCharKey[ 0 ] )
		{
			LPCHARACTER lpCharacter = prj.GetCharacter( pQuestProp->m_szEndCondDlgCharKey );
			strTemp.Format( GETTEXT( TID_QUEST_DIALOG ), lpCharacter->m_strName );
			pWndTreeCtrl->InsertItem( lpTreeElem, MakeString( strTemp, lpQuest->m_bDialog ), pQuestProp->m_DialogCharacterGoalData.m_dwGoalIndex, TRUE, FALSE, dwEndColor, dwSelectColor );
		}
		if( pQuestProp->m_szEndCondCharacter[ 0 ] )
		{
			LPCHARACTER lpCharacter = prj.GetCharacter( pQuestProp->m_szEndCondCharacter );
			strTemp.Format( GETTEXT( TID_QUEST_DESTINATION ), lpCharacter->m_strName );
			pWndTreeCtrl->InsertItem( lpTreeElem, strTemp, pQuestProp->m_MeetCharacterGoalData.m_dwGoalIndex, TRUE, FALSE, D3DCOLOR_XRGB( 255,  66,   0 )/*dwEndColor*/, dwSelectColor );
#if __VER >= 13 // __QUEST_HELPER
			if( bClick ) // 클릭해서 업데이트되는 경우에만 좌표를 요청한다
			{
				CWndWorld* pWndWorld = g_WndMng.m_pWndWorld;
				if( pWndWorld )
				{
					CWorld* pWorld = g_WorldMng();
					if( pWorld )
						pWorld->SetObjFocus(NULL);

					pWndWorld->m_bSetQuestNPCDest = FALSE;
					pWndWorld->m_pNextTargetObj = NULL;
#ifdef __BS_PUTNAME_QUESTARROW
					pWndWorld->m_strDestName = lpCharacter->m_strName;
#endif // __BS_PUTNAME_QUESTARROW
				}
#if __VER < 15 // __IMPROVE_QUEST_INTERFACE
				if( GetRootHeadQuest( pQuestProp->m_nHeadQuest ) != QUEST_KIND_EVENT )
					g_DPlay.SendReqNPCPos( lpCharacter->m_szKey );
#endif // __IMPROVE_QUEST_INTERFACE
			}
#endif //__QUEST_HELPER
		}
		else if( pQuestProp->m_lpszEndCondMultiCharacter )
		{
			for( int i = 0; i < 10; i++ )
			{
				CHAR* lpszChar = &pQuestProp->m_lpszEndCondMultiCharacter[ i * 64 ];
				if( lpszChar[ 0 ] )
				{
					LPCHARACTER lpCharacter = prj.GetCharacter( lpszChar );
					if( lpCharacter )
					{
						for( int j = 0; j < lpCharacter->m_anDstQuestItem.GetSize(); j++ )
						{
							if( g_pPlayer->GetItemNum( lpCharacter->m_anDstQuestItem.GetAt( j ) ) )
							{
								strTemp.Format( GETTEXT( TID_QUEST_DESTINATION ), lpCharacter->m_strName );
								i = 10;
								pWndTreeCtrl->InsertItem( lpTreeElem, strTemp, 0, TRUE, FALSE, D3DCOLOR_XRGB( 255,  66,   0 )/*dwEndColor*/, dwSelectColor );
								break;
							}
						}
					}
				}
			}
		}
	}
}
//-----------------------------------------------------------------------------
const CString MakeString( const CString& string, BOOL bCond )
{
	return CString( ( bCond ) ? "#cffa0a0a0" + string + "#nc" : string );
}
//-----------------------------------------------------------------------------
DWORD MakeTextColor( DWORD dwStartColor, DWORD dwEndColor, int nCurrentNumber, int nCompleteNumber )
{
	if( nCompleteNumber == 0 )
		return 0;

	int nStartColorRed = ( dwStartColor & 0x00ff0000 ) >> 16;
	int nStartColorGreen = ( dwStartColor & 0x0000ff00 ) >> 8;
	int nStartColorBlue = ( dwStartColor & 0x000000ff );
	int nEndColorRed = ( dwEndColor & 0x00ff0000 ) >> 16;
	int nEndColorGreen = ( dwEndColor & 0x0000ff00 ) >> 8;
	int nEndColorBlue = ( dwEndColor & 0x000000ff );

	int nDifferenceRed = abs( nEndColorRed - nStartColorRed );
	int nDifferenceGreen = abs( nEndColorGreen - nStartColorGreen );
	int nDifferenceBlue = abs( nEndColorBlue - nStartColorBlue );

	FLOAT fItemGettingRate = static_cast<FLOAT>( nCurrentNumber ) / static_cast<FLOAT>( nCompleteNumber );
	DWORD dwCompleteRedRate = static_cast<DWORD>( fItemGettingRate * static_cast<FLOAT>( nDifferenceRed ) );
	if( (int)( dwCompleteRedRate ) > nDifferenceRed )
		dwCompleteRedRate = nDifferenceRed;
	if( dwCompleteRedRate < 0 )
		dwCompleteRedRate = 0;
	DWORD dwCompleteGreenRate = static_cast<DWORD>( fItemGettingRate * static_cast<FLOAT>( nDifferenceGreen ) );
	if( (int)( dwCompleteGreenRate ) > nDifferenceGreen )
		dwCompleteGreenRate = nDifferenceGreen;
	if( dwCompleteGreenRate < 0 )
		dwCompleteGreenRate = 0;
	DWORD dwCompleteBlueRate = static_cast<DWORD>( fItemGettingRate * static_cast<FLOAT>( nDifferenceBlue ) );
	if( (int)( dwCompleteBlueRate ) > nDifferenceBlue )
		dwCompleteBlueRate = nDifferenceBlue;
	if( dwCompleteBlueRate < 0 )
		dwCompleteBlueRate = 0;

	DWORD dwCompleteRed = 0;
	if( nEndColorRed - nStartColorRed > 0 )
		dwCompleteRed = nStartColorRed + dwCompleteRedRate;
	else if( nEndColorRed - nStartColorRed < 0 )
		dwCompleteRed = nStartColorRed - dwCompleteRedRate;
	else
		dwCompleteRed = nStartColorRed;

	DWORD dwCompleteGreen = 0;
	if( nEndColorGreen - nStartColorGreen > 0 )
		dwCompleteGreen = nStartColorGreen + dwCompleteGreenRate;
	else if( nEndColorGreen - nStartColorGreen < 0 )
		dwCompleteGreen = nStartColorGreen - dwCompleteGreenRate;
	else
		dwCompleteGreen = nStartColorGreen;
	
	DWORD dwCompleteBlue = 0;
	if( nEndColorBlue - nStartColorBlue > 0 )
		dwCompleteBlue = nStartColorBlue + dwCompleteBlueRate;
	else if( nEndColorBlue - nStartColorBlue < 0 )
		dwCompleteBlue = nStartColorBlue - dwCompleteBlueRate;
	else
		dwCompleteBlue = nStartColorBlue;
	
	return D3DCOLOR_ARGB( 255, dwCompleteRed, dwCompleteGreen, dwCompleteBlue );
}
//-----------------------------------------------------------------------------
DWORD GetRootHeadQuest( DWORD dwHeadQuest )
{
	QuestProp* pHeadQuestProp = prj.m_aPropQuest.GetAt( dwHeadQuest );
	return ( pHeadQuestProp->m_nHeadQuest > 0 ) ? GetRootHeadQuest( pHeadQuestProp->m_nHeadQuest ) : dwHeadQuest;
}
//-----------------------------------------------------------------------------
DWORD SetQuestDestinationInformation( DWORD dwQuestID, DWORD dwGoalIndex )
{
	D3DXVECTOR3& rDestinationArrow = g_WndMng.m_pWndWorld->m_vDestinationArrow;
	rDestinationArrow = D3DXVECTOR3( -1.0F, 0.0F, -1.0F );

	if( dwGoalIndex == 0 )
		return 0;

	QuestProp* pQuestProp = prj.m_aPropQuest.GetAt( dwQuestID );
	if( pQuestProp == NULL )
		return 0;

	if( g_WndMng.m_pWndWorld == NULL )
		return 0;

	static const FLOAT CHARACTER_HEIGHT = 0.0F;
	int i = 0;
	
	int nEndCondItemNum = pQuestProp->m_nEndCondItemNum;
	for( i = 0; i < nEndCondItemNum; ++i )
	{
		QuestGoalData& roQuestGoalData = pQuestProp->m_paEndCondItem[ i ].m_ItemGoalData;
		if( roQuestGoalData.m_dwGoalIndex == 0 )
			continue;
		else if( roQuestGoalData.m_dwGoalIndex == dwGoalIndex )
		{
			rDestinationArrow = D3DXVECTOR3( roQuestGoalData.m_fGoalPositionX, CHARACTER_HEIGHT, roQuestGoalData.m_fGoalPositionZ );
			return roQuestGoalData.m_dwGoalTextID;
		}
	}

	for( i = 0 ; i < 2; ++i )
	{
		QuestGoalData& roQuestGoalData = pQuestProp->m_KillNPCGoalData[ i ];
		if( roQuestGoalData.m_dwGoalIndex == 0 )
			continue;
		else if( roQuestGoalData.m_dwGoalIndex == dwGoalIndex )
		{
			rDestinationArrow = D3DXVECTOR3( roQuestGoalData.m_fGoalPositionX, CHARACTER_HEIGHT, roQuestGoalData.m_fGoalPositionZ );
			return roQuestGoalData.m_dwGoalTextID;
		}
	}

	{
		QuestGoalData& roQuestGoalData = pQuestProp->m_MeetCharacterGoalData;
		if( roQuestGoalData.m_dwGoalIndex == 0 )
			return 0;
		else if( roQuestGoalData.m_dwGoalIndex == dwGoalIndex )
		{
			rDestinationArrow = D3DXVECTOR3( roQuestGoalData.m_fGoalPositionX, CHARACTER_HEIGHT, roQuestGoalData.m_fGoalPositionZ );
			return roQuestGoalData.m_dwGoalTextID;
		}
	}

	{
		QuestGoalData& roQuestGoalData = pQuestProp->m_PatrolWorldGoalData;
		if( roQuestGoalData.m_dwGoalIndex == 0 )
			return 0;
		else if( roQuestGoalData.m_dwGoalIndex == dwGoalIndex )
		{
			rDestinationArrow = D3DXVECTOR3( roQuestGoalData.m_fGoalPositionX, CHARACTER_HEIGHT, roQuestGoalData.m_fGoalPositionZ );
			return roQuestGoalData.m_dwGoalTextID;
		}
	}

	{
		QuestGoalData& roQuestGoalData = pQuestProp->m_DialogCharacterGoalData;
		if( roQuestGoalData.m_dwGoalIndex == 0 )
			return 0;
		else if( roQuestGoalData.m_dwGoalIndex == dwGoalIndex )
		{
			rDestinationArrow = D3DXVECTOR3( roQuestGoalData.m_fGoalPositionX, CHARACTER_HEIGHT, roQuestGoalData.m_fGoalPositionZ );
			return roQuestGoalData.m_dwGoalTextID;
		}
	}
	return 0;
}
//-----------------------------------------------------------------------------
void ProcessQuestDestinationWorldMap( DWORD dwGoalTextID )
{
	const D3DXVECTOR3& rDestinationArrow = g_WndMng.m_pWndWorld->m_vDestinationArrow;
	if( rDestinationArrow != D3DXVECTOR3( -1.0F, 0.0F, -1.0F ) )
	{
#ifdef __IMPROVE_MAP_SYSTEM
		CWndMapEx* pWndMapEx = ( CWndMapEx* )g_WndMng.CreateApplet( APP_MAP_EX );
		if( pWndMapEx == NULL )
		{
			return;
		}

		pWndMapEx->SetConstructionMode( CWndMapEx::DESTINATION );
#else // __IMPROVE_MAP_SYSTEM
		CWorldMap::GetInstance()->SetDestinationMode( TRUE );
		CWorldMap::GetInstance()->LoadWorldMap();
		CWorldMap::GetInstance()->UpdateDestinationPosition( rDestinationArrow );
#endif // __IMPROVE_MAP_SYSTEM
	}
	else
	{
		if( dwGoalTextID == 0 )
			return;

		g_WndMng.OpenMessageBoxWithTitle( prj.GetQuestDestination( dwGoalTextID ), prj.GetText( TID_GAME_DESTINATION_BOX_TITLE ) );
	}
}
//-----------------------------------------------------------------------------
#endif // __IMPROVE_QUEST_INTERFACE