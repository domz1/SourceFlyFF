// WndOption.h: interface for the CWndNeuz class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WNDOPTION_H__A93F3186_63D6_43C1_956F_EC8691E0C7D9__INCLUDED_)
#define AFX_WNDOPTION_H__A93F3186_63D6_43C1_956F_EC8691E0C7D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WndOptionGame.h"	 // 게임 옵션

class CWndOption : public CWndNeuz 
{ 
public: 
	CWndOption(); 
	~CWndOption(); 

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 

//////////////////////////////////////////////////////////////////////////////////////
// Sound
//

class CWndOptSound : public CWndNeuz 
{ 
public: 
	CTexture         m_Texture;
	CTexture         m_TexturePt;
	BOOL			 m_bLButtonClick;
	BOOL			 m_bLButtonClick2;
	int				 m_nStep[2];
	CWndOptSound(); 
	~CWndOptSound(); 

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual void OnMouseMove(UINT nFlags, CPoint point);

	void GetRangeSlider( DWORD dwWndId, int& nStep, CPoint point );
	int GetSliderStep( DWORD dwWndId, int &nStep, CPoint point );
	CPoint GetStepPos( int nStep, int nWidth, int nDivision );

	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	
}; 
class CWndOptVideo : public CWndNeuz 
{ 
public: 
	void GetRangeSlider( DWORD dwWndId, int& nStep, CPoint point );
	int GetSliderStep( DWORD dwWndId, int &nStep, CPoint point );
	void GetRangeSlider( DWORD dwWndId, int& nStep, CPoint point, int nDivision);
	int GetSliderStep( DWORD dwWndId, int &nStep, CPoint point, int nDivision);
	CPoint GetStepPos( int nStep, int nWidth, int nDivision );
	CWndOptVideo(); 
	~CWndOptVideo(); 

#ifdef __OPTION_EX
	CWndButton* pWndButton1[ 2 ];
	CWndButton* pWndButton2[ 2 ];
	CWndButton* pWndButton3[ 2 ];
	CWndButton* pWndButton4[ 2 ];
	CWndButton* pWndButton5[ 2 ];
	CWndButton* pWndButton6[ 2 ];
	CWndButton* pWndButton7[ 2 ];
	CWndButton* pWndButton8[ 2 ];
	CWndButton* pWndButton9[ 2 ];
	CWndButton* pWndButton10[ 2 ];
	CWndButton* pWndButton11[ 2 ];
	CWndButton* pWndButton12[ 2 ];
#endif//__OPTION_EX
#if __VER >= 8 //__Y_GAMMA_CONTROL_8	
	int				 m_nStep[5];
	int				 m_nBrightTable[11];

	BOOL			 m_bLButtonClick3;
	BOOL			 m_bLButtonClick4;
	BOOL			 m_bLButtonClick5;
#else //__Y_GAMMA_CONTROL_8
	int				 m_nStep[2];
#endif //__Y_GAMMA_CONTROL_8

	CTexture         m_Texture;
	CTexture         m_TexturePt;
	BOOL			 m_bLButtonClick;
	BOOL			 m_bLButtonClick2;
	virtual void OnMouseMove(UINT nFlags, CPoint point);
	virtual void OnMouseWndSurface( CPoint point );
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	
	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
}; 



class CWndOptMyInfo : public CWndNeuz 
{ 
public: 
	CWndOptMyInfo(); 
	~CWndOptMyInfo(); 

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 

class CWndOptWindow : public CWndNeuz 
{ 
public: 
	CWndOptWindow(); 
	~CWndOptWindow(); 

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 


class CWndTotalOption : public CWndNeuz
{
public:
	CWndOptVideo		m_OptTabVideoSnd;
	CWndOptionGame		m_OptTabGame;
#if __VER < 12 // __UPDATE_OPT
	CWndOption			m_OptTabEtc;
	CWndOptSound		m_OptTabSound;
#endif
	CWndTotalOption(); 
	~CWndTotalOption(); 

	void UpdateDataAll();

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
};

//BEGIN_WNDCLASS( CWndOptVideo )
//END_WNDCLASS

//BEGIN_WNDCLASS( CWndOptTheme )
//	CWndButton    m_wndPaper1;
//	CWndButton    m_wndPaper2;
//	CWndButton    m_wndPaper3;
//	CWndButton    m_wndPaper4;
//	CWndButton    m_wndPaper5;
//END_WNDCLASS

//BEGIN_WNDCLASS( CWndOptSound )
//END_WNDCLASS

//BEGIN_WNDCLASS( CWndOptMusic )
//END_WNDCLASS


//BEGIN_WNDCLASS( CWndOptMouse )
//END_WNDCLASS

//BEGIN_WNDCLASS( CWndOptHotkey )
//END_WNDCLASS

/*
class CWndSound : public CWndNeuz
{
	CWndButton m_wndMusic;
	CWndButton m_wndSound;
public:
	CWndSound(); 
	virtual ~CWndSound();
//	virtual CItem* GetFocusItem() { return NULL; }
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL,DWORD dwWndId = 0);
	// message
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase );
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
};

*/
#endif // !defined(AFX_WNDOPTION_H__A93F3186_63D6_43C1_956F_EC8691E0C7D9__INCLUDED_)
