// Campus.h: interface for the CCampus class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CAMPUS_H__D0177E19_6285_41A9_9AB5_6A145BBD08BC__INCLUDED_)
#define AFX_CAMPUS_H__D0177E19_6285_41A9_9AB5_6A145BBD08BC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if __VER >= 15 // __CAMPUS
#include "ar.h"

#define		CAMPUS_MASTER			1
#define		CAMPUS_PUPIL			2
#define		MAX_PUPIL_NUM			3
#define		MIN_LV2_POINT			41
#define		MIN_LV3_POINT			101

class CCampusMember
{
public:
	CCampusMember();
	~CCampusMember();

	void	Serialize( CAr & ar );

	u_long	GetPlayerId()	{	return m_idPlayer;	}
	void	SetPlayerId( u_long idPlayer )	{	m_idPlayer = idPlayer;	}

	int		GetLevel()	{	return m_nMemberLv;	}
	void	SetLevel( int nLevel )	{	m_nMemberLv = nLevel;	}

private:
	u_long	m_idPlayer;
	int		m_nMemberLv;
};
typedef map<u_long, CCampusMember*>		MAP_CM;

class CCampus
{
public:
	CCampus();
	~CCampus();

	void	Clear();
	void	Serialize( CAr & ar );
	
	u_long	GetCampusId()	{	return m_idCampus;	}
	void	SetCampusId( u_long idCampus )	{	m_idCampus = idCampus;	}
	u_long	GetMaster()		{	return m_idMaster;	}
	void	SetMaster( u_long idMaster )	{	m_idMaster = idMaster;	}
	BOOL	IsMaster( u_long idPlayer )		{	return ( idPlayer == m_idMaster );	}
	int		GetMemberSize()	{	return m_mapCM.size();	}

	BOOL	IsPupil( u_long idPlayer );
	vector<u_long>	GetPupilPlayerId();
	int		GetPupilNum();
	
	vector<u_long>	GetAllMemberPlayerId();
	int		GetMemberLv( u_long idPlayer );
	BOOL	IsMember( u_long idPlayer );
	BOOL	AddMember( CCampusMember* pCM );
	BOOL	RemoveMember( u_long idPlayer );
	CCampusMember*	GetMember( u_long idPlayer );
	

private:
	u_long	m_idCampus;
	u_long	m_idMaster;
	MAP_CM	m_mapCM;

#ifdef __WORLDSERVER
public:
	BOOL	IsChangeBuffLevel( u_long idPlayer );
	int		GetBuffLevel( u_long idPlayer );

private:
	int		m_nPreBuffLevel;
#endif // __WORLDSERVER
};
typedef map<u_long, CCampus*>	MAP_CAMPUS;
typedef map<u_long, u_long>		MAP_PID2CID;

class CCampusMng
{
public:
	CCampusMng();
	~CCampusMng();

	BOOL	IsEmpty()	{	return m_mapCampus.empty();	}
	void	Clear();
	void	Serialize( CAr & ar );

	u_long	AddCampus( CCampus* pCampus );
	BOOL	RemoveCampus( u_long idCampus );

	CCampus*	GetCampus( u_long idCampus );

	bool	AddPlayerId2CampusId( u_long idPlayer, u_long idCampus );
	void	RemovePlayerId2CampusId( u_long idPlayer )	{	m_mapPid2Cid.erase( idPlayer );	}
	u_long	GetCampusIdByPlayerId( u_long idPlayer );

private:
	u_long	m_idCampus;
	MAP_CAMPUS	m_mapCampus;
	MAP_PID2CID	m_mapPid2Cid;
};
#endif // __CAMPUS
#endif // !defined(AFX_CAMPUS_H__D0177E19_6285_41A9_9AB5_6A145BBD08BC__INCLUDED_)
