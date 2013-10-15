#ifndef __WND__H
#define __WND__H

#if __VER >= 11 // __SYS_IDENTIFY
class CWndAwakening : public CWndNeuz 
{ 
public: 

	CWndText*  m_pText;
	CItemElem* m_pItemElem;
	ItemProp*  m_pEItemProp;
	CTexture*  m_pTexture;

	CWndAwakening(); 
	~CWndAwakening(); 

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual void OnDestroy();
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point );
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point );
	virtual BOOL process();

	void SetDescription();
}; 
#endif

#ifdef __PROTECT_AWAKE
class CWndSelectAwakeCase : public CWndNeuz
{
	// 각성 보호 선택창 ( 두가지중에 하나 고름 < 각성전, 각성후 > 0
public:

	CWndSelectAwakeCase( );
	virtual ~CWndSelectAwakeCase( );

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
//	virtual void OnDestroy();
//	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
//	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
//	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point );
	virtual BOOL process();
	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();

	void SetItemIndex( const DWORD index )			{ m_dwItemIndex = index; }
	void SetData( BYTE byObjID, DWORD dwSerialNum, __int64 n64NewOption );
	void OutputOptionString( C2DRender* p2DRender, CItemElem* pItemElem, BOOL bNew = FALSE );

protected:
	DWORD m_dwOldTime;
	DWORD m_dwDeltaTime;

	DWORD m_dwItemIndex;
	CTexture*  m_pTexture;

	BYTE m_byObjID;
	DWORD m_dwSerialNum;
	__int64 m_n64NewOption;

	CTexture* m_pTexGuage;
	LPDIRECT3DVERTEXBUFFER9 m_pVertexBufferGauge;
};

#endif //__PROTECT_AWAKE

#endif
