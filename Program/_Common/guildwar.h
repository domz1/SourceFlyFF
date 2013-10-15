#ifndef __GUILDWAR_H__
#define	__GUILDWAR_H__

#include "ar.h"

typedef	struct	_WAR_ENTRY
{
	u_long	idGuild;
	int		nSize;
	int		nSurrender;
	int		nDead;
	int		nAbsent;
}
WAR_ENTRY, *PWAR_ENTRY;

#define	WF_WARTIME	'0'
#define	WF_W_GN		'1'
#define	WF_W_SR		'2'
#define	WF_L_GN		'3'
#define	WF_L_SR		'4'
#define	WF_TRUCE	'5'
#define	WF_DRAW		'6'
#define	WF_END		'9'

enum
{
	WR_DECL_GN, WR_ACPT_GN, WR_DECL_SR, WR_ACPT_SR, WR_DECL_AB, WR_ACPT_AB, 
		WR_DECL_DD, WR_ACPT_DD, WR_TRUCE, WR_DRAW,
};

//WR_DECL_GN : 마스터를 죽여서 선언측 승리
//WR_ACPT_GN : 마스터를 죽여서 수락측 승리
//WR_DECL_SR : 선언측 승( 항복 )
//WR_ACPT_SR : 수락층 승( 항복 )
//WR_DECL_AB : 전쟁 종료시 선언측이 마스터시간 많아서 승
//WR_ACPT_AB : 전쟁 종료시 선언측이 마스터시간 많아서 승
//WR_DECL_DD : 종료시 많이 죽여서 선언측 승
//WR_ACPT_DD : 종료시 많이 죽여서 선언측 승
//WR_TRUCE	 : 휴전( 비김 )
//WR_DRAW	 : 승패를 매길수 없음( 비김 )

class CGuildWar
{
public:
	u_long	m_idWar;
	WAR_ENTRY	m_Decl;
	WAR_ENTRY	m_Acpt;
	char	m_nFlag;
	CTime	m_time;	// start
public:
//	Constructions
	CGuildWar();
	~CGuildWar();
//	Operations
	void	Serialize( CAr & ar );
#ifdef __WORLDSERVER
	void	Process( CTime & time );
#endif	// __WORLDSERVER
//	Attributes
	BOOL	IsDecl( u_long idGuild )	{	return( m_Decl.idGuild == idGuild );	}
#ifdef __INTERNALSERVER
	CTime	GetEndTime( void )	{	return( m_time + CTimeSpan( 0, 0, 10, 0 ) );	}
#else	// __INTERNALSERVER
	CTime	GetEndTime( void )	{	return( m_time + CTimeSpan( 0, 2, 0, 0 ) );	}
#endif	// __INTERNALSERVER
};

#include <map>

class CMover;
class CGuild;

class CGuildWarMng
{
//private:
public:
	u_long	m_id;
public:
//	CRIT_SEC	m_AddRemoveLock;
	map<u_long, CGuildWar*>	m_mapPWar;
//	Constructions
	CGuildWarMng();
	~CGuildWarMng();
	void	Clear( void );
//	Operations
	void Serialize( CAr & ar );
	u_long	AddWar( CGuildWar* pGuildWar );
	BOOL	RemoveWar( u_long idWar );
	void	Result( CGuildWar* pWar, CGuild* pDecl, CGuild* pAcpt, int nType, int nWptDecl = 0, int nWptAcpt = 0 );
//	Attributes
	CGuildWar*	GetWar( u_long idWar );
	int		GetSize( void )	{	return m_mapPWar.size();	}
#ifdef __WORLDSERVER
	void	Process( void );
#endif	// __WORLDSERVER
};

#endif	// __GUILDWAR_H__