#ifndef __WNDCOMMAND__H
#define __WNDCOMMAND__H

class CWndCommand : public CWndListBox //CWndNeuz 
{ 
public: 
	CWndBase* m_pWndEdit;

	CWndCommand(); 
	~CWndCommand(); 

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point);
	virtual void OnSetFocus( CWndBase* pOldWnd );
	virtual void OnKillFocus(CWndBase* pNewWnd);
}; 
#endif
