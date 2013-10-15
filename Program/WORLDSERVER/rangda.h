#ifndef __RANGDA_H__
#define	__RANGDA_H__

// 랜덤 이벤트 몬스터가 나타나는 지점을 나타내는 구조체
typedef	struct	_RANGDA_POS
{
	DWORD	dwWorldId;	// 나타날 수 있는 곳의 월드 식별자
	D3DXVECTOR3 vPos;	// 나타날 수 있는 곳의 좌표
}	RANGDA_POS,	*PRANGDA_POS;

typedef	vector<RANGDA_POS>	VRP;
// 단일 랜덤 이벤트 몬스터를 제어하는 클래스
class CRangda
{
public:
	CRangda( DWORD dwMonster );
	CRangda( DWORD dwMonster, int nInterval, int nReplace, BOOL bActiveAttack );
	virtual	~CRangda();
	void	AddPos( const RANGDA_POS & vPos );	// 좌표 추가
	void	OnTimer();	// 틱 처리
	void	SetInterval( int nInterval )	{	m_nInterval	= nInterval;	}	// 재생 간격 설정
	void	SetReplace( int nReplace )	{	m_nReplace	= nReplace;		}	// 이동 간격 설정
	void	SetActiveAttack( BOOL bActiveAttack )	{	m_bActiveAttack		= bActiveAttack;	}	// 선공 여부 설정

private:
	RANGDA_POS	GetRandomPos( void );	// 임의 좌표 반환
	void	SetObj( OBJID objid );		// 실 객체 설정
	BOOL	HavetoGenerate( void );		// 재생 조건을 만족하는가?
	BOOL	HavetoReplace( void );		// 이동 조건을 만족하는가?
	void	CreateMonster( void );	// 실 객체 생성
	BOOL	IsDamaged( CMover* pMonster );	// 상처 입었나?
	void	ProcessReplace( CMover* pMonster );		// 이동 처리
	void	ProcessGenerate( void );	// 재생 처리
	CMover*		GetMonster( void );		// 실 객체 반환
private:
	const DWORD	m_dwMonster;	// 생성할 몬스터 종류 식별자
	int		m_nInterval;	// 재생 간격
	int		m_nReplace;		// 이동 간격
	BOOL	m_bActiveAttack;	// 선공 여부
	VRP	m_vvPos;	// 나타날 수 있는 좌표 집합
	int		m_nGenerateCountdown;	// 재생 대기 시간 틱당 1 감소
	int		m_nReplaceCountdown;	// 이동 대기 시간, 틱당 1 감소
	BOOL	m_bReplaceable;		// 이동 가능 플래그
	int		m_nOldPos;		// 이전 좌표
	OBJID	m_objid;	// 실 객체 식별자
};

typedef	vector<CRangda*>	VR;
// 모든 랜덤 이벤트 몬스터에 대한 제어 클래스
class CRangdaController
{
public:
	virtual	~CRangdaController();
	static	CRangdaController*	Instance( void );
	void	AddRangda( CRangda* pRangda );	// 랜덤 이벤트 몬스터 추가
	void	OnTimer( void );	// 틱
	BOOL	LoadScript( const char* szFile );
private:
	CRangdaController();
private:
	VR	m_vRangda;
};
#endif	// __RANGDA_H__