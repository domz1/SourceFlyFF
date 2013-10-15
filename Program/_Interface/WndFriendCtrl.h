// WndBase.h: interface for the CWndBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WNDFRIENDCTRL_H__0B45596D_70D7_48A4_BCB2_3D0F32F58E57__INCLUDED_)
#define AFX_WNDFRIENDCTRL_H__0B45596D_70D7_48A4_BCB2_3D0F32F58E57__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef __RT_1025
#include "rtmessenger.h"
#else	// __RT_1025
#include "messenger.h"
#endif	// __RT_1025

class C2DRender;
class CItemElem;

class CWndFriendCtrl : public CWndBase
{
	void InterpriteScript(CScanner& scanner,CPtrArray& ptrArray); 
	int           m_nCurSelect  ;
	int           m_nFontHeight ;
	CWndScrollBar m_wndScrollBar;

	u_long		  m_uParent		 ;
	int			  m_nSelectServer;
	int			  m_nSex		 ;
	char		  m_szId[MAX_PATH];
	int			  m_nServerCount[11];
	u_long		  m_uServerPlayerId[11][1024];

	int			  m_nDrawCount;

	CTexture m_texGauEmptyNormal ;
	CTexture m_texGauFillNormal  ;
	
	LPDIRECT3DVERTEXBUFFER9 m_pVBGauge;


// Constructors
public:
	CWndMenu m_menu;

	CRect         m_rect;
	BOOL          m_bDrag;
	DWORD         m_dwListCtrlStyle;


	void SetScrollBar();
#ifdef __RT_1025
	int		GetSelect( CPoint point, u_long & idPlayer, Friend** ppFriend );
	void	GetSelectFriend( int SelectCount, u_long & idPlayer, Friend** ppFriend );
#else	// __RT_1025
	int GetSelect( CPoint point, LPFRIEND* lppFriend );
	void	GetSelectFriend( int SelectCount, LPFRIEND* lppFriend );
#endif	// __RT_1025
	u_long GetSelectId( int SelectCount );
	
//	void InitItem( int nJob, int nExpert, LPSKILL apExpertSkill, u_long uParent = 0 );
	void SetSearch( int nSelectServer, int nSex, LPCTSTR strId );

	CWndFriendCtrl();
	~CWndFriendCtrl();

	void GetFriendName();
	void Create( DWORD m_dwListCtrlStyle, RECT& rect, CWndBase* pParentWnd, UINT nID );
	void LoadListBoxScript(LPCTSTR lpFileName); 
// Attributes
	COLORREF GetBkColor() const;
	BOOL SetBkColor(COLORREF cr);
	int GetItemCount() const;
	BOOL GetItem(LVITEM* pItem) const;
	BOOL SetItem(const LVITEM* pItem);
	BOOL SetItem(int nItem, int nSubItem, UINT nMask, LPCTSTR lpszItem,
		int nImage, UINT nState, UINT nStateMask, LPARAM lParam);

// Operations
	int InsertItem(const LVITEM* pItem);

// Overridables
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
//	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point = 0 );
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 

	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();

// Implementation
public:
	int InsertItem(UINT nMask, int nItem, LPCTSTR lpszItem, UINT nState,
		UINT nStateMask, int nImage, LPARAM lParam);

	void ScrollBarPos( int nPos );
	int	 GetDrawCount( void );

protected:
	void RemoveImageList(int nImageList);
protected:
	////{{AFX_MSG(CListCtrl)
	//afx_msg void OnNcDestroy();
	////}}AFX_MSG
	//DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_WNDSKILLCTRL_H__0B45596D_70D7_48A4_BCB2_3D0F32F58E57__INCLUDED_)

 