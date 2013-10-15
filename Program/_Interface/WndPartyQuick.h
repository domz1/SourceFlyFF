#if __VER >= 8 //__CSC_VER8_2

#ifndef __WNDPARTY_QUICK__H
#define __WNDPARTY_QUICK__H

class CWndParty;

class CWndPartyQuick : public CWndNeuz 
{ 
public:
	CWndButton* m_pBtnParty;
	CWndStatic* m_pWndMemberStatic[8];
	CTexture m_texGauEmptyNormal;
	CTexture m_texGauFillNormal;

	LPDIRECT3DVERTEXBUFFER9 m_pVBGauge;

	int m_MemberCount;
	CWndParty* m_pWndParty;
	CMover* m_pFocusMember;	
public: 

	CWndPartyQuick(); 
	~CWndPartyQuick(); 
	
	virtual void SerializeRegInfo( CAr& ar, DWORD& dwVersion );
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point);
	virtual void OnRButtonDblClk( UINT nFlags, CPoint point);
	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();	

	void SetActiveMember(int MemberNum);
}; 

#endif //__WNDPARTY_QUICK__H

#endif //__CSC_VER8_2