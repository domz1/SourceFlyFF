#ifndef __WNDBAGEX__H
#define __WNDBAGEX__H

#if __VER >= 11 // __SYS_POCKET
class CWndBagEx : public CWndNeuz 
{ 
public: 

	CWndItemCtrl	m_wndItemCtrl[3];
	CRect			m_RectItemCtrl[3];
	BOOL			m_bUse[3];
	CTexture*		m_pTexNouse;
	CTexture*		m_pTexIco;
	CTexture*		m_pTexIco_empty;

	CWndBagEx(); 
	~CWndBagEx(); 

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point );
	virtual void OnMouseWndSurface( CPoint point );
	virtual BOOL process();

	void	InitItem( void );
}; 
#endif
#endif
