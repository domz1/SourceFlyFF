#ifndef __CHATTING_H__
#define	__CHATTING_H__

#include "mempooler.h"
#include <map>

#define MAX_CHATTINGMEMBER	512

class CChatting
{
	TCHAR	m_sChatting[33];					// ChattingRoom Name
	u_long	m_idChatting;
public:
	int		m_nSizeofMember;
	u_long	m_idMember[MAX_CHATTINGMEMBER];		// ChattingMember
#ifdef __CLIENT
	TCHAR	m_szName[MAX_CHATTINGMEMBER][MAX_NAME];
#endif // __CLIENT
	BOOL	m_bState;
	//	Constructions
	CChatting();
	~CChatting();

#ifdef __CLIENT
	BOOL	AddChattingMember( u_long uidPlayer, char * szName );
#else // __CLIENT
	BOOL	AddChattingMember( u_long uidPlayer );
#endif // __CLIENT
	BOOL	RemoveChattingMember( u_long uidPlayer );

	BOOL	IsChattingMember( u_long uidPlayer ) { return( FindChattingMember( uidPlayer ) >= 0 ); }
	int		FindChattingMember( u_long uidPlayer );

	void	ClearMember( void );
	
	void	Serialize( CAr & ar );
	int		GetChattingMember() { return m_nSizeofMember; }
public:
#ifndef __VM_0820
#ifndef __MEM_TRACE
	static	MemPooler<CChatting>*	m_pPool;
	void*	operator new( size_t nSize )	{	return CChatting::m_pPool->Alloc();	}
	void*	operator new( size_t nSize, LPCSTR lpszFileName, int nLine )	{	return CChatting::m_pPool->Alloc();	}
	void	operator delete( void* lpMem )	{	CChatting::m_pPool->Free( (CChatting*)lpMem );	}
	void	operator delete( void* lpMem, LPCSTR lpszFileName, int nLine )	{	CChatting::m_pPool->Free( (CChatting*)lpMem );	}
#endif	// __MEM_TRACE
#endif	// __VM_0820
};

using	namespace	std;
typedef	map< u_long, CChatting*>	C2CharttingPtr;

class CChattingMng
{
	C2CharttingPtr	m_2ChatPtr;
public:
	CChattingMng();
	~CChattingMng();

	void	Clear( void );
	u_long	NewChattingRoom( u_long uChattingId );
	BOOL	DeleteChattingRoom( u_long uChattingId );
	CChatting* GetChttingRoom( u_long uidChtting );
};

#endif	//	_CHATTING_H
