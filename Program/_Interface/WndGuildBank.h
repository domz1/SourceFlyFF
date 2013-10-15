#ifndef __WNDGUILDBANK__H
#define __WNDGUILDBANK__H

#include "WndGuildMerit.h"

class CWndGuildBank : public CWndNeuz 
{ 
	CWndGuildMerit* m_pwndGuildMerit;		// ±æµå°øÇå.
public: 
	CWndItemCtrl m_wndItemCtrl;
	CWndGold     m_wndGold;

public:
	CWndGuildBank(); 
	~CWndGuildBank(); 

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
};

#if __VER >= 11 // __GUILD_BANK_LOG
class CWndAddItemLog : public CWndBase
{
public:
	int m_nCurSelect;
	int m_nFontHeight;
	int m_nDrawCount;
	
	CWndScrollBar m_wndScrollBar;
	vector < CString > m_vLogList;

public:
	CWndAddItemLog();
	~CWndAddItemLog();

	void Create( RECT& rect, CWndBase* pParentWnd, UINT nID );
	void UpdateLogList();
	void UpdateScroll();

	// Overridables
	virtual void OnInitialUpdate();
	virtual void OnDraw( C2DRender* p2DRender );

	// UI Func.
	int GetDrawCount( void );
};

class CWndRemoveItemLog : public CWndBase
{
public:
	int m_nCurSelect;
	int m_nFontHeight;
	int m_nDrawCount;
	
	CWndScrollBar m_wndScrollBar;
	vector < CString > m_vLogList;

public:
	CWndRemoveItemLog();
	~CWndRemoveItemLog();

	void Create( RECT& rect, CWndBase* pParentWnd, UINT nID );
	void UpdateLogList();
	void UpdateScroll();

	// Overridables
	virtual void OnInitialUpdate();
	virtual void OnDraw( C2DRender* p2DRender );

	// UI Func.
	int GetDrawCount( void );
};

class CWndReceivePenyaLog : public CWndBase
{
public:
	int m_nCurSelect;
	int m_nFontHeight;
	int m_nDrawCount;
	
	CWndScrollBar m_wndScrollBar;
	vector < CString > m_vLogList;

public:
	CWndReceivePenyaLog();
	~CWndReceivePenyaLog();

	void Create( RECT& rect, CWndBase* pParentWnd, UINT nID );
	void UpdateLogList();
	void UpdateScroll();

	// Overridables
	virtual void OnInitialUpdate();
	virtual void OnDraw( C2DRender* p2DRender );

	// UI Func.
	int GetDrawCount( void );
};

class CWndInvestPenyaLog : public CWndBase
{
public:
	int m_nCurSelect;
	int m_nFontHeight;
	int m_nDrawCount;
	
	CWndScrollBar m_wndScrollBar;
	vector < CString > m_vLogList;

public:
	CWndInvestPenyaLog();
	~CWndInvestPenyaLog();

	void Create( RECT& rect, CWndBase* pParentWnd, UINT nID );
	void UpdateLogList();
	void UpdateScroll();

	// Overridables
	virtual void OnInitialUpdate();
	virtual void OnDraw( C2DRender* p2DRender );

	// UI Func.
	int GetDrawCount( void );
};

class CWndGuildBankLog : public CWndNeuz 
{ 
public: 
	CWndGuildBankLog(); 
	~CWndGuildBankLog(); 

	CWndAddItemLog		m_wndAddItemLog;
	CWndRemoveItemLog	m_wndRemoveItemLog;
	CWndReceivePenyaLog	m_wndReceivePenyaLog;
	CWndInvestPenyaLog	m_wndInvestPenyaLog;

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase );
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point );

	void UpdateScroll();
};
#endif //__GUILD_BANK_LOG

#endif
