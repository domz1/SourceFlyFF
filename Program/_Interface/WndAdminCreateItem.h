#ifndef __WNDADMINCREATEITEM__H
#define __WNDADMINCREATEITEM__H

class CWndAdminCreateItem : public CWndNeuz 
{ 
public: 
	CWndAdminCreateItem(); 
	~CWndAdminCreateItem(); 

	CString MakeName( ItemProp *pProp );
		
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
