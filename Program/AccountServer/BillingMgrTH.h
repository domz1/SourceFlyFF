#ifndef __BILLINGMGRTH_H__
#define	__BILLINGMGRTH_H__

#include "BillingMgr.h"


class CDPBillingTH;

class CBillingMgrTH : public CBillingMgr
{
public:
	CBillingMgrTH();
	virtual ~CBillingMgrTH();

	virtual bool		Init( HWND hWnd );
	virtual void		Release();
	virtual BYTE		CheckAccount( int nType, DWORD dwKey, const char* szAccount, const char* szAddr );
	virtual BOOL		PreTranslateMessage( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam ); 
	virtual BOOL		SetConfig( BILLING_ENUM id, DWORD data );
	virtual void		OnTimer( CAccount* pAccount );
	virtual void		OnDBQuery( CQuery& query, tagDB_OVERLAPPED_PLUS* pOV ) {}
	virtual void		NotifyLogout( LPCTSTR lpszAccount, DWORD dwSession );
	
private:
	CDPBillingTH*		m_pDPBillings;
	int					m_nMaxConnect;
	vector< string >	m_strIPs;
	vector< u_short >	m_nPorts;

	bool				Connect();
	bool				RequestBillingInfo( LPCTSTR lpszAccount, LPCTSTR lpAddr, DWORD dwSession );
	void				SendKeepAlive();
	CDPBillingTH*		GetBilling( DWORD dwSession );

};

#endif	// __BILLINGMGR_H__