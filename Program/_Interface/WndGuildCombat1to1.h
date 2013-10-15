#if __VER >= 11 // __GUILD_COMBAT_1TO1

#ifndef __WNDGUILDCOMBAT1TO1__H
#define __WNDGUILDCOMBAT1TO1__H

//////////////////////////////////////////////////////////////////////////
// 1:1 GuildCombat Class
//////////////////////////////////////////////////////////////////////////

class CWndGuildCombat1to1Selection : public CWndNeuz
{
protected:	
	multimap<int, CGuildMember*>	m_mapSelectPlayer;   // 정렬된 길드원 리스트

	vector<u_long>					m_vecGuildList   ;   // 길드원 리스트
	vector<u_long>					m_vecSelectPlayer;   // 참가자 리스트

//	u_long							m_uidDefender;
//	CTexture						m_TexDefender;
//	int								m_nDefenderIndex;
	
//	int								nMaxJoinMember;
//	int								nMaxWarMember;
	
public: 
	void Reset();
	CWndGuildCombat1to1Selection();
	virtual ~CWndGuildCombat1to1Selection();
	
	virtual	BOOL	Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual	BOOL	OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual	void	OnDraw( C2DRender* p2DRender );
	virtual	void	OnInitialUpdate();
	virtual	BOOL	OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase );
	virtual	void	OnSize( UINT nType, int cx, int cy );
	virtual void	OnLButtonUp( UINT nFlags, CPoint point );
	virtual	void	OnLButtonDown( UINT nFlags, CPoint point );
	void			EnableFinish( BOOL bFlag );		

//	void			SetDefender( u_long uiPlayer );
	void			UpDateGuildListBox();

	void			AddCombatPlayer( u_long uiPlayer );
	void			AddGuildPlayer( u_long uiPlayer );

	void			RemoveCombatPlayer( int nIndex ) ;
	void			RemoveGuildPlayer( int nIndex ) ;

	u_long			FindCombatPlayer( u_long uiPlayer );
	u_long			FindGuildPlayer( u_long uiPlayer );

//	void			SetMemberSize( int nMaxJoin,  int nMaxWar );
}; 

class CWndGuildCombat1to1Offer : public CWndNeuz
{
protected:
	DWORD			m_dwReqGold;
	DWORD			m_dwMinGold;
	DWORD			m_dwBackupGold;
	
public:
	int				m_nCombatType; // 0 : 길드대전 , 1 : 1:1길드대전

public: 
	CWndGuildCombat1to1Offer(int nCombatType);
	virtual ~CWndGuildCombat1to1Offer();
	
	virtual	BOOL	Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual	BOOL	OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual	void	OnDraw( C2DRender* p2DRender );
	virtual	void	OnInitialUpdate();
	virtual	BOOL	OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase );
	virtual	void	OnSize( UINT nType, int cx, int cy );
	virtual void	OnLButtonUp( UINT nFlags, CPoint point );
	virtual	void	OnLButtonDown( UINT nFlags, CPoint point );
	virtual void	PaintFrame( C2DRender* p2DRender );
	void			SetGold( DWORD nCost );
//	void			SetTotalGold( __int64 nCost );
	void			SetMinGold( DWORD dwMinGold )		{ m_dwMinGold    = dwMinGold; }
	void			SetReqGold( DWORD dwReqGold )		{ m_dwReqGold    = dwReqGold; }
	void			SetBackupGold( DWORD dwBackupGold ) { m_dwBackupGold = dwBackupGold; }
	void			EnableAccept( BOOL bFlag );	
}; 

//////////////////////////////////////////////////////////////////////////
// Message Box Class
//////////////////////////////////////////////////////////////////////////

class CGuildCombat1to1SelectionResetConfirm : public CWndMessageBox
{ 
public: 
	CString m_strMsg;
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD dwWndId = 0 );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
}; 

class CWndGuildCombat1to1OfferMessageBox : public CWndMessageBox
{ 
public: 
	DWORD m_nCost;
	void	SetValue( CString str, DWORD nCost );
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD dwWndId = 0 );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
}; 
#endif //__WNDGUILDCOMBAT1TO1__H
#endif //__GUILD_COMBAT_1TO1