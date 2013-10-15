#ifndef __WNDGUILDWARREQUEST__H
#define __WNDGUILDWARREQUEST__H

class CWndGuildWarRequest : public CWndNeuz 
{ 
	DWORD m_idEnemyGuild;
	char m_szMaster[MAX_PLAYER];
public: 
	CWndGuildWarRequest(); 
	~CWndGuildWarRequest(); 

	// Initialize하기전에 적길드아이디랑 적길마 이름을 받아둠.
	void	Set( DWORD idEnemyGuild, LPCTSTR szMaster ) 
	{ 
		m_idEnemyGuild = idEnemyGuild; 
		strcpy( m_szMaster, szMaster );
	}

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
