#ifndef __WNDVENDORCTRL_H__
#define	__WNDVENDORCTRL_H__

#pragma	once

class C2DRender;
class CItemElem;

class CWndVendorCtrl : public CWndBase
{
	CTexture*		   m_pTex;
	void	InterpretScript( CScanner & s, CPtrArray & ptrArray );
	CItemBase*	m_pFocusItem;
	int		m_nCurSel;
	int		m_nFontHeight;
	DWORD	m_nWndColor;
	DWORD	m_nFontColor;
	DWORD	m_nSelectColor;
//	CPtrArray	m_aItems;
//	CPtrArray	m_aColumns;

public:
//	Constructions
	static	CTextureMng		m_textureMng;
	CWndVendorCtrl();
	~CWndVendorCtrl()	{}

	CRect	m_rect;
	BOOL	m_bDrag;
	DWORD	m_dwListCtrlStyle;
	CMover*		m_pMover;
	
	void	InitVendor( CMover* pMover );
	void	Create( DWORD dwListCtrlStyle, RECT & rect, CWndBase* pParentWnd, UINT nID );
//	void	LoadListBoxScript( LPCTSTR lpFileName );

	int		HitTest( CPoint point );
	CItemBase* GetItem( BYTE i );
#if __VER >= 8 // __S8_VENDOR_REVISION
	void	OnDrawItemInfo( C2DRender* p2DRender, CItemElem* pItemElem, int nX, int nY );
	CString GetNumberFormatSelling( LPCTSTR szNumber );
#endif // __VER >= 8 // __S8_VENDOR_REVISION

//	Attributes
//	COLORREF	GetBkColor()	const;
//	BOOL	SetBkColor( COLORREF cr );
//	int		GetItemCount()	const;
//	BOOL	GetItem( LVITEM* pItem )	const;
//	BOOL	SetItem( const LVITEM* pItem );
//	BOOL	SetItem( int nItem, int nSubItem, UINT nMask, LPCTSTR lpszItem,
//								int nImage, UINT nState, UINT nStateMask, LPARAM lParam );
//	UINT	GetCallbackMask()	const;
//	BOOL	SetCallbackMask( UINT nMask );
//	int		GetNextItem( int nItem, int nFlags )	const;
//	POSITION	GetFirstSelectedItemPosition()	const;
//	int		GetNextSelectedItem( POSITION & pos )	const;
//	BOOL	GetItemRect( int nItem, LPRECT lpRect, UINT nCode )	const;
//	BOOL	SetItemPosition( int nItem, POINT point );
//	BOOL	GetItemPosition( int nItem, LPPOINT lpPoint )	const;
//	int		GetStringWidth( LPCTSTR lpsz )	const;
//	CWndEdit*	GetEditControl()	const;
//	BOOL	GetColumn( int nCol, LVCOLUMN* pColumn )	const;
//	BOOL	SetColumn( int nCol, const LVCOLUMN* pColumn );
//	int		GetColumnWidth( int nCol )	const;
//	BOOL	SetColumnWidth( int nCol, int cx );
//	BOOL	GetViewRect( LPRECT lpRect )	const;
//	COLORREF	GetTextColor( void )	const;
//	BOOL	SetTextColor( COLORREF cr );
//	COLORREF	GetTextBkColor( void )	const;
//	BOOL	SetTextBkColor( COLORREF cr );
//	int		GetTopIndex( void )	const;
//	int		GetCountPerPage( void )		const;
//	BOOL	GetOrigin( LPPOINT lpPoint )	const;
//	BOOL	SetItemState( int nItem, LVITEM* pItem );
//	BOOL	SetItemState( int nItem, UINT nState, UINT nMask );
//	UINT	GetItemState( int nItem, UINT nMask )	const;
//	CString	GetItemText( int nItem, int nSubItem )	const;
//	int		GetItemText( int nItem, int nSubItem, LPTSTR lpszText, int nLen )	const;
//	BOOL	SetItemText( int nItem, int nSubItem, LPCTSTR lpszText );
//	void	SetItemCount( int nItems );
//	BOOL	SetItemData( int nItem, DWORD dwData );
//	DWORD	GetItemData( int nItem )	const;
//	UINT	GetSelectedCount( void )	const;
//	BOOL	SetColumnOrderArray( int iCount, LPINT piArray );
//	BOOL	GetColumnOrderArray( LPINT piArray, int iCount = -1 );
//	CSize	SetIconSpacing( CSize size );
//	CSize	SetIconSpacing( int cx, int cy );
//	BOOL	GetSubItemRect( int iItem, int iSubItem, int nArea, CRect & ref );
//	int		GetHotItem( void );
//	int		SetHotItem( int iIndex );
//	int		GetSelectionMark( void );
//	int		SetSelectionMark( int iIndex );
//	DWORD	GetExtendedStyle( void );
//	DWORD	SetExtendedStyle( DWORD dwNewStyle );
//	int		SubItemHitTest( LPLVHITTESTINFO pInfo );
//	void	SetWorkAreas( int nWorkAreas, LPRECT lpRect );
//	BOOL	SetItemCountEx( int iCount, DWORD dwFlags = LVSICF_NOINVALIDATEALL );
//	CSize	ApproximateViewRect( CSize sz	= CSize( -1, -1 ), int iCount = -1 )	const;
//	BOOL	GetBkImage( LVBKIMAGE* plvbkImage )		const;
//	DWORD	GetHoverTime( void )	const;
//	void	GetWorkAreas( int nWorkAreas, LPRECT prc )	const;
//	BOOL	SetBkImage( HBITMAP hbm, BOOL fTile = TRUE, int xOffsetPercent = 0, int yOffsetPercent = 0 );
//	BOOL	SetBkImage( LPTSTR pszUrl, BOOL fTile = TRUE, int xOffsetPercent = 0, int yOffsetPercent = 0 );
//	BOOL	SetBkImage( LVBKIMAGE* plvbkImage );
//	DWORD	SetHoverTime( DWORD dwHoverTime = (DWORD)-1 );
//	UINT	GetNumberOfWorkAreas( void )	const;
//	BOOL	GetCheck( int nItem )	const;
//	BOOL	SetCheck( int nItem, BOOL fCheck = TRUE );
//	Operations
//	int		InsertItem( const LVITEM* pItem );
//	int		InsertItem( int nItem, LPCTSTR lpszItem );
//	int		InsertItem( int nItem, LPCTSTR lpszItem, int nImage );
//	BOOL	DeleteItem( int nItem );
//	BOOL	DeleteAllItems( void );
//	BOOL	EnsureVisible( int nItem, BOOL bPartialOK );
//	BOOL	Scroll( CSize size );
//	BOOL	RedrawItems( int nFirst, int nLast );
//	BOOL	Arrange( UINT nCode );
//	CEdit*	EditLabel( int nItem );
//	int		InsertColumn( int nCol, const LVCOLUMN* pColumn );
//	int		InsertColumn( int nCol, LPCTSTR lpszColumnHeading,
//									int nFormat = LVCFMT_LEFT, int nWidth = -1, int nSubItem = -1 );
//	BOOL	DeleteColumn( int nCol );
//	BOOL	Update( int nItem );
//	Overridables
	virtual	void	SetWndRect( CRect rectWnd, BOOL bOnSize = TRUE );
	virtual	void	OnInitialUpdate( void );
	virtual	void	OnDraw( C2DRender* p2DRender );
	virtual	void	OnLButtonUp( UINT nFlags, CPoint point );
	virtual	void	OnLButtonDown( UINT nFlags, CPoint point );
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
	int		InsertItem( UINT nMask, int nItem, LPCTSTR lpszItem, UINT nState,
								UINT nStateMask, int nImage, LPARAM lParam );
protected:
//	void	RemoveImageList( int nImageList );
protected:
};

#endif	// __WNDVENDORCTRL_H__