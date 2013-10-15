#include "stdafx.h"
#include "dpwldsrvr.h"
#include "globalgiftbox.h"

CDPWldSrvr::CDPWldSrvr()
{
	BEGIN_MSG;
	ON_MSG( PACKETTYPE_GLOBALGIFTBOX, OnUpload );
	ON_MSG( PACKETTYPE_QUERYGLOBALGIFTBOX, OnQuery );
	ON_MSG( PACKETTYPE_RESTOREGLOBALGIFTBOX, OnRestore );
}

CDPWldSrvr::~CDPWldSrvr()
{
}

void CDPWldSrvr::SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )
{
	switch( lpMsg->dwType )
	{
		case DPSYS_CREATEPLAYERORGROUP:
			{
				LPDPMSG_CREATEPLAYERORGROUP lpCreatePlayer	= (LPDPMSG_CREATEPLAYERORGROUP)lpMsg;
				break;
			}
		case DPSYS_DESTROYPLAYERORGROUP:
			{
				LPDPMSG_DESTROYPLAYERORGROUP lpDestroyPlayer	= (LPDPMSG_DESTROYPLAYERORGROUP)lpMsg;
				break;
			}
	}
}

void CDPWldSrvr::UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )
{
	CAr ar( (LPBYTE)lpMsg, dwMsgSize );
	GETTYPE( ar )
	void ( theClass::*pfn )( theParameters )	=	GetHandler( dw );
	ASSERT( pfn );
	( this->*( pfn ) )( ar, idFrom );
}

CDPWldSrvr*	CDPWldSrvr::GetInstance( void )
{
	static CDPWldSrvr	sDPWldSrvr;
	return &sDPWldSrvr;
}

void CDPWldSrvr::OnUpload( CAr & ar, DPID dpid )
{
	CGlobalGiftbox::GetInstance()->OnUpload( ar );
}

void CDPWldSrvr::OnQuery( CAr & ar, DPID dpid )
{
	CGlobalGiftbox::GetInstance()->OnQuery( this, ar, dpid );
}

void CDPWldSrvr::OnRestore( CAr & ar, DPID dpid )
{
	CGlobalGiftbox::GetInstance()->OnRestore( ar );
}