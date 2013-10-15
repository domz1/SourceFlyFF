#include "stdafx.h"

#include "Ship.h"


D3DXMATRIX g_matView;
D3DXMATRIX g_matInvView;

CCamera::CCamera()
{
	m_vPos    = D3DXVECTOR3( 50.0f, 110.0f, 50.0f);
	m_vLookAt = D3DXVECTOR3( m_vPos.x + 1.0f, 100.0f, m_vPos.z + 1.0f);
	m_vOffset = D3DXVECTOR3(0.0f,0.0f,0.0f);
	m_nQuakeSec = 0;
	m_fQuakeSize = 0.06f;
	m_dwCamStyle = 0;
}
void CCamera::Process( LPDIRECT3DDEVICE9 pd3dDevice ,float fFactor )
{
}
void CCamera::Transform( LPDIRECT3DDEVICE9 pd3dDevice, CWorld* pWorld )
{
	D3DXVECTOR3	vTempPos = m_vPos;
	if( m_nQuakeSec > 0 )
	{
//		float x = (float)(8.0f - xRandom(16)) / 256.0f;
//		float y = (float)(8.0f - xRandom(16)) / 256.0f;
//		float z = (float)(8.0f - xRandom(16)) / 256.0f;

		float x = (m_fQuakeSize / 2.0f) - xRandomF( m_fQuakeSize );
		float y = (m_fQuakeSize / 2.0f) - xRandomF( m_fQuakeSize );
		float z = (m_fQuakeSize / 2.0f) - xRandomF( m_fQuakeSize );
		
		vTempPos.x += x;
		vTempPos.y += y;
		vTempPos.z += z;
		--m_nQuakeSec;
	}
	D3DXVECTOR3 vUp = D3DXVECTOR3(0, 1.0f, 0 );
#ifdef __CLIENT	
	if( g_WndMng.m_pWndWorld )
	{
		if( g_pPlayer && g_Option.m_bRollEffect )
		{
			D3DXMATRIX mRot;
			FLOAT	fRollAng = ((CWndWorld *)g_WndMng.m_pWndWorld)->m_fRollAng;
			D3DXMatrixRotationYawPitchRoll( &mRot, D3DXToRadian(-g_pPlayer->GetAngle()), 0/*D3DXToRadian(g_pPlayer->GetAngleX())*/, D3DXToRadian(fRollAng) );
			D3DXVec3TransformCoord( &vUp, &vUp, &mRot );
		}
	}
#endif // client
	D3DXMatrixLookAtLH( &m_matView, &vTempPos, &m_vLookAt, &vUp );

	pd3dDevice->SetTransform( D3DTS_VIEW, &m_matView );
	pWorld->UpdateCullInfo( &m_matView, &pWorld->m_matProj );
	// 거대한 오브젝트의 컬링을 위해 카메라를 실제보다 뒤로 조금 뺀다.
	/*
	D3DXVECTOR3 vTempLook = m_vLookAt;
	D3DXVec3Normalize(&(vTempLook),&(vTempLook));
	vTempPos -= ( vTempLook * 3);
	D3DXMATRIX mTempView;
	D3DXMatrixLookAtLH( &mTempView, &vTempPos, &m_vLookAt, &D3DXVECTOR3(0.0f,1.0f,0.0f) );
	pWorld->UpdateCullInfo( &mTempView, &pWorld->m_matProj );
	*/
	// 현재 카메라의 역함수를 구한다. 이것은 여기저기서 다목적으로 사용한다.
	D3DXMatrixInverse( &m_matInvView, NULL, &m_matView );
	m_matInvView._41 = 0.0f; m_matInvView._42 = 0.0f; m_matInvView._43 = 0.0f;
	g_matInvView = m_matInvView;
	g_matView = m_matView;
}
void CCamera::SetQuake( int nSec, float fSize )
{
	if( m_nQuakeSec <= 0 )
	{
		m_nQuakeSec = nSec;
		m_fQuakeSize = fSize;
	}
}
void CCamera::Reset() 
{ 
	//m_vPos2 = m_vPos; 
}
void CCamera::ControlCamera( DWORD dwStyle )
{
	m_dwCamStyle = dwStyle;
}
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

void CBackCamera::Lock()
{
#ifdef __CLIENT

	m_bLock = TRUE;
	if( g_WorldMng.Get() )
	{
		CMover* pMoverTarget = (CMover*)g_WorldMng.Get()->GetObjFocus() ;
		if( !(pMoverTarget && CMover::GetActiveMover() && CMover::GetActiveMover()->m_pActMover->IsFly()) ) {
			FLOAT fAngle = CMover::GetActiveMover()->GetAngle(); 
			m_fRotx=-fAngle;
		}
	}
#endif
}
void CBackCamera::Unlock()
{
	m_bLock = FALSE;
	m_fRotx = 0;
	m_fRoty = 0;
#if __VER >= 13 // __HOUSING
	m_nCamMode = CM_NORMAL;
#endif // __HOUSING
}

CBackCamera::CBackCamera()
{
	m_fZoom = 6.0f;
	m_bLock = FALSE;
	m_fRoty = -12.0f;

	m_fLength1 = 0.0f;
	m_fLength2 = 0.0f;
	m_bOld     = FALSE;
	m_bStart   = FALSE;
}
D3DXVECTOR3 m_vPosVal; 
void CBackCamera::ControlCamera( DWORD dwStyle )
{
	return;
	CCamera::ControlCamera( dwStyle );
	D3DXVECTOR3 vPos = CMover::GetActiveMover()->GetPos();
	D3DXVECTOR3 vTemp = vPos - m_vPos;

	switch( dwStyle )
	{
	case CAMSTY_HIT:
		m_vPosVal = vTemp;
		break;
		
	case CAMSTY_DAMAGE:
		m_vOffset += vTemp;
		break;
	}
}

int  CBackCamera::GetAnglePie( FLOAT fAngle )
{
	FLOAT fAngTemp = fabs(fAngle);

	if( fAngTemp > 0.0f && fAngTemp <= 90.0f )
	{
		return 4;
	}
	
	if( fAngTemp > 90.0f && fAngTemp <= 180.0f )
	{
		return 3;
	}
	
	if( fAngTemp > 180.0f && fAngTemp <= 270.0f )
	{
		return 2;
	}	
	
	if( fAngTemp > 270.0f && fAngTemp <= 360.0f )
	{
		return 1;
	}

	return 0;
}

#ifdef __HOUSING
BOOL CBackCamera::SetCamMode(int nType)
{

	m_nCamMode = nType;

	return m_nCamMode;
}
#endif

void CBackCamera::Process( LPDIRECT3DDEVICE9 pd3dDevice ,float fFactor )
{
#ifdef __CLIENT
	CMover *pMover = CMover::GetActiveMover();
	// 여기서 카메라 세팅!!!!!
	if( pMover == NULL )	return;

	CWorld* pWorld = pMover->GetWorld();
	if( pWorld == NULL )
		return;
	D3DXMATRIX matView, mat;

	FLOAT fAngle = 0, fAngleY = 0;

	D3DXVECTOR3 vPos = pMover->GetPos();
	
	vPos.y += 0.9f;
#if __VER >= 13 // __HOUSING
	if(m_nCamMode == CM_MYROOM)
	{
		if(m_fZoom <= 0.5f) m_fZoom = 0.5f;
	//	if(m_fZoom >= 3.0f) m_fZoom = 3.0f;
	}
#endif // __HOUSING
	CMover* pMoverTarget = (CMover*)g_WorldMng.Get()->GetObjFocus() ;
	D3DXVECTOR3 vTarget,vTemp;
	if( pMoverTarget && pMover->m_pActMover->IsFly() && (pMover->m_dwFlag & MVRF_TRACKING) ) 
	{	// 날고 있는 경우 타겟이 있다면
		// 타겟쪽으로 카메라 방향을 수정한다.
		vTemp = vPos - pMoverTarget->GetPos();
		if( vTemp.z > 0 ) 
		{
			fAngle =- (float)( atan( vTemp.x / vTemp.z ) * 180 / 3.1415926f );
		}
		else 
		{
			fAngle =- (float)( atan( vTemp.x / vTemp.z ) * 180 / 3.1415926f ) + 180;
		}
		D3DXVECTOR3	vDistXZ = vTemp;
		vDistXZ.y = 0;
		float fDistSq = D3DXVec3Length( &vDistXZ );		// XZ평면에서의 길이
		fAngleY = atan2( fDistSq, vTemp.y/* * vTemp.y*/ );
		fAngleY = D3DXToDegree( fAngleY );

		float fReg1 = vTemp.y / 40.0f;
		if( fReg1 > 0 )
		{
			if( fReg1 >= 2.0f )	fReg1 = 2.0f;
		} else
		if( fReg1 < 0 )
		{
			if( fReg1 <= -2.0f )	fReg1 = -2.0f;
		}

		m_fCurRoty = m_fRoty + m_fZoom * fReg1;
		if( m_bLock )
			fAngle = 0;

	}
	else 
	{
		fAngle = pMover->GetAngle();
		if( m_bLock )
			fAngle = 0;
		fAngleY = 90.0f;

		m_fCurRoty = m_fRoty + m_fZoom * 4;
	}
	m_vLookAt = vPos;

#ifdef __Y_CAMERA_SLOW_8
	if( !g_WndMng.m_pWndWorld->m_bRButtonDown && ( !g_bKeyTable[ VK_LEFT ] && !g_bKeyTable[ VK_RIGHT ] ) )
	{
		static FLOAT fSpeed = 2.0f;
		BOOL  bLeft = FALSE;
		BOOL  bRight = FALSE;
		FLOAT fTemp = 0.0f;

		fTemp = m_fRotx;

		if( (GetAnglePie(fTemp) == 1 && GetAnglePie(m_fCurRotx) == 4) )
			bRight = TRUE;

		if( (GetAnglePie(fTemp) == 4 && GetAnglePie(m_fCurRotx) == 1) )
			bLeft = TRUE;

		if( bRight )
		{
			m_fCurRotx += m_fRotx;

			if( m_fCurRotx < fTemp )
			{
				m_fCurRotx += fSpeed;
			}

			m_fCurRotx -= m_fRotx;

			if( m_fCurRotx >= 0.0f )
			{
				m_fCurRotx = -360.0f;
			}
		}

		if( bLeft )
		{
			fTemp += -360.0f;
			
			if( m_fCurRotx > fTemp )
			{
				m_fCurRotx += -fSpeed;

				if( m_fCurRotx < -360.0f )
					m_fCurRotx = 0.0f;
			}

			fTemp -= -360.0f;
		}

		if( !bLeft && !bRight )
		{
			FLOAT fGoal = fabs(m_fCurRotx - fTemp);
			if( m_fCurRotx < fTemp )
			{
				if( fGoal > fSpeed )
					m_fCurRotx += fSpeed;
			}
			else
			{
				if( fGoal > fSpeed )
					m_fCurRotx -= fSpeed;
			}
		}
	}
	else
	{
		m_fCurRotx = m_fRotx;
	}

#else //__Y_CAMERA_SLOW_8
	m_fCurRotx = m_fRotx;
#endif //__Y_CAMERA_SLOW_8

#ifdef __XUZHU
	_g_fReg[0] = fAngleY;
#endif
	
	float fAdjAng = (1.0f - fAngleY / 90.0f) * 45.0f;
	m_fCurRoty += fAdjAng;
	m_fCurRoty += pMover->GetAngleX();
	
	if( pMover->m_pActMover->IsFly() )	// 비행할땐 조금 들어주자
		m_fCurRoty += 0.0f;
	if( m_fCurRoty > 80.0f ) 
		m_fCurRoty = 80.0f;

#if __VER >= 13 // __HOUSING
	if(m_nCamMode == CM_MYROOM)
	{
		if(m_fCurRoty <= 10.0f) 
		{
			m_fCurRoty = 10.0f;
			if(m_fRoty > 0.0f) m_fRoty = 0.0f;
			if(m_fRoty < -30.0f) m_fRoty = -30.0f;
		}
	}
#endif // __HOUSING

	fAngle = m_fCurRotx - fAngle + 180.0f;

	D3DXMATRIX matTemp;
	// zoom 상태에 따라 카메라 위치를 조정
	extern float fDiv;
	
	if( fDiv == 2.0f )
		//vTemp = D3DXVECTOR3( 0.0f, 0.0f, -0.0f - (m_fZoom / 2.0f) * 2.0f );
		vTemp = D3DXVECTOR3( 0.0f, 0.0f, -0.0f - 2.0f );
	else
	{
		if( g_pShip )
			vTemp = D3DXVECTOR3( 0.0f, 0.0f, -4.0f - m_fZoom * 16.0f );
		else
			//vTemp = D3DXVECTOR3( 0.0f, 0.0f, -50.0f );
			vTemp = D3DXVECTOR3( 0.0f, 0.0f, -4.0f - m_fZoom * 2.0f );
	}
	
	D3DXVECTOR3 vecOut;
	D3DXMatrixRotationX( &matTemp, D3DXToRadian( m_fCurRoty / 1.0f ) );
	D3DXVec3TransformCoord( &vTemp, &vTemp, &matTemp );
	D3DXMatrixRotationY( &matTemp, D3DXToRadian( fAngle ) );
	D3DXVec3TransformCoord( &m_vOffsetDest, &vTemp, &matTemp );

	D3DXVECTOR3 vecOffsetDelta = ( ( m_vOffsetDest - m_vOffset ) + m_vPosVal ) / fFactor;

	m_vOffset += vecOffsetDelta;

	m_vPosVal /= 2;

	m_vPos = vPos + m_vOffset;

	BOOL  bCrash;
	FLOAT fLength;
	static D3DXVECTOR3 m_vLimitPos;
	D3DXVECTOR3 m_vOutPos = m_vPos;

	m_vLookAt.y += 0.4f;
#if __VER >= 11 // __GUILD_COMBAT_1TO1
	if( g_pPlayer && g_GuildCombat1to1Mng.IsPossibleMover( g_pPlayer ) )
		bCrash = FALSE;
	else
		bCrash = pWorld->CheckBound( &m_vPos, &m_vLookAt, &m_vOutPos, &fLength );
#else //__GUILD_COMBAT_1TO1
	bCrash = pWorld->CheckBound( &m_vPos, &m_vLookAt, &m_vOutPos, &fLength );
#endif //__GUILD_COMBAT_1TO1

	// 충돌이있다면 마지막으로 충돌했던 거리를 저장
	if( bCrash )
		m_fLength2 = fLength;

	// 전프레임에 충돌, 현재는 충돌이 아닐때...즉, 서서히 뒤로 가게하는 시점..
	if( m_bOld && bCrash == FALSE )
	{
		m_fLength1 = fLength;
		m_bStart = TRUE;
	}

	if( m_bStart )
	{
		D3DXVECTOR3 vCPos = vPos + m_vOffset;
		D3DXVECTOR3 vDir  = vCPos - m_vLookAt;
		D3DXVec3Normalize(&vDir, &vDir);
#if __VER >= 12 // __CAM_FAST_RECOVER
		m_fLength2 += 0.37f;
#else
		m_fLength2 += 0.07f;
#endif
		if( m_fLength2 > fLength )
			m_bStart = FALSE;

		m_vOutPos = m_vLookAt + (vDir * m_fLength2);
	}
	else
	if( bCrash )
	{/*
		if( fLength < 5.0f )
		{
			D3DXVECTOR3 vCPos = vPos + m_vOffset;
			D3DXVECTOR3 vDir  = vCPos  - m_vLookAt;
			D3DXVec3Normalize(&vDir, &vDir);
			
			FLOAT fff = m_vOutPos.y;
			m_vOutPos = m_vLookAt + (vDir * 5.0f);
			m_vOutPos.y = fff;
		}
	 */
	}

	m_bOld = bCrash;

	g_ModelGlobal.SetCamera( m_vOutPos, m_vLookAt );

	m_vPos = m_vOutPos;
#endif // CLIENT
}
CToolCamera::CToolCamera()
{
	m_vPos = D3DXVECTOR3( 0.0f, 110.0f, 0.0f);
	m_vVelocity = D3DXVECTOR3( 0,0,0);
	m_fYaw = 0;	
	m_fYawVelocity = 0;
	m_fPitch = 0;
	m_fPitchVelocity = 0;
	//m_matOrientation = 0;
}
void CToolCamera::Process( LPDIRECT3DDEVICE9 pd3dDevice, float fFactor )
{
}
void CToolCamera::Transform( LPDIRECT3DDEVICE9 pd3dDevice, CWorld* pWorld )
{
    D3DXQUATERNION qR;
    D3DXQuaternionRotationYawPitchRoll( &qR, m_fYaw, m_fPitch, 0.0f );
    //D3DXMatrixAffineTransformation( &m_matOrientation, 1.25f, NULL, &qR, &m_vPos );
	D3DXMatrixAffineTransformation( &m_matOrientation, 1.0f, NULL, &qR, &m_vPos );
    D3DXMatrixInverse( &m_matView, NULL, &m_matOrientation );
    pd3dDevice->SetTransform( D3DTS_VIEW, &m_matView );
	pWorld->UpdateCullInfo( &m_matView, &pWorld->m_matProj );
	D3DXMatrixInverse( &m_matInvView, NULL, &m_matView );
	m_matInvView._41 = 0.0f; m_matInvView._42 = 0.0f; m_matInvView._43 = 0.0f;
	g_matInvView = m_matInvView;
	g_matView = m_matView;
	
}
 