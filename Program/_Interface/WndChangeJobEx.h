//Dem Projekt Neuz hinzufügen


#ifndef __WND_JOBCHANGE___EX_
#define __WND_JOBCHANGE___EX_
class CWndJobChangeEx: public CWndNeuz
{
public:
	CWndJobChangeEx( );
	~CWndJobChangeEx( );

	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();

public:
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	/*virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); */
private:
	int newJob;
	int nJobMin;
	int nJobMax;
	int nCurJob;
	bool bInitialized;
};
#endif 


