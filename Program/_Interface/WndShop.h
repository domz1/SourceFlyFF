#ifndef __SHOP_H
#define __SHOP_H

class CWndItemCtrlVendor : public CWndItemCtrl
{
public:
	CWndItemCtrlVendor();
	~CWndItemCtrlVendor();
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point = 0 );

};
class CWndConfirmBuy : public CWndNeuz 
{ 
public: 
	CWndConfirmBuy(); 
	~CWndConfirmBuy(); 

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 

private:
	void		OnOK();
	void		OnChangeBuyCount( DWORD dwBuy );

public: 
	CMover*			m_pMover;
	CItemElem*		m_pItemElem;
	DWORD			m_dwItemId;
	CWndEdit*		m_pEdit;
	CWndStatic*		m_pStatic;
	CWndStatic*		m_pStaticGold;
#if __VER >= 11 // __CSC_VER11_3
	int				m_nBuyType;
#endif //__CSC_VER11_3
//
	int				m_nTotalCost;
//
};

class CWndWarning : public CWndNeuz 
{ 
public: 
	CMover* m_pMover;
	CItemElem* m_pItemElem;
	CWndWarning(); 
	~CWndWarning(); 
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
}; 

class CWndConfirmSell : public CWndNeuz 
{ 
public: 
	CMover* m_pMover;
	CItemElem* m_pItemElem;
	CWndEdit* m_pEdit;
	CWndStatic* m_pStatic;
	CWndStatic* m_pStaticGold;
	CWndConfirmSell(); 
	~CWndConfirmSell(); 

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
#if __VER >= 14 // __DROP_CONFIRM_BUG
	virtual BOOL Process( void );
#endif // __DROP_CONFIRM_BUG
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 
class CWndShop : public CWndNeuz 
{ 

public:
	CWndConfirmSell* m_pWndConfirmSell;
	CWndWarning*	m_pWndWarning;
//
	CWndConfirmBuy* m_pWndBuy;
//
public: 
	BOOL m_bSexSort;
	BOOL m_bLevelSort;

	CMover* m_pMover;
	CWndItemCtrlVendor m_wndItemCtrl[ MAX_VENDOR_INVENTORY_TAB ];
	CWndStatic m_static;

#ifdef __16_SHOP
	CWndShopCtrl	m_wndShopCtrl;
	void InsertItem( char cTab, CItemElem* pItemElem, int nNum );
#endif

	CWndShop(); 
	~CWndShop(); 

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual void OnDestroyChildWnd( CWndBase* pWndChild );
	virtual	void OnDestroy( void );

}; 

#if __VER >= 8 //__CSC_VER8_4
class CWndBeautyShopConfirm;
#ifdef __NEWYEARDAY_EVENT_COUPON
class CWndUseCouponConfirm;
#endif //__NEWYEARDAY_EVENT_COUPON
#endif //__CSC_VER8_4

class CWndBeautyShop : public CWndNeuz 
{ 
public:
	CTexture         m_Texture;
	CWndConfirmSell* m_pWndConfirmSell;
	CModelObject*	 m_pModel;

	DWORD			 m_dwHairMesh;
	BOOL			 m_bLButtonClick;
	CRect			 m_ColorRect[3];
	FLOAT			 m_fColor[3];
	int              m_nHairCost;
#if __VER >= 8 //__CSC_VER8_4
	int				 m_nHairColorCost;
#else
	int              m_nHairColorCostR;
	int              m_nHairColorCostG;
	int              m_nHairColorCostB;	
#endif //__CSC_VER8_4
	CPoint			 m_ColorScrollBar[3];
	CPoint			 m_OriginalColorScrollBar[3];
	
#ifdef __Y_BEAUTY_SHOP_CHARGE
	BOOL			 m_bChange;
#endif //__Y_BEAUTY_SHOP_CHARGE
#if __VER >= 8 //__CSC_VER8_4
	CModelObject*	 m_pApplyModel;
	CWndEdit*		 m_pRGBEdit[3];
	int				 m_ChoiceBar;
	CModelObject*	 m_pHairModel;
	DWORD			 m_nHairNum[4];
	DWORD			 m_dwSelectHairMesh;
	CWndBeautyShopConfirm* m_pWndBeautyShopConfirm;
#endif //__CSC_VER8_4
	
#ifdef __NEWYEARDAY_EVENT_COUPON
	BOOL m_bUseCoupon;
	CWndUseCouponConfirm* m_pWndUseCouponConfirm;
#endif //__NEWYEARDAY_EVENT_COUPON
	
public: 
	CWndBeautyShop(); 
	~CWndBeautyShop(); 
	void    ReSetBar( FLOAT r, FLOAT g, FLOAT b );

	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();
	
	virtual void OnMouseWndSurface( CPoint point );
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual void OnDestroyChildWnd( CWndBase* pWndChild );
	virtual	void OnDestroy( void );

#if __VER >= 8 //__CSC_VER8_4
	void SetRGBToEdit(float color, int editnum);
	void SetRGBToBar(int editnum);
	void DrawHairKind(C2DRender* p2DRender, D3DXMATRIX matView);
	void UpdateModels();
#endif //__CSC_VER8_4

#ifdef __NEWYEARDAY_EVENT_COUPON
	void UseHairCoupon(BOOL isUse);
#endif //__NEWYEARDAY_EVENT_COUPON
}; 

#if __VER >= 8 //__CSC_VER8_4

#ifdef __NEWYEARDAY_EVENT_COUPON
class CWndUseCouponConfirm : public CWndNeuz 
{	
public:
	BOOL  m_bUseCoupon;
	DWORD m_TargetWndId;
	BOOL  m_checkClose;
	int   m_MainFlag;
public: 
	CWndUseCouponConfirm(); 
	virtual ~CWndUseCouponConfirm(); 
	
	virtual void OnDestroy();
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 

	void SetInfo(DWORD targetWndId, int flag);
}; 
#endif //__NEWYEARDAY_EVENT_COUPON

class CWndBeautyShopConfirm : public CWndNeuz 
{
public:
	int m_ParentId;

public: 
	CWndBeautyShopConfirm(); 
	virtual ~CWndBeautyShopConfirm(); 
	
	virtual void OnDestroy();
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 

class CWndFaceShop : public CWndNeuz
{
public:
	CModelObject* m_pMainModel;
	CModelObject* m_pApplyModel;
	CModelObject* m_pFriendshipFace;
	CModelObject* m_pNewFace;

	DWORD m_dwFriendshipFace;
	DWORD m_dwNewFace;
	DWORD m_nSelectedFace;
	DWORD m_nFriendshipFaceNum[4];
	DWORD m_nNewFaceNum[4];
	int m_nCost;
	int m_ChoiceBar;
	
	CWndBeautyShopConfirm* m_pWndBeautyShopConfirm;

#ifdef __NEWYEARDAY_EVENT_COUPON
	BOOL m_bUseCoupon;
	CWndUseCouponConfirm* m_pWndUseCouponConfirm;	
#endif //__NEWYEARDAY_EVENT_COUPON

public:
	CWndFaceShop(); 
	~CWndFaceShop();
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual	void OnDestroy( void );
	virtual void OnDestroyChildWnd( CWndBase* pWndChild );
		
	void DrawFaces(int ChoiceFlag, C2DRender* p2DRender, D3DXMATRIX matView);
	void UpdateModels();

#ifdef __NEWYEARDAY_EVENT_COUPON
	void UseFaceCoupon(BOOL isUse);
#endif //__NEWYEARDAY_EVENT_COUPON
};
#endif //__CSC_VER8_4

#endif
