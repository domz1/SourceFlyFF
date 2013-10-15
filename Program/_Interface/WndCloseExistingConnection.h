#ifndef __WNDCLOSEEXISTINGCONNECTION__H
#define __WNDCLOSEEXISTINGCONNECTION__H

class CWndCloseExistingConnection : public CWndNeuz
{
protected:
	CWndText	m_wndText;
public: 
	CWndCloseExistingConnection();
	~CWndCloseExistingConnection();

	virtual	BOOL	Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual	BOOL	OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual	void	OnDraw( C2DRender* p2DRender );
	virtual	void	OnInitialUpdate();
	virtual	BOOL	OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase );
	virtual	void	OnSize( UINT nType, int cx, int cy );
	virtual void	OnLButtonUp( UINT nFlags, CPoint point );
	virtual	void	OnLButtonDown( UINT nFlags, CPoint point );
}; 
#endif	// __WNDCLOSEEXISTINGCONNECTION__H