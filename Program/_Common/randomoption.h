#ifndef __RANDOM_OPTION_EX_H__
#define	__RANDOM_OPTION_EX_H__

#include <vector>

#ifdef __MAX_AWAKE_CHANGE
#define	MAX_RANDOM_OPTION	5
#else//__MAX_AWAKE_CHANGE
#define	MAX_RANDOM_OPTION	3
#endif//__MAX_AWAKE_CHANGE

//	mulcom	BEGIN100405	각성 보호의 두루마리
#define _AWAKE_SAFE_FLAG	0x2000000000000000

#define _AWAKE_OLD_VALUE	1
#define _AWAKE_NEW_VALUE	2
//	mulcom	END100405	각성 보호의 두루마리


typedef	struct	_AdjData
{
	short	nAdj;
	DWORD	dwProb;
	_AdjData( short n, DWORD dw )
	{
		nAdj	= n;
		dwProb	= dw;
	}
}	AdjData;

typedef struct	_RANDOM_OPTION
{
	int		nDst;	// 파라미터
	int		nProb;	//	이 파라미터가 나올 확률
	vector<AdjData>	aAdjData;
	_RANDOM_OPTION()
	{
		nDst	= 0;
		nProb	= 0;
	}
	_RANDOM_OPTION( _RANDOM_OPTION* pRandomOption )
	{
		nDst	= pRandomOption->nDst;
		nProb	= pRandomOption->nProb;
		aAdjData.assign( pRandomOption->aAdjData.begin(), pRandomOption->aAdjData.end() );
	}
}	RANDOM_OPTION;

class CRandomOptionProperty
{
public:
#if __VER >= 12 // __PET_0519
	// 시스템 펫 각성과 먹펫 각성 추가
	enum	{	eAwakening, eBlessing,	eSystemPet, eEatPet, eMaxRandomOptionKind	};		// 랜덤 옵션 종류	// 각성: 0, 축복: 1
#else	// __PET_0519
	enum	{	eAwakening, eBlessing,	eMaxRandomOptionKind	};		// 랜덤 옵션 종류	// 각성: 0, 축복: 1
#endif	// __PET_0519

#if __VER >= 12 // __J12_0

#ifdef __MAX_AWAKE_CHANGE
	enum	{	eAwakeningExtension	= 5	};
#else//__MAX_AWAKE_CHANGE
	enum	{	eAwakeningExtension	= 3	};	// 0: 손, 1: 발, 2: 머리	// 각성 확장
#endif//__MAX_AWAKE_CHANGE

#endif	// __J12_0

	CRandomOptionProperty();
	virtual	~CRandomOptionProperty();
	static	CRandomOptionProperty*	GetInstance();
	
	BOOL	LoadScript( LPCTSTR szFile );
	void	LoadScriptBlock( CScript& s, int nRandomOptionKind );

	// i번째 랜덤 옵션 파라미터와 값을 반환, 비어있으면 FALSE 반환
	BOOL	GetParam( __int64 nRandomOptItemId, int i, int* pnDst, int* pnAdj );
	// 부여된 랜덤 옵션 파라미터의 개수 반환
	int		GetRandomOptionSize( __int64 nRandomOptItemId );

	//	mulcom	BEGIN100405	각성 보호의 두루마리
	int		GetViewRandomOptionSize( __int64 n64RandomOptItemId );
	//	mulcom	END100405	각성 보호의 두루마리

	// 장착 부위에 따른 랜덤 옵션의 종류 반환
	int		GetRandomOptionKind( CItemElem* pItemElem );

	// 랜덤 옵션 부여
	//	mulcom	BEGIN100405	각성 보호의 두루마리
	//BOOL	GenRandomOption( __int64* pnRandomOptItemId, int nRandomOptionKind, int nParts );
	BOOL	GenRandomOption( __int64* pnRandomOptItemId, int nRandomOptionKind, int nParts, bool bDecreaseFlag = false );
	//	mulcom	END100405	각성 보호의 두루마리

	// 랜덤 옵션 초기화
	void	InitializeRandomOption( __int64* pnRandomOptItemId );

	// 해당 파라미터를 기존 랜덤 옵션 다음 번째에 설정	- GenRandomOption에서 호출.
	void	SetParam( __int64* pnRandomOptItemId, int nDst, int nAdj );
private:
	int		DetermineRandomOptionSize( int nRandomOptionKind );
	RANDOM_OPTION*	DetermineRandomOptionDst( int nRandomOptionKind, int nParts );

	//	mulcom	BEGIN100405	각성 보호의 두루마리
	//short DetermineRandomOptionAdj( RANDOM_OPTION* pRandomOption );
	short DetermineRandomOptionAdj( RANDOM_OPTION* pRandomOption, bool bDecreaseAdj = false );
	//	mulcom	END100405	각성 보호의 두루마리

	int		GetRandomOptionKindIndex( int nRandomOptionKind, int nParts );
	int		GetUpperProbability( int iRandomOptionKindIndex );
#if __VER >= 12 // __J12_0
	void	AwakeningExtension( void );		// 각성 축복 테이블 확장
#endif	// __J12_0
private:
	int		m_anRandomOptionProb[eMaxRandomOptionKind][MAX_RANDOM_OPTION];
#if __VER >= 12 // __J12_0
	// 기본 + 확장
	vector<RANDOM_OPTION>	m_aRandomOption[eMaxRandomOptionKind + eAwakeningExtension];
#else	// __J12_0
	vector<RANDOM_OPTION>	m_aRandomOption[eMaxRandomOptionKind];
#endif	// __J12_0


	//	mulcom	BEGIN100405	각성 보호의 두루마리
public:
	bool	IsCheckedSafeFlag( __int64 n64RandomeOption );
	void	SetSafeFlag( __int64* pn64RandomOption );
	void	ResetSafeFlag( __int64* pn64RandomOption );
	//	mulcom	END100405	각성 보호의 두루마리

#ifdef __FASHION_COMBINE
public:
	int		MergeRandomOption( CItemElem* pItemElemMain, CItemElem* pItemElemDestroy, int nMaxCombinedSize = 3 );
#endif // __FASHION_COMBINE
};


#define g_xRandomOptionProperty		CRandomOptionProperty::GetInstance()

#endif	//__RANDOM_OPTION_EX_H__