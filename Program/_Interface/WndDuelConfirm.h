#ifndef __WNDDUELCONFIRM__H
#define __WNDDUELCONFIRM__H

class CWndDuelConfirm : public CWndNeuz 
{ 
public: 
	char	m_szSrc[ MAX_NAME ];	// 듀얼 신청자이름
	OBJID	m_idSrc;
	
	BOOL	m_bDuelParty;			// 파티 듀얼시 TRUE

	CWndDuelConfirm(); 
	~CWndDuelConfirm(); 

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 

#if __VER >= 8     // 8차 듀얼존에 관계없이 PVP가능하게함   Neuz, World
class CWndDuelResult : public CWndNeuz 
{ 
public: 
	char	m_szSrc[ MAX_NAME ];	// 듀얼 신청자이름
	OBJID	m_idSrc;
	
	BOOL	m_bDuelWin;			//  듀얼시 이겼을때TRUE

	CWndDuelResult(); 
	~CWndDuelResult(); 

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 
#endif	// __VER >= 8  

#endif
