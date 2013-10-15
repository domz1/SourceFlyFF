#ifndef __PET_H__
#define	__PET_H__

#if __VER >= 9	// __PET_0410

#ifdef __CLIENT
#include "..\_AIInterface\AIInterface.h"
#endif	// __CLIENT

#include "defineitem.h"

/*#if __VER >= 19
#include "defineItemGrade.h"
#include "defineItemType.h"
#endif//__VER >= 19*/

// 그냥 클라이언트에서 따로 따로 움직여도 상관 없지 않을까?
// 이렇게 될 경우, (전송과 처리가 가벼워지는 장점이 있다.)
	// 서버는 펫 자료 객체만,
	// 클라이언트는 CPet의 데이터를 가지고 있는 CMover 객체를 생성하면 된다.

enum	PETLEVEL	{	PL_EGG,	PL_D,	PL_C,	PL_B,	PL_A,	PL_S,	PL_MAX,	};

#define	MAX_PET_AVAIL_LEVEL	9
#define	MAX_ADD_LIFE	5

typedef	struct	_FEEDENERGY
{
	DWORD	dwCostMin;
	DWORD	dwCostMax;
	WORD	wEnergyMin;
	WORD	wEnergyMax;
}	FEEDENERGY,	*PFEEDENERGY;

typedef struct	_PETPENALTY
{
	FLOAT	fExp;
	WORD	wEnergy;
	_PETPENALTY()
	{
		fExp	= 0.0f;
		wEnergy	= 0;
	}
}	PETPENALTY, *PPETPENALTY;

#define	MAX_PET_SHAPE	3
typedef	struct	_PETAVAILPARAM
{
	DWORD	dwDstParam;		// 능력치 상승 파라미터
	DWORD	m_anParam[MAX_PET_AVAIL_LEVEL];		// 능력치 상승 값
	DWORD	m_dwItemId;	// 펫 아이템
	DWORD	m_adwIndex[MAX_PET_SHAPE];	// 펫 객체 모양
	DWORD	m_dwProbability;	// 부화 확률
}	PETAVAILPARAM, *PPETAVAILPARAM;

class CPetProperty
{
public:
	CPetProperty();
	virtual	~CPetProperty();

	PPETAVAILPARAM	GetAvailParam( BYTE nKind );	// 치유 속성 추가
	BYTE	GetLevelupAvailLevel( BYTE wLevel );		// 레벨 업 시 상승되는 능력치 레벨(임의)
	WORD	GetFeedEnergy( DWORD dwCost, int nIndex = 0 );		// 가격에 따른 먹이 생성(임의)
	DWORD	GetIncrementExp( BYTE nLevel );	// 분당 습득 경험치
	WORD	GetMaxEnergy( BYTE nLevel );
	WORD	GetAddLife( void );		// 수명 회복액에 의한 생명 추가

#ifdef __JEFF_11_3
	LPDWORD		GetLevelupAvailLevelProbabilityPtr( void )	{	return &m_adwLevelupAvailLevelProbability[0][0];	}
	LPBYTE	GetLevelupAvailLevelMaxPtr( void )		{	return m_anLevelupAvailLevelMax;	}
	vector<WORD>*	GetAddLifeProbabilityPtr( void )	{	return &m_awAddLifeProbability;		}
#endif	// __JEFF_11_3

	static	CPetProperty*	GetInstance( void );
	BOOL	LoadScript( LPCTSTR szFile );
	BYTE	Hatch( void );
	PPETPENALTY		GetPenalty( BYTE nLevel );
private:
	PETAVAILPARAM	m_aPetAvailParam[PK_MAX];
	BYTE	m_anLevelupAvailLevelMax[PL_MAX];
	DWORD	m_adwLevelupAvailLevelProbability[PL_MAX][MAX_PET_AVAIL_LEVEL];	// 확률
	vector<FEEDENERGY>	m_aFeedEnergy[2];	// 먹이 만들기 시 전리 가격에 따른 먹이 량 
	DWORD	m_adwIncrementExp[PL_MAX];		// 분당 습득 경험치
	WORD	m_awMaxEnergy[PL_MAX];
	vector<WORD>	m_awAddLifeProbability;

	PETPENALTY	m_aPenalty[PL_MAX];
};

#define	MAX_PET_LIFE	99
#define	MAX_PET_EGG_EXP		(DWORD)50000
#define	MAX_PET_EXP		(DWORD)100000

#define	PF_PET	0
#define	PF_PET_LEVEL_UP	1
#define	PF_PET_LEVEL_DOWN	2
#define	PF_PET_GET_AVAIL	3

#define	PETLOGTYPE_CALL	0
#define	PETLOGTYPE_LEVELUP	1
#define	PETLOGTYPE_RELEASE	2
#define	PETLOGTYPE_DEATH		4
#define	PETLOGTYPE_FEED	5
#define	PETLOGTYPE_MIRACLE	6
#define	PETLOGTYPE_MISTAKE	7
#define	PETLOGTYPE_LIFE	8

#ifdef __PET_1024
#define	MAX_PET_NAME_FMT	33	// 16 * 2 + 1
#define	MAX_PET_NAME	17	// 한글 8자
#endif	// __PET_1024

class CPet
{
public:
//	Constructions
	CPet();
	virtual	~CPet();

//	Operations
	void	Serialize( CAr & ar );

//	Attributions
	void	SetAvailLevel( BYTE nLevel, BYTE nAvailLevel );

	DWORD	GetIndex( void );
	BYTE	GetAvailLevel( BYTE nLevel );
	LPBYTE	GetAvailLevel( void )	{	return m_anAvailLevel;	}

	void	GetAvailDestParam( DWORD & dwDestParam, int & nParam );
	BYTE	GetKind( void )	{	return m_nKind;	}
	void	SetKind( BYTE nKind )	{	m_nKind	= nKind;	}
	BYTE	GetLevel( void )	{	return m_nLevel;	}
	void	IncLevel( void )		{	m_nLevel++;	}
	void	SetLevel( BYTE nLevel )	{	m_nLevel	= nLevel;	}
	DWORD	GetExp( void )	{	return m_dwExp;		}
	void	SetExp( DWORD dwExp );
	WORD	GetLife( void );
	void	SetLife( WORD wLife )	{	m_wLife	= wLife;	}
	WORD	GetEnergy( void )	{	return m_wEnergy;	}
	void	SetEnergy( WORD wEnergy );
	WORD	GetMaxEnergy( void )	{	return CPetProperty::GetInstance()->GetMaxEnergy( m_nLevel );	}
	int		GetEnergyPercent( void )	{	return GetEnergy() * 100 / GetMaxEnergy();	}
	int		GetExpPercent( void )	{	return( GetExp() * 100 / GetMaxExp() );		}
	int		GetMaxExp( void )	{	return ( m_nLevel == PL_EGG? MAX_PET_EGG_EXP: MAX_PET_EXP );	}
#ifdef __PET_1024
	void	SetName( const char* szName );
	const char*	GetName()	{	return m_szName;	}
	BOOL	HasName()	{	return strlen( m_szName ) > 0;	}
#endif	// __PET_1024
	void	InitEgg( void );
	DWORD	GetItemId( void )
		{
			if( m_nLevel == PL_EGG )
				return II_PET_EGG;
			return( CPetProperty::GetInstance()->GetAvailParam( m_nKind )->m_dwItemId );
		}
	CPet&	operator=( CPet & pet );
private:
	BYTE	m_nKind;	// 종류 : 0~6
	BYTE	m_nLevel;	// 레벨 : e		// e = 0, d = 1, c = 2, b = 3, a = 4, s = 5
	DWORD	m_dwExp;	// 경험치 : 0
	WORD	m_wEnergy;		// 기력 : 0
	WORD	m_wLife;	// 생명 :  0 ~ 99	// 디폴트 : 1	// 생명이 0인 상태에서 사망 시 객체 제거
	BYTE	m_anAvailLevel[PL_MAX];	// 능력치
#ifdef __PET_1024
	char	m_szName[MAX_PET_NAME];
#endif	// __PET_1024
};

#ifdef __CLIENT

class CAIEgg: public CAIInterface
{
private:
	OBJID m_idOwner;	// 주인님
	int		m_nState;	// 현재 상태.
	void MoveToDst(	OBJID idTarget );
	void MoveToDst(	D3DXVECTOR3 vDst );
	BOOL  MoveProcessIdle( const AIMSG & msg );
	BOOL  MoveProcessRage( const AIMSG & msg );
	void Init( void );
	void Destroy( void );
public:
	CAIEgg( CObj* pObj, OBJID idOwner );
	virtual ~CAIEgg();

	virtual void InitAI();

	int		GetState( void )	{	return m_nState;	}
	OBJID	GetOwnerId( void )	{	return m_idOwner;	}

	BOOL	StateInit( const AIMSG & msg );
	BOOL	StateIdle( const AIMSG & msg );
	BOOL	StateRage( const AIMSG & msg );

	DECLARE_AISTATE()
};
#endif	// __CLIENT

#endif	// __PET_0410

#if __VER >= 12 // __PET_0519
// 변환 시 필요한 개별 아이템 요소
typedef	struct	_TransformStuffComponent
{
	int		nItem;
	short	nNum;
	_TransformStuffComponent()
	{
		nItem	= 0;
		nNum	= 0;
	}
	_TransformStuffComponent( int nItem, short nNum )
	{
		this->nItem	= nItem;
		this->nNum	= nNum;
	}
}	TransformStuffComponent, *PTransformStuffComponent;
typedef	vector<TransformStuffComponent>	VTSC;

// 변환 시 필요한 아이템 집합
class CTransformStuff
{
public:
	CTransformStuff();
	CTransformStuff( int nTransform );
	virtual	~CTransformStuff();
	void	AddComponent( int nItem, short nNum );		// 필요 아이템 요소 추가
	virtual	void	Serialize( CAr & ar );
	int		GetTransform( void )		{	return m_nTransform;	}	// 변환 종류를 반환
	size_t	GetSize( void )		{	return m_vComponents.size();		}
	TransformStuffComponent*	GetComponent( int i )	{	return &m_vComponents[i];	}
private:
	int	m_nTransform;	// 변환 종류
	VTSC	m_vComponents;	// 필요 아이템 집합
};

#ifdef __WORLDSERVER
class ITransformer
{
protected:
	ITransformer()	{}
public:
	virtual	~ITransformer()	= 0;
	static	ITransformer*	Transformer( int nTransform );
	virtual	BOOL	IsValidStuff( CUser* pUser, CTransformStuff & stuff );
	// 사용자의 아이템을 변환한다
	void	Transform( CUser* pUser, CTransformStuff& stuff );
	// 사용자로부터 변환 재료를 제거한다
	void	RemoveItem( CUser* pUser, CTransformStuff& stuff );
	// 사용자에게 변환 결과 아이템을 만들어준다
	void	CreateItem( CUser* pUser, CTransformStuff& stuff );
};

// 알변환 클래스
class CTransformerEgg
	: public ITransformer
{
public:
	CTransformerEgg();
	virtual	~CTransformerEgg();
	virtual	BOOL	IsValidStuff( CUser* pUser, CTransformStuff & stuff );
	static	CTransformerEgg*	Instance( void );
};

// 변환 결과 아이템 요소
// 변형된 원형 패턴 사용
typedef	struct _TransformItemElement
{
	CItemElem*	pItem;	// 변환 결과 아이템
	int		nProb;	// 확률
	_TransformItemElement( CItemElem* pItem, int nProb )
	{
		this->pItem	= pItem;
		this->nProb	= nProb;
	}
}	TransformItemElement;

typedef	vector<TransformItemElement>	VTIE;

// 하나의 구체적인 변환을 의미하는 요소이다
class CTransformItemComponent
{
private:
	enum	{	eMaxProb	= 1000000	};
public:
	CTransformItemComponent( int nTransform );
	virtual	~CTransformItemComponent();
	void	Clear( void );
	void	AddElement( TransformItemElement element );
	void	AdjustmentProbability( TransformItemElement & element );
	CItemElem*	GetItem( void );
	void	SetStuffSize( u_int nStuffSize )	{	m_nStuffSize	= nStuffSize;	}
	u_int	GetStuffSize( void )	{	return m_nStuffSize;	}
	int		GetTransform( void )	{	return m_nTransform;	}
private:
	const	int		m_nTransform;
	u_int	m_nStuffSize;
	int		m_nTotalProb;
	VTIE	m_vTransformItemElements;
};

typedef	map<int, CTransformItemComponent*>	MPTIC;
// 모든 변환을 관리한다
class CTransformItemProperty
{
public:
	CTransformItemProperty();
	virtual	~CTransformItemProperty();
	static	CTransformItemProperty*	Instance( void );
	void	AddComponent( CTransformItemComponent* pComponent );
	u_int	GetStuffSize( int nTransform );
	CItemElem*	GetItem( int nTransform );
	BOOL	LoadScript( const char* szFile );
	CItemElem*	CreateItemGeneric( CScript& s );
	CItemElem*	CreateItemPet( CScript& s );
private:
	CTransformItemComponent* GetComponent( int nTransform );
private:
	MPTIC	m_mapComponents;
};

#endif	// __WORLDSERVER

#endif	// __PET_0519

#endif	// __PET_H__
