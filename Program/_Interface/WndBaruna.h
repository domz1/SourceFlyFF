#ifndef __WNDBARUNA__H
#define __WNDBARUNA__H

#ifdef __BARUNA_V18

class CWndBarunaElement : public CWndNeuz 
{
public:
	
	float m_nSmeltWidth;
	LPDIRECT3DVERTEXBUFFER9 m_pVBSmeltGauge;
	CTexture m_texGauEmptyNormal;
	BOOL m_bVBSmeltGauge;
	BOOL m_bStart;
	DWORD m_dwStartTime;
	DWORD m_dwEndTime;

	int m_nDelay;
	float m_fGaugeRate;

	CWndBarunaElement(); 
	virtual ~CWndBarunaElement(); 
	
	CItemElem * m_pItemElem[4];
	CTexture* m_pTexture[4];
	CRect m_nCtrlId[4];

	virtual BOOL    OnDropIcon( LPSHORTCUT pShortcut, CPoint point );

	virtual BOOL	Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL	OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void	OnDraw( C2DRender* p2DRender ); 
	virtual	void	OnInitialUpdate(); 
	virtual void	OnLButtonDblClk( UINT nFlags, CPoint point ); 
	virtual	void	OnDestroy( void );
	virtual BOOL	Process();
	
	HRESULT RestoreDeviceObjects();
	HRESULT InvalidateDeviceObjects();
	HRESULT DeleteDeviceObjects();

	void SetSwitch()
	{
		if( m_pItemElem[0]->GetExtra() > 0 && m_pItemElem[1]->GetExtra() > 0 && m_pItemElem[0]->m_nResistAbilityOption < 10 )
		{
			m_bStart=TRUE;
			m_dwStartTime = g_tmCurrent;
			m_dwEndTime = g_tmCurrent + SEC(m_nDelay);
		}
	}

}; 

class CWndBarunaWakeUp : public CWndNeuz 
{
public:
	
	float m_nSmeltWidth;
	LPDIRECT3DVERTEXBUFFER9 m_pVBSmeltGauge;
	CTexture m_texGauEmptyNormal;
	BOOL m_bVBSmeltGauge;
	BOOL m_bStart;
	DWORD m_dwStartTime;
	DWORD m_dwEndTime;

	int m_nDelay;
	float m_fGaugeRate;

	CWndBarunaWakeUp(); 
	virtual ~CWndBarunaWakeUp(); 
	
	CItemElem * m_pItemElem;
	CTexture* m_pTexture;
	CRect m_nCtrlId;

	virtual BOOL    OnDropIcon( LPSHORTCUT pShortcut, CPoint point );

	virtual BOOL	Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL	OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void	OnDraw( C2DRender* p2DRender ); 
	virtual	void	OnInitialUpdate(); 
	virtual void	OnLButtonDblClk( UINT nFlags, CPoint point ); 
	virtual	void	OnDestroy( void );
	virtual BOOL	Process();
	
	HRESULT RestoreDeviceObjects();
	HRESULT InvalidateDeviceObjects();
	HRESULT DeleteDeviceObjects();

}; 

class CWndBarunaWakeUpCancel : public CWndNeuz 
{
public:
	
	float m_nSmeltWidth;
	LPDIRECT3DVERTEXBUFFER9 m_pVBSmeltGauge;
	CTexture m_texGauEmptyNormal;
	BOOL m_bVBSmeltGauge;
	BOOL m_bStart;
	DWORD m_dwStartTime;
	DWORD m_dwEndTime;

	int m_nDelay;
	float m_fGaugeRate;

	CWndBarunaWakeUpCancel(); 
	virtual ~CWndBarunaWakeUpCancel(); 
	
	CItemElem * m_pItemElem[2];
	CTexture* m_pTexture[2];
	CRect m_nCtrlId[2];

	virtual BOOL    OnDropIcon( LPSHORTCUT pShortcut, CPoint point );

	virtual BOOL	Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL	OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void	OnDraw( C2DRender* p2DRender ); 
	virtual	void	OnInitialUpdate(); 
	virtual void	OnLButtonDblClk( UINT nFlags, CPoint point ); 
	virtual	void	OnDestroy( void );
	virtual BOOL	Process();
	
	HRESULT RestoreDeviceObjects();
	HRESULT InvalidateDeviceObjects();
	HRESULT DeleteDeviceObjects();

}; 
#endif//__BARUNA_V18

#endif