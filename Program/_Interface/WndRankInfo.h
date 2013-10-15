#ifndef __WNDRANKINFO__H
#define __WNDRANKINFO__H

class CWndRankInfoTabLevel : public CWndNeuz 
{ 
public: 
	CWndRankInfoTabLevel(); 
	~CWndRankInfoTabLevel(); 
	
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

class CWndRankInfoTabPlayTime : public CWndNeuz 
{ 
public: 
	CWndRankInfoTabPlayTime(); 
	~CWndRankInfoTabPlayTime(); 

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

class CWndRankInfo : public CWndNeuz 
{ 
public: 
	CWndRankInfo(); 
	~CWndRankInfo(); 
	
	CWndRankInfoTabLevel	m_WndRankInfoTabLevel;
	CWndRankInfoTabPlayTime m_WndRankInfoTabPlayTime;

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

