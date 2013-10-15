#include "stdafx.h"
#include <stdio.h>
#include "model.h"
#include "Bone.h"
#include "..\_UnhandledException\ExceptionHandler.h"


CBonesMng		g_BonesMng;

////////////////////////////////////////////////////////////////////////////////////
///////////////
///////////////
///////////////
///////////////
///////////////
///////////////
////////////////////////////////////////////////////////////////////////////////////
CBones :: CBones()
{
	Init();
}

CBones :: ~CBones()
{
	Destroy();
}

void	CBones :: Init( void )
{
	m_nID = 0;
	memset( m_szName, 0, sizeof(m_szName) );
	m_nMaxBone = 0;
	m_pBones = NULL;
	m_nRHandIdx = m_nLHandIdx = m_nRArmIdx = m_nLArmIdx = 0;

	D3DXMatrixIdentity( &m_mLocalRH );
	D3DXMatrixIdentity( &m_mLocalLH );
	D3DXMatrixIdentity( &m_mLocalShield );
	D3DXMatrixIdentity( &m_mLocalKnuckle );

	memset( m_vEvent, 0, sizeof(m_vEvent) );
//	memset( m_pEventParent, 0, sizeof(m_pEventParent) );
	memset( m_nEventParentIdx, 0, sizeof(m_nEventParentIdx) );

}

void	CBones :: Destroy( void )
{
	SAFE_DELETE_ARRAY( m_pBones );
	Init();
}

//
//  뼈대들중에서 nID값을 가지는 뼈대를 찾음
//
/*
BONE	*CBones :: FindBone( int nID )
{
	int		i;
	BONE	*pBone = m_pBones;

	i = m_nMaxBone;
	while( i-- )
	{
		if( pBone->m_nID == nID )	return pBone;
		pBone ++;
	}
	
	return NULL;
}
*/

//
//
//
int		CBones :: LoadBone( LPCTSTR szFileName )
{
	int		nNumBone;
	int		i;
	int		nLen;
	int		nVer;

	CResFile resFp;
	BOOL bRet = resFp.Open( MakePath( DIR_MODEL, szFileName ), "rb" );	
	if( bRet == FALSE )		
		return	FAIL;

	resFp.Read( &nVer, 4, 1 );		// version
	if( nVer < VER_BONE )
	{
		Error( "%s의 버전은 %d.  최신버전은 %d", szFileName, nVer, VER_BONE );
		resFp.Close();
		return FAIL;
	}

	// ID읽기를 넣음.
	resFp.Read( &m_nID, 4, 1 );

	if( strlen( szFileName ) > sizeof(m_szName) )
	{
		Error( "%s : file name is too long", szFileName );
		resFp.Close();
		return FAIL;
	}

	// 파일명 카피
	strcpy( m_szName, szFileName );

	resFp.Read( &nNumBone, 4, 1 );			// 본 개수 읽음
	m_nMaxBone = nNumBone;
	m_pBones = new BONE[ nNumBone ];			// 본 개수 만큼 할당
	memset( m_pBones, 0, sizeof(BONE) * nNumBone );		// zero clear

	for( i = 0; i < nNumBone; i ++ )
	{
		resFp.Read( &nLen, 4, 1 );		// 널 포함길이
		resFp.Read( m_pBones[i].m_szName,		nLen, 1 );		// bone node 이름 저장
		resFp.Read( &m_pBones[i].m_mTM,			sizeof(D3DXMATRIX), 1 );			// World(Object) TM
		resFp.Read( &m_pBones[i].m_mInverseTM,	sizeof(D3DXMATRIX), 1 );			// Inv NODE TM
		resFp.Read( &m_pBones[i].m_mLocalTM,		sizeof(D3DXMATRIX), 1 );			// LocalTM
		resFp.Read( &m_pBones[i].m_nParentIdx,	4, 1 );								// parent bone index
		if( strcmpi( m_pBones[i].m_szName + 6, "r hand") == 0 )	// 부모이름이 R Hand면...
			m_nRHandIdx = i;
		if( strcmpi( m_pBones[i].m_szName + 6, "l hand") == 0 )	// 부모이름이 L Hand면...
			m_nLHandIdx = i;
		if( strcmpi(m_pBones[i].m_szName + 6, "l forearm") == 0 )	// 부모이름이 L ForeArm면...
			m_nLArmIdx = i;
		if( strcmpi(m_pBones[i].m_szName + 6, "r forearm") == 0 )	// 부모이름이 R ForeArm면...
			m_nRArmIdx = i;
	}

	resFp.Read( &m_bSendVS, 4, 1 );
	
	// 오른손 무기쥐는 위치의 행렬
	resFp.Read( &m_mLocalRH, sizeof(D3DXMATRIX), 1 );
	resFp.Read( &m_mLocalShield, sizeof(D3DXMATRIX), 1 );
	resFp.Read( &m_mLocalKnuckle, sizeof(D3DXMATRIX), 1 );
	
	// m_vEvent가 추가된 버전.
//	int		nParentIdx[4];
	if( nVer == 5 )
	{
		resFp.Read( m_vEvent, sizeof(D3DXVECTOR3) * 4, 1 );
		resFp.Read( m_nEventParentIdx, sizeof(int) * 4, 1 );
	} else
	if( nVer >= 6 )
	{
		resFp.Read( m_vEvent, sizeof(D3DXVECTOR3) * MAX_MDL_EVENT, 1 );
		resFp.Read( m_nEventParentIdx, sizeof(int) * MAX_MDL_EVENT, 1 );
	}

	if( nVer == 7 )
	{
		resFp.Read( &m_mLocalLH, sizeof(D3DXMATRIX), 1 );
	}

	// 포인터로 할당.
//	for( i = 0; i < 4; i ++ )
//	{
//		m_pEventParent[i] = &m_pBones[ nParentIdx[i] ];
//	}

	resFp.Close();

	// 부모 포인터를 셋팅
	for( i = 0; i < nNumBone; i ++ )
	{
		if( m_pBones[i].m_nParentIdx == -1 )			// 부모가 없으면 부모포인터는 널
			m_pBones[i].m_pParent = NULL;
		else
			m_pBones[i].m_pParent = &m_pBones[ m_pBones[i].m_nParentIdx ];
	}

	return SUCCESS;
}







////////////////////////////////////////////////////////////////////////////////////
///////////////
///////////////
///////////////
///////////////
///////////////
///////////////
////////////////////////////////////////////////////////////////////////////////////
CBonesMng :: CBonesMng()
{
#if __VER < 12 // __LEAK_0827
	Init();
#endif	// __LEAK_0827
}

CBonesMng :: ~CBonesMng()
{
	Destroy();
}


void	CBonesMng :: Init( void )
{
#if __VER < 12 // __LEAK_0827
	int		i;
	m_nSize = 0;
	m_nMax = 0;
	for( i = 0; i < MAX_BONES; i ++ )	m_pBonesAry[i] = NULL;
#endif	// __LEAK_0827
}

void	CBonesMng :: Destroy( void )
{
#if __VER >= 12 // __LEAK_0827
	for( map<string, CBones*>::iterator i = m_mapBones.begin(); i != m_mapBones.end(); ++i )
		safe_delete( i->second );
	m_mapBones.clear();
#else	// __LEAK_0827
	int		i;
	CBones	**pAry = m_pBonesAry;
	CBones	*pBone;

	for( i = 0; i < MAX_BONES; i ++ )
	{
		pBone = *pAry++;
		if( pBone )
		{
			SAFE_DELETE( pBone );
		}
	}
	Init();
#endif	// __LEAK_0827
}

// 본 파일Load요청이 들어오면 파일을 읽어 메모리에 적재한다.
// 적재할땐 중복된것은 이미 로딩되었던 본의 포인터를 리턴한다.
// 파일의 첫머리에 고유 아이디를 넣어서 검색할때 파일이름으로 하지말고 아이디로 하도록 한다.
CBones *CBonesMng :: LoadBone( LPCTSTR szFileName )
{
#if __VER >= 12 // __LEAK_0827
	char sFile[MAX_PATH]	= { 0,};
	strcpy( sFile, szFileName );
	strlwr( sFile );

	map<string, CBones*>::iterator i	= m_mapBones.find( sFile );
	if( i != m_mapBones.end() )
		return i->second;
	// 로딩된게 아니었다면.  실제로 데이타 읽음.
	CBones* pBones	= new CBones;
	if( pBones->LoadBone( szFileName ) == FAIL )
	{
		safe_delete( pBones );
		return NULL;
	}
	bool bResult	= m_mapBones.insert( map<string, CBones*>::value_type( sFile, pBones ) ).second;
	return pBones;
#else	// __LEAK_0827
	int		i;
	CBones **pAry = m_pBonesAry;
	CBones *pBones;
	int		nIdx = -1;

	// 이미 메모리에 적재 되었는지 검색, 동시에 빈곳도 검색
	for( i = 0; i < MAX_BONES; i ++ )
	{
		pBones = *pAry++;
		if( pBones )				// 널이 아닌것은 무슨 본이든 로딩되어 있다는 뜻
		{
			if( strcmpi(pBones->m_szName, szFileName) == 0 )		// 같은걸 찾았으면 그걸 리턴
//			if( pBones->m_nIDstrcmp(pBones->m_szName, szFileName) == 0 )		// 같은걸 찾았으면 그걸 리턴
				return pBones;
		}
		else
		{
			if( nIdx == -1 )		nIdx = i;			// 빈곳이 있으면 가장처음 빈곳이 나온곳을 기억해둠
		}
	}

	pBones = new CBones;
	// 로딩된게 아니었다면.  실제로 데이타 읽음.
	if( pBones->LoadBone( szFileName ) == FAIL )
		return NULL;

	// 읽은 본포인터를 리스트에 등록
	m_pBonesAry[ nIdx ] = pBones;

	m_nMax ++;			// 가진 본 갯수 증가

	return pBones;		// 읽은 본 포인터 리턴
#endif	// __LEAK_0827
}

////////////////////////////////////////////////////////////////////////////////////
///////////////
///////////////
///////////////
///////////////
///////////////
///////////////
////////////////////////////////////////////////////////////////////////////////////
CMotionMng		g_MotionMng;


CMotionMng :: CMotionMng()
{
#if __VER < 12 // __LEAK_0827
	Init();
#endif	// __LEAK_0827
}

CMotionMng :: ~CMotionMng()
{
	Destroy();
}

void	CMotionMng :: Init( void )
{
#if __VER < 12 // __LEAK_0827
	int		i;
	m_nSize = 0;
	m_nMax = 0;
	m_nCachePos = 0;

	for( i = 0; i < MAX_MOTION; i ++ )
		m_pMotionAry[i] = NULL;
	
	for( i = 0; i < MAX_MOTION_CACHE; i ++ )
		m_pCache[i] = NULL;
#endif	// __LEAK_0827
}

void	CMotionMng :: Destroy( void )
{
#if __VER >= 12 // __LEAK_0827
	for( map<string, CMotion*>::iterator i = m_mapMotions.begin(); i != m_mapMotions.end(); ++i )
		safe_delete( i->second );
	m_mapMotions.clear();
#else	// __LEAK_0827
	int		i;
	CMotion		**pAry = m_pMotionAry;
	CMotion		*pMotion;

	for( i = 0; i < MAX_MOTION; i ++ )
	{
		pMotion = *pAry++;
		if( pMotion )
		{
			SAFE_DELETE( pMotion );
		}
	}
	Init();
#endif	// __LEAK_0827
}

//
//
//
CMotion *CMotionMng :: LoadMotion( LPCTSTR szFileName )
{
#if __VER >= 12 // __LEAK_0827
	char sFile[MAX_PATH]	= { 0,};
	strcpy( sFile, szFileName );
	strlwr( sFile );

	map<string, CMotion*>::iterator i	= m_mapMotions.find( sFile );
	if( i != m_mapMotions.end() )
		return i->second;

	// 로딩된게 아니었다면.  실제로 데이타 읽음.
	CMotion* pMotion	= new CMotion;
	if( pMotion->LoadMotion( szFileName ) == FAIL )
	{
		safe_delete( pMotion );
		return NULL;
	}
	bool bResult	= m_mapMotions.insert( map<string, CMotion*>::value_type( sFile, pMotion ) ).second;
	return pMotion;
#else	// __LEAK_0827
	int		i;
	CMotion **pAry, *pMotion;
	int		nIdx = -1;

	// 이미 메모리에 적재 되었는지 검색, 동시에 빈곳도 검색
	// 일단 캐쉬를 검색
	pAry = m_pCache;
	for( i = 0; i < MAX_MOTION_CACHE; i ++ )
	{
		pMotion = *pAry++;
		if( pMotion )
		{
			if( strcmpi(pMotion->m_szName, szFileName) == 0 )		// 같은걸 찾았으면 그걸 리턴
				return pMotion;
		}
	}

	// 캐쉬에서 못찾았으면 리스트에서 검색
	pAry = m_pMotionAry;
	for( i = 0; i < MAX_MOTION; i ++ )
	{
		pMotion = *pAry++;
		if( pMotion )				// 널이 아닌것은 무슨 모션이든 로딩되어 있다는 뜻
		{
			if( strcmpi(pMotion->m_szName, szFileName) == 0 )		// 같은걸 찾았으면 그걸 리턴
				return pMotion;
		} else
		{
			if( nIdx == -1 )		nIdx = i;			// 빈곳이 있으면 가장처음 빈곳이 나온곳을 기억해둠
		}
	}
	if( nIdx == -1 )	
	{
		Error( "%s : 읽을 수 있는 한계를 초과했다", szFileName );
	}

	pMotion = new CMotion;
	// 로딩된게 아니었다면.  실제로 데이타 읽음.
	if( pMotion->LoadMotion( szFileName ) == FAIL )
	{
		safe_delete( pMotion );
		return NULL;
	}
	// 읽은 모션포인터를 리스트에 등록
	m_pMotionAry[ nIdx ] = pMotion;
	
	// 캐시에도 넣음
	m_pCache[ m_nCachePos++ ] = pMotion;	
	if( m_nCachePos >= MAX_MOTION_CACHE )		m_nCachePos = 0;

	m_nMax ++;			// 가진 모션 갯수 증가

	return pMotion;		// 읽은 모션 포인터 리턴
#endif	// __LEAK_0827
}


////////////////////////////////////////////////////////////////////////////////////
///////////////
///////////////
///////////////
///////////////
///////////////
///////////////
////////////////////////////////////////////////////////////////////////////////////

CMotion :: CMotion()
{
	Init();
}

CMotion :: ~CMotion()
{
	Destroy();
}

void	CMotion :: Init( void )
{
	m_pMotion = NULL;
	m_pPath = NULL;
	m_pBoneFrame = NULL;
	m_fPerSlerp = 0.5f;
	m_nMaxEvent = 0;
	m_pBoneInfo = NULL;
	memset( m_vEvent, 0, sizeof(m_vEvent) );
	m_pAttr = NULL;
	m_pBoneInfo = NULL;
}

void	CMotion :: Destroy( void )
{
	int		i;

	SAFE_DELETE_ARRAY( m_pAttr );
	SAFE_DELETE_ARRAY( m_pMotion );
	SAFE_DELETE_ARRAY( m_pPath );
	if( m_pBoneFrame )
	{
		for( i = 0; i < m_nMaxBone; i ++ )
			m_pBoneFrame[i].m_pFrame = NULL;
	}
	SAFE_DELETE_ARRAY( m_pBoneFrame );
	SAFE_DELETE_ARRAY( m_pBoneInfo );

	Init();
}

//
//
//
int		CMotion :: LoadMotion( LPCTSTR szFileName )
{
	int		nNumBone;
	int		nNumFrame;
//	int		nNumSize;
//	int		i, j;
	int		nVer;

	CResFile resFp;
	BOOL bRet = resFp.Open( MakePath( DIR_MODEL, szFileName ), "rb" );
	if( bRet == FALSE )	
	{
		LPCTSTR szStr = Error( "%s : 찾을 수 없음", szFileName );
		ADDERRORMSG( szStr );
		resFp.Close();
		return FAIL;
	}

	resFp.Read( &nVer, 4, 1 );		// version
	if( nVer != VER_MOTION )
	{
		Error( "%s의 버전은 %d.  최신버전은 %d", szFileName, nVer, VER_MOTION );
		resFp.Close();
		return FAIL;
	}

	// ID읽기를 넣음.
	resFp.Read( &m_nID, 4, 1 );

	if( strlen( szFileName ) > sizeof(m_szName) )
	{
		Error( "%s : file name is too long", szFileName );
		resFp.Close();
		return FAIL;
	}

	// 파일명 카피
	strcpy( m_szName, szFileName );
	
	resFp.Read( &m_fPerSlerp, sizeof(float), 1 );		// 
	resFp.Seek( 32, SEEK_CUR );		// reserved

	resFp.Read( &nNumBone, 4, 1 );			// 뼈대 갯수 읽음
	resFp.Read( &nNumFrame, 4, 1 );		// 애니메이션 프레임 개수 읽음
	m_nMaxFrame = nNumFrame;
	m_nMaxBone = nNumBone;

	if( nNumFrame <= 0 )
	{
		Error( "CMotion::LoadMotion : %s read MaxFrame = %d", szFileName, nNumFrame );
		resFp.Close();
		return FAIL;
	}

	// path 
	int nTemp;
	resFp.Read( &nTemp, 4, 1 );	// path정보가 있는가?
	if( nTemp )
	{
		m_pPath = new D3DXVECTOR3[ nNumFrame ];
		resFp.Read( m_pPath, sizeof(D3DXVECTOR3) * nNumFrame, 1 );		// nNumFrame만큼 한방에 읽어버리기.
	}

	//
	// TM_Animation 데이타 읽음.
	ReadTM( &resFp, nNumBone, nNumFrame );
	
	// 프레임 속성 읽음.
	resFp.Read( m_pAttr, sizeof(MOTION_ATTR) * nNumFrame, 1 );

	resFp.Read( &m_nMaxEvent, 4, 1 );	// 이벤트 좌표 저장
	if( m_nMaxEvent > 0 )
		resFp.Read( m_vEvent, sizeof(D3DXVECTOR3) * m_nMaxEvent, 1 );
	

	resFp.Close();

//	m_vPivot = m_pMotion[0].m_vPos;			// Bip01의 POS값을 받음.  골반의 기준 좌표다.

	return SUCCESS;
}

//
//  Motion의 core부분을 읽음. 외부에서 단독 호출 가능.
//
void	CMotion :: ReadTM( CResFile *file, int nNumBone, int nNumFrame )
{
	int		nNumSize;
	int		nFrame;
	int		i;

	m_nMaxBone = nNumBone;		// LoadMotion()에서 불려졌다면 이부분은 필요없으나 ReadTM만 따로 불릴 일이 있으면 이게 필요하다.
	m_nMaxFrame = nNumFrame;

	//--- 뼈대구성정보 읽음.
	m_pBoneInfo = new BONE[ nNumBone ];			// 본 개수 만큼 할당
	memset( m_pBoneInfo, 0, sizeof(BONE) * nNumBone );		// zero clear
	
	int		nLen;
	for( i = 0; i < nNumBone; i ++ )
	{
		file->Read( &nLen, 4, 1 );
		if( nLen > 32 )		
			Error("CMotion::ReadTM - %s bonename is too long", m_szName );

		file->Read( m_pBoneInfo[i].m_szName,	nLen, 1 );		// bone node 이름
		file->Read( &m_pBoneInfo[i].m_mInverseTM,	sizeof(D3DXMATRIX), 1 );			// Inv NODE TM
		file->Read( &m_pBoneInfo[i].m_mLocalTM,		sizeof(D3DXMATRIX), 1 );			// LocalTM
		file->Read( &m_pBoneInfo[i].m_nParentIdx,	4, 1 );								// parent bone index
	}
	
	// 부모 포인터를 셋팅
	for( i = 0; i < nNumBone; i ++ )
	{
		if( m_pBoneInfo[i].m_nParentIdx == -1 )			// 부모가 없으면 부모포인터는 널
			m_pBoneInfo[i].m_pParent = NULL;
		else
			m_pBoneInfo[i].m_pParent = &m_pBoneInfo[ m_pBoneInfo[i].m_nParentIdx ];
	}
	

	file->Read( &nNumSize, 4, 1 );			// 프레임 사이즈 읽음 - 메모리 풀 사이즈
	//--- 모션 읽음.
	m_pMotion		= new TM_ANIMATION[ nNumSize ];		// 메모리 풀
	m_pBoneFrame	= new BONE_FRAME[ nNumBone ];
	m_pAttr			= new MOTION_ATTR[ nNumFrame ];
	memset( m_pAttr, 0, sizeof(MOTION_ATTR) * nNumFrame );	// nNumSize였는데 nNumFrame이 맞는거 같다.
	TM_ANIMATION	*p = m_pMotion;
	int		nCnt = 0;
	
	// 뼈대 수 만큼 루프
	for( i = 0; i < nNumBone; i ++ )
	{
		file->Read( &nFrame, 4, 1 );
		if( nFrame == 1 )		// 1이면 현재 뼈대에 프레임 있음
		{
			m_pBoneFrame[i].m_pFrame = p;
			file->Read( m_pBoneFrame[i].m_pFrame, sizeof(TM_ANIMATION) * nNumFrame, 1 );		// 한방에 읽어버리기.
			p += nNumFrame;
			nCnt += nNumFrame;
//			for( j = 0; j < nNumFrame; j ++ )
//			{
//				file->Read( &(p->m_mAniTM), sizeof(D3DXMATRIX), 1 );
//				file->Read( &(p->m_qRot), sizeof(D3DXQUATERNION), 1 );
//				file->Read( &(p->m_vPos), sizeof(D3DXVECTOR3), 1 );
//				p ++;
//				nCnt ++;
//			}
		} else			// 현재 뼈대에 프레임 없음
		{
			file->Read( &(m_pBoneFrame[i].m_mLocalTM), sizeof(D3DXMATRIX), 1 );			// 프레임이 없으면 LocalTM만 읽고
			m_pBoneFrame[i].m_pFrame = NULL;
			// m_mLocalTM에 넣었으므로 메모리 풀에는 넣을필요 없다.
		}
	}
	
	if( nCnt != nNumSize )
	{
		Error( "%s : frame size error", m_szName );
	}	
}

//
// 뼈대의 애니메이션
// 
void	CMotion :: AnimateBone( D3DXMATRIX *pmUpdateBone, CMotion *pMotionOld, float fFrameCurrent, int nNextFrame, int nFrameOld, BOOL bMotionTrans, float fBlendWeight )
{
	int		i;
	BONE	*pBone = m_pBoneInfo;		// 이 모델의 뼈대 시작 포인터.
	TM_ANIMATION	*pFrame = NULL;					// 현재 프레임
	TM_ANIMATION	*pNext = NULL;					// 다음 프레임
	TM_ANIMATION	*pFrameOld = NULL;				// 이전모션의 프레임
//	D3DXMATRIX	*pMatBone = m_mUpdateBone;
	D3DXMATRIX	*pMatBones = pmUpdateBone;			// 업데이트 매트릭스 리스트 시작 포인터
	int			nCurrFrame;
	float		fSlp;
	D3DXQUATERNION		qSlerp;
	D3DXVECTOR3			vSlerp;
	BONE_FRAME	*pBoneFrame = NULL;
	BONE_FRAME	*pBoneFrameOld = NULL;		// 본의 계층구조
	D3DXMATRIX	m1, m2;

	if( pMotionOld == NULL )	// pMotionOld-> 할때 널참조 에러만 안나게함.
		pMotionOld = this;

	// 애니메이션 동작이 있는 경우
	i = m_nMaxBone;
	// 보간을 위한 Slerp 계산.
	nCurrFrame = (int)fFrameCurrent;				// 소숫점 떼내고 정수부만..
	fSlp = fFrameCurrent - (float)nCurrFrame;	// 소숫점 부분만 가져옴
	/////////////////////////////

//	int	nNextFrame = GetNextFrame();
	pBoneFrame	  = m_pBoneFrame;
	pBoneFrameOld = pMotionOld->m_pBoneFrame;		// 이전동작의 뼈대 계층
	// 뼈대 갯수만큼 루프 돈다
	while( i-- )
	{
		if( pBoneFrameOld->m_pFrame )
			pFrameOld = &pBoneFrameOld->m_pFrame[nFrameOld];		// 일단은 0으로 한다.  이전모션의 nCurrFrame도 기억하고 있어야 한다.
		else
			pFrameOld = NULL;		// 일단은 0으로 한다.  이전모션의 nCurrFrame도 기억하고 있어야 한다.
		if( pBoneFrame->m_pFrame )		// 이 뼈대에 프레임이 있을때
		{
			pFrame = &pBoneFrame->m_pFrame[ nCurrFrame ];		// 현재 프레임 포인터 받음
			pNext  = &pBoneFrame->m_pFrame[ nNextFrame ];		// 다음 프레임 포인터 받음

			D3DXQuaternionSlerp( &qSlerp, &pFrame->m_qRot, &pNext->m_qRot, fSlp );	// 보간된 쿼터니언 구함
			D3DXVec3Lerp( &vSlerp, &pFrame->m_vPos, &pNext->m_vPos, fSlp );		// 보간된 Pos벡터 구함

			if( bMotionTrans )	// 이전 모션과 블렌딩 해야 하는가
			{
				float	fBlendSlp = fBlendWeight;
				if( pFrameOld )
				{
					D3DXQuaternionSlerp( &qSlerp, &pFrameOld->m_qRot, &qSlerp, fBlendSlp );	// 이전모션과 현재 프레임을 블렌딩
					D3DXVec3Lerp( &vSlerp, &pFrameOld->m_vPos, &vSlerp, fBlendSlp );		// 보간된 Pos벡터 구함
				} else
				{
					D3DXQUATERNION	qLocal;
					D3DXVECTOR3		vLocal;
					D3DXMATRIX		*pmLocalTM = &pBoneFrameOld->m_mLocalTM;
					D3DXQuaternionRotationMatrix( &qLocal, pmLocalTM );
					vLocal.x = pmLocalTM->_41;
					vLocal.y = pmLocalTM->_42;
					vLocal.z = pmLocalTM->_43;
					D3DXQuaternionSlerp( &qSlerp, &qLocal, &qSlerp, fBlendSlp );	// 이전모션과 현재 프레임을 블렌딩
					D3DXVec3Lerp( &vSlerp, &vLocal, &vSlerp, fBlendSlp );		// 보간된 Pos벡터 구함
				}
			}

			// matAniTM계산
			// 이동행렬 만듦
			D3DXMatrixTranslation( &m1,	 vSlerp.x,  vSlerp.y,  vSlerp.z );
		
			// 쿼터니온 보간된 회전키값을 회전행렬로 만듦
			D3DXMatrixRotationQuaternion( &m2, &qSlerp );
			m2 *= m1;		// 이동행렬 X 회전행렬 = 애니행렬

			if( pBone->m_pParent )
				m2 *= pMatBones[ pBone->m_nParentIdx ];		// 부모가 없다면 루트인데 항등행렬과 곱할 필요 없어서 생략했다.
//				else
//					m2 = m2 * *pmWorld;  // 이부분이 생략됐다.  mCenter는 항등행렬.
			
			// WorldTM = LocalTM * ParentTM;
			// 최종 변환된 월드매트릭스를 멤버에 넣어서 Render()에서 SetTransfrom함.
		} else
		// 이 뼈대에 프레임이 없을때.
		{
			if( bMotionTrans )	// 이전 모션과 블렌딩 해야 하는가
			{
				float	fBlendSlp = fBlendWeight;
				D3DXQUATERNION	qLocal;
				D3DXVECTOR3		vLocal;
				D3DXMATRIX		*pmLocalTM = &pBoneFrame->m_mLocalTM;
				D3DXQuaternionRotationMatrix( &qLocal, pmLocalTM );
				vLocal.x = pmLocalTM->_41;
				vLocal.y = pmLocalTM->_42;
				vLocal.z = pmLocalTM->_43;	// 현재프레임의 LocalTM을 쿼터니온과 벡터로 빼냄.

				if( pFrameOld )
				{
					D3DXQuaternionSlerp( &qSlerp, &pFrameOld->m_qRot, &qLocal, fBlendSlp );	// 이전모션과 현재 프레임을 블렌딩
					D3DXVec3Lerp( &vSlerp, &pFrameOld->m_vPos, &vLocal, fBlendSlp );		// 보간된 Pos벡터 구함
				} else
				{
					D3DXQUATERNION	qLocal2;
					D3DXVECTOR3		vLocal2;
					D3DXMATRIX		*pmLocalTM2 = &pBoneFrameOld->m_mLocalTM;
					D3DXQuaternionRotationMatrix( &qLocal2, pmLocalTM2 );
					vLocal2.x = pmLocalTM2->_41;
					vLocal2.y = pmLocalTM2->_42;
					vLocal2.z = pmLocalTM2->_43;
					D3DXQuaternionSlerp( &qSlerp, &qLocal2, &qLocal, fBlendSlp );	// 이전모션과 현재 프레임을 블렌딩
					D3DXVec3Lerp( &vSlerp, &vLocal2, &vLocal, fBlendSlp );		// 보간된 Pos벡터 구함
				}
				D3DXMatrixTranslation( &m1,	 vSlerp.x,  vSlerp.y,  vSlerp.z );
			
				// 쿼터니온 보간된 회전키값을 회전행렬로 만듦
				D3DXMatrixRotationQuaternion( &m2, &qSlerp );
				m2 *= m1;			// 이동행렬 X 회전행렬 = 애니행렬
			} else
				m2 = pBoneFrame->m_mLocalTM;

			if( pBone->m_pParent )
				m2 *= pMatBones[ pBone->m_nParentIdx ];
//				else
//					m2 = m2 * *pmWorld;
		}

		*pmUpdateBone = m2;		// 이 뼈대의 최종 월드 변환 매트릭스

		pBoneFrame ++;
		pBoneFrameOld ++;
		pBone ++;					// 본 ++
		pmUpdateBone ++;			// 본 트랜스폼 매트릭스 ++
	}
}

