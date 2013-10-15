#ifndef __WNDCOMMITEM__H
#define __WNDCOMMITEM__H

class C2DRender;
class CItemElem;

class CWndCommItemCtrl : public CWndBase
{
	void InterpriteScript(CScanner& scanner,CPtrArray& ptrArray); 
	//void PaintListBox(C2DRender* p2DRender,CPoint& pt,CPtrArray& ptrArray);
	int           m_nFontHeight ;
	CWndScrollBar m_wndScrollBar;
	
	CWndWorld* pWndWorld;
	int			m_dwDraw[SM_MAX+MAX_SKILLINFLUENCE];
	int			m_nMaxDraw;

	CTexture m_texGauEmptyNormal ;
	CTexture m_texGauFillNormal  ;
	
	LPDIRECT3DVERTEXBUFFER9 m_pVBGauge;


// Constructors
public:
	CRect         m_rect;
	DWORD         m_dwListCtrlStyle;


	void	SetScrollBar();
	int		GetMaxBuff();			// 출력해야할 MAX값얻어오기
	void	DrawSM( C2DRender* p2DRender, CPoint* pPoint, int x, int &nScroll );
	void	DrawSkill( C2DRender* p2DRender, CPoint* pPoint, int x, int &nScroll );

	CWndCommItemCtrl();
	~CWndCommItemCtrl();

	void GetFriendName();
	void Create( DWORD m_dwListCtrlStyle, RECT& rect, CWndBase* pParentWnd, UINT nID );
	void LoadListBoxScript(LPCTSTR lpFileName); 
	LPSKILL GetSkill( int i );
// Attributes
	COLORREF GetBkColor() const;
	BOOL SetBkColor(COLORREF cr);
	BOOL GetItem(LVITEM* pItem) const;
	BOOL SetItem(const LVITEM* pItem);
	BOOL SetItem(int nItem, int nSubItem, UINT nMask, LPCTSTR lpszItem,
		int nImage, UINT nState, UINT nStateMask, LPARAM lParam);
// Operations
	int InsertItem(const LVITEM* pItem);

// Overridables
	virtual void OnMouseWndSurface( CPoint point );
	virtual	void SetWndRect(CRect rectWnd, BOOL bOnSize = TRUE );
	virtual void OnInitialUpdate();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnEraseBkgnd(C2DRender* p2DRender);
	virtual	void PaintFrame(C2DRender* p2DRender);

	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();

// Implementation
public:
	int InsertItem(UINT nMask, int nItem, LPCTSTR lpszItem, UINT nState,
		UINT nStateMask, int nImage, LPARAM lParam);
protected:
	void RemoveImageList(int nImageList);
};

class CWndCommItem : public CWndNeuz 
{ 
public:
	CWndCommItemCtrl	m_wndCommItemCtrl;
public: 
	CWndCommItem(); 
	~CWndCommItem(); 
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 

class CWndCommercialElem : public CWndNeuz 
{ 
public: 
	CItemElem* m_pItemElem[2];
	CRect      m_Rect[2];

	void InitSetting( void );
	BOOL IsRestrictionItem( CItemElem* pItemElem, BOOL bMessage = FALSE );
	BOOL IsUpgradeItem( CItemElem* pItemElem, BOOL bMessage = FALSE );
	BOOL IsSMItem( CItemElem* pItemElem, BOOL bMessage = FALSE );
	
	
	CWndCommercialElem(); 
	~CWndCommercialElem(); 
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual void OnRButtonUp( UINT nFlags, CPoint point );
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point );
}; 

class CWndRemoveElem : public CWndNeuz 
{ 
public: 
	BYTE		m_nType;
	int			m_nParts;
	DWORD		m_dwItemId;
	OBJID		m_objid;
	CItemElem*	m_pItemElem;
	BOOL		m_bSetting;

	CWndRemoveElem(); 
	~CWndRemoveElem(); 

	void OnSetItem( BYTE nType, DWORD dwItemId, OBJID objid, int nParts, CItemElem* pItemElem );
	
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
