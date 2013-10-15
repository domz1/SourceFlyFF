#if __VER >= 12 // __SECRET_ROOM

#ifndef __WNDGUILDSECRETROOM__H
#define __WNDGUILDSECRETROOM__H

//////////////////////////////////////////////////////////////////////////
// Secret Room 
//////////////////////////////////////////////////////////////////////////

class CWndSecretRoomSelection : public CWndNeuz
{
protected:	
	multimap<int, CGuildMember*>	m_mapSelectPlayer;   // 정렬된 길드원 리스트

	vector<u_long>					m_vecGuildList   ;   // 길드원 리스트
	vector<u_long>					m_vecSelectPlayer;   // 참가자 리스트
	
public: 
	void Reset();
	CWndSecretRoomSelection();
	virtual ~CWndSecretRoomSelection();
	
	virtual	BOOL	Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual	BOOL	OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual	void	OnDraw( C2DRender* p2DRender );
	virtual	void	OnInitialUpdate();
	virtual	BOOL	OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase );
	virtual	void	OnSize( UINT nType, int cx, int cy );
	virtual void	OnLButtonUp( UINT nFlags, CPoint point );
	virtual	void	OnLButtonDown( UINT nFlags, CPoint point );
	void			EnableFinish( BOOL bFlag );		

	void			UpDateGuildListBox();

	void			AddCombatPlayer( u_long uiPlayer );
	void			AddGuildPlayer( u_long uiPlayer );

	void			RemoveCombatPlayer( int nIndex ) ;
	void			RemoveGuildPlayer( int nIndex ) ;

	u_long			FindCombatPlayer( u_long uiPlayer );
	u_long			FindGuildPlayer( u_long uiPlayer );

//	void			SetMaster();
}; 

class CWndSecretRoomOffer : public CWndNeuz
{
protected:
	DWORD			m_dwReqGold;
	DWORD			m_dwMinGold;
	DWORD			m_dwBackupGold;
	
public: 
	CWndSecretRoomOffer();
	virtual ~CWndSecretRoomOffer();
	
	virtual	BOOL	Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual	BOOL	OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual	void	OnDraw( C2DRender* p2DRender );
	virtual	void	OnInitialUpdate();
	virtual	BOOL	OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase );
	virtual	void	OnSize( UINT nType, int cx, int cy );
	virtual void	OnLButtonUp( UINT nFlags, CPoint point );
	virtual	void	OnLButtonDown( UINT nFlags, CPoint point );
	void			SetGold( DWORD nCost );
	void			SetMinGold( DWORD dwMinGold )		{ m_dwMinGold    = dwMinGold; }
	void			SetReqGold( DWORD dwReqGold )		{ m_dwReqGold    = dwReqGold; }
	void			SetBackupGold( DWORD dwBackupGold ) { m_dwBackupGold = dwBackupGold; }
	void			EnableAccept( BOOL bFlag );	
}; 

class CWndSecretRoomOfferState : public CWndNeuz 
{ 
private: 
	CTimeSpan		m_ct;
	time_t    		m_tEndTime;		// timegettime+
	time_t    		m_tCurrentTime; // timegettime+
	
public:
	CWndSecretRoomOfferState();
	virtual ~CWndSecretRoomOfferState(); 
	
	void		 InsertTitle( const char szTitle[] );
	int          GetSelectIndex( const CPoint& point );	
	void		 Init( time_t lTime );
	void		 InsertGuild( const char szGuild[], const char szName[], int nNum );	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual	void OnRButtonUp( UINT nFlags, CPoint point );
	virtual void OnRButtonDown( UINT nFlags, CPoint point );
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point);	
	virtual BOOL Process ();
	void		 SetGold( int nGold );
	void		 SetRate( int nRate );
	void	 	 SetTime( time_t tTime ) { m_tCurrentTime = 0; m_tEndTime = time_null() + tTime; }
};

class CWndSecretRoomBoard : public CWndNeuz
{
public: 
	CWndSecretRoomBoard();
	virtual ~CWndSecretRoomBoard();
	
	virtual	BOOL	Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual	BOOL	OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual	void	OnDraw( C2DRender* p2DRender );
	virtual	void	OnInitialUpdate();
	virtual	BOOL	OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase );
	virtual	void	OnSize( UINT nType, int cx, int cy );
	virtual void	OnLButtonUp( UINT nFlags, CPoint point );
	virtual	void	OnLButtonDown( UINT nFlags, CPoint point );
	void			SetString();
}; 
/*
class CWndSecretRoomGuildMember : public CWndNeuz
{
public:
	BOOL m_bSelect;
	u_long m_uGuildMemberId;
	CTexture m_texGauHPEmptyNormal;
	CTexture m_texGauHPFillNormal;

	LPDIRECT3DVERTEXBUFFER9 m_pVBHPGauge;
public:
	CWndSecretRoomGuildMember();
	~CWndSecretRoomGuildMember();

	virtual	BOOL	Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual	BOOL	OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual	void	OnDraw( C2DRender* p2DRender );
	virtual	void	OnInitialUpdate();
	virtual void	OnLButtonUp( UINT nFlags, CPoint point );
	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();
	virtual void	OnDestroy();
	virtual void	OnMouseWndSurface( CPoint point );

	void SetSelectColor(BOOL bSet) {m_bSelect = bSet;};
};

#define MAX_SECRETROOM_MEMBER 11

typedef struct __SECRETROOM_GUILDMEMBER
{
	u_long m_uGuildMemberId;
	CWndSecretRoomGuildMember* m_pWndGuildMember;
} __SECRETROOM_GUILDMEMBER;

class CWndSecretRoomGuildMemMng : public CWndNeuz
{
private:
	CWndListBox* m_pWndListBox;
	int m_nGuildMemCount;
	CWndSecretRoomGuildMember* m_pWndSelectMember;
	CWndSecretRoomGuildMember* m_pWndLastMember;
	vector<__SECRETROOM_GUILDMEMBER> m_vecGuildMember;

public:
	CWndSecretRoomGuildMemMng();
	~CWndSecretRoomGuildMemMng();

	virtual	BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual	BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual	void OnDraw( C2DRender* p2DRender );
	virtual	void OnInitialUpdate();

	void SetGuildMemCount(int nGuildMemCount) {m_nGuildMemCount = nGuildMemCount;};
	void SetGuildMember(u_long uGuildMemberId);
	void SetSelect(CWndSecretRoomGuildMember* pWndSelectMember);
	BOOL IsSelect(CWndSecretRoomGuildMember* pWndMember);
	void DestroyMemberWnd(u_long uMember);
	CWndSecretRoomGuildMember* GetBeforeWnd();
	void SetVisibleMng(BOOL bVisible);
};
*/

#define MAX_SECRETROOM_MEMBER 11

class CWndSecretRoomQuick : public CWndNeuz 
{ 
public:
	int m_StaticID[MAX_SECRETROOM_MEMBER];
	CWndStatic* m_pWndMemberStatic[MAX_SECRETROOM_MEMBER];
	CTexture m_texGauEmptyNormal;
	CTexture m_texGauFillNormal;

	LPDIRECT3DVERTEXBUFFER9 m_pVBGauge;

	vector<u_long> m_vecGuildMemberId;
	u_long m_FocusMemberid;
	int m_MemberCount;
	int m_nWndHeight;
	BOOL m_bMini;
public: 

	CWndSecretRoomQuick(); 
	~CWndSecretRoomQuick(); 
	
	virtual void SerializeRegInfo( CAr& ar, DWORD& dwVersion );
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();	

	void SetActiveMember(int MemberNum);
	void SetGuildMemCount(int nGuildMemCount) {m_MemberCount = nGuildMemCount;};
	void SetGuildMember(u_long uGuildMemberId);
	void SortMemberList();
};

class CWndSecretRoomChangeTaxRate : public CWndNeuz 
{
public:
	int m_nDefaultSalesTax;
	int m_nDefaultPurchaseTax;
	int m_nChangeSalesTax;
	int m_nChangePurchaseTax;
	int m_nMinTax;
	int m_nMaxTax;

	BYTE m_nCont;
	
public:
	CWndSecretRoomChangeTaxRate();
	virtual ~CWndSecretRoomChangeTaxRate(); 
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate();

	void SetDefaultTax(int nMinTax, int nMaxTax, BYTE nCont);
}; 

class CWndSecretRoomCheckTaxRate : public CWndNeuz 
{ 
public:
	int m_nSalesTax;
	int m_nPurchaseTax;
	DWORD m_dwGuildId;

public:
	CWndSecretRoomCheckTaxRate();
	virtual ~CWndSecretRoomCheckTaxRate(); 
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate();
}; 

//////////////////////////////////////////////////////////////////////////
// Message Box Class
//////////////////////////////////////////////////////////////////////////

class CWndSecretRoomSelectionResetConfirm : public CWndMessageBox
{ 
public: 
	CString m_strMsg;
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD dwWndId = 0 );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
}; 

class CWndSecretRoomOfferMessageBox : public CWndMessageBox
{ 
public: 
	DWORD m_nCost;
	void	SetValue( CString str, DWORD nCost );
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD dwWndId = 0 );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
}; 

class CWndSecretRoomInfoMsgBox : public CWndNeuz
{
public:
	CWndSecretRoomInfoMsgBox();

	virtual ~CWndSecretRoomInfoMsgBox();
	void	SetString( CHAR* szChar );
	virtual	void OnInitialUpdate();
	virtual	BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );

	void	SetString( CString strMsg );
};

class CWndSecretRoomChangeTaxRateMsgBox : public CWndMessageBox
{ 
public: 
	int m_nSalesTax;
	int m_nPurchaseTax;
	BYTE m_nCont;

public:
	void	SetValue( CString str, int nSalesTax, int nPurchaseTax, BYTE nCont );
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD dwWndId = 0 );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
};

class CWndSecretRoomCancelConfirm : public CWndNeuz
{
public:
	CWndSecretRoomCancelConfirm();
	virtual ~CWndSecretRoomCancelConfirm();
	
	virtual	BOOL	Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual	BOOL	OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual	void	OnDraw( C2DRender* p2DRender );
	virtual	void	OnInitialUpdate();
	virtual	BOOL	OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase );
	virtual	void	OnSize( UINT nType, int cx, int cy );
	virtual void	OnLButtonUp( UINT nFlags, CPoint point );
	virtual	void	OnLButtonDown( UINT nFlags, CPoint point );
};

#endif //__WNDGUILDSECRETROOM__H
#endif //__SECRET_ROOM