#ifndef __WNDGUILDMERIT__H
#define __WNDGUILDMERIT__H


class C2DRender;
class CItemElem;

class CWndGuildMeritCtrl : public CWndBase
{
	void	InterpretScript( CScanner & s, CPtrArray & ptrArray );
	CItemBase*	m_pFocusItem;
	int		m_nCurSel;
	int		m_nFontHeight;
	DWORD	m_nWndColor;
	DWORD	m_nFontColor;
	DWORD	m_nSelectColor;
	CWndScrollBar	m_wndScrollBar;
	
public:
//	Constructions
	static	CTextureMng		m_textureMng;
	CWndGuildMeritCtrl();
	~CWndGuildMeritCtrl();

	CRect	m_rect;
	BOOL	m_bDrag;
	DWORD	m_dwListCtrlStyle;
	vector<CItemBase*>		m_pItemContainer;
	
	void	Create( DWORD dwListCtrlStyle, RECT & rect, CWndBase* pParentWnd, UINT nID );

	int		HitTest( CPoint point );

//	Overridables
	int				GetTotalCount();
	void			RestoreItem();
	int				GetItemSize();
	virtual	void	SetWndRect( CRect rectWnd, BOOL bOnSize = TRUE );
	virtual	void	OnInitialUpdate( void );
	virtual	void	OnDraw( C2DRender* p2DRender );
//	virtual	void	OnLButtonUp( UINT nFlags, CPoint point );
//	virtual	void	OnLButtonDown( UINT nFlags, CPoint point );
	virtual	void	OnRButtonUp( UINT nFlags, CPoint point );
	virtual	void	OnRButtonDown( UINT nFlags, CPoint point );
	virtual	void	OnLButtonDblClk( UINT nFlags, CPoint point );
	virtual	void	OnRButtonDblClk( UINT nFlags, CPoint point );
	virtual	void	OnSize( UINT nType, int cx, int cy );
	virtual	BOOL	OnEraseBkgnd( C2DRender* p2DRender );
	virtual	void	PaintFrame( C2DRender* p2DRender );
	virtual	void	OnMouseMove( UINT nFlags, CPoint point );
	virtual	BOOL	OnDropIcon( LPSHORTCUT pShortcut, CPoint point = 0 );
//	Implementation
public:
	void ClearItem();
	void AddItem(CItemBase* pItemContainer);
	int		InsertItem( UINT nMask, int nItem, LPCTSTR lpszItem, UINT nState,
								UINT nStateMask, int nImage, LPARAM lParam );
protected:
protected:
};

class CWndGuildMerit : public CWndNeuz 
{ 
	CWndGuildMeritCtrl	m_wndctrlMerit;

	int		m_nItem;
	int		m_nPenya;	// Æä³Ä °øÇå·®.
public: 
	CWndGuildMerit(); 
	~CWndGuildMerit(); 
	
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
