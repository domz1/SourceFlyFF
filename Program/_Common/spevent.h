#ifndef __SPEVENT_H__
#define	__SPEVENT_H__

class CSPEvent
{
private:

public:
	CSPEvent()	{}
	virtual	~CSPEvent()	{}
};

class CXMasEvent : public CSPEvent
{
public:
	CXMasEvent();
	virtual	~CXMasEvent();
	BOOL	LoadScript( LPCTSTR lpFilename );
	BOOL	IsTimeout( int nHour );
	void	Skip( LONG lSkip );

	static	CXMasEvent*	GetInstance( void );

private:
	DWORD	m_adwInterval[24];
	DWORD	m_dwTimeout;
	LONG		m_lSkip;
};

#define	MAX_EVENT_TITLE		100
typedef	struct	_EVENT_GENERIC
{
	int	nId;	// 이벤트 번호, 500이 시작, 최대는 531
	int	nFlag;		// 이벤트 플레그, 1값을 (이벤트 번호 - 500) 왼쪽 시프트
	time_t	tStart;		// 시작 시간
	time_t	tEnd;		// 끝 시간
	FLOAT	fExpFactor;
#ifdef __ITEMDROPRATE
	FLOAT	fItemDropRate;
#endif // __ITEMDROPRATE
	char	pszTitle[MAX_EVENT_TITLE];
	_EVENT_GENERIC()
	{
		nId	= 0;
		nFlag	= 0;
		tStart	= 0;
		tEnd	= 0;
		*pszTitle	= '\0';
		fExpFactor	= 1.0f;
#ifdef __ITEMDROPRATE
		fItemDropRate = 1.0f;
#endif // __ITEMDROPRATE
	}
}	EVENT_GENERIC,	*PEVENT_GENERIC;

class CEventItem
{
private:
	int		m_nMax;		// 일일 최대 드롭
	DWORD	m_adwInterval[24];
	DWORD	m_dwTimeout;
	LONG	m_lSkip;
public:
	DWORD	m_dwItemId;		// 아이템 번호
	int		m_nNum;		// 일회 최대 드롭
public:
//
	CEventItem();
	CEventItem( DWORD dwItemId, int nMax, int nNum );
	virtual	~CEventItem()	{}
//	op
	BOOL	IsTimeout( int nHour );		// 드롭 주기 인가?
	void	Skip( LONG lSkip );		// 드롭 주기에 드롭 무시 회수 설정
	void	Serialize( CAr & ar );
};
// 코어 서버에는 프로젝트를 로딩하지 않으므로
// 트랜스 서버에서 객체를 관리한다.

#ifdef __EVENT_0117
typedef	struct	_REGION_GENERIC		// 스폰 이벤트 (아이템, 몬스터)에 쓰기 위한 영역 정보
{
	int		nLevel;	// 영역 레벨
	DWORD	dwWorldId;		// 월드 번호
	CRespawnInfo* pi;	// 참조 영역
}	REGION_GENERIC,	*PREGION_GENERIC;

class CSpawn
{
private:
	int		m_nMax;		// 일일 최대 스폰
	float	m_fRatio;	// 최저 레벨 영역에 대비 최고레벨의 스폰 확률
	DWORD	m_dwInterval;	// 스폰 간격 ms
	DWORD	m_dwTimeout;	// 스폰 시간 타임 아웃
public:
	DWORD	m_dwType;	// 스폰 객체 타입	
	DWORD	m_dwIndex;		// 스폰 객체 인덱스
public:
// Constructions
	CSpawn();
	CSpawn( DWORD dwType, DWORD dwIndex, int nMax, float fRatio, DWORD dwInterval );
	virtual	~CSpawn()	{}
//	Operations
	BOOL	IsTimeout( void );	// 스폰 주기 인가?
private:
	DWORD	GetMaxSpawnProbability( int nMax )
	{
		ASSERT( m_fRatio > 0 );
		return	( (DWORD)( ( nMax + ( m_fRatio - 1.0F ) * ( nMax - 1 ) / 2.0F ) * 100.0F ) );
	}
public:
	int	GetSpawnIndex( int nMax )
	{
		ASSERT( m_fRatio > 0 );
		DWORD dwProb	= xRand() % GetMaxSpawnProbability( nMax );
		float c	= ( m_fRatio - 1.0F ) / (float)nMax;
		float D	= pow( c + 2, 2 ) - 4 * c * ( 2 - dwProb / 50.0F );
		int nIndex	= 0;
		if( D > 0 )
			nIndex	= (int)( ( - ( 2 + c ) + sqrt( D ) ) / ( 2 * c ) + 1 );
		else
			nIndex	= (int)( ( - ( 2 + c ) - sqrt( D ) ) / ( 2 * c ) + 1 );
		if( nIndex < 0 || nIndex >= nMax )
		{
			// error
			nIndex	= 0;
		}
		return nIndex;
	}
};
#endif	// __EVENT_0117

#ifdef __EVENT_0117
class CRespawnInfo;
#endif	// __EVENT_0117
class CEventGeneric
{
private:
	DWORD	m_dwFlag;
	list<PEVENT_GENERIC>	m_lspEvent;
	map<int, list<CEventItem*>*>	m_mapEventItemList;

#ifdef __EVENT_0117
	vector<REGION_GENERIC>	m_aRegionGeneric;	// 모든 월드의 리스폰 영역을 벡터에 저장한다.	// 월드 번호, 지역, 레벨
	map<int, CSpawn*>	m_mapSpawn;		// 이벤트가 활성화 되었을 때, 해당 이벤트가 스폰을 포함한다면 가져오기 위한 맵이다.
#endif	// __EVENT_0117

public:
	CEventGeneric();
	virtual	~CEventGeneric();
//	op
	void Clear( BOOL bDestructor = TRUE );
	// 스크립트 로드
	BOOL	LoadScript( LPCSTR lpFilename );
	// 이벤트 활성화 시 드롭되는 아이템 추가
	BOOL	AddItem( int nEvent, DWORD dwItemId, int nMax, int nNum );
#ifdef __WORLDSERVER
	CEventItem*	GetItem( int* pnNum );
	FLOAT	GetExpFactor( void );
#ifdef __ITEMDROPRATE
	FLOAT	GetItemDropRateFactor( void );
#endif // __ITEMDROPRATE
#ifdef __EVENT_1101
	LONG	GetEventElapsed( void );
	void	CallTheRoll( void );
#endif	// __EVENT_1101

#ifdef __EVENT_0117
	void	AddSpawn( int nEvent, DWORD dwType, DWORD dwIndex, int nMax, float fRatio, DWORD dwInterval );		// 스폰 이벤트 정보 등록
	void	AddRegionGeneric( int nLevel, DWORD dwWorldId, CRespawnInfo* pi );	// 스폰 영역 등록
	void	SortRegionGeneric( void );	// 스폰 영역 정렬
	void	Spawn( void );
#endif	// __EVENT_0117

#endif	// __WORLDSERVER
	static	CEventGeneric*	GetInstance( void );
	void	Serialize( CAr & ar );
	DWORD	GetFlag( void )		{	return m_dwFlag;	}
	BOOL	Run( void );
	list<PEVENT_GENERIC>*	GetEventList( void )	{	return &m_lspEvent;		}
	PEVENT_GENERIC	GetEvent( int nEvent );
};

#endif	// __SPEVENT_H__