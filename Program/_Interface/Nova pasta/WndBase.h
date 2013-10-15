// WndBase.h: interface for the CWndBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WNDBASE_H__0B45596D_70D7_48A4_BCB2_3D0F32F58E57__INCLUDED_)
#define AFX_WNDBASE_H__0B45596D_70D7_48A4_BCB2_3D0F32F58E57__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
/*
#ifdef __CLIENT
#define CLIENT_WIDTH  ( g_Neuz.m_d3dsdBackBuffer.Width )
#define CLIENT_HEIGHT ( g_Neuz.m_d3dsdBackBuffer.Height )
#define RECT_CLIENT   CRect( 0, 0, CLIENT_WIDTH, CLIENT_HEIGHT )
#endif
*/

#define LAYOUT_NONE    0
#define LAYOUT_RULERS  1
#define LAYOUT_GRID    2

#define WSIZE_WINDOW   0
#define WSIZE_MIN      1
#define WSIZE_MAX      2

#include "WndStyle.h"

#define D3DCOLOR_TEMP(a,b,g,r) \
    ((D3DCOLOR)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff))) 

// 클라이언트의 좌표 얻기 
#define GET_CLIENT_POINT( hwnd, pt ) CPoint pt; ::GetCursorPos( &pt ); ::ScreenToClient( hwnd, &pt );

// 좌표를 맞추기 위한 다이얼로그 : 사용하면 아주 유용함 
#define COORDI_DLG(c,x1,y1,x2,y2)	static CCoordiDlg c(x1,y1,x2,y2); if(c.m_hWnd == NULL) c.Create(CCoordiDlg::IDD);

class C2DRender;

#include "ResManager.h"

//_CHILD   
class CWndBase;
 
typedef struct tagWNDMESSAGE
{
	CWndBase* m_pWndBase ;
	UINT      m_message  ;
	WPARAM    m_wParam   ;
	LPARAM    m_lParam   ;
} WNDMESSAGE,* LPWNDMESSAGE;

//////////////////////////////////////////////////////////////////////////////
// CWndBase 
//////////////////////////////////////////////////////////////////////////////

class CWndBase  
{
	LPDIRECT3DVERTEXBUFFER9 m_pVB;

static CPoint    m_pointOld;
static CRect     m_rectOld;

public:
static CWndBase* m_pWndCapture;
	//BOOL      m_bCapture;
	bool      m_bPush; // 포커스 윈도가 눌렸나?
	int       m_nResizeDir;

//oid AddWnd(CWndBase* pWnd); 
	//void RemoveWnd(CWndBase* pWnd);
	void DestroyAllWnd(CWndBase* pWndRoot); // 모든 윈도를 강제 삭제 ; 종료할때 호출 
	void SetChildFocus( CWndBase* pWndBase, POINT point );
	CWndBase* GetChildFocus( CWndBase* pWndBase, POINT point );
//protected:
public:
static CResManager m_resMng;
static CMapStringToPtr m_strWndTileMap;
	CD3DApplication* m_pApp;
	CTheme*   m_pTheme;
	CWndBase* m_pParentWnd;
	CPtrArray m_wndArray;
	UINT      m_nIdWnd;
	bool      m_bVisible;
	CSize     m_sizeTile;
	DWORD     m_dwStyle;
	bool      m_bLButtonUp;
	bool      m_bLButtonDown;
	bool      m_bRButtonDown;
	bool      m_bMButtonDown;
	bool	  m_bLButtonDowned;
	CString   m_strTitle;
	CString   m_strToolTip;
	C2DRender*   m_p2DRender;
	CRect     m_rectCurrentWindow;
	CRect     m_rectCurrentClient;
	CRect     m_rectWindow ; // Window ; 패어런트부터의 좌표 
	CRect     m_rectClient ; // Client ; 패어런트부터의 좌표 ( 보통 에찌 때문에 m_rectWindow 보다는 조금 안쪽 좌표를 갖는다. )
	CRect     m_rectLayout ; // Client ; 패어런트부터의 좌표 ( 보통 에찌 때문에 m_rectWindow 보다는 조금 안쪽 좌표를 갖는다. )
	CPoint    m_ptMouse;
	BOOL      m_bGroup;
	CD3DFont* m_pFont;
	bool      m_bAutoFree;
	bool      m_bEnable;
	CWndBase* m_pWndFocusChild;
	bool      m_bTabStop;
	bool      m_bDefault;
	bool      m_bActive;
	int       m_nToolTipPos;
	BYTE      m_byWndType;
	BOOL      m_bKeyButton;
	DWORD     m_dwColor;
	int       m_nAlphaCount;
	DWORD     m_dwALPHA[10];
	bool      m_bTile;
	CString   m_strTexture;
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	BOOL      m_bNoCloseButton;
#endif // __IMPROVE_QUEST_INTERFACE

static BOOL           m_bCling          ;
static BOOL           m_bEdit           ;
static BOOL           m_bFullWnd        ;
static CWndBase*      m_pWndRoot        ;
static CWndBase*      m_pWndFocus       ; // 다이얼로그 윈도 포커스 
static CWndBase*      m_pCurFocus       ; // 다이얼로그, 차일드 중 최종 현재 포커스 

#ifndef __VS2003
static CPtrArray      m_wndOrder        ;
#endif

static CPtrArray      m_wndRemove       ;
static CPtrArray      m_postMessage     ;
//static CTexturePack   m_texturePack     ;
static CTexture*      m_pTexForbid;
static CTimer         m_timerForbid;
static BOOL           m_bForbid;
static CPoint         m_ptForbid;
static HWND           m_hWnd;
static HCURSOR        m_hDefaultCursor;
static CWndBase*      m_pWndOnMouseMove;
static CWndBase*      m_pWndOnSetCursor;
static int            m_nAlpha;
static CTheme         m_Theme;

	static BOOL SetForbidTexture( LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR lpszFileName );
	static void SetForbid( BOOL bForbid );
	static BOOL IsForbid() { return m_pTexForbid && m_bForbid; }

	CTexture*  m_pTexture;
//	CString m_strTexture;

	BOOL IsPush() { return m_bPush; }
	CRect GetScreenRect();

public:
static CTextureMng    m_textureMng;
static SHORTCUT       m_GlobalShortcut;
	BOOL m_bPickup; // 포커스 윈도를 들었나?

#if __VER >= 19
	BOOL m_bPickup_SLIDE;
#endif // __VER >= 19

	void AddWnd(CWndBase* pWnd); 
	void RemoveWnd(CWndBase* pWnd);

//	CString m_strSndEffect;
	int       m_nWinSize; // 0 = nomal, 1 - minimaize, 2 = maximize

	void FitTextureSize();
static void FreeTileTexture();
	
	CWndBase();
	virtual ~CWndBase();
	BOOL Create(DWORD dwStyle,const RECT& rect,CWndBase* pParentWnd,UINT nID);
	void SetTexture( LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR lpszFileName, BOOL bMyLoader = FALSE );
	void SetTexture( LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR lpKey, CTexture* m_pTexture );

	void RemoveDestroyWnd();
	void MoveParentCenter(); // 윈도를 부모 윈도의 중앙으로 옮긴다.
	void MoveRectCenter(CRect rect); // 윈도를 rect의 중앙으로 옮긴다.
	void MovePointCenter(CPoint pt); // 윈도를 pt의 중앙으로 옮긴다.
	void Move( CPoint pt ); // 왼도의 left,top을 pt로 
	void Move( int x, int y ) { Move( CPoint( x, y ) ); } // 왼도의 left,top을 pt로 
	BOOL IsVisible() { return m_bVisible; } // 윈도가 보이는 상태인가.
	void SetVisible(BOOL bVisible) { m_bVisible = ( bVisible != FALSE ); } // 윈도를 보이거나 감춘다.
	void SetCapture();
	void ReleaseCapture();
	CRect  MakeCenterRect( int nWidth, int nHeight );
	CPoint GetClientCenter(); // Client의 중앙 좌표를 돌려준다.
	CPoint GetStrCenter(C2DRender* p2DRender,LPCTSTR str); // 화면 중심에서 스트링의 중앙에 출력하기 위한 좌표를 돌려준다.
	CPoint GetStrRectCenter(C2DRender* p2DRender,CRect rect,LPCTSTR str); // rect의 중심에서 스트링의 중앙에 출력하기 위한 좌표를 돌려준다.
	CString GetTitle() { return m_strTitle; } 
	void SetTitle(CString strTitle) { m_strTitle = strTitle; }
	DWORD GetStyle() { return m_dwStyle; }
	CWndBase* GetParentWnd() { return m_pParentWnd; }
	CRect GetClientRect( BOOL bParent = FALSE );
	CRect GetWindowRect( BOOL bParent = FALSE );
	CRect GetLayoutRect( BOOL bParent = FALSE );
	CRect GetWndRect() { return m_rectWindow; }
	BOOL IsWndRoot() { return this == m_pWndRoot; }
	BOOL IsOnWndBase(CPoint pt);  // 포인트가 윈도 위에 있는가?
#ifndef __VS2003
	BOOL IsOpenAnywnd() { return m_wndOrder.GetSize() ? TRUE : FALSE; }
#endif
	BOOL IsOpenModalWnd();
	CWndBase* GetFocusChild() { return m_pWndFocusChild; }
	CWndBase* GetFocusWnd() { return m_pWndFocus; }
	void GetLogFont(C2DRender* p2DRender,LOGFONT* pLogFont);
	int  GetFontHeight();
	BOOL IsWndStyle(DWORD dwStyle) { return (m_dwStyle & dwStyle) ? TRUE : FALSE; }
	int  GetWndStyle() { return m_dwStyle; }
	void SetWndStyle(DWORD dwStyle) { m_dwStyle = dwStyle; }
	int  GetWndId() { return m_nIdWnd; }
	void SetWndId( DWORD dwWndId ) { m_nIdWnd = dwWndId; }
	void AddWndStyle(DWORD dwStyle) { m_dwStyle |= dwStyle; }
	void DelWndStyle(DWORD dwStyle) { m_dwStyle &= ~dwStyle; }
	CWndBase* GetWndBase( UINT idWnd );
	CWndBase* GetWndBase();
	CWndBase* FindWnd(UINT idWnd) { return GetWndBase( idWnd ); }
	CWndBase* GetChildWnd( UINT nID );
	CWndBase* GetDlgItem( UINT nID ) 
	{ 
		CWndBase *pWnd = GetChildWnd( nID );
		if( pWnd == NULL )
			Error( "GetDlgItem : nID=%d not Found.", nID );
		return pWnd; 
	}
	BOOL IsOpenWnd(UINT nId);
	BOOL IsOpenWnd(CWndBase* pWnd);
	BOOL IsOpenWnd() { return m_nIdWnd ? TRUE : FALSE; }
	BOOL IsFocusWnd() { return m_pWndFocus == this; }
	BOOL IsFocusChild() { return m_pParentWnd ? m_pParentWnd->m_pWndFocusChild == this : FALSE; }
	CPoint GetMousePoint() { return m_ptMouse; } 
	void SetGroup(BOOL bGroup) { m_bGroup = bGroup; }
	BOOL IsGroup() { return m_bGroup; }
	CPtrArray* GetWndArray() { return &m_wndArray; }
	void SetFont(CD3DFont* pFont) { m_pFont = pFont; }
	CD3DFont* GetFont() { return m_pFont; }
	void SetAutoFree(BOOL bFree) { m_bAutoFree = (bFree != FALSE ); }
	BOOL IsAutoFree() { return m_bAutoFree; }
	BOOL IsParentWnd(CWndBase* pParentWnd);
	BOOL IsWindowEnabled() const { return m_bEnable; }
	void EnableWindow(BOOL bEnable = TRUE) { m_bEnable = (bEnable != FALSE ); }
	LRESULT SendMessage(UINT message,WPARAM wParam = 0, LPARAM lParam = 0);
	BOOL PostMessage(UINT message,WPARAM wParam = 0, LPARAM lParam = 0);
	void  SetToolTip(LPCTSTR lpszToolTip,int nPos = 0) { m_strToolTip = lpszToolTip; m_nToolTipPos = nPos; }
	void SetTabStop(BOOL bTabStop) { m_bTabStop = (bTabStop != FALSE); }
	virtual void SetFocus();
	void KillFocus( CWndBase* pWndFocus, CWndBase* pWndNew );
	void SetDefault(BOOL bDefault) { m_bDefault = (bDefault != FALSE); }
	BOOL IsDefault() { return m_bDefault; }
	BOOL IsFullWnd() { return m_bFullWnd; }
	CWndBase* FindFullWnd();
	void GradationRect( C2DRender* p2DRender, CRect rect, DWORD dwColor1t, DWORD dwColor1b, DWORD dwColor2b, int nMidPercent = 40 );
	BOOL WindowToScreen( LPPOINT lpPoint );
	BOOL WindowToScreen( LPRECT lpRect   );
	BOOL ClientToScreen( LPPOINT lpPoint );
	BOOL ClientToScreen( LPRECT lpRect   );
	BOOL ScreenToWindow( LPPOINT lpPoint );
	BOOL ScreenToWindow( LPRECT lpRect   );
	BOOL ScreenToClient( LPPOINT lpPoint );
	BOOL ScreenToClient( LPRECT lpRect   );
	
	CWndBase* GetFrameWnd();
	BOOL AdjustMinRect( CRect* pRect, int nWidth, int nHeight );
	BOOL AdjustMaxRect( CRect* pRect, int nWidth, int nHeight );

	void SetWndType( int nWndType) { m_byWndType = (BYTE)nWndType; }
	DWORD GetWndType() { return (DWORD)m_byWndType; }
	LPWNDCTRL GetWndCtrl( DWORD dwWndId ) { return m_resMng.GetAtControl( m_nIdWnd, dwWndId ); }
	LPWNDAPPLET GetWndApplet() { return m_resMng.GetAt( m_nIdWnd ); }

static void ClipStrArray(C2DRender* p2DRender,CRect rect,int nLineSpace,
						 CStringArray* pStringArray,CStringArray* pNewStringArray);

virtual void AlighWindow( CRect rcOld, CRect rcNew );
 
	BOOL IsDestroy();
	void Destroy(BOOL bAutoFree = FALSE); // 윈도를 파괴한다.
	void PaintChild(C2DRender* p2DRender);
	void Paint(C2DRender* p2DRender, BOOL bPaintChild = TRUE );
	void SetWndSize( int cx, int cy );
	virtual void OnMouseWndSurface( CPoint point );
	virtual void PaintRoot( C2DRender* p2DRender );
	virtual	void PaintFrame( C2DRender* p2DRender );
	virtual	void SetWndRect( CRect rectWnd, BOOL bOnSize = TRUE);
	virtual int  GetResizeDir(CPoint ptClient);
	virtual BOOL IsPickupSpace(CPoint point); 
	virtual	BOOL Process();
	virtual	void OnDraw(C2DRender* p2DRender);
	virtual	BOOL OnDrawIcon(CWndBase* pWndBase,C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual	void OnUpdate();
	virtual void OnDestroyChildWnd( CWndBase* pWndChild );
	virtual BOOL OnSetCursor( CWndBase* pWndBase, UINT nHitTest, UINT message );
	// message
//Protected:
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnRButtonUp(UINT nFlags, CPoint point);
	virtual void OnRButtonDown(UINT nFlags, CPoint point);
	virtual void OnMButtonUp(UINT nFlags, CPoint point);
	virtual void OnMButtonDown(UINT nFlags, CPoint point);
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point);
	virtual void OnRButtonDblClk( UINT nFlags, CPoint point);
	virtual void OnMButtonDblClk( UINT nFlags, CPoint point);
	virtual void OnNonClientLButtonDblClk( UINT nFlags, CPoint point);
	virtual void OnMouseMove(UINT nFlags, CPoint point);
	virtual BOOL OnMouseWheel( UINT nFlags, short zDelta, CPoint pt );
	virtual void OnChar(UINT nChar);
	virtual void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnPaint();
	virtual void OnClose();
	virtual void OnDestroy();
	virtual void OnTimer(UINT nIDEvent);
	virtual void OnMove(int x,int y);
	virtual void OnSetFocus(CWndBase* pOldWnd);
	virtual void OnKillFocus(CWndBase* pNewWnd);
	virtual BOOL OnEraseBkgnd(C2DRender* p2DRender);
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point = 0 );

public:
	virtual BOOL OnSystemNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual BOOL OnParentNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual BOOL OnChildNotify( UINT nCode, UINT nID, LRESULT* pLResult );
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL );
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD dwWndId = 0);
	///virtual LRESULT WndMsgProc( UINT message, WPARAM wParam, LPARAM lParam );

	LRESULT WindowRootProc( UINT message, WPARAM wParam, LPARAM lParam );
#ifdef __V050823_JAPAN_ATOK
	virtual LRESULT WindowProc(UINT message,WPARAM wParam,LPARAM lParam);
#else
	LRESULT WindowProc(UINT message,WPARAM wParam,LPARAM lParam);
#endif
	virtual LRESULT DefWindowProc(UINT message,WPARAM wParam,LPARAM lParam);

	//virtual HRESULT OneTimeSceneInit();
	virtual HRESULT InitDeviceObjects();
	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();
	//virtual HRESULT FinalCleanup() { return S_OK; }
	//virtual HRESULT Render(); 
	//virtual HRESULT FrameMove();
	virtual void AfterSkinTexture( LPWORD pDest, CSize size, D3DFORMAT d3dFormat = D3DFMT_A4R4G4B4 );
	virtual void AdditionalSkinTexture( LPWORD pDest, CSize size, D3DFORMAT d3dFormat = D3DFMT_A4R4G4B4 );
	virtual void AdjustWndBase( D3DFORMAT d3dFormat = D3DFMT_A4R4G4B4 );

	void MakeVertexBuffer();
	void RenderWnd();
	
friend class CWndButton;
};
#endif // !defined(AFX_WNDBASE_H__0B45596D_70D7_48A4_BCB2_3D0F32F58E57__INCLUDED_)

