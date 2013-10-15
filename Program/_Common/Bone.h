#ifndef	__BONE_H__
#define	__BONE_H__

#include <d3dx9.h>
#include "file.h"

#define		MAX_VS_BONE		28		// VS에서 최대사용가능한 뼈대레지스터의 개수.

struct BONE;
struct BONE
{
	BONE		*m_pParent;			// 상위 부모 노드

//	int			m_nID;				// 계층순서 대로 부여된 번호 - 어차피 필요없을거 같다.
	int			m_nParentIdx;		// 부모 인덱스
	char		m_szName[32];		// * 스트링형태의 노드 이름
	D3DXMATRIX		m_mTM;		// ase원본 TM
	D3DXMATRIX		m_mInverseTM;	// * 미리 변환된 인버스TM 
	D3DXMATRIX		m_mLocalTM;	// * 미리변환된 로컬 TM
};

#define MAX_MDL_EVENT		8	// #event xx

//
// 본셋트의 구조
class CBones
{
public:
	int		m_nID;				// 본의 고유 ID(파일내에 있다)
	char	m_szName[64];		// 본의 파일명 - 나중에 이건 빠진다.
	int		m_nMaxBone;			// 뼈대 갯수
	BONE	*m_pBones;			// 뼈대 리스트
	int		m_bSendVS;			// 본개수가 MAX_VS_BONE보다 적은지...

	D3DXMATRIX	m_mLocalRH;		// 오른손 무기의 RHand로부터의 상대TM
	D3DXMATRIX	m_mLocalLH;		// 오른손 무기의 LHand로부터의 상대TM
	D3DXMATRIX	m_mLocalShield;	// 방패의 LForeArm으로부터의 상대TM
	D3DXMATRIX	m_mLocalKnuckle;	// 너클.
	int		m_nRHandIdx;		// 오른손 인덱스
	int		m_nLHandIdx;		// 왼손 인덱스
	int		m_nRArmIdx;			// 오른손 팔뚝
	int		m_nLArmIdx;			// 왼손 팔뚝 인덱스.
	D3DXVECTOR3	m_vEvent[MAX_MDL_EVENT];	
//	BONE	*m_pEventParent[MAX_MDL_EVENT];	// 이벤트 좌표들 부모
	int		m_nEventParentIdx[MAX_MDL_EVENT];	// 이벤트좌표들 부모인덱스.
	
private:
	void Init( void );
	void Destroy( void );

public:
	CBones();
	~CBones();

//	BONE	*FindBone( int nID );								// nID값을 가지는 본을 찾음
	BONE	*GetBone( int nIdx ) { return &m_pBones[ nIdx ]; }

	int		LoadBone( LPCTSTR szFileName );		// 실제로 본 파일을 읽음
};


#if __VER < 12 // __LEAK_0827
#define		MAX_BONES			128
#endif	// __LEAK_0827

// read only bone database
class CBonesMng
{
public:
#if __VER >= 12 // __LEAK_0827
	map<string, CBones*>	m_mapBones;
#else	// __LEAK_0827
	int			m_nSize;							// 관리자가 로딩해서 가지고 있는 본의 사이즈가 얼마나 되는가.
	int			m_nMax;								// 갯수
	CBones *m_pBonesAry[ MAX_BONES ];
#endif	// __LEAK_0827

//#ifdef __WORLDSERVER
//	CRIT_SEC	m_AccessLock;
//#endif	// __WORLDSERVER

	CBonesMng();
	~CBonesMng();

	void	Init( void );
	void	Destroy( void );

	CBones *LoadBone( LPCTSTR szFileName );									// Bone파일을 읽어서 메모리에 적재시킨다.

};

extern CBonesMng		g_BonesMng;


//////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////
////////////////
////////////////
////////////////
////////////////
////////////////
////////////////
////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
#define		MA_HIT			(0x00000001)		// 타점
#define		MA_SOUND		(0x00000002)		// 소리
#define		MA_QUAKE		(0x00000004)		// 지진.
struct TM_ANIMATION
{
//	D3DXMATRIX			m_mAniTM;
	D3DXQUATERNION		m_qRot;
	D3DXVECTOR3			m_vPos;
};

// 모션은 본과 밀접한 관계가 있다.  본과 크기가 같고, 같은 본에서 나온 모션이 아니면 오동작 할 수가있다.
// 캐릭터 동작하나에 대한 클래스
struct	BONE_FRAME
{
	TM_ANIMATION	*m_pFrame;
	D3DXMATRIX		m_mLocalTM;
};

// 각 프레임의 속성들
struct	MOTION_ATTR
{
	DWORD	m_dwAttr;
//	char	m_szFileName[32];
	int		m_nSndID;		// 사운드 리소스 아이디
	float	m_fFrame;		// 소숫점 단위의 정확한 프레임 번호 
};

class CMotion
{
private:
	TM_ANIMATION	*m_pMotion;			// 뼈대순서대로 각각의 애니메이션이 들어있다.  애니가 없는 뼈대는 LocalTM만 있다.
	MOTION_ATTR		*m_pAttr;			// 프레임크기만큼 할당.  각 프레임속성.
	int		m_nMaxFrame;				// 모션 프레임수
public:
	int		m_nID;						// 모션의 고유 ID(파일내에 있다)
	char	m_szName[64];				// 모션 파일명 - 나중에 이건 빠진다.
	float	m_fPerSlerp;				//
	int		m_nMaxBone;					// 이것은 어디까지나 확인용 변수일뿐이다.  실제 본의 갯수가 필요하다면 CBone에서 읽어야 한다.
	D3DXVECTOR3	m_vPivot;				// 0,0,0(발)기준에서 골반(루트)의 좌표.  0번키의 좌표와 같다.
	BONE_FRAME	*m_pBoneFrame;		// 뼈대갯수 만큼 할당. 로딩이 끝난후 사용하기 좋도록 포인터를 재배치한다.  프레임이 없는곳은 NULL
	BONE		*m_pBoneInfo;		// 뼈대구성정보
	int		m_nMaxEvent;
	D3DXVECTOR3	m_vEvent[4];
	D3DXVECTOR3		*m_pPath;			// path
	
	CMotion();
	~CMotion();

	void Init( void );
	void Destroy( void );

	MOTION_ATTR *GetMotionAttr( int nNumFrm ) { return m_pAttr + nNumFrm; }	
	DWORD	GetAttr( int nNumFrm ) 
	{ 
		return m_pAttr[ nNumFrm ].m_dwAttr; 
	}
//#if 0
	DWORD	IsAttrHit( float fOldFrm, float fNumFrm ) 
	{ 
		MOTION_ATTR		*pAttr = &m_pAttr[ (int)fNumFrm ];
		if( pAttr->m_dwAttr & MA_HIT )
			if( fOldFrm < pAttr->m_fFrame && pAttr->m_fFrame <= fNumFrm )	// 이전 프레임이랑 현재 프레임 사이에 타점이 들어있었는가.
				return pAttr->m_dwAttr;
			return 0;
	}
	MOTION_ATTR *IsAttrSound( float fOldFrm, float fNumFrm )
	{ 
		MOTION_ATTR		*pAttr = &m_pAttr[ (int)fNumFrm ];
		if( pAttr->m_dwAttr & MA_SOUND )
			if( fOldFrm < pAttr->m_fFrame && pAttr->m_fFrame <= fNumFrm )	// 이전 프레임이랑 현재 프레임 사이에 타점이 들어있었는가.
				return pAttr;
			return NULL;
	}
	DWORD	IsAttrQuake( float fOldFrm, float fNumFrm ) 
	{ 
		MOTION_ATTR		*pAttr = &m_pAttr[ (int)fNumFrm ];
		if( pAttr->m_dwAttr & MA_QUAKE )
			if( fOldFrm < pAttr->m_fFrame && pAttr->m_fFrame <= fNumFrm )	// 이전 프레임이랑 현재 프레임 사이에 타점이 들어있었는가.
				return pAttr->m_dwAttr;
			return 0;
	}
	void	SetAttr( float fNumFrm, DWORD dwAttr ) 
	{
		m_pAttr[ (int)fNumFrm ].m_dwAttr |= dwAttr;
		m_pAttr[ (int)fNumFrm ].m_fFrame = fNumFrm;
	}
	void	ResetAttr( int nNumFrm, DWORD dwAttr ) { m_pAttr[ nNumFrm ].m_dwAttr &= (~dwAttr); }
	TM_ANIMATION*	GetFrame( int nNumFrm ) { return &m_pMotion[nNumFrm]; }
	int		GetMaxFrame( void ) { return m_nMaxFrame; }

	int		LoadMotion( LPCTSTR strFileName );			// 파일에서 모션파일을 읽음
	void	ReadTM( CResFile *file, int nNumBone, int nNumFrame );

	void	AnimateBone( D3DXMATRIX *pmUpdateBone, CMotion *pMotionOld, float fFrameCurrent, int nNextFrame, int nFrameOld, BOOL bMotionTrans, float fBlendWeight );
	
};

#if __VER < 12 // __LEAK_0827
#define		MAX_MOTION					2048
#define		MAX_MOTION_CACHE		16
#endif	// __LEAK_0827

// read only motion database
class CMotionMng
{
public:
#if __VER >= 12 // __LEAK_0827
	map<string, CMotion*>	m_mapMotions;
#else	// __LEAK_0827
	int		m_nSize;
	int		m_nMax;
	int		m_nCachePos;				// 캐시 큐 인덱스
	CMotion		*m_pCache[ MAX_MOTION_CACHE ];			// 최근에 읽은 동작의 포인터를 가지고 있음
	CMotion		*m_pMotionAry[ MAX_MOTION ];
#endif	// __LEAK_0827
	
	CMotionMng();
	~CMotionMng();

	void	Init( void );
	void	Destroy( void );

	CMotion	*LoadMotion( LPCTSTR strFileName );			// 동작을 메모리에 적재한다.  중복해서 올리지 않는다.
};

extern CMotionMng		g_MotionMng;




#endif
