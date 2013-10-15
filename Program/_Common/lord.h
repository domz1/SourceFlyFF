#ifndef __LORD_H__
#define	__LORD_H__

#if __VER >= 12 // __LORD

#include <string>
#include <vector>
#include <functional>
#include <boost/shared_ptr.hpp>

#ifdef __WORLDSEVER
#include "lordskillexecutable.h"
#else	// __WORLDSEVER
#include "lordskill.h"
#endif	// __WORLDSEVER

using	namespace	std;

////////////////////////////////////////////////////////////////////////////////
class CAr;
class	CCandidate
{
public:
	enum	{	nMaxPledgeLen	= 255, nMinDeposit	= 100000000, nSetPledgeCost	= 1000000	};

	CCandidate();
	CCandidate( u_long idPlayer, __int64 iDeposit, const char* szPledge, int nVote, time_t tCreate );
	~CCandidate();

	__int64		AddDeposit( __int64 iDeposit );
	void	SetPledge( const char*	szPledge )	{	lstrcpy( m_szPledge, szPledge );	}
	u_long	GetIdPlayer( void )		{	return m_idPlayer;		}
	const char*		GetPledge( void )	const	{	return m_szPledge;	}
	__int64		GetDeposit( void )	const	{	return m_iDeposit;	}
	void	SetVote( int nVote )	{	m_nVote		= nVote;	}
	int		GetVote( void )		{	return m_nVote;		}
	void	Serialize( CAr & ar );
	time_t	GetCreate( void )	{	return m_tCreate;	}

private:
	u_long	m_idPlayer;
	__int64		m_iDeposit;
	char	m_szPledge[nMaxPledgeLen];
	int		m_nVote;
	time_t	m_tCreate;
};

////////////////////////////////////////////////////////////////////////////////
typedef	boost::shared_ptr<CCandidate>	SPC;		// smart pointer to CCandidate
typedef	vector<SPC>		VSPC;

class IElection	// abstract
{
public:
	enum	ELECTION_STATE	{	eReady, eCandidacy, eVote, eExpired	};
	enum	{	nMaxCandidates	= 10, nLevelRequirements = 60	};
protected:
	IElection( CLord* pLord );

public:
	virtual	~IElection()	= 0;

	CCandidate*	AddCandidate( u_long idPlayer, __int64 iDeposit = 0, const char* szPledge = "", int nVote = 0, time_t tCreate = 0 );
	CCandidate*	AddCandidate( CCandidate* pCandidate );

	u_long	GetId( void )	{	return m_idElection;	}
	CCandidate*		GetCandidate( u_long idPlayer );
	int		GetOrder( u_long idPlayer );
	void	SetProperty( int nDayOfWeek, int nHour, time_t tCandidacy, time_t tVote );
	void	Serialize( CAr & ar );

	void	SetState( ELECTION_STATE eState )	{	m_eState	= eState;	}
	ELECTION_STATE	GetState( void )	{	return m_eState;	}
	ELECTION_STATE	GetPropertyState( void );

	void	SetBegin( time_t tBegin )	{	m_tBegin	= tBegin;	}
	time_t	GetBegin( void )	{	return m_tBegin;	}
	void	PrepareNext( void );

	size_t	GetCandidatesSize( void )	{	return m_vCandidates.size();	}

protected:
	virtual	BOOL	DoTestBeginCandidacy( void )	= 0;
	virtual	void	DoReturnDeposit( void )		{}
	virtual	BOOL	DoTestBeginVote( int & nRequirement )	= 0;
	virtual	BOOL	DoTestEndVote( u_long idPlayer )	= 0;
	virtual	BOOL	DoTestAddDeposit( u_long idPlayer, __int64 iDeposit, time_t tCreate )		{	return TRUE;	}
	virtual	void	DoAddDepositComplete( u_long idPlayer, __int64 iDeposit, time_t tCreate )	{}
	virtual	BOOL	DoTestSetPledge( u_long idPlayer, const char* szPledge )	{	return TRUE;	}
	virtual	void	DoSetPlegeComplete( void )		{}
	virtual	BOOL	DoTestIncVote( u_long idPlayer, u_long idElector )	{	return TRUE;	}
	virtual	void	DoIncVoteComplete( void )	{}
	virtual	void	DoEndVoteComplete( void )	{}

public:
	void	BeginCandidacy( void );
	void	BeginVote( int nRequirement );
	void	EndVote( u_long idPlayer );
	void	AddDeposit( u_long idPlayer, __int64 iDeposit, time_t tCreate );
	void	SetPledge( u_long idPlayer, const char*	szPledge );
	void	IncVote( u_long idPlayer, u_long idElector );

	BOOL	HasPledge( u_long idPlayer );

	int		GetVote( void );
	int		GetRequirement( void )	{	return m_nRequirement;	}
	void	SetRequirement( int nRequirement )	{	m_nRequirement	= nRequirement;		}
	BOOL	IsValid( void )		{	return( GetVote() >= m_nRequirement );	}
	u_long	GetResult( void );

	time_t	GetNextBegin( void );
	
protected:
	void	SetNextBegin( void );
	void	SortDeposit( void );
	void	SortVote( void );

protected:
	CLord*	m_pLord;
	u_long	m_idElection;
	int		m_nRequirement;
	ELECTION_STATE	m_eState;
	VSPC	m_vCandidates;
	time_t	m_tBegin;

	// 스크립트
	struct	sProperty
		{	
			int		nDayOfWeek;		// 시작 요일
			int		nHour;		// 시작 시간
			time_t	tCandidacy;		// 입후보 기간
			time_t	tVote;	// 투표 기간
			int		nDays;	// 군주 투표 간격
			vector<float>	m_vReturnDepositRates;	// 입찰금 반환 비율
			vector<int>	m_vItems[2];	// 군주 아이템
			float	fRequirementFactor;		// 최소 군주 득표 비율
			sProperty()	: nDayOfWeek( 0 ), nHour( 0 ), tCandidacy( 0 ), tVote( 0 ), fRequirementFactor( 0.0f ), nDays( 0 )	{}
			void	Serialize( CAr & ar );
		}	property;
};

// 후보간 입찰금 비교 함수자
struct	DepositCompare:
	public binary_function<SPC, SPC, bool>	{
	bool operator() ( const SPC & lhs, const SPC & rhs )	const
	{
//		return lhs->GetDeposit() > rhs->GetDeposit();
		return ( lhs->GetDeposit() == rhs->GetDeposit()?
			lhs->GetCreate() < rhs->GetCreate():
			lhs->GetDeposit() > rhs->GetDeposit() );
	}
};

////////////////////////////////////////////////////////////////////////////////
struct	VoteCompare:
	public binary_function<SPC, SPC, bool>	{
	bool operator() ( const SPC & lhs, const SPC & rhs )	const
	{
//		return lhs->GetVote() > rhs->GetVote();
		return ( lhs->GetVote() == rhs->GetVote()?
			lhs->GetCreate() < rhs->GetCreate():
			lhs->GetVote() > rhs->GetVote() );
	}
};

////////////////////////////////////////////////////////////////////////////////
class CLEComponent
{
public:
	CLEComponent();
	CLEComponent( int t, u_long p, float e, float i );
	virtual	~CLEComponent();
	void	Serialize( CAr & ar );
	int		GetTick( void )		{	return nTick;	}
	void	SetTick( const int nTick )	{	this->nTick	= nTick;	}
	u_long	GetIdPlayer( void )	{	return idPlayer;	}
	float	GetEFactor( void )	{	return fEFactor;	}
	float	GetIFactor( void )	{	return fIFactor;		}
	int		Decrement( void );
#ifdef __CLIENT
	CTexture*	GetTexture( void )	{	return m_pTexture;	}
	void	SetTexture( void );
#endif	// __CLIENT

private:
	int		nTick;
	u_long	idPlayer;
	float	fEFactor;
	float	fIFactor;
#ifdef __CLIENT
	CTexture*	m_pTexture;
#endif	// __CLIENT
};

////////////////////////////////////////////////////////////////////////////////
typedef pair<float, __int64>	PFI64;
typedef vector<PFI64>	VPFI64;
typedef	vector<CLEComponent*>	VLEC;

class CLord;
class ILordEvent
{
public:
	enum	{	eTick	= 60,	};
protected:
	ILordEvent( CLord* pLord );
public:
	virtual	~ILordEvent();

	void	Clear( void );
	void	AddComponent( CLEComponent* pComponent, BOOL bHook = TRUE );
	void	AddComponent( u_long idPlayer, int iEEvent, int iIEvent );
	void	Initialize( void );
	CLEComponent*	GetComponent( u_long idPlayer );
	CLEComponent*	GetComponentAt( int i );
	int		GetComponentSize( void )		{	return m_vComponents.size();	} 
	__int64		GetCost( int iEEvent, int iIEvent );
	
	float	GetEFactor( int iEEvent );
	float	GetIFactor( int iIEvent );
	int		GetEFactorSize( void )	{	return m_vEFactor.size();	}
	int		GetIFactorSize( void )	{	return m_vIFactor.size();	}
	float	GetEFactor( void );
	float	GetIFactor( void );

	void	Serialize( CAr & ar );
	BOOL	Initialize( const char* szFile );
	void	SerializeTick( CAr & ar );
	void	EraseExpiredComponents( void );

protected:
	virtual	BOOL	DoTestAddComponent( CLEComponent* pComponent )	= 0;
	virtual	BOOL	DoTestInitialize( void )	= 0;
	
private:
	VLEC::iterator	Remove( VLEC::iterator i );

private:
	VPFI64	m_vEFactor;
	VPFI64	m_vIFactor;
protected:
	VLEC	m_vComponents;
	CLord*	m_pLord;
};

////////////////////////////////////////////////////////////////////////////////
class CLController;
class CLord	// mediator
{
public:
	~CLord();
	void	Serialize( CAr & ar );
	
	void	Set( u_long idPlayer )	{	m_idPlayer	= idPlayer;	}
	u_long	Get( void )		{	return m_idPlayer;	}
	BOOL	IsLord( u_long idPlayer )	{	return m_idPlayer == idPlayer;	}

	void	ResetSkill( void )	{	m_pSkills->Reset();		}

	IElection*	GetElection( void )		{	return m_pElection;		}
	ILordEvent*	GetEvent( void )	{	return m_pEvent;	}
	CLordSkill*		GetSkills( void )	{	return m_pSkills;	}
	virtual	void	CreateColleagues( void )	= 0;
	virtual void	DestroyColleagues( void )	= 0;
	virtual	CLController*	GetController( void )	{	return NULL;	}
protected:
	CLord();
private:
	u_long	m_idPlayer;
protected:
	IElection*	m_pElection;
	ILordEvent*	m_pEvent;
	CLordSkill*		m_pSkills;;
};

inline CAr& operator<<( CAr & ar, IElection::ELECTION_STATE e )
	{	return ar.operator<<( (int)e );		}

inline CAr& operator>>( CAr & ar, IElection::ELECTION_STATE & e )
	{	return ar.operator>>( (int&)e );	}

////////////////////////////////////////////////////////////////////////////////
namespace	election
{
	void	OutputDebugString( LPCTSTR lpszFormat, ... );
};

#endif	// __LORD

#endif	// __LORD_H__