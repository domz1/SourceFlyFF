#ifndef __WNDINVENREMOVEITEM__H
#define __WNDINVENREMOVEITEM__H

class CWndInvenRemoveItem : public CWndNeuz 
{ 
public: 
	CItemElem*		m_pItemElem;
	LPWNDCTRL		m_pWndItemCtrl;
	CWndEdit*		m_pWndEditNum;

	int				m_nRemoveNum;
	void InitItem( CItemElem* pItemElem );
	BOOL OnButtonOK( void );

	CWndInvenRemoveItem(); 
	~CWndInvenRemoveItem(); 

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual void OnMouseWndSurface( CPoint point );
}; 
#endif
