// WndBase.h: interface for the CWndBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WNDCONTROL_H__0B45596D_70D7_48A4_BCB2_3D0F32F58E57__INCLUDED_)
#define AFX_WNDCONTROL_H__0B45596D_70D7_48A4_BCB2_3D0F32F58E57__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class C2DRender;

// Notification code
#define WNM_ERRSPACE            (-2)
#define WNM_SELCHANGE           1
#define WNM_DBLCLK              2
#define WNM_SELCANCEL           3
#define WNM_SETFOCUS            4 
#define WNM_KILLFOCUS           5
#define WNM_CLICKED             6   
///////
#define WNM_ITEMCHANGING        10
#define WNM_ITEMCHANGED         11
#define WNM_INSERTITEM          12
#define WNM_DELETEITEM          13
#define WNM_DELETEALLITEMS      14
#define WNM_BEGINLABELEDITA     15
#define WNM_BEGINLABELEDITW     16
#define WNM_ENDLABELEDITA       17
#define WNM_ENDLABELEDITW       18
#define WNM_COLUMNCLICK         19
#define WNM_BEGINDRAG           20
#define WNM_BEGINRDRAG          21
///////
//#define WNM_ITEMCHANGING        30
//#define WNM_ITEMCHANGED         31 
#define WNM_ITEMCLICK           32
#define WNM_ITEMDBLCLICK        33
#define WNM_DIVIDERDBLCLICK     34
#define WNM_BEGINTRACK          35
#define WNM_ENDTRACK            36
#define WNM_TRACK               37

//  WndItemCtrl Norification Message
#define WIN_DBLCLK              40
#define WIN_ITEMDROP            41   


//NM_CLICK
//WNM_BEGINRDRAG
//HDN_TRACK

/**
 윈도 매시지 박스에서 사용하는 버튼 종류 
 MB_OK                 
 MB_OKCANCEL           
 MB_ABORTRETRYIGNORE   
 MB_YESNOCANCEL        
 MB_YESNO              
 MB_RETRYCANCEL        
**/
// 이것은 추가된 것
#define MB_CANCEL           0x00000006L

//////////////////////////////////////////////////////////////////////////////
// CWndButton 
//////////////////////////////////////////////////////////////////////////////


class CWndMenu;

class CWndStatic : public CWndBase
{
public:
	CWndStatic();// { m_byWndType = WTYPE_STATIC; m_dwColor = 0xff646464; }
	~CWndStatic() { }
	virtual BOOL IsPickupSpace(CPoint point); 
	void OnDraw( C2DRender* p2DRender );
	void SetWndRect( CRect rectWnd, BOOL bOnSize );
	BOOL Create( LPCTSTR lpszCaption, DWORD dwStyle, const RECT& rect, CWndBase* pParentWnd, UINT nID );
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual	void PaintFrame( C2DRender* p2DRender );
};

class CWndCustom : public CWndBase
{
public:
	CWndCustom() { m_byWndType = WTYPE_CUSTOM; }
	~CWndCustom() { }

	void OnDraw( C2DRender* p2DRender );
	BOOL Create( LPCTSTR lpszCaption, DWORD dwStyle, const RECT& rect, CWndBase* pParentWnd, UINT nID )	{
		if( lpszCaption && strlen( lpszCaption ) ) m_strTitle = lpszCaption;
		return CWndBase::Create( dwStyle | WBS_CHILD | WBS_NOFRAME | WBS_NODRAWFRAME, rect,  pParentWnd, nID );
	}
};
class CWndButton : public CWndBase
{
protected:
	DWORD     m_nFontColor;
	DWORD     m_nPushColor;
	DWORD     m_nHighlightColor;
	DWORD     m_nDisableColor;

	BOOL      m_bCheck;
	int       m_nTimePush;
	BOOL      m_bHighLight;
	CPoint    m_ptPush;
	CWndBase* m_pWndExecute; // 이 버튼을 누르므로서 실행될 윈도. 
//	CTimer m_timerPush;

	//DWORD     m_dwButtonStyle;

public:
	CHAR      m_cHotkey;

	CWndMenu*  m_pWndMenu;
	BOOL       m_bTopDown;
	SHORTCUT   m_shortcut;
		/*
	DWORD     m_dwShortCutType    ;
	DWORD     m_dwShortCutIndex   ;
	DWORD     m_dwShortCutUserId  ;
	DWORD     m_dwShortCutData    ;
	CString   m_strShortCutString ;
//WORD     m_dwShortCutIndex;
*/
	CWndButton();
	~CWndButton();

	//BOOL Create(LPCTSTR lpszCaption,DWORD dwStyle,const RECT& rect,CWndBase* pParentWnd,UINT nID,CSprPack* pSprPack,int nSprIdx,int nColorTable = 0);
	BOOL Create(LPCTSTR lpszCaption,DWORD dwStyle,const RECT& rect,CWndBase* pParentWnd,UINT nID);// { return Create(lpszCaption,dwStyle,rect,pParentWnd,nID,NULL,0); }
	virtual void OnDraw( C2DRender* p2DRender );
	virtual void PaintFrame( C2DRender* p2DRender );
	virtual	BOOL Process();

	void SetMenu( CWndMenu* pWndMenu );

	void FitTextureSize();

	// Attributes
	//BOOL IsButtonStyle( DWORD dwStyle ) { return ( m_dwButtonStyle & dwStyle ) ? TRUE : FALSE; }
	void SetPushTime(int nTime); // 버튼을 누르고 nTime이후에 계속 OnCommand를 패어런트에게 보낸다.
	UINT GetState() const; // Retrieves the check state, highlight state, and focus state of a button control.
	void SetState(BOOL bHighlight); // Sets the highlighting state of a button control
	int  GetCheck() const; // Retrieves the check state of a button control.
	void SetCheck(int nCheck); // Sets the check state of a button control.
	//UINT GetButtonStyle() const; // Retrieves information about the button control style.
	//void SetButtonStyle(UINT nStyle, BOOL bRedraw = TRUE); // Changes the style of a button.
	void SetString( CString strSndEffect ); 
	BOOL IsHighLight() { return m_bHighLight; }
	void SetFontColor     (DWORD dwColor) { m_nFontColor      = (DWORD)dwColor; }
	void SetPushColor     (DWORD dwColor) { m_nPushColor      = (DWORD)dwColor; }
	void SetDisableColor  (DWORD dwColor) { m_nDisableColor   = (DWORD)dwColor; }
	void SetHighLightColor(DWORD dwColor) { m_nHighlightColor = (DWORD)dwColor; }
	DWORD GetFontColor     () { return m_nFontColor     ; }
	DWORD GetPushColor     () { return m_nPushColor     ; }
	DWORD GetDisableColor  () { return m_nDisableColor  ; }
	DWORD GetHighLightColor() { return m_nHighlightColor; }

	void SetWndExecute(CWndBase* pWndBase);
	CWndBase* GetWndExecute() { return m_pWndExecute; }
	void SetPushPoint(int x,int y) { m_ptPush = CPoint(x,y); }
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point = 0 );
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnRButtonUp(UINT nFlags, CPoint point);
	virtual void OnRButtonDown(UINT nFlags, CPoint point);
	virtual void OnLButtonDblClk(UINT nFlags, CPoint point);
	virtual void OnRButtonDblClk(UINT nFlags, CPoint point);
	virtual void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void OnMouseMove(UINT nFlags, CPoint point);
	virtual LRESULT DefWindowProc(UINT message,WPARAM wParam,LPARAM lParam);
};

//////////////////////////////////////////////////////////////////////////////
// CWndScrollBar
//////////////////////////////////////////////////////////////////////////////

class CWndScrollBar : public CWndBase
{
protected:
	int        m_nScrollPos;
	int        m_nScrollMinPos;
	int        m_nScrollMaxPos;
	int        m_nScrollBarIdx;
	int        m_nScrollPage;
	int        m_nPos;
	BOOL       m_bPushPad; 
	CRect      m_rectStick;
	CWndButton m_wndArrow1;
	CWndButton m_wndArrow2;

public:
	CTexture* m_pTexButtVScrUp   ;
	CTexture* m_pTexButtVScrDown ;
	CTexture* m_pTexButtVScrBar  ;
	CTexture* m_pTexButtVScrPUp  ;
	CTexture* m_pTexButtVScrPDown;
	CTexture* m_pTexButtVScrPBar ;

//static CTexturePack m_texturePack;
	
	CWndScrollBar();
	~CWndScrollBar();

	BOOL Create(DWORD dwScrollStyle,const RECT& rect,CWndBase* pParentWnd,UINT nID);//,CSprPack* pSprPack,int nSprIdx);

	int  GetMinScrollPos();
	int  GetMaxScrollPos();

	void SetMinScrollPos();
	void SetMaxScrollPos();
	int  GetScrollPos() const; // Retrieves the current position of a scroll box. 
	int  SetScrollPos(int nPos, BOOL bRedraw = TRUE); // Sets the current position of a scroll box. 
	void GetScrollRange(LPINT lpMinPos, LPINT lpMaxPos) const; // Retrieves the current minimum and maximum scroll-bar positions for the given scroll bar. 
	int  GetScrollRange() const;
	void SetScrollRange(int nMinPos, int nMaxPos, BOOL bRedraw = TRUE); // Sets minimum and maximum position values for the given scroll bar. 
	void SetScrollPage(int nPage);
	int  GetScrollPage() const;
	int  GetScrollAbiliableRange();
	int  GetScrollAbiliablePos();
	int  GetScrollAbiliablePage(); 
	void ShowScrollBar(BOOL bShow = TRUE); // Shows or hides a scroll bar. 
	BOOL EnableScrollBar(UINT nArrowFlags = ESB_ENABLE_BOTH); // Enables or disables one or both arrows of a scroll bar. 
	BOOL SetScrollInfo(LPSCROLLINFO lpScrollInfo, BOOL bRedraw = TRUE); // Sets information about the scroll bar. 
	BOOL GetScrollInfo(LPSCROLLINFO lpScrollInfo, UINT nMask = SIF_ALL); // Retrieves information about the scroll bar. 
	int  GetScrollLimit(); // Retrieves the limit of the scroll bar
	void SetScrollDown();
/*
	CSprite* GetSprVertArrow1();
	CSprite* GetSprVertArrow2();
	CSprite* GetSprVertPad();
	CSprite* GetSprVertBar();
	CSprite* GetSprHoriArrow1();
	CSprite* GetSprHoriArrow2();
	CSprite* GetSprHoriPad();
	CSprite* GetSprHoriBar();
*/
	virtual void SetWndRect(CRect rectWnd, BOOL bOnSize = TRUE);
	virtual	void PaintFrame(C2DRender* p2DRender);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point);
	virtual void OnRButtonUp(UINT nFlags, CPoint point);
	virtual void OnRButtonDown(UINT nFlags, CPoint point);
	virtual void OnMouseMove(UINT nFlags, CPoint point);
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL );
	virtual void OnSize(UINT nType, int cx, int cy);
};

//////////////////////////////////////////////////////////////////////////////
// CWndMenu

class CWndMenu : public CWndBase
{
	BOOL IsOnMenu(CPoint pt);
	int m_nLargeWidth;
	CTexture	m_TOOLTIPMEU[ MAX_TT * 9 ];

public:
	CPtrArray  m_awndMenuItem;
	bool	m_bUsesSkin;
	//	CWndButton m_pWndCommand[18];
	
	// Constructors
	CWndMenu();
	~CWndMenu();
	
	CWndButton* GetMenuItem( int nPos );
	void SetVisibleSub( BOOL bVisible );
	void SetVisibleAllMenu( BOOL bVisible );
	
	BOOL CreateMenu(CWndBase* pWndParent);
	BOOL CreatePopupMenu();
	BOOL LoadMenu(LPCTSTR lpszResourceName);
	BOOL LoadMenu(UINT nIDResource);
	BOOL LoadMenuIndirect(const void* lpMenuTemplate);
	BOOL DestroyMenu();
	virtual void OnKillFocus(CWndBase* pNewWnd);
	virtual void PaintFrame(C2DRender* p2DRender);
	virtual BOOL OnEraseBkgnd(C2DRender* p2DRender);
	virtual void SetWndRect( CRect rectWnd, BOOL bOnSize = TRUE );
	// Attributes
	//HMENU m_hMenu;          // must be first data member
	//HMENU GetSafeHmenu() const;
	//operator HMENU() const;
	
	//static CMenu* PASCAL FromHandle(HMENU hMenu);
	//static void PASCAL DeleteTempMap();
	//BOOL Attach(HMENU hMenu);
	//HMENU Detach();
	
	// CMenu Operations
	void DeleteAllMenu();
	BOOL DeleteMenu(UINT nPosition, UINT nFlags);
	//BOOL TrackPopupMenu(UINT nFlags, int x, int y,CWnd* pWnd, LPCRECT lpRect = 0);
	//BOOL operator==(const CMenu& menu) const;
	//BOOL operator!=(const CMenu& menu) const;
	
	// CMenuItem Operations
	virtual CWndButton* AppendMenu(UINT nFlags, UINT nIDNewItem = 0,	LPCTSTR lpszNewItem = NULL);
	//BOOL AppendMenu(UINT nFlags, UINT nIDNewItem, const CBitmap* pBmp);
	UINT CheckMenuItem(UINT nIDCheckItem, UINT nCheck);
	UINT EnableMenuItem(UINT nIDEnableItem, UINT nEnable);
	UINT GetMenuItemCount() const;
	UINT GetMenuItemID(int nPos) const;
	UINT GetMenuState(UINT nID, UINT nFlags) const;
	int  GetMenuString(UINT nIDItem, LPTSTR lpString, int nMaxCount,	UINT nFlags) const;
	int  GetMenuString(UINT nIDItem, CString& rString, UINT nFlags) const;
	//BOOL GetMenuItemInfo(UINT nIDItem, LPMENUITEMINFO lpMenuItemInfo,	BOOL fByPos = FALSE);
	CWndMenu* GetSubMenu(int nPos) const;
	BOOL InsertMenu(UINT nPosition, UINT nFlags, UINT nIDNewItem = 0,	LPCTSTR lpszNewItem = NULL);
	//BOOL InsertMenu(UINT nPosition, UINT nFlags, UINT nIDNewItem,	const CBitmap* pBmp);
	BOOL ModifyMenu(UINT nPosition, UINT nFlags, UINT nIDNewItem = 0,	LPCTSTR lpszNewItem = NULL);
	//BOOL ModifyMenu(UINT nPosition, UINT nFlags, UINT nIDNewItem,	const CBitmap* pBmp);
	BOOL RemoveMenu(UINT nPosition, UINT nFlags);
	//BOOL SetMenuItemBitmaps(UINT nPosition, UINT nFlags, const CBitmap* pBmpUnchecked, const CBitmap* pBmpChecked);
	BOOL CheckMenuRadioItem(UINT nIDFirst, UINT nIDLast, UINT nIDItem, UINT nFlags);
	BOOL SetDefaultItem(UINT uItem, BOOL fByPos = FALSE);
	UINT GetDefaultItem(UINT gmdiFlags, BOOL fByPos = FALSE);
	void SetLargeWidth( int nLargeWidth ) { m_nLargeWidth = nLargeWidth; }
	
	// Context Help Functions
	//BOOL SetMenuContextHelpId(DWORD dwContextHelpId);
	//DWORD GetMenuContextHelpId() const;
	
	// Overridables (must override draw and measure for owner-draw menu items)
	//virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual BOOL OnChildNotify(UINT message,UINT nID,LRESULT* pLResult);
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL );
};
//////////////////////////////////////////////////////////////////////////////
// CWndText
//////////////////////////////////////////////////////////////////////////////
#define EBS_LEFT             0x00000000L
#define EBS_CENTER           0x00000001L
#define EBS_RIGHT            0x00000002L
#define EBS_MULTILINE        0x00000004L
#define EBS_UPPERCASE        0x00000008L
#define EBS_LOWERCASE        0x00000010L
#define EBS_PASSWORD         0x00000020L
#define EBS_AUTOVSCROLL      0x00000040L
#define EBS_AUTOHSCROLL      0x00000080L
#define EBS_NOHIDESEL        0x00000100L
#define EBS_OEMCONVERT       0x00000400L
#define EBS_READONLY         0x00000800L
#define EBS_WANTRETURN       0x00001000L
#define EBS_NUMBER           0x00002000L

class CWndText : public CWndBase
{
protected:
	CTimer m_timerCaret;
	BOOL   m_bCaretVisible;
	CPoint m_ptCaret;
	BOOL   m_bLButtonDown;

	DWORD m_dwBlockBegin;
	DWORD m_dwBlockEnd;
	DWORD m_dwOffset;
	
	BOOL	m_bScr;
	int		m_nLineRefresh;

public:
	void Delete( int nIndex, int nLen );
	DWORD GetBlockBegine() { return m_dwBlockBegin; }
	DWORD GetBlockEnd() { return m_dwBlockEnd; }
	
	void BlockDelete( LPCTSTR pstr, LPCTSTR pstr2 );
	void BlockInsert( LPCTSTR pstr, LPCTSTR pstr2 );
	void Insert( int nIndex, LPCTSTR pstr );
	void BlockSetStyle( DWORD dwStyle );
	void BlockClearStyle( DWORD dwStyle );
	void BlockSetColor( DWORD dwColor );
	CWndMenu m_wndMenu;
	
	TCHAR m_TextComp[3];
	TCHAR m_szCaret[ 3 ];
	BOOL m_bEnableClipboard;
	//CTexture* m_apTexture[ 9 ];
	int m_nLineSpace; 
	CWndScrollBar m_wndScrollBar;
	CEditString m_string;

	CWndText();
	~CWndText();
	BOOL Create( DWORD dwTextStyle, const RECT& rect, CWndBase* pParentWnd, UINT nID );
	CPoint m_ptDeflateRect;

	virtual	void OnInitialUpdate();
	virtual void SetWndRect( CRect rectWnd, BOOL bOnSize = TRUE );
	virtual	void PaintFrame( C2DRender* p2DRender );
	virtual void OnDraw( C2DRender* p2DRender );
	virtual void OnLButtonDown( UINT nFlags, CPoint point );
	virtual void OnLButtonUp( UINT nFlags, CPoint point );
	virtual void OnRButtonUp( UINT nFlags, CPoint point );
	virtual BOOL OnCommand ( UINT nID, DWORD dwMessage, CWndBase* pWndBase );
	virtual BOOL OnChildNotify ( UINT nCode, UINT nID, LRESULT* pLResult );
	virtual void OnMouseMove( UINT nFlags, CPoint point );
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point );
	virtual void OnSetFocus( CWndBase* pOldWnd );
	virtual void OnKillFocus( CWndBase* pNewWnd );
	virtual void OnSize( UINT nType, int cx, int cy );
	virtual void OnChar( UINT nChar );
	virtual BOOL OnMouseWheel( UINT nFlags, short zDelta, CPoint pt );

	int GetFontHeight() { return m_pFont->GetMaxHeight() + m_nLineSpace; }

	LONG GetOffset( CPoint point );
	void UpdateScrollBar();
	BOOL IsEmptyBlock() { return m_dwBlockBegin == m_dwBlockEnd; }
	CPoint OffsetToPoint( DWORD dwOffset, TCHAR* pszStr );
	CPoint GetCaretPos() { return m_ptCaret; }
virtual void DrawCaret( C2DRender* p2DRender );
	void SetCaretPos( CPoint ptCaret ) { m_ptCaret = ptCaret; m_timerCaret.Reset(); }
	void HideCaret() { m_bCaretVisible = FALSE; }
	void ShowCaret() { m_bCaretVisible = TRUE; }

	void SetString( LPCTSTR pszString, DWORD dwColor = 0xffffffff );//ADEILSON 0xff000000
	void AddString( LPCTSTR pszString, DWORD dwColor = 0xffffffff, DWORD dwPStyle = 0x00000001 );//ADEILSON 0xff000000
	void ResetString();
};
//////////////////////////////////////////////////////////////////////////////
// CWndStatic
//////////////////////////////////////////////////////////////////////////////
/*
class CWndStatic : public CWndBase
{
public:

	CWndStatic();
	~CWndStatic();
	BOOL Create(DWORD dwTextStyle,const RECT& rect,CWndBase* pParentWnd,UINT nID);

	virtual	void OnInitialUpdate();
	virtual void SetWndRect(CRect rectWnd, BOOL bOnSize = TRUE);
	virtual	void PaintFrame(C2DRender* p2DRender);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnMouseMove(UINT nFlags, CPoint point);
	virtual void OnLButtonDblClk(UINT nFlags, CPoint point);
	virtual void OnSetFocus(CWndBase* pOldWnd);
	virtual void OnKillFocus(CWndBase* pNewWnd);
	virtual void OnSize(UINT nType, int cx, int cy);

	LONG GetOffset(CPoint point);
	void UpdateScrollBar();

	CPoint OffsetToPoint(DWORD dwOffset);
	CPoint GetCaretPos() { return m_ptCaret; }
	void DrawCaret(C2DRender* p2DRender);
	void SetCaretPos(CPoint ptCaret) { m_ptCaret = ptCaret; }
	void HideCaret() { m_bCaretVisible = FALSE; }
	void ShowCaret() { m_bCaretVisible = TRUE; }

	void SetString(LPCTSTR pszString);
	void AddString(LPCTSTR pszString);
	void ResetString();
};
*/
//////////////////////////////////////////////////////////////////////////////
// CWndViewCtrl
//////////////////////////////////////////////////////////////////////////////

typedef struct tagScriptElem
{
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	tagScriptElem( void );
	tagScriptElem( CRect* pRect );
	tagScriptElem* m_lpParent;
	DWORD m_dwColor;
	DWORD m_dwSelectColor;
	CEditString m_strKeyword;
	DWORD m_dwData;
	CPtrArray m_ptrArray;
	BOOL m_bOpen;
	CWndButton* m_pWndCheckBox;
#else // __IMPROVE_QUEST_INTERFACE
	tagScriptElem* m_lpParent;
	DWORD m_dwColor;
	CString   m_strKeyword;
	DWORD     m_dwData;
	CPtrArray m_ptrArray;
	BOOL      m_bOpen;
#endif // __IMPROVE_QUEST_INTERFACE
} 
TREEELEM,* LPTREEELEM;

class CWndTreeCtrl : public CWndBase
{
public:							//sun!!
	typedef struct tagITEM
	{
		CRect      m_rect;
		BOOL       m_bButton;
		LPTREEELEM m_lpTreeElem;
	} TREEITEM,* LPTREEITEM;

private:
	CPtrArray m_treeItemArray;
	void InterpriteScript( CScript& script, CPtrArray& ptrArray ); 
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	virtual void PaintTree( C2DRender* p2DRender, CPoint& pt, CPtrArray& ptrArray );
#else // __IMPROVE_QUEST_INTERFACE
	void PaintTree( C2DRender* p2DRender, CPoint& pt, CPtrArray& ptrArray );
#endif // __IMPROVE_QUEST_INTERFACE
	LPTREEELEM m_pFocusElem;
	int  m_nFontHeight ;
	DWORD m_nWndColor   ;
	TREEELEM m_treeElem;
	CWndScrollBar m_wndScrollBar;
	void FreeTree( CPtrArray& ptrArray );
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
public:
	enum { CHECK_BOX_SIZE_XY = 13 };
private:
	BOOL m_bMemberCheckingMode;
	int m_nMaxCheckNumber;
	int m_nTreeTabWidth;
	int m_nCategoryTextSpace;
	int m_nTreeItemsMaxWidth;
#endif // __IMPROVE_QUEST_INTERFACE
public:
	int   m_nLineSpace  ;
	CTexture* m_pTexButtOpen;
	CTexture* m_pTexButtClose;
	DWORD m_nFontColor  ; 
	DWORD m_nSelectColor;

	CWndTreeCtrl();
	~CWndTreeCtrl();
	int GetFontHeight() { return m_pFont->GetMaxHeight() + m_nLineSpace; }
	BOOL DeleteAllItems();
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	void DeleteItemArray( void );
#endif // __IMPROVE_QUEST_INTERFACE
	LPTREEELEM GetCurSel();
	LPTREEELEM GetRootElem();
	LPTREEELEM GetNextElem( LPTREEELEM pElem, int& nPos );
	LPTREEELEM FindTreeElem( DWORD dwData );
	LPTREEELEM FindTreeElem( CPtrArray& ptrArray, DWORD dwData );
	LPTREEELEM SetCurSel( DWORD dwData );
	LPTREEELEM SetCurSel( LPCTSTR lpszKeyword );
	LPTREEELEM FindTreeElem( LPCTSTR lpszKeyword );
	LPTREEELEM FindTreeElem( CPtrArray& ptrArray, LPCTSTR lpszKeyword );
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	LPTREEELEM InsertItem( LPTREEELEM lpParent, 
						   LPCTSTR lpString, 
						   DWORD dwData, 
						   BOOL bForbidChecking = TRUE, 
						   BOOL bCheck = FALSE, 
						   DWORD dwColor = D3DCOLOR_ARGB( 255, 255, 255, 255 ), /*adeilson*/
						   DWORD dwSelectColor = D3DCOLOR_ARGB( 255, 0, 255, 255 ) );/*adeilson*/
#else // __IMPROVE_QUEST_INTERFACE
	LPTREEELEM InsertItem( LPTREEELEM lpParent, LPCTSTR lpString, DWORD dwData );
#endif // __IMPROVE_QUEST_INTERFACE
	void LoadTreeScript( LPCTSTR lpFileName ); 
	BOOL Create(DWORD dwTextStyle,const RECT& rect,CWndBase* pParentWnd,UINT nID);
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	BOOL CheckParentTreeBeOpened( LPTREEELEM lpTreeElem );
	CPtrArray* GetTreeItemArray( void );
	void SetFocusElem( const LPTREEELEM pFocusElem );
	const LPTREEELEM GetFocusElem( void ) const;
	void SetLineSpace( int nLineSpace );
	int GetLineSpace( void ) const;
	void SetMemberCheckingMode( BOOL bMemberCheckingMode );
	BOOL GetMemberCheckingMode( void ) const;
	void SetMaxCheckNumber( int nMaxCheckNumber );
	int GetMaxCheckNumber( void ) const;
	void SetTreeTabWidth( int nTreeTabWidth );
	int GetTreeTabWidth( void ) const;
	void SetCategoryTextSpace( int nCategoryTextSpace );
	int GetCategoryTextSpace( void ) const;
	int GetTreeItemsNumber( void ) const;
	void SetTreeItemsMaxWidth( int nTreeItemsMaxWidth );
	int GetTreeItemsMaxWidth( void ) const;
	int GetTreeItemsMaxHeight( void );
	void SetTextColor( DWORD dwCategoryTextColor, DWORD dwNormalTextColor, DWORD dwSelectedCategoryTextColor, DWORD dwSelectedNormalTextColor );

private:
	void CalculateTreeItemsNumber( int& nSumTreeItemsNumber, const CPtrArray& rPtrArray ) const;
	void CalculateTreeItemsMaxHeight( int& nSumHeight, const CPtrArray& rPtrArray );
	void CalculateTextColor( DWORD dwCategoryTextColor, DWORD dwNormalTextColor, DWORD dwSelectedCategoryTextColor, DWORD dwSelectedNormalTextColor, const CPtrArray& rPtrArray );

public:
#endif // __IMPROVE_QUEST_INTERFACE
	virtual	void PaintFrame( C2DRender* p2DRender );
	virtual	void SetWndRect( CRect rectWnd, BOOL bOnSize = TRUE );
	virtual void OnInitialUpdate();
	virtual void OnDraw( C2DRender* p2DRender );
	virtual void OnLButtonUp( UINT nFlags, CPoint point );
	virtual void OnLButtonDown( UINT nFlags, CPoint point );
	virtual void OnRButtonUp( UINT nFlags, CPoint point );
	virtual void OnRButtonDown( UINT nFlags, CPoint point );
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point );
	virtual void OnRButtonDblClk( UINT nFlags, CPoint point );
	virtual void OnSize( UINT nType, int cx, int cy );
	virtual BOOL OnEraseBkgnd( C2DRender* p2DRender );

	virtual BOOL OnMouseWheel( UINT nFlags, short zDelta, CPoint pt );
	
	//virtual BOOL OnChildNotify(UINT message,UINT nID,LRESULT* pLResult);
/*
	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

// Attributes
	BOOL GetItemRect(HTREEITEM hItem, LPRECT lpRect, BOOL bTextOnly) const;
	UINT GetCount() const;
	UINT GetIndent() const;
	void SetIndent(UINT nIndent);
	CImageList* GetImageList(UINT nImageList) const;
	CImageList* SetImageList(CImageList* pImageList, int nImageListType);
	HTREEITEM GetNextItem(HTREEITEM hItem, UINT nCode) const;
	HTREEITEM GetChildItem(HTREEITEM hItem) const;
	HTREEITEM GetNextSiblingItem(HTREEITEM hItem) const;
	HTREEITEM GetPrevSiblingItem(HTREEITEM hItem) const;
	HTREEITEM GetParentItem(HTREEITEM hItem) const;
	HTREEITEM GetFirstVisibleItem() const;
	HTREEITEM GetNextVisibleItem(HTREEITEM hItem) const;
	HTREEITEM GetPrevVisibleItem(HTREEITEM hItem) const;
	HTREEITEM GetSelectedItem() const;
	HTREEITEM GetDropHilightItem() const;
	HTREEITEM GetRootItem() const;
	BOOL GetItem(TVITEM* pItem) const;
	CString GetItemText(HTREEITEM hItem) const;
	BOOL GetItemImage(HTREEITEM hItem, int& nImage, int& nSelectedImage) const;
	UINT GetItemState(HTREEITEM hItem, UINT nStateMask) const;
	DWORD GetItemData(HTREEITEM hItem) const;
	BOOL SetItem(TVITEM* pItem);
	BOOL SetItem(HTREEITEM hItem, UINT nMask, LPCTSTR lpszItem, int nImage,
		int nSelectedImage, UINT nState, UINT nStateMask, LPARAM lParam);
	BOOL SetItemText(HTREEITEM hItem, LPCTSTR lpszItem);
	BOOL SetItemImage(HTREEITEM hItem, int nImage, int nSelectedImage);
	BOOL SetItemState(HTREEITEM hItem, UINT nState, UINT nStateMask);
	BOOL SetItemData(HTREEITEM hItem, DWORD dwData);
	BOOL ItemHasChildren(HTREEITEM hItem) const;
	CEdit* GetEditControl() const;
	UINT GetVisibleCount() const;
	CToolTipCtrl* GetToolTips() const;
	CToolTipCtrl* SetToolTips(CToolTipCtrl* pWndTip);
	COLORREF GetBkColor() const;
	COLORREF SetBkColor(COLORREF clr);
	SHORT GetItemHeight() const;
	SHORT SetItemHeight(SHORT cyHeight);
	COLORREF GetTextColor() const;
	COLORREF SetTextColor(COLORREF clr);
	BOOL SetInsertMark(HTREEITEM hItem, BOOL fAfter = TRUE);
	BOOL GetCheck(HTREEITEM hItem) const;
	BOOL SetCheck(HTREEITEM hItem, BOOL fCheck = TRUE);
	COLORREF GetInsertMarkColor() const;
	COLORREF SetInsertMarkColor(COLORREF clrNew);

// Operations
	HTREEITEM InsertItem(LPTVINSERTSTRUCT lpInsertStruct);
	HTREEITEM InsertItem(UINT nMask, LPCTSTR lpszItem, int nImage,
		int nSelectedImage, UINT nState, UINT nStateMask, LPARAM lParam,
		HTREEITEM hParent, HTREEITEM hInsertAfter);
	HTREEITEM InsertItem(LPCTSTR lpszItem, HTREEITEM hParent = TVI_ROOT,
		HTREEITEM hInsertAfter = TVI_LAST);
	HTREEITEM InsertItem(LPCTSTR lpszItem, int nImage, int nSelectedImage,
		HTREEITEM hParent = TVI_ROOT, HTREEITEM hInsertAfter = TVI_LAST);
	BOOL DeleteItem(HTREEITEM hItem);
	BOOL DeleteAllItems();
	BOOL Expand(HTREEITEM hItem, UINT nCode);
	BOOL Select(HTREEITEM hItem, UINT nCode);
	BOOL SelectItem(HTREEITEM hItem);
	BOOL SelectDropTarget(HTREEITEM hItem);
	BOOL SelectSetFirstVisible(HTREEITEM hItem);
	CEdit* EditLabel(HTREEITEM hItem);
	HTREEITEM HitTest(CPoint pt, UINT* pFlags = NULL) const;
	HTREEITEM HitTest(TVHITTESTINFO* pHitTestInfo) const;
	CImageList* CreateDragImage(HTREEITEM hItem);
	BOOL SortChildren(HTREEITEM hItem);
	BOOL EnsureVisible(HTREEITEM hItem);
	BOOL SortChildrenCB(LPTVSORTCB pSort);
	*/
};
//////////////////////////////////////////////////////////////////////////////
// CWndSliderCtrl
//////////////////////////////////////////////////////////////////////////////

class CWndSliderCtrl : public CWndBase
{
public:
};
//////////////////////////////////////////////////////////////////////////////
// CWndListBox
//////////////////////////////////////////////////////////////////////////////

/*
 * Listbox Styles
 */
#define WLBS_NOTIFY            0x0001L
#define WLBS_SORT              0x0002L
#define WLBS_NOREDRAW          0x0004L
#define WLBS_MULTIPLESEL       0x0008L
#define WLBS_OWNERDRAWFIXED    0x0010L
#define WLBS_OWNERDRAWVARIABLE 0x0020L
#define WLBS_HASSTRINGS        0x0040L
#define WLBS_USETABSTOPS       0x0080L
#define WLBS_NOINTEGRALHEIGHT  0x0100L
#define WLBS_MULTICOLUMN       0x0200L
#define WLBS_WANTKEYBOARDINPUT 0x0400L
#define WLBS_EXTENDEDSEL       0x0800L
#define WLBS_DISABLENOSCROLL   0x1000L
#define WLBS_NODATA            0x2000L
#define WLBS_NOSEL             0x4000L
#define WLBS_STANDARD          (LBS_NOTIFY | LBS_SORT | WS_VSCROLL | WS_BORDER)

class CWndListBox : public CWndBase
{
	void InterpriteScript(CScanner& scanner,CPtrArray& ptrArray); 
	void PaintListBox(C2DRender* p2DRender,CPoint& pt,CPtrArray& ptrArray);

public:
	typedef struct tagITEM
	{
		CRect      m_rect;
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
		CEditString m_strWord;
		BOOL		m_bIsValid;
		BOOL		m_bIsVisible;
#else // __IMPROVE_QUEST_INTERFACE
		BOOL       m_bButton;
		CString    m_strWord;
#endif // __IMPROVE_QUEST_INTERFACE
		DWORD      m_dwData;
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
		CString    m_strKey;
		DWORD      m_dwData2;
		tagITEM( void ) : m_rect( 0, 0, 0, 0 ), m_strWord( _T("") ), m_bIsValid( TRUE ), m_bIsVisible( TRUE ), m_dwData( 0 ), m_strKey( _T("") ), m_dwData2( 0 ) {}
#endif // __IMPROVE_QUEST_INTERFACE
	} LISTITEM,* LPLISTITEM;

protected:
	CPtrArray m_listItemArray;
	LPLISTITEM m_pFocusItem;
	int           m_nCurSelect  ;
	DWORD         m_nWndColor   ;
	LISTITEM      m_listItem    ;
	CWndScrollBar m_wndScrollBar;
public:
	DWORD         m_nFontColor  ; 
	DWORD         m_nSelectColor;
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
private:
	DWORD         m_dwOnMouseColor;
	DWORD         m_dwInvalidColor;
	DWORD         m_nLeftMargin;
public:
#endif // __IMPROVE_QUEST_INTERFACE
	int           m_nLineSpace  ;
	int           m_nFontHeight ;

	CWndListBox();
	~CWndListBox();

	void Create( DWORD dwListBoxStyle, RECT& rect, CWndBase* pParentWnd, UINT nID );
	int GetFontHeight() { return m_pFont->GetMaxHeight() + m_nLineSpace; }
	int   GetCount() const;
	DWORD GetItemData(int nIndex) const;
	void* GetItemDataPtr(int nIndex) const;
	int   SetItemData(int nIndex,DWORD dwItemData);
	int   SetItemDataPtr(int nIndex,void* pData);
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	DWORD GetItemData2( int nIndex ) const;
	void* GetItemData2Ptr( int nIndex ) const;
	BOOL GetItemValidity( int nIndex );
	BOOL GetItemVisibility( int nIndex );
	int SetItemData2( int nIndex,DWORD dwItemData );
	int SetItemData2Ptr( int nIndex,void* pData );
	int SetItemValidity( int nIndex, BOOL bValidity );
	int SetItemVisibility( int nIndex, BOOL bIsVisible );
#endif // __IMPROVE_QUEST_INTERFACE
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	const CRect& GetItemRect( int nIndex ) const;
#else // __IMPROVE_QUEST_INTERFACE
	int   GetItemRect(int nIndex,LPRECT lpRect) const;
#endif // __IMPROVE_QUEST_INTERFACE
	int   GetSel(int nIndex) const;
	int   GetText(int nIndex,LPSTR lpszBuffer) const;
	void  GetText(int nIndex,CString& rString) const;
	int   GetTextLen(int nIndex) const;
	
	int   GetCurSel() const;
	int   SetCurSel(int nSelect);
	int   SetSel(int nIndex,BOOL bSelect = TRUE);
	int   GetSelCount() const;
	int   GetSelItems(int nMaxItems,LPINT rgIndex) const;
	int   GetScrollPos() {return m_wndScrollBar.GetScrollPos();};
	void  SetScrollPos( int nPos, BOOL bRedraw = TRUE ) { m_wndScrollBar.SetScrollPos( nPos, bRedraw ); }	//gmpbigsun: added
	int   AddString(LPCTSTR lpszItem);
	int   DeleteString(UINT nIndex);
	int   InsertString(int nIndex,LPCTSTR lpszItem);
	void	SetString( int nIndex, LPCTSTR lpszItem );
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	const CString& GetString( int nIndex ) const;
	void SetListStringAlpha( int nIndex, BYTE byAlpha );
	void SetKeyString( int nIndex, LPCTSTR lpszItem );
	const CString& GetKeyString( int nIndex ) const;
	void SetOnMouseColor( DWORD dwOnMouseColor );
	DWORD GetOnMouseColor( void ) const;
	void SetInvalidColor( DWORD dwInvalidColor );
	DWORD GetInvalidColor( void ) const;
	void SetLeftMargin( int nLeftMargin );
	int GetLeftMargin( void ) const;
#endif // __IMPROVE_QUEST_INTERFACE
#ifdef __IMPROVE_MAP_SYSTEM
	int GetItemIndex( const CString& strItem ) const;
	int GetItemIndex( DWORD dwItem ) const;
#endif // __IMPROVE_MAP_SYSTEM
	void  ResetContent();
	int   FindString(int nStartAfter,LPCTSTR lpszItem) const;
	int   FindStringExact(int nIndexStart,LPCTSTR lpszItem) const;
	int   SelectString(int nStartAfter,LPCTSTR lpszItem);
	void  SortListBox();

	void LoadListBoxScript(LPCTSTR lpFileName); 

	virtual	void SetWndRect(CRect rectWnd, BOOL bOnSize = TRUE);
	virtual void OnInitialUpdate();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnRButtonUp(UINT nFlags, CPoint point);
	virtual void OnRButtonDown(UINT nFlags, CPoint point);
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point);
	virtual void OnRButtonDblClk( UINT nFlags, CPoint point);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnEraseBkgnd( C2DRender* p2DRender );
	virtual	void PaintFrame( C2DRender* p2DRender );
	virtual void OnSetFocus( CWndBase* pOldWnd );
	virtual BOOL OnMouseWheel( UINT nFlags, short zDelta, CPoint pt );
	friend int QSortListBox( const VOID* arg1, const VOID* arg2 );
};
// class CListCtrl  CButton

/////////////////////////////////////////////////////////////////////////////
// CListCtrl
// begin_r_commctrl
// List view Control

#define WLVS_ICON                0x0000
#define WLVS_REPORT              0x0001
#define WLVS_SMALLICON           0x0002
#define WLVS_LIST                0x0003
#define WLVS_TYPEMASK            0x0003
#define WLVS_SINGLESEL           0x0004
#define WLVS_SHOWSELALWAYS       0x0008
#define WLVS_SORTASCENDING       0x0010
#define WLVS_SORTDESCENDING      0x0020
#define WLVS_SHAREIMAGELISTS     0x0040
#define WLVS_NOLABELWRAP         0x0080
#define WLVS_AUTOARRANGE         0x0100
#define WLVS_EDITLABELS          0x0200
#define WLVS_NOSCROLL            0x2000
#define WLVS_TYPESTYLEMASK       0xfc00
#define WLVS_ALIGNTOP            0x0000
#define WLVS_ALIGNLEFT           0x0800
#define WLVS_ALIGNMASK           0x0c00
#define WLVS_OWNERDRAWFIXED      0x0400
#define WLVS_NOCOLUMNHEADER      0x4000
#define WLVS_NOSORTHEADER        0x8000

// end_r_commctrl

class CWndEdit;

class CWndListCtrl : public CWndBase
{
	void InterpriteScript(CScanner& scanner,CPtrArray& ptrArray); 
	//void PaintListBox(C2DRender* p2DRender,CPoint& pt,CPtrArray& ptrArray);
	LVITEM*       m_pFocusItem;
	int           m_nCurSelect  ;
	int           m_nFontHeight ;
	DWORD         m_nWndColor   ;
//	DWORD         m_nFontColor  ; 
//	DWORD         m_nSelectColor;
	CWndScrollBar m_wndScrollBar;
	CPtrArray     m_aItems;
	CPtrArray     m_aColumns;

// Constructors
public:
	int           m_nLineSpace  ;
	DWORD         m_nFontColor  ; 
	DWORD         m_nSelectColor;
	//	DWORD         m_dwListCtrlStyle;

	//CWndListCtrl();
	//BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	CWndListCtrl();
	~CWndListCtrl();

	void Create( DWORD dwListCtrlStyle, RECT& rect, CWndBase* pParentWnd, UINT nID );
	void LoadListBoxScript(LPCTSTR lpFileName); 
	int GetFontHeight() { return m_pFont->GetMaxHeight() + m_nLineSpace; }
// Attributes
	int   GetCurSel() const;
	int   SetCurSel(int nSelect);
	COLORREF GetBkColor() const;
	BOOL SetBkColor(COLORREF cr);
	//CImageList* GetImageList(int nImageList) const;
	//CImageList* SetImageList(CImageList* pImageList, int nImageListType);
	int GetItemCount() const;
	BOOL GetItem(LVITEM* pItem) const;
	BOOL SetItem(const LVITEM* pItem);
	BOOL SetItem(int nItem, int nSubItem, UINT nMask, LPCTSTR lpszItem,
		int nImage, UINT nState, UINT nStateMask, LPARAM lParam);
	UINT GetCallbackMask() const;
	BOOL SetCallbackMask(UINT nMask);
	int GetNextItem(int nItem, int nFlags) const;
	POSITION GetFirstSelectedItemPosition() const;
	int GetNextSelectedItem(POSITION& pos) const;
	BOOL GetItemRect(int nItem, LPRECT lpRect, UINT nCode) const;
	BOOL SetItemPosition(int nItem, POINT pt);
	BOOL GetItemPosition(int nItem, LPPOINT lpPoint) const;
	int GetStringWidth(LPCTSTR lpsz) const;
	CWndEdit* GetEditControl() const;
	BOOL GetColumn(int nCol, LVCOLUMN* pColumn) const;
	BOOL SetColumn(int nCol, const LVCOLUMN* pColumn);
	int GetColumnWidth(int nCol) const;
	BOOL SetColumnWidth(int nCol, int cx);
	BOOL GetViewRect(LPRECT lpRect) const;
	COLORREF GetTextColor() const;
	BOOL SetTextColor(COLORREF cr);
	COLORREF GetTextBkColor() const;
	BOOL SetTextBkColor(COLORREF cr);
	int GetTopIndex() const;
	int GetCountPerPage() const;
	BOOL GetOrigin(LPPOINT lpPoint) const;
	BOOL SetItemState(int nItem, LVITEM* pItem);
	BOOL SetItemState(int nItem, UINT nState, UINT nMask);
	UINT GetItemState(int nItem, UINT nMask) const;
	CString GetItemText(int nItem, int nSubItem) const;
	int GetItemText(int nItem, int nSubItem, LPTSTR lpszText, int nLen) const;
	BOOL SetItemText(int nItem, int nSubItem, LPCTSTR lpszText);
	void SetItemCount(int nItems);
	BOOL SetItemData(int nItem, DWORD dwData);
	DWORD GetItemData(int nItem) const;
	UINT GetSelectedCount() const;
	BOOL SetColumnOrderArray(int iCount, LPINT piArray);
	BOOL GetColumnOrderArray(LPINT piArray, int iCount = -1);
	CSize SetIconSpacing(CSize size);
	CSize SetIconSpacing(int cx, int cy);
	//CHeaderCtrl* GetHeaderCtrl();
	//HCURSOR GetHotCursor();
	//HCURSOR SetHotCursor(HCURSOR hc);
	BOOL GetSubItemRect(int iItem, int iSubItem, int nArea, CRect& ref);
	int GetHotItem();
	int SetHotItem(int iIndex);
	int GetSelectionMark();
	int SetSelectionMark(int iIndex);
	DWORD GetExtendedStyle();
	DWORD SetExtendedStyle(DWORD dwNewStyle);
	int SubItemHitTest(LPLVHITTESTINFO pInfo);
	void SetWorkAreas(int nWorkAreas, LPRECT lpRect);
	BOOL SetItemCountEx(int iCount, DWORD dwFlags = LVSICF_NOINVALIDATEALL);
	CSize ApproximateViewRect(CSize sz = CSize(-1, -1), int iCount = -1) const;
	BOOL GetBkImage(LVBKIMAGE* plvbkImage) const;
	DWORD GetHoverTime() const;
	void GetWorkAreas(int nWorkAreas, LPRECT prc) const;
	BOOL SetBkImage(HBITMAP hbm, BOOL fTile = TRUE, int xOffsetPercent = 0, int yOffsetPercent = 0);
	BOOL SetBkImage(LPTSTR pszUrl, BOOL fTile = TRUE, int xOffsetPercent = 0, int yOffsetPercent = 0);
	BOOL SetBkImage(LVBKIMAGE* plvbkImage);
	DWORD SetHoverTime(DWORD dwHoverTime = (DWORD)-1);
	UINT GetNumberOfWorkAreas() const;
	BOOL GetCheck(int nItem) const;
	BOOL SetCheck(int nItem, BOOL fCheck = TRUE);

// Operations
	int InsertItem(const LVITEM* pItem);
	int InsertItem(int nItem, LPCTSTR lpszItem);
	int InsertItem(int nItem, LPCTSTR lpszItem, int nImage);
	BOOL DeleteItem(int nItem);
	BOOL DeleteAllItems();
	//int FindItem(LVFINDINFO* pFindInfo, int nStart = -1) const;
	//int HitTest(LVHITTESTINFO* pHitTestInfo) const;
	int HitTest(CPoint pt, UINT* pFlags = NULL) const;
	BOOL EnsureVisible(int nItem, BOOL bPartialOK);
	BOOL Scroll(CSize size);
	BOOL RedrawItems(int nFirst, int nLast);
	BOOL Arrange(UINT nCode);
	CEdit* EditLabel(int nItem);
	int InsertColumn(int nCol, const LVCOLUMN* pColumn);
	int InsertColumn(int nCol, LPCTSTR lpszColumnHeading,
		int nFormat = LVCFMT_LEFT, int nWidth = -1, int nSubItem = -1);
	BOOL DeleteColumn(int nCol);
//	CImageList* CreateDragImage(int nItem, LPPOINT lpPoint);
	BOOL Update(int nItem);
	//BOOL SortItems(PFNLVCOMPARE pfnCompare, DWORD dwData);

// Overridables
	//virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual	void SetWndRect(CRect rectWnd, BOOL bOnSize = TRUE);
	virtual void OnInitialUpdate();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnRButtonUp(UINT nFlags, CPoint point);
	virtual void OnRButtonDown(UINT nFlags, CPoint point);
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point);
	virtual void OnRButtonDblClk( UINT nFlags, CPoint point);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnEraseBkgnd(C2DRender* p2DRender);
	virtual	void PaintFrame(C2DRender* p2DRender);
// Implementation
public:
	int InsertItem(UINT nMask, int nItem, LPCTSTR lpszItem, UINT nState,
		UINT nStateMask, int nImage, LPARAM lParam);
	//virtual ~CWndListCtrl();
protected:
	void RemoveImageList(int nImageList);
//	virtual BOOL OnChildNotify(UINT, WPARAM, LPARAM, LRESULT*);
protected:
	////{{AFX_MSG(CListCtrl)
	//afx_msg void OnNcDestroy();
	////}}AFX_MSG
	//DECLARE_MESSAGE_MAP()
};


//////////////////////////////////////////////////////////////////////////////
// CWndGroupBox
//////////////////////////////////////////////////////////////////////////////

class CWndGroupBox : public CWndBase
{
protected:

public:
	CWndGroupBox();
	~CWndGroupBox();
	BOOL Create( DWORD dwTextStyle, const RECT& rect, CWndBase* pParentWnd, UINT nID );

	virtual	void OnInitialUpdate();
	//virtual void SetWndRect(CRect rectWnd, BOOL bOnSize = TRUE);
	virtual	void PaintFrame(C2DRender* p2DRender);
	virtual void OnDraw(C2DRender* p2DRender);
	//virtual void OnLButtonDown(UINT nFlags, CPoint point);
	//virtual void OnLButtonUp(UINT nFlags, CPoint point);
	//virtual void OnMouseMove(UINT nFlags, CPoint point);
	//virtual void OnLButtonDblClk(UINT nFlags, CPoint point);
	//virtual void OnSetFocus(CWndBase* pOldWnd);
	//virtual void OnKillFocus(CWndBase* pNewWnd);aa
	//virtual void OnSize(UINT nType, int cx, int cy);
};
/////////////////////////////////////////////////////////////////////////////
// CTabCtrl

typedef struct tagWTCITEM
{
    UINT mask;
    DWORD dwState;
    DWORD dwStateMask;
    //TCHAR szText[ 32 ];
	LPCTSTR pszText;
    int cchTextMax;
    int iImage;
    LPARAM lParam;
	CWndBase* pWndBase;
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	tagWTCITEM( void );
#endif // __IMPROVE_QUEST_INTERFACE
} WTCITEM, FAR *LPWTCITEM;


class CWndTabCtrl: public CWndBase
{
//	DECLARE_DYNAMIC(CTabCtrl)
	vector< LPWTCITEM > m_aTab;
//	CObjArray::iterator m_itor;
// Constructors
	int m_nCurSelect;
//#ifdef __NEWTAB
	CTexture m_aTexture[ 6 ];
	CTexture m_INV_TextureS;
	CTexture m_INV_TextureF;
//#else
//	CTexture* m_apTexture[ 10 ];
//#endif
	int m_nTabButtonLength;
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
public:
	enum TabTitleAlign { ALIGN_LEFT, ALIGN_RIGHT, ALIGN_CENTER };

private:
	TabTitleAlign m_eTabTitleAlign;
#endif // __IMPROVE_QUEST_INTERFACE
public:
	CWndTabCtrl();
	BOOL Create(DWORD dwStyle, const RECT& rect, CWndBase* pParentWnd, UINT nID);

	void SetButtonLength( int nLength ) { m_nTabButtonLength = nLength; }
	BOOL InsertTexture( int nItem, LPCTSTR lpszFileName );
	int GetSize() { return m_aTab.size(); }

	virtual HRESULT InitDeviceObjects();
	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();
	
	virtual	void PaintFrame( C2DRender* p2DRender );
	virtual void OnDraw( C2DRender* p2DRender );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual	void SetWndRect( CRect rectWnd, BOOL bOnSize = TRUE);
	virtual void AdditionalSkinTexture( LPWORD pDest, CSize sizeSurface, D3DFORMAT d3dFormat = D3DFMT_A4R4G4B4 );
	virtual	void OnInitialUpdate();
	
// Attributes
	//CImageList* GetImageList() const;
	//CImageList* SetImageList(CImageList* pImageList);
	//int GetItemCount() const;
	BOOL GetItem(int nItem, WTCITEM* pTabCtrlItem) const;
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	LPWTCITEM GetTabItem( int nItemNumber ) const;
	LPWTCITEM GetSelectedTab( void ) const;
	void SetTabTitleAlign( const TabTitleAlign eTabTitleAlign );
	const TabTitleAlign GetTabTitleAlign( void ) const;
#endif // __IMPROVE_QUEST_INTERFACE
	//BOOL SetItem(int nItem, TCITEM* pTabCtrlItem);
	//BOOL SetItemExtra(int nBytes);
	//BOOL GetItemRect(int nItem, LPRECT lpRect) const;
	int GetCurSel() const;
	int SetCurSel(int nItem);
	/*
	void SetCurFocus(int nItem);
	CSize SetItemSize(CSize size);
	void SetPadding(CSize size);
	int GetRowCount() const;
	CToolTipCtrl* GetToolTips() const;
	void SetToolTips(CToolTipCtrl* pWndTip);
	int GetCurFocus() const;
	int SetMinTabWidth(int cx);
	DWORD GetExtendedStyle();
	DWORD SetExtendedStyle(DWORD dwNewStyle, DWORD dwExMask = 0);
	DWORD GetItemState(int nItem, DWORD dwMask) const;
	BOOL SetItemState(int nItem, DWORD dwMask, DWORD dwState);
*/
// Operations
	BOOL InsertItem(int nItem, WTCITEM* pTabCtrlItem);
	BOOL InsertItem(int nItem, LPCTSTR lpszItem);

/////////////////////
	BOOL InsertItemMEU(int nItem, WTCITEM* pTabCtrlItem, int nIndex);
	BOOL InsertItemMEU(int nItem, LPCTSTR lpszItem, int nIndex);
/////////////////////
	/*
	BOOL InsertItem(int nItem, LPCTSTR lpszItem, int nImage);
	BOOL InsertItem(UINT nMask, int nItem, LPCTSTR lpszItem,
		int nImage, LPARAM lParam);
	BOOL InsertItem(UINT nMask, int nItem, LPCTSTR lpszItem,
		int nImage, LPARAM lParam, DWORD dwState, DWORD dwStateMask);
	BOOL DeleteItem(int nItem);
	BOOL DeleteAllItems();
	void AdjustRect(BOOL bLarger, LPRECT lpRect);
	void RemoveImage(int nImage);
	int HitTest(TCHITTESTINFO* pHitTestInfo) const;
	void DeselectAll(BOOL fExcludeFocus);
	BOOL HighlightItem(int idItem, BOOL fHighlight = TRUE);
*/
// Overridables
	//virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

// Implementation
public:
	virtual ~CWndTabCtrl();
//protected:
	//virtual BOOL OnChildNotify(UINT, WPARAM, LPARAM, LRESULT*);
	////{{AFX_MSG(CTabCtrl)
	//afx_msg void OnDestroy();
	////}}AFX_MSG
	//DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////
// CWndComboBox
//////////////////////////////////////////////////////////////////////////////

#define WCBS_SIMPLE            0x0001L
#define WCBS_DROPDOWN          0x0002L
#define WCBS_DROPDOWNLIST      0x0003L
#define WCBS_OWNERDRAWFIXED    0x0010L
#define WCBS_OWNERDRAWVARIABLE 0x0020L
#define WCBS_AUTOHSCROLL       0x0040L
#define WCBS_OEMCONVERT        0x0080L
#define WCBS_SORT              0x0100L
#define WCBS_HASSTRINGS        0x0200L
#define WCBS_NOINTEGRALHEIGHT  0x0400L
#define WCBS_DISABLENOSCROLL   0x0800L
#define WCBS_UPPERCASE         0x2000L
#define WCBS_LOWERCASE         0x4000L

#include "WndEditCtrl.h"

#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
class CWndComboListBox : public CWndListBox
{
public:
#ifdef __IMPROVE_MAP_SYSTEM
	virtual	void PaintFrame( C2DRender* p2DRender );
#endif // __IMPROVE_MAP_SYSTEM
	virtual void OnKillFocus( CWndBase* pNewWnd );
};
#endif // __IMPROVE_QUEST_INTERFACE

class CWndComboBox : public CWndEdit
{
public: 
//	DWORD m_dwComboBoxStyle; 
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	CWndComboListBox m_wndListBox;
#else // __IMPROVE_QUEST_INTERFACE
	CWndListBox m_wndListBox;
#endif // __IMPROVE_QUEST_INTERFACE
	CWndButton  m_wndButton;

	CWndComboBox();
	~CWndComboBox();

// Attributes
	// for entire combo box
	int GetCount() const;
	int GetCurSel() const;
	int SetCurSel(int nSelect);
	LCID GetLocale() const;
	LCID SetLocale(LCID nNewLocale);
// Win4
	int GetTopIndex() const;
	int SetTopIndex(int nIndex);
	int InitStorage(int nItems, UINT nBytes);
	void SetHorizontalExtent(UINT nExtent);
	UINT GetHorizontalExtent() const;
	int SetDroppedWidth(UINT nWidth);
	int GetDroppedWidth() const;

	// for edit control
	DWORD GetEditSel() const;
	BOOL LimitText(int nMaxChars);
	BOOL SetEditSel(int nStartChar, int nEndChar);

	// for combobox item
	DWORD GetItemData(int nIndex) const;
	int SetItemData(int nIndex, DWORD dwItemData);
	void* GetItemDataPtr(int nIndex) const;
	int SetItemDataPtr(int nIndex, void* pData);
#ifdef __IMPROVE_MAP_SYSTEM
	DWORD GetSelectedItemData( void ) const;
	void GetListBoxText( int nIndex, CString& strString ) const;
	int GetListBoxTextLength( int nIndex ) const;
	int GetListBoxSize( void ) const;
#else // __IMPROVE_MAP_SYSTEM
	int GetLBText(int nIndex, LPTSTR lpszText) const;
	void GetLBText(int nIndex, CString& rString) const;
	int GetLBTextLen(int nIndex) const;
#endif // __IMPROVE_MAP_SYSTEM

	int SetItemHeight(int nIndex, UINT cyItemHeight);
	int GetItemHeight(int nIndex) const;
	int FindStringExact(int nIndexStart, LPCTSTR lpszFind) const;
	int SetExtendedUI(BOOL bExtended = TRUE);
	BOOL GetExtendedUI() const;
	void GetDroppedControlRect(LPRECT lprect) const;
	BOOL GetDroppedState() const;

// Operations
	// for drop-down combo boxes
	void ShowDropDown(BOOL bShowIt = TRUE);

	// manipulating listbox items
	int AddString(LPCTSTR lpszString);
	int DeleteString(UINT nIndex);
	int InsertString(int nIndex, LPCTSTR lpszString);
	void ResetContent();
	int Dir(UINT attr, LPCTSTR lpszWildCard);

	// selection helpers
	int FindString(int nStartAfter, LPCTSTR lpszString) const;
	int SelectString(int nStartAfter, LPCTSTR lpszString);

	// Clipboard operations
	void Clear();
	void Copy();
	void Cut();
	void Paste();

#ifdef __IMPROVE_MAP_SYSTEM
	void SelectItem( const CString& strItem );
	void SelectItem( DWORD dwItem );

private:
	void SetSelectedItemInformation( void );

public:
#endif // __IMPROVE_MAP_SYSTEM
/*
// Overridables (must override draw, measure and compare for owner draw)
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	virtual int CompareItem(LPCOMPAREITEMSTRUCT lpCompareItemStruct);
	virtual void DeleteItem(LPDELETEITEMSTRUCT lpDeleteItemStruct);
*/


	void Create( DWORD dwComboBoxStyle, const RECT& rect, CWndBase* pParentWnd, UINT nID );
	virtual	void OnInitialUpdate();
	virtual void SetWndRect( CRect rectWnd, BOOL bOnSize = TRUE );
	virtual	void PaintFrame( C2DRender* p2DRender );
	virtual void OnDraw( C2DRender* p2DRender );
	virtual void OnLButtonDown( UINT nFlags, CPoint point );
	virtual void OnLButtonUp( UINT nFlags, CPoint point );
	virtual BOOL OnChildNotify(UINT message,UINT nID,LRESULT* pLResult);
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
private:
	void OpenListBox( void );
	CPtrArray m_listItemArray;
	BOOL m_bOpen;
#endif // __IMPROVE_QUEST_INTERFACE
};

#endif // !defined(AFX_WNDCONTROL_H__0B45596D_70D7_48A4_BCB2_3D0F32F58E57__INCLUDED_)

