#ifndef __WNDCREDIT__H
#define __WNDCREDIT__H

class CWndCredit : public CWndNeuz 
{ 
	void LoadScreenShot();
public: 
	float   m_fOldMusicVolume;
	CTimer m_Starttimer;
	CSound* m_pSound;
	BOOL   m_bPlayVoice;
	CWndCredit(); 
	~CWndCredit(); 
	
	CStringArray m_strArray;
	int m_nLine;
	CTimer m_timer;
	CString m_strWord;
	CPoint m_ptWord;
	int m_nStyle;
	int m_nParam1;
	int m_nParam2;
	int m_nParam3;

	int m_nTexCount;
	CTexture m_texScreenShot;
	CTexture m_aTexScreenShot[18];
	CTimer m_timerScreenShot;
	
	CD3DFont* m_pFont;

	virtual	BOOL Process();
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual void SetWndRect( CRect rectWnd, BOOL bOnSize );
	
	virtual HRESULT InitDeviceObjects();
	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();
	virtual	void	OnDestroy( void );
	
}; 

class CWndAbout : public CWndNeuz 
{ 
public: 
	CD3DFont* m_pFont;
	CWndAbout(); 
	~CWndAbout(); 

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual HRESULT InitDeviceObjects();
	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();
	virtual void OnDestroy();
}; 
#endif
