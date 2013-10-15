// DPCollectClient.cpp: implementation of the CDPCollectClient class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DPCollectClient.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDPCollectClient::CDPCollectClient()
{

}

CDPCollectClient::~CDPCollectClient()
{

}

CDPCollectClient* CDPCollectClient::GetInstance()
{
	static CDPCollectClient sDPC;
	return & sDPC;
}

void CDPCollectClient::SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID dpId )
{
	switch( lpMsg->dwType )
	{
		case DPSYS_CREATEPLAYERORGROUP:
			{
			}
			break;
		case DPSYS_DESTROYPLAYERORGROUP:
			{
			}
			break;
	}
}

void CDPCollectClient::UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID dpId )
{
	CAr ar( (LPBYTE)lpMsg, dwMsgSize );

	GETTYPE( ar );
	
	void ( theClass::*pfn )( theParameters )	=	GetHandler( dw );

	if( pfn ) {
		( this->*( pfn ) )( ar, dpId );
	}
}

void CDPCollectClient::SendCollectionCertify( const char* lpszCertifyIP, const char* lpszClientVer, int nGameVer, int nLang )
{
	BEFORESEND( ar, PACKETTYPE_COLLECTION_CERTIFY );
	ar.WriteString( lpszCertifyIP );
	ar.WriteString( lpszClientVer );
	ar << nGameVer << nLang;
	SEND( ar, this, DPID_SERVERPLAYER );
}