#ifndef __WNDGUILDVOTE__H
#define __WNDGUILDVOTE__H



class CWndGuildVoteSeting;

class CWndGuildVote : public CWndNeuz 
{ 
public: 
	void SelChange( CGuild* pGuild, int nIndex );
	void SetszString(char *text1, char *text2, char *text3, char *text4 );
	void SetCount(int ncount1, int ncount2, int ncount3, int ncount4 );

	void SetQuestion( const char* question );
	void SetRadioGroup( int ncount );
	CWndGuildVoteSeting* m_pWndGuildVoteSeting;

	CWndGuildVote(); 
	~CWndGuildVote(); 
	
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

class CWndGuildVoteSeting : public CWndNeuz 
{ 
public: 
	int m_nSelect;
	CWndGuildVoteSeting(); 
	~CWndGuildVoteSeting(); 
	
	virtual BOOL Initialize( CWndBase* pWndParent );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 


#endif