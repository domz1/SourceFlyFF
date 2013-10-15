#ifndef __TLORD_H__
#define	__TLORD_H__

#if __VER >= 12 // __LORD

#include "lord.h"
#include "dbcontroller.h"
#include "post.h"

////////////////////////////////////////////////////////////////////////////////
class CLController;
// 트랜스 서버용 군주 클래스
class CTLord
	: public CLord
{
public:
	// 처리 요청
	enum	{	eBeginCandidacy, eBeginVote, eEndVote, eAddDeposit, eSetPledge, eIncVote, eInit, eLEventCreate, eLEventInitialize, eLordSkillUse	};
public:
	CTLord();
	virtual	~CTLord();
	static	CTLord*	Instance( void );
	// 협력 객체 생성
	virtual	void	CreateColleagues( void );
	// 협력 객체 제거
	virtual void	DestroyColleagues( void );
	// 모든 군주 시스템 협력 객체의 복원
	BOOL	RestoreAll( void );
	// 군주 변수 복원
	BOOL	Restore( CQuery* pQuery );
	// 군주 연산 요청에 대한 핸들러
	void	Handler( LPDB_OVERLAPPED_PLUS pov, DWORD dwCompletionKey );
	// 틱
	void	OnTimer( void );
	// 처리 요청
	BOOL	PostRequest( int nQuery, BYTE* lpBuf = NULL, int nBufSize = 0, DWORD dwCompletionKey = 0 );
	virtual	CLController*	GetController( void )	{	return m_pController;	}
private:
	CLController*	m_pController;
};

////////////////////////////////////////////////////////////////////////////////
typedef	struct	_PostItemStruct
{
	int	nItem;
	DWORD dwKeepTime;
	_PostItemStruct() : nItem( 0 ), dwKeepTime( 0 )
	{
	}
}	PostItemStruct;

class CQuery;
// 트랜스 서버용 선거 클래스
class CTElection :
	public IElection
{
public:
	CTElection( CLord* pLord );
	virtual	~CTElection();
	// 초기화 및 스크립트 로드
	BOOL	Initialize( const char* szFile );
	// 해당 사용자의 순위에 따른 입찰금 반환률
	float	GetRetRate( u_long idPlayer );
	// 프로세스 가부 설정
	void	SetRun( BOOL bRun )		{	m_bRun	= bRun;		}
	// 현재 군주 프로세스가 실행중인가?
	BOOL	IsRunable( void )	{	return m_bRun;	}
	// 틱
	void	OnTimer( void );
	// 복원
	BOOL	Restore( CQuery* pQuery );

public:
	// 시간
	static	CString	ttos( time_t t );
	static	time_t	stot( const char* sz );

protected:
	// 템플릿 메소드 함수들
	// 입후보 시작 상태의 저장 및 전송
	virtual	BOOL	DoTestBeginCandidacy( void );
	// 입찰금의 우편을 통한 반환
	virtual	void	DoReturnDeposit( void );
	// 투표 시작 상태의 저장 및 전송
	virtual	BOOL	DoTestBeginVote( int & nRequirement );
	// 투표 종료 상태의 저장 및 전송(군주 설정)
	virtual	BOOL	DoTestEndVote( u_long idPlayer );
	// 군주 입찰 저장 및 결과 전송
	virtual	BOOL	DoTestAddDeposit( u_long idPlayer, __int64 iDeposit, time_t tCreate );
	virtual	void	DoAddDepositComplete( u_long idPlayer, __int64 iDeposit, time_t tCreate );
	// 공약 설정 저장 및 결과 전송
	virtual	BOOL	DoTestSetPledge( u_long idPlayer, const char* szPledge );
	// 투표 저장 및 결과 전송
	virtual	BOOL	DoTestIncVote( u_long idPlayer, u_long idElector );

private:
	// 스크립트 로드
	BOOL	ReadIni( const char* szFile );
	// 아이템 우편 지급	- 범용 함수로 변경 필요
	void	PostItem( u_long idPlayer, PostItemStruct & pi );
	// 후보자들에게 입찰금 반환
	void	PostDepositToAllCandidate( void );
	// 선출된 군주에게 군주 아이템 우편 지급
	void	PostLordItems( u_long idLord );

private:
	// 프로세스 실행 여부
	BOOL	m_bRun;
};

// 트랜스 서버용 군주 스킬
class CTLordSkill
	: public CLordSkill
{
public:
	CTLordSkill( CLord* pLord );
	virtual	~CTLordSkill();
	virtual	CLordSkillComponentExecutable*	CreateSkillComponent( int nType );	// template method
	// 복원
	BOOL	Restore( CQuery* pQuery );
	// 틱
	void	OnTimer( void );
};

////////////////////////////////////////////////////////////////////////////////
class CQuery;
// 트랜스 서버용 군주 이벤트
class CLEvent : public ILordEvent
{
public:
	CLEvent( CLord* pLord );
	virtual	~CLEvent();
	// 복원
	BOOL	Restore( CQuery * pQuery );
	// 틱
	void	OnTimer( void );
protected:
	// 템플릿 메소드 함수
	// 군주 이벤트 추가에 대한 저장 및 결과 전송
	virtual	BOOL	DoTestAddComponent( CLEComponent* pComponent );
	// 군주 이벤트 초기화 적용 및 결과 전송
	virtual	BOOL	DoTestInitialize( void );
	// 모든 군주 이벤트의 지속 시간 감소
	BOOL	DecrementAllComponentTick( void );
};

// 입찰금 반환을 처리하기 위한 함수자 클래스
struct	PostDeposit:
	public unary_function< SPC, void >	{
	void operator() ( const SPC & lhs )	const;
	PostDeposit( CQuery* pQuery )	{	m_pQuery	= pQuery;	}
private:
	CQuery* m_pQuery;
};

////////////////////////////////////////////////////////////////////////////////
class CLordSkillComponentODBC;
// 군주와 관련된 모든 처리의 직렬화를 위한 클래스
// 별도의 스레드 생성
// 데이터베이스 접근 객체 생성
class CLController : public CDbController
{
public:
	CLController( CTLord* pLord );
	virtual	~CLController();

	// 복원
	BOOL	Restore( void );
	// 입후보 시작 상태 저장
	BOOL	BeginCandidacy( void );
	// 투표 시작 상태 저장
	int		BeginVote( void );
	// 투표 종료 상태 저장
	BOOL	EndVote( u_long idPlayer );
	// 군주 입찰 저장
	BOOL	AddDeposit( u_long idPlayer, __int64 iDeposit, time_t tCreate );
	// 공약 설정 저장
	BOOL	SetPledge( u_long idPlayer, const char* szPledge );
	// 투표 저장
	BOOL	IncVote( u_long idPlayer, u_long idElector );
	// 군주 이벤트 생성 저장
	BOOL	AddLEComponent( CLEComponent* pComponent );
	// 군주 이벤트 초기화 저장
	BOOL	InitializeLEvent( void );
	// 군주 스킬 재사용 대기 시간 저장
	BOOL	UpdateLordSkillTick( CLordSkillComponent* pSkill, int nTick );
	// 군주 이벤트 지속 시간 저장
	BOOL	UpdateLordEventTick( CLEComponent* pComponent );
	// 처리를 m_pLord에 위임
	virtual	void	Handler( LPDB_OVERLAPPED_PLUS pov, DWORD dwCompletionKey );
	// 처리를 m_pLord에 위임
	virtual	void	OnTimer( void ); 

private:
	CTLord*	m_pLord;	// 군주 객체
};

#endif	// __LORD

#endif	// __TLORD_H__
