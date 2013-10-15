// WndBase.h: interface for the CWndBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WNDPARTYCTRL_H__0B45596D_70D7_48A4_BCB2_3D0F32F58E57__INCLUDED_)
#define AFX_WNDPARTYCTRL_H__0B45596D_70D7_48A4_BCB2_3D0F32F58E57__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class C2DRender;
class CItemElem;

class CWndPartyCtrl : public CWndBase
{
	void InterpriteScript(CScanner& scanner,CPtrArray& ptrArray); 
	//void PaintListBox(C2DRender* p2DRender,CPoint& pt,CPtrArray& ptrArray);
	int           m_nCurSelect  ;
	int           m_nFontHeight ;
	CWndScrollBar m_wndScrollBar;

	CTexture m_texGauEmptyNormal ;
	CTexture m_texGauFillNormal  ;
	
	LPDIRECT3DVERTEXBUFFER9 m_pVBGauge;
	
// Constructors
public:
	CRect         m_rect;
	BOOL          m_bDrag;
	DWORD         m_dwListCtrlStyle;

	CWndPartyCtrl();
	~CWndPartyCtrl();

	void Create( DWORD m_dwListCtrlStyle, RECT& rect, CWndBase* pParentWnd, UINT nID );
	void LoadListBoxScript(LPCTSTR lpFileName); 
// Attributes
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
	virtual	void SetWndRect(CRect rectWnd, BOOL bOnSize = TRUE );
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
	virtual void OnMouseMove(UINT nFlags, CPoint point);

	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();

// Implementation
public:
	int InsertItem(UINT nMask, int nItem, LPCTSTR lpszItem, UINT nState,
		UINT nStateMask, int nImage, LPARAM lParam);
	//virtual ~CWndItemList();
	
	void ScrollBarPos( int nPos );

protected:
	void RemoveImageList(int nImageList);
//	virtual BOOL OnChildNotify(UINT, WPARAM, LPARAM, LRESULT*);
protected:
	////{{AFX_MSG(CListCtrl)
	//afx_msg void OnNcDestroy();
	////}}AFX_MSG
	//DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_WNDSKILLCTRL_H__0B45596D_70D7_48A4_BCB2_3D0F32F58E57__INCLUDED_)

