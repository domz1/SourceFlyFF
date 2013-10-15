// Theme.h: interface for the CWndBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_THEME) //_H__0B45596D_70D7_48A4_BCB2_3D0F32F58E57__INCLUDED_)
#define AFX_THEME //_H__0B45596D_70D7_48A4_BCB2_3D0F32F58E57__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CWndButton;

// WallPaper Type

#define WPT_STRETCH        1
#define WPT_CENTER         2
#define WPT_CENTERSTRETCH  3
#define WPT_TILE           4

class CWndBase;

class CTheme
{
	LPDIRECT3DDEVICE9 m_pd3dDevice; 
	CMapStringToPtr m_mapFont;

#ifdef __FLYFF_INITPAGE_EXT
public:
	CWorld* m_pTitleWorld;
	BOOL m_bLoadTerrainScript;
	BOOL m_bRenderTitleWorld;

	CTexture* m_pFlyffLogo;
	CTexture* m_pGameGrade;
	CTexture* m_pAeonLogo;
	CTexture* m_pGalaLogo;

	DWORD m_dwTexturAlpha1;
	DWORD m_dwTexturAlpha2;

	DWORD m_dwStartTime;
	DWORD m_dwEndTime;
	BOOL m_bStartCameraWork;
#endif //__FLYFF_INITPAGE_EXT

public:
	void SetVersion( int Lang );
	//D3DSURFACE_DESC* m_pd3dsdBackBuffer;   // 
	//CWndBase* m_pWndMng;
	CWndBase* m_pActiveDesktop;

	CD3DFont* m_pFontText;
//	CD3DFont* m_pFontTextMin;
	CD3DFont* m_pFontGameTitle;
	CD3DFont* m_pFontWndTitle;

#if __VER >= 19
	CD3DFont* m_pFontNewNames;
#endif//__VER >= 19

#ifdef __WINDOWS_V19
	CD3DFont* m_pFontWndNewTitle;
#endif// __WINDOWS_V19

#ifdef __BUTTONS_V19
	CD3DFont* m_pFontButton;
#endif//__BUTTONS_V19

	CD3DFont* m_pFontChat;
	CD3DFont* m_pFontCaption;
	CD3DFont* m_pFontWorld;
	CD3DFont* m_pFontStatus;
	CD3DFont* m_pFontGuildCombatText;
#if __VER >= 12 // __SECRET_ROOM
	CD3DFont* m_pFontSRMyGiuld;
	CD3DFont* m_pFontSRGiuld;
#endif //__SECRET_ROOM
	//CD3DFontAPI* m_pFontAPICaption;
	//CD3DFontAPI* m_pFontAPITitle;

	LPDIRECT3DVERTEXBUFFER9 m_pVBTexture;
	LPDIRECT3DVERTEXBUFFER9 m_pVBGauge;

	// Background Color
	D3DCOLOR m_d3dcBackground     ;

	// Window
	D3DCOLOR m_d3dcWndBkgr        ;
	D3DCOLOR m_d3dcWndBolder      ;
	D3DCOLOR m_d3dcWndFrame       ;
	D3DCOLOR m_d3dcWndTitle       ;
	D3DCOLOR m_d3dcWndDeAct       ;

	// TextBox
	D3DCOLOR m_d3dcTextBkgr       ;
	D3DCOLOR m_d3dcTextBolder     ;
	D3DCOLOR m_d3dcTextFrame      ;

	// Button
	D3DCOLOR m_d3dcButtonBkgr     ;
	D3DCOLOR m_d3dcButtonBolder   ;
	D3DCOLOR m_d3dcButtonFrame    ; 
	D3DCOLOR m_d3dcFont           ;
	D3DCOLOR m_d3dcHighLightFont  ;
	D3DCOLOR m_d3dcPushFont       ;
	D3DCOLOR m_d3dcHighLightBar   ;

	// MenuItem
	D3DCOLOR m_d3dcMenuItemBkgr   ;
	D3DCOLOR m_d3dcMenuItemBolder ;
	D3DCOLOR m_d3dcMenuItemFrame  ;

	// Chat or Message
	D3DCOLOR m_d3dcChatMyFont     ;
	D3DCOLOR m_d3dcMsgFont        ;
	D3DCOLOR m_d3dcMsgFontWarning ; 
	D3DCOLOR m_d3dcMsgFontEffect  ;
	D3DCOLOR m_d3dcMsgFontNotice  ;

	// ScrollBar
	D3DCOLOR m_d3dcSBarPad;
	D3DCOLOR m_d3dcSBarStick;

	// WallPaper
	CTexture  m_texWallPaper;
	CTexture  m_texWndPaper;
	CTexture  m_FrameTexture[9];

#ifdef __GAME_GRADE_SYSTEM
	CTexture  m_GameGradeScreenTexture;
	CTexture* m_pGameGradeTexture;
#endif // __GAME_GRADE_SYSTEM
	DWORD       m_dwWallPaperType;

	BOOL m_bNudeSkin;

	CTheme();
	~CTheme();

	BOOL LoadTheme( LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR lpszFileName );
	BOOL SaveTheme( LPCTSTR lpszFileName );

	void DeleteTheme();
	void RenderTitle( C2DRender* p2DRender );
	void RenderDesktop( C2DRender* p2DRender );
#ifdef __GAME_GRADE_SYSTEM
	void RenderGameGradeScreen( C2DRender* p2DRender );
	void RenderGameGradeMark( C2DRender* p2DRender, DWORD dwAlpha );
#endif // __GAME_GRADE_SYSTEM

	HRESULT InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
	HRESULT InvalidateDeviceObjects();
	HRESULT DeleteDeviceObjects();
	HRESULT FrameMove();
	HRESULT RestoreDeviceObjects();

	// GUI 랜더에 필요한 함수들 
	void  GradationRect( C2DRender* p2DRender, CRect* pRect, DWORD dwColor1t, DWORD dwColor1b, DWORD dwColor2b, int nMidPercent = 40 );
	DWORD GetButtonFontColor( CWndButton* pWndButton );
	POINT GetButtonTextPos( C2DRender* p2DRender, CWndButton* pWndButton );

	// 윈도 윈터페이스 랜더 시작 
	void RenderEdge              ( C2DRender* p2DRender, CRect* pRect, BOOL bClient = TRUE );
	void RenderWndBaseTitleBar   ( C2DRender* p2DRender, CRect* pRect, LPCTSTR lpszTitle, DWORD dwColor = 0xffffffff );//adeilson 0xff000000
	void RenderWndBaseFrame      ( C2DRender* p2DRender, CRect* pRect );
	void RenderWndBaseBkgr       ( C2DRender* p2DRender, CRect* pRect );
	void RenderWndTextFrame      ( C2DRender* p2DRender, CRect* pRect );
	void RenderWndTextBkgr       ( C2DRender* p2DRender, CRect* pRect );
	void RenderWndEditFrame      ( C2DRender* p2DRender, CRect* pRect );
	void RenderWndEditBkgr       ( C2DRender* p2DRender, CRect* pRect );
	void RenderWndTreeFrame      ( C2DRender* p2DRender, CRect* pRect );
	void RenderWndTreeBkgr       ( C2DRender* p2DRender, CRect* pRect );
	void RenderWndListBoxFrame   ( C2DRender* p2DRender, CRect* pRect );
	void RenderWndListBoxBkgr    ( C2DRender* p2DRender, CRect* pRect );
	void RenderWndComboBoxFrame  ( C2DRender* p2DRender, CRect* pRect );
	void RenderWndComboBoxBkgr   ( C2DRender* p2DRender, CRect* pRect );
	void RenderWndComboBoxButton ( C2DRender* p2DRender, CRect* pRect );
	void RenderWndButton         ( C2DRender* p2DRender, CWndButton* pWndButton );
	void RenderWndButtonRadio    ( C2DRender* p2DRender, CWndButton* pWndButton );
	void RenderWndButtonCheck    ( C2DRender* p2DRender, CWndButton* pWndButton );
	void RenderWndButtonText     ( C2DRender* p2DRender, CWndButton* pWndButton );

	void RenderWndButton_4Texture( C2DRender* p2DRender, CWndButton* pWndButton );
	void RenderWndButton_6Texture( C2DRender* p2DRender, CWndButton* pWndButton );

	void RenderWndTaskBar        ( C2DRender* p2DRender, CRect* pRect );
	void RenderWndMenuTask       ( C2DRender* p2DRender, CRect* pRect );
	void RenderWndMenu           ( C2DRender* p2DRender, CRect* pRect );
	void RenderWndMenuItem       ( C2DRender* p2DRender, CWndButton* pWndButton );

	void RenderWndGauge          ( C2DRender* p2DRender, CRect* pRect, DWORD dwColor );
	void RenderWndGauge2         ( C2DRender* p2DRender, CRect* pRect, DWORD dwColor );

	BOOL MakeGaugeVertex         ( LPDIRECT3DDEVICE9 pd3dDevice, CRect* pRect, DWORD dwColor, LPDIRECT3DVERTEXBUFFER9 pVB, CTexture* pTexture );
	void RenderGauge             ( LPDIRECT3DDEVICE9 pd3dDevice, LPDIRECT3DVERTEXBUFFER9 pVB, CTexture* pTexture );
	void RenderGauge             ( C2DRender* p2DRender, CRect* pRect, DWORD dwColor, LPDIRECT3DVERTEXBUFFER9 pVBGauge, CTexture* pTexture );

#ifdef __FLYFF_INITPAGE_EXT
	void ReadTitleWorld();
	void DestoryTitleWorld();
#endif //__FLYFF_INITPAGE_EXT
};
#endif // !defined(AFX_THEME_H__0B45596D_70D7_48A4_BCB2_3D0F32F58E57__INCLUDED_)

