#if __VER >= 8 //__CSC_VER8_5

#ifndef __WNDSUMMONANGEL__H
#define __WNDSUMMONANGEL__H

struct GENMATDIEINFO{
	LPWNDCTRL wndCtrl;
	BOOL isUse;
	int staticNum;
	CItemElem* pItemElem;
};

struct ItemCountSet{
	OBJID itemid;
	int extracount;
};

#define MAX_MATDIE	20
#define ITEM_VALID			0
#define ITEM_MAX_OVERFLOW	1
#define ITEM_INVALID		2

class CWndSummonAngel : public CWndNeuz 
{ 
public:
	CWndText* m_pText;
	CWndStatic* m_pStatic[3];
	GENMATDIEINFO m_MatDie[MAX_MATDIE];
	ItemCountSet m_ItemInfo[MAX_MATDIE];

	int m_nitemcount;
	int m_nSelecCtrl;
	int m_nOrichalcum;
	int m_nMoonstone;

	float m_WhiteAngelRate;
	float m_RedAngelRate;
	float m_BlueAngelRate;
	float m_GreenAngelRate;

	BOOL m_nowStarting;
//	BOOL m_isCreateSuccess;
//	CString m_CreateAngel;
	
	CWndInventory* m_pWndInventory;
public: 
	
	CWndSummonAngel(); 
	~CWndSummonAngel(); 
	
	virtual void SerializeRegInfo( CAr& ar, DWORD& dwVersion );
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point );
	virtual void OnRButtonDblClk( UINT nFlags, CPoint point );
	virtual	void OnMouseMove( UINT nFlags, CPoint point );
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point );
	virtual void OnDestroy( void );

	void SetQuestText( CHAR* szChar );
	int HitTest( CPoint point );
	void SummonRateRefresh();
	void ReFreshAll(BOOL extracheck);
//	void CreateAngelIs(BOOL isSuccess, char* createAngel);
//	void SummonAngel();
	void SetDie(CItemElem* pItemElem);
}; 

#endif

#endif //__CSC_VER8_5
