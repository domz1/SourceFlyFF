
#include "stdafx.h"

#ifndef __RT_1025

#include "messenger.h"

#ifdef __CORESERVER
#include "dpcoresrvr.h"
extern	CDPCoreSrvr g_dpCoreSrvr;
#include "player.h"
extern	CPlayerMng	g_ONLINEerMng;
#endif // __CORESERVER

CMessenger::CMessenger()
{
	m_dwMyState = FRS_ONLINE;
}

CMessenger::~CMessenger()
{
	Clear();
}
LPFRIEND CMessenger::GetAt( int nIndex )
{
	int count = 0;
	for( C2FriendPtr::iterator i = m_aFriend.begin() ; i != m_aFriend.end() ; i++, count++ )
	{
		LPFRIEND lpFriend = (LPFRIEND)i->second;
		if( count == nIndex )
			return lpFriend;
	}
	return NULL;
}

#if __VER >= 11 // __SYS_PLAYER_DATA
BOOL CMessenger::AddFriend( u_long uidPlayer )
{
	if( FALSE == IsFriend( uidPlayer ) )
	{
		LPFRIEND pFrined	= new FRIEND;
		pFrined->dwUserId = uidPlayer;
#ifdef __WORLDSERVER
		pFrined->bSave	= TRUE;
#endif // __WORLDSERVER
		m_aFriend.insert( C2FriendPtr::value_type( uidPlayer, pFrined ) );
		return TRUE;
	}
	return FALSE;
}

BOOL CMessenger::AddFriend( u_long uidPlayer, DWORD dwSate, BOOL bSave )
{
	if( FALSE == IsFriend( uidPlayer ) )
	{
		LPFRIEND pFrined	= new FRIEND;
		pFrined->dwUserId = uidPlayer;
		pFrined->dwState = dwSate;
		pFrined->bSave	= bSave;
#ifdef __WORLDSERVER
		pFrined->bSave	= TRUE;
#endif // __WORLDSERVER
		m_aFriend.insert( C2FriendPtr::value_type( uidPlayer, pFrined ) );
		return TRUE;
	}
	return FALSE;
}
#else	// __SYS_PLAYER_DATA
BOOL CMessenger::AddFriend( u_long uidPlayer, LONG nJob, BYTE nSex )
{
	if( FALSE == IsFriend( uidPlayer ) )
	{
		LPFRIEND pFrined	= new FRIEND;
		pFrined->dwUserId = uidPlayer;
#if __VER < 11 // __SYS_PLAYER_DATA
		pFrined->nSex = nSex;
		pFrined->nJob = nJob;
#endif	// __SYS_PLAYER_DATA
#ifdef __WORLDSERVER
		pFrined->bSave = TRUE;
#endif // __WORLDSERVER
		m_aFriend.insert( C2FriendPtr::value_type( uidPlayer, pFrined ) );
		return TRUE;
	}
	return FALSE;
}

BOOL CMessenger::AddFriend( u_long uidPlayer, LONG nJob, BYTE nSex, DWORD dwSate, BOOL bSave )
{
	if( FALSE == IsFriend( uidPlayer ) )
	{
		LPFRIEND pFrined	= new FRIEND;
		pFrined->dwUserId = uidPlayer;
#if __VER < 11 // __SYS_PLAYER_DATA
		pFrined->nSex = nSex;
		pFrined->nJob = nJob;
#endif	// __SYS_PLAYER_DATA
		pFrined->dwState = dwSate;
		pFrined->bSave	= bSave;
#ifdef __WORLDSERVER
		pFrined->bSave = TRUE;
#endif // __WORLDSERVER
		m_aFriend.insert( C2FriendPtr::value_type( uidPlayer, pFrined ) );
		return TRUE;
	}
	return FALSE;
}

BOOL CMessenger::AddFriend( u_long uidPlayer, LONG nJob, BYTE nSex, LPSTR lpName )
{
	if( FALSE == IsFriend( uidPlayer ) )
	{
		LPFRIEND pFrined	= new FRIEND;
		pFrined->dwUserId = uidPlayer;
#if __VER < 11 // __SYS_PLAYER_DATA
		pFrined->nSex = nSex;
		pFrined->nJob = nJob;
#endif	// __SYS_PLAYER_DATA
#ifdef __WORLDSERVER
		pFrined->bSave = TRUE;
#endif // __WORLDSERVER
		strcpy( pFrined->szName, lpName );
		m_aFriend.insert( C2FriendPtr::value_type( uidPlayer, pFrined ) );
		return TRUE;
	}
	return FALSE;
}
#endif	// __SYS_PLAYER_DATA

int  CMessenger::GetSize( void )
{
	return m_aFriend.size();
}

BOOL CMessenger::AddDefferntFriend( u_long uidPlayer )
{
	if( FALSE == IsDefferntFriend( uidPlayer ) )
	{
		LPFRIEND pFrined	= new FRIEND;
		pFrined->dwUserId = uidPlayer;
		m_adifferntFriend.insert( C2FriendPtr::value_type( uidPlayer, pFrined ) );
		return TRUE;
	}
	return FALSE;
}

CMessenger& CMessenger::operator =( CMessenger & rMessenger )
{
	Clear();
	for( C2FriendPtr::iterator i	= rMessenger.m_aFriend.begin(); i != rMessenger.m_aFriend.end(); ++i )
#if __VER >= 11 // __SYS_PLAYER_DATA
		AddFriend( i->second->dwUserId, i->second->dwState, i->second->bSave );
#else	// __SYS_PLAYER_DATA
		AddFriend( i->second->dwUserId, i->second->nJob, i->second->nSex, i->second->dwState, i->second->bSave );
#endif	// __SYS_PLAYER_DATA
	for( C2FriendPtr::iterator j	= rMessenger.m_adifferntFriend.begin(); j != rMessenger.m_adifferntFriend.end(); ++j )
		AddDefferntFriend( j->second->dwUserId );
	m_dwMyState	= rMessenger.m_dwMyState;

	return *this;
}

void CMessenger::Clear( void )
{
	for( C2FriendPtr::iterator i	= m_aFriend.begin(); i != m_aFriend.end(); ++i )
		safe_delete( i->second );
	
	m_aFriend.clear();

	for( C2FriendPtr::iterator j	= m_adifferntFriend.begin(); j != m_adifferntFriend.end(); ++j )
		safe_delete( j->second );
	
	m_adifferntFriend.clear();
}

BOOL CMessenger::RemoveFriend( u_long uidPlayer )
{
	LPFRIEND pFriend;
	pFriend = GetFriend( uidPlayer );
	if( pFriend )
	{
		safe_delete( pFriend );
		m_aFriend.erase( uidPlayer );
		return TRUE;
	}	
	return FALSE;
}

BOOL CMessenger::RemoveDifferntFriend( u_long uidPlayer )
{
	LPFRIEND pFriend;
	pFriend = GetDefferntFriend( uidPlayer );
	if( pFriend )
	{
		safe_delete( pFriend );
		m_adifferntFriend.erase( uidPlayer );
		return TRUE;
	}
	return FALSE;
}

LPFRIEND CMessenger::GetFriend( u_long uidPlayer )
{
	C2FriendPtr::iterator i		= m_aFriend.find( uidPlayer );
	if( i != m_aFriend.end() )
		return i->second;
	return NULL;
}

LPFRIEND CMessenger::GetDefferntFriend( u_long uidPlayer )
{
	C2FriendPtr::iterator i		= m_adifferntFriend.find( uidPlayer );
	if( i != m_adifferntFriend.end() )
		return i->second;
	return NULL;
}

BOOL CMessenger::IsFriend( u_long uidPlayer )
{
	C2FriendPtr::iterator i = m_aFriend.find( uidPlayer );
	if( i != m_aFriend.end() )
		return TRUE;
	return FALSE;
}

BOOL CMessenger::IsDefferntFriend( u_long uidPlayer )
{
	C2FriendPtr::iterator i = m_adifferntFriend.find( uidPlayer );
	if( i != m_adifferntFriend.end() )
		return TRUE;
	return FALSE;
}

void CMessenger::Serialize( CAr & ar )
{
	if( ar.IsStoring() )
	{
		int nCount	= 0;
		u_long uOffset	= ar.GetOffset();
		ar << nCount;
		for( C2FriendPtr::iterator i	= m_aFriend.begin(); i != m_aFriend.end(); ++i )
		{
			LPFRIEND pFriend = (LPFRIEND)i->second;
			ar << pFriend->bGroup;
			ar << pFriend->dwUserId;
#if __VER < 11 // __SYS_PLAYER_DATA
			ar << pFriend->nSex;
			ar << pFriend->nJob;
#endif	// __SYS_PLAYER_DATA
			ar << pFriend->bSave;
			if( pFriend->dwState == FRS_BLOCK )
				ar << (BYTE)1;
			else
				ar << (BYTE)0;
			nCount++;
		}
		int nBufSize1;
		LPBYTE lpBuf1	= ar.GetBuffer( &nBufSize1 );
		*(UNALIGNED int*)( lpBuf1 + uOffset )	= nCount;


		nCount	= 0;
		uOffset	= ar.GetOffset();
		ar << nCount;
		for( i	= m_adifferntFriend.begin(); i != m_adifferntFriend.end(); ++i )
		{
			LPFRIEND pFriend = (LPFRIEND)i->second;
			ar << pFriend->bGroup;
			ar << pFriend->dwUserId;
			nCount++;
		}

		ar << m_dwMyState;

		lpBuf1	= ar.GetBuffer( &nBufSize1 );
		*(UNALIGNED int*)( lpBuf1 + uOffset )	= nCount;
	}
	else
	{
		Clear();

		int nCount = 0;
		ar >> nCount;
		for( int i = 0 ; i < nCount ; i++ )
		{
			LPFRIEND pFriend = new FRIEND;
			ar >> pFriend->bGroup;
			ar >> pFriend->dwUserId;
#if __VER < 11 // __SYS_PLAYER_DATA
			ar >> pFriend->nSex;
			ar >> pFriend->nJob;
#endif	// __SYS_PLAYER_DATA
			ar >> pFriend->bSave;
			BYTE bState;
			ar >> bState;
			if( bState == 1 )
				pFriend->dwState = FRS_BLOCK;
			m_aFriend.insert( C2FriendPtr::value_type( pFriend->dwUserId, pFriend ) );
		}

		ar >> nCount;

		for( i = 0 ; i < nCount ; i++ )
		{
			LPFRIEND pFriend = new FRIEND;
			ar >> pFriend->bGroup;
			ar >> pFriend->dwUserId;
			m_adifferntFriend.insert( C2FriendPtr::value_type( pFriend->dwUserId, pFriend ) );
		}

		ar >> m_dwMyState;
	}
}

void CMessenger::SetSave( void )
{
	for( C2FriendPtr::iterator i	= m_aFriend.begin(); i != m_aFriend.end(); ++i )
	{
		LPFRIEND pFriend = (LPFRIEND)i->second;
		pFriend->bSave = FALSE;
	}
}

#endif	// __RT_1025