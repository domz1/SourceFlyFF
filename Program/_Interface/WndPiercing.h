#ifndef __WNDPIERCING__H
#define __WNDPIERCING__H

class CPiercingMessageBox : public CWndMessageBox
{ 
public: 
	CPiercingMessageBox() {};
	~CPiercingMessageBox() {};
	OBJID m_Objid[3];
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD dwWndId = 0 );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
}; 

class CWndPiercing : public CWndNeuz 
{
public: 
	CSfx*      m_pSfx;
	CItemElem* m_pItemElem[3];
	CRect      m_Rect[3];
	CPiercingMessageBox* m_pPiercingMessageBox;
	
	CWndPiercing(); 
	~CWndPiercing(); 
	
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
