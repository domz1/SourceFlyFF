#ifndef __WNDLVREQDOWN__H
#define __WNDLVREQDOWN__H

class CWndLvReqDown : public CWndNeuz 
{ 
public: 

	CWndText* m_pText;
	CItemElem* m_pItemElem;
	ItemProp* m_pEItemProp;
	CTexture* m_pTexture;

	CWndLvReqDown(); 
	~CWndLvReqDown(); 

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual void OnDestroy();
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point );
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point );

	void SetDescription();
}; 
#endif
