#ifndef __WNDPARTYCONFIRM__H
#define __WNDPARTYCONFIRM__H

class CWndPartyConfirm : public CWndNeuz 
{ 
public: 
	u_long m_uLeader, m_uMember;
	LONG m_nLeaderLevel, m_nMemberLevel;
	LONG m_nLeaderJob, m_nMemberJob;
	DWORD m_dwLeaderSex, m_dwMemberSex;
	BOOL bTroup;

	TCHAR m_szLeaderName[ MAX_NAME ];
	TCHAR m_szMemberName[ MAX_NAME ];
	void SetMember( u_long uLeader, LONG nLLevel, LONG nLJob, DWORD dwLSex, u_long uMember, LONG nMLevel, LONG nMJob, DWORD dwMSex, char * szLName, BOOL bTroupbuf );
	CWndPartyConfirm(); 
	~CWndPartyConfirm(); 

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
