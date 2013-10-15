#ifndef __RT_MESSENGER_H__
#define	__RT_MESSENGER_H__

#include "messenger.h"

typedef struct	_Friend
{
	BOOL	bBlock;
	DWORD	dwState;
	_Friend()
	{
		bBlock	= FALSE;
		dwState		= 0;
	}
}	Friend;

class CRTMessenger	: public	map<u_long, Friend>
{
public:
	CRTMessenger();
	virtual	~CRTMessenger();
//
//	void	SetFriend( u_long idFriend, BOOL bBlock, DWORD dwState );
	void	SetFriend( u_long idFriend, Friend * pFriend );
	void	RemoveFriend( u_long idFriend )		{	erase( idFriend );	}
	Friend*	GetFriend( u_long idFriend );

	void	SetBlock( u_long idFriend, BOOL bBlock );
	BOOL	IsBlock( u_long idFriend );

	DWORD	GetState( void )	{	return m_dwState;	}
	void	SetState( DWORD dwState )	{	m_dwState	= dwState;	}

//	void	Serialize( CAr & ar );
	int	Serialize( CAr & ar );

	virtual	CRTMessenger &	CRTMessenger::operator =( CRTMessenger & rRTMessenger );
private:
	DWORD	m_dwState;
};

#endif	// __RT_MESSENGER_H__
