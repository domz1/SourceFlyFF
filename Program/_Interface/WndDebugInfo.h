// WndArcane.h: interface for the CWndNeuz class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WNDDEBUGINFO_H__A93F3186_63D6_43C1_956F_EC8691E0C7D9__INCLUDED_)
#define AFX_WNDDEBUGINFO_H__A93F3186_63D6_43C1_956F_EC8691E0C7D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

BEGIN_WNDCLASS( CWndCapture )
	CWndEdit      m_wndEdit;
	CWndButton    m_wndYes;
	CWndButton    m_wndNo;
	CWndButton    m_wndCancel;
END_WNDCLASS

//////////////////////////////////////////////////////////////////////////////////////
// DebugInfo
//
class CWndDebugInfo : public CWndNeuz
{
	/*
	CWndButton m_wndExtend;
	//CWndButton m_wndDebug[ 30 ];
	CWndEdit   m_wndEditExp;
	CWndButton m_wndButtonExp;

	CWndButton m_wndSetSkinSet;
	CWndButton m_wndSetHair;
	CWndButton m_wndSetColor;
	CWndButton m_wndSetFace;
	
	CWndButton m_wndViewGrid;
	CWndButton m_wndViewWireframe;
	CWndButton m_wndViewSkybox;
	CWndButton m_wndViewTerrain;
	CWndButton m_wndViewObject;
	CWndButton m_wndViewFog;
	CWndButton m_wndViewLOD;
	CWndButton m_wndViewWater;
	CWndButton m_wndViewName;
*/
//	CWndCapture* m_pWndCapture;

public:
	CWndDebugInfo(); 
	virtual ~CWndDebugInfo();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL,DWORD dwWndId = 0);
	// message
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL );
	virtual void OnSize( UINT nType, int cx, int cy );
	virtual void OnLButtonUp( UINT nFlags, CPoint point );
	virtual void OnLButtonDown( UINT nFlags, CPoint point );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual void OnDestroyChildWnd( CWndBase* pWndChild );

//friend CWndCapture;
};
// Control 테스트 윈도 

class CWndControlTest : public CWndNeuz
{
	CWndText m_wndText;
	CWndButton m_wndMusic;
	CWndButton m_wndSound;
	CWndComboBox m_wndComboBox;
	CWndListCtrl m_wndlistCtrl;
	CWndButton   m_wndButtonIcon;
	CWndButton   m_wndButtonReport;
	CWndButton   m_wndButtonList;
public:
	CWndControlTest(); 
	virtual ~CWndControlTest();
//	virtual CItem* GetFocusItem() { return NULL; }
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL,DWORD dwWndId = 0);
	// message
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL );
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
};


#endif // !defined(AFX_WNDDEBUGINFO_H__A93F3186_63D6_43C1_956F_EC8691E0C7D9__INCLUDED_)
