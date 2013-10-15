#include "stdafx.h"
#include "ActionShip.h"
#include "..\_Common\Ship.h"

#ifdef __CLIENT
#include "DPClient.h"
extern	CDPClient	g_DPlay;
#endif


CActionShip::CActionShip()
{
	Init();
}

CActionShip::~CActionShip()
{
	Destroy();
}

void CActionShip::Init( void )
{
	m_pShip = NULL;
}

void CActionShip::Destroy( void )
{
	// 파괴코드.

	Init();
}

//
//
//
int	CActionShip::Process( void )
{
	CShip *pShip = m_pShip;
	
	if( GetState() == 0 )
	{
		SendActMsg( OBJMSG_STAND );
	}

	if( GetState() & OBJSTA_MOVE_ALL )
		ProcessState( GetState() & OBJSTA_MOVE_ALL );

	if( GetState() & OBJSTA_TURN_ALL )	// 좌/우 턴
		ProcessState( GetState() & OBJSTA_TURN_ALL );
	
	if( GetState() & OBJSTA_LOOK_ALL )	// 고도 상/하강
		ProcessState( GetState() & OBJSTA_LOOK_ALL );

	if( GetState() & OBJSTA_ATK_ALL )	// 발포
		ProcessState( GetState() & OBJSTA_ATK_ALL );

	if( GetState() & OBJSTA_DMG_ALL )	// 피격.
		ProcessState( GetState() & OBJSTA_DMG_ALL );

	if( GetTurnState() == 0 )		// 턴을 멈췄을때 서서히 서는것.
	{
		if( pShip->m_fAccAng > 0 )		// 왼쪽으로 턴하고 있었다.
		{
			pShip->m_fAccAng -= 0.002f;
			if( m_pShip->m_fAccAng < 0 )
				m_pShip->m_fAccAng = 0;
		} else
		if( pShip->m_fAccAng < 0 )
		{
			pShip->m_fAccAng += 0.002f;
			if( m_pShip->m_fAccAng > 0 )
				m_pShip->m_fAccAng = 0;
		}
		if( m_pShip->m_fAccAng )
			m_pShip->SetAngle( m_pShip->GetAngle() + m_pShip->m_fAccAng );
	}

	return TRUE;
}

#define AE_SUCCESS	 1		// 명령을 성공적으로 실행했음.
#define AE_ALREADY	 0		// 이미 실행중이므로 다시 실행하지 않음.
#define AE_FAIL		-1		// 명령을 실행하기에 적합한 상황이 아니어서 걍 리턴함.

//
// 오브젝트에게 내려진 명령이 내려진 최초순간을 실행하고 상태를 전환시킴.
// return : #define AE_xxxx 참조.
int	CActionShip::SendActMsg( OBJMSG dwMsg, int nParam1, int nParam2, int nParam3 )
{
	switch( dwMsg )
	{
	case OBJMSG_STAND:	// 대기상태로 있어라.
		if( GetMoveState() == OBJSTA_STAND )	return AE_ALREADY;		// 이미 실행중이므로 리턴.
		SetMoveState( OBJSTA_STAND );		// 대기 상태로 전환.
		break;
	case OBJMSG_STOP:	// 멈춰라.
		if( GetMoveState() == 0 )	return AE_ALREADY;		// 이미 멈춰있으므로 리턴.
		ResetState( OBJSTA_MOVE_ALL );		// 이동상태를 해제
		SetMoveState( OBJSTA_STAND );
		break;
	case OBJMSG_FORWARD:	// 전진을 시작해라.
		if( GetMoveState() == OBJSTA_FMOVE )	return AE_ALREADY;	// 이미 전진상태이면 리턴.
		SetMoveState( OBJSTA_FMOVE );	// 전진 상태로 전환.
		break;
	
	case OBJMSG_LTURN:	// 좌회전을 시작해라.
		if( GetTurnState() == OBJSTA_LTURN )	return AE_ALREADY;	// 이미 왼쪽턴중.
		SetTurnState( OBJSTA_LTURN );
		break;
	case OBJMSG_RTURN:	// 우회전을 시작해라.
		if( GetTurnState() == OBJSTA_RTURN )	return AE_ALREADY;	// 이미 오른쪽턴중.
		SetTurnState( OBJSTA_RTURN );
		break;
	case OBJMSG_STOP_TURN:	// 회전을 멈춤.
		if( GetTurnState() == 0 )	return AE_ALREADY;	// 이미 멈춰있음.
		ResetState( OBJSTA_TURN_ALL );	// 턴상태 해제.
		break;

	case OBJMSG_LOOKUP:		// 고도상승 해라.
		if( GetLookState() == OBJSTA_LOOKUP )	return AE_ALREADY;	// 이미 상승중.
		SetLookState( OBJSTA_LOOKUP );
		break;
	case OBJMSG_LOOKDOWN:	// 고도하강.
		if( GetLookState() == OBJSTA_LOOKDOWN )	return AE_ALREADY;	// 이미 하강중
		SetLookState( OBJSTA_LOOKDOWN );
		break;
	case OBJMSG_STOP_LOOK:	// 상승/하강 멈춰라.
		if( GetLookState() == 0 )	return AE_ALREADY;	// 이미 멈췄음.
		ResetState( OBJSTA_LOOK_ALL );	// 상승/하강 상태 해제
		break;
	}
#ifdef __CLIENT
	if( m_pShip->GetMover()->IsActiveMover() )
		g_DPlay.SendShipActMsg( m_pShip, dwMsg, nParam1, nParam2 );
#endif // CLIENT	
	return 1;
}

//
// 오브젝트의 현재 상태를 실행함
//
void CActionShip::ProcessState( DWORD dwState, float fSpeed )
{
	CShip *pShip = m_pShip;
	FLOAT fAng = pShip->GetAngle();
	
	switch( dwState )
	{
	case OBJSTA_STAND:	// 대기중
		pShip->m_vAcc = D3DXVECTOR3( 0, 0, 0 );		// 대기상태에선 추진력이 없음.
		break;
	case OBJSTA_FMOVE:	// 전진중
		AngleToVectorXZ( &pShip->m_vAcc, fAng, 0.001f );		// fAng방향으로 추진력발생.
		break;
	case OBJSTA_LTURN:	// 왼쪽턴중
		pShip->m_fAccAng += 0.01f;
		if( pShip->m_fAccAng > 0.1f )
			pShip->m_fAccAng = 0.1f;
		fAng += pShip->m_fAccAng;
		pShip->SetAngle( fAng );
		break;
	case OBJSTA_RTURN:	// 오른턴중.
		pShip->m_fAccAng -= 0.01f;
		if( pShip->m_fAccAng < -0.1f )
			pShip->m_fAccAng = -0.1f;
		fAng += pShip->m_fAccAng;
		pShip->SetAngle( fAng );
		break;
	case OBJSTA_LOOKUP:		// 고도 상승중
		pShip->m_vAcc.y += 0.0001f;
		break;
	case OBJSTA_LOOKDOWN:	// 고도 하강중
		pShip->m_vAcc.y -= 0.0001f;
		break;

	}
}

