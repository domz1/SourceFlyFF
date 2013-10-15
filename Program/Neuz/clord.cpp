#include "stdafx.h"

#if __VER >= 12 // __LORD

#include "clord.h"
#include "playerdata.h"
#include "definetext.h"
#include "resdata.h"
#include "DPClient.h"
extern	CDPClient	g_DPlay;

////////////////////////////////////////////////////////////////////////////////
// 클라이언트용 군주 클래스
CCLord::CCLord()
:
CLord()
{
}

CCLord::~CCLord()
{
}

CCLord* CCLord::Instance( void )
{
	static CCLord sLord;
	return &sLord;
}

void CCLord::CreateColleagues( void )
{
	// 생성
	CCElection * pElection	= new CCElection( this );
	CLEvent* pEvent		= new CLEvent( this );
	CLordSkill* pSkills		= new CLordSkill( this );
	// 스크립트 로드
	pEvent->Initialize( "lordevent.inc" );
	pSkills->Initialize( "lordskill.txt" );
	// 설정
	m_pElection		= pElection;
	m_pEvent	= pEvent;
	m_pSkills	= pSkills;
}

void CCLord::DestroyColleagues( void )
{	// 협력 객체들의 제거
	SAFE_DELETE( m_pElection );
	SAFE_DELETE( m_pEvent );
	SAFE_DELETE( m_pSkills );
}

////////////////////////////////////////////////////////////////////////////////
// 클라이언트용 선거 클래스
CCElection::CCElection( CLord* pLord )
:
IElection( pLord )
{
}

CCElection::~CCElection()
{
}

void CCElection::PrintCaption( const char* lpCaption )
{
	CWndWorld* pWndWorld	= static_cast<CWndWorld*>( g_WndMng.GetWndBase( APP_WORLD ) );
	if( !pWndWorld )
		return;
	if( ::GetLanguage() != LANG_JAP )
		g_Caption1.AddCaption( lpCaption, pWndWorld->m_pFontAPICaption );
	else
		g_Caption1.AddCaption( lpCaption, NULL );
}

BOOL CCElection::DoTestBeginCandidacy( void )
{	// 군주 입후보 시작
	PrintCaption( prj.GetText( TID_GAME_ELECTION_CAPTION_BEGIN_CANDIDACY_01 ) );
	UpdateUI();
	return TRUE;
}

BOOL CCElection::DoTestBeginVote( int & nRequirement )
{	// 군주 후보 확정
	// 투표 시작
	PrintCaption( prj.GetText( TID_GAME_ELECTION_CAPTION_BEGIN_VOTE_01 ) );
	PrintCaption( prj.GetText( TID_GAME_ELECTION_CAPTION_BEGIN_VOTE_02 ) );
	for( VSPC::iterator i = m_vCandidates.begin(); i != m_vCandidates.end(); ++i )
	{
		SPC ptr	= *i;
		if( election::IsActivePlayer( ptr->GetIdPlayer() ) )
			g_WndMng.OpenMessageBoxUpper( prj.GetText( TID_GAME_ELECTION_BEGIN_VOTE_S001 ) );	// "축하합니다. 최종 10명의 군주 후보가 되셨습니다. 군주보좌관 NPC를 통해 공약을 설정하실 수 있습니다."
	}
	UpdateUI();
	return TRUE;
}

BOOL CCElection::DoTestEndVote( u_long idPlayer )
{	// 투표 종료
	PrintCaption( prj.GetText( TID_GAME_ELECTION_CAPTION_END_VOTE_01 ) );
	char lpString[100]	= { 0,};
	DWORD dwColor	= 0xff000000;//adeilson 0xffffffff
	if( idPlayer != NULL_ID )
	{
		// OOO님이 군주로 선출되었습니다. 
		sprintf( lpString, prj.GetText( TID_GAME_ELECTION_CAPTION_END_VOTE_02 ), CPlayerDataCenter::GetInstance()->GetPlayerString( idPlayer ) );
		PrintCaption( lpString );
		// OOO님이 선출되어, 앞으로 2주 동안 군주로 집권을 하게 됩니다.
		sprintf( lpString, prj.GetText( TID_GAME_ELECTION_END_VOTE_S000 ), CPlayerDataCenter::GetInstance()->GetPlayerString( idPlayer ) );
		dwColor		= prj.GetTextColor( TID_GAME_ELECTION_END_VOTE_S000 );
	}
	else
	{
		// 이번 군주 선거는 무효가 되어, 2주 동안 군주가 없습니다. 
		sprintf( lpString, prj.GetText( TID_GAME_ELECTION_END_VOTE_S001 ) );
		dwColor		= prj.GetTextColor( TID_GAME_ELECTION_END_VOTE_S001 );
	}
	g_WndMng.PutString( lpString, NULL, dwColor );

	UpdateUI();
	return TRUE;
}

BOOL CCElection::DoTestAddDeposit( u_long idPlayer, __int64 iDeposit, time_t tCreate )
{	// 군주 입찰
	if( election::IsActivePlayer( idPlayer ) )
	{
		CCandidate* pCandidate	= GetCandidate( idPlayer );
		__int64 iOld	= ( pCandidate? pCandidate->GetDeposit(): 0 );
		char szText[260]	= { 0,};
		if( iOld )
			sprintf( szText, prj.GetText( TID_GAME_ELECTION_ADD_DEPOSIT_S001 ), iOld, iDeposit, iOld + iDeposit );
		else
			sprintf( szText, prj.GetText( TID_GAME_ELECTION_ADD_DEPOSIT_S002 ), iDeposit );
		g_WndMng.OpenMessageBoxUpper( szText );
		g_WndMng.PutString( prj.GetText( TID_GAME_ELECTION_ADD_DEPOSIT_S003 ), NULL, prj.GetTextColor( TID_GAME_ELECTION_ADD_DEPOSIT_S003 ) );
	}
	return TRUE;
}

void CCElection::DoAddDepositComplete( u_long idPlayer, __int64 iDeposit, time_t tCreate )
{
	if(g_WndMng.m_pWndLordTender) g_WndMng.m_pWndLordTender->RefreshDeposit();
	UpdateUI();
}

void CCElection::DoSetPlegeComplete( void )
{
	UpdateUI();
}

BOOL CCElection::DoTestIncVote( u_long idPlayer, u_long idElector )
{
	if( election::IsActivePlayer( idElector ) )
	{
		const char* szPlayer	= CPlayerDataCenter::GetInstance()->GetPlayerString( idPlayer );
		//  투표창이 닫히며, 'OOO님에게 투표를 하였습니다.' 라는 내용의 확인창 생성.
		if(g_WndMng.m_pWndLordVote)g_WndMng.m_pWndLordVote->Destroy();
		CString			string;
		string.Format(prj.GetText(TID_GAME_VOTE_COMPLETE), szPlayer);
		g_WndMng.OpenMessageBox(string);
	}
	return TRUE;
}

void CCElection::DoIncVoteComplete( void )
{
	UpdateUI();
}

void CCElection::DoEndVoteComplete( void )
{
	if( !g_pPlayer )
		return;
	if( !CCLord::Instance()->IsLord( g_pPlayer->m_idPlayer ) )
	{
		// 군주관련 숏컷제거 - 단축키
		CWndTaskBar* pTaskBar = g_WndMng.m_pWndTaskBar;
		for(int nSlot = 0; nSlot < MAX_SLOT_ITEM_COUNT; ++nSlot)
		{
			for(int nIndex = 0; nIndex < MAX_SLOT_ITEM; ++nIndex)
			{
				if(pTaskBar->m_aSlotItem[nSlot][nIndex].m_dwShortcut == SHORTCUT_LORDSKILL)
				{
					g_DPlay.SendRemoveItemTaskBar( nSlot, nIndex );
					pTaskBar->m_aSlotItem[nSlot][nIndex].Empty();

				}
			}
		}
		// 군주관련 숏컷제거 - 퀵슬롯
		for(nSlot = 0; nSlot < MAX_SLOT_APPLET; ++nSlot)
		{
			if(pTaskBar->m_aSlotApplet[nSlot].m_dwShortcut == SHORTCUT_LORDSKILL)
			{
				g_DPlay.SendRemoveAppletTaskBar(nSlot);
				pTaskBar->m_aSlotApplet[nSlot].Empty();
			}
			
		}
		// 군주 스킬 창 제거
		CWndLordSkill* pWndLordSkill = (CWndLordSkill*)g_WndMng.GetWndBase( APP_LORD_SKILL );
		if(pWndLordSkill) pWndLordSkill->Destroy();
		// 군주 이벤트 창 제거
		if(g_WndMng.m_pWndLordEvent) g_WndMng.m_pWndLordEvent->Destroy();
	}
}


void CCElection::UpdateUI( void )
{
#ifdef __INTERNALSERVER
	State();
#endif	// __INTERNALSERVER
}

void CCElection::State( void )
{	// 선거 상태 출력
#ifdef _DEBUG
	char lpString[100]	= { 0,};
	ELECTION_STATE eState	= GetState();
	sprintf( lpString, "state: %d", eState );
	g_WndMng.PutString( lpString, NULL, 0xff00ff00 );
	sprintf( lpString, "candidates list" );
	g_WndMng.PutString( lpString, NULL, 0xff00ff00 );
	for( int i = 0; i < (int)( m_vCandidates.size() ); i++ )
	{
		SPC ptr		= m_vCandidates[i];
		sprintf( lpString, "%02d: %07d: %I64d: %s: %d: %d", i, ptr->GetIdPlayer(), ptr->GetDeposit(), ptr->GetPledge(), ptr->GetVote(), ptr->GetCreate() );
		g_WndMng.PutString( lpString, NULL, 0xff00ff00 );
	}
#endif	// _DEBUG
}

SPC CCElection::GetRanker(int nRanking)
{
	SPC ptr;

	if(nRanking <= (int)( m_vCandidates.size() ) && nRanking > 0)
	{
		ptr	= m_vCandidates[nRanking - 1];
	}
	
	return ptr;
}

////////////////////////////////////////////////////////////////////////////////
// 클라이언트 용 군주 이벤트 클래스
CLEvent::CLEvent( CLord* pLord )
:
ILordEvent( pLord )
{
}

CLEvent::~CLEvent()
{
}

BOOL CLEvent::DoTestInitialize( void )
{
	return TRUE;
}

BOOL CLEvent::DoTestAddComponent( CLEComponent* pComponent )
{	// 이벤트 시작 화면 출력
	char lpString[255]	= { 0,};
	char lpCaption[255]		= { 0,};
	
	if( pComponent->GetEFactor() > 0.0F && pComponent->GetIFactor() > 0.0F )
	{
		lstrcpy( lpCaption, prj.GetText( TID_GAME_L_EVENT_CREATE_S001 ) );
		sprintf( lpString, prj.GetText( TID_GAME_L_EVENT_CREATE_S004 ), CPlayerDataCenter::GetInstance()->GetPlayerString( m_pLord->Get() ), pComponent->GetEFactor() * 100, pComponent->GetIFactor() * 100 );
	}
	else if( pComponent->GetEFactor() > 0.0F )
	{
		lstrcpy( lpCaption, prj.GetText( TID_GAME_L_EVENT_CREATE_S002 ) );
		sprintf( lpString, prj.GetText( TID_GAME_L_EVENT_CREATE_S005 ), CPlayerDataCenter::GetInstance()->GetPlayerString( m_pLord->Get() ), pComponent->GetEFactor() * 100 );
	}
	else if( pComponent->GetIFactor() > 0.0F )
	{
		lstrcpy( lpCaption, prj.GetText( TID_GAME_L_EVENT_CREATE_S003 ) );
		sprintf( lpString, prj.GetText( TID_GAME_L_EVENT_CREATE_S006 ), CPlayerDataCenter::GetInstance()->GetPlayerString( m_pLord->Get() ), pComponent->GetIFactor() * 100 );
	}
	
//	if( ::GetLanguage() != LANG_GER && ::GetLanguage() != LANG_FRE )
	{
		CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );
		if( pWndWorld )
			g_Caption1.AddCaption( lpCaption, pWndWorld->m_pFontAPITitle, FALSE, D3DCOLOR_ARGB( 255, 255, 255, 153 ) );
	}

	g_WndMng.PutString( lpString, NULL, prj.GetTextColor( TID_ADMIN_ANNOUNCE ), CHATSTY_SYSTEM );
	UpdateUI();
	return TRUE;
}

void CLEvent::UpdateUI( void )
{
}

////////////////////////////////////////////////////////////////////////////////
namespace	election
{

BOOL IsActivePlayer( u_long idPlayer )
{
	CMover* pPlayer		= CMover::GetActiveMover();
	return( IsValidObj( pPlayer ) && pPlayer->m_idPlayer == idPlayer );
}

////////////////////////////////////////////////////////////////////////////////

};

#endif	// __LORD