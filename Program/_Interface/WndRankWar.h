#ifndef __WNDRANKWAR__H
#define __WNDRANKWAR__H

class CWndRankWarTabGiveUp : public CWndNeuz 
{ 
public: 
	CWndRankWarTabGiveUp(); 
	~CWndRankWarTabGiveUp(); 
	
	int		m_nCurrentList;			// 출력될 멤버리스트의 시작 인덱스.
	int		m_nMxOld, m_nMyOld;			// 과거 좌표.
	
	virtual BOOL OnMouseWheel( UINT nFlags, short zDelta, CPoint pt );
	virtual void OnMouseMove(UINT nFlags, CPoint point );
	virtual void OnRButtonDown( UINT nFlags, CPoint point );
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 

class CWndRankWarTabLose : public CWndNeuz 
{ 
public: 
	CWndRankWarTabLose(); 
	~CWndRankWarTabLose(); 

	int		m_nCurrentList;			// 출력될 멤버리스트의 시작 인덱스.
//	int		m_nSelect;
	int		m_nMxOld, m_nMyOld;			// 과거 좌표.
	
	virtual BOOL OnMouseWheel( UINT nFlags, short zDelta, CPoint pt );
	virtual void OnMouseMove(UINT nFlags, CPoint point );
	virtual void OnRButtonDown( UINT nFlags, CPoint point );
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 

class CWndRankWarTabWin : public CWndNeuz 
{ 
public: 
	CWndRankWarTabWin(); 
	~CWndRankWarTabWin(); 

	int		m_nCurrentList;			// 출력될 멤버리스트의 시작 인덱스.
//	int		m_nSelect;
	int		m_nMxOld, m_nMyOld;			// 과거 좌표.
	
	virtual BOOL OnMouseWheel( UINT nFlags, short zDelta, CPoint pt );
	virtual void OnMouseMove(UINT nFlags, CPoint point );
	virtual void OnRButtonDown( UINT nFlags, CPoint point );
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 

class CWndRankWar : public CWndNeuz 
{ 
public: 
	CWndRankWar(); 
	~CWndRankWar(); 

	CWndRankWarTabGiveUp	m_WndRankWarTabGiveUp;
	CWndRankWarTabLose		m_WndRankWarTabLose;
	CWndRankWarTabWin		m_WndRankWarTabWin;	

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
