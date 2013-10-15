#pragma once

#if __VER >= 13 // __HOUSING
#include "DbController.h"
#include "Housing.h"
////////////////////////////////////////////////////////////////////
/// CHousingDBCtrl /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
enum
{
	QUERY_HOUSING_LOAD = 0,
	QUERY_HOUSING_INSERT_LIST,
	QUERY_HOUSING_SETUP_FURNITURE,
	QUERY_HOUSING_SET_VISITALLOW,
	QUERY_HOUSING_GM_REMOVEALL,
};


class CHousingDBCtrl : public CDbController
{
public:
	CHousingDBCtrl(void);
	virtual ~CHousingDBCtrl(void);

	virtual	void Handler( LPDB_OVERLAPPED_PLUS pov, DWORD dwCompletionKey );
	virtual void OnTimer();
	
	void DeleteFurnitureList( DWORD dwPlayerId, HOUSINGINFO housingInfo );

private:
	void LoadHousingInfo( DWORD dwPlayerId, DPID dpId );
	void InsertFurnitureList( DWORD dwPlayerId, DWORD dwItemId, DPID dpId );
	void SetupFurniture( DWORD dwPlayerId, HOUSINGINFO housingInfo, DPID dpId );
	void SetVisitAllow( DWORD dwPlayerId, DWORD dwTargetId, BOOL bAllow, DPID dpId );
};


////////////////////////////////////////////////////////////////////
/// CHousingDBMng //////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
class CHousingDBMng
{
public:
	CHousingDBMng(void);
	~CHousingDBMng(void);

	static CHousingDBMng* GetInstance();
	BOOL PostRequest( int nQuery, BYTE* lpBuf = NULL, int nBufSize = 0, DWORD dwCompletionKey = 0 )
	{ return m_HousingDBCtrl.PostRequest( nQuery, lpBuf, nBufSize, dwCompletionKey );	}

	void DeleteFurnitureList( DWORD dwPlayerId, HOUSINGINFO housingInfo );

private:
	CHousingDBCtrl m_HousingDBCtrl;

};
#endif // __HOUSING