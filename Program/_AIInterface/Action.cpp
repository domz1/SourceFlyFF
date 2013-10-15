// Billboard.cpp : implementation of the CBeastView class
//

#include "stdafx.h"
#include "Action.h"


CAction::CAction()
{
	Init();
}
CAction::CAction( CMover* pMover ) 
{ 
	Init();
	m_pMover = pMover;
}

CAction::~CAction()
{
}

void CAction::Init( void )
{
	m_pMover		= NULL;
	m_dwState		= 0;
	m_dwStateFlag	= 0;
}

void	CAction::AddStateFlag( DWORD dwFlag ) 
{
#ifdef __BS_EFFECT_LUA
	//상태가 추가될 경우 최초인경우에 효과를 발생시킨다.
	if( dwFlag & OBJSTAF_COMBAT && !IsStateFlag( OBJSTAF_COMBAT ) )	
	{
		// 만약 전투상태로 처음 돌입하는거라면 상태별 효과를 호출한다. 일단 npc류만 
		if( m_pMover->IsNPC( ) )
			run_lua_sfx( OBJSTAF_COMBAT, m_pMover->GetId(), m_pMover->GetNameO3D() );
	}
#endif //__BS_EFFECT_LUA
	m_dwStateFlag |= dwFlag; 
}

void	CAction::RemoveStateFlag( DWORD dwFlag )
{
#ifdef __BS_EFFECT_LUA
	// 상태가 해제될경우 lua에서 호출한 모든 상태효과를 해제한다.
	if( dwFlag & OBJSTAF_COMBAT )
	{
		//전투 상태효과 해제
		CSfxModelMng::GetThis()->SubData( m_pMover->GetId(), OBJSTAF_COMBAT );
	}
#endif //__BS_EFFECT_LUA
	m_dwStateFlag &= (~dwFlag);
}

void CAction::ProcessAtkMsg( void )
{
	if( !m_qMeleeAtkMsg.Process( this ) )
		m_qMagicAtkMsg.Process( GetMover() );
}


CWorld* CAction::GetWorld( ) 
{ 
	return m_pMover->GetWorld(); 
}

// OBJSTA_MOVE_ALL 스테이트가 클리어되는 순간 발생.
void	CAction::OnEndMoveState( void )
{
	// 이곳에 그외 필요한 코드를 넣으셈.
	switch( m_dwState & OBJSTA_MOVE_ALL )
	{
	case OBJSTA_SIT:		// 착지 동작 중.
		m_dwStateFlag &= (~OBJSTAF_SIT);
		break;
	}	
}

void	CAction::OnEndTurnState( void )
{
	// 이곳에 그외 필요한 코드를 넣으셈.
}

void	CAction::OnEndLookState( void )
{
	// 이곳에 그외 필요한 코드를 넣으셈.
}

void	CAction::OnEndJumpState( DWORD dwState )
{
	// 이곳에 그외 필요한 코드를 넣으셈.
	switch( dwState & OBJSTA_JUMP_ALL )
	{
	case OBJSTA_SJUMP4:		// 착지 동작 중.
		m_pMover->OnActEndJump4();
		break;
	}
}

void	CAction::OnEndAttackState( DWORD dwState )
{
	// 이곳에 그외 필요한 코드를 넣으셈.
	switch( dwState & OBJSTA_ATK_ALL )
	{
	case OBJSTA_ATK1:
	case OBJSTA_ATK2:
	case OBJSTA_ATK3:
	case OBJSTA_ATK4:
	case OBJSTA_ATK_MAGIC1:
	case OBJSTA_ATK_RANGE1:
	case OBJSTA_RANGE3:
	case OBJSTA_RANGE4:
#ifdef __WORLDSERVER
		if( m_pMover->IsPlayer() && m_pMover->IsFly() )	
			return;

		if( !IsAtkMsgEmpty() )
			ProcessAtkMsg();
#else // worldserver
		if( m_pMover->IsActiveMover() == FALSE )
		{
			if( !IsAtkMsgEmpty() )
				ProcessAtkMsg();
		}
#endif // not worldserver
		break;
	case OBJSTA_ATK_MELEESKILL:
		m_pMover->OnActEndMeleeSkill();		// 근접스킬 동작 끝남.
		break;
	case OBJSTA_ATK_MAGICSKILL:
		m_pMover->OnActEndMagicSkill();		// 마법스킬 동작 끝남.
		break;
	}
}

void	CAction::OnEndDamageState( void )
{
	// 이곳에 그외 필요한 코드를 넣으셈.
}

void	CAction::OnEndActionState( void )
{
	// 이곳에 그외 필요한 코드를 넣으셈.
}

void	CAction::ClearState( void )
{
	DWORD dwState = m_dwState;
	// 0xFFFFFFFF 로 한꺼번에 리셋 시키지 말것.
//	ResetState( OBJSTA_MOVE_ALL | OBJSTA_TURN_ALL | OBJSTA_LOOK_ALL | OBJSTA_JUMP_ALL | OBJSTA_ATK_ALL | OBJSTA_DMG_ALL );
	ResetState( OBJSTA_ALL );
	
	if( m_dwState )
	{
		Error( "CAction::ClearState : 아직도 뭔가 클리어되지 않은 값이 있다. %08x, %08x", dwState, m_dwState );
		m_dwState = 0; 
	}
}

// dwState에는 OBJSTA들이 섞여서 올 수 있다.
// 리셋하고자 하는 스테이트만 클리어시키면서 해당 핸들러 불러줌.
void	CAction::ResetState( DWORD dwState )
{ 
	DWORD dw	= m_dwState;
	if( dwState & OBJSTA_MOVE_ALL )
	{
		OnEndMoveState();				// 핸들러 호출
		m_dwState &= (~dwState);		// 해당 비트값 클리어.
	}
	
	if( dwState & OBJSTA_TURN_ALL )
	{
		OnEndTurnState();
		m_dwState &= (~dwState);		// 해당 비트값 클리어.
	}
	
	if( dwState & OBJSTA_LOOK_ALL )
	{
		OnEndLookState();
		m_dwState &= (~dwState);		// 해당 비트값 클리어.
	}
	
	if( dwState & OBJSTA_JUMP_ALL )
	{
		m_dwState &= (~dwState);		// 해당 비트값 클리어.
		OnEndJumpState( dw );
	}
	
	if( dwState &	OBJSTA_ATK_ALL )
	{
		m_dwState &= (~dwState);		// 해당 비트값 클리어.
		OnEndAttackState(dw);
	}
	
	if( dwState &	OBJSTA_DMG_ALL )
	{
		OnEndDamageState();
		m_dwState &= (~dwState);		// 해당 비트값 클리어.
	}
	
	if( dwState & OBJSTA_ACTION_ALL )
	{
		OnEndActionState();
		m_dwState &= (~dwState);
	}
}

#ifndef __OPT_MEM_0811
//{{AFX
CActMsgq::CActMsgq()
:
m_uHead( 0 ),
m_uTail( 0 )
{
}

CActMsgq::~CActMsgq()
{
}

void CActMsgq::RemoveHead()
{
	if( IsEmpty() )
		return;
	LPACTMSG lpActMsg	= m_aActMsg + m_uHead;
	m_uHead		= ( m_uHead + 1 ) % MAX_ACTMSG;
}

void CActMsgq::AddTail( const ACTMSG & rActMsg )
{
	u_long uTail	= ( m_uTail + 1 ) % MAX_ACTMSG;
	if( uTail == m_uHead )
		return;
	memcpy( ( m_aActMsg + m_uTail ), &rActMsg, sizeof(ACTMSG) );
	m_uTail	= uTail;
}

void CActMsgq::AddTail( OBJMSG dwMsg, int nParam1, int nParam2, int nParam3 )
{
	u_long uTail	= ( m_uTail + 1 ) % MAX_ACTMSG;
	if( uTail == m_uHead )
		return;
		LPACTMSG lpActMsg	= m_aActMsg + m_uTail;
	lpActMsg->dwMsg	= dwMsg;
	lpActMsg->nParam1	= nParam1;
	lpActMsg->nParam2	= nParam2;
	lpActMsg->nParam3	= nParam3;
	m_uTail	= uTail;
}

LPACTMSG CActMsgq::GetHead()
{
	return ( IsEmpty()? NULL: m_aActMsg + m_uHead );
}
//}}AFX
#endif	// __OPT_MEM_0811