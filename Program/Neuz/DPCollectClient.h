// DPCollectClient.h: interface for the CDPCollectClient class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DPCOLLECTCLIENT_H__B440BAE5_C880_419E_9EE5_902576282132__INCLUDED_)
#define AFX_DPCOLLECTCLIENT_H__B440BAE5_C880_419E_9EE5_902576282132__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "dpmng.h"
#include "msghdr.h"

#undef	theClass
#define theClass	CDPCollectClient
#undef theParameters
#define theParameters	CAr & ar, DPID

#define	DPCollectClient	CDPCollectClient::GetInstance()
class CDPCollectClient : public CDPMng
{
public:
	CDPCollectClient();
	virtual ~CDPCollectClient();

	static	CDPCollectClient*	GetInstance();

	virtual void	SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID dpId );
	virtual void	UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID dpId );

	void	SendCollectionCertify( const char* lpszCertifyIP, const char* lpszClientVer, int nGameVer, int nLang );

	USES_PFNENTRIES;
};

#endif // !defined(AFX_DPCOLLECTCLIENT_H__B440BAE5_C880_419E_9EE5_902576282132__INCLUDED_)
