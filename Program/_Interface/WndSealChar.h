#ifndef __WNDSEALCHAR__H
#define __WNDSEALCHAR__H

class CWndSealChar : public CWndNeuz 
{ 
public: 
	
	CWndSealChar(); 
	~CWndSealChar(); 

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 

class CWndSealCharSelect : public CWndNeuz 
{ 
public: 
	char	m_szSrc1[ MAX_NAME ];	// 이름
	OBJID	m_idSrc1;
	char	m_szSrc2[ MAX_NAME ];	// 이름
	OBJID	m_idSrc2;
	LONG	m_lPlayerSlot1;
	LONG	m_lPlayerSlot2;
	short	m_sCount;
	
	CWndSealCharSelect(); 
	~CWndSealCharSelect(); 

	void	SetData( short sCount,LONG lPlayerSolt1,LONG lPlayerSolt2,u_long uPlayerID1,u_long uPlayerID2, char* szName1, char* szName2 );
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 

class CWndSealCharSend : public CWndNeuz 
{ 
public: 
	char	m_szSrc1[ MAX_NAME ];	// 이름
	OBJID	m_idSrc1;
	
	CWndSealCharSend(); 
	~CWndSealCharSend(); 

	void	SetData( u_long uPlayerID1, char* szName1 );
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 

class CWndSealCharSet : public CWndNeuz 
{ 
public: 
	char	m_szSrc1[ MAX_NAME ];	// 이름
	OBJID	m_idSrc1;
	DWORD	m_dwData;

	CWndSealCharSet(); 
	~CWndSealCharSet(); 

	void SetData( DWORD dwId, WORD wReset );
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
