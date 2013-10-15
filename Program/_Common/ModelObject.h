#ifndef		__MODELOBJECT_H__
#define		__MODELOBJECT_H__

#include "Object3D.h"
#include "Bone.h"
#include "xuzhu.h"
#include "xUtil3D.h"
#include "ModelGlobal.h"
#ifdef __ATTACH_MODEL
#include <boost/shared_ptr.hpp>
#endif //__ATTACH_MODEL

#define		MAX_SF_SLERP		3		// 키와키 사이를 몇단계로 보간할 것인가
#define		MAX_SF_SWDFORCE		512
#define		MAX_SF_SPLINE		4096

struct FVF_SWDFORCE
{
    D3DXVECTOR3 position;	// The 3D position for the vertex
    DWORD		color;		// The vertex color
};
#define	D3DFVF_SWDFORCE			(D3DFVF_XYZ | D3DFVF_DIFFUSE)

class CSwordForce
{
public:
	int		m_nMaxVertex;
	int		m_nMaxSpline;
	int		m_nMaxDraw;			// 스플라인 리스트중에 그릴 리스트
	DWORD	m_dwColor;			// 검광색.
	FVF_SWDFORCE	m_aList[ MAX_SF_SWDFORCE ];
	FVF_SWDFORCE	m_aList2[ MAX_SF_SWDFORCE ];
	FVF_SWDFORCE	m_aSpline[ MAX_SF_SPLINE ];

	CSwordForce() { Init(); }
	~CSwordForce() { Destroy(); }

	void	Init( void )
	{
		m_nMaxVertex = 0;
		m_nMaxSpline = 0;
		m_nMaxDraw = 0;
		m_dwColor = D3DCOLOR_ARGB( 255, 120, 120, 230 );
		memset( m_aList, 0, sizeof(FVF_SWDFORCE) * MAX_SF_SWDFORCE );
		memset( m_aSpline, 0, sizeof(FVF_SWDFORCE) * MAX_SF_SPLINE );
	}
	void	Destroy( void )
	{
		Init();
	}

	void	Clear( void )
	{
		m_nMaxVertex = 0;
		m_nMaxSpline = 0;
		m_nMaxDraw = 0;
	}
	BOOL	IsEmpty( void ) { (m_nMaxSpline == 0) ? TRUE : FALSE; }

	void	Add( D3DXVECTOR3 v1, D3DXVECTOR3 v2 );
	void	Process( void );
	void	Draw( LPDIRECT3DDEVICE9 pd3dDevice, const D3DXMATRIX *mWorld );
	void	MakeSpline( void );

};

//
// 객체를 구성하는 o3d들.  Element라 칭하자.
//
struct O3D_ELEMENT
{
	int				m_nParentIdx;
	CObject3D		*m_pObject3D;
	D3DXMATRIX		m_mLocalTM;				// m_pParent로부터의 LocalTM. 부모가 임의로 바뀔수 있기땜에 따로 가짐.
	D3DXMATRIX		m_mLocalRH;				// 오른손에 쥐는 무기의 로컬매트릭스
	D3DXMATRIX		m_mLocalLH;				// 왼손에 쥐는 무기의 로컬매트릭스
	D3DXMATRIX		m_mLocalShield;			// 방패위치에 대한 로컬 "
	D3DXMATRIX		m_mLocalKnuckle;		// 너클위치에 대한 로컬 "

	// CModelObject에서 독자적으로 가지고 있는 m_ppd3d_VB는 
	// 스키닝 오브젝트의 처리를 위한 독립적인 버텍스버퍼 공간이다.  
	// CObject3D가 로딩되면서 이것이 피지크 정보를 가지고 있다면
	// CObject3D내의 m_ppd3d_VB에 버텍스 버퍼를 생성 하지 않고.
	// 그 상위 다이나믹레벨인 CModelObject에 m_ppd3d_VB에 버텍스 버퍼를 생성하고 매 프레임 이곳에 데이타 갱신을 한다.
	// 버텍스정보가 들어있는 메모리 버퍼는 항상 CObject3D에만 갖고 있고(read only로)
	// 스태틱 오브젝트인가 다이나믹 오브젝트인가에 따라서 버텍스 버퍼는 CObject3D에 갖고 있거나 CModelObject에 갖고 있게 된다.
	// 
	LPDIRECT3DVERTEXBUFFER9		*m_ppd3d_VB;	// 버퍼 풀 - 지울땐 이것만 날리면 된다 .
	LPDIRECT3DVERTEXBUFFER9		*m_ppd3d_VB1;	// 스킨오브젝트를 위한 독자적인 버텍스버퍼 m_pObject3D->m_nMaxObject만큼 생성된다. - 직접 지워선 안된다.
	LPDIRECT3DVERTEXBUFFER9		*m_ppd3d_VB2;	// LOD2
	LPDIRECT3DVERTEXBUFFER9		*m_ppd3d_VB3;	// LOD3
	LPDIRECT3DVERTEXBUFFER9		*m_ppd3d_VBSel;	// 현재 선택된 버퍼
	LPDIRECT3DTEXTURE9			m_pTextureEx;	// 추가 텍스쳐.(멀티텍스쳐로 옷을 덧입힐때 쓴다.) 텍스쳐스테이지1번에 들어간다.
	MATERIAL					m_Mtrl;			// mes에 포함된 텍스쳐이외에 텍스쳐를 따로 쓸때.  여기에 값이 없다면 디폴트것을 쓴다.
	int							m_nEffect;		// 각 파츠의 외부지정 이펙트 설정.
	int							m_nPartsIdx;
};

#define		MAX_ELEMENT		21

#ifdef __ATTACH_MODEL
class CModelObject;
typedef	boost::shared_ptr<CModelObject>	SpModelObject;
typedef	map<int, SpModelObject> MapSpModelObject;
#endif //__ATTACH_MODEL

// 애니메이션 개체. 게임내 오브젝트의 최소단위.
// 모션이 여기에 포함되어 있기때문에 바이페드든 일반이든 애니메이션을 시킨다면
// 반드시 이것으로 생성시켜서 쓸것.
// 독자적인 월드좌표를 가진다.
class CModelObject : public CModel
{
public:
	BOOL				m_bMotionBlending;				// 모션 블렌딩 플래그
	float				m_fBlendWeight;					// 모션트랜지션의 가중치 0.0에서 1.0까지 되면 끝난다.
	D3DXMATRIX			*m_pBaseBoneInv;				// InverseTM - destroy하지 말것
	BOOL				m_bCollObj;						// 충돌메시를 포함하고 있는가?
	LPDIRECT3DTEXTURE9	m_pCloakTexture;				// 외부 지정 망토 텍스쳐 
	O3D_ELEMENT			m_Element[ MAX_ELEMENT ];		// 객체를 구성하는 o3d들.  Element라 칭하자.
#if __VER >= 9 // __Y_ADV_ENCHANT_EFFECT
	int					m_nEffectCount;
#endif //__Y_ADV_ENCHANT_EFFECT
	
public:
	// Bone / animation
	// 원래 Motion은 Element안에 있어야 맞는것이지만 문제를 간략화 하기 위해서 
	// CModelObject로 빼냈다.  1뼈대 n메쉬 구조가 더 이해하기가 쉽기 때문이다.
	CBones				*m_pBone;						// 뼈대 클래스 - 이것을 직접 파괴시키면 안된다.
	CMotion				*m_pMotionOld;					// 이전 모션
	CMotion				*m_pMotion;						// 현재 로딩된 모션 포인터 - 직접 파괴시키면 안된다.
	CSwordForce			*m_pForce;						// 검광
	CSwordForce			*m_pForce2;						// 검광-왼손
	int					m_nFrameOld;					// 이전동작의 프레임번호
	D3DXMATRIX			*m_mUpdateBone;					// 뼈대의 애니메이션 계산후 변환된 매트릭스 - 뼈대 갯수만큼 생성됨 - 뼈대가 없는 오브젝트는 이것을 사용하지 않음.
	CPartsEffect		*m_pPartsEffect;				// 파츠에 붙는 이펙트.
	CPartsEffect		*m_pPartsEffect2;				// 왼손 무기에 붙는 이펙트.
#ifdef __NEW_WEAPON_GLOW
	CPartsEffect		*m_pm_pPartsEffectRegular[2];
	CPartsEffect		*m_pm_pPartsEffectElement[2];
#endif // __NEW_WEAPON_GLOW	
#if __VER >= 9 // __Y_ADV_ENCHANT_EFFECT
	CPartsEffect		*m_pPartsEffect1_Detail;				// 파츠에 붙는 이펙트.
	CPartsEffect		*m_pPartsEffect2_Detail;				// 왼손 무기에 붙는 이펙트.
#endif //__Y_ADV_ENCHANT_EFFECT

	D3DXVECTOR3			m_vForce1, m_vForce2;
#if __VER >= 9 // __CSC_VER9_5
	D3DXVECTOR3			m_vForce3, m_vForce4;
#endif //__CSC_VER9_5

#ifdef __ATTACH_MODEL
private:
	MapSpModelObject	m_mapAttachModel;
#endif //__ATTACH_MODEL

private:
	void				Init( int nParts );
	void				Init( void );
	void				Destroy( void );
	BOOL				Destroy( int nParts );

public:
	CModelObject();
	virtual ~CModelObject();

	void				CreateForce( int nParts );		// 검광클래스 생성
	int					GetRHandIdx( void ) { return m_pBone->m_nRHandIdx; }
	int					GetLHandIdx( void ) { return m_pBone->m_nLHandIdx; }
	int					GetRArmIdx( void ) { return m_pBone->m_nRArmIdx; }
	int					GetLArmIdx( void ) { return m_pBone->m_nLArmIdx; }

#ifdef __ATTACH_MODEL
	void				SetAttachModel(int nEventIndex, CModelObject* pModelObject);
	void				SetDetachModel(int nEventIndex);
	void				RenderAttachModel(LPDIRECT3DDEVICE9 pd3dDevice, const D3DXMATRIX *mWorld);
	int					RenderAttachModelElem(LPDIRECT3DDEVICE9 pd3dDevice, const D3DXMATRIX *mWorld);
	void				FrameMoveAttachModel( D3DXVECTOR3 *pvSndPos = NULL, float fSpeed = 1.0f );

	void				InitAttachModelDeviceObjects(LPDIRECT3DDEVICE9 pd3dDevice);
	void				RestoreAttachModelDeviceObjects();
	void				InvalidateAttachModelDeviceObjects();
	void				DeleteAttachModelDeviceObjects();
#endif //__ATTACH_MODEL
#if __VER >= 14 // __WING_ITEM
	CString				GetMotionFileName( const CString& rstrMotion ) const;
#endif // __WING_ITEM

	O3D_ELEMENT	*GetParts( int nParts ) 
	{ 
		for( int i=0; i<MAX_ELEMENT; i++ )
		{
			if( m_Element[i].m_nPartsIdx == nParts )
				return &(m_Element[i]);
		}
		return NULL; 
	}

	int	GetPartsIdx( int nParts ) 
	{ 
		for( int i=0; i<MAX_ELEMENT; i++ )
		{
			if( m_Element[i].m_nPartsIdx == nParts )
				return i;
		}
		return -1; 
	}
	
	CObject3D	*GetObject3D( void ) { return m_Element[0].m_pObject3D; }		// 이것은 단일메쉬에만 쓴다.

	CObject3D	*GetObject3D( int nNum ) 
	{ 
		for( int i=0; i<MAX_ELEMENT; i++ )
		{
			if( m_Element[i].m_nPartsIdx == nNum )
				return m_Element[i].m_pObject3D;
		}
		return NULL; 
	}		
	void	SetExtTexture( LPDIRECT3DTEXTURE9 pTexture ) { m_pCloakTexture = pTexture; }
	void	GetForcePos( D3DXVECTOR3 *vOut, int nIdx, int nParts, const D3DXMATRIX &mWorld );
	void	GetForcePos( D3DXVECTOR3 *vOut, int nIdx, int nParts, const D3DXMATRIX &mWorld, float fScale);
	void	GetHandPos( D3DXVECTOR3 *vOut, int nParts, const D3DXMATRIX &mWorld );
		
	int		MovePart( int nDstPart, int nSrcPart );
	void	SetMotionBlending( BOOL bFlag = TRUE );
	void	TakeOffParts( int nParts );		// 장착해제할때 쓴다.
	void	SetGroup( int nNum ) 
	{
		if( nNum < 0 || nNum >= MAX_GROUP )
		{
			Error( "SetGroup : lv=%d", nNum );
			nNum = 0;
		}

		for( int i = 0; i < MAX_ELEMENT; i ++ )
		{
			if( m_Element[i].m_pObject3D )	
			{
				if( m_Element[i].m_pObject3D->m_bLOD == 0 )		
					m_Element[i].m_pObject3D->SetLOD( 0 );		// LOD데이타가 없다면 항상 High레벨로만..
				else
				{
					m_Element[i].m_pObject3D->SetLOD( nNum );
					switch( nNum )
					{
					case 0:		m_Element[i].m_ppd3d_VBSel = m_Element[i].m_ppd3d_VB1;	break;		// lod가 선택될때마다 버퍼 포인터를 스위칭 해줌.
					case 1:		m_Element[i].m_ppd3d_VBSel = m_Element[i].m_ppd3d_VB2;	break;
					case 2:		m_Element[i].m_ppd3d_VBSel = m_Element[i].m_ppd3d_VB3;	break;
					}
				}
				
			}
		}
	}		// Set LOD Group
	// 각 파츠의 렌더링 이펙트를 외부에서 지정할때 쓴다.	XE_??? 시리즈를 조합해서 쓰자.
	O3D_ELEMENT*	SetEffect( int nParts, int nEffect ) 
	{ 
		if( GetParts(nParts) )
		{
			GetParts(nParts)->m_nEffect |= nEffect; 
			return GetParts(nParts);
		}
		return NULL;
	} 
	O3D_ELEMENT*	SetEffectOff( int nParts, int nEffect ) 
	{ 
		if( GetParts(nParts) )
		{
			GetParts(nParts)->m_nEffect &= (~nEffect); 
			return GetParts(nParts);
		}
		return NULL;
	} 
	void	SetTextureEx( int nNumEx )		// 몇번 텍스쳐 셋을 쓰느냐
	{
		
		m_Element[0].m_pObject3D->SetTextureEx( nNumEx );		// 몬스터는 [0]하나만 쓴다고 가정하고 하자.
	}

	
	BOOL	IsLoadAttr( void ) { if(m_pMotion) return TRUE; else return FALSE; }
	MOTION_ATTR *GetMotionAttr( int nNumFrm ) { return m_pMotion->GetMotionAttr(nNumFrm); }	
	DWORD	IsAttrHit( void ) 
	{ 
		if( m_pMotion )
			return m_pMotion->IsAttrHit( m_fFrameOld, m_fFrameCurrent ); 
		else
			return GetObject3D()->IsAttrHit( m_fFrameCurrent );
	}	// 현재 프레임에 타격속성이 있는가?
	DWORD	IsAttrHit( float fNumFrm ) { return m_pMotion->IsAttrHit( m_fFrameOld, fNumFrm ); }	// nNumFrm프레임에 타격속성이 있는가?
	MOTION_ATTR *IsAttrSound( void ) 
	{ 
		if( m_pMotion == NULL )
		{
			LPCTSTR szErr = Error( "IsAttrSound : pMotion==NULL %d %d", m_fFrameOld, m_fFrameCurrent );
			ADDERRORMSG( szErr );
		}
		return m_pMotion->IsAttrSound( m_fFrameOld, m_fFrameCurrent ); 
	}	// 현재 프레임에 사운드속성이 있는가?
	MOTION_ATTR *IsAttrSound( float fNumFrm ) 
	{ 
		if( m_pMotion)	//gmpbigsun: m_pMotion이 NULL인경우 shit ..
			return m_pMotion->IsAttrSound( m_fFrameOld, fNumFrm ); 	// nNumFrm프레임에 사운드속성이 있는가?	
		return NULL;
	}
	DWORD	IsAttrQuake( float fNumFrm ) { return m_pMotion->IsAttrQuake( m_fFrameOld, fNumFrm ); }
	DWORD	IsAttrQuake( void ) { return m_pMotion->IsAttrQuake( m_fFrameOld, m_fFrameCurrent ); }
	void	SetAttr( float fNumFrm, DWORD dwAttr ) { m_pMotion->SetAttr( fNumFrm, dwAttr ); }
	void	UpdateMatrixBone( void )
	{
		if( m_pMotion )		// 현재 프레임의 모션에 대한 매트릭스를 갱신시킴.
			m_pMotion->AnimateBone( m_mUpdateBone, m_pMotionOld, m_fFrameCurrent, GetNextFrame(), m_nFrameOld, m_bMotionBlending, m_fBlendWeight );	
	}
	void	GetEventPos( D3DXVECTOR3 *pOut, int nIdx = 0 )		// 업데이트된 본의 기준으로 계산된 이벤트좌표.
	{ 
		if( m_pBone == NULL )	return;
		if( m_mUpdateBone == NULL )	return;
		D3DXVec3TransformCoord( pOut, &m_pBone->m_vEvent[nIdx], &m_mUpdateBone[ m_pBone->m_nEventParentIdx[nIdx] ] );
		// 곱한 결과인 pOut은 로컬기준이므로 최종 m_mWorld와 곱해줘야 한다.
		// vFinal = pOut * m_mWorld;
	}
	D3DXMATRIX *GetMatrixBone( int nBoneIdx )
	{
		if( m_pBone == NULL )	return NULL;
		if( m_mUpdateBone == NULL )	return NULL;
		return &m_mUpdateBone[ nBoneIdx ];
	}
	BOOL	IsEmptyElement( void )
	{
		for( int i = 0; i < MAX_ELEMENT; i ++ )
			if( m_Element[i].m_pObject3D )	return FALSE;
		return TRUE;
	}
	D3DXVECTOR3 *GetPath( void ) { return ( m_pMotion ) ? m_pMotion->m_pPath : NULL; }

	int		IsHaveCollObj( void ) { return m_bCollObj; }
	HRESULT	CreateDeviceBuffer( O3D_ELEMENT *pElem );
	int		LoadBone( LPCTSTR szFileName );
	int		LoadMotion( LPCTSTR szFileName );
	int		LoadElement( LPCTSTR szFileName, int nParts = 0 );
	void	SetParent( int nParts, int nBoneIdx );
	void	SetTextureMulti( LPCTSTR szBitmap, int nParts );
	void	ClearTextureEx( int nParts );
	void	ChangeTexture( int nParts, LPCTSTR szSrc, LPCTSTR szDest );
	
	void	CreateParticle( int nParts, const D3DXMATRIX *pmWorld, int nType, int nLevel, int nLevel2 = 0 );
	void	RenderItemElec( int nParts, const D3DXMATRIX *pmWorld, int nLevel );
	void	RenderItemElec_Adv( int nParts, const D3DXMATRIX *pmWorld, int nLevel );
#ifdef __NEW_WEAPON_GLOW
	void	CreateWeaponEffect( int nParts, DWORD dwItemKind3, const D3DXMATRIX *pmWorld, int nType, int nLevel, int nLevel2 = 0 );
#endif // __NEW_WEAPON_GLOW
#ifndef __WORLDSERVER
	BOOL	Render( LPDIRECT3DDEVICE9 pd3dDevice, const D3DXMATRIX *mWorld );
#ifdef __NEW_WEAPON_GLOW
	void	RenderEffect( LPDIRECT3DDEVICE9 pd3dDevice, const D3DXMATRIX *mWorld, DWORD dwItemKind3 = NULL_ID, DWORD dwItemKind3Right = NULL_ID, int nLevelL = 0, int nLeveR = 0);
#else // __NEW_WEAPON_GLOW
	void	RenderEffect( LPDIRECT3DDEVICE9 pd3dDevice, const D3DXMATRIX *mWorld, DWORD dwItemKind3 = NULL_ID, int nLevelL = 0, int nLeveR = 0);
#endif // __NEW_WEAPON_GLOW
#ifdef __SHADOW
	BOOL	RenderShadow( LPDIRECT3DDEVICE9 pd3dDevice, const D3DXMATRIX *mWorld );
	void	SetStateShadow( LPDIRECT3DDEVICE9 pd3dDevice );
	void	ResetStateShadow( LPDIRECT3DDEVICE9 pd3dDevice );
#endif

#if __VER >= 15 // __GUILD_HOUSE
	int LoadClonedElement( LPCTSTR szFileName );		//gmpbigsun:복제본 생성 
#endif

#endif //__WORLDSERVER

	void	MakeSWDForce( int nParts, DWORD dwItemKind3, BOOL bSlow = FALSE, DWORD dwColor = D3DCOLOR_ARGB(255,120,120,230), float fSpeed = 1.0f );		// m_pMotion의 검광을 생성
	void	FrameMove( D3DXVECTOR3 *pvSndPos = NULL, float fSpeed = 1.0f );

	// collision detect
	BOOL	Intersect( const D3DXVECTOR3 &vRayOrig, const D3DXVECTOR3 &vRayDir, const D3DXMATRIX &mWorld, D3DXVECTOR3* pvIntersect, FLOAT* pfDist, BOOL bColl = FALSE  ) 
	{ 
		if( IntersectRayTri( vRayOrig, vRayDir, mWorld, pvIntersect, pfDist, bColl ) ) 
			return TRUE; 
		else 
			return FALSE; 
	}

	D3DXVECTOR3 *IntersectRayTri( const D3DXVECTOR3 &vRayOrig, const D3DXVECTOR3 &vRayDir, const D3DXMATRIX &mWorld, D3DXVECTOR3* pvIntersect, FLOAT* pfDist, BOOL bColl = FALSE ) 
	{	
		O3D_ELEMENT *pElem = m_Element;
		D3DXVECTOR3 *pTri;
		for( int i = 0; i < MAX_ELEMENT; i ++ )
		{
			if( pElem->m_pObject3D )
			{
				if( m_pBone )
					pElem->m_pObject3D->SetExternBone( m_mUpdateBone, m_pBaseBoneInv );	// 외장본이 있다면 그것을 넘겨준다.
				if( pTri = pElem->m_pObject3D->IntersectRayTri( vRayOrig, vRayDir, mWorld, pvIntersect, pfDist, bColl ) )
					return pTri;
			}
			pElem ++;
		}
		return NULL;
	}
	BOOL	IsTouchOBB_Line( const D3DXVECTOR3 &vStart, const D3DXVECTOR3 &vEnd, const D3DXMATRIX &mWorld, D3DXVECTOR3* pvIntersect, BOOL bNeedCollObject = TRUE )
	{ 
		CObject3D *pObj = m_Element[0].m_pObject3D;
		if( pObj->m_CollObject.m_Type == GMT_ERROR && bNeedCollObject )	// 충돌메시가 없으면 검사하지 않음.
			return FALSE;
		
		return ::IsTouchOBB_Line( m_vMin, m_vMax, mWorld, vStart, vEnd, pvIntersect );
	}

	BOOL	TestIntersectionOBB_Line( const Segment3& segment, const CObj* pObj, BOOL bNeedCollObject = TRUE );

	// virtual
	int LoadModel( LPCTSTR szFileName ) { return LoadElement( szFileName, 0 ); }

#ifdef __BS_EFFECT_LUA
	BOOL CModelObject::GetPosBone( D3DXVECTOR3* pOut, const char* bonename );
#endif

	// 디바이스 초기화 및 삭제 
	HRESULT InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
	HRESULT RestoreDeviceObjects();
	HRESULT InvalidateDeviceObjects();
	HRESULT DeleteDeviceObjects();
};

extern BOOL		g_bUsableVS;			// 하드웨어 버텍스 쉐이더(1.1)를 쓸수 있는가.
extern DWORD	g_dwUsablePS_Level;		// 하드웨서 픽셀쉐이더 버전 체크 2.0이하는 2 


#endif // MODELOBJECT