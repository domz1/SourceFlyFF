#ifndef __COLLECTINGWND__H
#define __COLLECTINGWND__H

class CCollectingWnd : public CWndNeuz 
{ 
public: 
	CCollectingWnd(); 
	~CCollectingWnd(); 

	bool		m_bIsCollecting;
	
	CModelObject	*m_pModel; // ÄÝ·ºÅÍ ¸ðµ¨
	int				m_nDisplay;
	CItemElem		*m_pElem;
	D3DXMATRIX		m_matModel;
	float			m_fAngle;
	CSfxModel		*m_pSfx;
	CSfxBase		*m_pSfxBase;
	CRect			m_BetteryRect;
	CRect			m_LevelRect;
	CTexture		*m_pTexLevel, *m_pTexBatt;
	CTexture		*m_pTexGauEmptyNormal;
	//CTexture		m_texGauEmptySmall;
	CTexture		*m_pTexGauFillNormal;
	//CTexture		m_texGauFillSmall;
	char			m_pbufText[24];
	LPDIRECT3DVERTEXBUFFER9 m_pVBGauge;
	BOOL 			m_bVBGauge;
	int 			m_nGWidth;

	virtual	BOOL Process();
	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual void OnDestroy();
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual BOOL OnEraseBkgnd(C2DRender* p2DRender);
	virtual void OnMouseWndSurface( CPoint point );
	void	Update();
	BOOL	SetButtonCaption(bool bIsStart);
	bool	IsChangeState();

	void	AddSfx();
	void	DeleteSfx();
	
}; 
#endif
