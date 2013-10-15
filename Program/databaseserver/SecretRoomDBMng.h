#pragma once

#if __VER >= 12 // __SECRET_ROOM
#include "dbcontroller.h"

enum {	QUERY_SECRETROOM_LOAD = 0,
QUERY_SECRETROOM_TENDER_INSERT,
QUERY_SECRETROOM_TENDER_UPDATE,
QUERY_SECRETROOM_LINEUP_INSERT,
QUERY_SECRETROOM_CLOSED,
};

class CSecretRoomDbController : public CDbController
{
public:
	CSecretRoomDbController(void);
	~CSecretRoomDbController(void);

	virtual	void	Handler( LPDB_OVERLAPPED_PLUS pov, DWORD dwCompletionKey );

private:
	void LoadSecretRoom( DPID dpId );
	void InsertTenderToDB( BYTE nContinent, DWORD dwGuildId, int nPenya );
	void UpdateTenderToDB( BYTE nContinent, DWORD dwGuildId, int nPenya, char chState, DWORD dwWorldId, int nWarState, int nKillCount );
	void InsertLineUpMemberToDB( BYTE nContinent, DWORD dwGuildId, vector<DWORD>& vecMemberId );

	int m_nTimes;
};

class CSecretRoomDBMng
{
public:
	CSecretRoomDBMng(void);
	~CSecretRoomDBMng(void);
	static CSecretRoomDBMng* GetInstance( void );

	BOOL	PostRequest( int nQuery, BYTE* lpBuf = NULL, int nBufSize = 0, DWORD dwCompletionKey = 0 );
	
private:
	CSecretRoomDbController m_SRDbController;

};

#endif // __SECRET_ROOM