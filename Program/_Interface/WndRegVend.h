#ifndef __WNDREGVEND__H
#define __WNDREGVEND__H

typedef struct CALC_DATA
{
	int  m_nValue;
	char m_ch;
} CALC_DATA;


class CWndRegVend : public CWndNeuz
{
private:
	CWndEdit*	m_pWndEdit;
	
	DWORD		m_dwOldFocus;

	CItemBase*	m_pItemBase;
	int		m_iIndex;
public:
	CWndRegVend();
	~CWndRegVend();

//	Operations
	void	SetItem( int iIndex, CItemBase* pItemBase );

	virtual	BOOL	Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual	BOOL	OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual	void	OnDraw( C2DRender* p2DRender );
	virtual	void	OnInitialUpdate();
	virtual	BOOL	OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase );
	virtual	void	OnSize( UINT nType, int cx, int cy );
	virtual	void	OnLButtonUp( UINT nFlags, CPoint point );
	virtual	void	OnLButtonDown( UINT nFlags, CPoint point );

	DWORD			m_dwFocus;
	CPoint			m_pt[2];
	BOOL	        m_bIsFirst;
	CALC_DATA       m_Calc;
	virtual	void	OnChar(UINT nChar);
	int				ProcessCalc( CALC_DATA calc, int num );
	CTexture*		m_pTex;
};

#endif