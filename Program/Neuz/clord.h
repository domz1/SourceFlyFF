#ifndef __CLORD_H__
#define	__CLORD_H__

#if __VER >= 12 // __LORD

#include "lord.h"

// 아래의 클라이언트용 클래스들은
// 기본 클래스들을 상속 받는 후 
// 후크 함수(Do...로 시작하는)들을 재정의 하여
// 화면 출력을 처리하게 한다
class CCLord
	: public CLord
{
public:
	CCLord();
	virtual	~CCLord();
	static	CCLord*	Instance( void );
	virtual	void	CreateColleagues( void );
	virtual void	DestroyColleagues( void );
};

////////////////////////////////////////////////////////////////////////////////
class CCElection :
	public IElection
{
public:
		CCElection( CLord* pLord );
		virtual	~CCElection();

		void	UpdateUI( void );
		void	State( void );
		SPC GetRanker(int nRanking);

		time_t	GetRestTimeBeginCandidacy( void )
			{	return GetBegin() - time_null();	}
		time_t	GetRestTimeBeginVote( void )
			{	return GetBegin() + property.tCandidacy - time_null();	}
		time_t	GetRestTimeEndVote( void )
			{	return GetBegin() + property.tCandidacy + property.tVote - time_null();	}

		void	PrintCaption( const char* lpCaption );
protected:
	virtual	BOOL	DoTestBeginCandidacy( void );
	virtual	BOOL	DoTestBeginVote( int & nRequirement );
	virtual	BOOL	DoTestEndVote( u_long idPlayer );
	virtual	BOOL	DoTestAddDeposit( u_long idPlayer, __int64 iDeposit, time_t tCreate );
	virtual	void	DoAddDepositComplete( u_long idPlayer, __int64 iDeposit, time_t tCreate );
	virtual	void	DoSetPlegeComplete( void );
	virtual	BOOL	DoTestIncVote( u_long idPlayer, u_long idElector );
	virtual	void	DoIncVoteComplete( void );
	virtual	void	DoEndVoteComplete( void );
};

////////////////////////////////////////////////////////////////////////////////
class CLEvent : public ILordEvent
{
public:
	CLEvent( CLord* pLord );
	virtual	~CLEvent();
	void	UpdateUI();
protected:
	virtual	BOOL	DoTestAddComponent( CLEComponent* pComponent );
	virtual	BOOL	DoTestInitialize( void );
};

////////////////////////////////////////////////////////////////////////////////
namespace	election
{
	BOOL	IsActivePlayer( u_long idPlayer );
};

////////////////////////////////////////////////////////////////////////////////

#endif	// __LORD

#endif	// __CLORD_H__