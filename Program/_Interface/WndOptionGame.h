#ifndef __WNDOPTIONGAME__H
#define __WNDOPTIONGAME__H

class CWndOptionGame : public CWndNeuz 
{ 
public: 
	CWndOptionGame(); 
	~CWndOptionGame(); 


	CWndButton* pWndButton1[ 2 ];
	CWndButton* pWndButton2[ 2 ];
	CWndButton* pWndButton3[ 2 ];
	CWndButton* pWndButton4[ 2 ];
	CWndButton* pWndButton5[ 2 ];
	CWndButton* pWndButton6[ 2 ];
	CWndButton* pWndButton7[ 2 ];
	CWndButton* pWndButton8[ 2 ];
	CWndButton* pWndButton9[ 2 ];
	CWndButton* pWndButton10[ 2 ];
	CWndButton* pWndButton11[ 2 ];
	CWndButton* pWndButton12[ 2 ];
	CWndButton* pWndHelp;
	CWndButton* pWndCheck;
	CWndButton* pWndAlpha;
	CWndButton* pWndCheckBattleBGM;
#ifdef __GAME_GRADE_SYSTEM
	CWndButton* pWndCheckGameGrade;
#endif // __GAME_GRADE_SYSTEM

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
#if __VER >= 12 // __UPDATE_OPT
	CTexture         m_Texture;
	CTexture         m_TexturePt;
	BOOL			 m_bLButtonClick;
	BOOL			 m_bLButtonClick2;
	int				 m_nStep[2];

	virtual void OnMouseMove(UINT nFlags, CPoint point);

	void GetRangeSlider( DWORD dwWndId, int& nStep, CPoint point );
	int GetSliderStep( DWORD dwWndId, int &nStep, CPoint point );
	CPoint GetStepPos( int nStep, int nWidth, int nDivision );

	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
#endif
}; 
#endif
