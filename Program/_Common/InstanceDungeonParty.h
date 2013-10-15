// InstanceDungeonParty.h: interface for the CInstanceDungeonParty class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INSTANCEDUNGEONPARTY_H__6DEF4167_3C00_4369_9AA3_A846FDF6EFD9__INCLUDED_)
#define AFX_INSTANCEDUNGEONPARTY_H__6DEF4167_3C00_4369_9AA3_A846FDF6EFD9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if __VER >= 14 // __INSTANCE_DUNGEON
#include "InstanceDungeonBase.h"

class CInstanceDungeonParty : public CInstanceDungeonBase
{
public:
	CInstanceDungeonParty();
	virtual ~CInstanceDungeonParty();
	static CInstanceDungeonParty* GetInstance();

#ifdef __WORLDSERVER
	virtual BOOL	EnteranceDungeon( CUser* pUser, DWORD dwWorldId );
	virtual BOOL	LeaveDungeon( CUser* pUser, DWORD dwWorldId );
	BOOL			SetPartyLeaveTime( DWORD dwPlayerId );
	void			Process();
private:
	BOOL			IsPartyDungeon( DWORD dwWorldId ) { return CInstanceDungeonHelper::GetInstance()->IsDungeonType( dwWorldId, GetType() ); }
	
	typedef map<DWORD, DWORD> MAP_PLTIME;
	MAP_PLTIME m_mapPLTime;
#endif // __WORLDSERVER
};

#endif // __INSTANCE_DUNGEON

#endif // !defined(AFX_INSTANCEDUNGEONPARTY_H__6DEF4167_3C00_4369_9AA3_A846FDF6EFD9__INCLUDED_)
