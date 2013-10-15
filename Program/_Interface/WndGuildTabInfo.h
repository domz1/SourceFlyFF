#ifndef __WNDGUILD_TAB_INFO__H
#define __WNDGUILD_TAB_INFO__H

#include "WndGuildName.h"

class CWndGuildDisMiss : public CWndNeuz 
{ 
public: 
	CWndGuildDisMiss(); 
	~CWndGuildDisMiss(); 
	
	virtual BOOL Initialize( CWndBase* pWndParent );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 


class CWndGuildNotice : public CWndNeuz 
{ 
public: 
	CWndGuildNotice(); 
	~CWndGuildNotice(); 
	
	virtual BOOL Initialize( CWndBase* pWndParent );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 

class CWndGuildSetLogo : public CWndNeuz 
{ 
public: 
	CRect    m_rect[CUSTOM_LOGO_MAX];
	int      m_nSelectLogo;
	
	CWndGuildSetLogo(); 
	~CWndGuildSetLogo(); 
	
	virtual BOOL Initialize( CWndBase* pWndParent );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 

class CWndGuildTabInfo : public CWndNeuz 
{ 
public: 
#ifdef _DEBUG
	int m_nMx, m_nMy;
#endif

	CWndGuildName*	m_pwndGuildName;
	CWndGuildNotice* m_pwndGuildNotice;
	CWndGuildSetLogo* m_pwndGuildSetLogo;
	CWndGuildDisMiss* m_pwndGuildDisMiss;
				
	CWndGuildTabInfo(); 
	~CWndGuildTabInfo(); 
	
	void UpdateData( void );
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual void OnMouseMove(UINT nFlags, CPoint point );
}; 


#endif