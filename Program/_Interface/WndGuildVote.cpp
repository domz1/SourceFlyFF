#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndGuildVote.h"
#include "DPClient.h"
extern	CDPClient	g_DPlay;

#include "guild.h"
extern	CGuildMng	g_GuildMng;




/****************************************************
  WndId : APP_GUILDVOTE - 길드투표창
  CtrlId : WIDC_TABCTRL1 - TabCtrl
****************************************************/
CWndGuildVote::CWndGuildVote() 
{ 
	m_pWndGuildVoteSeting = NULL;
} 
CWndGuildVote::~CWndGuildVote() 
{ 
	SAFE_DELETE( m_pWndGuildVoteSeting );
} 
void CWndGuildVote::OnDraw( C2DRender* p2DRender ) 
{ 
} 

void CWndGuildVote::UpdateDataAll()
{
}

void CWndGuildVote::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요

	CWndEdit* pEdit;
	
	pEdit = (CWndEdit*) GetDlgItem( WIDC_EDIT1 );
	pEdit->EnableWindow(FALSE);
	pEdit = (CWndEdit*) GetDlgItem( WIDC_EDIT4 );
	pEdit->EnableWindow(FALSE);
	pEdit = (CWndEdit*) GetDlgItem( WIDC_EDIT2 );
	pEdit->EnableWindow(FALSE);
	pEdit = (CWndEdit*) GetDlgItem( WIDC_EDIT5 );
	pEdit->EnableWindow(FALSE);
	pEdit = (CWndEdit*) GetDlgItem( WIDC_EDIT3 );
	pEdit->EnableWindow(FALSE);

	CGuild* pGuild = g_pPlayer->GetGuild();
	if( !pGuild )
	{
		g_WndMng.PutString( "소속된 길드가 없어서 길드투표창을 열 수 없습니다." );
		Destroy();
		return;
	}
	else
	if( !(pGuild->m_nLevel > 7) )
	{
		g_WndMng.PutString( prj.GetText(TID_GAME_GUILDNOTLEVEL) );
		Destroy();
		return;
	}

	CWndComboBox* pCombo = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX1);

	pCombo->ResetContent();
	
	list <CGuildVote*>::iterator it = pGuild->m_votes.begin();
	
	for ( ; it != pGuild->m_votes.end() ; ++it )
	{
		int nIndex;
		nIndex = pCombo->AddString( (*it)->GetTitle() );
		pCombo->SetItemData( nIndex, (*it)->GetID() );
	}
		
	CWndButton* pWndButton[4];

	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_RADIO1 );
	pWndButton[ 1 ] = (CWndButton*)GetDlgItem( WIDC_RADIO2 );
	pWndButton[ 2 ] = (CWndButton*)GetDlgItem( WIDC_RADIO3 );
	pWndButton[ 3 ] = (CWndButton*)GetDlgItem( WIDC_RADIO4 );
	pWndButton[ 0 ]->SetGroup( TRUE );

	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndGuildVote::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_GUILD_VOTE, 0, CPoint( 0, 0 ), pWndParent );
} 
BOOL CWndGuildVote::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndGuildVote::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndGuildVote::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndGuildVote::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndGuildVote::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	CGuild* pGuild = g_pPlayer->GetGuild();

	if( pGuild == NULL )
		return FALSE;

	switch( nID )
	{
		case WIDC_COMBOBOX1:
			{
				if( message == WNM_SELCHANGE )
				{
					CWndComboBox* pCombo = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX1);
					SelChange( pGuild, pCombo->GetCurSel());
				}
				break;
			}
		// 길드 투표 설정
		case WIDC_VOTE_SETTING:
			{
				if(	pGuild->IsMaster( g_pPlayer->m_idPlayer ) == FALSE )
				{
					g_WndMng.OpenMessageBox( "길드마스터만 설정할수 있습니다.", MB_OK, this );
					break;
				}

				SAFE_DELETE( m_pWndGuildVoteSeting );
				m_pWndGuildVoteSeting = new CWndGuildVoteSeting;
				CWndComboBox* pCombo = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX1);
				m_pWndGuildVoteSeting->m_nSelect = pCombo->GetCurSel();
				m_pWndGuildVoteSeting->Initialize( this );
				break;
			}
		// 길드 투표
		case WIDC_VOTE:
			{
				// 현재 투표할 메뉴가 완료된것이냐?
				CWndComboBox* pCombo = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX1);

				if( pCombo->GetCurSel() == -1 )
				{
					g_WndMng.OpenMessageBox( "종류를 고르세요.", MB_OK, this );
					return FALSE;
				}
				
				
				if( pGuild->m_votes.size() )
				{
					list <CGuildVote*>::iterator it = pGuild->m_votes.begin();
					
					for ( ; it != pGuild->m_votes.end() ; ++it )
					{
						if( (*it)->GetID() == pCombo->GetItemData(pCombo->GetCurSel()) )
						{
							if( (*it)->IsCompleted() )
							{
								g_WndMng.OpenMessageBox( "투표가 완료된 사항입니다.", MB_OK, this );
								return FALSE;
							}
						}
					}
				}
				
				int nSelect = -1;

				CWndButton* pWndButton[4];				
				pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_RADIO1 );
				pWndButton[ 1 ] = (CWndButton*)GetDlgItem( WIDC_RADIO2 );
				pWndButton[ 2 ] = (CWndButton*)GetDlgItem( WIDC_RADIO3 );
				pWndButton[ 3 ] = (CWndButton*)GetDlgItem( WIDC_RADIO4 );
				
				if( pWndButton[ 0 ]->GetCheck() )
					nSelect = 0;
				else
				if( pWndButton[ 1 ]->GetCheck() )
					nSelect = 1;
				else
				if( pWndButton[ 2 ]->GetCheck() )
					nSelect = 2;
				else
				if( pWndButton[ 3 ]->GetCheck() )
					nSelect = 3;


				CWndEdit* pEdit;

				switch(nSelect)
				{
					case 0:
						{
							pEdit = (CWndEdit*) GetDlgItem( WIDC_EDIT4 );

							if( strlen( pEdit->GetString() ) <= 0 )
								return FALSE;
						}
						break;
					case 1:
						{
							pEdit = (CWndEdit*) GetDlgItem( WIDC_EDIT2 );
							
							if( strlen( pEdit->GetString() ) <= 0 )
								return FALSE;
						}
						break;
					case 2:
						{
							pEdit = (CWndEdit*) GetDlgItem( WIDC_EDIT5 );
							
							if( strlen( pEdit->GetString() ) <= 0 )
								return FALSE;
						}
						break;
					case 3:
						{
							pEdit = (CWndEdit*) GetDlgItem( WIDC_EDIT3 );
							
							if( strlen( pEdit->GetString() ) <= 0 )
								return FALSE;
						}
						break;
				}

				g_DPlay.SendCastVote( pCombo->GetItemData( pCombo->GetCurSel() ), nSelect );
				break;
			}
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 
void CWndGuildVote::OnMouseMove(UINT nFlags, CPoint point )
{
}


/****************************************************
  WndId : APP_GUILD_VOTESETING
****************************************************/
CWndGuildVoteSeting::CWndGuildVoteSeting() 
{ 
	m_nSelect = -1;
} 
CWndGuildVoteSeting::~CWndGuildVoteSeting() 
{ 
} 
void CWndGuildVoteSeting::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndGuildVoteSeting::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요

	if( m_nSelect != -1 )
	{
		//WIDC_EDIT1
		CWndEdit* pEdit;
		CWndComboBox* pCombo = (CWndComboBox*)GetParentWnd()->GetDlgItem(WIDC_COMBOBOX1);

		CString str = pCombo->GetString();

		pEdit = (CWndEdit*) GetDlgItem( WIDC_EDIT1 );
		pEdit->SetString( str );


		pEdit  = (CWndEdit*) GetDlgItem( WIDC_EDIT2 );
		pEdit->SetString( ((CWndEdit*) GetParentWnd()->GetDlgItem(WIDC_EDIT1))->GetString() );

		pEdit = (CWndEdit*) GetDlgItem( WIDC_EDIT3 );
		pEdit->SetString( ((CWndEdit*) GetParentWnd()->GetDlgItem(WIDC_EDIT4))->GetString() );

		pEdit = (CWndEdit*) GetDlgItem( WIDC_EDIT5 );
		pEdit->SetString( ((CWndEdit*) GetParentWnd()->GetDlgItem(WIDC_EDIT2))->GetString() );

		pEdit = (CWndEdit*) GetDlgItem( WIDC_EDIT6 );
		pEdit->SetString( ((CWndEdit*) GetParentWnd()->GetDlgItem(WIDC_EDIT5))->GetString() );

		pEdit = (CWndEdit*) GetDlgItem( WIDC_EDIT4 );
		pEdit->SetString( ((CWndEdit*) GetParentWnd()->GetDlgItem(WIDC_EDIT3))->GetString() );
		
	}

	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndGuildVoteSeting::Initialize( CWndBase* pWndParent ) 
{ 
	LPWNDAPPLET lpWndApplet = m_resMng.GetAt ( APP_GUILD_VOTESETING );
	CRect rect = CRect( 0, 0, lpWndApplet->size.cx, lpWndApplet->size.cy );

	return CWndNeuz::Create( /*WBS_THICKFRAME |*/ WBS_MOVE | WBS_SOUND | WBS_CAPTION | WBS_MODAL, rect, pWndParent, APP_GUILD_VOTESETING ); 
} 

BOOL CWndGuildVoteSeting::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndGuildVoteSeting::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndGuildVoteSeting::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndGuildVoteSeting::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndGuildVoteSeting::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch( nID )
	{
	// 길드 설정창 클리어
	case WIDC_VOTE_CLEAR:
		{
			CWndEdit* pEdit;
			
			pEdit = (CWndEdit*) GetDlgItem( WIDC_EDIT1 );
			pEdit->SetString( "" );
			pEdit = (CWndEdit*) GetDlgItem( WIDC_EDIT2 );
			pEdit->SetString( "" );
			pEdit = (CWndEdit*) GetDlgItem( WIDC_EDIT3 );
			pEdit->SetString( "" );
			pEdit = (CWndEdit*) GetDlgItem( WIDC_EDIT5 );
			pEdit->SetString( "" );
			pEdit = (CWndEdit*) GetDlgItem( WIDC_EDIT6 );
			pEdit->SetString( "" );
			pEdit = (CWndEdit*) GetDlgItem( WIDC_EDIT4 );
			pEdit->SetString( "" );

			m_nSelect = -1;
		}
		break;
	// 길드 투표 시작
	case WIDC_VOTE_START:
		{
			CGuild* pGuild = g_pPlayer->GetGuild();

			if( pGuild )
			{
				// 현재 투표할 메뉴가 완료된것이냐?
				CWndComboBox* pCombo = (CWndComboBox*)GetParentWnd()->GetDlgItem(WIDC_COMBOBOX1);

				if( m_nSelect != -1 )
				{
					if( pGuild->m_votes.size() )
					{
						list <CGuildVote*>::iterator it = pGuild->m_votes.begin();
						
						for ( ; it != pGuild->m_votes.end() ; ++it )
						{
							if( (*it)->GetID() == pCombo->GetItemData(m_nSelect) )
							{
								if( (*it)->IsCompleted() )
								{
									g_WndMng.OpenMessageBox( "투표가 완료된 사항입니다.", MB_OK, this );
									return FALSE;
								}
								else
								{
									g_WndMng.OpenMessageBox( "이미 진행중입니다.", MB_OK, this );
									return FALSE;
								}
								
							}
						}
					}
				}
				

				// 입력사항 체크 부분...
				CWndEdit* pEdit1 = (CWndEdit*) GetDlgItem( WIDC_EDIT1 );
				CWndEdit* pEdit2 = (CWndEdit*) GetDlgItem( WIDC_EDIT2 );

				if( strlen( pEdit1->GetString() ) <= 0 || strlen( pEdit2->GetString() ) <= 0 )
				{
					g_WndMng.OpenMessageBox( "투표제목과 내용은 필수 입력 사항입니다.", MB_OK, this );
					break;
				}

				int nChk = -1;
				CWndEdit* pEdit[4];
				
				pEdit[0] = (CWndEdit*) GetDlgItem( WIDC_EDIT3 );
				pEdit[1] = (CWndEdit*) GetDlgItem( WIDC_EDIT5 );
				pEdit[2] = (CWndEdit*) GetDlgItem( WIDC_EDIT6 );
				pEdit[3] = (CWndEdit*) GetDlgItem( WIDC_EDIT4 );

				for( int i=0; i<4; i++ )
				{
					if( strlen( pEdit[i]->GetString() ) > 0 )
						nChk = i;
				}

				if( nChk != -1 )
				{
					if( nChk < 1 )
					{
						g_WndMng.OpenMessageBox( "투표 항목은 최소한 2개 이상이어야 합니다.", MB_OK, this );
						break;
					}

					for( i=0; i<=nChk; i++ )
					{
						if( strlen( pEdit[i]->GetString() ) <= 0 )
						{
							g_WndMng.OpenMessageBox( "중간에 빈 내용이 있습니다.", MB_OK, this );
							return FALSE;
						}
					}
					
				}
				else
				{
					g_WndMng.OpenMessageBox( "투표 항목은 최소한 2개 이상이어야 합니다.", MB_OK, this );
					break;
				}

				LPCTSTR ptext[4];

				ptext[0] = pEdit[0]->GetString();
				ptext[1] = pEdit[1]->GetString();
				ptext[2] = pEdit[2]->GetString();
				ptext[3] = pEdit[3]->GetString();
				
				g_DPlay.SendAddVote( pEdit1->GetString(), pEdit2->GetString(), ptext );
			}

			Destroy();
		}
		break;
	// 길드투표 끝내기
	case WIDC_VOTE_FINISH:
		{
			CWndComboBox* pCombo = (CWndComboBox*)GetParentWnd()->GetDlgItem(WIDC_COMBOBOX1);
			
			if( pCombo->GetCurSel() != -1 )
				g_DPlay.SendCloseVote( pCombo->GetItemData( pCombo->GetCurSel() ) );

			Destroy();
		}
		break;
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 


void CWndGuildVote::SetRadioGroup(int ncount)
{
	CWndButton* pWndButton[4] = { NULL };

	if( ncount == 1 )
	{
		pWndButton[ 2 ] = (CWndButton*)GetDlgItem( WIDC_RADIO3 );
		pWndButton[ 3 ] = (CWndButton*)GetDlgItem( WIDC_RADIO4 );

		pWndButton[ 2 ]->EnableWindow( FALSE );
		pWndButton[ 3 ]->EnableWindow( FALSE );
	}

	if( ncount == 2 )
	{
		pWndButton[ 3 ] = (CWndButton*)GetDlgItem( WIDC_RADIO4 );

		pWndButton[ 3 ]->EnableWindow( FALSE );
	}
}



void CWndGuildVote::SetQuestion( const char *question)
{
	CWndEdit* pEdit;
	pEdit = (CWndEdit*) GetDlgItem( WIDC_EDIT1 );
	pEdit->SetString(question);
}


void CWndGuildVote::SetszString(char *text1, char *text2, char *text3, char *text4 )
{
	CWndEdit* pEdit;

	pEdit = (CWndEdit*) GetDlgItem( WIDC_EDIT4 );
	pEdit->SetString( text1 );

	pEdit = (CWndEdit*) GetDlgItem( WIDC_EDIT2 );
	pEdit->SetString( text2 );

	pEdit = (CWndEdit*) GetDlgItem( WIDC_EDIT5 );
	pEdit->SetString( text3 );

	pEdit = (CWndEdit*) GetDlgItem( WIDC_EDIT3 );
	pEdit->SetString( text4 );
}

void CWndGuildVote::SetCount(int ncount1, int ncount2, int ncount3, int ncount4 )
{
	CWndStatic* pStatic;
	
	char buffer[255] = { 0 };

	pStatic = (CWndStatic*) GetDlgItem( WIDC_STATIC5 );
	_itoa( ncount1, buffer, 10 );
	pStatic->SetTitle( buffer );
		
	pStatic = (CWndStatic*) GetDlgItem( WIDC_STATIC6 );
	_itoa( ncount2, buffer, 10 );
	pStatic->SetTitle( buffer );

	pStatic = (CWndStatic*) GetDlgItem( WIDC_STATIC7 );
	_itoa( ncount3, buffer, 10 );
	pStatic->SetTitle( buffer );

	pStatic = (CWndStatic*) GetDlgItem( WIDC_STATIC8 );
	_itoa( ncount4, buffer, 10 );
	pStatic->SetTitle( buffer );

	pStatic = (CWndStatic*) GetDlgItem( WIDC_STATIC10 );
	int sum = ncount1+ncount2+ncount3+ncount4;
	_itoa( sum, buffer, 10 );
	pStatic->SetTitle( buffer );
}

void CWndGuildVote::SelChange(CGuild* pGuild, int nIndex)
{
	GUILD_VOTE_SELECT guildvotesel[4];
	
	CWndComboBox* pCombo = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX1);
	
	list <CGuildVote*>::iterator it = pGuild->m_votes.begin();
	
	for ( ; it != pGuild->m_votes.end() ; ++it )
	{
		if( (*it)->GetID() == pCombo->GetItemData(nIndex) )
		{
			guildvotesel[0] = (*it)->GetVoteSelect(0);
			guildvotesel[1] = (*it)->GetVoteSelect(1);
			guildvotesel[2] = (*it)->GetVoteSelect(2);
			guildvotesel[3] = (*it)->GetVoteSelect(3);
			
			SetQuestion( (*it)->GetQuestion() );
			break;
		}
	}
	
	SetszString( guildvotesel[0].szString, guildvotesel[1].szString, guildvotesel[2].szString, guildvotesel[3].szString );
	SetCount( guildvotesel[0].cbCount, guildvotesel[1].cbCount, guildvotesel[2].cbCount, guildvotesel[3].cbCount );
}



