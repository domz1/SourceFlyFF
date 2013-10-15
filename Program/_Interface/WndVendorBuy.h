#ifndef __WNDVENDORBUY__H
#define __WNDVENDORBUY__H

#include "WndRegVend.h"

class CWndVendorBuy : public CWndNeuz
{
	CItemBase*	m_pItemBase;
	int	m_iIndex;
public:
	CWndVendorBuy( CItemBase* pItemBase, int iIndex );
	~CWndVendorBuy();

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual void OnDraw( C2DRender* p2DRender );
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase );
	virtual void OnSize( UINT nType, int cx, int cy );
	virtual void OnLButtonUp( UINT nFlags, CPoint point );
	virtual void OnLButtonDown( UINT nFlags, CPoint point );
	DWORD			m_dwFocus;
	CPoint			m_pt[2];
	BOOL	        m_bIsFirst;
	CALC_DATA       m_Calc;
	virtual	void	OnChar(UINT nChar);
	int				ProcessCalc( CALC_DATA calc, int num );
};

#endif
