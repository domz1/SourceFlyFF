// DPCollectionSrvr.cpp: implementation of the CDPCollectionSrvr class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DPCollectionSrvr.h"
#include "DbManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDPCollectionSrvr::CDPCollectionSrvr()
{
	BEGIN_MSG;
	ON_MSG( PACKETTYPE_COLLECTION_CERTIFY, OnCollectionCertify );
}

CDPCollectionSrvr::~CDPCollectionSrvr()
{

}

CDPCollectionSrvr* CDPCollectionSrvr::GetInstance()
{
	static CDPCollectionSrvr sCDPCS;
	return & sCDPCS;
}

void CDPCollectionSrvr::SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID dpid )
{
	switch( lpMsg->dwType )
	{
	case DPSYS_CREATEPLAYERORGROUP:
		{
			break;
		}
	case DPSYS_DESTROYPLAYERORGROUP:
		{
			break;
		}
	}
}

void CDPCollectionSrvr::UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID dpid )
{
	CAr ar( (LPBYTE)lpMsg, dwMsgSize );
	
	if( dwMsgSize < 4 )
	{
		WriteError( "PACKET//0" );
		return;
	}

	GETTYPE( ar );
	void ( theClass::*pfn )( theParameters )
		=	GetHandler( dw );

	if( pfn )
		( this->*( pfn ) )( ar, dpid, (LPBYTE)lpMsg + sizeof(DWORD), dwMsgSize - sizeof(DWORD) );
}

void CDPCollectionSrvr::OnCollectionCertify( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize )
{
	char szCertifyIP[16] = {0, };
	char szClientVer[30] = {0, };
	int nFlyffVer = 0;
	int nLang = 0;
	char szClientIP[16] = {0, };

	ar.ReadString( szCertifyIP, sizeof( szCertifyIP ) );
	ar.ReadString( szClientVer, sizeof( szClientVer ) );
	ar >> nFlyffVer >> nLang;
	GetPlayerAddr( dpid, szClientIP );

	LPDB_OVERLAPPED_PLUS pData	= DbManager->m_pDbIOData->Alloc();
	strcpy( pData->szCertifyIP, szCertifyIP );
	strcpy( pData->szClientVer, szClientVer );
	pData->nFlyffVer = nFlyffVer;
	pData->nLang = nLang;
	strcpy( pData->szClientIP, szClientIP );
	pData->nQueryMode = LOG_CERTIFY_INFO;
	DbManager->PostQ( pData );

	DestroyPlayer( dpid );
}