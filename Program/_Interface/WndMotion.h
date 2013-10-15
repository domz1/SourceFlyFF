#ifndef __WNDMOTION__H
#define __WNDMOTION__H



class CWndMotion1 : public CWndBase
{
	MotionProp* m_pSelectMotion;
	int m_nSelect;
	
	CPtrArray m_motionArray;
	
public:
	CWndMotion1();
	virtual ~CWndMotion1();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL,DWORD dwWndId = 0);
	// message
	virtual BOOL Process();
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL );
	virtual BOOL OnChildNotify(UINT message,UINT nID,LRESULT* pLResult);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnMouseWndSurface( CPoint point );
	virtual void OnMouseMove(UINT nFlags, CPoint point);
};

class CWndEmoticon : public CWndBase
{
	CTexture*   m_pSelectTexture;

	int m_nSelect;
	
	CPtrArray m_emoticonArray;
	
public:
	CWndEmoticon();
	virtual ~CWndEmoticon();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL,DWORD dwWndId = 0);
	// message
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL );
	virtual BOOL OnChildNotify(UINT message,UINT nID,LRESULT* pLResult);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnMouseWndSurface( CPoint point );
	virtual void OnMouseMove(UINT nFlags, CPoint point);
};

class CWndMotion : public CWndNeuz 
{ 
public: 
	CWndMotion1		m_wndMotion1;
	CWndEmoticon    m_wndEmoticon;

	CWndMotion(); 
	~CWndMotion(); 
	virtual void OnMouseWndSurface( CPoint point );
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual void OnMouseMove(UINT nFlags, CPoint point);
}; 





#endif
