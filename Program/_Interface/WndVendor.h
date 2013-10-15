#ifndef __WNDVENDOR__H
#define __WNDVENDOR__H

#include "wndvendorctrl.h"
#include "wndregvend.h"
#include "wndvendorbuy.h"

class CWndVendorMessage;

class CWndVendor : public CWndNeuz 
{
public:
	CWndVendorCtrl	m_wndctrlVendor;
	CWndVendorBuy*	m_pWndVendorBuy;
	CMover*	m_pVendor;
	CWndRegVend*	m_pWndRegVend;

	CWndVendorMessage* m_pwndVenderMessage;

public: 
	CWndVendor(); 
	~CWndVendor(); 

	void	SetVendor( CMover* pVendor );

	virtual BOOL	Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	void			ReloadItemList();
	virtual BOOL	OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void	OnDraw( C2DRender* p2DRender ); 
	virtual BOOL	Process();
	virtual	void	OnInitialUpdate(); 
	virtual BOOL	OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void	OnSize( UINT nType, int cx, int cy ); 
	virtual void	OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void	OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual void	OnDestroyChildWnd( CWndBase* pWndChild );
	virtual	void	OnDestroy( void );
}; 

class CWndVendorMessage : public CWndNeuz 
{ 
public: 
	void RemoveChattingMemver(LPCTSTR lpszName);
	void AddChattingMemver( LPCTSTR lpszName );
	CWndVendorMessage(); 
	~CWndVendorMessage(); 
	
	CString m_strPlayer;

#if __VER >= 11 // __MOD_VENDOR
	CWndText		m_wndChat;
	CWndText		m_wndInfo;
	BOOL			m_nIsOwner;

	void WriteBuyInfo(char *pBuyerName, CString strItem, int nItemNum, int	nTotalCost);
#endif
	
	void InitSize( void );
	void AddMessage( LPCTSTR lpszFrom, LPCTSTR lpszMessage );
	void OnInputString();
	
	virtual void SetWndRect( CRect rectWnd, BOOL bOnSize );
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 

#if __VER >= 8 // __S8_VENDOR_REVISION
class CWndVendorConfirm : public CWndMessageBox
{ 
public: 
	CString m_strVendorName;
	void	SetVendorName( CString str );
	void	SetValue( CString str );
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD dwWndId = 0 );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
}; 
#endif // __VER >= 8 // __S8_VENDOR_REVISION

#endif
