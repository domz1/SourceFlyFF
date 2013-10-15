// DPCollectionSrvr.h: interface for the CDPCollectionSrvr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DPCOLLECTIONSRVR_H__EB1E89B7_9EE1_4D05_A7DB_EF27A405C682__INCLUDED_)
#define AFX_DPCOLLECTIONSRVR_H__EB1E89B7_9EE1_4D05_A7DB_EF27A405C682__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "dpmng.h"
#include "msghdr.h"
#include "misc.h"

using	namespace	std;

#undef	theClass
#define theClass	CDPCollectionSrvr
#undef theParameters
#define theParameters	CAr & ar, DPID, LPBYTE, u_long

#define	DPCollectionSrvr	CDPCollectionSrvr::GetInstance()
class CDPCollectionSrvr : public CDPMng
{
public:
	CDPCollectionSrvr();
	virtual ~CDPCollectionSrvr();

	static CDPCollectionSrvr*	GetInstance();

	virtual void	SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID dpid );
	virtual void	UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID dpid );

	USES_PFNENTRIES;
	void	OnCollectionCertify( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize );
};

#endif // !defined(AFX_DPCOLLECTIONSRVR_H__EB1E89B7_9EE1_4D05_A7DB_EF27A405C682__INCLUDED_)
