#ifndef __WNDMAP__H
#define __WNDMAP__H

class CWndMap : public CWndNeuz 
{ 
public: 
	CHAR m_szMapFile[ 64 ];
	CWndMap(); 
	virtual ~CWndMap(); 

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual void OnRButtonUp( UINT nFlags, CPoint point );
}; 
#endif
