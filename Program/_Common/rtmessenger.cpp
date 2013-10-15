#include "stdafx.h"
#include "rtmessenger.h"

CRTMessenger::CRTMessenger()
{
	m_dwState	= 0;
}

CRTMessenger::~CRTMessenger()
{
	clear();
}

void	CRTMessenger::SetFriend( u_long idFriend, Friend * pFriend )
{
	if( idFriend == 0 )
		return;
	Friend f;	//
	if( !pFriend )
		pFriend		= &f;
	map<u_long, Friend>::iterator i	= find( idFriend );
	if( i != end() )
		memcpy( &i->second, pFriend, sizeof(Friend) );
	else
		insert( map<u_long, Friend>::value_type( idFriend, *pFriend ) );
}

Friend*	CRTMessenger::GetFriend( u_long idFriend )
{
	map<u_long, Friend>::iterator i		= find( idFriend );
	if( i != end() )
		return &i->second;
	return NULL;
}

int CRTMessenger::Serialize( CAr & ar )
{
	if( ar.IsStoring() )
	{
		ar << m_dwState;
		ar << static_cast<int>( size() );
		for( map<u_long, Friend>::iterator i = begin(); i != end(); ++i )
		{
			ar << i->first;
			ar.Write( &i->second, sizeof(Friend) );
		}
	}
	else
	{
		clear();
		ar >> m_dwState;
		int nSize;
		ar >> nSize;

		if( nSize > MAX_FRIEND )
			return nSize;

		u_long idFriend;

		for( int i = 0; i < nSize; i++ )
		{
			ar >> idFriend;
			Friend f;
			ar.Read( &f, sizeof(Friend) );
			SetFriend( idFriend, &f );
		}
	}
	return 0;
}

CRTMessenger &	CRTMessenger::operator =( CRTMessenger & rRTMessenger )
{
	clear();
	for( map<u_long, Friend>::iterator i = rRTMessenger.begin(); i != rRTMessenger.end(); ++i )
		SetFriend( i->first, &i->second );
	m_dwState	= rRTMessenger.GetState();
	return *this;
}

void	CRTMessenger::SetBlock( u_long idFriend, BOOL bBlock )
{
	Friend* pFriend		= GetFriend( idFriend );
	if( pFriend )
		pFriend->bBlock		= bBlock;
}

BOOL	CRTMessenger::IsBlock( u_long idFriend )
{
	Friend* pFriend		= GetFriend( idFriend );
	if( pFriend )
		return pFriend->bBlock;
	return TRUE;
}
