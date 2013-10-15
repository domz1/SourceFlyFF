//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Campus.h"

#if __VER >= 15 // __CAMPUS
#ifdef __WORLDSERVER
#include "User.h"
extern CUserMng			g_UserMng;
#endif // __WORLDSERVER

//////////////////////////////////////////////////////////////////////
// CCampusMember Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCampusMember::CCampusMember()
: m_idPlayer( 0 ), m_nMemberLv( 0 )
{

}

CCampusMember::~CCampusMember()
{

}

void CCampusMember::Serialize( CAr & ar )
{
	if( ar.IsStoring() )
		ar << m_idPlayer << m_nMemberLv;
	else
		ar >> m_idPlayer >> m_nMemberLv;
}

//////////////////////////////////////////////////////////////////////
// CCampus Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCampus::CCampus()
: m_idCampus( 0 ), m_idMaster( 0 )
{

}

CCampus::~CCampus()
{
	Clear();
}

void CCampus::Clear()
{
	for( MAP_CM::iterator it = m_mapCM.begin(); it != m_mapCM.end(); ++it )
		SAFE_DELETE( it->second );

	m_mapCM.clear();
}

void CCampus::Serialize( CAr & ar )
{
	if( ar.IsStoring() )
	{
		ar << m_idCampus << m_idMaster << m_mapCM.size();
		for( MAP_CM::iterator it = m_mapCM.begin(); it != m_mapCM.end(); ++it )
			( it->second )->Serialize( ar );
	}
	else
	{
		Clear();
		size_t nSize;
		ar >> m_idCampus >> m_idMaster >> nSize;
		for( int i = 0; i < (int)( nSize ); ++i )
		{
			CCampusMember* pMember = new CCampusMember;
			pMember->Serialize( ar );
			m_mapCM.insert( MAP_CM::value_type( pMember->GetPlayerId(), pMember ) );
		}
	}
}

BOOL CCampus::IsPupil( u_long idPlayer )
{
	if( GetMemberLv( idPlayer ) == CAMPUS_PUPIL )
		return TRUE;

	return FALSE;
}

vector<u_long> CCampus::GetPupilPlayerId()
{
	vector<u_long> vecPupil;
	for( MAP_CM::iterator it = m_mapCM.begin(); it != m_mapCM.end(); ++it )
	{
		if( (it->second)->GetLevel() == CAMPUS_PUPIL )
			vecPupil.push_back( (it->second)->GetPlayerId() );
	}
	return vecPupil;
}

int CCampus::GetPupilNum()
{
	int nPupil = 0;
	for( MAP_CM::iterator it = m_mapCM.begin(); it != m_mapCM.end(); ++it )
	{
		if( ( it->second )->GetLevel() == CAMPUS_PUPIL )
			++nPupil;
	}
	return nPupil;
}

vector<u_long> CCampus::GetAllMemberPlayerId()
{
	vector<u_long> vecMember;
	for( MAP_CM::iterator it = m_mapCM.begin(); it != m_mapCM.end(); ++ it )
		vecMember.push_back( (it->second)->GetPlayerId() );
	return vecMember;
}

int CCampus::GetMemberLv( u_long idPlayer )
{
	CCampusMember* pMember = GetMember( idPlayer );
	if( pMember )
		return pMember->GetLevel();
	
	return 0;
}

BOOL CCampus::IsMember( u_long idPlayer )
{
	CCampusMember* pCM = GetMember( idPlayer );
	if( pCM )
		return TRUE;
	
	return FALSE;
}

BOOL CCampus::AddMember( CCampusMember* pMember )
{
	if( GetMember( pMember->GetPlayerId() ) )
	{
		Error( "Pupil is already campus member - idCampus : %d, idPlayer : %d", GetCampusId(), pMember->GetPlayerId() );
		return FALSE;
	}
	if( GetPupilNum() >= MAX_PUPIL_NUM )
	{
		Error( "Pupil is full - idCampus : %d", GetCampusId() );
		return FALSE;
	}
	m_mapCM.insert( MAP_CM::value_type( pMember->GetPlayerId(), pMember ) );
	return TRUE;
}

BOOL CCampus::RemoveMember( u_long idPlayer )
{
	CCampusMember* pMember	= GetMember( idPlayer );
	if( pMember )
	{
		SAFE_DELETE( pMember );
		m_mapCM.erase( idPlayer );
		return TRUE;
	}
	Error( "Member not found - idCampus : %d, idPlayer : %d", GetCampusId(), idPlayer );
	return FALSE;
}

CCampusMember* CCampus::GetMember( u_long idPlayer )
{
	MAP_CM::iterator it = m_mapCM.find( idPlayer );
	if( it != m_mapCM.end() )
		return it->second;
	
	return NULL;
}

#ifdef __WORLDSERVER
BOOL CCampus::IsChangeBuffLevel( u_long idPlayer )
{
	if( IsMaster( idPlayer ) )
	{
		int nLevel = GetBuffLevel( idPlayer );
		if( m_nPreBuffLevel != nLevel )
		{
			m_nPreBuffLevel = nLevel;
			return TRUE;
		}
	}
	return FALSE;
}

int CCampus::GetBuffLevel( u_long idPlayer )
{
	int nLevel = 0;
	if( IsMaster( idPlayer ) )
	{
		for( MAP_CM::iterator it = m_mapCM.begin(); it != m_mapCM.end(); ++it )
		{
			CUser* pPupil = g_UserMng.GetUserByPlayerID( ( it->second )->GetPlayerId() );
			if( IsValidObj( pPupil ) && ( it->second )->GetLevel() == CAMPUS_PUPIL )
				++nLevel;
		}
	}
	else
	{
		CUser* pMaster = g_UserMng.GetUserByPlayerID( GetMaster() );
		if( IsValidObj( pMaster ) )
			++nLevel;
	}
	return nLevel;
}
#endif // __WORLDSERVER

//////////////////////////////////////////////////////////////////////
// CCampus Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCampusMng::CCampusMng()
:m_idCampus( 0 )
{

}

CCampusMng::~CCampusMng()
{
	Clear();
}

void CCampusMng::Clear()
{
	for( MAP_CAMPUS::iterator it = m_mapCampus.begin(); it != m_mapCampus.end(); ++it )
		SAFE_DELETE( it->second );
	
	m_mapCampus.clear();
	m_mapPid2Cid.clear();
}

void CCampusMng::Serialize( CAr & ar )
{
	int	i = 0;
	
	if( ar.IsStoring() )
	{
		ar << m_idCampus << m_mapCampus.size();
		for( MAP_CAMPUS::iterator it = m_mapCampus.begin(); it != m_mapCampus.end(); ++it )
			( it->second )->Serialize( ar );

		ar << m_mapPid2Cid.size();
		for( MAP_PID2CID::iterator it2 = m_mapPid2Cid.begin(); it2 != m_mapPid2Cid.end(); ++it2 )
			ar << it2->first << it2->second;

	}
	else
	{
		Clear();
		size_t nSize;
		ar >> m_idCampus >> nSize;
		for( i = 0; i < (int)( nSize ); ++i )
		{
			CCampus* pCampus = new CCampus;
			pCampus->Serialize( ar );
			m_mapCampus.insert( MAP_CAMPUS::value_type( pCampus->GetCampusId(), pCampus ) );
		}
		ar >> nSize;
		for( i = 0; i < (int)( nSize ); ++i )
		{
			u_long idPlayer, idCampus;
			ar >> idPlayer >> idCampus;
			AddPlayerId2CampusId( idPlayer, idCampus );
		}
	}
}

u_long CCampusMng::AddCampus( CCampus* pCampus )
{
	m_idCampus	= ( pCampus->GetCampusId() != 0? pCampus->GetCampusId(): m_idCampus + 1 );
	if( GetCampus( m_idCampus ) )
		return 0;
	pCampus->SetCampusId( m_idCampus );
	m_mapCampus.insert( MAP_CAMPUS::value_type( m_idCampus, pCampus ) );
	return m_idCampus;
}

BOOL CCampusMng::RemoveCampus( u_long idCampus )
{
	CCampus* pCampus = GetCampus( idCampus );
	if( pCampus )
	{
		SAFE_DELETE( pCampus );
		m_mapCampus.erase( idCampus );
		return TRUE;
	}
	return FALSE;
}

CCampus* CCampusMng::GetCampus( u_long idCampus )
{
	MAP_CAMPUS::iterator it	= m_mapCampus.find( idCampus );
	if( it != m_mapCampus.end() )
		return it->second;
	
	return NULL;
}

bool CCampusMng::AddPlayerId2CampusId( u_long idPlayer, u_long idCampus )
{
	bool bResult = m_mapPid2Cid.insert( MAP_PID2CID::value_type( idPlayer, idCampus ) ).second;
	return bResult;
}

u_long CCampusMng::GetCampusIdByPlayerId( u_long idPlayer )
{
	MAP_PID2CID::iterator it	= m_mapPid2Cid.find( idPlayer );
	if( it != m_mapPid2Cid.end() )
		return it->second;
	
	return NULL;
}
#endif // __CAMPUS