#if __VER >= 9 // __CSC_VER9_1

#ifndef __WNDPETSYSTEM__H
#define __WNDPETSYSTEM__H

/*******************************
	펫 시스템 관련 Window
********************************/
struct PETCAMTABLE
{
	D3DXVECTOR3 CamPos;
	D3DXVECTOR3 CamLook;
	float Scale;
};

#define MAX_PET_CLASS 7

#if __VER >= 12 // __PET_0519
class CWndPetAwakCancel : public CWndNeuz 
{ 
public: 	
	CWndText* m_pText;
	CItemElem* m_pItemElem;
	ItemProp* m_pEItemProp;
	CTexture* m_pTexture;

	CWndPetAwakCancel(); 
	~CWndPetAwakCancel(); 

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

class CWndPetStatus : public CWndNeuz
{
public:
	BOOL m_bHPVisible;
	BOOL m_bExpVisible;

	//CModel* m_pModel;
	CModelObject* m_pPetModel; // Pet Model Object.
	int	m_nDisplay;
	CItemElem*	m_pPetElem;
	PETCAMTABLE m_PetCameTable[MAX_PET_CLASS];

public:
	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();
	
	CTexture m_texGauEmptyNormal;
	CTexture m_texGauEmptySmall;
	CTexture m_texGauFillNormal;
	CTexture m_texGauFillSmall;

#ifdef __STATUS_V19
	CTexture m_HPGaugeTexture;
	CTexture m_EXPGaugeTexture;
#endif// __STATUS_V19

	CTexture* m_pTexPetLv[6];
	CTexture* m_pTexPetLvBg;
	CTexture* m_pTexPetStatusBg;
	int		  m_aPetLv[6];
	
	LPDIRECT3DVERTEXBUFFER9 m_pVBHPGauge;
	LPDIRECT3DVERTEXBUFFER9 m_pVBEXPGauge;
	
	BOOL m_bVBHPGauge;
	BOOL m_bVBEXPGauge;

	int m_nHPWidth;
	int m_nEXPWidth;
	
	BOOL m_bExtension;
	int m_nLockLevel[2];
	BYTE m_nBackUpLevel[2];
	CString m_strPathLvImage[9];
	int m_nChoiceLevel;

	void MakeGaugeVertex();
	
	CWndPetStatus(); 
	virtual ~CWndPetStatus();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL,DWORD dwWndId = 0);
	// message
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL );
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnEraseBkgnd(C2DRender* p2DRender);
	virtual	void SetWndRect( CRect rectWnd, BOOL bOnSize = TRUE );
	virtual	void PaintFrame( C2DRender* p2DRender );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual BOOL Process();
	virtual void OnMouseWndSurface( CPoint point );

	void DrawPetInformation(C2DRender* p2DRender);
	void SetExtension(BOOL eflag);
	void SetPetCamTable();
	void LockShowLevel(BOOL lFlag, int nLevel, int nPos);
};

#if __VER >= 15 // __PETVIS

class CWndConfirmVis : public CWndNeuz 
{ 
public: 
	enum ConfirmVisSection
	{
		CVS_EQUIP_VIS,				// 비스착용 
		CVS_UNEQUIP_VIS,			// 비스 착용해제  
		CVS_EQUIP_VISKEY,			// 비스키 사용 
		CVS_PICKUP_TO_BUFF,			// 버프펫으로 변환

#ifdef __PROTECT_AWAKE
		ETC_PROTECT_AWAKE,				// 각성 보호 
#endif //__PROTECT_AWAKE
	};
	CWndConfirmVis(); 
	virtual ~CWndConfirmVis(); 

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 

	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL );
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 

	void SendEquipPacket( );

 	DWORD m_dwItemId;
 	OBJID m_objid;
 	int m_nSlot;
	DWORD m_dwItemIndex;

	//hm.. need refactoring
	ConfirmVisSection m_eSection;
};

class CWndBuffPetStatus : public CWndNeuz
{
public:
	//gmpbigsun: 버프펫 상태 윈도우
	CWndBuffPetStatus( );
	virtual ~CWndBuffPetStatus( );
	
	virtual void OnDestroy();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL,DWORD dwWndId = 0);
	virtual BOOL Process();
	
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point );
	virtual void OnLButtonDown( UINT nFlags, CPoint point );
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point);
	virtual void OnMouseWndSurface( CPoint point );
//	virtual	void PaintFrame( C2DRender* p2DRender );
	
	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();

	void UpdateVisState( );			//장착된 비스에 대해 유효성 검사 
	CItemElem* GetItemElemByIndex( const int index );
	BOOL DoModal_ConfirmQuestion( DWORD dwItemId, OBJID objid, DWORD dwIndex, int nSlot = 0,  CWndConfirmVis::ConfirmVisSection eSection = CWndConfirmVis::CVS_EQUIP_VIS );			//OK => SendDoUseItem
	int GetSlotIndexByPoint( const CPoint& point );
	BOOL IsVisItem( DWORD index );
	BOOL IsFreeSlot( const int index );
		
protected:
	void DrawSlotItems( C2DRender* p2DRender );	

protected:
	CModelObject* m_pPetModel; // Pet Model Object.

	CItemElem*	m_pItemElem[MAX_VIS];
	int			m_nCtrlId[MAX_VIS];
	CTexture*	m_pTexture[MAX_VIS];

	CWndConfirmVis* m_pWndConfirmVis;

	vector<BYTE> m_cVisStates;
	CTexture* m_pTexPetStatusBg;

	float	m_fRadius;
};

#endif //#ifdef __PETVIS

class CWndFoodConfirm : public CWndNeuz 
{ 
public: 
	CItemElem* m_pItemElem;
	CWndEdit* m_pEdit;
	int m_nParent;
	
	CWndFoodConfirm();
	CWndFoodConfirm(int nParent);
	virtual ~CWndFoodConfirm(); 
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate();
	virtual	void PaintFrame( C2DRender* p2DRender );
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point );

	void SetItem(DWORD dwObjId);
};

class CWndPetMiracle : public CWndNeuz 
{
public:
	int m_nMiracleLv[2];
	int m_nCount[2];
	int m_nDelay[2];			//회전시키는 Delay값
	int m_nStatus[2];			//현재의 상태 (Delay관련 적용 위해 사용)
	int m_nPreLvCount;
	int m_nCurLvCount;
	int m_nResPreLevel;
	int m_nResCurLevel;
	int m_nPetLevel;		//펫 레벨
	
	BOOL m_bReciveResult[2];	//서버로 부터 결과를 받았는지 확인
	BOOL m_bEnd;				//회전이 끝난 시점
	BOOL m_bLocked[2];			//상태창에 레벨고정을 하였었는지.

	DWORD m_dwObjId;
	
	CString m_strPetLevel[2];
	CString m_strPathLvImage[9];
	
	CWndText* m_pText;
	
public: 
	CWndPetMiracle(); 
	virtual ~CWndPetMiracle(); 
	
	virtual void OnDestroy();
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL Process ();
	
	void ReceiveResult(int nPreLevel, int nCurLevel);		//서버로 부터 결과 값을 받음
	void PreLevelImgProcess();
	void CurLevelImgProcess();
	void SetDescription(CHAR* szChar);
	void SetItem(DWORD dwObjId);
}; 

class CWndPetFoodMill : public CWndNeuz
{
public:
	OBJID m_dwObjId;		//상용화 아이템 objid
	CItemElem* m_pItemElem;	//분쇄 되는 아이템
	CTexture*	m_pTexture;

	CWndFoodConfirm* m_pWndFoodConfrim;
	int m_nItemCount;
public:
	CWndPetFoodMill();
	virtual ~CWndPetFoodMill(); 
	
	virtual void OnDestroy();
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate();
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point );
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point );
	void SetItem(DWORD dwObjId);
	void SetItemForFeed(CItemElem* pItemElem, int nCount);
	void ReceiveResult(int nResult, int nCount);
};

class CWndPetLifeConfirm : public CWndNeuz 
{	
public:
	CWndButton m_wndButton1;
	CWndButton m_wndButton2;
	int m_nId;
	
protected:
	CString m_strText;
	CWndText m_wndText;
	
public:
	CWndPetLifeConfirm();
	~CWndPetLifeConfirm();
	
	virtual void OnDestroy();
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD dwWndId = 0 );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point );

	void SetItem(int nId);
}; 

#if __VER >= 12 // __CSC_VER12_5
#define MAX_TRANS_EGG 10

class CWndPetTransEggs : public CWndNeuz 
{
public:
	CItemElem*	m_pItemElem[MAX_TRANS_EGG];
	int			m_nCtrlId[MAX_TRANS_EGG];
	CWndText*	m_pText;
	CTexture*	m_pEggTexture;
	
public: 
	CWndPetTransEggs(); 
	virtual ~CWndPetTransEggs(); 
	
	virtual void OnDestroy();
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point );
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point );
	virtual void OnMouseWndSurface(CPoint point);

	void SetDescription(CHAR* szChar);
	void SetItem(CItemElem* pItemElem);

private:
	void ResetEgg();
	void CheckFull();
}; 
#endif //__CSC_VER12_5

#endif //__WNDPETSYSTEM__H
#endif //__CSC_VER9_1