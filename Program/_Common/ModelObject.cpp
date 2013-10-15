#include "stdafx.h"
#include <d3d9.h>
#include <d3dx9.h>
#include <dxerr9.h>
#include <stdio.h>
#include "xUtil3d.h"
#include "defineObj.h"
#include "defineNeuz.h"
#include "Material.h"
#include "ModelGlobal.h"
#include "ModelObject.h"
#include "..\_UnhandledException\ExceptionHandler.h"



#ifdef	_DEBUG
extern LPDIRECT3DVERTEXDECLARATION9 g_pSkinVertexDeclaration;
extern LPDIRECT3DVERTEXSHADER9	g_pSkiningVS;
#endif

BOOL	g_bUsableVS = TRUE;		// 하드웨어 버텍스 쉐이더(1.1)를 쓸수 있는가.
DWORD	g_dwUsablePS_Level = 0;		

extern float g_fHairLight;
extern float s_fDiffuse[];
extern float s_fAmbient[];
extern D3DXVECTOR4 s_vLight;		// 라이트 벡터
extern D3DXVECTOR4 s_vLightPos;		// 라이트 포지션
extern D3DXMATRIX s_mViewProj;		// 
extern D3DXMATRIX s_mView;		// 
extern D3DXMATRIX s_mProj;		// 

extern BOOL s_bLight;


char   g_szLastFile[512];

/////////////////////////////////////////////////////////////////////////////////////
////////////
////////////
////////////
////////////
////////////
/////////////////////////////////////////////////////////////////////////////////////
CModelObject::CModelObject()
{
	Init();
}

CModelObject::~CModelObject()
{
	Destroy();
}

void CModelObject::Init( void )
{
	int		i;

	for( i = 0; i < MAX_ELEMENT; i ++ )
	{
		Init( i );
	}

	m_pBone = NULL;
	m_pForce = m_pForce2 = NULL;
	m_pPartsEffect = m_pPartsEffect2 = NULL;
#ifdef __NEW_WEAPON_GLOW
	m_pm_pPartsEffectRegular[0] = m_pm_pPartsEffectRegular[1] = NULL;
	m_pm_pPartsEffectElement[0] = m_pm_pPartsEffectElement[1] = NULL;
#endif // __NEW_WEAPON_GLOW
	m_pMotion = m_pMotionOld = NULL;
	m_mUpdateBone = NULL;
	m_nModelType = MODELTYPE_NONE;//BIPED_MESH;
	m_bMotionBlending = FALSE;
	m_fBlendWeight = 0;
	m_bCollObj = 0;
	m_pBaseBoneInv = NULL;

	m_nFrameOld = 0;

	m_vMin.x = m_vMin.y = m_vMin.z = 65535.0f;
	m_vMax.x = m_vMax.y = m_vMax.z = -65535.0f;
	m_vForce1.x = m_vForce1.y = m_vForce1.z = 0.0f;
	m_vForce2.x = m_vForce2.y = m_vForce2.z = 0.0f;
#if __VER >= 9 // __CSC_VER9_5
	m_vForce3.x = m_vForce3.y = m_vForce3.z = 0.0f;
	m_vForce4.x = m_vForce4.y = m_vForce4.z = 0.0f;
#endif //__CSC_VER9_5
	m_pCloakTexture = NULL;

#if __VER >= 9 // __Y_ADV_ENCHANT_EFFECT
	m_nEffectCount  = 0;

	m_pPartsEffect1_Detail = NULL;
	m_pPartsEffect2_Detail = NULL;

#endif //__Y_ADV_ENCHANT_EFFECT
}

void CModelObject::Init( int nIdx )
{
	if( nIdx < 0 )	return;
	memset( &m_Element[nIdx], 0, sizeof(O3D_ELEMENT) );
	m_Element[nIdx].m_nParentIdx = -1;
	m_Element[nIdx].m_nPartsIdx = -1;
	D3DXMatrixIdentity( &m_Element[nIdx].m_mLocalRH );
	D3DXMatrixIdentity( &m_Element[nIdx].m_mLocalLH );
	D3DXMatrixIdentity( &m_Element[nIdx].m_mLocalShield );
	D3DXMatrixIdentity( &m_Element[nIdx].m_mLocalKnuckle );
	D3DXMatrixIdentity( &m_Element[nIdx].m_mLocalTM );

	m_nFrameOld = 0;
}

//
void CModelObject::Destroy( void )
{
	int		i;

	// 메쉬 삭제
	for( i = 0; i < MAX_ELEMENT; i ++ )
	{
		Destroy( i );
	}
	SAFE_DELETE_ARRAY( m_mUpdateBone );

	SAFE_DELETE( m_pForce );
	SAFE_DELETE( m_pForce2 );
	SAFE_DELETE( m_pPartsEffect );
	SAFE_DELETE( m_pPartsEffect2 );
#ifdef __NEW_WEAPON_GLOW
	SAFE_DELETE( m_pm_pPartsEffectRegular[0]);
	SAFE_DELETE( m_pm_pPartsEffectRegular[1]);
	SAFE_DELETE( m_pm_pPartsEffectElement[0]);
	SAFE_DELETE( m_pm_pPartsEffectElement[1]);
#endif // __NEW_WEAPON_GLOW
#if __VER >= 9 // __Y_ADV_ENCHANT_EFFECT
	m_nEffectCount  = 0;
	SAFE_DELETE( m_pPartsEffect1_Detail );
	SAFE_DELETE( m_pPartsEffect2_Detail );
#endif //__Y_ADV_ENCHANT_EFFECT

	Init();
	// m_pBone를 직접 삭제하면 안된다.
	// m_pMotion을 직접 삭제하면 안된다.
	// CObject3D::m_pObject3D를 직접 삭제하면 안된다.  CObject3DMng에서 삭제해 준다.

#ifdef __ATTACH_MODEL
	m_mapAttachModel.clear();
#endif //__ATTACH_MODEL
}

#ifdef __ATTACH_MODEL
void CModelObject::SetAttachModel(int nEventIndex, CModelObject* pModelObject)
{
	if(nEventIndex < MAX_MDL_EVENT)
		m_mapAttachModel.insert(map<int, SpModelObject>::value_type(nEventIndex, SpModelObject(pModelObject)));
}

void CModelObject::SetDetachModel(int nEventIndex)
{
	map<int, SpModelObject>::iterator iter = m_mapAttachModel.find(nEventIndex);
	if(iter != m_mapAttachModel.end())
		m_mapAttachModel.erase(iter);
}

void CModelObject::InitAttachModelDeviceObjects(LPDIRECT3DDEVICE9 pd3dDevice)
{
	for(map<int, SpModelObject>::const_iterator i=m_mapAttachModel.begin(); i!=m_mapAttachModel.end(); ++i)
	{
		SpModelObject pModelObject = i->second;
		if(pModelObject.get() != NULL)
		{
			pModelObject->InitDeviceObjects(pd3dDevice);
		}
	}
}

void CModelObject::RestoreAttachModelDeviceObjects()
{
	for(map<int, SpModelObject>::const_iterator i=m_mapAttachModel.begin(); i!=m_mapAttachModel.end(); ++i)
	{
		SpModelObject pModelObject = i->second;
		if(pModelObject.get() != NULL)
		{
			pModelObject->RestoreDeviceObjects();
		}
	}
}

void CModelObject::InvalidateAttachModelDeviceObjects()
{
	for(map<int, SpModelObject>::const_iterator i=m_mapAttachModel.begin(); i!=m_mapAttachModel.end(); ++i)
	{
		SpModelObject pModelObject = i->second;
		if(pModelObject.get() != NULL)
		{
			pModelObject->InvalidateDeviceObjects();
		}
	}
}

void CModelObject::DeleteAttachModelDeviceObjects()
{
	for(map<int, SpModelObject>::const_iterator i=m_mapAttachModel.begin(); i!=m_mapAttachModel.end(); ++i)
	{
		SpModelObject pModelObject = i->second;
		if(pModelObject.get() != NULL)
		{
			pModelObject->DeleteDeviceObjects();
		}
	}
}

void CModelObject::RenderAttachModel(LPDIRECT3DDEVICE9 pd3dDevice, const D3DXMATRIX *mWorld)
{
	for(map<int, SpModelObject>::const_iterator i=m_mapAttachModel.begin(); i!=m_mapAttachModel.end(); ++i)
	{
		int nEventIndex = i->first;
		SpModelObject pModelObject = i->second;
		if(pModelObject.get() != NULL)
		{
			D3DXVECTOR3 vecAttachModel;
			GetEventPos(&vecAttachModel, nEventIndex);
			D3DXVec3TransformCoord(&vecAttachModel, &vecAttachModel, mWorld);
			
			D3DXMATRIX matRot, matAttachModel;
			D3DXMatrixIdentity(&matRot);
			D3DXMATRIX* matEventBone = GetMatrixBone(m_pBone->m_nEventParentIdx[nEventIndex]);
			matAttachModel = *matEventBone;

			D3DXQUATERNION qEventBone;
			D3DXVECTOR3 vYPW;
	
			D3DXQuaternionIdentity(&qEventBone);
			D3DXQuaternionRotationMatrix(&qEventBone, &matAttachModel);
			QuaternionRotationToYPW(qEventBone, vYPW);
			
			vYPW.x += D3DXToRadian(-90.0f);
			vYPW.y += D3DXToRadian(90.0f);
			//vYPW.z += D3DXToRadian(90.0f);
			D3DXMatrixRotationYawPitchRoll(&matRot,vYPW.y,vYPW.x,vYPW.z);
			//D3DXMatrixRotationX( &matRot, vYPW.x );
			//D3DXMatrixRotationY( &matRot, vYPW.y );
			//D3DXMatrixRotationZ( &matRot, vYPW.z );

			D3DXMATRIX matAttachModelWorld = matRot * matAttachModel * (*mWorld);
			matAttachModelWorld._41 = vecAttachModel.x;
			matAttachModelWorld._42 = vecAttachModel.y;
			matAttachModelWorld._43 = vecAttachModel.z;

			pModelObject->SetBlendFactor(m_dwBlendFactor);

			pModelObject->m_nNoEffect = m_nNoEffect;
			pModelObject->RenderAttachModelElem(pd3dDevice, &matAttachModelWorld);
			pModelObject->m_nNoEffect = 0;
		}
	}
}

int CModelObject::RenderAttachModelElem(LPDIRECT3DDEVICE9 pd3dDevice, const D3DXMATRIX *mWorld)
{
#ifdef	__WORLDSERVER
	return 1;
#else
	CObject3D	*pObject3D;
	O3D_ELEMENT	*pElem;
	int		i;
	int		nNextFrame;
	D3DXMATRIX	m1;

#ifndef __CLIENT						// 게임클라이언트가 아닐때...
	if( IsEmptyElement() == TRUE )		// 모델이 로드가 안되어 있으면 걍 리턴
		return 1;
#endif

#ifdef	_DEBUG
	if( m_mUpdateBone && g_pSkiningVS == NULL )
		Error( "CMyD3DApplication::RestoreDeviceObjects()에 CreateSkiningVS()를 호출하시오.\r\nCMyD3DApplication::InvalidateDeviceObjects()에는 DeleteSkiningVS()를 호출하시오.\r\n혹은 *.vsh파일을 읽지 못했다." );
	if( m_nLoop == 0 )
		Error( "경고 : %s : CModelObject::m_nLoop가 지정되지 않았다.", m_pMotion->m_szName );
#endif

	if( m_pMotion )		// m_pMotion이 널이라면 m_mUpdateBone도 널이다.
		m_pMotion->AnimateBone( m_mUpdateBone, m_pMotionOld, m_fFrameCurrent, GetNextFrame(), m_nFrameOld, m_bMotionBlending, m_fBlendWeight );		// 일단 뼈대가 있다면 뼈대 애니메이션 시킴

	if( m_pBone )		// m_pBone이 있다면 뼈대가 있다는 얘기. VS를 써야 한다.
	{
		D3DXMATRIX *pmBones;
		D3DXMATRIX mWorldTranspose;
		D3DXMATRIX *pmBonesInv = m_pBaseBoneInv ;
		BONE	*pBoneList = m_pBone->m_pBones;
		pmBones = m_mUpdateBone;

		if( m_pBone->m_bSendVS )	// 뼈대개수가 MAX_VS_BONE이하라서 한번에 다 전송한다.
		{
			int		nMaxBone = m_pBone->m_nMaxBone;

			if( nMaxBone > MAX_VS_BONE )	
				Error( "CModelObject::Render : overflow bone count - %d", nMaxBone );

			for( i = 0; i < nMaxBone; i ++ )	// MAX_VS_BONE개 이하	
			{
				mWorldTranspose = pmBonesInv[i] * pmBones[i];				
				D3DXMatrixTranspose( &mWorldTranspose, &mWorldTranspose );		// 매트릭스를 돌린다음.
				m_pd3dDevice->SetVertexShaderConstantF( i * 3, (float*)&mWorldTranspose, 3 );		// 상수레지스터에 집어넣음.
			}
		}
		D3DXMATRIX	mView, mProj;
		D3DXMATRIX	mViewProj, mViewProjTranspose, mInvWorld;

		D3DXVECTOR4 vLight = s_vLight;
		D3DXVECTOR4 vLightPos = s_vLightPos;

		mViewProj = *mWorld * s_mView * s_mProj;
		
		D3DXMatrixTranspose( &mViewProjTranspose, &mViewProj );
		D3DXMatrixTranspose( &mWorldTranspose, mWorld );

		D3DXMatrixInverse( &mInvWorld, NULL, mWorld );
		D3DXVec4Transform( &vLight, &vLight, &mInvWorld );
		D3DXVec4Normalize( &vLight, &vLight );
		D3DXVec4Transform( &vLightPos, &vLightPos, &mInvWorld );

		m_pd3dDevice->SetVertexShaderConstantF( 84, (float*)&mViewProjTranspose, 4 );
		m_pd3dDevice->SetVertexShaderConstantF( 91, (float*)&vLightPos, 1 );
		m_pd3dDevice->SetVertexShaderConstantF( 92, (float*)&vLight,   1 );
		m_pd3dDevice->SetVertexShaderConstantF( 93, (float*)&s_fDiffuse, 1 );
		m_pd3dDevice->SetVertexShaderConstantF( 94, (float*)&s_fAmbient, 1 );
	}

	nNextFrame = GetNextFrame();
	pd3dDevice->SetMaterial( g_TextureMng.GetMaterial( pd3dDevice, 0 ) );

	D3DXVECTOR3 vec3LightBackup = D3DXVECTOR3( s_vLight[0], s_vLight[1], s_vLight[2] );
	D3DXVECTOR4 vec4Diffuse = D3DXVECTOR4( s_fDiffuse[0], s_fDiffuse[1], s_fDiffuse[2], s_fDiffuse[3] );;
	
	// 엘리먼트엔 스킨,일반,모핑 심지어는 파티클까지도 포함될수있다.
	pElem = &m_Element[0];
	pObject3D = pElem->m_pObject3D;

	if(pElem != NULL && pObject3D != NULL && !(pElem->m_nEffect & XE_HIDE))
	{
		pObject3D->m_nNoTexture = 0; // m_nNoTexture;
		pObject3D->m_nNoEffect = m_nNoEffect;
		m1 = *mWorld;

		if( m_pBone )
			pObject3D->SetExternBone( m_mUpdateBone, m_pBaseBoneInv );	// 외장본이 있다면 그것을 넘겨준다.
		
		if( m_SparkInfo.m_bUsed )
		{
			SetLightVec( m_SparkInfo.m_v3SparkDir );
			
			D3DXVECTOR3 vec3Diffuse;
			D3DXVec3Lerp( &vec3Diffuse, &D3DXVECTOR3(0.0f,0.0f,0.0f), &m_SparkInfo.m_v3Color, m_SparkInfo.m_fLerp );
			SetDiffuse( vec3Diffuse.x, vec3Diffuse.y, vec3Diffuse.z );
		}
		
		DWORD dwBlendFactor = m_dwColor | ( m_dwBlendFactor << 24 );
		pObject3D->Render( pd3dDevice, pElem->m_ppd3d_VBSel, m_fFrameCurrent, nNextFrame, &m1, pElem->m_nEffect, dwBlendFactor );

		pObject3D->m_nNoEffect = 0;
	}
	
	return 1;
#endif // !__WORLDSERVER
}

void CModelObject::FrameMoveAttachModel(D3DXVECTOR3 *pvSndPos, float fSpeed)
{
	for(map<int, SpModelObject>::const_iterator i=m_mapAttachModel.begin(); i!=m_mapAttachModel.end(); ++i)
	{
		SpModelObject pModelObject = i->second;
		if(pModelObject.get() != NULL)
		{
			pModelObject->FrameMove(pvSndPos, fSpeed);
		}
	}
}
#endif //__ATTACH_MODEL

#if __VER >= 14 // __WING_ITEM
CString CModelObject::GetMotionFileName( const CString& rstrMotion ) const
{
	static TCHAR s_szRoot[ MAX_OBJTYPE ][32] = { "obj", "ani", "ctrl", "sfx", "item", "mvr", "region", "obj", "path" };

	CString strBoneFileName;
	if( m_pBone == NULL || m_pModelElem == NULL )
		return strBoneFileName;

	strBoneFileName.Format( _T("%s_%s_%s.ani"), s_szRoot[ m_pModelElem->m_dwType ], m_pModelElem->m_szName, rstrMotion );
	return strBoneFileName;
}
#endif // __WING_ITEM

// OBB와 line의 충돌테스트 
// BOOL bNeedCollObject = TRUE
BOOL CModelObject::TestIntersectionOBB_Line( const Segment3& segment, const CObj* pObj, BOOL bNeedCollObject )
{ 
	CObject3D *pObject3D = m_Element[0].m_pObject3D;
	if( pObject3D->m_CollObject.m_Type == GMT_ERROR && bNeedCollObject )	// 충돌메시가 없으면 검사하지 않음.
		return FALSE;
#if __VER >= 11 // __FIX_COLLISION
	// 길드대전장이 너무 예외적인 모양이라 퍼포먼스 측면에서 예외처리함.. 보기 좋지는 않다.. 
	char* pString = ((CModelObject *)pObj->m_pModel)->GetObject3D()->m_szFileName;
	if( strcmp("obj_guildcombatground.o3d", pString) == 0 )
	{
		return TRUE;
	}
#endif
	#ifdef __CPU_UTILDOWN_060502	//sun!!
		return IntrSegment3Box3_Test( segment, pObj->m_OBB );
	#else
		const D3DXMATRIX& mWorld = pObj->GetMatrixWorld();

		D3DXVECTOR3 vTemp;
		D3DXVECTOR3 vStart, vEnd;
		vStart = segment.Origin - segment.Extent * segment.Direction;
		vEnd   = segment.Origin + segment.Extent * segment.Direction;
		return ::IsTouchOBB_Line( m_vMin, m_vMax, mWorld, vStart, vEnd, &vTemp );
	#endif
}

//
BOOL CModelObject::Destroy( int nIdx )
{
	if( nIdx < 0 )	return FALSE;
	O3D_ELEMENT	*pElem = &m_Element[ nIdx ];
	int		i;

	if( pElem->m_pObject3D )
	{
		if( pElem->m_ppd3d_VB )
		{
			pElem->m_pObject3D->SetLOD(0);
			for( i = 0; i < pElem->m_pObject3D->GetMaxObject(); i ++ )		// 
				SAFE_RELEASE( pElem->m_ppd3d_VB1[i] );
			pElem->m_pObject3D->SetLOD(1);
			for( i = 0; i < pElem->m_pObject3D->GetMaxObject(); i ++ )
				SAFE_RELEASE( pElem->m_ppd3d_VB2[i] );
			pElem->m_pObject3D->SetLOD(2);
			for( i = 0; i < pElem->m_pObject3D->GetMaxObject(); i ++ )
				SAFE_RELEASE( pElem->m_ppd3d_VB3[i] );
			pElem->m_pObject3D->SetLOD(0);
			
			SAFE_DELETE_ARRAY( pElem->m_ppd3d_VB );		// 전체 메모리 풀을 한꺼번에 날림.
		}
		g_Object3DMng.DeleteObject3D( pElem->m_pObject3D );	// pObject3D가 공유된게 아니면 여기서 날려 버림.

		Init( nIdx );
		return TRUE;
	}
	Init( nIdx );
	return FALSE;
}

// 검광클래스 생성
void	CModelObject::CreateForce( int nParts )
{
#ifdef __CLIENT
	if( nParts == PARTS_RWEAPON )
	{
		if( m_pForce )	return;		// 이미 할당되어 있으면 다시 할당하지 않음.
		m_pForce = new CSwordForce;
	} else
	if( nParts == PARTS_LWEAPON )
	{
		if( m_pForce2 )	return;		// 이미 할당되어 있으면 다시 할당하지 않음.
		m_pForce2 = new CSwordForce;
	} else
	{
		ASSERT(0);
	}
#endif
}


void	CModelObject::TakeOffParts( int nParts )
{
	if( nParts < 0 )
		Error( "CModelObject::TakeOffParts : nParts = %d", nParts );


	// 파츠를 제거했는데, 아래 부분을 안해주면 old 파츠에서 매모리 릭 발생 - vampyre
	O3D_ELEMENT	*pElem;
	int nIdx = GetPartsIdx(nParts);
	pElem = GetParts(nParts);
	if( pElem && pElem->m_ppd3d_VB )
	{
		pElem->m_pObject3D->SetLOD(0);
		for( int j = 0; j < pElem->m_pObject3D->GetMaxObject(); j ++ )		// 
			SAFE_RELEASE( pElem->m_ppd3d_VB1[j] );
		pElem->m_pObject3D->SetLOD(1);
		for( j = 0; j < pElem->m_pObject3D->GetMaxObject(); j ++ )
			SAFE_RELEASE( pElem->m_ppd3d_VB2[j] );
		pElem->m_pObject3D->SetLOD(2);
		for( j = 0; j < pElem->m_pObject3D->GetMaxObject(); j ++ )
			SAFE_RELEASE( pElem->m_ppd3d_VB3[j] );
		pElem->m_pObject3D->SetLOD(0);
		SAFE_DELETE_ARRAY( pElem->m_ppd3d_VB );		// 전체 메모리 풀을 한꺼번에 날림.
	}

	if(nIdx != -1)
	{
	#if __VER >= 9 // __Y_ADV_ENCHANT_EFFECT
		if( nParts == PARTS_LWEAPON || nParts == PARTS_RWEAPON )
		{
			m_nEffectCount  = 0;

			SAFE_DELETE(m_pPartsEffect);
			SAFE_DELETE(m_pPartsEffect2);
#ifdef __NEW_WEAPON_GLOW
			SAFE_DELETE( m_pm_pPartsEffectRegular[0]);
			SAFE_DELETE( m_pm_pPartsEffectRegular[1]);
			SAFE_DELETE( m_pm_pPartsEffectElement[0]);
			SAFE_DELETE( m_pm_pPartsEffectElement[1]);
#endif // __NEW_WEAPON_GLOW
			SAFE_DELETE(m_pPartsEffect1_Detail);
			SAFE_DELETE(m_pPartsEffect2_Detail);
			//m_pPartsEffect = m_pPartsEffect2 = NULL;
			// m_pPartsEffect1_Detail = NULL;
			// m_pPartsEffect2_Detail = NULL;
		}
	#endif //__Y_ADV_ENCHANT_EFFECT

		Init( nIdx );
	}
}

//
//		LoadBone
//
int		CModelObject::LoadBone( LPCTSTR szFileName )
{
	if( m_pBone )
		Error( "CModelObject::LoadBone ; 이미 본이 로딩되어 있다. 0x%08x", (int)m_pBone );

	// Bone 관리자를 통해 본을 로딩한후 그 포인터를 받아온다.
	m_pBone = g_BonesMng.LoadBone( szFileName );

	// 본을 못읽었다.
	if( m_pBone == NULL )
	{
		LPCTSTR szStr = Error( "%s : 찾을 수 없음", szFileName );
		ADDERRORMSG( szStr );
		return FAIL;
	}

	m_bSkin = TRUE;		// 스키닝 오브젝트라는 플래그
#ifdef _DEBUG
	if( m_mUpdateBone )
		Error( "CModelObject::LoadBone : %s 이미 읽었는데 또 읽은것 같다.", szFileName );
#endif

	// 오브젝트의 계층구조가 애니메이션되면서 실시간으로 변환되는 매트릭스 배열
	m_mUpdateBone = new D3DXMATRIX[ m_pBone->m_nMaxBone * 2 ];	// Inv랑 같이 쓰려고 * 2로 잡는다,.
	m_pBaseBoneInv = m_mUpdateBone + m_pBone->m_nMaxBone;
	
	int		i;
	for( i = 0; i < m_pBone->m_nMaxBone; i ++ )
	{
		m_mUpdateBone[i] = m_pBone->m_pBones[i].m_mTM;		// 기본셋은 미리 카피해둠.
		m_pBaseBoneInv[i] = m_pBone->m_pBones[i].m_mInverseTM;	// Inv도 미리 받아둠.
	}
	return SUCCESS;
}


//
//	LoadMotion
// 오브젝트를 로딩하지않고도 독립적으로 동작해야한다.
// 
int		CModelObject::LoadMotion( LPCTSTR szFileName )
{
	int		i;
	// 본이 아직 할당 되어 있지 않으면 애니를 읽을수 없다.
	if( m_pBone == NULL )		
	{
		Error( "%s : 본을 로딩하지 않고 모션을 읽음", szFileName );
		return FAIL;
	}
	m_pMotionOld = m_pMotion;		// 예전 모션받아둠.
	m_nFrameOld = (int)m_fFrameCurrent;
	// Motion관리자를 통해 동작을 로딩한후 그 포인터를 받아온다.
	m_pMotion = g_MotionMng.LoadMotion( szFileName );
	if( m_pMotion == NULL )		// 모션을 못읽었다.
	{
		return FAIL;
	}
	if( m_pMotionOld == NULL )
		m_pMotionOld = m_pMotion;		// 이전프레임이 없으면 현재 프레임을 Old로 쓴다.  NULL이 되는상황이 없게 할것.

	InitAnimate();

	m_nFrameMax = m_pMotion->GetMaxFrame();
	m_fPerSlerp = (float)((int)(m_pMotion->m_fPerSlerp * 1000.0f)) / 1000.0f;		// 소숫점 3째 이하에서 오차가 생길수 있으므로 3째까지 에서 잘라버린다,.

	if( m_pMotionOld->m_nMaxBone != m_pMotion->m_nMaxBone )		// 이전에 읽은 모션의 본개수와 다르면 에러냄.
	{
		LPCTSTR szErr = Error( "CModelObject::LoadElement - %s bone count(%d) - old count(%d)", szFileName, m_pMotion->m_nMaxBone, m_pMotionOld->m_nMaxBone );
		ADDERRORMSG( szErr );
	}

	// 이미 스킨 오브젝트가 로딩되어 있는 상태면 m_mUpdateBone을 그 오브젝트에 등록시킴.
	for( i = 0; i < MAX_ELEMENT; i ++ )
	{
		if( m_Element[i].m_pObject3D )
			m_Element[i].m_pObject3D->SetExternBone( m_mUpdateBone, m_pBaseBoneInv );	// 함수내부에서 스킨인넘만 등록시킨다.
	}
	return SUCCESS;
}

// 파츠를 옮김.
// 왼손에 든걸 오른손에 옮기는등...
int	CModelObject::MovePart( int nDstPart, int nSrcPart )
{
	O3D_ELEMENT* pDestPart = GetParts( nDstPart );
	O3D_ELEMENT* pSrcPart = GetParts( nSrcPart );

	if( !pDestPart && pSrcPart )
	{
		LoadElement( pSrcPart->m_pObject3D->m_szFileName, nDstPart );
		memset( pSrcPart, 0, sizeof(O3D_ELEMENT) );		// 소스측은 클리어.	
		return 1;
	}

	if( !pDestPart || !pSrcPart )
		return 0;

	if( pDestPart->m_pObject3D )
		Error( "CModelObject::MovePart : m_Element[%d].m_pObject3D == 0x%08x", nDstPart, (int)pDestPart->m_pObject3D );
	
	memcpy( pDestPart, pSrcPart, sizeof(O3D_ELEMENT) );	// 카피하고
	memset( pSrcPart, 0, sizeof(O3D_ELEMENT) );		// 소스측은 클리어.	
	return 1;
}

//
//	오브젝트 로딩.  모션을 읽지 않은 상태에서도 독립적으로 동작한다.
//
int		CModelObject::LoadElement( LPCTSTR szFileName, int nParts )
{
	CObject3D	*pObject3D;
	O3D_ELEMENT	*pElem;

	if( nParts < 0 )
	{
		Error( "CModelObject::LoadElement : nParts 에러 - nParts = %d, %s", nParts, szFileName );
		return FAIL;
	}

	if( nParts == PARTS_RIDE )
		return FAIL;

	if(  nParts >= PARTS_LOWER2 && nParts <= PARTS_BULLET )
		return FAIL;

	int nIdx = GetPartsIdx(nParts);
	Destroy( nIdx );

#ifdef __CLIENT
	HRESULT hr;
	// Object3D관리자를 통해 메쉬를 로딩한 후 그 포인터를 받아온다.
	if( FAILED( hr = m_pd3dDevice->TestCooperativeLevel() ) )		// 디바이스가 허접하면 에러남김.
	{
		LPCTSTR szErr = Error( "%s 디바이스실패 %08x", szFileName, (int)hr );
		ADDERRORMSG( szErr );
	}
#endif
	pObject3D = g_Object3DMng.LoadObject3D( m_pd3dDevice, szFileName );
	if( pObject3D == NULL )
	{
		LPCTSTR szStr = Error( "%s : 찾을 수 없음", MakePath( DIR_MODEL, szFileName ) );
		ADDERRORMSG( szStr );
		return FAIL;
	}

	pObject3D->InitDeviceObjects( m_pd3dDevice );

	// 스킨파츠의 경우 외부본을 읽었는지 검사.
	if( pObject3D->IsUseExternBone() )		// 외장본을 사용하는 오브젝이다.
	{
		if( m_pBone == NULL )			// 본을 아직 안읽은 상태다...
		{
#ifdef __CLIENT
			Error( "%s : bone not loaded.", szFileName );		// 본을 로딩하지 않고 메쉬를 읽음
#endif // __CLIENT
			return FAIL;
		}
	}
	
	for( int i=0; i<MAX_ELEMENT; i++ )
	{
		if( m_Element[ i ].m_nPartsIdx == -1 )
			break;
	}

	if( i == MAX_ELEMENT )
		return FAIL;

	pElem = &m_Element[ i ];
	memset( pElem, 0, sizeof(O3D_ELEMENT) );
	pElem->m_nPartsIdx = nParts;
	pElem->m_nParentIdx = -1;
	D3DXMatrixIdentity( &(pElem->m_mLocalRH) );
	D3DXMatrixIdentity( &(pElem->m_mLocalLH) );
	D3DXMatrixIdentity( &(pElem->m_mLocalShield) );
	D3DXMatrixIdentity( &(pElem->m_mLocalKnuckle) );


	pElem->m_pObject3D = pObject3D;
	
	memset( &g_szLastFile, 0, sizeof(char)*512 );
	strcpy( g_szLastFile, szFileName );

	if( pObject3D->GetGMOBJECT() == NULL )
	{
		LPCTSTR szStr = Error( "CModelObject::LoadElement에서 튕김 : 읽은 파일명 %s", MakePath( DIR_MODEL, szFileName ) );
		ADDERRORMSG( szStr );
		return FAIL;
	}
	
	if( pObject3D->GetGMOBJECT()->m_bOpacity )
		m_dwRenderEffect = REFF_ALPHA;
	m_bCollObj = pObject3D->IsHaveCollObj();
	if( pObject3D->m_nHavePhysique )	m_bSkin = TRUE;

	if( m_nFrameMax > 0 )	// 이미 모션이나 오브젝을 읽어서 maxframe이 있을때 이번에 읽은 오브젝틔 maxframe하고 비교. 틀리면 에러.
	{
		if( pObject3D->GetMaxFrame() )
			if( m_nFrameMax != pObject3D->GetMaxFrame() )
			{
				LPCTSTR szErr = Error( "CModelObject::LoadElement - %s의 MaxFrame(%d)과 이전에 읽은 MaxFrame(%d)이 다르다.", szFileName, pObject3D->GetMaxFrame(), m_nFrameMax );
				ADDERRORMSG( szErr );
			}
	}
	if( m_nFrameMax == 0 )	// maxframe이 세팅된적이 없을때만 최초 세팅함.
		m_nFrameMax = pObject3D->GetMaxFrame();

	// 애니메이션 속도
	m_fPerSlerp = (float)((int)(pObject3D->m_fPerSlerp * 1000.0f)) / 1000.0f;		// 소숫점 3째 이하에서 오차가 생길수 있으므로 3째까지 에서 잘라버린다,.


	// 이미 본이 로딩되어 있는 상태면 m_pUpdateBone을 오브젝트의 외부뼈대로 등록함.
	if( m_pBone )
		pObject3D->SetExternBone( m_mUpdateBone, m_pBaseBoneInv );

	// 새로 읽은 Mesh의  BB를 기준으로 CModelObject의 BB를 갱신
	// 이부분계산은 현재 정확하지 않다.  Min,Max모두 Local기준인데다가
	// 사람이 창을 쥐고 있을경우 창까지 바운딩 박스에 포함되고 있다.
	if( pObject3D->m_vBBMin.x < m_vMin.x )	m_vMin.x = pObject3D->m_vBBMin.x;
	if( pObject3D->m_vBBMin.y < m_vMin.y )	m_vMin.y = pObject3D->m_vBBMin.y;
	if( pObject3D->m_vBBMin.z < m_vMin.z )	m_vMin.z = pObject3D->m_vBBMin.z;
	if( pObject3D->m_vBBMax.x > m_vMax.x )	m_vMax.x = pObject3D->m_vBBMax.x;
	if( pObject3D->m_vBBMax.y > m_vMax.y )	m_vMax.y = pObject3D->m_vBBMax.y;
	if( pObject3D->m_vBBMax.z > m_vMax.z )	m_vMax.z = pObject3D->m_vBBMax.z;
	SetBB( m_BB.m_vPos, m_vMin, m_vMax );

	// 읽어온 엘리먼트는 this의 뼈대에 부모지정이 안되어 있게 한다.
	// 만약 칼을 읽어왔다면 수동으로 SetParent()하도록 한다.
	pElem->m_nParentIdx = -1;

	if( pObject3D->m_nHavePhysique )	// 피지크를 가지고 있다면 스킨은 버텍스버퍼를 따로 가짐
	{
		if( pElem->m_ppd3d_VB )
			Error( "CModelObject::LoadElement: %s pElem->m_ppd3d_VB is not NULL", szFileName );

		int nTotal = 0;
		pObject3D->SetLOD(0);	nTotal += pObject3D->GetMaxObject();
		pObject3D->SetLOD(1);	nTotal += pObject3D->GetMaxObject();
		pObject3D->SetLOD(2);	nTotal += pObject3D->GetMaxObject();
		pObject3D->SetLOD(0);
		
		if( nTotal <= 0 )
			Error( "%s LoadElement : nTotal=%d", pObject3D->m_szFileName, nTotal );

		pElem->m_ppd3d_VB = new LPDIRECT3DVERTEXBUFFER9[ nTotal ];
		memset( pElem->m_ppd3d_VB, 0, sizeof(LPDIRECT3DVERTEXBUFFER9) * nTotal );
		LPDIRECT3DVERTEXBUFFER9 *ppd3d_VB = pElem->m_ppd3d_VB;
		pObject3D->SetLOD(0);		pElem->m_ppd3d_VB1 = ppd3d_VB;		ppd3d_VB += pObject3D->GetMaxObject();
		pObject3D->SetLOD(1);		pElem->m_ppd3d_VB2 = ppd3d_VB;		ppd3d_VB += pObject3D->GetMaxObject();
		pObject3D->SetLOD(2);		pElem->m_ppd3d_VB3 = ppd3d_VB;		
		pObject3D->SetLOD(0);
		pElem->m_ppd3d_VBSel = pElem->m_ppd3d_VB1;
	}
	
	pObject3D->RestoreDeviceObjects( pElem->m_ppd3d_VB );		// 이걸 여기서 하면 안되는 이유가 있어서 안한건데 그 이유가 기억이 안난다 -_-;;;

	return SUCCESS;
}

#if __VER >= 15 // __GUILD_HOUSE
#ifdef __CLIENT

int CModelObject::LoadClonedElement( LPCTSTR szFileName )
{
	//gmpbigsun: 완젼 복제 모델 생성( 독립 텍스쳐 ) .. 급조함.
	CObject3D* pObject3D = NULL;
	O3D_ELEMENT* pElem = NULL;

	pObject3D	= new CObject3D;
	pObject3D->InitDeviceObjects( m_pd3dDevice );
	if( pObject3D->LoadObject( szFileName ) == FAIL )
	{
		assert( 0 );
		SAFE_DELETE( pObject3D );
		return -1;
	}

	pObject3D->InitDeviceObjects( m_pd3dDevice );

	pElem = &m_Element[ 0 ];
	memset( pElem, 0, sizeof(O3D_ELEMENT) );
	pElem->m_nPartsIdx = 0;
	pElem->m_nParentIdx = -1;
	D3DXMatrixIdentity( &(pElem->m_mLocalRH) );
	D3DXMatrixIdentity( &(pElem->m_mLocalLH) );
	D3DXMatrixIdentity( &(pElem->m_mLocalShield) );
	D3DXMatrixIdentity( &(pElem->m_mLocalKnuckle) );

	pElem->m_pObject3D = pObject3D;

	m_bCollObj = pObject3D->IsHaveCollObj();

	memset( &g_szLastFile, 0, sizeof(char)*512 );
	strcpy( g_szLastFile, szFileName );
	if( pObject3D->m_nHavePhysique )	
		m_bSkin = TRUE;

	if( m_nFrameMax == 0 )	// maxframe이 세팅된적이 없을때만 최초 세팅함.
		m_nFrameMax = pObject3D->GetMaxFrame();

	m_fPerSlerp = (float)((int)(pObject3D->m_fPerSlerp * 1000.0f)) / 1000.0f;		// 소숫점 3째 이하에서 오차가 생길수 있으므로 3째까지 에서 잘라버린다,.

	if( m_nFrameMax > 0 )	// 이미 모션이나 오브젝을 읽어서 maxframe이 있을때 이번에 읽은 오브젝틔 maxframe하고 비교. 틀리면 에러.
	{
		if( pObject3D->GetMaxFrame() )
			if( m_nFrameMax != pObject3D->GetMaxFrame() )
			{
				LPCTSTR szErr = Error( "CModelObject::LoadElement - %s의 MaxFrame(%d)과 이전에 읽은 MaxFrame(%d)이 다르다.", szFileName, pObject3D->GetMaxFrame(), m_nFrameMax );
				ADDERRORMSG( szErr );
			}
	}
	if( m_nFrameMax == 0 )	// maxframe이 세팅된적이 없을때만 최초 세팅함.
		m_nFrameMax = pObject3D->GetMaxFrame();

	// 애니메이션 속도
	m_fPerSlerp = (float)((int)(pObject3D->m_fPerSlerp * 1000.0f)) / 1000.0f;		// 소숫점 3째 이하에서 오차가 생길수 있으므로 3째까지 에서 잘라버린다,.


	// 이미 본이 로딩되어 있는 상태면 m_pUpdateBone을 오브젝트의 외부뼈대로 등록함.
	if( m_pBone )
		pObject3D->SetExternBone( m_mUpdateBone, m_pBaseBoneInv );

	// 새로 읽은 Mesh의  BB를 기준으로 CModelObject의 BB를 갱신
	// 이부분계산은 현재 정확하지 않다.  Min,Max모두 Local기준인데다가
	// 사람이 창을 쥐고 있을경우 창까지 바운딩 박스에 포함되고 있다.
	if( pObject3D->m_vBBMin.x < m_vMin.x )	m_vMin.x = pObject3D->m_vBBMin.x;
	if( pObject3D->m_vBBMin.y < m_vMin.y )	m_vMin.y = pObject3D->m_vBBMin.y;
	if( pObject3D->m_vBBMin.z < m_vMin.z )	m_vMin.z = pObject3D->m_vBBMin.z;
	if( pObject3D->m_vBBMax.x > m_vMax.x )	m_vMax.x = pObject3D->m_vBBMax.x;
	if( pObject3D->m_vBBMax.y > m_vMax.y )	m_vMax.y = pObject3D->m_vBBMax.y;
	if( pObject3D->m_vBBMax.z > m_vMax.z )	m_vMax.z = pObject3D->m_vBBMax.z;
	SetBB( m_BB.m_vPos, m_vMin, m_vMax );

	// 읽어온 엘리먼트는 this의 뼈대에 부모지정이 안되어 있게 한다.
	// 만약 칼을 읽어왔다면 수동으로 SetParent()하도록 한다.
	pElem->m_nParentIdx = -1;

	if( pObject3D->m_nHavePhysique )	// 피지크를 가지고 있다면 스킨은 버텍스버퍼를 따로 가짐
	{
		if( pElem->m_ppd3d_VB )
			Error( "CModelObject::LoadElement: %s pElem->m_ppd3d_VB is not NULL", szFileName );

		int nTotal = 0;
		pObject3D->SetLOD(0);	nTotal += pObject3D->GetMaxObject();
		pObject3D->SetLOD(1);	nTotal += pObject3D->GetMaxObject();
		pObject3D->SetLOD(2);	nTotal += pObject3D->GetMaxObject();
		pObject3D->SetLOD(0);
		
		if( nTotal <= 0 )
			Error( "%s LoadElement : nTotal=%d", pObject3D->m_szFileName, nTotal );

		pElem->m_ppd3d_VB = new LPDIRECT3DVERTEXBUFFER9[ nTotal ];
		memset( pElem->m_ppd3d_VB, 0, sizeof(LPDIRECT3DVERTEXBUFFER9) * nTotal );
		LPDIRECT3DVERTEXBUFFER9 *ppd3d_VB = pElem->m_ppd3d_VB;
		pObject3D->SetLOD(0);		pElem->m_ppd3d_VB1 = ppd3d_VB;		ppd3d_VB += pObject3D->GetMaxObject();
		pObject3D->SetLOD(1);		pElem->m_ppd3d_VB2 = ppd3d_VB;		ppd3d_VB += pObject3D->GetMaxObject();
		pObject3D->SetLOD(2);		pElem->m_ppd3d_VB3 = ppd3d_VB;		
		pObject3D->SetLOD(0);
		pElem->m_ppd3d_VBSel = pElem->m_ppd3d_VB1;
	}
	
	pObject3D->RestoreDeviceObjects( pElem->m_ppd3d_VB );

	return 1;

}
#endif //__CLIENT

#endif //__GUILD_HOUSE

// SetParent( PARTS_LWEAPON, BONE_RHAND );
// nParts의 오브젝트의 부모를 nBoneIdx로 세팅
// 부모를 바꿀때 쓴다.
void	CModelObject::SetParent( int nParts, int nBoneIdx )
{
	O3D_ELEMENT	*pElem = GetParts( nParts );
	if( !pElem )
		return;


	pElem->m_nParentIdx = nBoneIdx;
	pElem->m_mLocalTM = pElem->m_pObject3D->GetGMOBJECT()->m_mLocalTM;

	// 만약 본의 개수가 바뀌었다면 BONE_RHAND의 인덱스도 바껴야 한다. 인간형모델은 본 개수를 통일 시켜야 한다.
	// 본 인덱스를 쉽게 알수 있는 방법은 CModelObject->m_pMotion->m_pBoneInfo[]에 0~32까지를 입력시켜보면서 Bip01 R Hand를 찾는다,
	if( nBoneIdx == GetRHandIdx() )	
		pElem->m_mLocalRH = m_pBone->m_mLocalRH;

	if( nBoneIdx == GetLHandIdx() )	
		pElem->m_mLocalLH = m_pBone->m_mLocalLH;
	
	if( nBoneIdx == GetLArmIdx() )	
		pElem->m_mLocalShield = m_pBone->m_mLocalShield;

	if( nBoneIdx == GetRArmIdx() )
		pElem->m_mLocalKnuckle = m_pBone->m_mLocalKnuckle;
}

//
// nParts에 텍스쳐파일 szBitmap을 멀티텍스쳐로 등록
// 기존 텍스쳐를 파괴할 필요는 없다.
// 반드시 LoadMesh()이후에 되어야 한다.
//
void	CModelObject::SetTextureMulti( LPCTSTR szBitmap, int nParts )
{
#if !defined(__WORLDSERVER)
	D3DMATERIAL9	d3dmtrl;
	MATERIAL		*mtrl;

	mtrl = g_TextureMng.AddMaterial( m_pd3dDevice, &d3dmtrl, szBitmap );
	O3D_ELEMENT* pParts = GetParts(nParts);

	if( pParts )
		pParts->m_pTextureEx = mtrl->m_pTexture;
#endif
}

//
void	CModelObject::ClearTextureEx( int nParts )
{
	O3D_ELEMENT* pParts = GetParts(nParts);
	
	if( pParts )
		pParts->m_pTextureEx = NULL;
}

//
void	CModelObject::ChangeTexture( int nParts, LPCTSTR szSrc, LPCTSTR szDest )
{
#ifdef __WORLDSERVER
	return;
#endif

	O3D_ELEMENT* pParts = GetParts(nParts);
	if( pParts )
		pParts->m_pObject3D->ChangeTexture( szSrc, szDest );
}

#ifndef __WORLDSERVER
//---------------------------------------------------------------------------------------------
//
// Render()를 부를때 내부에서 SetTransform()을 해주고 외부에선 매트릭스만 넘기는것을 원칙으로 하자.
// 넘겨주는 매트릭스는 오브젝트의 원점을 넘겨준다.
// 가령 인간의 경우는 센터부분이다.
//
FLOAT   g_fDiffuse[4];
FLOAT   g_fAmbient[4];
/*
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
만약에 Render했는데 화면에 안나온다!!! 그러면!
static D3DXVECTOR4 vConst( 1.0f, 1.0f, 1.0f, 100.0f );
m_pd3dDevice->SetVertexShaderConstantF( 95, (float*)&vConst, 1 );
SetTransformView( matView );
SetTransformProj( matProj );
SetDiffuse( 1.0, 1.0, 1.0 );
SetAmbient( 1.0, 1.0, 1.0 );

이걸 렌더 하기전에 불러줬는지 확인해라!!!!!!!!!!!!!!!!!!!!! 크아아악!
좆도 고생했네!

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
 */
int		CModelObject::Render( LPDIRECT3DDEVICE9 pd3dDevice, const D3DXMATRIX *mWorld )
{
#ifdef	__WORLDSERVER
	return 1;
#else
	CObject3D	*pObject3D;
	O3D_ELEMENT	*pElem;
	int		i;
	int		nNextFrame;
	D3DXMATRIX	m1;

#ifndef __CLIENT						// 게임클라이언트가 아닐때...
	if( IsEmptyElement() == TRUE )		// 모델이 로드가 안되어 있으면 걍 리턴
		return 1;
#endif

#ifdef	_DEBUG
	if( m_mUpdateBone && g_pSkiningVS == NULL )
		Error( "CMyD3DApplication::RestoreDeviceObjects()에 CreateSkiningVS()를 호출하시오.\r\nCMyD3DApplication::InvalidateDeviceObjects()에는 DeleteSkiningVS()를 호출하시오.\r\n혹은 *.vsh파일을 읽지 못했다." );
	if( m_nLoop == 0 )
		Error( "경고 : %s : CModelObject::m_nLoop가 지정되지 않았다.", m_pMotion->m_szName );
#endif

	if( m_pMotion )		// m_pMotion이 널이라면 m_mUpdateBone도 널이다.
		m_pMotion->AnimateBone( m_mUpdateBone, m_pMotionOld, m_fFrameCurrent, GetNextFrame(), m_nFrameOld, m_bMotionBlending, m_fBlendWeight );		// 일단 뼈대가 있다면 뼈대 애니메이션 시킴

	if( m_pBone )		// m_pBone이 있다면 뼈대가 있다는 얘기. VS를 써야 한다.
	{
		D3DXMATRIX *pmBones;
		D3DXMATRIX mWorldTranspose;
		D3DXMATRIX *pmBonesInv = m_pBaseBoneInv ;
		BONE	*pBoneList = m_pBone->m_pBones;
		pmBones = m_mUpdateBone;

		if( m_pBone->m_bSendVS )	// 뼈대개수가 MAX_VS_BONE이하라서 한번에 다 전송한다.
		{
			int		nMaxBone = m_pBone->m_nMaxBone;

			if( nMaxBone > MAX_VS_BONE )	
				Error( "CModelObject::Render : overflow bone count - %d", nMaxBone );

			for( i = 0; i < nMaxBone; i ++ )	// MAX_VS_BONE개 이하	
			{
				mWorldTranspose = pmBonesInv[i] * pmBones[i];				
#ifdef	__YENV		
				CString str;
				str.Format( "mBoneMatrix[%d]", i );
				HRESULT hr = g_Neuz.m_pEffect->SetMatrix( str, &mWorldTranspose );
#else //__YENV
				D3DXMatrixTranspose( &mWorldTranspose, &mWorldTranspose );		// 매트릭스를 돌린다음.
				m_pd3dDevice->SetVertexShaderConstantF( i * 3, (float*)&mWorldTranspose, 3 );		// 상수레지스터에 집어넣음.
#endif //__YENV		
			}
		}
		D3DXMATRIX	mView, mProj;
		D3DXMATRIX	mViewProj, mViewProjTranspose, mInvWorld;

		D3DXVECTOR4 vLight = s_vLight;
		D3DXVECTOR4 vLightPos = s_vLightPos;

		mViewProj = *mWorld * s_mView * s_mProj;
		
		D3DXMatrixTranspose( &mViewProjTranspose, &mViewProj );
		D3DXMatrixTranspose( &mWorldTranspose, mWorld );

		D3DXMatrixInverse( &mInvWorld, NULL, mWorld );
		D3DXVec4Transform( &vLight, &vLight, &mInvWorld );
		D3DXVec4Normalize( &vLight, &vLight );
		D3DXVec4Transform( &vLightPos, &vLightPos, &mInvWorld );

#ifdef __YENV
		/*
		// 투영 설정...
		g_Neuz.m_pEffect->SetMatrix( g_Neuz.m_hmWVP, &mViewProjTranspose );
		
		// 라이트 위치 설정
		D3DXVECTOR4 v;
		D3DXVECTOR4 vLight_Pos = s_vLight;
		D3DXMATRIX mLocal;
		D3DXMatrixInverse( &mLocal, NULL, &mViewProjTranspose );						
		D3DXVec4Transform( &v, &vLight_Pos, &mLocal );						// 로컬좌표로 변환
		D3DXVec3Normalize( (D3DXVECTOR3*)&v, (D3DXVECTOR3*)&v );			// 정규화
		v.w = -0.6f;														// 환경광의 밝기(Ambint) Def : -0.3f
		
		// 라이트 방향 설정
		g_Neuz.m_pEffect->SetVector( g_Neuz.m_hvLightDir, &v );
		
		g_Neuz.m_pEffect->SetVector( g_Neuz.m_hvDiffuse, (D3DXVECTOR4*)&s_fDiffuse[0] );	
		g_Neuz.m_pEffect->SetVector( g_Neuz.m_hvAmbient, (D3DXVECTOR4*)&s_fAmbient[0] );
		*/
#else //__YENV
		m_pd3dDevice->SetVertexShaderConstantF( 84, (float*)&mViewProjTranspose, 4 );
//		m_pd3dDevice->SetVertexShaderConstantF( 88, (float*)&mWorldTranspose, 4 );
//		m_pd3dDevice->SetVertexShaderConstantF( 88, (float*)&vEyePos,  1 );		// specular use
//		m_pd3dDevice->SetVertexShaderConstantF( 89, (float*)&fSpecular, 1 );	// specular use
//		m_pd3dDevice->SetVertexShaderConstantF( 90, (float*)&fLightCol, 1 );	// specular use
		m_pd3dDevice->SetVertexShaderConstantF( 91, (float*)&vLightPos, 1 );
		m_pd3dDevice->SetVertexShaderConstantF( 92, (float*)&vLight,   1 );
		m_pd3dDevice->SetVertexShaderConstantF( 93, (float*)&s_fDiffuse, 1 );
		m_pd3dDevice->SetVertexShaderConstantF( 94, (float*)&s_fAmbient, 1 );
//		m_pd3dDevice->SetVertexShaderConstant( 95, &vConst, 1 );
#endif //__YENV
	}

	if( m_nNoEffect == 0 )
	{
		// 기본 설정 
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
		pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL   );		
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA  );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		
	}
	DWORD dwBlendFactor = m_dwColor | ( m_dwBlendFactor << 24 );
	// 오브젝트의 반투명 효과 세팅 
	if( m_dwBlendFactor < 255 || m_dwColor )
	{
		pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, dwBlendFactor );
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );

		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
	}

	nNextFrame = GetNextFrame();
	pd3dDevice->SetMaterial( g_TextureMng.GetMaterial( pd3dDevice, 0 ) );

	D3DXVECTOR3 vec3LightBackup = D3DXVECTOR3( s_vLight[0], s_vLight[1], s_vLight[2] );
	D3DXVECTOR4 vec4Diffuse = D3DXVECTOR4( s_fDiffuse[0], s_fDiffuse[1], s_fDiffuse[2], s_fDiffuse[3] );;
	
	// 엘리먼트엔 스킨,일반,모핑 심지어는 파티클까지도 포함될수있다.
	for( i = 0; i < MAX_ELEMENT; i ++ )
	{
		pElem = &m_Element[i];		

		pObject3D = pElem->m_pObject3D;
		if( pObject3D == NULL )		
			continue;

		if( pElem->m_nEffect & XE_HIDE )	
			continue;

		pObject3D->m_nNoTexture = 0; // m_nNoTexture;
		pObject3D->m_nNoEffect = m_nNoEffect;
		// 뼈대에 링크되어 있다.
		if( pElem->m_nParentIdx != -1 )		// 본의 인덱스를 뜻한다.
		{
			// 만약 본의 개수가 바뀌었다면 ParentIdx도 틀려졌기 때문에 바꿔줘야 한다.
			m1 = m_mUpdateBone[ pElem->m_nParentIdx ] * *mWorld;		// 뼈대에 링크되어 있다면 뼈대위치가 센터가 된다.
			if( pElem->m_nParentIdx == GetRHandIdx() )
				m1 = pElem->m_mLocalRH * m1;
			else if( pElem->m_nParentIdx == GetLHandIdx() )
				m1 = pElem->m_mLocalLH * m1;
			else if( pElem->m_nParentIdx == GetLArmIdx() )
				m1 = pElem->m_mLocalShield * m1;
			else if( pElem->m_nParentIdx == GetRArmIdx() )
				m1 = pElem->m_mLocalKnuckle * m1;
		}
		else
			m1 = *mWorld;

		if( pElem->m_nPartsIdx == PARTS_HAIR )
		{
			//gmpbigsun : 
			memcpy( g_fDiffuse, s_fDiffuse, sizeof(FLOAT)*4 );
			memcpy( g_fAmbient, s_fAmbient, sizeof(FLOAT)*4 );

#ifndef __YENV
			SetDiffuse( 0, 0, 0 );
#endif //__YENV
			
#ifdef __CLIENT
			if( s_bLight )
			{
				float fblack = (CWorld::m_light.Diffuse.r*g_fHairLight) + (CWorld::m_light.Diffuse.g*g_fHairLight) + (CWorld::m_light.Diffuse.b*g_fHairLight);
				fblack = fblack / 3.0f;
				fblack *= 0.6f;

#if __VER >= 14 // __BS_FIX_HAIR_AMBIENT	// ambient 설정값이 너무 낮을경우 거의 검은색에 가까워지는걸 방지 
				if( fblack < 0.3f )		
					fblack = 0.3f;
#endif // __BS_FIX_HAIR_AMBIENT
				SetAmbient( min( pObject3D->m_fAmbient[0]*fblack, 1.0f ), min( pObject3D->m_fAmbient[1]*fblack, 1.0f ), min( pObject3D->m_fAmbient[2]*fblack, 1.0f ) );
			}
			else
#endif // CLIENT
				SetAmbient( pObject3D->m_fAmbient[0], pObject3D->m_fAmbient[1], pObject3D->m_fAmbient[2] );
			
#ifdef __YENV
			SetDiffuse( pObject3D->m_fAmbient[0], pObject3D->m_fAmbient[1], pObject3D->m_fAmbient[2] );
#endif //__YENV
				
#ifdef __YENV
			g_Neuz.m_pEffect->SetVector( g_Neuz.m_hvDiffuse, (D3DXVECTOR4*)&s_fDiffuse[0] );	
			g_Neuz.m_pEffect->SetVector( g_Neuz.m_hvAmbient, (D3DXVECTOR4*)&s_fAmbient[0] );			
#else //__YENV
			pd3dDevice->SetVertexShaderConstantF( 93, (float*)&s_fDiffuse, 1 );
			pd3dDevice->SetVertexShaderConstantF( 94, (float*)&s_fAmbient, 1 );
#endif //__YENV
		} else
		if( pElem->m_nPartsIdx == PARTS_CLOAK )
		{
			g_ModelGlobal.SetTexture( m_pCloakTexture );		// 외부지정 망토텍스쳐가 있다면.
		}

		if( m_pBone )
			pObject3D->SetExternBone( m_mUpdateBone, m_pBaseBoneInv );	// 외장본이 있다면 그것을 넘겨준다.
		
		if( m_SparkInfo.m_bUsed )
		{
			SetLightVec( m_SparkInfo.m_v3SparkDir );
			
			D3DXVECTOR3 vec3Diffuse;
			D3DXVec3Lerp( &vec3Diffuse, &D3DXVECTOR3(0.0f,0.0f,0.0f), &m_SparkInfo.m_v3Color, m_SparkInfo.m_fLerp );
			SetDiffuse( vec3Diffuse.x, vec3Diffuse.y, vec3Diffuse.z );
		}
		
		pObject3D->Render( pd3dDevice, pElem->m_ppd3d_VBSel, m_fFrameCurrent, nNextFrame, &m1, pElem->m_nEffect, dwBlendFactor );

		pObject3D->m_nNoEffect = 0;

		if( pElem->m_nPartsIdx == PARTS_HAIR )
		{
			memcpy( s_fDiffuse, g_fDiffuse, sizeof(FLOAT)*4 );
			memcpy( s_fAmbient, g_fAmbient, sizeof(FLOAT)*4 );
			
#ifdef __YENV
			g_Neuz.m_pEffect->SetVector( g_Neuz.m_hvDiffuse, (D3DXVECTOR4*)&s_fDiffuse[0] );	
			g_Neuz.m_pEffect->SetVector( g_Neuz.m_hvAmbient, (D3DXVECTOR4*)&s_fAmbient[0] );			
#else //__YENV		
			pd3dDevice->SetVertexShaderConstantF( 93, (float*)&s_fDiffuse, 1 );
			pd3dDevice->SetVertexShaderConstantF( 94, (float*)&s_fAmbient, 1 );
#endif //__YENV
		} else

		if( pElem->m_nPartsIdx == PARTS_CLOAK )
			g_ModelGlobal.SetTexture( NULL );
	}

#ifdef __ATTACH_MODEL
	RenderAttachModel(pd3dDevice, mWorld);
#endif //__ATTACH_MODEL

	// 상태 해제
	if( m_dwBlendFactor < 255 || m_dwColor )
	{
		if( m_nNoEffect == 0 )
		{
			pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB( 255, 0, 0, 0) );
			pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
			pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
			pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
		}
	}

	SetLightVec( vec3LightBackup );
	SetDiffuse( vec4Diffuse.x, vec4Diffuse.y, vec4Diffuse.z );

	return 1;
#endif // !__WORLDSERVER
}
#ifdef __NEW_WEAPON_GLOW
void CModelObject::RenderEffect( LPDIRECT3DDEVICE9 pd3dDevice, const D3DXMATRIX *mWorld, DWORD dwItemKind3, DWORD dwItemKind3Right, int nLevelL, int nLeveR )
#else // __NEW_WEAPON_GLOW
void CModelObject::RenderEffect( LPDIRECT3DDEVICE9 pd3dDevice, const D3DXMATRIX *mWorld, DWORD dwItemKind3, int nLevelL, int nLeveR )
#endif // __NEW_WEAPON_GLOW
{

#ifdef __SFX_OPT
	if(g_Option.m_nSfxLevel <= 0 && dwItemKind3 != IK3_YOYO) return;
#endif

	O3D_ELEMENT	*pElem;
	int	i;
//#ifdef __CSC_ENCHANT_EFFECT_2
	BOOL bYoYoAttakLEquip = FALSE;
//#endif //__CSC_ENCHANT_EFFECT_2
	if( m_nNoEffect == 0 )
	{
		// 검광 렌더.
		if( m_pForce && m_pForce->m_nMaxSpline > 0 )	m_pForce->Draw( pd3dDevice, mWorld );
		if( m_pForce2 && m_pForce2->m_nMaxSpline > 0 )	m_pForce2->Draw( pd3dDevice, mWorld );
		
#ifdef __CLIENT
		if( CObj::GetActiveObj() )
		{
			int nEffect, nLevel, nLevel2;
			nLevel2 = nLeveR;

			pElem = GetParts( PARTS_RWEAPON );

#ifdef __SFX_OPT
			if( g_Option.m_nSfxLevel > 0 && pElem && (pElem->m_nEffect & XE_HIDE) == 0 )			// 하이드가 아닐때만 출력해야함
#else //__SFX_OPT
			if( pElem && (pElem->m_nEffect & XE_HIDE) == 0 )			// 하이드가 아닐때만 출력해야함
#endif //__SFX_OPT
			{
				nEffect = pElem->m_nEffect & 0x00FFFFFF;	// 하위 24비트
				nLevel = (pElem->m_nEffect >> 24);			// 상위 8비트
#ifdef __NEW_WEAPON_GLOW
				if( nEffect & XE_ITEM_FIRE )
					CreateWeaponEffect( PARTS_RWEAPON, dwItemKind3Right, mWorld, PE_FIRE, nLevel, nLevel2 );
				else
				if( nEffect & XE_ITEM_ELEC )
					CreateWeaponEffect( PARTS_RWEAPON, dwItemKind3Right, mWorld, PE_ELEC, nLevel, nLevel2 );
				else
				if( nEffect & XE_ITEM_WATER )
					CreateWeaponEffect( PARTS_RWEAPON, dwItemKind3Right, mWorld, PE_WATER, nLevel, nLevel2 );
				else
				if( nEffect & XE_ITEM_WIND )
					CreateWeaponEffect( PARTS_RWEAPON, dwItemKind3Right, mWorld, PE_WIND, nLevel, nLevel2 );
				else
				if( nEffect & XE_ITEM_EARTH )
					CreateWeaponEffect( PARTS_RWEAPON, dwItemKind3Right, mWorld, PE_EARTH, nLevel, nLevel2 );
				else
				if( nEffect & XE_ITEM_FIRE_AL )
					CreateWeaponEffect( PARTS_RWEAPON, dwItemKind3Right, mWorld, PE_FIRE_AL, nLevel, nLevel2 );
				else
				if( nEffect & XE_ITEM_ELEC_AL )
					CreateWeaponEffect( PARTS_RWEAPON, dwItemKind3Right, mWorld, PE_ELEC_AL, nLevel, nLevel2 );
				else
				if( nEffect & XE_ITEM_WATER_AL )
					CreateWeaponEffect( PARTS_RWEAPON, dwItemKind3Right, mWorld, PE_WATER_AL, nLevel, nLevel2 );
				else
				if( nEffect & XE_ITEM_WIND_AL )
					CreateWeaponEffect( PARTS_RWEAPON, dwItemKind3Right, mWorld, PE_WIND_AL, nLevel, nLevel2 );
				else
				if( nEffect & XE_ITEM_EARTH_AL )
					CreateWeaponEffect( PARTS_RWEAPON, dwItemKind3Right, mWorld, PE_EARTH_AL, nLevel, nLevel2 );
				// reg
				if( nEffect & XE_ITEM_GEN )
					CreateWeaponEffect( PARTS_RWEAPON, dwItemKind3Right, mWorld, PE_GEN, nLevel, nLevel2 );
				else
				if( nEffect & XE_ITEM_GEN_AL )
					CreateWeaponEffect( PARTS_RWEAPON, dwItemKind3Right, mWorld, PE_GEN_AL, nLevel, nLevel2 );
#else // __NEW_WEAPON_GLOW
				if( nEffect & XE_ITEM_FIRE )
				{
					CreateParticle( PARTS_RWEAPON, mWorld, PE_FIRE, nLevel, nLevel2 );
				}
				else
				if( nEffect & XE_ITEM_ELEC )
				{
			#if __VER >= 9 // __Y_ADV_ENCHANT_EFFECT
					CreateParticle( PARTS_RWEAPON, mWorld, PE_ELEC, nLevel, nLevel2 );
			#ifdef __CSC_ENCHANT_EFFECT_2
					RenderItemElec_Adv( PARTS_RWEAPON, mWorld, nLevel2 );
			#endif //__CSC_ENCHANT_EFFECT_2
			#else //__Y_ADV_ENCHANT_EFFECT
					RenderItemElec( PARTS_RWEAPON, mWorld, nLevel );
			#endif //__Y_ADV_ENCHANT_EFFECT
				}
				else
				if( nEffect & XE_ITEM_WATER )
				{
					CreateParticle( PARTS_RWEAPON, mWorld, PE_WATER, nLevel, nLevel2 );
				}
				else
				if( nEffect & XE_ITEM_WIND )
				{
					CreateParticle( PARTS_RWEAPON, mWorld, PE_WIND, nLevel, nLevel2 );
				}
				else
				if( nEffect & XE_ITEM_EARTH )
				{
					CreateParticle( PARTS_RWEAPON, mWorld, PE_EARTH, nLevel, nLevel2 );
				}
			#if __VER >= 9 // __Y_ADV_ENCHANT_EFFECT
				else
				if( nEffect & XE_ITEM_GEN )
				{
					CreateParticle( PARTS_RWEAPON, mWorld, PE_GEN, nLevel, nLevel2 );
				}
				else
				if( nEffect & XE_ITEM_GEN_AL )
				{
					CreateParticle( PARTS_RWEAPON, mWorld, PE_GEN_AL, nLevel, nLevel2 );
				}
				else
				if( nEffect & XE_ITEM_FIRE_AL )
				{
					CreateParticle( PARTS_RWEAPON, mWorld, PE_FIRE_AL, nLevel, nLevel2 );
				}
				else
				if( nEffect & XE_ITEM_ELEC_AL )
				{
					CreateParticle( PARTS_RWEAPON, mWorld, PE_ELEC_AL, nLevel, nLevel2 );
			#ifdef __CSC_ENCHANT_EFFECT_2
					RenderItemElec_Adv( PARTS_RWEAPON, mWorld, nLevel2 );
			#endif //__CSC_ENCHANT_EFFECT_2
				}
				else
				if( nEffect & XE_ITEM_WATER_AL )
				{
					CreateParticle( PARTS_RWEAPON, mWorld, PE_WATER_AL, nLevel, nLevel2 );
				}
				else
				if( nEffect & XE_ITEM_WIND_AL )
				{
					CreateParticle( PARTS_RWEAPON, mWorld, PE_WIND_AL, nLevel, nLevel2 );
				}
				else
				if( nEffect & XE_ITEM_EARTH_AL )
				{
					CreateParticle( PARTS_RWEAPON, mWorld, PE_EARTH_AL, nLevel, nLevel2 );
				}
			#endif //__Y_ADV_ENCHANT_EFFECT
#endif // __NEW_WEAPON_GLOW
			}
#ifdef __NEW_WEAPON_GLOW
			if( dwItemKind3Right != IK3_YOYO )
#else  //	__NEW_WEAPON_GLOW
			if( dwItemKind3 != IK3_YOYO )
#endif //  __NEW_WEAPON_GLOW
			{
				pElem = GetParts( PARTS_LWEAPON );
#ifdef __NEW_WEAPON_GLOW
				if( !pElem )
					pElem = GetParts( PARTS_SHIELD );
#endif // __NEW_WEAPON_GLOW
			}
//#ifdef __CSC_ENCHANT_EFFECT_2
			else //요요 일 경우 기본적으로 오른쪽 파츠에 정보를 사용 단 공격시는 다르게 설정
			{				
				O3D_ELEMENT	*pRweaponElem = GetParts( PARTS_RWEAPON );
				O3D_ELEMENT	*pLweaponElem = GetParts( PARTS_LWEAPON );
				if( (pRweaponElem && (pRweaponElem->m_nEffect & XE_HIDE) == 0 ) && //양쪽에 다 착용하고 있을 경우
					(pLweaponElem && (pLweaponElem->m_nEffect & XE_HIDE) == 0 ) )
					pElem = GetParts( PARTS_RWEAPON );
				else if( (pRweaponElem && (pRweaponElem->m_nEffect & XE_HIDE) == 0 ) && //오른쪽만 착용하고 있을 경우
					(pLweaponElem && (pLweaponElem->m_nEffect & XE_HIDE) != 0 ) )
					pElem = GetParts( PARTS_LWEAPON );
				else if( (pRweaponElem && (pRweaponElem->m_nEffect & XE_HIDE) != 0 ) && //왼쪽만 착용하고 있을 경우
					(pLweaponElem && (pLweaponElem->m_nEffect & XE_HIDE) == 0 ) )
				{
					pElem = GetParts( PARTS_RWEAPON );
					bYoYoAttakLEquip = TRUE; //정보는 오른쪽 파츠를 참고하되 아래 XE_HIDE를 패스하기 위해 임시로 사용
				}
			}
#ifdef __SFX_OPT
			if( g_Option.m_nSfxLevel > 0 && pElem && (bYoYoAttakLEquip || (pElem->m_nEffect & XE_HIDE) == 0) )			// 하이드가 아닐때만 출력해야함
#else //__SFX_OPT
			if( pElem && (bYoYoAttakLEquip || (pElem->m_nEffect & XE_HIDE) == 0) )			// 하이드가 아닐때만 출력해야함
#endif //__SFX_OPT
//#else //__CSC_ENCHANT_EFFECT_2
//			if( pElem && (pElem->m_nEffect & XE_HIDE) == 0 )			// 하이드가 아닐때만 출력해야함
//#endif //__CSC_ENCHANT_EFFECT_2
			{
			#ifndef __CSC_ENCHANT_EFFECT_2
				nLevel2 = nLevelL;
			#endif //__CSC_ENCHANT_EFFECT_2
				nEffect = pElem->m_nEffect & 0x00FFFFFF;	// 하위 24비트
				nLevel = (pElem->m_nEffect >> 24);			// 상위 8비트
				
			#if __VER >= 9 // __Y_ADV_ENCHANT_EFFECT
#ifdef __NEW_WEAPON_GLOW
			if( dwItemKind3Right == IK3_YOYO )
			{
				dwItemKind3 = dwItemKind3Right;
				nLevel2 = nLeveR;				
			}
			else
			{
				nLevel2 = nLevelL;
			}
#else  //	__NEW_WEAPON_GLOW
			#ifdef __CSC_ENCHANT_EFFECT_2
			if( dwItemKind3 == IK3_YOYO )
					nLevel2 = nLevelL;
			#else //__CSC_ENCHANT_EFFECT_2
			if( dwItemKind3 == IK3_YOYO )
					nLevel2 =  nLevel;
			#endif //__CSC_ENCHANT_EFFECT_2
			#endif
#endif // __NEW_WEAPON_GLOW
#ifdef __NEW_WEAPON_GLOW			
				if( nEffect & XE_ITEM_FIRE )
					CreateWeaponEffect( PARTS_LWEAPON, dwItemKind3, mWorld, PE_FIRE, nLevel, nLevel2 );
				else
				if( nEffect & XE_ITEM_ELEC )
					CreateWeaponEffect( PARTS_LWEAPON, dwItemKind3, mWorld, PE_ELEC, nLevel, nLevel2 );
				else
				if( nEffect & XE_ITEM_WATER )
					CreateWeaponEffect( PARTS_LWEAPON, dwItemKind3, mWorld, PE_WATER, nLevel, nLevel2 );
				else
				if( nEffect & XE_ITEM_WIND )
					CreateWeaponEffect( PARTS_LWEAPON, dwItemKind3, mWorld, PE_WIND, nLevel, nLevel2 );
				else
				if( nEffect & XE_ITEM_EARTH )
					CreateWeaponEffect( PARTS_LWEAPON, dwItemKind3, mWorld, PE_EARTH, nLevel, nLevel2 );
				else
				if( nEffect & XE_ITEM_FIRE_AL )
					CreateWeaponEffect( PARTS_LWEAPON, dwItemKind3, mWorld, PE_FIRE_AL, nLevel, nLevel2 );
				else
				if( nEffect & XE_ITEM_ELEC_AL )
					CreateWeaponEffect( PARTS_LWEAPON, dwItemKind3, mWorld, PE_ELEC_AL, nLevel, nLevel2 );
				else
				if( nEffect & XE_ITEM_WATER_AL )
					CreateWeaponEffect( PARTS_LWEAPON, dwItemKind3, mWorld, PE_WATER_AL, nLevel, nLevel2 );
				else
				if( nEffect & XE_ITEM_WIND_AL )
					CreateWeaponEffect( PARTS_LWEAPON, dwItemKind3, mWorld, PE_WIND_AL, nLevel, nLevel2 );
				else
				if( nEffect & XE_ITEM_EARTH_AL )
					CreateWeaponEffect( PARTS_LWEAPON, dwItemKind3, mWorld, PE_EARTH_AL, nLevel, nLevel2 );
				// reg
				if( nEffect & XE_ITEM_GEN )
					CreateWeaponEffect( PARTS_LWEAPON, dwItemKind3, mWorld, PE_GEN, nLevel, nLevel2 );
				else
				if( nEffect & XE_ITEM_GEN_AL )
					CreateWeaponEffect( PARTS_LWEAPON, dwItemKind3, mWorld, PE_GEN_AL, nLevel, nLevel2 );
#else // __NEW_WEAPON_GLOW
				if( nEffect & XE_ITEM_FIRE )
				{
					CreateParticle( PARTS_LWEAPON, mWorld, PE_FIRE, nLevel, nLevel2 );
				}
				else
				if( nEffect & XE_ITEM_ELEC )
				{
			#if __VER >= 9 // __Y_ADV_ENCHANT_EFFECT
					CreateParticle( PARTS_LWEAPON, mWorld, PE_ELEC, nLevel, nLevel2 );
			#ifdef __CSC_ENCHANT_EFFECT_2
					RenderItemElec_Adv( PARTS_LWEAPON, mWorld, nLevel2 );
			#endif //__CSC_ENCHANT_EFFECT_2
			#else //__Y_ADV_ENCHANT_EFFECT
					RenderItemElec( PARTS_LWEAPON, mWorld, nLevel );
			#endif //__Y_ADV_ENCHANT_EFFECT
				}
				else
				if( nEffect & XE_ITEM_WATER )
				{
					CreateParticle( PARTS_LWEAPON, mWorld, PE_WATER, nLevel, nLevel2 );
				}
				else
				if( nEffect & XE_ITEM_WIND )
				{
					CreateParticle( PARTS_LWEAPON, mWorld, PE_WIND, nLevel, nLevel2 );
				}
				else
				if( nEffect & XE_ITEM_EARTH )
				{
					CreateParticle( PARTS_LWEAPON, mWorld, PE_EARTH, nLevel, nLevel2 );
				}
			#if __VER >= 9 // __Y_ADV_ENCHANT_EFFECT
				else
				if( nEffect & XE_ITEM_GEN )
				{
					CreateParticle( PARTS_LWEAPON, mWorld, PE_GEN, nLevel, nLevel2 );
				}
				else
				if( nEffect & XE_ITEM_GEN_AL )
				{
					CreateParticle( PARTS_LWEAPON, mWorld, PE_GEN_AL, nLevel, nLevel2 );
				}
				else
				if( nEffect & XE_ITEM_FIRE_AL )
				{
					CreateParticle( PARTS_LWEAPON, mWorld, PE_FIRE_AL, nLevel, nLevel2 );
				}
				else
				if( nEffect & XE_ITEM_ELEC_AL )
				{
					CreateParticle( PARTS_LWEAPON, mWorld, PE_ELEC_AL, nLevel, nLevel2 );
			#ifdef __CSC_ENCHANT_EFFECT_2
					RenderItemElec_Adv( PARTS_LWEAPON, mWorld, nLevel2 );
			#endif //__CSC_ENCHANT_EFFECT_2
				}
				else
				if( nEffect & XE_ITEM_WATER_AL )
				{
					CreateParticle( PARTS_LWEAPON, mWorld, PE_WATER_AL, nLevel, nLevel2 );
				}
				else
				if( nEffect & XE_ITEM_WIND_AL )
				{
					CreateParticle( PARTS_LWEAPON, mWorld, PE_WIND_AL, nLevel, nLevel2 );
				}
				else
				if( nEffect & XE_ITEM_EARTH_AL )
				{
					CreateParticle( PARTS_LWEAPON, mWorld, PE_EARTH_AL, nLevel, nLevel2 );
				}
			#endif //__Y_ADV_ENCHANT_EFFECT
#endif // __NEW_WEAPON_GLOW
			}
		}
#endif
		
		for( i = 0; i < MAX_ELEMENT; i ++ )
		{
			pElem = &m_Element[i];
			pElem->m_nEffect = 0;		// 쓰고나면 초기화 해주자
		}
	}
}
#endif // WORLDSERVER


//
HRESULT CModelObject::InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{ 
	m_pd3dDevice = pd3dDevice;

#ifdef __ATTACH_MODEL
	InitAttachModelDeviceObjects(pd3dDevice);
#endif //__ATTACH_MODEL
	return S_OK; 
}

HRESULT CModelObject::RestoreDeviceObjects()
{
	int		i;
	O3D_ELEMENT	*pElem;
	CObject3D	*pObject3D;

	SetGroup(0);
	for( i = 0; i < MAX_ELEMENT; i ++ )
	{
		pElem = &m_Element[i];
		pObject3D = pElem->m_pObject3D;
		if( pObject3D )
		{
			// pElem이 스킨일땐 m_ppd3d_VB[]에 버텍스버퍼가 담겨서 돌아온다.
			pObject3D->RestoreDeviceObjects( pElem->m_ppd3d_VB );
		}
	}
	
	if( m_pPartsEffect )
		m_pPartsEffect->RestoreDeviceObjects( m_pd3dDevice );
	if( m_pPartsEffect2 )
		m_pPartsEffect2->RestoreDeviceObjects( m_pd3dDevice );
#ifdef __NEW_WEAPON_GLOW
	if( m_pm_pPartsEffectRegular[0] )
		m_pm_pPartsEffectRegular[0]->RestoreDeviceObjects( m_pd3dDevice );
	if( m_pm_pPartsEffectRegular[1] )
		m_pm_pPartsEffectRegular[1]->RestoreDeviceObjects( m_pd3dDevice );
	if( m_pm_pPartsEffectElement[0] )
		m_pm_pPartsEffectElement[0]->RestoreDeviceObjects( m_pd3dDevice );
	if( m_pm_pPartsEffectElement[1] )
		m_pm_pPartsEffectElement[1]->RestoreDeviceObjects( m_pd3dDevice );
#endif // __NEW_WEAPON_GLOW
#if __VER >= 9 // __Y_ADV_ENCHANT_EFFECT
	if( m_pPartsEffect1_Detail )
		m_pPartsEffect1_Detail->RestoreDeviceObjects( m_pd3dDevice );
	if( m_pPartsEffect2_Detail )
		m_pPartsEffect2_Detail->RestoreDeviceObjects( m_pd3dDevice );
#endif //__Y_ADV_ENCHANT_EFFECT

#ifdef __ATTACH_MODEL
	RestoreAttachModelDeviceObjects();
#endif //__ATTACH_MODEL

	return S_OK;
}	
HRESULT CModelObject::InvalidateDeviceObjects()
{
	DeleteDeviceObjects();

	if( m_pPartsEffect )
		m_pPartsEffect->InvalidateDeviceObjects( m_pd3dDevice );
	if( m_pPartsEffect2 )
		m_pPartsEffect2->InvalidateDeviceObjects( m_pd3dDevice );
#ifdef __NEW_WEAPON_GLOW
	if( m_pm_pPartsEffectRegular[0] )
		m_pm_pPartsEffectRegular[0]->InvalidateDeviceObjects( m_pd3dDevice );
	if( m_pm_pPartsEffectRegular[1] )
		m_pm_pPartsEffectRegular[1]->InvalidateDeviceObjects( m_pd3dDevice );
	if( m_pm_pPartsEffectElement[0] )
		m_pm_pPartsEffectElement[0]->InvalidateDeviceObjects( m_pd3dDevice );
	if( m_pm_pPartsEffectElement[1] )
		m_pm_pPartsEffectElement[1]->InvalidateDeviceObjects( m_pd3dDevice );
#endif // __NEW_WEAPON_GLOW
#if __VER >= 9 // __Y_ADV_ENCHANT_EFFECT
	if( m_pPartsEffect1_Detail )
		m_pPartsEffect1_Detail->InvalidateDeviceObjects( m_pd3dDevice );
	if( m_pPartsEffect2_Detail )
		m_pPartsEffect2_Detail->InvalidateDeviceObjects( m_pd3dDevice );
#endif //__Y_ADV_ENCHANT_EFFECT

	return  S_OK;
}	
HRESULT CModelObject::DeleteDeviceObjects()
{
	int		i, j;
	O3D_ELEMENT	*pElem;
	CObject3D				*pObject3D;

	for( i = 0; i < MAX_ELEMENT; i ++ )
	{
		pElem = &m_Element[i];
		pObject3D = pElem->m_pObject3D;
		if( pObject3D )
		{
			if( pElem->m_ppd3d_VB )
			{
				pElem->m_pObject3D->SetLOD(0);
				for( j = 0; j < pElem->m_pObject3D->GetMaxObject(); j ++ )		// 
					SAFE_RELEASE( pElem->m_ppd3d_VB1[j] );
				pElem->m_pObject3D->SetLOD(1);
				for( j = 0; j < pElem->m_pObject3D->GetMaxObject(); j ++ )
					SAFE_RELEASE( pElem->m_ppd3d_VB2[j] );
				pElem->m_pObject3D->SetLOD(2);
				for( j = 0; j < pElem->m_pObject3D->GetMaxObject(); j ++ )
					SAFE_RELEASE( pElem->m_ppd3d_VB3[j] );
				pElem->m_pObject3D->SetLOD(0);
	//				SAFE_DELETE_ARRAY( pElem->m_ppd3d_VB );	// 여기서 이걸 삭제하면 안된다. device 자원만 삭제.
			}
//			pObject3D->DeleteDeviceObjects();
		}
	}

#ifdef __ATTACH_MODEL
	DeleteAttachModelDeviceObjects();
#endif //__ATTACH_MODEL

	return  S_OK;
}	

void	CModelObject::FrameMove( D3DXVECTOR3 *pvSndPos, float fSpeed )
{
#ifdef __CLIENT
	if( m_pMotion )		// CModel::FrameMove에서 프레임이 증가되기전에 검사해봐야 한다.
	{
#if __VER >= 9	//__AI_0509
		MOTION_ATTR* pAttr	= IsAttrSound();
		if( pAttr && pAttr->m_nSndID > 0 && m_nPause == 0 )
			PLAYSND( pAttr->m_nSndID, pvSndPos );

		int i	= ( m_nFrameOld + 1 ) % GetMaxFrame();
		int nFrame	= ( static_cast<int>( m_fFrameCurrent ) + 1 ) % GetMaxFrame();
		while( i != nFrame )
		{
			BOOL bAttr	= FALSE;
			/*
			MOTION_ATTR* pAttr	= IsAttrSound( i );
			if( pAttr && pAttr->m_nSndID > 0 && m_nPause == 0 )
			{
				PLAYSND( pAttr->m_nSndID, pvSndPos );
				bAttr	= TRUE;
			}
			*/
			BOOL bQuake	= IsAttrQuake( (float)( i ) );
			if( bQuake && m_nPause == 0 )
			{
				CMover *pMover	= CMover::GetActiveMover();
				if( pMover )
				{
					ItemProp* pItemProp	= pMover->GetActiveHandItemProp();
					if( pItemProp && pItemProp->dwItemKind3 != IK3_YOYO && !pMover->IsActiveMover() )
					{
						pMover->GetWorld()->m_pCamera->SetQuake( 15 );				
						bAttr	= TRUE;
					}
				}
			}
			if( bAttr )
				break;
			i	= ( i + 1 ) % GetMaxFrame();
		}
#else	// __AI_0509
		MOTION_ATTR *pAttr = IsAttrSound();
		if( pAttr )
			if( pAttr->m_nSndID > 0 && m_nPause == 0 )		// 효과음 속성이 있다면 플레이, pause상태면 사운드 출력 안함
				PLAYSND( pAttr->m_nSndID, pvSndPos );
		BOOL bQuake = IsAttrQuake();
		if( bQuake && m_nPause == 0 )
		{
			CMover *pMover = CMover::GetActiveMover();
			if( pMover )
			{
				ItemProp* pItemProp = pMover->GetActiveHandItemProp();
				
				if( pItemProp && pItemProp->dwItemKind3 != IK3_YOYO && !pMover->IsActiveMover() )
					pMover->GetWorld()->m_pCamera->SetQuake( 15 );				
			}
		}
#endif	// __AI_0509
	} else
	{
		CObject3D *pObject = GetObject3D();
#if __VER >= 9	//__AI_0509
		if( pObject->GetMaxFrame() )
		{
			MOTION_ATTR* pAttr	= pObject->IsAttrSound( m_fFrameCurrent );
			if( pAttr && pAttr->m_nSndID > 0 && m_nPause == 0 )
				PLAYSND( pAttr->m_nSndID, pvSndPos );

			int i	= ( m_nFrameOld + 1 ) % pObject->GetMaxFrame();
			int nFrame	= ( static_cast<int>( m_fFrameCurrent ) + 1 ) % pObject->GetMaxFrame();
			while( i != nFrame )
			{
				BOOL bAttr	= FALSE;
				/*
				MOTION_ATTR* pAttr	= pObject->IsAttrSound( i );
				if( pAttr && pAttr->m_nSndID > 0 && m_nPause == 0 )
				{
					PLAYSND( pAttr->m_nSndID, pvSndPos );
					bAttr	= TRUE;
				}
				*/
				BOOL bQuake	= pObject->IsAttrQuake( (float)( i ) );
				if( bQuake && m_nPause == 0 )
				{
					CMover *pMover	= CMover::GetActiveMover();
					if( pMover && m_nPause == 0 )
					{
						pMover->GetWorld()->m_pCamera->SetQuake( 30, 1.0f );
						bAttr	= TRUE;
					}
				}
				if( bAttr )
					break;
				i	= ( i + 1 ) % GetMaxFrame();
			}
		}
#else	// __AI_0509
		MOTION_ATTR *pAttr = pObject->IsAttrSound( m_fFrameCurrent );
		if( pAttr )
			if( pAttr->m_nSndID > 0 && m_nPause == 0 )		// 효과음 속성이 있다면 플레이, pause상태면 사운드 출력 안함
				PLAYSND( pAttr->m_nSndID, pvSndPos );
			
		BOOL bQuake = pObject->IsAttrQuake(m_fFrameCurrent);
		if( bQuake && m_nPause == 0 )
		{
			CMover *pMover = CMover::GetActiveMover();
			if( pMover && m_nPause == 0 )
			{
				//Error("Quake - pos 2 ");
				pMover->GetWorld()->m_pCamera->SetQuake( 30, 1.0f );
			}
		}
#endif	// __AI_0509
	}
#endif
	
	CModel::FrameMove( NULL, fSpeed );

#if defined(__CPU_UTILDOWN_060502)
	#if defined(__WORLDSERVER)
		return;
	#endif
#endif		

	if( m_pForce )
	{
		if( /*!m_bEndFrame &&*/ m_pForce->m_nMaxSpline && !m_nPause )
		{
			m_pForce->m_nMaxDraw += (int)((MAX_SF_SLERP*2+2) /** fSpeed*/);
			if( m_pForce->m_nMaxDraw > m_pForce->m_nMaxSpline )
			{
				if( m_nLoop & ANILOOP_LOOP )	// 루핑되는 애니일경우는 첨부터 다시 그린다.
					m_pForce->m_nMaxDraw = 0;
				else
					m_pForce->m_nMaxDraw = m_pForce->m_nMaxSpline;
			}
				
		}

		m_pForce->Process();
	}

	if( m_pForce2 )
	{
		if( m_pForce2->m_nMaxSpline && !m_nPause )
		{
			m_pForce2->m_nMaxDraw += (int)((MAX_SF_SLERP*2+2) /** fSpeed*/);
			if( m_pForce2->m_nMaxDraw > m_pForce2->m_nMaxSpline )
			{
				if( m_nLoop & ANILOOP_LOOP )	// 루핑되는 애니일경우는 첨부터 다시 그린다.
					m_pForce2->m_nMaxDraw = 0;
				else
					m_pForce2->m_nMaxDraw = m_pForce2->m_nMaxSpline;
			}
			
		}
		
		m_pForce2->Process();
	}
	
	if( m_bMotionBlending )
	{
		m_fBlendWeight += 0.05f;
		if( m_fBlendWeight >= 1.0f )		// 블렌딩이 1.0까지 다 진행됐으면 블렌딩 해제
			SetMotionBlending( FALSE );
	}

#ifdef __ATTACH_MODEL
	FrameMoveAttachModel();
#endif //__ATTACH_MODEL
}

//
//	검광을 생성
//	m_pMotion의 첫프레임~마지막프레임까지의 검광을 생성한다.
void	CModelObject::MakeSWDForce( int nParts, DWORD dwItemKind3, BOOL bSlow, DWORD dwColor, float fSpeed )
{
#ifdef __CLIENT
	D3DXVECTOR3		v1, v2;
	O3D_ELEMENT		*pElem = GetParts( nParts );		// 오른손 무기의 포인터
	D3DXMATRIX	m1;
	if( !pElem ) 
		return;

	if( pElem->m_pObject3D == NULL )		return;
	
	if( pElem->m_pObject3D->m_vForce1.x == 0 )	return;
	if( pElem->m_pObject3D->m_vForce2.x == 0 )	return;

	// FrameCurrent를 0으로
	m_fFrameCurrent = 0.0f;
	m_nPause = 0;
	m_bEndFrame = FALSE;
	int nLoop = m_nLoop;
	m_nLoop = ANILOOP_1PLAY;
	
	CreateForce( nParts );	// 검광 오브젝트 생성
	CSwordForce *pForce;
	if( nParts == PARTS_RWEAPON )
		pForce = m_pForce;
	else
		pForce = m_pForce2;

	pForce->Clear();
	pForce->m_dwColor = dwColor;

	D3DXMATRIX *pmLocal;
	if( pElem->m_nParentIdx == GetRHandIdx() )
		pmLocal = &pElem->m_mLocalRH;
	else if( pElem->m_nParentIdx == GetLHandIdx() )
		pmLocal = &pElem->m_mLocalLH;
	else if( pElem->m_nParentIdx == GetLArmIdx() )
		pmLocal = &pElem->m_mLocalShield;
	else if( pElem->m_nParentIdx == GetRArmIdx() )
		pmLocal = &pElem->m_mLocalKnuckle;
	

	while(1)
	{
		if( m_pMotion )
			m_pMotion->AnimateBone( m_mUpdateBone, m_pMotionOld, m_fFrameCurrent, GetNextFrame(), m_nFrameOld, m_bMotionBlending, m_fBlendWeight );		// 일단 뼈대가 있다면 뼈대 애니메이션 시킴

		// 무기 WorldTM = 무기LocalTM X 무기부모WorldTM  
		D3DXMatrixMultiply( &m1, pmLocal, &m_mUpdateBone[ pElem->m_nParentIdx ] );
		
		// 칼끝 버텍스의 월드 좌표 계산
		D3DXVec3TransformCoord( &v1, &(pElem->m_pObject3D->m_vForce1), &m1 );
		D3DXVec3TransformCoord( &v2, &(pElem->m_pObject3D->m_vForce2), &m1 );
		pForce->Add( v1, v2 );

		CModel::FrameMove( NULL, fSpeed );
		if( bSlow && IsAttrHit( m_fFrameCurrent ) )
			m_bSlow = TRUE;
		if( m_bEndFrame )
			break;
	}

	pForce->MakeSpline();		// 스플라인 생성
	

	// rewind
	m_fFrameCurrent = 0.0f;
	m_nPause = 0;
	m_bEndFrame = FALSE;
	m_nLoop = nLoop;
	m_bSlow = FALSE;
#endif
}

#ifdef __NEW_WEAPON_GLOW
void	CModelObject::CreateWeaponEffect( int nParts, DWORD dwItemKind3, const D3DXMATRIX *pmWorld, int nType, int nLevel, int nLevel2 )
{
#ifdef __CLIENT

	if( nType >= PE_GEN && nLevel < 3 ) // no element, regular upgrade is below 3
		return;
	if( nType < PE_EARTH_AL && nLevel2 < 3 && nLevel < 3 ) // both element and regular are below 3
		return;

	// get element level
	short nElementLevel = 0;
	if		( nLevel2 >= 20 )	nElementLevel = 5;
	else if ( nLevel2 >= 16 )	nElementLevel = 4;
	else if ( nLevel2 >= 11 )	nElementLevel = 3;
	else if ( nLevel2 >= 6 )	nElementLevel = 2;
	else if	( nLevel2 >= 3 )	nElementLevel = 1;

	// get regular level
	short nRegularLevel = 0;
	if		( nLevel >= 10 )	nRegularLevel = 5;
	else if	( nLevel >= 9 )		nRegularLevel = 4;
	else if	( nLevel >= 7 )		nRegularLevel = 3;
	else if	( nLevel >= 5 )		nRegularLevel = 2;
	else if	( nLevel >= 3)		nRegularLevel = 1;

	O3D_ELEMENT		*pElem = GetParts( nParts );
	if( !pElem )
	{
		if( nParts == 9 )
		{
			pElem = GetParts( PARTS_SHIELD );
			if( !pElem )
				return;
		}
		else
			return;	
	}			
	if( pElem->m_pObject3D == NULL )
		return;	

	// getmatrix
	D3DXMATRIX *pmLocal = NULL;
	if( pElem->m_nParentIdx == GetRHandIdx() )
		pmLocal = &pElem->m_mLocalRH;	
	else
	if( pElem->m_nParentIdx == GetLHandIdx() )
		pmLocal = &pElem->m_mLocalLH;
	else 
	if( pElem->m_nParentIdx == GetLArmIdx() )
		pmLocal = &pElem->m_mLocalShield;
	else
	if( pElem->m_nParentIdx == GetRArmIdx() )
		pmLocal = &pElem->m_mLocalKnuckle;
	assert( pmLocal );

	D3DXMATRIX m1;
	D3DXMatrixIdentity( &m1 );
	D3DXMATRIX m2;
	D3DXMatrixIdentity( &m2 );

	D3DXMatrixMultiply( &m1, pmLocal, &m_mUpdateBone[ pElem->m_nParentIdx ] );
	m2 = m1 * *pmWorld;


	int dwSfxRegular = XI_WEA_SWORD01; // base
	int dwSfxElement = XI_WEA_SWORD01; // element
	float fOffset = 0;

	switch( dwItemKind3 ) // get the type of the weapon
	{
		case IK3_SWD			: dwSfxRegular = XI_WEA_SWORD01;	fOffset = 0.10f; break;
		case IK3_THSWD			: dwSfxRegular = XI_WEA_TWOSWORD01;	fOffset = 0.00f; break;
		case IK3_AXE			: dwSfxRegular = XI_WEA_AXE01;		fOffset = 0.10f; break;
		case IK3_THAXE			: dwSfxRegular = XI_WEA_TWOAXE01;	fOffset = 0.10f; break;
		case IK3_YOYO			: dwSfxRegular = XI_WEA_YOYO01;		fOffset = 0.00f; break;
		case IK3_KNUCKLEHAMMER 	: dwSfxRegular = XI_WEA_KNUCK01;	fOffset = 0.10f; break;
		case IK3_CHEERSTICK		: dwSfxRegular = XI_WEA_STICK01;	fOffset = 0.00f; break;
		case IK3_STAFF			: dwSfxRegular = XI_WEA_STAFF01;	fOffset = 0.00f; break;
		case IK3_CROSSBOW		: dwSfxRegular = XI_WEA_CROSSBOW01;	fOffset = 0.10f; break;
		case IK3_BOW			: dwSfxRegular = XI_WEA_BOW01;		fOffset = 0.00f; break;
		case IK3_WAND			: dwSfxRegular = XI_WEA_WAND01;		fOffset = 0.10f; break;
		case IK3_MAGICBARUNA	: dwSfxRegular = XI_WEA_MAGICBOOK01;fOffset = 0.00f; break;
		case IK3_ZEMBARUNA		: dwSfxRegular = XI_WEA_FORCEZEM01;	fOffset = 0.00f; break;
		default: return; // any other shield
	}
	dwSfxElement = dwSfxRegular; // copy

	switch( nType ) // get element upgrade, if avail
	{
		case PE_FIRE	: dwSfxElement += 5;  break;
		case PE_FIRE_AL	: dwSfxElement += 5;  break;
		case PE_WATER	: dwSfxElement += 10; break;
		case PE_WATER_AL: dwSfxElement += 10; break;
		case PE_WIND	: dwSfxElement += 15; break;
		case PE_WIND_AL	: dwSfxElement += 15; break;
		case PE_ELEC	: dwSfxElement += 20; break;
		case PE_ELEC_AL	: dwSfxElement += 20; break;
		case PE_EARTH	: dwSfxElement += 25; break;
		case PE_EARTH_AL: dwSfxElement += 25; break;		
		default: break; // PE_GEN + PE_GEN_AL
	}
	// render element
	D3DXVECTOR3 v3( 0.0f + fOffset, 0.0f, 0.0f );
	if( nElementLevel > 0 ) // has element
	{
		dwSfxElement += ( nElementLevel - 1 ); // add element upgrade amount

		CPartsWeaponEffect *pEffectElement = NULL;
		if( nParts == PARTS_LWEAPON )
		{
			if( m_pm_pPartsEffectElement[0] && m_pm_pPartsEffectElement[0]->m_nType != nType )
				SAFE_DELETE( m_pm_pPartsEffectElement[0] );
			if( m_pm_pPartsEffectElement[0] == NULL )
				m_pm_pPartsEffectElement[0] = new CPartsWeaponEffect( nType );		
			pEffectElement = (CPartsWeaponEffect *)m_pm_pPartsEffectElement[0];
		} else
		{
			if( m_pm_pPartsEffectElement[1] && m_pm_pPartsEffectElement[1]->m_nType != nType )	
				SAFE_DELETE( m_pm_pPartsEffectElement[1] );
			if( m_pm_pPartsEffectElement[1] == NULL )
				m_pm_pPartsEffectElement[1] = new CPartsWeaponEffect( nType );		
			pEffectElement = (CPartsWeaponEffect *)m_pm_pPartsEffectElement[1];
		}
		pEffectElement->Create( m_pd3dDevice, v3, dwSfxElement );
		pEffectElement->Render( m_pd3dDevice, &m2 );	
	}
	// render reg upgrade
	if( nRegularLevel > 0 ) // has regular upgrade
	{
		dwSfxRegular += ( nRegularLevel - 1);

		CPartsWeaponEffect *pEffectRegular = NULL;

		if( nParts == PARTS_LWEAPON )
		{
			if( m_pm_pPartsEffectRegular[0] && m_pm_pPartsEffectRegular[0]->m_nType != nType )
				SAFE_DELETE( m_pm_pPartsEffectRegular[0] );
			if( m_pm_pPartsEffectRegular[0] == NULL )
				m_pm_pPartsEffectRegular[0] = new CPartsWeaponEffect( nType );
			pEffectRegular = (CPartsWeaponEffect *)m_pm_pPartsEffectRegular[0];
		} else
		{
			if( m_pm_pPartsEffectRegular[1] && m_pm_pPartsEffectRegular[1]->m_nType != nType )	
				SAFE_DELETE( m_pm_pPartsEffectRegular[1] );
			if( m_pm_pPartsEffectRegular[1] == NULL )
				m_pm_pPartsEffectRegular[1] = new CPartsWeaponEffect( nType );
			pEffectRegular = (CPartsWeaponEffect *)m_pm_pPartsEffectRegular[1];
		}
		pEffectRegular->Create( m_pd3dDevice, v3, dwSfxRegular );
		pEffectRegular->Render( m_pd3dDevice, &m2 );	
	}
#endif // client
}
#endif // __NEW_WEAPON_GLOW

void	CModelObject::CreateParticle( int nParts, const D3DXMATRIX *pmWorld, int nType, int nLevel, int nLevel2 )
{
#ifdef __CLIENT
#if __VER < 9 // __Y_ADV_ENCHANT_EFFECT
	if( nLevel < 0 || nLevel > 5 )
		return;
#else //__Y_ADV_ENCHANT_EFFECT

#ifdef __CSC_ENCHANT_EFFECT_2
	int nEffLevel_2 = 0;
	DWORD dwSfx_2 = 0;

#if __VER >= 13 // __EXT_ENCHANT
	if( nLevel2 >= 10 )
		nLevel2 = 10;
#endif //__EXT_ENCHANT

	if( nLevel2 == 10 )		nEffLevel_2 = 5;
	else if( nLevel2 == 9 )	nEffLevel_2 = 4;
	else if( nLevel2 == 8 )	nEffLevel_2 = 3;
	else if( nLevel2 == 7 )	nEffLevel_2 = 2;
	else if( nLevel2 >= 4 )	nEffLevel_2 = 1;	// 4 ~ 6
	else 
		nEffLevel_2 = 0;

	if(nType == PE_GEN && (nEffLevel_2 == 0 && nLevel < 6)) return;
	if(nType <= PE_EARTH && (nEffLevel_2 == 0 && nLevel < 6)) return; 
	
#endif //__CSC_ENCHANT_EFFECT_2
#endif //__Y_ADV_ENCHANT_EFFECT
	O3D_ELEMENT		*pElem = GetParts( nParts );		// 오른손 무기의 포인터

	if( !pElem )
		return;
	
	if( pElem->m_pObject3D == NULL )		return;
	
	if( pElem->m_pObject3D->m_vForce1.x == 0 )	return;
	if( pElem->m_pObject3D->m_vForce2.x == 0 )	return;

#if __VER >= 9 // __CSC_VER9_5
	BOOL IsSecondLine = FALSE;
	D3DXVECTOR3 v2_1( 0.0F, 0.0F, 0.0F );
	D3DXVECTOR3 v2_2( 0.0F, 0.0F, 0.0F );
	D3DXVECTOR3 v2_3( 0.0F, 0.0F, 0.0F );
	if( pElem->m_pObject3D->m_vForce3.x != 0 && pElem->m_pObject3D->m_vForce4.x != 0 )
		IsSecondLine = TRUE;

	if(IsSecondLine)
	{
		v2_1 = pElem->m_pObject3D->m_vForce3;
		v2_2 = pElem->m_pObject3D->m_vForce4;
	}
#endif //__CSC_VER9_5
	D3DXMATRIX m1;
	D3DXMatrixIdentity( &m1 );
	D3DXMATRIX m2;
	D3DXMatrixIdentity( &m2 );
	D3DXVECTOR3 v1( 0.0F, 0.0F, 0.0F );
	D3DXVECTOR3 v2( 0.0F, 0.0F, 0.0F );
	D3DXVECTOR3 v3( 0.0F, 0.0F, 0.0F );
#if __VER >= 13 // __EXT_ENCHANT
	float		fTemp = 0.0f;
#else // __EXT_ENCHANT
	float		fTemp;
#endif // __EXT_ENCHANT
	D3DXMATRIX *pmLocal = NULL;
	if( pElem->m_nParentIdx == GetRHandIdx() )
		pmLocal = &pElem->m_mLocalRH;
	else if( pElem->m_nParentIdx == GetLHandIdx() )
		pmLocal = &pElem->m_mLocalLH;
	else if( pElem->m_nParentIdx == GetLArmIdx() )
		pmLocal = &pElem->m_mLocalShield;
	else if( pElem->m_nParentIdx == GetRArmIdx() )
		pmLocal = &pElem->m_mLocalKnuckle;

	assert( pmLocal );
	D3DXMatrixMultiply( &m1, pmLocal, &m_mUpdateBone[ pElem->m_nParentIdx ] );
	m2 = m1 * *pmWorld;

	v1 = pElem->m_pObject3D->m_vForce1;
	v2 = pElem->m_pObject3D->m_vForce2;
	
#if __VER >= 9 // __Y_ADV_ENCHANT_EFFECT
	D3DXVECTOR3 vScale( 0.0F, 0.0F, 0.0F );
	int nEffCount = MAX_PARTS_FIRE;
#ifdef __SFX_OPT
	switch(g_Option.m_nSfxLevel)
	{
		case 1:
			nEffCount = MAX_PARTS_FIRE/3;
			break;
		case 2:
			nEffCount = MAX_PARTS_FIRE/2;
			break;
		case 3:
			nEffCount = (MAX_PARTS_FIRE/3) * 2;
			break;
		case 4:
			nEffCount = (MAX_PARTS_FIRE/5) * 4;
			break;
		case 5:
			nEffCount = MAX_PARTS_FIRE;
			break;
	};
#endif

	if( m_nEffectCount < nEffCount )
	{
		fTemp = (FLOAT)m_nEffectCount / (FLOAT)nEffCount;

		D3DXVec3Lerp( &v3, &v1, &v2, fTemp );
#if __VER >= 9 // __CSC_VER9_5
		if(IsSecondLine)
			D3DXVec3Lerp( &v2_3, &v2_1, &v2_2, fTemp );
#endif //__CSC_VER9_5
		vScale = D3DXVECTOR3( 0.5f, 0.5f, 0.5f );

		if( fTemp < 0.4f || fTemp > 0.6f )
			vScale = D3DXVECTOR3( 1.1f, 1.1f, 1.1f );

		if( fTemp < 0.25f || fTemp > 0.75f )
			vScale = D3DXVECTOR3( 0.85f, 0.85f, 0.85f );

		if( fTemp < 0.1f || fTemp > 0.9f )
			vScale = D3DXVECTOR3( 0.6f, 0.6f, 0.6f );

		if( fTemp > 0.4f && fTemp < 0.6f )
			vScale = D3DXVECTOR3( 1.2f, 1.2f, 1.2f );

		m_nEffectCount++;
	}
	else
	{
		m_nEffectCount = 0;
	}

#else //__Y_ADV_ENCHANT_EFFECT

	float fSlp = xRandomF(1);
	D3DXVec3Lerp( &v3, &v1, &v2, fSlp );

#endif //__Y_ADV_ENCHANT_EFFECT


	CPartsFire *pFire = NULL;
	
	if( nParts == PARTS_LWEAPON )
	{
		if( m_pPartsEffect2 && m_pPartsEffect2->m_nType != nType )	// 이펙트가 이미 지정되어 있고  같은게 아니라면
			SAFE_DELETE( m_pPartsEffect2 );							// 이펙트 삭제하고 다시 할당.
		if( m_pPartsEffect2 == NULL )
			m_pPartsEffect2 = new CPartsFire( nType );
		
		pFire = (CPartsFire *)m_pPartsEffect2;
	} else
	{
		if( m_pPartsEffect && m_pPartsEffect->m_nType != nType )	// 이펙트가 이미 지정되어 있고  같은게 아니라면
			SAFE_DELETE( m_pPartsEffect );							// 이펙트 삭제하고 다시 할당.
		if( m_pPartsEffect == NULL )
			m_pPartsEffect = new CPartsFire( nType );
		
		pFire = (CPartsFire *)m_pPartsEffect;
	}
	if( (g_nRenderCnt & 3) == 0 )
	{
		FLOAT fScalLevel = 0.0f;

		DWORD dwSfx = 0;
		switch( nType )
		{
	#if __VER >= 9 // __Y_ADV_ENCHANT_EFFECT
			case PE_FIRE_AL : dwSfx = XI_NAT_FIRE01_ADV_AL; break;
			case PE_ELEC_AL : dwSfx = XI_NAT_ELEC01_ADV_AL; break;	// 이경우 발생함...향상버전에서는
			case PE_WATER_AL: dwSfx = XI_NAT_WATER01_ADV_AL; break;
			case PE_WIND_AL : dwSfx = XI_NAT_WIND01_ADV_AL; break;
			case PE_EARTH_AL: dwSfx = XI_NAT_EARTH01_ADV_AL; break;
			case PE_FIRE : dwSfx = XI_NAT_FIRE01_ADV; break;
			case PE_ELEC : dwSfx = XI_NAT_ELEC01_ADV; break;		// 이경우 발생함...향상버전에서는
			case PE_WATER: dwSfx = XI_NAT_WATER01_ADV; break;
			case PE_WIND : dwSfx = XI_NAT_WIND01_ADV; break;
			case PE_EARTH: dwSfx = XI_NAT_EARTH01_ADV; break;
			case PE_GEN_AL: dwSfx = XI_NAT_NONE01_ADV; break;
			case PE_GEN: dwSfx = XI_NAT_NONE01_ADV; break;
			default: dwSfx = XI_NAT_FIRE01_ADV;
	#else //__Y_ADV_ENCHANT_EFFECT
			case PE_FIRE : dwSfx = XI_NAT_FIRE01;	break;
			case PE_ELEC : dwSfx = XI_NAT_FIRE01;	break;	// 이경우는 발생하지 않음.
			case PE_WATER: dwSfx = XI_NAT_WATER01;	break;
			case PE_WIND : dwSfx = XI_NAT_WIND01;	break;
			case PE_EARTH: dwSfx = XI_NAT_EARTH01;	break;
			default: dwSfx = XI_NAT_FIRE01;
	#endif //__Y_ADV_ENCHANT_EFFECT
		}

	#if __VER >= 9 // __Y_ADV_ENCHANT_EFFECT

		if(nLevel > 0)
		{
			switch( nType )
			{
				case PE_FIRE_AL:
				case PE_ELEC_AL:
				case PE_WATER_AL:
				case PE_WIND_AL:
				case PE_EARTH_AL:
				case PE_GEN_AL:
					fScalLevel = prj.m_fEnchantLevelScal[1][nLevel-1];
					break;
				case PE_FIRE:
				case PE_ELEC:
				case PE_WATER:
				case PE_WIND:
				case PE_EARTH:
				case PE_GEN:
					fScalLevel = prj.m_fEnchantLevelScal[0][nLevel-1];
					break;
			}
		}

		if( nLevel2 <=0 )
			nLevel2 = 1;

		dwSfx += (nLevel2-1);
	#else //__Y_ADV_ENCHANT_EFFECT
		dwSfx += nLevel;
	#endif //__Y_ADV_ENCHANT_EFFECT

	#if __VER >= 9 // __Y_ADV_ENCHANT_EFFECT
#ifdef __CSC_ENCHANT_EFFECT_2
		switch(nType)
		{
			case PE_FIRE:
			case PE_FIRE_AL:
				dwSfx_2 = XI_NAT_FIRE01;
				dwSfx_2 += nEffLevel_2;
				break;
			case PE_WATER:
			case PE_WATER_AL:
				dwSfx_2 = XI_NAT_WATER01;
				dwSfx_2 += nEffLevel_2;
				break;
			case PE_WIND:
			case PE_WIND_AL:
				dwSfx_2 = XI_NAT_WIND01;
				dwSfx_2 += nEffLevel_2;
				break;
			case PE_EARTH:
			case PE_EARTH_AL:
				dwSfx_2 = XI_NAT_EARTH01;
				dwSfx_2 += nEffLevel_2;
				break;
		}
#endif //__CSC_ENCHANT_EFFECT_2
#ifdef __CSC_ENCHANT_EFFECT_2
		if(nLevel > 0)
#endif //__CSC_ENCHANT_EFFECT_2
		pFire->Create( m_pd3dDevice, v3, dwSfx, vScale * fScalLevel );	// 해당 sfx로 파티클 생성시킴.
#ifdef __CSC_ENCHANT_EFFECT_2
		if(nEffLevel_2 > 0 && (nType == PE_FIRE || nType == PE_WATER || nType == PE_WIND || nType == PE_EARTH
			|| nType == PE_FIRE_AL || nType == PE_WATER_AL || nType == PE_WIND_AL || nType == PE_EARTH_AL)
			&& (fTemp > 0.1f && fTemp < 0.93f))
			pFire->Create( m_pd3dDevice, v3, dwSfx_2 );	// 해당 sfx로 파티클 생성시킴.
#endif //__CSC_ENCHANT_EFFECT_2
	#if __VER >= 9 // __CSC_VER9_5
		if(IsSecondLine)
		{
#ifdef __CSC_ENCHANT_EFFECT_2
			if(nLevel > 0)
#endif //__CSC_ENCHANT_EFFECT_2
			pFire->Create( m_pd3dDevice, v2_3, dwSfx, vScale * fScalLevel );	// 해당 sfx로 파티클 생성시킴.
#ifdef __CSC_ENCHANT_EFFECT_2
		if(nEffLevel_2 > 0 && (nType == PE_FIRE || nType == PE_WATER || nType == PE_WIND || nType == PE_EARTH
			|| nType == PE_FIRE_AL || nType == PE_WATER_AL || nType == PE_WIND_AL || nType == PE_EARTH_AL)
			&& (fTemp > 0.1f && fTemp < 0.93f))
				pFire->Create( m_pd3dDevice, v2_3, dwSfx_2 );	// 해당 sfx로 파티클 생성시킴.
#endif //__CSC_ENCHANT_EFFECT_2
		}
	#endif //__CSC_VER9_5
	#else //__Y_ADV_ENCHANT_EFFECT
		pFire->Create( m_pd3dDevice, v3, dwSfx );	// 해당 sfx로 파티클 생성시킴.
	#endif //__Y_ADV_ENCHANT_EFFECT
	}

	pFire->Render( m_pd3dDevice, &m2 );

#endif // client
}

void	CModelObject::RenderItemElec_Adv( int nParts, const D3DXMATRIX *pmWorld, int nLevel )
{
#if __VER >= 9 // __Y_ADV_ENCHANT_EFFECT
#ifdef __CLIENT

#ifdef __CSC_ENCHANT_EFFECT_2
#if __VER >= 13 // __EXT_ENCHANT
	if( nLevel >= 10 )		nLevel = 5;
#else //__EXT_ENCHANT
	if( nLevel == 10 )		nLevel = 5;
#endif //__EXT_ENCHANT
	else if( nLevel == 9 )	nLevel = 4;
	else if( nLevel == 8 )	nLevel = 3;
	else if( nLevel == 7 )	nLevel = 2;
	else if( nLevel >= 4 )	nLevel = 1;	// 4 ~ 6
	else 
		nLevel = 0;

	if(nLevel <= 0 || nLevel > 5)
		return;
#endif //__CSC_ENCHANT_EFFECT_2

	O3D_ELEMENT		*pElem = GetParts( nParts );		// 오른손 무기의 포인터

	if( !pElem )
		return;
	
	if( pElem->m_pObject3D == NULL )		return;
	
	if( pElem->m_pObject3D->m_vForce1.x == 0 )	return;
	if( pElem->m_pObject3D->m_vForce2.x == 0 )	return;

#if __VER >= 9 // __CSC_VER9_5
	BOOL IsSecondLine = FALSE;
	D3DXVECTOR3 v2_1, v2_2;
	if( pElem->m_pObject3D->m_vForce3.x != 0 && pElem->m_pObject3D->m_vForce4.x != 0 )
		IsSecondLine = TRUE;

	if(IsSecondLine)
	{
		v2_1 = pElem->m_pObject3D->m_vForce3;
		v2_2 = pElem->m_pObject3D->m_vForce4;
	}
#endif //__CSC_VER9_5
	
	D3DXMATRIX	m1, m2;
	D3DXVECTOR3	v1, v2, v3;
	
	D3DXMATRIX *pmLocal;
	if( pElem->m_nParentIdx == GetRHandIdx() )
		pmLocal = &pElem->m_mLocalRH;
	else if( pElem->m_nParentIdx == GetLHandIdx() )
		pmLocal = &pElem->m_mLocalLH;
	else if( pElem->m_nParentIdx == GetLArmIdx() )
		pmLocal = &pElem->m_mLocalShield;
	else if( pElem->m_nParentIdx == GetRArmIdx() )
		pmLocal = &pElem->m_mLocalKnuckle;
	
	D3DXMatrixMultiply( &m1, pmLocal, &m_mUpdateBone[ pElem->m_nParentIdx ] );
	m2 = m1 * *pmWorld;
	
	v1 = pElem->m_pObject3D->m_vForce1;		// 로컬로 건네주고
	v2 = pElem->m_pObject3D->m_vForce2;

	CPartsBeam *pBeam;
	if( nParts == PARTS_LWEAPON )
	{
		if( m_pPartsEffect2_Detail && m_pPartsEffect2_Detail->m_nType != PE_ELEC )
		{
			SAFE_DELETE( m_pPartsEffect2_Detail );
		}

		if( m_pPartsEffect2_Detail == NULL )
			m_pPartsEffect2_Detail = new CPartsBeam;

		pBeam = (CPartsBeam *)m_pPartsEffect2_Detail;
	} else
	{
		if( m_pPartsEffect1_Detail && m_pPartsEffect1_Detail->m_nType != PE_ELEC )
		{
			SAFE_DELETE( m_pPartsEffect1_Detail );
		}
		
		if( m_pPartsEffect1_Detail == NULL )
			m_pPartsEffect1_Detail = new CPartsBeam;
		
		pBeam = (CPartsBeam *)m_pPartsEffect1_Detail;
	}
	
	pBeam->Render( m_pd3dDevice, &m2, g_ModelGlobal.m_vCameraPos, g_ModelGlobal.m_vCameraForward, v1, v2, nLevel );
#if __VER >= 9 // __CSC_VER9_5
	if(IsSecondLine)
		pBeam->Render( m_pd3dDevice, &m2, g_ModelGlobal.m_vCameraPos, g_ModelGlobal.m_vCameraForward, v2_1, v2_2, nLevel );
#endif //__CSC_VER9_5

#endif // CLIENT
#endif //__Y_ADV_ENCHANT_EFFECT
}


void	CModelObject::RenderItemElec( int nParts, const D3DXMATRIX *pmWorld, int nLevel )
{
#ifdef __CLIENT
	O3D_ELEMENT		*pElem = GetParts( nParts );		// 오른손 무기의 포인터

	if( !pElem )
		return;
	
	if( pElem->m_pObject3D == NULL )		return;
	
	if( pElem->m_pObject3D->m_vForce1.x == 0 )	return;
	if( pElem->m_pObject3D->m_vForce2.x == 0 )	return;
	
	D3DXMATRIX	m1, m2;
	D3DXVECTOR3		v1, v2, v3;
	
	D3DXMATRIX *pmLocal;
	if( pElem->m_nParentIdx == GetRHandIdx() )
		pmLocal = &pElem->m_mLocalRH;
	else if( pElem->m_nParentIdx == GetLHandIdx() )
		pmLocal = &pElem->m_mLocalLH;
	else if( pElem->m_nParentIdx == GetLArmIdx() )
		pmLocal = &pElem->m_mLocalShield;
	else if( pElem->m_nParentIdx == GetRArmIdx() )
		pmLocal = &pElem->m_mLocalKnuckle;
	
	D3DXMatrixMultiply( &m1, pmLocal, &m_mUpdateBone[ pElem->m_nParentIdx ] );
	m2 = m1 * *pmWorld;
	
	v1 = pElem->m_pObject3D->m_vForce1;		// 로컬로 건네주고
	v2 = pElem->m_pObject3D->m_vForce2;

	CPartsBeam *pBeam;
	if( nParts == PARTS_LWEAPON )
	{
		if( m_pPartsEffect2 && m_pPartsEffect2->m_nType != PE_ELEC )
		{
			SAFE_DELETE( m_pPartsEffect2 );
		}

		if( m_pPartsEffect2 == NULL )
			m_pPartsEffect2 = new CPartsBeam;

		pBeam = (CPartsBeam *)m_pPartsEffect2;
	} else
	{
		if( m_pPartsEffect && m_pPartsEffect->m_nType != PE_ELEC )
		{
			SAFE_DELETE( m_pPartsEffect );
		}
		
		if( m_pPartsEffect == NULL )
			m_pPartsEffect = new CPartsBeam;
		
		pBeam = (CPartsBeam *)m_pPartsEffect;
	}
	
	pBeam->Render( m_pd3dDevice, &m2, g_ModelGlobal.m_vCameraPos, g_ModelGlobal.m_vCameraForward, v1, v2, nLevel );
#endif // CLIENT
}

//
//
//
void	CModelObject::SetMotionBlending( BOOL bFlag )
{ 
	if( bFlag == m_bMotionBlending )	return;
	m_bMotionBlending = bFlag;
	m_fBlendWeight = 0.0f;
}


void	CModelObject::GetForcePos( D3DXVECTOR3 *vOut, int nIdx, int nParts, const D3DXMATRIX &mWorld )
{
	D3DXMATRIX *pmLocal;
	O3D_ELEMENT		*pElem = GetParts( nParts );		// 오른손 무기의 포인터
	D3DXMATRIX	m1;
	D3DXVECTOR3		v1;

	if( !pElem )
		return;
	
	if( pElem->m_pObject3D == NULL )		return;
	
	if( pElem->m_pObject3D->m_vForce1.x == 0 )	return;
	if( pElem->m_pObject3D->m_vForce2.x == 0 )	return;
	
	if( pElem->m_nParentIdx == GetRHandIdx() )
		pmLocal = &pElem->m_mLocalRH;
	else if( pElem->m_nParentIdx == GetLHandIdx() )
		pmLocal = &pElem->m_mLocalLH;
	else
	{
		D3DXMatrixIdentity( &m1 );
		pmLocal = &m1;						// 일단 에러는 안나게 이렇게 한다.
	}
		
	
//	if( m_pMotion )
//		m_pMotion->AnimateBone( m_mUpdateBone, m_pMotionOld, m_fFrameCurrent, GetNextFrame(), m_nFrameOld, m_bMotionBlending, m_fBlendWeight );		// 일단 뼈대가 있다면 뼈대 애니메이션 시킴
	
	D3DXMatrixMultiply( &m1, pmLocal, &m_mUpdateBone[ pElem->m_nParentIdx ] );
	if( nIdx == 0 )
		D3DXVec3TransformCoord( &v1, &(pElem->m_pObject3D->m_vForce1), &m1 );
	else
		D3DXVec3TransformCoord( &v1, &(pElem->m_pObject3D->m_vForce2), &m1 );
	
	D3DXVec3TransformCoord( &v1, &v1, &mWorld );
	*vOut = v1;
	
}


void	CModelObject::GetForcePos( D3DXVECTOR3 *vOut, int nIdx, int nParts, const D3DXMATRIX &mWorld, float fScale)
{
	D3DXMATRIX mLocal;
	O3D_ELEMENT		*pElem = GetParts( nParts );		// 오른손 무기의 포인터
	D3DXMATRIX	m1;
	D3DXVECTOR3		v1;

	if( !pElem )
		return;
	
	if( pElem->m_pObject3D == NULL )		return;
	
	if( pElem->m_pObject3D->m_vForce1.x == 0 )	return;
	if( pElem->m_pObject3D->m_vForce2.x == 0 )	return;
	
	if( pElem->m_nParentIdx == GetRHandIdx() )
	{
		mLocal = pElem->m_mLocalRH;
		D3DXMatrixScaling(&mLocal, fScale, fScale, fScale);
	}
	else if( pElem->m_nParentIdx == GetLHandIdx() )
	{
		mLocal = pElem->m_mLocalLH;
		D3DXMatrixScaling(&mLocal, fScale, fScale, fScale);
	}
	else
	{
		D3DXMatrixIdentity( &m1 );
		mLocal = m1;						// 일단 에러는 안나게 이렇게 한다.
	}
		
	
//	if( m_pMotion )
//		m_pMotion->AnimateBone( m_mUpdateBone, m_pMotionOld, m_fFrameCurrent, GetNextFrame(), m_nFrameOld, m_bMotionBlending, m_fBlendWeight );		// 일단 뼈대가 있다면 뼈대 애니메이션 시킴
	
	D3DXMatrixMultiply( &m1, &mLocal, &m_mUpdateBone[ pElem->m_nParentIdx ] );
	if( nIdx == 0 )
		D3DXVec3TransformCoord( &v1, &(pElem->m_pObject3D->m_vForce1), &m1 );
	else
		D3DXVec3TransformCoord( &v1, &(pElem->m_pObject3D->m_vForce2), &m1 );
	
	D3DXVec3TransformCoord( &v1, &v1, &mWorld );
	*vOut = v1;
	
}


// 주먹 중앙의 위치를 계산할때.
void	CModelObject::GetHandPos( D3DXVECTOR3 *vOut, int nParts, const D3DXMATRIX &mWorld )
{
	D3DXMATRIX *pmLocal;
	D3DXMATRIX	m1;
	D3DXVECTOR3		v1;

	int nParentIdx = 0;
	if( m_pBone == NULL )	return;
	if( nParts == PARTS_RWEAPON )
	{
		nParentIdx = GetRHandIdx();
		pmLocal = &m_pBone->m_mLocalRH;
	}
	else if( nParts == PARTS_LWEAPON  )
	{
		nParentIdx = GetLHandIdx();
		pmLocal = &m_pBone->m_mLocalLH;
	}
	else
	{
		D3DXMatrixIdentity( &m1 );
		pmLocal = &m1;						// 일단 에러는 안나게 이렇게 한다.
	}
	
	
//	if( m_pMotion )
//		m_pMotion->AnimateBone( m_mUpdateBone, m_pMotionOld, m_fFrameCurrent, GetNextFrame(), m_nFrameOld, m_bMotionBlending, m_fBlendWeight );		// 일단 뼈대가 있다면 뼈대 애니메이션 시킴
	
	// 애니메이션끝난 본의 매트릭스와 LocalR/LH를 곱해서 최종 매트릭스 구함.
	D3DXMatrixMultiply( &m1, pmLocal, &m_mUpdateBone[ nParentIdx ] );
	v1.x = m1._41;		v1.y = m1._42;		v1.z = m1._43;		// 좌표만 필요함.
	
	D3DXVec3TransformCoord( &v1, &v1, &mWorld );
	*vOut = v1;
	
}

#ifdef __BS_EFFECT_LUA
BOOL CModelObject::GetPosBone( D3DXVECTOR3* pOut, const char* bonename )
{
	//gmpbigsun : 본이름으로 본좌표 추출 
	for( int i =0; i < m_pBone->m_nMaxBone; ++i )
	{
		BONE* pUnitBone = m_pBone->GetBone( i );
		if( !pUnitBone )
		{
			assert( 0 );
			continue;
		}

		if( strcmp( bonename, pUnitBone->m_szName ) == 0 )
		{
			D3DXMATRIX matTemp;
			if( pUnitBone->m_pParent )
				D3DXMatrixMultiply( &matTemp, &pUnitBone->m_mLocalTM, &m_mUpdateBone[ pUnitBone->m_nParentIdx ] );
			else 
				D3DXMatrixMultiply( &matTemp, &pUnitBone->m_mLocalTM, &m_mUpdateBone[ i ] );

			pOut->x = matTemp._41;
			pOut->y = matTemp._42;
			pOut->z = matTemp._43;

			return TRUE;
		}
	}

	return FALSE;
	
}

#endif //__BS_EFFECT_LUA



///////////////////////////////////////////////////////////////////////////////////
//
//
//
D3DXVECTOR3		SplineSlerp( D3DXVECTOR3 *v1, D3DXVECTOR3 *v2, D3DXVECTOR3 *v3, D3DXVECTOR3 *v4, float fSlerp )
{
	D3DXVECTOR3		vResult;

	float	t = fSlerp;
	float	t2 = t * t;
	float	t3 = t2 * t;

	D3DXVECTOR3		m0, m1;
	const float alpha = 0.0f;

	m0 = ((1 - alpha) / 2.0f) * 
		 ((*v2 - *v1) + *v3 - *v2);
	m1 = ((1 - alpha) / 2.0f) *
		 ((*v3 - *v2) + *v4 - *v3);

	vResult = (((2 * t3) - (3 * t2) + 1) * *v2) +
			   ((t3 - (2 * t2) + t) * m0) + 
			   ((t3 - t2) * m1) +
			   (((-2 * t3) + (3 * t2)) *
			   *v3 );

	return vResult;
}

void	CSwordForce::Add( D3DXVECTOR3 v1, D3DXVECTOR3 v2 )
{
	if( m_nMaxVertex >= MAX_SF_SWDFORCE )
	{
		LPCTSTR szErr = Error( "SWDForceAdd : 범위 초과 %d", m_nMaxVertex );
		ADDERRORMSG( szErr );
		return;
	}
	FVF_SWDFORCE*	pList = &m_aList[ m_nMaxVertex ];

	pList->position = v1;
	pList->color = 0xffffffff;

	pList = &m_aList2[ m_nMaxVertex ];
	pList->position = v2;
	pList->color = 0xffffffff;

	m_nMaxVertex ++;
}

// 등록된 키리스트를 스플라인 보간된 리스트로 바꾼다.
// 이렇게 만들어진 리스트를 최종 그린다.
void	CSwordForce::MakeSpline( void )
{
	FVF_SWDFORCE *pKey;
	int		i, j;
	D3DXVECTOR3 vSlp;
	int		a, b, c, d;
	int		nMaxVertex = m_nMaxVertex;

	m_nMaxSpline = 0;
	for( i = 0; i < nMaxVertex-1; i ++ )
	{
		// i ~ i+1사이를 보간한다.  
		a = i - 1;		if( a < 0 )	a = 0;
		b = i;
		c = i+1;
		d = i+2;		if( d >= nMaxVertex )	d = nMaxVertex - 1;
		for( j = 0; j < MAX_SF_SLERP+1; j ++ )
		{
			if( m_nMaxSpline >= MAX_SF_SPLINE )
			{
				LPCTSTR szErr = Error( "SWDForceAdd Spline : 범위 초과 %d", m_nMaxSpline );
				ADDERRORMSG( szErr );
				break;
			}
			pKey = m_aList;
			vSlp = SplineSlerp( &pKey[a].position, &pKey[b].position, &pKey[c].position, &pKey[d].position, (float)j / MAX_SF_SLERP );
			m_aSpline[ m_nMaxSpline ].position = vSlp;
			m_aSpline[ m_nMaxSpline ].color = 0xffffffff;
			m_nMaxSpline ++;
			pKey = m_aList2;
			vSlp = SplineSlerp( &pKey[a].position, &pKey[b].position, &pKey[c].position, &pKey[d].position, (float)j / MAX_SF_SLERP );
			m_aSpline[ m_nMaxSpline ].position = vSlp;
			m_aSpline[ m_nMaxSpline ].color = 0xffffffff;
			m_nMaxSpline ++;
		}
	}
}

void	CSwordForce::Process( void )
{
	FVF_SWDFORCE *pList = m_aSpline;
	int		i;
	int		nRed1 =  0;
	int		nGreen1 =  0;
	int		nBlue1 =   0;
	int		nRed2 = (m_dwColor >> 16) & 0xff;
	int		nGreen2 = (m_dwColor >> 8) & 0xff;
	int		nBlue2 = m_dwColor & 0xff;

	if( m_nMaxSpline == 0 )		return;

	for( i = m_nMaxDraw - 1; i >= 0; i -- )
	{
		if( i % 2 )
			pList[i].color = D3DCOLOR_ARGB( 255, nRed1, nGreen1, nBlue1 ); 
		else
			pList[i].color = D3DCOLOR_ARGB( 255, nRed2, nGreen2, nBlue2 ); 
		
		nRed1   -= 3; if( nRed1   < 0 )	nRed1   = 0;
		nGreen1 -= 3; if( nGreen1 < 0 ) nGreen1 = 0;
		nBlue1  -= 3; if( nBlue1  < 0 )	nBlue1  = 0;

		nRed2   -= 3; if( nRed2   < 0 )	nRed2   = 0;
		nGreen2 -= 3; if( nGreen2 < 0 ) nGreen2 = 0;
		nBlue2  -= 3; if( nBlue2  < 0 )	nBlue2  = 0;
	}

}

//
//
//
void	CSwordForce::Draw( LPDIRECT3DDEVICE9 pd3dDevice, const D3DXMATRIX *mWorld )
{
	pd3dDevice->SetTransform( D3DTS_WORLD, mWorld );
	pd3dDevice->SetVertexShader( NULL );
	pd3dDevice->SetFVF( D3DFVF_SWDFORCE );

    pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );

	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
	pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE   );
	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE   );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
    pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );


	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );

	{
		int		nShare, nRest;
		int		i;
		FVF_SWDFORCE *p = m_aSpline;
		int		nMaxDraw = m_nMaxDraw;

		nMaxDraw -= 2;
		nShare = nMaxDraw / 64;
		nRest  = nMaxDraw % 64;
		for( i = 0; i < nShare; i ++ )			// 프리미티브 갯수에 제한이 있어서 이렇게 했다.
		{
			pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 64, p, sizeof(FVF_SWDFORCE) );
			p += 64;
		}

		if( nRest > 0 )
			pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, nRest, p, sizeof(FVF_SWDFORCE) );

	}

	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   FALSE );
    pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
    pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

}


