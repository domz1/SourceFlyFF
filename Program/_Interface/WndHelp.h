#ifndef __WNDHELP_H
#define __WNDHELP_H

//////////////////////////////////////////////////////////////////////////////////////
// µµ¿ò¸»
//
class CWndHelp : public CWndNeuz
{
public:
	//CWndText      m_wndText;
	CString       m_strKeyword;
	//CWndTreeCtrl  m_wndViewCtrl;
	//CStringArray  m_strArray;

	CWndHelp();
	virtual ~CWndHelp();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL,DWORD dwWndId = 0);
	// message
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL );
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnChildNotify(UINT message,UINT nID,LRESULT* pLResult);
};
class CWndHelpFAQ : public CWndNeuz
{
public:
	CMapStringToString m_mapFAQ;

	//CWndText      m_wndText;
	CString       m_strKeyword;
	//CWndTreeCtrl  m_wndViewCtrl;
	//CStringArray  m_strArray;
	BOOL LoadFAQ( LPCTSTR lpszFileName );

	CWndHelpFAQ();
	virtual ~CWndHelpFAQ();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL,DWORD dwWndId = 0);
	// message
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL );
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnChildNotify(UINT message,UINT nID,LRESULT* pLResult);
};
class CWndHelpTip : public CWndNeuz 
{ 
	CStringArray m_aString;
	int m_nPosString;
public: 
	CWndHelpTip(); 
	~CWndHelpTip(); 

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 


class CWndHelpInstant : public CWndNeuz 
{ 
public: 
	CString m_strHelpKey;
	CWndHelpInstant(); 
	~CWndHelpInstant(); 

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 
#endif
