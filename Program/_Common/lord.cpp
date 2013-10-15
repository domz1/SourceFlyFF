#include "stdafx.h"

#if __VER >= 12 // __LORD

#include "lord.h"

#pragma warning ( disable : 4355 )

CCandidate::CCandidate()
:
m_idPlayer( 0 ),
m_iDeposit( 0 ),
m_nVote( 0 ),
m_tCreate( 0 )
{
	*m_szPledge		= '\0';
}

CCandidate::CCandidate( u_long idPlayer, __int64 iDeposit, const char* szPledge, int nVote, time_t tCreate )
:
m_idPlayer( idPlayer ),
m_iDeposit( iDeposit ),
m_nVote( nVote ),
m_tCreate( tCreate )
{
	lstrcpy( m_szPledge, szPledge );
}

CCandidate::~CCandidate()
{
}

__int64 CCandidate::AddDeposit( __int64 iDeposit )
{
	m_iDeposit		+= iDeposit;
	return m_iDeposit;
}

void CCandidate::Serialize( CAr & ar )
{
	if( ar.IsStoring() )
	{
		ar << m_idPlayer << m_iDeposit << m_nVote;
		ar << m_tCreate;
		ar.WriteString( m_szPledge );
	}
	else
	{
		ar >> m_idPlayer >> m_iDeposit >> m_nVote;
		ar >> m_tCreate;
		ar.ReadString( m_szPledge, nMaxPledgeLen );
	}
}

////////////////////////////////////////////////////////////////////////////////
void IElection::sProperty::Serialize( CAr & ar )
{
	if( ar.IsStoring() )
	{
		ar << nDayOfWeek << nHour << tCandidacy << tVote << fRequirementFactor << nDays;
		ar << m_vReturnDepositRates.size();
		for( vector<float>::iterator i1 = m_vReturnDepositRates.begin(); i1 != m_vReturnDepositRates.end(); ++i1 )
			ar << *( i1 );
		for( int iIndex = 0; iIndex < 2; iIndex++ )
		{
			ar << m_vItems[iIndex].size();
			for( vector<int>::iterator i2 = m_vItems[iIndex].begin(); i2 != m_vItems[iIndex].end(); ++i2 )
				ar << *( i2 );
		}
	}
	else
	{
		ar >> nDayOfWeek >> nHour >> tCandidacy >> tVote >> fRequirementFactor >> nDays;
		m_vReturnDepositRates.clear();
		size_t nSize;
		ar >> nSize;
		float f;
		for( size_t i = 0; i < nSize; i++ )
		{
			ar >> f;
			m_vReturnDepositRates.push_back( f );
		}
		for( int iIndex = 0; iIndex < 2; iIndex++ )
		{
			m_vItems[iIndex].clear();
			ar >> nSize;
			int n;
			for( i = 0; i < nSize; i++ )
			{
				ar >> n;
				m_vItems[iIndex].push_back( n );
			}
		}
	}
}
////////////////////////////////////////////////////////////////////////////////
IElection::IElection( CLord* pLord )
:
m_idElection( 0 ),
m_eState( eReady ),
m_tBegin( 0 ),
m_pLord( pLord ),
m_nRequirement( 0 )
{
}

IElection::~IElection()
{
	m_vCandidates.clear();
}

CCandidate*	IElection::AddCandidate( u_long idPlayer, __int64 iDeposit, const char* szPledge, int nVote, time_t tCreate )
{
	return AddCandidate( new CCandidate( idPlayer, iDeposit, szPledge, nVote, tCreate ) );
}

CCandidate* IElection::AddCandidate( CCandidate * pCandidate )
{
	m_vCandidates.push_back( SPC( pCandidate ) );
	return pCandidate;
}

CCandidate* IElection::GetCandidate( u_long idPlayer )
{
	for( VSPC::iterator i = m_vCandidates.begin(); i != m_vCandidates.end(); ++i )
	{
		if( ( *i )->GetIdPlayer() == idPlayer )
			return ( *i ).get();
	}
	return NULL;
}

int IElection::GetOrder( u_long idPlayer )
{
	for( VSPC::iterator i = m_vCandidates.begin(); i != m_vCandidates.end(); ++i )
	{
		if( ( *i )->GetIdPlayer() == idPlayer )
			return std::distance( m_vCandidates.begin(), i );
	}
	return -1;
}

void IElection::SetProperty( int nDayOfWeek, int nHour, time_t tCandidacy, time_t tVote )
{
	property.nDayOfWeek	= nDayOfWeek;
	property.nHour	= nHour;
	property.tCandidacy	= tCandidacy;
	property.tVote	= tVote;
}

void IElection::Serialize( CAr & ar )
{
	if( ar.IsStoring() )
	{
		ar << m_idElection << m_eState << m_tBegin;
		ar << m_nRequirement;
		property.Serialize( ar );
		ar << m_vCandidates.size();
		for( VSPC::iterator i = m_vCandidates.begin(); i != m_vCandidates.end(); ++i )
			( *i )->Serialize( ar );
	}
	else
	{
		m_vCandidates.clear();
		ar >> m_idElection >> m_eState >> m_tBegin;
		ar >> m_nRequirement;
		property.Serialize( ar );
		size_t nSize;
		ar >> nSize;
		for( size_t i = 0; i < nSize; i++ )
		{
			CCandidate* pCandidate	= new CCandidate;
			pCandidate->Serialize( ar );
			AddCandidate( pCandidate );
		}
	}
}

void IElection::SetNextBegin( void )
{
	SetBegin( GetNextBegin() );
}

time_t IElection::GetNextBegin( void )
{
	// 간격 설정
	CTime timeNext	= CTime( GetBegin() ) + CTimeSpan( property.nDays, 0, 0, 0 );
	if( !GetBegin() )	// 최초 선거라면
	{	// 현재 시간 이 후 가장 가까운 시작 시간을 계산
		CTime timeCur	= CTime::GetCurrentTime();
		int nDayOfWeek	= timeCur.GetDayOfWeek();
		int d	= property.nDayOfWeek - nDayOfWeek;
		if( d < 0 )
			d	+= 7;
		timeNext	= CTime( timeCur.GetYear(), timeCur.GetMonth(), timeCur.GetDay(), property.nHour, 0, 0 ) + CTimeSpan( d, 0, 0, 0 );
		if( timeNext <= timeCur )
			timeNext	+= CTimeSpan( 7, 0, 0, 0 );
	}
	return	( (time_t)( timeNext.GetTime() ) );
}

IElection::ELECTION_STATE IElection::GetPropertyState( void )
{
	if( !GetBegin() )
		return eReady;

	time_t	tCur	= time( NULL );
	time_t	tEndCandidacy	= GetBegin() + property.tCandidacy;
	time_t	tEndVote	= GetBegin() + property.tCandidacy + property.tVote;

	if( tCur >= GetBegin() && tCur < tEndCandidacy )
		return eCandidacy;
	else if( tCur >= tEndCandidacy && tCur < tEndVote )
		return eVote;
	else if( tCur > tEndVote )
		return eExpired;

	return eReady;
}

void IElection::BeginCandidacy( void )
{
	if( DoTestBeginCandidacy() )
		SetState( eCandidacy );
}

void IElection::BeginVote( int nRequirement )
{
	election::OutputDebugString( "IElection.BeginVote" );
	if( DoTestBeginVote( nRequirement ) )
	{
		SetState( eVote );
		if( m_vCandidates.size() > nMaxCandidates )
		{
			DoReturnDeposit();
			m_vCandidates.erase( m_vCandidates.begin() + nMaxCandidates, m_vCandidates.end() );
		}
		SetRequirement( nRequirement );
	}
}

void IElection::EndVote( u_long idPlayer )
{
	election::OutputDebugString( "IElection::EndVote( %d )", idPlayer );
	SortVote();

	if( idPlayer == 0 )
		idPlayer	= GetResult();	// 투표 결과 반환
	if( DoTestEndVote( idPlayer ) )
	{
		m_pLord->Set( idPlayer );	// 군주 설정
		m_pLord->ResetSkill();	// 군주 스킬 대기 시간 초기화
		PrepareNext();	// 선거 준비 상태로 변경
									// 입후보자 목록 초기화
									// 개시 시간 설정
									// 선거 식별자 증가
	}
	DoEndVoteComplete();
}

void IElection::PrepareNext( void )
{
	SetState( eReady );
	m_vCandidates.clear();
	SetNextBegin();
	m_idElection++;
}

void IElection::AddDeposit( u_long idPlayer, __int64 iDeposit, time_t tCreate )
{
	election::OutputDebugString( "IElection::AddDeposit( %07d, %I64d, %d )", idPlayer, iDeposit, tCreate );
	if( DoTestAddDeposit( idPlayer, iDeposit, tCreate ) )
	{
		CCandidate* pCandidate	= GetCandidate( idPlayer );
		if( !pCandidate )
			pCandidate	= AddCandidate( idPlayer, 0, "", 0, tCreate );
		pCandidate->AddDeposit( iDeposit );
		SortDeposit();
		DoAddDepositComplete( idPlayer, iDeposit, tCreate );
	}
}

void IElection::SetPledge( u_long idPlayer, const char* szPledge )
{
	election::OutputDebugString( "IElection::SetPledge( %07d, %s )", idPlayer, szPledge );
	if( DoTestSetPledge( idPlayer, szPledge ) )
	{
		CCandidate* pCandidate	= GetCandidate( idPlayer );
		if( pCandidate )
		{
			pCandidate->SetPledge( szPledge );
			DoSetPlegeComplete();
		}
	}
}

BOOL IElection::HasPledge( u_long idPlayer )
{
	CCandidate* pCandidate	= GetCandidate( idPlayer );
	return( pCandidate && lstrlen( pCandidate->GetPledge() ) > 0 );
}

void IElection::IncVote( u_long idPlayer, u_long idElector )
{
	election::OutputDebugString( "IElection::IncVote( %07d, %07d )", idPlayer, idElector );
	if( DoTestIncVote( idPlayer, idElector ) )
	{
		CCandidate* pCandidate	= GetCandidate( idPlayer );
		if( pCandidate )
		{
			pCandidate->SetVote( pCandidate->GetVote() + 1 );
			DoIncVoteComplete();
		}
	}
}

void IElection::SortDeposit( void )
{
	std::sort( m_vCandidates.begin(), m_vCandidates.end(), DepositCompare() );
}

void IElection::SortVote( void )
{
	std::sort( m_vCandidates.begin(), m_vCandidates.end(), VoteCompare() );
}

int IElection::GetVote()
{
	int nTotal	= 0;
	for( VSPC::iterator i = m_vCandidates.begin(); i != m_vCandidates.end(); ++i )
		nTotal	+= ( *i )->GetVote();
	return nTotal;
}

u_long IElection::GetResult( void )
{
	if( !IsValid() )
		return NULL_ID;
	if( m_vCandidates.empty() )
		return NULL_ID;
	return m_vCandidates[0]->GetIdPlayer();
}

////////////////////////////////////////////////////////////////////////////////
CLord::CLord()
:
m_idPlayer( NULL_ID ),
m_pElection( 0 ),
m_pEvent( 0 ),
m_pSkills( 0 )
{
}

CLord::~CLord()
{
	SAFE_DELETE( m_pElection );
	SAFE_DELETE( m_pEvent );
	SAFE_DELETE( m_pSkills );
}

void CLord::Serialize( CAr & ar )
{
	if( ar.IsStoring() )
		ar << m_idPlayer;
	else
		ar >> m_idPlayer;

	m_pElection->Serialize( ar );
	m_pEvent->Serialize( ar );
	m_pSkills->SerializeTick( ar );
}

////////////////////////////////////////////////////////////////////////////////
CLEComponent::CLEComponent()
:
nTick( 0 ),
idPlayer( NULL_ID ),
fEFactor( 1.0F ),
#ifdef __CLIENT
m_pTexture( NULL ),
#endif	// __CLIENT
fIFactor( 1.0F )
{
}

CLEComponent::CLEComponent( int t, u_long p, float e, float i )
:
nTick( t ),
idPlayer( p ),
fEFactor( e ),
#ifdef __CLIENT
m_pTexture( NULL ),
#endif	// __CLIENT
fIFactor( i )
{
}

CLEComponent::~CLEComponent()
{
}

void CLEComponent::Serialize( CAr & ar )
{
	if( ar.IsStoring() )
		ar << nTick << idPlayer << fEFactor<< fIFactor;
	else
	{
		ar >> nTick >> idPlayer >> fEFactor >> fIFactor;
#ifdef __CLIENT
		SetTexture();
#endif	// __CLIENT
	}
}

int CLEComponent::Decrement( void )
{
	if( nTick > 0 )
		nTick--;
	return nTick;
}

#ifdef __CLIENT
void CLEComponent::SetTexture( void )
{
	m_pTexture	= CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ICON, "Skill_TroGiftbox02.dds" ), 0xffff00ff );
}
#endif	// __CLIENT

////////////////////////////////////////////////////////////////////////////////
ILordEvent::ILordEvent( CLord* pLord )
:
m_pLord( pLord )
{
}

ILordEvent::~ILordEvent()
{
	Clear();
}

void ILordEvent::Clear( void )
{
	for( VLEC::iterator i = m_vComponents.begin(); i != m_vComponents.end(); ++i )
		safe_delete( *i );
	m_vComponents.clear();
}

BOOL ILordEvent::Initialize( const char* szFile )
{
	CScript s;
	if( s.Load( szFile ) == FALSE )
		return FALSE;

	s.GetToken();
	while( s.tok != FINISHED )
	{
		if( s.Token == _T( "exp" ) )
		{
			s.GetToken();	// {
			float fRate	= s.GetFloat();
			while( *s.token != '}' )
			{
				__int64 iCost	= s.GetInt64();
				m_vEFactor.push_back( make_pair( fRate, iCost ) );
				fRate	= s.GetFloat();
			}
		}
		else if( s.Token == _T( "item" ) )
		{
			s.GetToken();	// {
			float fRate	= s.GetFloat();
			while( *s.token != '}' )
			{
				__int64 iCost	= s.GetInt64();
				m_vIFactor.push_back( make_pair( fRate, iCost ) );
				fRate	= s.GetFloat();
			}
		}
		s.GetToken();
	}
	return TRUE;
}

__int64 ILordEvent::GetCost( int iEEvent, int iIEvent )
{
	if( iEEvent == 0 && iIEvent == 0 )
		return 0;
	if( iEEvent >= (int)( m_vEFactor.size() ) || iIEvent >= (int)( m_vIFactor.size() ) )
		return 0;
	return m_vEFactor[iEEvent].second + m_vIFactor[iIEvent].second;
}

float ILordEvent::GetEFactor( int iEEvent )
{
	if( iEEvent >= (int)( m_vEFactor.size() ) )
		return 1.0F;
	// second로 되있이서 수정했습니다~ 
	return m_vEFactor[iEEvent].first;
}

float ILordEvent::GetIFactor( int iIEvent )
{
	if( iIEvent >= (int)( m_vIFactor.size() ) )
		return 1.0F;
	return m_vIFactor[iIEvent].first;
}

void ILordEvent::Initialize( void )
{
	if( DoTestInitialize() )
		Clear();
}

void ILordEvent::AddComponent( u_long idPlayer, int iEEvent, int iIEvent )
{
	if( iEEvent == 0 && iIEvent == 0 )
	{
#ifdef __DBSERVER
		Error( "iEEvent == 0 && iIEvent == 0" );
#endif	// __DBSERVER
		return;
	}
	if( iEEvent >= (int)( m_vEFactor.size() ) || iIEvent >= (int)( m_vIFactor.size() ) )
	{
#ifdef __DBSERVER
		Error( "iEEvent >= m_vEFactor.size() || iIEvent >= m_vIFactor.size()" );
#endif	// __DBSERVER
		return;
	}
	CLEComponent* pComponent
		= new CLEComponent( eTick, idPlayer, m_vEFactor[iEEvent].first, m_vIFactor[iIEvent].first );
	AddComponent( pComponent );
}

void ILordEvent::AddComponent( CLEComponent* pComponent, BOOL bHook )
{
	if( !bHook || DoTestAddComponent( pComponent ) )
	{
		m_vComponents.push_back( pComponent );
	}
	else
		safe_delete( pComponent );
}

VLEC::iterator ILordEvent::Remove( VLEC::iterator i )
{
	return m_vComponents.erase( i );
}

CLEComponent* ILordEvent::GetComponent( u_long idPlayer )
{
	for( VLEC::iterator i = m_vComponents.begin(); i != m_vComponents.end(); ++i )
	{
		if( ( *i )->GetIdPlayer() == idPlayer )
			return( *i );
	}
	return NULL;
}

CLEComponent* ILordEvent::GetComponentAt( int i )
{
	ASSERT( i >= 0 && i < GetComponentSize() );
	return m_vComponents[i];
}

float ILordEvent::GetEFactor()
{
	float fExpFactor	= 1.0F;
	for( VLEC::iterator i = m_vComponents.begin(); i != m_vComponents.end(); ++i )
		fExpFactor	+= ( *i )->GetEFactor();
	return fExpFactor;
}

float ILordEvent::GetIFactor()
{
	float fItemFactor		= 1.0F;
	for( VLEC::iterator i = m_vComponents.begin(); i != m_vComponents.end(); ++i )
		fItemFactor		+= ( *i )->GetIFactor();
	return fItemFactor;
}

void ILordEvent::Serialize( CAr & ar )
{
	if( ar.IsStoring() )
	{
		ar << m_vComponents.size();
		for( VLEC::iterator i = m_vComponents.begin(); i != m_vComponents.end(); ++i )
			( *i )->Serialize( ar );
	}
	else
	{
		Clear();
		size_t nSize;
		ar >> nSize;
		for( size_t i = 0; i < nSize; i++ )
		{
			CLEComponent* pComponent	= new CLEComponent;
			pComponent->Serialize( ar );
			AddComponent( pComponent, FALSE );
		}
	}
}

void ILordEvent::SerializeTick( CAr & ar )
{
	if( ar.IsStoring() )
	{
		ar << m_vComponents.size();
		for( VLEC::iterator i = m_vComponents.begin(); i != m_vComponents.end(); ++i )
		{
			ar << ( *i )->GetIdPlayer();
			ar << ( *i )->GetTick();
		}
	}
	else
	{
		size_t nSize;
		ar >> nSize;
		for( size_t i = 0; i < nSize; i++ )
		{
			u_long idPlayer;
			int nTick;
			ar >> idPlayer >> nTick;
			CLEComponent* pComponent	= GetComponent( idPlayer );
			if( pComponent )
				pComponent->SetTick( nTick );
		}
	}
}

void ILordEvent::EraseExpiredComponents( void )
{
	for( VLEC::iterator i = m_vComponents.begin(); i != m_vComponents.end(); )
	{
		if( ( *i )->GetTick() == 0 )
		{
			safe_delete( *i );
			i	= m_vComponents.erase( i );
		}
		else	++i;
	}
}
////////////////////////////////////////////////////////////////////////////////
namespace	election
{
	void OutputDebugString( LPCTSTR lpszFormat, ... )
	{
		char lpString[1024]		= { 0,};
		va_list	args;
		va_start( args, lpszFormat );
		int n	= _vsntprintf( lpString, 1024, lpszFormat, args );
		va_end( args );
		if( n > 0 )
			::OutputDebugString( lpString );
	}
};

#endif	// __LORD