#ifndef __WNDFRIENDCONFIRM__H
#define __WNDFRIENDCONFIRM__H

class CWndFriendConFirm : public CWndNeuz 
{ 
public:
	u_long m_uLeader, m_uMember;
	BYTE m_nLeaderSex;
	LONG m_nLeaderJob;
	TCHAR m_szLeaderName[ MAX_NAME ];

public:
	void SetMember( u_long uLeader, u_long uMember, LONG nLeaderJob, BYTE nLeaderSex, char * szLeadName );
public: 
	CWndFriendConFirm(); 
	~CWndFriendConFirm(); 

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 

class CWndAddFriend : public CWndNeuz 
{ 
public: 
	CWndAddFriend(); 
	~CWndAddFriend(); 
	
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
