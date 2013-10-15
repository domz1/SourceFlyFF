// WndWebBox.h: interface for the CWndWebBox class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __WNDWEBOX__H
#define __WNDWEBOX__H


void InitWebGlobalVar();

class CWndWebBox : public CWndNeuz 
{ 
public: 
	CRect    m_rectOldBackup;
	
	CWndWebBox(); 
	~CWndWebBox(); 
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual HRESULT RestoreDeviceObjects();
	virtual BOOL Process();
//	virtual void OnDestroy( void );
};

class CWndWebBox2 : public CWndNeuz 
{ 
public: 
	CRect    m_rectOldBackup;
	
	CWndWebBox2(); 
	~CWndWebBox2(); 
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual HRESULT RestoreDeviceObjects();
//	virtual void OnDestroy( void );
};
#endif // __WNDWEBOX__H
