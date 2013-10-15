#ifndef __WNDGUILDTABWAR__H
#define __WNDGUILDTABWAR__H

#include "WndGuildWarDecl.h"
#include "WndGuildWarGiveUp.h"
#include "WndGuildWarPeace.h"

class CWndGuildTabWar : public CWndNeuz 
{ 
	CWndGuildWarDecl*	m_pWndGuildWarDecl;
	CWndGuildWarGiveUp*	m_pWndGuildWarGiveUp;
	CWndGuildWarPeace*	m_pWndGuildWarPeace;
	
public: 
	

	CWndGuildTabWar(); 
	~CWndGuildTabWar(); 

	void UpdateData( void );
	
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
