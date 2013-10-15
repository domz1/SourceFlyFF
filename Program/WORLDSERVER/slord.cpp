#include "stdafx.h"

#if __VER >= 12 // __LORD

#include "slord.h"
#include "lordskillexecutable.h"
#include "definelordskill.h"
#include "definetext.h"
#include "user.h"
extern	CUserMng	g_UserMng;

////////////////////////////////////////////////////////////////////////////////
CSLord::CSLord()
:
CLord()
{
}

CSLord::~CSLord()
{
}

CSLord* CSLord::Instance( void )
{
	static CSLord sLord;
	return &sLord;
}

void CSLord::CreateColleagues( void )
{
	CSElection * pElection	= new CSElection( this );
	CLEvent* pEvent		= new CLEvent( this );
	CLordSkill* pSkills		= new CSLordSkill( this );
	pEvent->Initialize( "lordevent.inc" );
	pSkills->Initialize( "lordskill.txt" );
	m_pElection		= pElection;
	m_pEvent	= pEvent;
	m_pSkills	= pSkills;
}

void CSLord::DestroyColleagues( void )
{
	SAFE_DELETE( m_pElection );
	SAFE_DELETE( m_pEvent );
	SAFE_DELETE( m_pSkills );
}

////////////////////////////////////////////////////////////////////////////////
CSElection::CSElection( CLord* pLord )
:
IElection( pLord )
{
}

CSElection::~CSElection()
{
}

BOOL CSElection::DoTestBeginCandidacy( void )
{
	g_UserMng.AddElectionBeginCandidacy();
	return TRUE;
}

BOOL CSElection::DoTestBeginVote( int & nRequirement )
{
	g_UserMng.AddElectionBeginVote( nRequirement );
	return TRUE;
}

BOOL CSElection::DoTestEndVote( u_long idPlayer )
{
	g_UserMng.AddQueryPlayerData( idPlayer );
	g_UserMng.AddElectionEndVote( idPlayer );
	return TRUE;
}

void CSElection::DoAddDepositComplete( u_long idPlayer, __int64 iDeposit, time_t tCreate )
{
	g_UserMng.AddElectionAddDeposit( idPlayer, iDeposit, tCreate );
}

BOOL CSElection::DoTestSetPledge( u_long idPlayer, const char* szPledge )
{
	g_UserMng.AddElectionSetPledge( idPlayer, szPledge );
	return TRUE;
}

BOOL CSElection::DoTestIncVote( u_long idPlayer, u_long idElector )
{
	g_UserMng.AddElectionIncVote( idPlayer, idElector );
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
CLEvent::CLEvent( CLord* pLord )
:
ILordEvent( pLord )
{
}

CLEvent::~CLEvent()
{
}

BOOL CLEvent::DoTestAddComponent( CLEComponent* pComponent )
{
	g_UserMng.AddLEventCreate( pComponent );
	return TRUE;
}

BOOL CLEvent::DoTestInitialize( void )
{
	g_UserMng.AddLEventInitialize();
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
CSLordSkill::CSLordSkill( CLord* pLord )
: CLordSkill( pLord )
{
}

CSLordSkill::~CSLordSkill()
{
}

CLordSkillComponentExecutable* CSLordSkill::CreateSkillComponent( int nType )
{
	switch( nType )
	{
		case LT_ITEMBUF:
			return new CLordSkillComponentItemBuf;
		case LT_SUMMON:	
			return new CLordSkillComponentSummon;
		case LT_TELEPORT:
			return new CLordSkillComponentTeleport;
		case LT_PASSIVE:
		default:
			return new CLordSkillComponentExecutable;
	}
}

////////////////////////////////////////////////////////////////////////////////
namespace	election
{
	int	AddDepositRequirements( CUser* pUser, __int64 iTotal, __int64 & iDeposit )
	{
		if( !pUser->IsMaster() && !pUser->IsHero() )
			return TID_GAME_ELECTION_ADD_DEPOSIT_E001;	// "히어로, 마스터 등급의 캐릭터만 군주 후보에 입찰을 하실 수 있습니다."
		if( iTotal < CCandidate::nMinDeposit )
			return TID_GAME_ELECTION_ADD_DEPOSIT_E002;	// "입찰 금액이 부족합니다."
		IElection* pElection	= CSLord::Instance()->GetElection();

		if( pElection->GetState() != IElection::eCandidacy )
			return TID_GAME_ELECTION_ADD_DEPOSIT_E004;		//  후보 등록 기간이 아닙니다.

		CCandidate* pCandidate	= pElection->GetCandidate( pUser->m_idPlayer );
		__int64 iOld	= ( pCandidate? pCandidate->GetDeposit(): 0 );
		if( iOld >= iTotal )
			return TID_GAME_ELECTION_ADD_DEPOSIT_E003;	// "기존 페냐보다 더 많은 금액을 신청하시기 바랍니다."
		if( pUser->IsQuerying() )
			return TID_GAME_LORD_IS_QUERYING;
		iDeposit	= iTotal - iOld;
		if( iDeposit > INT_MAX || pUser->GetGold() < static_cast<int>( iDeposit ) )
			return TID_GAME_ELECTION_ADD_DEPOSIT_E002;	// "입찰 금액이 부족합니다."
		if( iDeposit < static_cast<__int64>( 10000000 ) )
			return TID_GAME_ELECTION_ADD_DEPOSIT_E005;	// 추가 입찰은 최소 10,000,000 페냐가 필요합니다.
		pUser->SetQuerying( TRUE );
		return 0;
	}

	int SetPledgeRequirements( CUser* pUser, int & nCost )
	{
		nCost	= 0;
		IElection* pElection	= CSLord::Instance()->GetElection();
		CCandidate* pCandidate	= pElection->GetCandidate( pUser->m_idPlayer );
		if( pCandidate )
		{
			if( pElection->GetState() != IElection::eVote )
				return TID_GAME_ELECTION_SET_PLEDGE_E003;

			if( lstrlen( pCandidate->GetPledge() ) > 0 )
			{
				if( pUser->GetGold() < CCandidate::nSetPledgeCost )
					return TID_GAME_ELECTION_SET_PLEDGE_E002;
				nCost	= CCandidate::nSetPledgeCost;
			}
			if( pUser->IsQuerying() )
				return TID_GAME_LORD_IS_QUERYING;

			pUser->SetQuerying( TRUE );
			return 0;
		}
		return TID_GAME_ELECTION_SET_PLEDGE_E001;
	}

	int	IncVoteRequirements( CUser* pUser, u_long idPlayer )
	{
		IElection* pElection	= CSLord::Instance()->GetElection();
		CCandidate* pCandidate	= pElection->GetCandidate( idPlayer );
		if( !pCandidate )
			return TID_GAME_ELECTION_INC_VOTE_E001;		// 후보에 없거나 후보명을 잘못 입력 하셨습니다. 확인 후 다시 입력해 주시기 바랍니다.
		if( pElection->GetState() != IElection::eVote )
			return TID_GAME_ELECTION_INC_VOTE_E002;		// 투표 기간이 아닙니다.
		if( pUser->GetLevel() < IElection::nLevelRequirements )
			return TID_GAME_ELECTION_INC_VOTE_E003;		// 히어로/마스터 등급, 캐릭터 레벨 60이상의 캐릭터만 투표권을 가집니다.
		if( pUser->IsQuerying() )
			return TID_GAME_LORD_IS_QUERYING;
		if( pUser->GetElection() == pElection->GetId() )	// 康
			return TID_GAME_ELECTION_INC_VOTE_E004;		// 이미 투표에 참여하셨습니다.

		pUser->SetQuerying( TRUE );
		return 0;
	}
};

////////////////////////////////////////////////////////////////////////////////
namespace	lordevent
{
	int	CreateRequirements( CUser* pUser, int iEEvent, int iIEvent )
	{
		ILordEvent* pEvent		= CSLord::Instance()->GetEvent();
		if( !CSLord::Instance()->IsLord( pUser->m_idPlayer ) )	// is he lord?
			 return TID_GAME_L_EVENT_CREATE_E001;
		if( pEvent->GetComponent( pUser->m_idPlayer ) )		// is there no event?
			return TID_GAME_L_EVENT_CREATE_E002;
		if( pUser->IsQuerying() )
			return TID_GAME_LORD_IS_QUERYING;
		__int64 iCost	= pEvent->GetCost( iEEvent, iIEvent );
		if( iCost == 0 )
			return TID_GAME_L_EVENT_CREATE_E004;
		if( iCost > pUser->GetTotalGold() )
			return TID_GAME_L_EVENT_CREATE_E003;

		pUser->SetQuerying( TRUE );
		return 0;
	}
};

////////////////////////////////////////////////////////////////////////////////
#endif	// __LORD