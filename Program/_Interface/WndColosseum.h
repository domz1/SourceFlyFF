#ifndef __WNDCOLOSSEUM__H
#define __WNDCOLOSSEUM__H

#ifdef	__COLOSSEUM

class CWndColosseumComplete : public CWndNeuz 
{ 
public: 
	CWndColosseumComplete(); 
	~CWndColosseumComplete(); 
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 

	time_t			m_nTime;
	CTimeSpan		m_ct;
}; 

class CWndColosseumEnter : public CWndNeuz 
{ 
public: 
	CWndColosseumEnter(); 
	~CWndColosseumEnter(); 
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual BOOL Process ();

	BYTE	byDifficult;
	CWndButton*		m_pWndButton[4];
}; 

class CWndColosseumMassage : public CWndNeuz 
{ 
public: 
	CWndColosseumMassage(); 
	~CWndColosseumMassage(); 
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 

	TCHAR m_tPartyLeader[MAX_NAME];

}; 

class CWndColosseumRankGuild : public CWndNeuz 
{ 
public: 
	CWndColosseumRankGuild(); 
	~CWndColosseumRankGuild(); 
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
}; 

class CWndColosseumRankNomal : public CWndNeuz 
{ 
public: 
	CWndColosseumRankNomal(); 
	~CWndColosseumRankNomal(); 
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
}; 

class CWndColosseumRetry : public CWndNeuz 
{ 
public: 
	CWndColosseumRetry(); 
	~CWndColosseumRetry(); 
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
}; 

class CWndColosseumStart : public CWndNeuz 
{ 
public: 
	CWndColosseumStart(); 
	~CWndColosseumStart(); 
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual BOOL Process();
	time_t			m_nTime;
	CTimeSpan		m_ct;
}; 

class CWndColosseumStartReady1 : public CWndNeuz 
{ 
public: 
	CWndColosseumStartReady1(); 
	~CWndColosseumStartReady1(); 
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual BOOL Process();

	time_t m_nTime;
	BYTE m_nStage;
	CTimeSpan		m_ct;
}; 

class CWndColosseumStartReady2 : public CWndNeuz 
{ 
public: 
	CWndColosseumStartReady2(); 
	~CWndColosseumStartReady2(); 
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual BOOL Process();

	time_t m_nTime;
	CTimeSpan		m_ct;

}; 





#endif //__COLOSSEUM

#endif //__WNDCOLOSSEUM_H