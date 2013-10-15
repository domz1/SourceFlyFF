// WndBase.h: interface for the CWndBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WNDSHOPCTRL_H__0B45596D_70D7_48A4_BCB2_3D0F32F58E57__INCLUDED_)
#define AFX_WNDSHOPCTRL_H__0B45596D_70D7_48A4_BCB2_3D0F32F58E57__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#ifdef __16_SHOP

class C2DRender;
class CItemElem;

class CWndShopCtrl : public CWndBase
{
	int           m_nCurSelect  ;
	int           m_nFontHeight ;
	DWORD         m_nWndColor   ;
	DWORD         m_nFontColor  ; 
	DWORD         m_nSelectColor;

	int				m_nLastPoint;

// Constructors
public:
	static CTextureMng m_textureMng;
	CRect         m_rect;
	BOOL          m_bDrag;
	DWORD         m_dwListCtrlStyle;
	CItemElem*		m_pItemElem[MAX_CART];
	char			m_cTab[MAX_CART];
	int				m_nBuy[MAX_CART];
	int				m_nTabType;

	CWndShopCtrl();
	virtual ~CWndShopCtrl();

	void Create( DWORD m_dwListCtrlStyle, RECT& rect, CWndBase* pParentWnd, UINT nID );
	void InsertItem( CItemElem* pItemElem, char cTab, int nBuy );
	void Clear( int nIndex = -1 );
// Attributes
	virtual void OnInitialUpdate();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual	void PaintFrame(C2DRender* p2DRender);
	virtual void OnMouseMove(UINT nFlags, CPoint point);
	virtual void SetWndRect( CRect rectWnd, BOOL bOnSize );
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point = 0 );

	int GetCost( int nType = 0 );
	int GetCount();
};

#endif

#endif

