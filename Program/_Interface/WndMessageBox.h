// WndArcane.h: interface for the CWndNeuz class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WNDMESSAGEBOX_H__A93F3186_63D6_43C1_956F_EC8691E0C7D9__INCLUDED_)
#define AFX_WNDMESSAGEBOX_H__A93F3186_63D6_43C1_956F_EC8691E0C7D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CWndMessageBox : public CWndNeuz
{
	CWndButton m_wndButton1;
	CWndButton m_wndButton2;
	CWndButton m_wndButton3;

protected:
	CWndText   m_wndText   ;
	UINT       m_nType     ;

public:
	CWndMessageBox();
	virtual ~CWndMessageBox();

	BOOL Create(LPCTSTR lpszMessage,UINT nType,const RECT& rect,UINT nID, CWndBase* pWndParent = NULL );
	virtual void OnEnter( UINT nChar );
	BOOL IsDisable();

	
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD dwWndId = 0 );
	virtual BOOL Initialize( LPCTSTR lpszMessage, CWndBase* pWndParent, DWORD nType = MB_OK );
	//BOOL Initialize(CString strMessage,UINT nType = MB_OK);

	// message
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
//	virtual BOOL OnCommand(UINT nID,DWORD dwMessage);
};
class CWndMessageBoxUpper : public CWndNeuz
{
protected:
	CWndText   m_wndText   ;
	UINT       m_nType     ;
	BOOL	   m_bPostLogoutMsg	;

public:
	CWndButton m_wndButton1;
	CWndButton m_wndButton2;
	CWndButton m_wndButton3;

	CWndMessageBoxUpper();
	~CWndMessageBoxUpper();
	
	BOOL Create(LPCTSTR lpszMessage,UINT nType,const RECT& rect,UINT nID, CWndBase* pWndParent = NULL );
	
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD dwWndId = 0 );
	virtual BOOL Initialize( LPCTSTR lpszMessage, CWndBase* pWndParent, DWORD nType = MB_OK, BOOL bPostLogoutMsg = FALSE );
	//BOOL Initialize(CString strMessage,UINT nType = MB_OK);
	
	// message
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	//	virtual BOOL OnCommand(UINT nID,DWORD dwMessage);
};
#define DECLARE_WNDMESSAGEBOX( class_name ) \
	class class_name : public CWndMessageBox \
	{ \
		public: class_name() { } ~class_name() { } \
		virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); \
		BOOL OnChildNotify(UINT message,UINT nID,LRESULT* pLResult); \
	}; 

#endif // !defined(AFX_WNDMESSAGEBOX_H__A93F3186_63D6_43C1_956F_EC8691E0C7D9__INCLUDED_)
