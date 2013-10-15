#ifndef __BILLINGMGRJP_H__
#define	__BILLINGMGRJP_H__

#include "BillingMgr.h"


class CDPBilling;

class CBillingMgrJP : public CBillingMgr
{
public:
	CBillingMgrJP();
	virtual ~CBillingMgrJP();

	virtual bool		Init( HWND hWnd );
	virtual void		Release();
	virtual BYTE		CheckAccount( int nType, DWORD dwKey, const char* szAccount, const char* szAddr );
	virtual BOOL		PreTranslateMessage( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam ); 
	virtual BOOL		SetConfig( BILLING_ENUM id, DWORD data );
	virtual void		OnTimer( CAccount* pAccount );
	virtual void		OnDBQuery( CQuery& query, tagDB_OVERLAPPED_PLUS* pOV ) {}
	virtual void		NotifyLogout( LPCTSTR lpszAccount, DWORD dwSession ) {}

private:
	int					m_iBillingFreePass;
	CDPBilling*			m_pDPBillings;
	int					m_nMaxConnect;
	vector< string >	m_strIPs;
	vector< u_short >	m_nPorts;

	bool				Connect();
	bool				RequestBillingInfo( LPCTSTR lpszAccount, LPCTSTR lpAddr );
	void				SendKeepAlive();
};

#endif	// __BILLINGMGR_H__