#include "stdafx.h"
#include "Ship.h"

#ifdef __WORLDSERVER
#include "User.h"
extern	CUserMng	g_UserMng;
#endif



#define		FRIC_AIR	0.011f		// 비행중 마찰계수

CShip::CShip()
{
	m_dwType	= OT_SHIP;
	m_Act.SetShip( this );
	Init();
}

CShip::~CShip()
{
	Destroy();
}

void CShip::Init( void )
{
	m_pMover = NULL;
	m_vDelta.x = m_vDelta.y = m_vDelta.z = 0;
	m_vDeltaUnit.x = m_vDeltaUnit.y = m_vDeltaUnit.z = 0;
	m_vAcc.x = m_vAcc.y = m_vAcc.z = 0;
	m_fAccAng = 0;
	m_nCount = 0;
	m_fDeltaAng = 0;
	memset( m_LinkCtrl, 0, sizeof(m_LinkCtrl) );
	
}

void CShip::Destroy( void )
{
	Init();
}

OBJID *CShip::FindCtrl( OBJID idCtrl )
{
	int		i;
	OBJID *pList = m_LinkCtrl;
	OBJID *pNode;
	
	for( i = 0; i < MAX_LINKCTRL; i ++ )
	{
		pNode = pList++;
		if( *pNode == 0 ) continue;		// 빈 노드는 검사할필요 없음.
		if( *pNode == idCtrl )
			return pNode;
	}

	return NULL;
}

void CShip::AddCtrl( OBJID idCtrl )	
{	
	int		i;
	OBJID *pList = m_LinkCtrl;
	OBJID *pNode;
	BOOL	bSuccess = FALSE;
	
	for( i = 0; i < MAX_LINKCTRL; i ++ )
	{
		pNode = pList++;
		if( *pNode ) continue;		// 빈노드가 아니라면 다른 노드를 찾음.
		*pNode = idCtrl;			// 빈 노드에 채움.
		bSuccess = TRUE;
		break;
	}

	if( bSuccess == FALSE )
		Error( "CShip::AddCtrl : Add실패." );
}

// idCtrl을 찾아서 지움.
void CShip::RemoveCtrl( OBJID idCtrl )
{
	int		i;
	OBJID *pList = m_LinkCtrl;
	OBJID *pNode;
	
	for( i = 0; i < MAX_LINKCTRL; i ++ )
	{
		pNode = pList++;
		if( *pNode == 0 ) continue;
		if( *pNode == idCtrl )
			*pNode = 0;			// 노드를 지움.
		return;
	}
}


void CShip::Control( void )
{
#ifdef __CLIENT
	static BOOL	s_bAcced = 0;
	static BOOL	s_bLefted = 0;
	static BOOL	s_bRighted = 0;
	static BOOL s_bBoarded = 0;
	
	BOOL bAcc = g_bKeyTable[ VK_SPACE ];
	BOOL bLeft = g_bKeyTable[g_Neuz.Key.chLeft];
	BOOL bDown = g_bKeyTable[ g_Neuz.Key.chUp ];
	BOOL bRight = g_bKeyTable[ 'D' ];
	BOOL bUp = g_bKeyTable[ 'S' ];
	BOOL bBoard = g_bKeyTable[ 'B' ];
	
	if( bAcc && !s_bAcced )		// 가속/정지 토글.
	{
		if( m_Act.GetMoveState() == OBJSTA_FMOVE )		// 전진중이라면
			SendActMsg( OBJMSG_STOP );		// 멈추고
		else
			SendActMsg( OBJMSG_FORWARD );	// 아니라면 전진시작.
	}
	s_bAcced = bAcc;

	if( bLeft )
	{
		SendActMsg( OBJMSG_LTURN );		// 왼쪽으로 턴
	} else
	if( bRight )
	{
		SendActMsg( OBJMSG_RTURN );		// 오른쪽으로 턴.
	} else
	{
		SendActMsg( OBJMSG_STOP_TURN );	// 좌/우키가 안눌려져있다면 회전 정지.
	}

	if( bUp )
	{
		SendActMsg( OBJMSG_LOOKUP );
	} else
	if( bDown )
	{
		SendActMsg( OBJMSG_LOOKDOWN );
	} else
	{
		SendActMsg( OBJMSG_STOP_LOOK );	// 위/아래 키가 안눌려져있다면 상승/하강 정지.
	}

	if( bBoard && !s_bBoarded )			// 배 조종을 해제하기.
	{
		g_pShip = NULL;
		SetMover( NULL );
	}
	s_bBoarded = bBoard;

	
#endif // CLIENT
}

void CShip::Process()
{
	D3DXMatrixInverse( GetInvTM(), NULL, &m_matWorld );		// 좌표변환이 이뤄지기전 매트릭스로 역행렬을 구해놔야 한다.
	
	D3DXVECTOR3	vPos = GetPos();
	D3DXVECTOR3 vDeltaAccu;

	vDeltaAccu = D3DXVECTOR3(0, 0, 0);		// 누적 벡터는 항상 초기화 해줘야 한다.

#ifdef __X15
	int		i;
	for( i = 0; i < 4; i ++ )
#endif	
	{
		m_Act.Process();			// 오브젝트의 현재움직임 상태를 수행.
		
		m_vDeltaUnit += m_vAcc;			// 현재 속도에 새로운 힘을 더함.
		m_vDeltaUnit *= (1.0f - FRIC_AIR);
		vDeltaAccu += m_vDeltaUnit;		// 서버 15프레임에서만 사용되는 것으로 4번을 누적함.
	}
#ifdef __CLIENT
	FLOAT fLenSq = D3DXVec3Length( &m_vDelta );		// 1/60 sec 속도
	extern int g_nFlySpeed;
	if( m_pMover->IsActiveMover() )
	{
		g_nFlySpeed = (int)( (fLenSq * 60.0f) * 60.0f * 60.0f );
		g_nFlySpeed	= (int)( g_nFlySpeed / 200.0f );
	}
	
#endif	

	m_vDelta = vDeltaAccu;		// 이번프레임의 this 이동량이된다.  항상 m_vDelta에 최종 이동량이 들어가게 하자.
	vPos += m_vDelta;
	SetPos( vPos );
	m_fDeltaAng = m_fAccAng;
	
	// IA오브젝트는 다른 오브젝트를 태우고 다녀야 하므로
	// 실시간으로 매트릭스가 갱신되어야 한다.
	UpdateLocalMatrix();		


	
}

//////////////////////////////////////////////////////////////////////////
CShipLoop::CShipLoop()
{
	Init();
}

CShipLoop::~CShipLoop()
{
	Destroy();
}

void CShipLoop::Init( void )
{
}

void CShipLoop::Destroy( void )
{
	Init();
}

//
//
//
void CShipLoop::Process()
{
	D3DXMatrixInverse( GetInvTM(), NULL, &m_matWorld );		// 좌표변환이 이뤄지기전 매트릭스로 역행렬을 구해놔야 한다.
	
	D3DXVECTOR3	vPos = GetPos();
	D3DXVECTOR3 vDeltaAccu;
	FLOAT fAng = GetAngle();
	
	vDeltaAccu = D3DXVECTOR3(0, 0, 0);		// 누적 벡터는 항상 초기화 해줘야 한다.
	m_fDeltaAng = 0.07f;
#ifdef __X15
	int		i;
	for( i = 0; i < 4; i ++ )
#endif	
	{
		fAng += m_fDeltaAng;		// 왼쪽으로 계속 회전
		AngleToVectorXZ( &m_vAcc, fAng, 0.05f );		// fAng방향으로 추진력발생.
		vDeltaAccu += m_vAcc;		// 서버 15프레임에서만 사용되는 것으로 4번을 누적함.
	}
	
	m_vDelta = vDeltaAccu;
	vPos += m_vDelta;
	SetPos( vPos );
	SetAngle( fAng );
	
#ifdef __WORLDSERVER
	if( (m_nCount & 127) == 0 )
	{
		OBJID idCtrl = NULL_ID;

		g_UserMng.AddSetPosAngle( this, GetPos(), GetAngle() );		// 먼저 this(Ship)의 위치를 sync시킴.
		// 링크되어 있는 모든 ctrl의 위치를 다시 sync시킴.
		for( i = 0; i < MAX_LINKCTRL; i ++ )		
		{
			if( m_LinkCtrl[i] == 0 )	continue;
			idCtrl = m_LinkCtrl[i];
			CCtrl *pCtrl = prj.GetCtrl( idCtrl );
			if( IsValidObj( pCtrl ) )
			{
				if( pCtrl->GetIAObjLink() == this )
					g_UserMng.AddSetPosAngle( pCtrl, pCtrl->GetPos(), pCtrl->GetAngle() );		// this(Ship)의 위치를 sync시킴.
				else
					RemoveCtrl( pCtrl->GetId() );
			}
		}
	}
#endif
	
	// IA오브젝트는 다른 오브젝트를 태우고 다녀야 하므로
	// 실시간으로 매트릭스가 갱신되어야 한다.
	UpdateLocalMatrix();		

	m_nCount ++;
}
