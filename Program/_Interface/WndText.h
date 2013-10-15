#ifndef __WNDTEXT__H
#define __WNDTEXT__H

class CWndTextQuest : public CWndMessageBox
{ 
public: 
	CItemBase* m_pItemBase;

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD dwWndId = 0 );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
}; 

class CWndTextBook : public CWndNeuz 
{ 
public: 
	CItemBase* m_pItemBase;

	CWndTextBook(); 
	~CWndTextBook(); 

	void SetItemBase( CItemBase* pItemBase );

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, CItemBase* pItemBase = NULL );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 

class CWndTextScroll : public CWndNeuz 
{ 
public: 
	CItemBase* m_pItemBase;

	CWndTextScroll(); 
	~CWndTextScroll(); 

	void SetItemBase( CItemBase* pItemBase );

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, CItemBase* pItemBase = NULL );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 

class CWndTextLetter : public CWndNeuz 
{ 
public: 
	CItemBase* m_pItemBase;

	CWndTextLetter(); 
	~CWndTextLetter(); 

	void SetItemBase( CItemBase* pItemBase );

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, CItemBase* pItemBase = NULL );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 
#endif
