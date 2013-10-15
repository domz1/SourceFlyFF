#ifndef __CAMERA_H
#define __CAMERA_H

class CWorld;
class CObj;

#define CAMSTY_NONE    0
#define CAMSTY_HIT     1
#define CAMSTY_DAMAGE  2

extern D3DXMATRIX g_matView;
extern D3DXMATRIX g_matInvView;

class CCamera 
{
//protected:
public:
	D3DXVECTOR3    m_vPos        ; // 카메라가 있는 위치 
	//D3DXVECTOR3    m_vPos2       ; // 카메라가 있는 위치 2

	D3DXMATRIX     m_matView     ; // View 행렬 저장 
	D3DXMATRIX     m_matInvView  ; // View 역행렬 저장 

	D3DXVECTOR3    m_vLookAt     ; // 카메라가 보는 방향

	D3DXVECTOR3    m_vPosDest    ; // 카메라 이동 목표
	D3DXVECTOR3    m_vLookAtDest ; // 카메라 보는 위치 이동 목표 

	D3DXVECTOR3    m_vOffset     ; // 캐릭터 위치에서 카메라 상대 위치
	D3DXVECTOR3    m_vOffsetDest ; // 캐릭터 위치에서 카메라 상대 위치 이동 목표

	DWORD          m_dwCamStyle  ;
	FLOAT m_fRotx,m_fRoty; // 마우스 조작에 의한 x,y축 회전값의 목표점
	FLOAT m_fCurRotx,m_fCurRoty; // 현재 마우스 조작에 의한 x,y축 회전값. 서서히 목표값에 가까와진다.
	
	int		m_nQuakeSec;
	float	m_fQuakeSize;		// 진동크기. 진동폭을 넣는다. 1.0f는 1미터폭으로 흔들린다.

	CCamera();

	void Reset();
	void SetPos( D3DXVECTOR3& vPos ) { m_vPos = vPos; }
	D3DXVECTOR3 GetPos() { return m_vPos; }

	void	SetQuake( int nSec, float fSize = 0.06f );

virtual	void Process( LPDIRECT3DDEVICE9 pd3dDevice, float fFactor=15.0f );
virtual	void Transform( LPDIRECT3DDEVICE9 pd3dDevice, CWorld* pWorld );
virtual void ControlCamera( DWORD dwStyle );
};

class CBackCamera : public CCamera
{
	BOOL m_bLock;

public:
	FLOAT m_fLength1;
	FLOAT m_fLength2;
	BOOL  m_bOld;
	BOOL  m_bStart;
	FLOAT m_fZoom;

#if __VER >= 13 // __HOUSING
#define CM_NORMAL	0
#define CM_MYROOM	1

	int m_nCamMode;
	BOOL SetCamMode(int nType);
#endif	// __HOUSING
	CBackCamera();
	
	BOOL IsLock() { return m_bLock; }
	void Lock();
	void Unlock();

	int  GetAnglePie( FLOAT fAngle );
	
	virtual	void Process( LPDIRECT3DDEVICE9 pd3dDevice, float fFactor=15.0f );
	virtual void ControlCamera( DWORD dwStyle );
};

//
//
//
class CFlyCamera : public CCamera
{
	BOOL m_bLock;
	FLOAT	m_fAngle;		// 카메라가 보는 좌/우 방향.
	FLOAT	m_fAngleY;		// 카메라가 보는 위/아래 방향
	
public:
	CObj* pObjTarget;
	FLOAT m_fZoom;
	
	CFlyCamera();
	
	BOOL IsLock() { return m_bLock; }
	void Lock();
	void Unlock();
	
	virtual	void Process( LPDIRECT3DDEVICE9 pd3dDevice, float fFactor=15.0f );
	virtual void ControlCamera( DWORD dwStyle );
};

class CToolCamera : public CCamera
{
public:
	//D3DXVECTOR3        m_vPosition;
	D3DXVECTOR3        m_vVelocity;
	FLOAT              m_fYaw;
	FLOAT              m_fYawVelocity;
	FLOAT              m_fPitch;
	FLOAT              m_fPitchVelocity;
	//D3DXMATRIXA16         m_matView;
	D3DXMATRIXA16         m_matOrientation;
	CToolCamera();
	virtual	void Process( LPDIRECT3DDEVICE9 pd3dDevice, float fFactor=15.0f );
	virtual	void Transform( LPDIRECT3DDEVICE9 pd3dDevice, CWorld* pWorld );
	
};


#endif