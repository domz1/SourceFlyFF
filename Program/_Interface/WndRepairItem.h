#ifndef __WNDREPAIRITEM__H
#define __WNDREPAIRITEM__H

#include "WndRepairItemCtrl.h"

class CWndRepairItem : public CWndNeuz 
{ 
public:
//	CWndItemCtrl		m_wndItemCtrl;
//	CItemContainer< CItemElem > pRepairItem;
	CWndStatic* pWndStaticCost;
	CWndRepairItemCtrl	m_wndItemCtrl;
	DWORD	m_adwIdRepair[MAX_REPAIRINGITEM];
	DWORD	m_dwCost;


	void OnInit( void );

public: 
	CWndRepairItem(); 
	~CWndRepairItem(); 

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual	void OnDestroy( void );
}; 
#endif	// __WNDREPAIRITEM__H
