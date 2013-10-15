#ifndef __WNDPETRES__H
#define __WNDPETRES__H

#ifdef __JEFF_11
class CWndPetRes : public CWndNeuz 
{ 
public: 
	CItemElem* m_pItemElem;
	ItemProp* m_pEItemProp;
	CTexture*  m_pTexture;

	CWndPetRes(); 
	~CWndPetRes();

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
	
}; 
#endif
#endif