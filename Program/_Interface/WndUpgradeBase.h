#ifndef __WNDUPGRADEBASE__H
#define __WNDUPGRADEBASE__H


#define MAX_UPGRADE      6

class CWndUpgradeBase : public CWndNeuz 
{
public: 
	DWORD      m_dwReqItem[2];
	int        m_nCost;
	CItemElem* m_pItemElem[MAX_UPGRADE];
	CRect      m_Rect[MAX_UPGRADE];

	int        m_nCount[2];
	int        m_nMaxCount;
	
	CWndUpgradeBase(); 
	~CWndUpgradeBase(); 
	
	virtual BOOL    OnDropIcon( LPSHORTCUT pShortcut, CPoint point );

	virtual BOOL	Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL	OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void	OnDraw( C2DRender* p2DRender ); 
	virtual	void	OnInitialUpdate(); 
	virtual BOOL	OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void	OnSize( UINT nType, int cx, int cy ); 
	virtual void	OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void	OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual void	OnDestroyChildWnd( CWndBase* pWndChild );
	virtual	void	OnDestroy( void );
	virtual void	OnRButtonUp( UINT nFlags, CPoint point );
}; 


#endif
