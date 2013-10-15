#ifndef __ACTION_H
#define __ACTION_H

#include "MoverMsg.h"

/////////////////////////////////////////////////////////////////////////////
//
//  Mover action msg / state

#define	MAX_ACTMSG	0xff

// 무버의 동작상태를 기술
enum ACTTYPE
{
	ACT_NONE,

	ACT_STOP,				// 대기
	ACT_STOPWALK,			// 제자리걷기
	ACT_FORWARD,			// 전진
	ACT_BACKWARD,			// 후진
	ACT_LEFT,				// 왼쪽으로이동
	ACT_RIGHT,				// 오른쪽으로 이동

	ACT_WALKMODE,			// 걷기/뛰기모드

	ACT_LEFTTURN,			// 왼족으로 도는중
	ACT_RIGHTTURN,			// 오른쪽으로 도는중

	ACT_FJUMPREADY,
	ACT_FJUMP,				// 앞으로 점프중
	ACT_FLAND,
	ACT_SJUMPREADY,
	ACT_SJUMP,				// 제자리 점프중
	ACT_SLAND,
	ACT_BJUMPREADY,
	ACT_BJUMP,				// 백 점프중
	ACT_BLAND,
	ACT_LJUMPREADY,
	ACT_LJUMP,				// 왼족 점프중
	ACT_LLAND,
	ACT_RJUMPREADY,
	ACT_RJUMP,				// 오른쪽 점프중
	ACT_RLAND,

	ACT_ATTACKMODE,			// 전투모드
	ACT_ATTACK1,			// 공격동작 1
	ACT_ATTACK2,
	ACT_ATTACK3,
	ACT_ATTACK4,

	ACT_RANGE1,				// 공격동작 1
	ACT_RANGE2,
	ACT_RANGE3,
	ACT_RANGE4,

	ACT_DAMAGE,				// 피격중
	ACT_DIE					// 죽어있는 중(?)
};

class CAction;
typedef struct tagACTMSG
{
	DWORD	dwMsg;
	int		nParam1;
	int		nParam2;
	int		nParam3;
	tagACTMSG() : dwMsg( 0 ), nParam1( 0 ), nParam2( 0 ), nParam3( 0 )	{}
	tagACTMSG( DWORD dw, int n1, int n2, int n3 ) : dwMsg( dw ), nParam1( n1 ), nParam2( n2 ), nParam3( n3 )	{}
}	ACTMSG, *LPACTMSG;

#ifdef __OPT_MEM_0811
template <class T>
#endif	// __OPT_MEM_0811
class CActMsgq
{
private:
#ifdef __OPT_MEM_0811
	deque<T*>	m_qpam;
#else	// __OPT_MEM_0811
//{{AFX
	ACTMSG	m_aActMsg[MAX_ACTMSG];
	u_long	m_uHead;
	u_long	m_uTail;
//}}AFX
#endif	// __OPT_MEM_0811
public:
	CActMsgq();
	virtual ~CActMsgq();
public:
	void	RemoveHead();
#ifdef __OPT_MEM_0811
	void	AddTail( T * ptr );
	T*	GetHead();
#else	// __OPT_MEM_0811
//{{AFX
	void	AddTail( const ACTMSG & rActMsg );
	void	AddTail( OBJMSG dwMsg, int nParam1, int nParam2, int nParam3 );
	LPACTMSG	GetHead();
//}}AFX
#endif	// __OPT_MEM_0811
	BOOL	IsEmpty();
	void	Empty();
};

//--------------------------------------------------------------------------------
#ifdef __OPT_MEM_0811
template <class T> CActMsgq<T>::CActMsgq()
{
}

template <class T>CActMsgq<T>::~CActMsgq()
{
	for( deque<T*>::iterator i = m_qpam.begin(); i != m_qpam.end(); ++i )
		safe_delete( *i );
	m_qpam.clear();
}

template <class T> inline BOOL CActMsgq<T>::IsEmpty()
{
	return m_qpam.empty();
}

template <class T> inline void CActMsgq<T>::Empty()
{
	m_qpam.clear();
}

template <class T> void CActMsgq<T>::RemoveHead()
{
	if( !IsEmpty() )
	{
		safe_delete( m_qpam.front() );
		m_qpam.pop_front();
	}
}

template <class T> void CActMsgq<T>::AddTail( T* ptr )
{
	if( m_qpam.size() >= MAX_ACTMSG )
	{
		safe_delete( ptr );
		return;
	}
	m_qpam.push_back( ptr );
}

template <class T>T* CActMsgq<T>::GetHead()
{
	return( IsEmpty()? NULL: m_qpam.front() );
}
#else	// __OPT_MEM_0811 
//{{AFX
inline BOOL CActMsgq::IsEmpty()
{
	return( m_uHead == m_uTail );
}

inline void CActMsgq::Empty()
{
	m_uHead	= m_uTail	= 0;
}
//}}AFX
#endif	// __OPT_MEM_0811 
//--------------------------------------------------------------------------------

#ifdef __OPT_MEM_0811
class CMeleeAtkMsgq : public CActMsgq<ACTMSG>
{
public:
	CMeleeAtkMsgq()	{}
	virtual ~CMeleeAtkMsgq() {}
	BOOL	Process( CAction* pActMover );
};
#else	// __OPT_MEM_0811
//{{AFX
class CMeleeAtkMsgq : public CActMsgq
{
public:
	CMeleeAtkMsgq()	{}
	virtual ~CMeleeAtkMsgq() {}
	BOOL	Process( CAction* pActMover );
};
//}}AFX
#endif	// __OPT_MEM_0811

typedef	struct	tagMAGICATKMSG : public ACTMSG
{
	int		nMagicPower;
	int		idSfxHit;
	tagMAGICATKMSG() : ACTMSG( 0, 0, 0, 0 ), nMagicPower( 0 ), idSfxHit( 0 )	{}
	tagMAGICATKMSG( DWORD dw, int n1, int n2, int n3, int p, int s ) : ACTMSG( dw, n1, n2, n3 ), nMagicPower( p ), idSfxHit( s )	{}
}	MAGICATKMSG, *LPMAGICATKMSG;

class CMover;

#ifdef __OPT_MEM_0811
class CMagicAtkMsgq : public CActMsgq<MAGICATKMSG>
{
public:
	CMagicAtkMsgq()	{}
	virtual	~CMagicAtkMsgq()	{}
	BOOL	Process( CMover* pMover );
};

#else	// __OPT_MEM_0811
//{{AFX
class CMagicAtkMsgq
{
private:
	MAGICATKMSG	m_aMagicAtkMsg[MAX_ACTMSG];
	u_long	m_uHead;
	u_long	m_uTail;

public:
	//	Constructions
	CMagicAtkMsgq() { m_uHead = m_uTail	= 0; }
	virtual  ~CMagicAtkMsgq()	{}
	//	Operations
	void	AddTail( OBJMSG dwAtkMsg, int nParam1, int nParam2, int nParam3, int nMagicPower, int idSfxHit )
	{
		u_long uTail	= ( m_uTail + 1 ) % MAX_ACTMSG;
		if( uTail == m_uHead )
			return;
		
		LPMAGICATKMSG pMagicAtkMsg	= m_aMagicAtkMsg + m_uTail;
		pMagicAtkMsg->dwMsg	= dwAtkMsg;
		pMagicAtkMsg->nParam1	= nParam1;
		pMagicAtkMsg->nParam2	= nParam2;
		pMagicAtkMsg->nParam3	= nParam3;
		pMagicAtkMsg->nMagicPower	= nMagicPower;
		pMagicAtkMsg->idSfxHit	= idSfxHit;
		m_uTail	= uTail;
	}
	LPMAGICATKMSG	RemoveHead()
	{
		if( IsEmpty() )
			return NULL;
		LPMAGICATKMSG pMagicAtkMsg	= m_aMagicAtkMsg + m_uHead;
		m_uHead		= ( m_uHead + 1 ) % MAX_ACTMSG;
		return pMagicAtkMsg;
	}
	LPMAGICATKMSG	GetHead()
	{
		if( IsEmpty() )
			return NULL;
		return( m_aMagicAtkMsg + m_uHead );
	}
	BOOL	IsEmpty()
	{
		return( m_uHead == m_uTail );
	}
	void	Empty()
	{
		m_uHead	= m_uTail	= 0;
	}
	BOOL	Process( CMover* pMover );
};
//}}AFX
#endif	// __OPT_MEM_0811

class CAction
{
public:
	CAction();
	CAction( CMover* pMover );
	virtual	~CAction();

protected:
	DWORD			m_dwState;			// 동작 상태 - 외부에서 직접 건드리지 말것.
	DWORD			m_dwStateFlag;		// 동작 상태 플래그
	CMover*			m_pMover;
	
public:
	CMeleeAtkMsgq	m_qMeleeAtkMsg;
	CMagicAtkMsgq	m_qMagicAtkMsg;
	
public:	
	void			SetMover( CMover* pMover ) { m_pMover = pMover; }
	CMover*			GetMover() { return m_pMover; }
	CWorld*			GetWorld();
	void			Init();
	void			__ForceSetState( DWORD dwState ) { m_dwState = dwState; }		// dwState값으로 강제로 세팅. 서버와 동기화때 외엔 사용하지 말것.
	void			ClearStateFlag() { m_dwStateFlag = 0; }

	void			AddStateFlag( DWORD dwFlag );
	void			RemoveStateFlag( DWORD dwFlag );

	DWORD			GetStateFlag() { return m_dwStateFlag; }

	// 동작 설정/참조
	DWORD			GetState() { return m_dwState; }
	void			SetState( DWORD dwType, DWORD dwState )	{ ResetState(dwType);	m_dwState |= dwState; }
	void			SetMoveState( DWORD dwState ) { ResetState(OBJSTA_MOVE_ALL);	m_dwState |= dwState; }
	void			SetTurnState( DWORD dwState ) { ResetState(OBJSTA_TURN_ALL);	m_dwState |= dwState; }
	void			SetLookState( DWORD dwState ) { ResetState(OBJSTA_LOOK_ALL);	m_dwState |= dwState; }
	void			SetJumpState( DWORD dwState ) { ResetState(OBJSTA_JUMP_ALL);	m_dwState |= dwState; }
	void			SetActionState( DWORD dwState ) { ResetState(OBJSTA_ACTION_ALL);	m_dwState |= dwState; }
	void			ClearState();
	void			ResetState( DWORD dwState );
	DWORD			GetMoveState() { return m_dwState & OBJSTA_MOVE_ALL; }
	DWORD			GetTurnState() { return m_dwState & OBJSTA_TURN_ALL; }
	DWORD			GetLookState() { return m_dwState & OBJSTA_LOOK_ALL; }
	DWORD			GetJumpState() { return m_dwState & OBJSTA_JUMP_ALL; }
	DWORD			GetDmgState() { return m_dwState & OBJSTA_DMG_ALL; }
	DWORD			GetActionState() { return m_dwState & OBJSTA_ACTION_ALL; }
	
	BOOL			IsState( DWORD dwState ) { return ( m_dwState & dwState ) ? TRUE : FALSE; }
	BOOL			IsStateFlag( DWORD dwStateFlag ) { return ( m_dwStateFlag & dwStateFlag ) ? TRUE : FALSE; }
	BOOL			IsFly() { return (m_dwStateFlag & OBJSTAF_FLY) ? TRUE : FALSE; }
	BOOL			IsDie() { return (m_dwState & OBJSTA_DIE_ALL) ? TRUE : FALSE; }
	BOOL			IsSit() { return (m_dwStateFlag & OBJSTAF_SIT) ? TRUE : FALSE; }
	BOOL			IsRun() { return ((m_dwStateFlag & OBJSTAF_WALK) == 0) ? TRUE : FALSE; }
	BOOL			IsWalk() { return (m_dwStateFlag & OBJSTAF_WALK) ? TRUE : FALSE; }
	BOOL			IsAction() { return (m_dwState & OBJSTA_ACTION_ALL) ? TRUE : FALSE; }
#ifdef __Y_INTERFACE_VER3
	BOOL			IsMove() { return ( GetMoveState() == OBJSTA_FMOVE || GetMoveState() == OBJSTA_BMOVE || GetMoveState() == OBJSTA_LEFT || GetMoveState() == OBJSTA_RIGHT || GetMoveState() == OBJSTA_LMOVE || GetMoveState() == OBJSTA_RMOVE ) ? TRUE : FALSE; }  
#else //__Y_INTERFACE_VER3
	BOOL			IsMove() { return ( GetMoveState() == OBJSTA_FMOVE || GetMoveState() == OBJSTA_BMOVE || GetMoveState() == OBJSTA_LEFT || GetMoveState() == OBJSTA_RIGHT ) ? TRUE : FALSE; }  	
#endif //__Y_INTERFACE_VER3
	
	BOOL			IsActJump()		{ return m_dwState & OBJSTA_JUMP_ALL; }
	BOOL			IsActAttack()		{ return m_dwState & OBJSTA_ATK_ALL; }
	BOOL			IsActDamage()		{ return m_dwState & OBJSTA_DMG_ALL; }
	BOOL			IsActTurn()		{ return m_dwState & OBJSTA_TURN_ALL; }
	BOOL			IsActLook() 		{ return m_dwState & OBJSTA_LOOK_ALL; }

	// event handler
	void			OnEndMoveState();
	void			OnEndTurnState();
	void			OnEndLookState();
	void			OnEndJumpState( DWORD dwState );
	void			OnEndAttackState( DWORD dwState );
	void			OnEndDamageState();
	void			OnEndActionState();
		
	virtual	int		SendActMsg( OBJMSG dwMsg, int nParam1 = 0, int nParam2 = 0, int nParam3 = 0, int nParam4 = 0, int nParam5 = 0 )	{	return( 1 );	}
	virtual	void	ProcessState( DWORD dwState, float fSpeed ) { }
	virtual	BOOL	Render( LPDIRECT3DDEVICE9 pd3dDevice, D3DXMATRIX* pmatWorld = NULL ) { return TRUE; }

private:
	BOOL			IsAtkMsgEmpty()	{	return( m_qMeleeAtkMsg.IsEmpty() && m_qMagicAtkMsg.IsEmpty() );	}
	void			ProcessAtkMsg();
};

#endif

