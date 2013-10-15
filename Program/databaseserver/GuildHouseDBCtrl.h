// GuildHouseDBCtrl.h: interface for the CGuildHouseDBCtrl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GUILDHOUSEDBCTRL_H__C2521D9F_8544_4454_810A_F334CA03AABC__INCLUDED_)
#define AFX_GUILDHOUSEDBCTRL_H__C2521D9F_8544_4454_810A_F334CA03AABC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if __VER >= 15 // __GUILD_HOUSE
#include "dbcontroller.h"
#include "GuildHouse.h"

//////////////////////////////////////////////////////////////////////
// CGuildHouseDBCtrl::CDbController
//////////////////////////////////////////////////////////////////////
enum
{
	GUILDHOUSE_LOAD = 0,
	GUILDHOUSE_SEND,
	GUILDHOUSE_CREATE,
	GUILDHOUSE_REMOVE,
	GUILDHOUSE_DEFAULT_PACKET,
	GUILDHOUSE_SAVE_RESTPOINT,
#ifdef __GUILD_HOUSE_MIDDLE
	GUILDHOUSE_TENDER_JOIN,
	GUILDHOUSE_TENDER_UPDATE,
	GUILDHOUSE_GRADE_UPDATE,
	GUILDHOUSE_LEVEL_UPDATE,
	GUILDHOUSE_EXPIRED,
	GUILDHOUSE_TENDER_END,
#endif // __GUILD_HOUSE_MIDDLE
};

class CGuildHouseDBCtrl : public CDbController  
{
public:
	CGuildHouseDBCtrl();
	virtual ~CGuildHouseDBCtrl();

	virtual	void Handler( LPDB_OVERLAPPED_PLUS pov, DWORD dwCompletionKey );
	virtual void OnTimer();

private:
	BOOL	LoadGuildHouse() { return GuildHouseMng->DBLoadGuildHouse( GetQueryObject() ); }
	void	SendGuildHouse( DPID dpId );
	void	CreateGuildHouse( CAr & ar, DPID dpId );
	void	RemoveGuildHouse( DWORD dwGuildId );
	void	RunDefaultPacket( CAr & ar, DPID dpId );
	BOOL	ExcuteQuery( DWORD dwGuildId, CGuildHouseBase* pGuildHouse, int nPacketType, GH_Fntr_Info & gfi, int nIndex );

#ifdef __GUILD_HOUSE_MIDDLE
private:
	void	SendGuildHouseTenderInfo( DPID dpId );
	
	void	JoinGuildHouseTender( CAr & ar, DPID dpId );
	bool	InsertGuildHouseTender( CGuildHouseMng::GUILDHOUSE_TENDER& GHT );
	bool	UpdateGuildHouseTender( CGuildHouseMng::GUILDHOUSE_TENDER& GHT );

	void	UpgradeGuildHouseGrade( CAr & ar, DPID dpId );
	void	RemoveAllFurniture( DWORD dwGuildId );
	void	UpdateGuildHouseGrade( DWORD dwGuildId, DWORD dwWorldId, OBJID objGHId );

	void	UpgradeGuildHouseLevel( CAr & ar, DPID dpId );

	void	UpdateGuildHouseExpired( CAr & ar );

	CGuildHouseBase*	ReMakeGuildHouse( DWORD dwGuildId, DWORD dwWorldId );
#endif // __GUILD_HOUSE_MIDDLE
};


//////////////////////////////////////////////////////////////////////
// CGuildHouseDBMng
//////////////////////////////////////////////////////////////////////
#define GuildHouseDBMng CGuildHouseDBMng::GetInstance()
class CGuildHouseDBMng
{
public:
	CGuildHouseDBMng();
	~CGuildHouseDBMng();
	static CGuildHouseDBMng* GetInstance();
	
	BOOL PostRequest( int nQuery, BYTE* lpBuf = NULL, int nBufSize = 0, DWORD dwCompletionKey = 0 )
	{ return m_GHDBCtrl.PostRequest( nQuery, lpBuf, nBufSize, dwCompletionKey );	}

private:
	CGuildHouseDBCtrl	m_GHDBCtrl;
};

#endif // __GUILD_HOUSE
#endif // !defined(AFX_GUILDHOUSEDBCTRL_H__C2521D9F_8544_4454_810A_F334CA03AABC__INCLUDED_)
