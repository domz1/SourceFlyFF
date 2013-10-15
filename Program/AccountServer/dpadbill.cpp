#include "stdafx.h"
#include "dpdbsrvr.h"
#include "dpadbill.h"

extern	CDPDBSrvr	g_dpDbSrvr;

CDPAdbill::CDPAdbill()
{
	m_dwKey	= 0;
}

CDPAdbill::~CDPAdbill()
{

}

void CDPAdbill::SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )
{
	switch( lpMsg->dwType )
	{
		case DPSYS_CREATEPLAYERORGROUP:
			{
//				LPDPMSG_CREATEPLAYERORGROUP lpCreatePlayer	= (LPDPMSG_CREATEPLAYERORGROUP)lpMsg;
//				OnAddConnection( lpCreatePlayer->dpId );
				break;
			}
		case DPSYS_DESTROYPLAYERORGROUP:
			{
//				LPDPMSG_DESTROYPLAYERORGROUP lpDestroyPlayer	= (LPDPMSG_DESTROYPLAYERORGROUP)lpMsg;
//				OnRemoveConnection( lpDestroyPlayer->dpId );
				break;
			}
	}
}

void CDPAdbill::UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )
{
	PBUYING_INFO3 pbi3	= new BUYING_INFO3;
	memcpy( (void*)pbi3, lpMsg, sizeof(BUYING_INFO) );
	pbi3->dpid	= idFrom;
	pbi3->dwKey	= m_dwKey++;
	pbi3->dwTickCount	= GetTickCount();

	CBuyingInfoMng::GetInstance()->Add( pbi3 );
	g_dpDbSrvr.SendBuyingInfo( (PBUYING_INFO2)pbi3 );

//	TRACE( "RECV PACKETTYPE_BUYING_INFO FROM ADBILLSOFT\n" );
	static	char lpOutputString[260]	= { 0, };
	sprintf( lpOutputString, "ACCOUNTSERVER.EXE\t// Recv from billing\t// dwServerIndex = %d\tdwPlayerId = %d\tdwSenderId=%d\tdwItemId = %d\tdwItemNum = %d", 
		pbi3->dwServerIndex, pbi3->dwPlayerId, pbi3->dwSenderId, pbi3->dwItemId, pbi3->dwItemNum );
	OutputDebugString( lpOutputString );
//	TRACE( "dwServerIndex = %d\tdwPlayerId = %d\tdwItemId = %d\tdwItemNum = %d\n", 
//		pbi3->dwServerIndex, pbi3->dwPlayerId, pbi3->dwItemId, pbi3->dwItemNum );
//	FILEOUT( "buyinginfo.txt", "dwServerIndex = %d\tdwPlayerId = %d\tdwItemId = %d\tdwItemNum = %d\n", 
//		pbi3->dwServerIndex, pbi3->dwPlayerId, pbi3->dwItemId, pbi3->dwItemNum );
}

CDPAdbill* CDPAdbill::GetInstance()
{
	static CDPAdbill	sdpAdbill;
	return &sdpAdbill;
}

CBuyingInfoMng*	CBuyingInfoMng::GetInstance()
{
	static CBuyingInfoMng	sBuyingInfoMng;
	return &sBuyingInfoMng;
}