#ifndef __WNDGUILD__H
#define __WNDGUILD__H

#include "WndGuildTabApp.h"
#include "WndGuildTabInfo.h"
#include "WndGuildTabMember.h"
#include "WndGuildTabWar.h"

#if __VER >= 15 // __GUILD_HOUSE
#include "WndGuildTabPower.h"
#endif


class CWndGuild : public CWndNeuz 
{ 
public: 
	CWndGuild(); 
	~CWndGuild(); 

	CWndGuildTabApp     m_WndGuildTabApp;
	CWndGuildTabInfo    m_WndGuildTabInfo;
	CWndGuildTabMember  m_WndGuildTabMember;
	CWndGuildTabWar		m_WndGuildTabWar;

#if __VER >= 15 // __GUILD_HOUSE
	CWndGuildTabPower m_WndGuildTabPower;

	void SetCurTab( int index );		//선택된 탭 변경 
#endif

	void UpdateDataAll();

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


class CWndGuildConfirm : public CWndNeuz 
{ 
public: 
	CWndGuildConfirm(); 
	~CWndGuildConfirm(); 

	u_long	m_idMaster;
	CString strGuildName;
	void	SetGuildName( char* pGuildName );
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
