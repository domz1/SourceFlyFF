#ifndef __WNDSELECTVILLAGE__H
#define __WNDSELECTVILLAGE__H


class CReturnScrollMsgBox : public CWndMessageBox
{ 
private:
	DWORD m_dwItemId;
public: 
	void SetItemID( DWORD dwItemId ) { m_dwItemId = dwItemId; }
	DWORD GetItemID() { return m_dwItemId; }
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD dwWndId = 0 );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
}; 

class CWndSelectVillage : public CWndNeuz 
{ 
public: 
	CWndSelectVillage(); 
	~CWndSelectVillage(); 

	DWORD	m_dwItemId;

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 

#endif	// __WNDSELECTVILLAGE__H

