#include "stdafx.h"
#include "Chatting.h"

#ifndef __VM_0820
#ifndef __MEM_TRACE
	#ifdef __VM_0819
	MemPooler<CChatting>*	CChatting::m_pPool		= new MemPooler<CChatting>( 512, "CChatting" );
	#else	// __VM_0819
	MemPooler<CChatting>*	CChatting::m_pPool		= new MemPooler<CChatting>( 512 );
	#endif	// __VM_0819
#endif	// __MEM_TRACE
#endif	// __VM_0820

CChatting::CChatting()
{
	ZeroMemory( m_sChatting, sizeof( m_sChatting ) );
	ZeroMemory( m_idMember, sizeof( u_long ) * MAX_CHATTINGMEMBER );
	m_nSizeofMember = 0;
	m_idChatting = 0;
#ifdef __CLIENT
	ZeroMemory( m_szName, sizeof( m_szName ) );
#endif // __CLIENT
}

CChatting::~CChatting()
{

}

#ifdef __CLIENT
BOOL CChatting::AddChattingMember( u_long uidPlayer, char * szName )
{
	if( IsChattingMember( uidPlayer ) == FALSE && m_nSizeofMember < MAX_CHATTINGMEMBER )
	{
		m_idMember[m_nSizeofMember] = uidPlayer;
		strcpy( m_szName[m_nSizeofMember], szName );
		m_nSizeofMember++;
		return TRUE;
	}
	return FALSE;
}
#else // __CLIENT
BOOL CChatting::AddChattingMember( u_long uidPlayer )
{
	if( IsChattingMember( uidPlayer ) == FALSE && m_nSizeofMember < MAX_CHATTINGMEMBER )
	{
		m_idMember[m_nSizeofMember] = uidPlayer;
		m_nSizeofMember++;
		return TRUE;
	}
	return FALSE;
}
#endif // __CLIENT

#ifdef __CLIENT
BOOL CChatting::RemoveChattingMember( u_long uidPlayer )
{
	int nFindMember = FindChattingMember( uidPlayer );
	if( nFindMember >= 0 )
	{
		for( int i = nFindMember ; i < m_nSizeofMember-1 ; ++i )
		{
			m_idMember[i] = m_idMember[i+1];
			strcpy( m_szName[i], m_szName[i+1] );
		}
		m_nSizeofMember--;
		return TRUE;
	}
	return FALSE;
}
#else // __CLIENT
BOOL CChatting::RemoveChattingMember( u_long uidPlayer )
{
	int nFindMember = FindChattingMember( uidPlayer );
	if( nFindMember >= 0 )
	{
		for( int i = nFindMember ; i < m_nSizeofMember-1 ; ++i )
		{
			m_idMember[i] = m_idMember[i+1];
		}
		m_nSizeofMember--;
		return TRUE;
	}
	return FALSE;
}
#endif // __CLIENT

int CChatting::FindChattingMember( u_long uidPlayer )
{
	for( int i = 0 ; i < m_nSizeofMember ; ++i )
	{
		if( m_idMember[i] == uidPlayer )
		{
			return i;
		}
	}
	return -1;
}

void CChatting::ClearMember()
{
	ZeroMemory( m_sChatting, sizeof( m_sChatting ) );
	ZeroMemory( m_idMember, sizeof( u_long ) * MAX_CHATTINGMEMBER );
	m_nSizeofMember = 0;
#ifdef __CLIENT
	ZeroMemory( m_szName, sizeof( m_szName ) );
#endif // __CLIENT
}

void CChatting::Serialize( CAr & ar )
{
	if( ar.IsStoring() )
	{
	}
	else
	{

	}
}

CChatting g_Chatting;













//////////////////////////////////////////////////////////////////////////////////////////////////
//		ChattingMng
//////////////////////////////////////////////////////////////////////////////////////////////////
CChattingMng::CChattingMng()
{
}

CChattingMng::~CChattingMng()
{
	Clear();
}

void CChattingMng::Clear( void )
{
	for( C2CharttingPtr::iterator i	= m_2ChatPtr.begin(); i != m_2ChatPtr.end(); ++i )
		safe_delete( i->second );
	
	m_2ChatPtr.clear();
}

u_long CChattingMng::NewChattingRoom( u_long uChattingId )
{
	CChatting* pChatting = GetChttingRoom( uChattingId );
	if( pChatting )
	{
		pChatting->ClearMember();		
	}
	else
	{
		CChatting* pChatting = new CChatting;
		m_2ChatPtr.insert( C2CharttingPtr::value_type( uChattingId, pChatting ) );
	}
	return uChattingId;
}

BOOL CChattingMng::DeleteChattingRoom( u_long uChattingId )
{
	CChatting* pChatting = GetChttingRoom( uChattingId );
	if( pChatting )
	{
		safe_delete( pChatting );
		m_2ChatPtr.erase( uChattingId );
		return TRUE;
	}
	return FALSE;
}

CChatting* CChattingMng::GetChttingRoom( u_long uidChtting )
{
	C2CharttingPtr::iterator i = m_2ChatPtr.find( uidChtting );
	if( i != m_2ChatPtr.end() )
		return i->second;
	return NULL;
}

CChattingMng	g_ChattingMng;

