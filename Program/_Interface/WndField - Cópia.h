#if !defined(AFX_WNDFIELD_H__A93F3186_63D6_43C1_956F_EC8691E0C7D9__INCLUDED_)
#define AFX_WNDFIELD_H__A93F3186_63D6_43C1_956F_EC8691E0C7D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "post.h"
#include "guild.h"

#if __VER >= 13 // __CSC_VER13_2
#include "couple.h"
#include "couplehelper.h"
#endif //__CSC_VER13_2

extern 	int g_nSkillCurSelect;
const int MAX_WANTED_LIST =	100;

#if __VER >= 13 // __HONORABLE_TITLE
#include "honor.h"
#endif	// __HONORABLE_TITLE

#ifdef __MAIL_REQUESTING_BOX
#include "WndMailRequestingBox.h"
#endif // __MAIL_REQUESTING_BOX
//////////////////////////////////////////////////////////////////////////////////////
// 애플랫 윈도 
//
class CWndApplet : public CWndNeuz
{
public:
	CWndListBox m_wndList;
	CWndButton m_wndButton1;
	CWndButton m_wndButton2;
	CWndButton m_wndButton3;

	CWndApplet();
	virtual ~CWndApplet();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL,DWORD dwWndId = 0);
	// message
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL );
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnChildNotify(UINT message,UINT nID,LRESULT* pLResult);

};

//////////////////////////////////////////////////////////////////////////////////////
// 드롭아이템 
//
class CWndDropItem : public CWndNeuz 
{ 
public: 
	CItemElem* m_pItemElem;
	D3DXVECTOR3 m_vPos;
	CWndEdit* m_pEdit;

	CWndDropItem(); 
	virtual ~CWndDropItem(); 
	
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

class CWndDropConfirm : public CWndNeuz 
{ 
public: 
	CItemElem* m_pItemElem;
	D3DXVECTOR3 m_vPos;
	
	CWndDropConfirm(); 
	virtual ~CWndDropConfirm(); 
	
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

class CWndRandomScrollConfirm : public CWndNeuz 
{ 
public:
	BOOL bFlag;
	OBJID objid, objid1;
	void SetItem( OBJID objidBuf, OBJID objid1Buf, BOOL bFlagBuf = FALSE );
public: 
	CWndRandomScrollConfirm(); 
	virtual ~CWndRandomScrollConfirm(); 
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 

class CWndQuestItemWarning : public CWndNeuz 
{ 
public: 
	CItemElem* m_pItemElem;
	D3DXVECTOR3 m_vPos;

	CWndQuestItemWarning(); 
	virtual ~CWndQuestItemWarning(); 
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 


//////////////////////////////////////////////////////////////////////////////////////
//  퀘스트 아이템 정보 창
//
class CWndQuestItemInfo : public CWndNeuz
{
public:
	CWndQuestItemInfo();
	virtual ~CWndQuestItemInfo();

	CItemBase*	m_pItemBase;
	CPoint		m_Position;

	inline void SetItemBase(CItemBase* pItemBase = NULL) { m_pItemBase = pItemBase; };
	
	BOOL Create(CItemBase* pItemBase, UINT nID, CWndBase* pWndParent = NULL );
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, CItemBase* pItemBase = NULL); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
};

//////////////////////////////////////////////////////////////////////////////////////
// 인벤토리 
//
#if __VER >= 9 // __CSC_VER9_1
class CWndRemoveJewelConfirm;
#endif //__CSC_VER9_1

class CWndGold : public CWndButton
{
public:
	CTexture m_texture;
	CWndGold();
	virtual ~CWndGold();
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnMouseMove(UINT nFlags, CPoint point);
	virtual	void OnInitialUpdate();
};

class CWndConfirmBuy;
class CWndInventory : public CWndNeuz
{
	CPoint				m_OldPos;
	FLOAT				m_fRot;
	CModelObject*		m_pModel;
	BOOL				m_bLButtonDownRot;
	
	CRect		 m_InvenRect[MAX_HUMAN_PARTS];

	CModelObject m_meshStage;
	CItemElem* m_pSelectItem;
	BOOL m_bReport;
	
	CTexture* m_TexRemoveItem;

	CTexture* m_TabTexture;
public:
	void UpDateModel();

	void UpdateParts();
	CModelObject* GetModel() { return m_pModel; }
	void BaseMouseCursor();
	void RunUpgrade( CItemBase* pItem );
	void SetEnchantCursor();
	
	CSfx*		m_pSfxUpgrade;
	CItemBase*	m_pUpgradeItem;
	CItemBase*	m_pUpgradeMaterialItem;
	BOOL		m_bIsUpgradeMode;
	DWORD		m_dwEnchantWaitTime;

	CWndConfirmBuy* m_pWndConfirmBuy;
	CWndGold     m_wndGold;
	CWndItemCtrl m_wndItemCtrl;

	CWndItemCtrl m_wndInvCtrl[4];

#if __VER >= 9 // __CSC_VER9_1
	BOOL		m_bRemoveJewel;
	CWndRemoveJewelConfirm* m_pWndRemoveJewelConfirm;
#endif //__CSC_VER9_1
	CWndInventory();
	virtual ~CWndInventory();
	virtual BOOL Process ();
	virtual void OnMouseWndSurface( CPoint point );
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL,DWORD dwWndId = 0);
	// message
	virtual BOOL OnChildNotify(UINT message,UINT nID,LRESULT* pLResult);
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL );
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point);
	virtual void OnRButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point = 0 );
	virtual void OnMouseMove(UINT nFlags, CPoint point);
	virtual void OnDestroyChildWnd( CWndBase* pWndChild );
	virtual void OnDestroy( void );
	virtual BOOL OnSetCursor ( CWndBase* pWndBase, UINT nHitTest, UINT message );
	
#if __VER >= 9 // __CSC_VER9_1
public:
	void UpdateTooltip( void )	{	m_wndItemCtrl.UpdateTooltip();	}
#endif //__CSC_VER9_1

};

class CWndQueryEquip : public CWndNeuz
{
	CPoint				m_OldPos;
	FLOAT				m_fRot;
	CModelObject*		m_pModel;
	BOOL				m_bLButtonDownRot;
	DWORD		 m_ObjID;
	CRect		 m_InvenRect[MAX_HUMAN_PARTS];

	EQUIP_INFO_ADD	m_aEquipInfoAdd[MAX_HUMAN_PARTS];
public:

	CMover* GetMover() 
	{ 
		if( m_ObjID != NULL_ID )
			return prj.GetMover( m_ObjID ); 

		return NULL;
	}
	void	SetMover( DWORD		 ObjID );
	void	SetEquipInfoAdd( EQUIP_INFO_ADD* aEquipInfoAdd );
	
	CWndQueryEquip();
	virtual ~CWndQueryEquip();
	virtual BOOL Process ();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual void OnMouseWndSurface( CPoint point );
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL,DWORD dwWndId = 0);
	virtual void OnMouseMove (UINT nFlags, CPoint point);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
};


//////////////////////////////////////////////////////////////////////////////////////
// 상태 
//
class CWndStateConfirm : public CWndNeuz 
{ 
public:
	UINT m_nId;
	void OnSetState( UINT nId );
	void SendYes( void );
public: 
	CWndStateConfirm(); 
	virtual ~CWndStateConfirm(); 
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 
class CWndCharacterBase : public CWndBase
{
	CTexture m_txInfo;

public:
	CWndCharacterBase();
	virtual ~CWndCharacterBase();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL,DWORD dwWndId = 0);
	// message
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL );
	virtual BOOL OnChildNotify(UINT message,UINT nID,LRESULT* pLResult);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
};

class CWndCharacterDetail : public CWndBase
{
	CWndButton m_wndStr;
	CWndButton m_wndSta;
	CWndButton m_wndDex;
	CWndButton m_wndInt;
	
	int			m_nCnt;

public:
	BOOL	m_fWaitingConfirm;
	CWndCharacterDetail();
	virtual ~CWndCharacterDetail();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL,DWORD dwWndId = 0);
	// message
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL );
	virtual BOOL OnChildNotify(UINT message,UINT nID,LRESULT* pLResult);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnMouseWndSurface( CPoint point );

protected:
	void RenderATK( C2DRender* p2DRender, int x, int y );
};

#if __VER >= 9 // __CSC_VER9_2
class CWndCharacterDetail2 : public CWndBase
{
public:
	CWndButton m_wndStrPlus, m_wndStrMinus;
	CWndButton m_wndStaPlus, m_wndStaMinus;
	CWndButton m_wndDexPlus, m_wndDexMinus;
	CWndButton m_wndIntPlus, m_wndIntMinus;
	
	CWndButton m_wndApply, m_wndReset;
	
	CWndEdit m_editStrCount;
	CWndEdit m_editStaCount;
	CWndEdit m_editDexCount;
	CWndEdit m_editIntCount;

	int m_nStrCount;
	int m_nStaCount;
	int m_nDexCount;
	int m_nIntCount;

	int m_nGpPoint;

	int m_nATK;
	int m_nDEF;
	int m_nCritical;
	int m_nATKSpeed;
	
public:
	CWndCharacterDetail2();
	virtual ~CWndCharacterDetail2();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL,DWORD dwWndId = 0);
	// message
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL );
	virtual BOOL OnChildNotify(UINT message,UINT nID,LRESULT* pLResult);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnMouseWndSurface( CPoint point );
	virtual BOOL Process ();
	
	void RefreshStatPoint();
	void GetVirtualATK(int* pnMin, int* pnMax);
	int GetVirtualDEF();
	int GetVirtualCritical();
	float GetVirtualATKSpeed();

protected:
	void RenderATK( C2DRender* p2DRender, int x, int y );
};
#endif //__CSC_VER9_2

class CWndChangeJob;

class CWndPvpBase : public CWndBase
{
	CTexture m_txInfo;
	
public:
	CWndButton m_wndChangeJob;
	CWndChangeJob* m_pWndChangeJob;
	BOOL	m_fWaitingConfirm;
	BOOL m_bExpert;
	int m_nDisplay;			// 직업창 화면 옵션

	CWndPvpBase();
	virtual ~CWndPvpBase();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL,DWORD dwWndId = 0);
	// message
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL );
	virtual BOOL OnChildNotify(UINT message,UINT nID,LRESULT* pLResult);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
};

class CWndChangeJob : public CWndNeuz
{
public:
	int m_nJob;
	CWndButton m_wndExpert[ 6 ];
	CWndButton m_wndOk;
	CWndButton m_wndCancel;

	CWndChangeJob( int nJob );
	virtual ~CWndChangeJob();
	virtual void OnDraw( C2DRender* p2DRender );
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize( CWndBase* pWndParent = NULL,DWORD dwWndId = 0 );
	// message
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp( UINT nFlags, CPoint point );
	virtual void OnLButtonDown( UINT nFlags, CPoint point );
};

#if __VER >= 13 // __RENEW_CHARINFO
class CWndCharInfo : public CWndBase
{

public:
	CWndButton		m_wndChangeJob;
	CWndChangeJob*	m_pWndChangeJob;
	BOOL			m_fWaitingConfirm;
	BOOL			m_bExpert;
	int				m_nDisplay;

	CWndButton m_wndStrPlus, m_wndStrMinus;
	CWndButton m_wndStaPlus, m_wndStaMinus;
	CWndButton m_wndDexPlus, m_wndDexMinus;
	CWndButton m_wndIntPlus, m_wndIntMinus;
	
	CWndButton m_wndApply, m_wndReset;
	
	CWndEdit m_editStrCount;
	CWndEdit m_editStaCount;
	CWndEdit m_editDexCount;
	CWndEdit m_editIntCount;

	int m_nStrCount;
	int m_nStaCount;
	int m_nDexCount;
	int m_nIntCount;

	int m_nGpPoint;

	int m_nATK;
	int m_nDEF;
	int m_nCritical;
	int m_nATKSpeed;

public:

	CWndCharInfo();
	virtual ~CWndCharInfo();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL,DWORD dwWndId = 0);
	// message
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL );
	virtual BOOL OnChildNotify(UINT message,UINT nID,LRESULT* pLResult);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnMouseWndSurface( CPoint point );
	virtual BOOL Process ();
	void RefreshStatPoint();
	void GetVirtualATK(int* pnMin, int* pnMax);
	int GetVirtualDEF();
	int GetVirtualCritical();
	float GetVirtualATKSpeed();



protected:
	void RenderATK( C2DRender* p2DRender, int x, int y );
};

class CWndHonor : public CWndNeuz
{
private:
	int						m_nSelectedId;
	int						m_nCount;
	vector<EarnedTitle>		m_vecTitle;	
public:

	CWndHonor();
	virtual ~CWndHonor();
	virtual void OnDraw( C2DRender* p2DRender );
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize( CWndBase* pWndParent = NULL,DWORD dwWndId = 0 );
	// message
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp( UINT nFlags, CPoint point );
	virtual void OnLButtonDown( UINT nFlags, CPoint point );
	void RefreshList();

};
#endif // __RENEW_CHARINFO

class CWndCharacter : public CWndNeuz
{
public:
#if __VER >= 13 // __RENEW_CHARINFO
	CWndCharInfo		m_wndCharInfo;
	CWndHonor			m_wndHonor;
#else // __RENEW_CHARINFO
#if __VER >= 9 // __CSC_VER9_2
	CWndCharacterDetail2 m_wndStateDetail;
#else //__CSC_VER9_2
	CWndCharacterDetail m_wndStateDetail;
#endif
	CWndCharacterBase   m_wndStateBase;	
	CWndPvpBase         m_wndPvp;
#endif // __RENEW_CHARINFO
public:
	CWndCharacter();
	virtual ~CWndCharacter();
	virtual void SerializeRegInfo( CAr& ar, DWORD& dwVersion );
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL,DWORD dwWndId = 0);
	// message
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL );
	virtual BOOL OnChildNotify(UINT message,UINT nID,LRESULT* pLResult);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
};


class CWndReSkillControl : public CWndNeuz
{
public:
	CWndReSkillControl();
	virtual ~CWndReSkillControl();
	virtual void OnDraw( C2DRender* p2DRender );
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize( CWndBase* pWndParent = NULL,DWORD dwWndId = 0 );
	// message
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp( UINT nFlags, CPoint point );
	virtual void OnLButtonDown( UINT nFlags, CPoint point );
};

class CWndReSkillWarning : public CWndNeuz
{
	BOOL	m_bParentDestroy;
public:
	CWndReSkillWarning();
	virtual ~CWndReSkillWarning();

	void InitItem( BOOL bParentDestroy );
	virtual void OnDraw( C2DRender* p2DRender );
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize( CWndBase* pWndParent = NULL,DWORD dwWndId = 0 );
	// message
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp( UINT nFlags, CPoint point );
	virtual void OnLButtonDown( UINT nFlags, CPoint point );
	virtual void OnDestroy();
};



//////////////////////////////////////////////////////////////////////////////////////
// 스킬 EX 
//
#ifdef __NEW_SKILL_TREE
class CWndSkillTreeEx : public CWndNeuz
{
protected:
	BOOL			m_bDrag;					//마우스로 클릭했는데 스킬아이콘 영역 안 인 경우 TRUE
	LPSKILL			m_pFocusItem;				//선택된 아이템 : 즉 스킬을 더블클릭하거나 해서 레벨을 올릴수있는 상태로 만든 스킬아이템
	int				m_nCurrSkillPoint;		//현재 남은 스킬포인트
	CTexturePack	m_textPackNum;			//스킬 레벨숫자 표시용 그림 ( 1, 2,..... max )
	DWORD			m_dwMouseSkill;			//마우스에 위치한 스킬
	CWndButton*		m_pWndButton[4];		//+, -, reset, finish
	LPSKILL			m_apSkill;				//스킬 목록
	CTexture*		m_atexSkill[ MAX_SKILL_JOB ];
	CTexture*		m_aTexSkillTrees[ 4 ];
	char			m_cOldJob;
	char			m_cActiveSlot;
	char			m_cActiveJobType;
	CTexture*		m_atexTopDown[2];	//these store the up and down icons
	CPoint			m_pointTreePos;
	CTexture*		m_texSelectSkill;	
	CString			m_strHeroSkillIcon;
public:
	BOOL	IsReSkillPointDone();
	int		GetCurrSkillPoint() { return m_nCurrSkillPoint; }
	void	SubSkillPointDown( LPSKILL lpSkill );
	LPSKILL GetSkill() { return m_apSkill; }
	BOOL	IsDownPoint(DWORD skill);
	int		GetCurSelect() { return g_nSkillCurSelect; }
	BOOL	GetSkillPoint(DWORD dwSkillID, CRect& rect );
	LPSKILL GetSkill( int i );
	LPSKILL GetdwSkill( DWORD dwSkill );
	BOOL	CheckSkill( int i );
	void LoadTextureSkillicon();
	void InitItem( int nJob, LPSKILL apSkill, BOOL bReset = FALSE );
	CWndSkillTreeEx();
	virtual ~CWndSkillTreeEx();

	void SetSlot( char cSlot );
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual BOOL Process ();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL,DWORD dwWndId = 0);
	virtual void OnMouseMove(UINT nFlags, CPoint point);
	virtual void OnMouseWndSurface( CPoint point );
	// message
	virtual BOOL OnChildNotify(UINT message,UINT nID,LRESULT* pLResult);
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL );
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point);
	void SetActiveSlot( int nSlot, BOOL bFlag );
	virtual void SerializeRegInfo ( CAr& ar, DWORD& dwVersion );
	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();	
};
#else
class CWndSkillTreeEx : public CWndNeuz
{
protected:
	int			  m_nCount;					//현재 직업에 해당하는 스킬 총 갯수

	int           m_nCurSelect  ;			//잉 안쓰고있는데?
	BOOL          m_bDrag;					//마우스로 클릭했는데 스킬아이콘 영역 안 인 경우 TRUE
	LPSKILL       m_pFocusItem;				//선택된 아이템 : 즉 스킬을 더블클릭하거나 해서 레벨을 올릴수있는 상태로 만든 스킬아이템
	int				m_nCurrSkillPoint;		//현재 남은 스킬포인트
	CTexturePack	m_textPackNum;			//스킬 레벨숫자 표시용 그림 ( 1, 2,..... max )
	DWORD			m_dwMouseSkill;			//마우스에 위치한 스킬
	CWndButton*		m_pWndButton[4];		//+, -, reset, finish
	LPSKILL       m_apSkill;				//스킬 목록
	CTexture*     m_atexSkill[ MAX_SKILL_JOB ];
	CTexture*	  m_aSkillLevel[ 3 ];
	//CTexture*     m_atexJobPannel[ 2 ];
	LPIMAGE       m_atexJobPannel[ 2 ];
#if __VER >= 10 // __CSC_VER9_1
	CString		  m_strHeroSkilBg;			//히어로 이미지 파일 이름
#endif //__CSC_VER9_1
	int           m_nJob         ;			//class 번호
	
	CTexture m_texGauEmptyNormal ;
	CTexture m_texGauFillNormal  ;
	
	LPDIRECT3DVERTEXBUFFER9 m_pVBGauge;

	CTexture*     m_atexTopDown[2];
	int 	m_nTopDownGap;
#if __VER >= 10 // __CSC_VER9_1
	BOOL	m_bSlot[4];
#else
	BOOL	m_bSlot[3];
#endif //__CSC_VER9_1

#if __VER >= 10 // __CSC_VER9_1
	BOOL m_bLegend;							//전승
	CWndStatic* m_pWndHeroStatic[2];
#endif //__CSC_VER9_1

public:
	BOOL	IsReSkillPointDone();
	int		GetCurrSkillPoint() { return m_nCurrSkillPoint; }
	void	SubSkillPointDown( LPSKILL lpSkill );
	LPSKILL GetSkill() { return m_apSkill; }
	BOOL	IsDownPoint(DWORD skill);
	int		GetCalcImagePos( int nIndex );
	int		GetCurSelect() { return g_nSkillCurSelect; }
	BOOL	GetSkillPoint(DWORD dwSkillID, CRect& rect );
	LPSKILL GetSkill( int i );
	LPSKILL GetdwSkill( DWORD dwSkill );
	BOOL	CheckSkill( int i );
	void LoadTextureSkillicon();
	void InitItem( int nJob, LPSKILL apSkill, BOOL bReset = FALSE );
	CWndSkillTreeEx();
	virtual ~CWndSkillTreeEx();

	void AfterSkinTexture( LPWORD pDest, CSize size, D3DFORMAT d3dFormat = D3DFMT_A4R4G4B4 );
	void SetJob( int nJob );
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual BOOL Process ();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL,DWORD dwWndId = 0);
	virtual void OnMouseMove(UINT nFlags, CPoint point);
	virtual void OnMouseWndSurface( CPoint point );
	// message
	virtual BOOL OnChildNotify(UINT message,UINT nID,LRESULT* pLResult);
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL );
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point);
	virtual void OnRButtonDblClk( UINT nFlags, CPoint point);
	void SetActiveSlot( int nSlot, BOOL bFlag );
	virtual void SerializeRegInfo ( CAr& ar, DWORD& dwVersion );
	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();	
};
#endif

//////////////////////////////////////////////////////////////////////////////////////
// 매신저  
//

//////////////////////////////////////////////////////////////////////////////////////
// Navigator
//

class CWndNavigator : public CWndNeuz
{
	CWndButton m_wndPlace;
	CWndButton m_wndMover;
	CWndButton m_wndZoomIn;
	CWndButton m_wndZoomOut;

	CWndButton m_wndClose;
	CWndButton m_wndHelp;

    LPDIRECT3DVERTEXBUFFER9 m_pVBTexture;

	CWndMenu   m_wndMenuPlace;
	CWndMenu   m_wndMenuMover;
	BILLBOARD  m_billboard;
	CTexture   m_texArrow;
	CTexture   m_texDunFog;
    CTexture   m_ALPHACIRCLE;
	CTexture   m_NEWNAV;
	CTexture   m_TEST;
	CTexturePack m_texNavObjs;
	DWORD		 m_iFrame;
	DWORD		 m_iPastTime;
	CTexturePack m_texNavPos;
	inline void	 AccuFrame() { 
		DWORD CurTime = g_tmCurrent;
		if (CurTime - m_iPastTime > 120)
		{
			m_iFrame++; 
			m_iPastTime = CurTime;
		}
		if ( m_iFrame >= 4 )
			m_iFrame = 0;
	};
	void RenderMark( C2DRender* p2DRender, CMover* Player );	//	참조되는 변수는 플레이어와 파티플레이어를 얻을 수 있는 것이어야 하지만 
																//	현재는 자신만을 찍는 것을 하고 그 다음에 파티 플레이어들을 찾을수 있는 방법을 찾아 보자꾸나
	void RenderMarkAll( C2DRender* p2DRender , CMover* Player );
	CBillboard m_billArrow;
	CSize      m_size;
	int        m_nSizeCnt;
	TCHAR      m_szName[ 64 ];
	void ResizeMiniMap();
	
	CTexture m_GuildCombatTextureMask;
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	CTexture* m_pDestinationPositionTexture;
#endif // __IMPROVE_QUEST_INTERFACE

public:
	LPDIRECT3DVERTEXBUFFER9 m_pVBMEUTEST;

	BOOL m_bObjFilterPlayer ;
	BOOL m_bObjFilterParty  ;
	BOOL m_bObjFilterNPC    ;
	BOOL m_bObjFilterMonster;

	void RenderPartyMember( C2DRender* p2DRender, TEXTUREVERTEX** pVertices, CRect rect, D3DXVECTOR3 vPos, u_long uIdPlayer, LPCTSTR lpStr );
	void SetRegionName( TCHAR *tszName );		// 지역
		
	virtual void SerializeRegInfo( CAr& ar, DWORD& dwVersion );
	
	CWndNavigator(); 
	virtual ~CWndNavigator();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL,DWORD dwWndId = 0);
	// message
	virtual	void SetWndRect( CRect rectWnd, BOOL bOnSize = TRUE );
	virtual BOOL OnChildNotify(UINT message,UINT nID,LRESULT* pLResult);
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL );
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnRButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnEraseBkgnd(C2DRender* p2DRender);
	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point);
};
//////////////////////////////////////////////////////////////////////////////////////
// CWndStatus
//

class CWndStatus : public CWndNeuz
{
	BOOL m_bHPVisible;
	BOOL m_bExpVisible;
public:
	CModel* m_pModel;
	int		m_nDisplay;
public:
	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();
	
	CTexture m_texGauEmptyNormal ;
	CTexture m_texGauEmptySmall  ;
	CTexture m_texGauFillNormal  ;
	CTexture m_texGauFillSmall   ;
	
#ifdef __STATUS_V19
	CTexture m_HPGaugeTexture;
	CTexture m_MPGaugeTexture;
	CTexture m_FPGaugeTexture;
	CTexture m_EXPGaugeTexture;
	CTexture m_AEXPGaugeTexture;
#endif// __STATUS_V19

	LPDIRECT3DVERTEXBUFFER9 m_pVBHPGauge;
	LPDIRECT3DVERTEXBUFFER9 m_pVBMPGauge;
	LPDIRECT3DVERTEXBUFFER9 m_pVBFPGauge;
	LPDIRECT3DVERTEXBUFFER9 m_pVBEXPGauge;
	
	BOOL m_bVBHPGauge;
	BOOL m_bVBMPGauge;
	BOOL m_bVBFPGauge;
	BOOL m_bVBEXPGauge;

#if __VER >= 15 // __GUILD_HOUSE
	BOOL m_bShowTip_AEXP;
	BOOL m_bVBAEXPGauge;
	LPDIRECT3DVERTEXBUFFER9 m_pVBAEXPGauge;
#endif
	
	int m_nHPWidth ;
	int m_nMPWidth ;
	int m_nFPWidth ;
	int m_nEXPWidth;
	int m_nPXPWidth;
	
	void MakeGaugeVertex();
	
	CWndStatus(); 
	virtual ~CWndStatus();
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

#if __VER >= 15 // __GUILD_HOUSE
	virtual void OnMouseWndSurface( CPoint point );
#endif
};

class CWndQuit : public CWndMessageBox
{
	BOOL	m_bFlag;	
public:
	CWndQuit() { }
	virtual ~CWndQuit() { }
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD dwWndId = 0 );
	// message
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
};	
class CWndLogOut : public CWndMessageBox
{
public:
	CWndLogOut() { }
	virtual ~CWndLogOut() { }
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD dwWndId = 0 );
	// message
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
};	


class CWndLogOutWaitting : public CWndNeuz
{
	BOOL  m_bIsLogOut;
	DWORD dwTime;
public:
	CWndLogOutWaitting();
	virtual ~CWndLogOutWaitting();
	void	SetIsLogOut(BOOL bFlag) { m_bIsLogOut = bFlag; }
	virtual BOOL Process ();
	virtual BOOL Initialize( CWndBase* pWndParent );
	virtual	void OnInitialUpdate();
	// message
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
};	

//DECLARE_WNDMESSAGEBOX( CWndLogOut    ) // 로그아웃 처리하는 메시지 박스 클레스 

//BEGIN_WNDCLASS( CWndMacro )
//END_WNDCLASS

//BEGIN_WNDCLASS( CWndStyle )
//END_WNDCLASS

class CWndTradeGold : public CWndNeuz 
{ 
public: 
	UINT m_nIdWndTo;
	SHORTCUT m_Shortcut;
	D3DXVECTOR3 m_vPos;
	DWORD m_dwGold;
	CWndBase* m_pWndBase;
	CWndStatic* pStatic;
	CWndStatic* pStaticCount;
#if __VER >= 11 // __SYS_POCKET
	char m_nSlot;
	char m_nPutSlot;
#else
	BYTE m_nSlot;
	BYTE m_nPutSlot;
#endif
	CWndTradeGold(); 
	virtual ~CWndTradeGold(); 

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual BOOL Process( void );
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
};

class CWndConfirmTrade : public CWndNeuz 
{ 
	OBJID m_objid;
public: 
	CWndConfirmTrade(); 
	virtual ~CWndConfirmTrade(); 

	void OnSetName( const char* szName, OBJID objid );
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 

class CWndTradeConfirm : public CWndNeuz 
{ 
public: 
	BOOL bMsg;
	CWndTradeConfirm(); 
	virtual ~CWndTradeConfirm(); 
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 

BEGIN_WNDCLASS( CWndTrade )

	CWndTradeCtrl m_wndItemCtrlYou;
	CWndTradeCtrl m_wndItemCtrlI  ;
	CWndStatic    m_wndGold       ;
	int           m_nGoldI, m_nGoldYou;

	void DoCancel();

END_WNDCLASS

//////////

BEGIN_WNDCLASS( CWndEmotion )
	CWndTreeCtrl  m_wndViewCtrl;
	CWndEdit      m_wndEdit;
	CWndButton    m_wndDefault;
	LPTREEELEM    m_lpSelectTreeElem;
END_WNDCLASS


class CWndRevival : public CWndNeuz 
{ 
public: 
	CWndButton* m_pLodeLight;
	CWndButton* m_pLodeStar;
	CWndButton* m_pRevival;
#if __VER >= 9 // __S_9_ADD
	CWndButton* m_pShop;
#endif // __S_9_ADD
	CWndRevival(); 
	virtual ~CWndRevival(); 

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	void	EnableButton( int nButton, BOOL bEnable );
}; 

class CWndReWanted : public CWndNeuz 
{ 
public: 
	CWndReWanted(); 
	virtual ~CWndReWanted(); 

private:
	CString      m_strWanted;
	
public: 
	BOOL         CheckWantedInfo( int nGold, LPCTSTR szMsg );
	void		 SetWantedName( LPCTSTR szWanted );

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 

class CWndResurrectionConfirm : public CWndNeuz 
{ 
public: 
	CWndResurrectionConfirm(); 
	virtual ~CWndResurrectionConfirm(); 
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 

class CWndWantedConfirm : public CWndNeuz 
{ 
	int		 m_nGold;
	char	 m_szName[ MAX_NAME ];
public: 
	CWndWantedConfirm(); 
	virtual ~CWndWantedConfirm(); 
	
	void		 SetInfo( const char szName[], const int nGold );
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 


struct WANTEDLIST
{
	char	 szName[ MAX_NAME ];		// 이름.
	__int64	 nGold;						// 현상금
	char     szDate[32];
	char	 szMsg[WANTED_MSG_MAX + 1];	// 십자평 
};

class CWndWanted : public CWndNeuz 
{ 
private: 
	CWndWantedConfirm*  m_pWantedConfirm;
	time_t				m_recvTime;
	CWndScrollBar		m_wndScrollBar;
	int					m_nMax;
	WANTEDLIST			m_aList[MAX_WANTED_LIST];
	int					m_nSelect;

public: 
	CWndWanted(); 
	virtual ~CWndWanted(); 
	
	int          GetSelectIndex( const CPoint& point );	
	void		 Init( time_t lTime );
	void		 InsertWanted( const char szName[], __int64 nGold,  int nTime, const char szMsg[] );	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual	void OnRButtonUp( UINT nFlags, CPoint point );
	virtual void OnRButtonDown( UINT nFlags, CPoint point );
	virtual void OnMouseMove(UINT nFlags, CPoint point );
	virtual BOOL OnMouseWheel( UINT nFlags, short zDelta, CPoint pt );
	virtual void OnMouseWndSurface( CPoint point );
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point);	
}; 


class CWndCommItemDlg : public CWndNeuz 
{ 
public: 
	CWndEdit*		m_pWndEdit;
	DWORD			m_dwObjId;
	DWORD			m_dwCtrlId;
	void			SetItem( DWORD dwDefindText, DWORD dwObjId, DWORD dwCtrlId );
	CWndCommItemDlg(); 
	virtual ~CWndCommItemDlg(); 
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 

class CWndChangeClass1 : public CWndNeuz 
{ 
public: 
	int			nJob;
	void		SetJob();

	CWndChangeClass1(); 
	virtual ~CWndChangeClass1(); 
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 

class CWndChangeClass2 : public CWndNeuz 
{ 
public: 
	int			nJob;
	void		SetJob();
	
	CWndChangeClass2(); 
	virtual ~CWndChangeClass2(); 
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 

class CWndPostSendConfirm;

class CWndPostSend : public CWndNeuz
{
	int		m_nCost;
	int		m_nCount;
	BYTE    m_nItem;

public:	
	void ClearData();
	CWndPostSend(); 
	virtual ~CWndPostSend(); 
	
	
	BYTE GetItemId() { return m_nItem; }
	void SetItemId( BYTE nId );
	void SetCost( int nCost ) { m_nCost = nCost; }
	void SetCount( int nCount ) { m_nCount = nCount; }
	void SetReceive( char* pchar );
	void SetTitle( char* pchar );	
	void SetText( char* pchar );	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point );		
	virtual void OnRButtonUp( UINT nFlags, CPoint point );
	virtual void AdditionalSkinTexture( LPWORD pDest, CSize sizeSurface, D3DFORMAT d3dFormat = D3DFMT_A4R4G4B4 );
};

class CWndPostItemWarning : public CWndNeuz 
{ 
public: 
	int	   m_nMailIndex;
	
	CWndPostItemWarning(); 
	virtual ~CWndPostItemWarning(); 
	
	void	SetIndex( int nIndex ) { m_nMailIndex = nIndex; }
	void	SetString( char* string );
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 

class CWndPostDeleteConfirm;
class CWndPostRead : public CWndNeuz
{
	int					m_nMailIndex;
	CWndGold			m_wndGold;
	BOOL				m_bDrag;
	CWndPostDeleteConfirm* m_pDeleteConfirm;
	
public:	
	CWndPostItemWarning*		m_pWndPostItemWarning;
	void MailReceiveItem();
	void MailReceiveGold();
	void ClearData();
	CWndPostRead(); 
	virtual ~CWndPostRead(); 
	
	void SetValue( int nMailIndex );
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual void OnMouseMove(UINT nFlags, CPoint point );	
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point );		
	virtual void OnRButtonUp( UINT nFlags, CPoint point );
};


class CWndPostReceive : public CWndNeuz
{
	CTexture			m_Texture[3];
	CWndScrollBar		m_wndScrollBar;
	int					m_nSelect;
	int					m_nMax;
	CWndPostRead*		m_wndPostRead;
public:	
	void UpdateScroll();
	
	CWndPostReceive(); 
	virtual ~CWndPostReceive(); 

	int          GetSelectIndex( const CPoint& point );	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual BOOL OnMouseWheel( UINT nFlags, short zDelta, CPoint pt );
	virtual	void OnLButtonDblClk( UINT nFlags, CPoint point );
	virtual HRESULT DeleteDeviceObjects();
	virtual void OnMouseWndSurface( CPoint point );
};
class CWndPost : public CWndNeuz
{
public:
	CWndPostSend		m_PostTabSend;
	CWndPostReceive		m_PostTabReceive;
	
	CWndPost(); 
	virtual ~CWndPost(); 
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 

#ifdef __MAIL_REQUESTING_BOX
public:
	void LoadLastMailBox( void );
	void SaveLastMailBox( void );
	void CloseMailRequestingBox( void );

private:
	CWndMailRequestingBox* m_pWndMailRequestingBox;
#endif // __MAIL_REQUESTING_BOX
};
class CWndPostSendConfirm : public CWndNeuz
{
protected:
	CWndText	m_wndText;
	
	BYTE		m_nItem;
	short		m_nItemNum;
	int			m_nGold;
	char		m_szReceiver[MAX_NAME];
	char		m_szTitle[MAX_MAILTITLE];	
	char		m_szText[MAX_MAILTEXT];		
public: 
	CWndPostSendConfirm();
	virtual ~CWndPostSendConfirm();
	
	void			SetValue( BYTE nItem, short nItemNum, char* lpszReceiver, int nGold, char* lpszTitle, char* lpszText );
	virtual	BOOL	Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual	BOOL	OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual	void	OnDraw( C2DRender* p2DRender );
	virtual	void	OnInitialUpdate();
	virtual	BOOL	OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase );
	virtual	void	OnSize( UINT nType, int cx, int cy );
	virtual void	OnLButtonUp( UINT nFlags, CPoint point );
	virtual	void	OnLButtonDown( UINT nFlags, CPoint point );
}; 

class CWndPostDeleteConfirm : public CWndNeuz
{
protected:
	CWndText	m_wndText;
	int		m_nIndex;
public: 
	CWndPostDeleteConfirm();
	virtual ~CWndPostDeleteConfirm();
	
	void			SetValue( int nIndex ) { m_nIndex = nIndex; }
	virtual	BOOL	Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual	BOOL	OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual	void	OnDraw( C2DRender* p2DRender );
	virtual	void	OnInitialUpdate();
	virtual	BOOL	OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase );
	virtual	void	OnSize( UINT nType, int cx, int cy );
	virtual void	OnLButtonUp( UINT nFlags, CPoint point );
	virtual	void	OnLButtonDown( UINT nFlags, CPoint point );
}; 

struct GUILDLIST
{
	char	 szGuild[ MAX_NAME ];		// 이름.
	char	 szName[ MAX_NAME ];		// 이름.
	int		 nNum;
};

class CWndGuildCombatOffer : public CWndNeuz
{
protected:
	DWORD			m_dwReqGold;
	DWORD			m_dwMinGold;
	DWORD			m_dwBackupGold;

public: 
	CWndGuildCombatOffer();
	virtual ~CWndGuildCombatOffer();
	
	virtual	BOOL	Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual	BOOL	OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual	void	OnDraw( C2DRender* p2DRender );
	virtual	void	OnInitialUpdate();
	virtual	BOOL	OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase );
	virtual	void	OnSize( UINT nType, int cx, int cy );
	virtual void	OnLButtonUp( UINT nFlags, CPoint point );
	virtual	void	OnLButtonDown( UINT nFlags, CPoint point );
	void			SetGold( DWORD nCost );
	void			SetTotalGold( __int64 nCost );
	void			SetMinGold( DWORD dwMinGold )		{ m_dwMinGold    = dwMinGold; }
	void			SetReqGold( DWORD dwReqGold )		{ m_dwReqGold    = dwReqGold; }
	void			SetBackupGold( DWORD dwBackupGold ) { m_dwBackupGold = dwBackupGold; }
	void			EnableAccept( BOOL bFlag );	
}; 

class CWndGuildCombatBoard : public CWndNeuz
{
protected:
#if __VER >= 11 // __GUILD_COMBAT_1TO1
public:
	int m_nCombatType;
#endif	//__GUILD_COMBAT_1TO1
public: 
#if __VER >= 11 // __GUILD_COMBAT_1TO1
	CWndGuildCombatBoard(int m_nCombatType);
#else //__GUILD_COMBAT_1TO1
	CWndGuildCombatBoard();
#endif //__GUILD_COMBAT_1TO1
	virtual ~CWndGuildCombatBoard();
	
	virtual	BOOL	Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual	BOOL	OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual	void	OnDraw( C2DRender* p2DRender );
	virtual	void	OnInitialUpdate();
	virtual	BOOL	OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase );
	virtual	void	OnSize( UINT nType, int cx, int cy );
	virtual void	OnLButtonUp( UINT nFlags, CPoint point );
	virtual	void	OnLButtonDown( UINT nFlags, CPoint point );
#if __VER >= 11 // __GUILD_COMBAT_1TO1
	virtual void	PaintFrame( C2DRender* p2DRender );
#endif //__GUILD_COMBAT_1TO1
	void			SetString( CHAR* szChar );
}; 

class CGuildCombatSelectionClearMessageBox : public CWndMessageBox
{ 
public: 
	CString m_strMsg;
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD dwWndId = 0 );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
}; 

class CGuildCombatInfoMessageBox : public CWndNeuz
{
#if __VER >= 11 // __GUILD_COMBAT_1TO1
public:
	int	m_nCombatType;
#endif //__GUILD_COMBAT_1TO1
public:
#if __VER >= 11 // __GUILD_COMBAT_1TO1
	CGuildCombatInfoMessageBox(int nCombatType);
#else //__GUILD_COMBAT_1TO1
	CGuildCombatInfoMessageBox();
#endif //__GUILD_COMBAT_1TO1
	virtual ~CGuildCombatInfoMessageBox();
	void	SetString( CHAR* szChar );
	virtual	void OnInitialUpdate();
	virtual	BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
#if __VER >= 11 // __GUILD_COMBAT_1TO1
	virtual void PaintFrame( C2DRender* p2DRender );
	void	SetString( CString strMsg );
#endif //__GUILD_COMBAT_1TO1
}; 

class CGuildCombatInfoMessageBox2 : public CWndNeuz
{ 
public: 
	CGuildCombatInfoMessageBox2();
	virtual ~CGuildCombatInfoMessageBox2();
	void	SetString( CHAR* szChar );
	virtual	void OnInitialUpdate();
	virtual	BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
}; 



class CWndGuildCombatSelection : public CWndNeuz
{
protected:	
	multimap<int, CGuildMember*>	m_mapSelectPlayer;   // 길드리스트...레벨소팅

	vector<u_long>					m_vecGuildList   ;   // 길드 리스트
	vector<u_long>					m_vecSelectPlayer;   // 참가자 리스트..

	u_long							m_uidDefender;
	CTexture						m_TexDefender;
	int								m_nDefenderIndex;
	
	int								nMaxJoinMember;
	int								nMaxWarMember;
	
public: 
	void Reset();
	CWndGuildCombatSelection();
	virtual ~CWndGuildCombatSelection();
	
	virtual	BOOL	Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual	BOOL	OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual	void	OnDraw( C2DRender* p2DRender );
	virtual	void	OnInitialUpdate();
	virtual	BOOL	OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase );
	virtual	void	OnSize( UINT nType, int cx, int cy );
	virtual void	OnLButtonUp( UINT nFlags, CPoint point );
	virtual	void	OnLButtonDown( UINT nFlags, CPoint point );
	void			EnableFinish( BOOL bFlag );		

	void			SetDefender( u_long uiPlayer );
	void			UpDateGuildListBox();

	void			AddCombatPlayer( u_long uiPlayer );
	void			AddGuildPlayer( u_long uiPlayer );

	void			RemoveCombatPlayer( int nIndex ) ;
	void			RemoveGuildPlayer( int nIndex ) ;

	u_long			FindCombatPlayer( u_long uiPlayer );
	u_long			FindGuildPlayer( u_long uiPlayer );

	void			SetMemberSize( int nMaxJoin,  int nMaxWar );
}; 

class CWndGuildCombatState : public CWndNeuz 
{ 
private: 
	CTimeSpan		m_ct;
	time_t    		m_tEndTime; // timegettime+
	time_t    		m_tCurrentTime; // timegettime+
	
#if __VER >= 11 // __GUILD_COMBAT_1TO1
public:
	int				m_nCombatType;
#endif //__GUILD_COMBAT_1TO1

public:
#if __VER >= 11 // __GUILD_COMBAT_1TO1
	CWndGuildCombatState(int nCombatType);
#else //__GUILD_COMBAT_1TO1
	CWndGuildCombatState();
#endif //__GUILD_COMBAT_1TO1
	virtual ~CWndGuildCombatState(); 
	
	void		 InsertTitle( const char szTitle[] );
	int          GetSelectIndex( const CPoint& point );	
	void		 Init( time_t lTime );
	void		 InsertGuild( const char szGuild[], const char szName[], int nNum );	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual	void OnRButtonUp( UINT nFlags, CPoint point );
	virtual void OnRButtonDown( UINT nFlags, CPoint point );
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point);	
#if __VER >= 11 // __GUILD_COMBAT_1TO1
	virtual void	PaintFrame( C2DRender* p2DRender );
#endif //__GUILD_COMBAT_1TO1
	virtual BOOL Process ();
#if __VER < 11 // __GUILD_COMBAT_1TO1
	void		 SetTotalGold( __int64 nGold );
#endif //__GUILD_COMBAT_1TO1
	void		 SetGold( int nGold );
	void		 SetRate( int nRate );
	void	 	 SetTime( time_t tTime ) { m_tCurrentTime = 0; m_tEndTime = time_null() + tTime; }
}; 

class CWndGuildCombatJoinSelection : public CWndNeuz
{
protected:
	int	   m_nMapNum;
	CTimer m_timerInputTimeOut;
	DWORD  m_dwOldTime;
	
public: 
	CWndGuildCombatJoinSelection();
	virtual ~CWndGuildCombatJoinSelection();
	
	virtual	BOOL	Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual	BOOL	OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual	void	OnDraw( C2DRender* p2DRender );
	virtual	void	OnInitialUpdate();
	virtual	BOOL	OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase );
	virtual	void	OnSize( UINT nType, int cx, int cy );
	virtual void	OnLButtonUp( UINT nFlags, CPoint point );
	virtual	void	OnLButtonDown( UINT nFlags, CPoint point );
	virtual BOOL	Process ();

	void			SetTimeOut( DWORD dwSec ) { m_dwOldTime = g_tmCurrent + dwSec; }
	void			SetMapNum( int nMap ) { m_nMapNum = nMap; }
}; 

class CWndGuildWarAppConfirm : public CWndNeuz
{
protected:
	CWndText	m_wndText;
public: 
	CWndGuildWarAppConfirm();
	virtual ~CWndGuildWarAppConfirm();
	
	virtual	BOOL	Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual	BOOL	OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual	void	OnDraw( C2DRender* p2DRender );
	virtual	void	OnInitialUpdate();
	virtual	BOOL	OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase );
	virtual	void	OnSize( UINT nType, int cx, int cy );
	virtual void	OnLButtonUp( UINT nFlags, CPoint point );
	virtual	void	OnLButtonDown( UINT nFlags, CPoint point );
}; 
class CWndGuildWarCancelConfirm : public CWndNeuz
{
protected:
	CWndText	m_wndText;
#if __VER >= 11 // __GUILD_COMBAT_1TO1
	int			m_nCombatType;
#endif //__GUILD_COMBAT_1TO1
public:
#if __VER >= 11 // __GUILD_COMBAT_1TO1
	CWndGuildWarCancelConfirm(int nCombatType);
#else //__GUILD_COMBAT_1TO1
	CWndGuildWarCancelConfirm();
#endif //__GUILD_COMBAT_1TO1
	virtual ~CWndGuildWarCancelConfirm();
	
	virtual	BOOL	Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual	BOOL	OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual	void	OnDraw( C2DRender* p2DRender );
	virtual	void	OnInitialUpdate();
	virtual	BOOL	OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase );
	virtual	void	OnSize( UINT nType, int cx, int cy );
	virtual void	OnLButtonUp( UINT nFlags, CPoint point );
	virtual	void	OnLButtonDown( UINT nFlags, CPoint point );
#if __VER >= 11 // __GUILD_COMBAT_1TO1
	virtual void	PaintFrame( C2DRender* p2DRender );
#endif //__GUILD_COMBAT_1TO1
}; 
class CWndGuildWarJoinConfirm : public CWndNeuz
{
#if __VER >= 11 // __GUILD_COMBAT_1TO1
public:
	int			m_nCombatType;
#endif //__GUILD_COMBAT_1TO1
protected:
	CWndText	m_wndText;
public:
#if __VER >= 11 // __GUILD_COMBAT_1TO1
	CWndGuildWarJoinConfirm(int nCombatType);
#else //__GUILD_COMBAT_1TO1
	CWndGuildWarJoinConfirm();
#endif //__GUILD_COMBAT_1TO1
	virtual ~CWndGuildWarJoinConfirm();
	
	virtual	BOOL	Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual	BOOL	OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual	void	OnDraw( C2DRender* p2DRender );
	virtual	void	OnInitialUpdate();
	virtual	BOOL	OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase );
	virtual	void	OnSize( UINT nType, int cx, int cy );
	virtual void	OnLButtonUp( UINT nFlags, CPoint point );
	virtual	void	OnLButtonDown( UINT nFlags, CPoint point );
#if __VER >= 11 // __GUILD_COMBAT_1TO1
	virtual void	PaintFrame( C2DRender* p2DRender );
#endif //__GUILD_COMBAT_1TO1
}; 

class CWndGuildWarState : public CWndNeuz 
{ 
private: 
	CWndScrollBar		m_wndScrollBar;
	int					m_nMax;
	GUILDLIST			m_aList[MAX_WANTED_LIST];
	int					m_nSelect;
	
public: 
	CWndGuildWarState(); 
	virtual ~CWndGuildWarState(); 
	
	void		 InsertTitle( const char szTitle[] );
	int          GetSelectIndex( const CPoint& point );	
	void		 Init( time_t lTime );
	void		 InsertGuild( const char szGuild[], const char szName[], int nNum );	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual	void OnRButtonUp( UINT nFlags, CPoint point );
	virtual void OnRButtonDown( UINT nFlags, CPoint point );
	virtual void OnMouseMove(UINT nFlags, CPoint point );
	virtual BOOL OnMouseWheel( UINT nFlags, short zDelta, CPoint pt );
	virtual void OnMouseWndSurface( CPoint point );
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point);	
}; 


struct GUILDNAME
{
	char	 szGuild[ MAX_NAME ];		// 이름.
};

class CWndGuildCombatRanking : public CWndNeuz 
{ 
private: 
	CWndScrollBar		m_wndScrollBar;
	int					m_nMax;
	multimap< int, GUILDNAME > m_multimapRanking;
	int					m_nSelect;
	
public: 
	void SortRanking();
	CWndGuildCombatRanking(); 
	virtual ~CWndGuildCombatRanking(); 
	
	int          GetSelectIndex( const CPoint& point );	
	void		 Init( time_t lTime );
	void		 InsertGuild( const char szGuild[], int nWinCount );	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual	void OnRButtonUp( UINT nFlags, CPoint point );
	virtual void OnRButtonDown( UINT nFlags, CPoint point );
	virtual void OnMouseMove(UINT nFlags, CPoint point );
	virtual BOOL OnMouseWheel( UINT nFlags, short zDelta, CPoint pt );
	virtual void OnMouseWndSurface( CPoint point );
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point);	
}; 

// 길드 결과 로그 - 길드
class CWndGuildCombatTabResultRate : public CWndNeuz 
{ 
public: 
	CWndGuildCombatTabResultRate(); 
	virtual ~CWndGuildCombatTabResultRate(); 
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
}; 
// 길드 결과 로그 - 개인
class CWndGuildCombatTabResultLog : public CWndNeuz 
{ 
public: 
	CWndGuildCombatTabResultLog(); 
	virtual ~CWndGuildCombatTabResultLog(); 
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
}; 

// 길드 결과 로그창
class CWndGuildCombatResult : public CWndNeuz 
{ 
public: 
	void InsertLog( CString str );
	void InsertPersonRate( CString str );
	void InsertGuildRate( CString str );
	CWndGuildCombatResult(); 
	virtual ~CWndGuildCombatResult(); 
	
	CWndGuildCombatTabResultRate   m_WndGuildCombatTabResultRate;
	CWndGuildCombatTabResultLog    m_WndGuildCombatTabResultLog;
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual void OnMouseMove(UINT nFlags, CPoint point );
}; 



typedef struct __GUILDCOMBAT_RANK_INFO
{
	u_long	uidPlayer;
	int		nJob;
} __GUILDCOMBAT_RANK_INFO;

typedef struct __GUILDCOMBAT_RANK_INFO2
{
	CString strName;
	CString strJob;
	u_long	uidPlayer;
	int		nPoint;	
} __GUILDCOMBAT_RANK_INFO2;

#define MAX_GUILDCOMBAT_RANK_PER_PAGE 11
#define MAX_GUILDCOMBAT_RANK		  100


// 길드 랭킹 탭- 직업별
class CWndGuildCombatRank_Class : public CWndNeuz 
{ 
public: 
	CWndScrollBar		m_wndScrollBar;
	int					m_nMax;
	int					m_nSelect;

	int		m_nRate;
	int		m_nOldRate;

	__GUILDCOMBAT_RANK_INFO2		m_listRank[MAX_GUILDCOMBAT_RANK];

	CWndGuildCombatRank_Class(); 
	virtual ~CWndGuildCombatRank_Class(); 

	void		 InsertRank( int nJob, u_long uidPlayer, int nPoint );
	int          GetSelectIndex( const CPoint& point );	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual BOOL OnMouseWheel( UINT nFlags, short zDelta, CPoint pt );
	virtual	void OnLButtonDown( UINT nFlags, CPoint point );
}; 

// 길드 랭킹 - 직업별
class CWndGuildCombatRank_Person : public CWndNeuz 
{ 
public: 
	multimap< int, __GUILDCOMBAT_RANK_INFO > m_mTotalRanking;
			
	CWndGuildCombatRank_Person(); 
	virtual ~CWndGuildCombatRank_Person(); 
	
	CWndGuildCombatRank_Class    m_WndGuildCombatTabClass_Tot;
	CWndGuildCombatRank_Class    m_WndGuildCombatTabClass_Mer;
	CWndGuildCombatRank_Class    m_WndGuildCombatTabClass_Mag;
	CWndGuildCombatRank_Class    m_WndGuildCombatTabClass_Acr;
	CWndGuildCombatRank_Class    m_WndGuildCombatTabClass_Ass;
	
	CWndGuildCombatRank_Class*  __GetJobKindWnd( int nJob );
	void						InsertRank( int nJob, u_long	uidPlayer, int nPoint );
	void						UpdateList();
	void						DivisionList();
	void						UpdatePlayer( int nJob, u_long idPlayer );
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual void OnMouseMove(UINT nFlags, CPoint point );

}; 

class CWndFontEdit : public CWndNeuz 
{ 
	CTexture*        m_pTexture;
	CPoint			 m_ColorScrollBar[3];
	CPoint			 m_OriginalColorScrollBar[3];
	
	CRect			 m_ColorRect[3];
	FLOAT			 m_fColor[3];
	
	BOOL			 m_bLButtonClick;
	
public: 
	CWndText* m_pWndText;
	CWndFontEdit(); 
	virtual ~CWndFontEdit(); 
	
	void	ReSetBar( FLOAT r, FLOAT g, FLOAT b );
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual void OnMouseWndSurface( CPoint point );
}; 

#if __VER >= 8 //__CSC_VER8_3

#include "WndWorld.h"

class CWndBuffStatus : public CWndNeuz 
{
public:
	vector< multimap<DWORD, BUFFSKILL> > m_pBuffTexture;
	list<BUFFICON_INFO> m_pBuffIconInfo;

	int m_BuffIconViewOpt;
public:
	CWndBuffStatus(); 
	virtual ~CWndBuffStatus(); 
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual void OnRButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnRButtonDown( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point );
	virtual void OnRButtonDblClk( UINT nFlags, CPoint point );
	virtual void OnMouseWndSurface( CPoint point );
	virtual	void PaintFrame( C2DRender* p2DRender );

#ifdef __BUFF_1107
	void RenderBuffIcon( C2DRender *p2DRender, IBuff* pBuff, BOOL bPlayer, BUFFICON_INFO* pInfo, CPoint ptMouse );
#else	// __BUFF_1107
	void RenderBuffIcon( C2DRender *p2DRender, SKILLINFLUENCE* pSkill, BOOL bPlayer, BUFFICON_INFO* pInfo, CPoint ptMouse );
#endif	// __BUFF_1107
	void RenderOptBuffTime(C2DRender *p2DRender, CPoint& point, CTimeSpan &ct, DWORD dwColor );
	void SetBuffIconInfo();
	BOOL GetHitTestResult();
};
#endif //__CSC_VER8_3

#if __VER >= 9 // __CSC_VER9_1
/*******************************
	제련 시스템 관련 Window
********************************/
#include "WndSummonAngel.h"

class CWndMixJewelConfirm : public CWndNeuz
{
public: 
	CWndMixJewelConfirm(); 
	virtual ~CWndMixJewelConfirm(); 
	
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

class CWndMixJewel : public CWndNeuz 
{ 
public:
	CWndText* m_pText;
	GENMATDIEINFO m_MatJewel[MAX_JEWEL];	// cr : uw :
	ItemCountSet m_ItemInfo[MAX_JEWEL];		// cr : uw :
	
	int m_nitemcount;
	int m_nSelectCtrl;
	int m_nOrichalcum;
	int m_nMoonstone;
	
	BOOL m_bStart;
	
	CWndInventory* m_pWndInventory;
	CWndMixJewelConfirm* m_pConfirm;
public: 
	
	CWndMixJewel(); 
	~CWndMixJewel(); 
	
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
	virtual	void	OnDestroyChildWnd( CWndBase * pWndChild );	// cr : uw :
	
	void SetDescription( CHAR* szChar );		
	int HitTest( CPoint point );
	void ReceiveResult(int nResult);
	void SetJewel(CItemElem* pItemElem);
	void SetStartBtn(BOOL buse);
	void SetConfirmInit();
};

class CWndExtraction : public CWndNeuz
{
public:
	CWndText* m_pText;
	CItemElem* m_pItemElem;
	ItemProp* m_pEItemProp;
	
public: 
	CWndExtraction(); 
	virtual ~CWndExtraction(); 
	
	virtual void OnDestroy();
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point );
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point );
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point );
	
	void SetDescription(CHAR* szChar);
	void ReceiveResult(int result, int nCount);
	void SetWeapon(CItemElem* pItemElem);
};

class CWndSmeltJewel : public CWndNeuz
{
public:
	CModelObject* m_pMainItem;
	CItemElem* m_pItemElem;
	CItemElem* m_pJewelElem;
	CWndText* m_pText;
	
	DWORD m_dwJewel[5];
	int m_nJewelSlot[5];
	int m_nJewelCount;
	int m_nUsableSlot;
	OBJID m_objJewelId;

	float m_fRotate;
	float m_fAddRot;

	int m_nStatus;
	int m_nCount;
	int m_nDelay;
	int m_nAlpha;
	float m_nEyeYPos;
	BOOL m_bFlash;
	
public:
	CWndSmeltJewel(); 
	~CWndSmeltJewel();
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point );
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point );
	virtual	void OnDestroy( void );
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point );
	virtual BOOL Process ();
	
	void SetDescription(CHAR* szChar);
	void ReceiveResult(int result);
	void InitializeJewel(CItemElem* pItemElem);
	void SetJewel(CItemElem* pItemElem);
};

class CWndChangeWeapon : public CWndNeuz
{
public:
	CWndText* m_pText1;
	CWndText* m_pText2;
	CItemElem* m_pWItemElem;
	CItemElem* m_pJItemElem[2];
	int m_nWeaponType;
	BOOL m_bIsSendChange;
	
public:
	CWndChangeWeapon(int nType);
	virtual ~CWndChangeWeapon(); 
	
	virtual void OnDestroy();
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point );
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point );
	virtual void OnMouseWndSurface( CPoint point );	
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point );
	virtual BOOL Process ();
	
	void SetDescription(CHAR* szChar);
	void ReceiveResult(int result);
	void SetItem(CItemElem* pItemElem);
};

class CWndRemoveJewelConfirm : public CWndNeuz 
{
public:
	CWndInventory* m_pInventory;
	CItemBase*	m_pUpgradeItem;

public: 
	CWndRemoveJewelConfirm(); 
	virtual ~CWndRemoveJewelConfirm(); 
	
	virtual void OnDestroy();
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point );

	void SetItem(CItemBase*	m_pItem);
};
#endif //__CSC_VER9_1

#if __VER >= 10 // __CSC_VER9_1 -> __LEGEND
/*******************************
	전승 시스템 관련 Window
********************************/
class CWndHeroSkillUp : public CWndNeuz
{
public:
	CWndText* m_pText;
	CItemElem* m_pItemElem[5];
	int m_JewelID[5];
	int m_SlotID[5];
	int m_PicJewel[5];
	BOOL m_bSendHeroSkillup;
	
public: 
	CWndHeroSkillUp(); 
	virtual ~CWndHeroSkillUp(); 
	
	virtual void OnDestroy();
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point );
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point );
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point );
	
	int HitTest( CPoint point );
	void SetDescription(CHAR* szChar);
	void ReceiveResult(int nresult);
	void SetJewel(CItemElem* pItemElem);
};

#endif //__CSC_VER9_1 -> __LEGEND

#ifdef __TRADESYS

#define MAX_LIST_COUNT 15

class CWndDialogEvent : public CWndNeuz
{
public:
	vector<int> m_nVecList;
	vector<int> m_nDescList;

	int m_nChoiceNum;
	int m_nListCount;
	int m_nDescCount;
	int m_nCurrentPage;
	int m_nMMI;
	int m_nGap;			//List Gap
	int m_nListOffset;	//Selected List Rect Offset
	
public:
	CWndDialogEvent();
	virtual ~CWndDialogEvent(); 
	
	virtual void OnDestroy();
	virtual BOOL Initialize( CWndBase* pWndParent = NULL,DWORD dwWndId = 0 ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	
	void SetDescription(CHAR* szChar);
	void SetMMI(int mmi);
	void ReceiveResult(int result);
};
#endif //__TRADESYS

#if __VER >= 12 // __HEAVEN_TOWER
#define MAX_FLOOR_COUNT 15

class CWndHeavenTower : public CWndNeuz
{
public:
	vector<int> m_nVecList;
	vector<int> m_nDescList;
	vector<int> m_nMsgList;
	vector<int>	m_nCost;

	int m_nChoiceNum;
	int m_nListCount;
	int m_nDescCount;
	int m_nCurrentPage;
	int m_nGap;			//List Gap
	int m_nListOffset;	//Selected List Rect Offset
	
public:
	CWndHeavenTower();
	virtual ~CWndHeavenTower(); 
	
	virtual void OnDestroy();
	virtual BOOL Initialize( CWndBase* pWndParent = NULL,DWORD dwWndId = 0 ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	
	void InitText();
	void InitWnd();
};

class CWndHeavenTowerEntranceConfirm : public CWndMessageBox
{ 
public: 
	DWORD	m_nFloor;

public:
	void	SetValue( CString str, DWORD nFloor );
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD dwWndId = 0 );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
}; 

#endif //__HEAVEN_TOWER

#if __VER >= 10 // __REMOVE_ATTRIBUTE

class CWndRemoveAttributeConfirm : public CWndNeuz 
{
public: 
	CWndRemoveAttributeConfirm(); 
	virtual ~CWndRemoveAttributeConfirm(); 
	
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

class CWndRemoveAttribute : public CWndNeuz
{
public:
	CWndText* m_pText;
	CItemElem* m_pItemElem;
	ItemProp* m_pEItemProp;
	CTexture* m_pTexture;
	
	CWndRemoveAttributeConfirm* m_pWndConfirm;

public: 
	CWndRemoveAttribute(); 
	virtual ~CWndRemoveAttribute(); 
	
	virtual void OnDestroy();
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point );
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point );
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point );
	
	void SetDescription(CHAR* szChar);
	void ReceiveResult(BOOL result);
	void SetWeapon(CItemElem* pItemElem);
};

#endif //__REMOVE_ATTRIBUTE

#if __VER >= 11 // __PIERCING_REMOVE
class CWndRemovePiercing : public CWndNeuz
{
public:
	CWndText*	m_pText;
	CItemElem*	m_pItemElem;
	ItemProp*	m_pEItemProp;
	CTexture*	m_pTexture;
#if __VER >= 12 // __CSC_VER12_4
	int			m_nInfoSlot[10];
#endif //__CSC_VER12_4
	
public: 
	CWndRemovePiercing(); 
	virtual ~CWndRemovePiercing(); 
	
	virtual void OnDestroy();
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point );
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point );
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point );
	
	void SetDescription(CHAR* szChar);
#if __VER >= 12 // __CSC_VER12_4
	virtual void OnMouseWndSurface( CPoint point );
	void SetItem(CItemElem* pItemElem);
#else //__CSC_VER12_4
	void SetSuit(CItemElem* pItemElem);
#endif //__CSC_VER12_4
};
#endif //__PIERCING_REMOVE

#if __VER >= 12 // __CSC_VER12_4
class CWndRemoveJewel : public CWndNeuz
{
public:
	CWndText*	m_pText;
	CItemElem*	m_pItemElem;
	CItemElem*	m_pMoonstone;
	ItemProp*	m_pEItemProp;
	CTexture*	m_pTexture;
	CTexture*	m_pMoonstoneTex;
	int			m_nJewelCount;
	int			m_nJewelID[5];
	CTexture*	m_pJewelTex[5];
	int			m_nJewelSlot[5];
	int			m_nInfoSlot[5];
	
public: 
	CWndRemoveJewel(); 
	virtual ~CWndRemoveJewel(); 
	
	virtual void OnDestroy();
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point );
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point );
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point );
	virtual void OnMouseWndSurface( CPoint point );

	void SetDescription(CHAR* szChar);
	void SetItem(CItemElem* pItemElem);

private:
	void ResetJewel();
};
#endif //__CSC_VER12_4

#if __VER >= 13 // __EXT_ENCHANT
class CWndChangeAttribute : public CWndNeuz 
{
public:
	CWndText* m_pText;
	int m_nAttributeNum;
	int m_nAttributeStaticID[5];
	int m_nTooltipTextIndx[6];
	CItemElem* m_pItemElem;
	CItemElem* m_pChangeItem;
	CTexture* m_pTexture;

public: 
	CWndChangeAttribute(); 
	virtual ~CWndChangeAttribute();
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual void OnLButtonUp( UINT nFlags, CPoint point );
	virtual BOOL OnSetCursor( CWndBase* pWndBase, UINT nHitTest, UINT message );
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point );
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point );
	virtual BOOL Process();

public:
	void SetChangeItem( CItemElem* pItemElem );
	void FillRect(C2DRender *p2DRender, CRect rectBg, DWORD dwColorstart, DWORD dwColorend);
};
#endif //__EXT_ENCHANT

#if __VER >= 13 // __CSC_VER13_2
class CWndCoupleMessage : public CWndMessageBox
{
public:
	enum { CM_SENDPROPOSE = 0, CM_RECEIVEPROPOSE, CM_CANCELCOUPLE };

	CString m_strText;
	int m_nMode;
	CObj* m_pTargetObj;

public:	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD dwWndId = 0 );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );

public:
	void SetMessageMod(CString strText, int nMode, CObj* pTargetObj = NULL);
};

class CWndCoupleTabInfo : public CWndNeuz 
{
public:
	CCouple* m_pCouple;

	CTexture m_texGauEmptyNormal;
	CTexture m_texGauFillNormal;
	LPDIRECT3DVERTEXBUFFER9 m_pVBGauge;

public: 
	CWndCoupleTabInfo(); 
	virtual ~CWndCoupleTabInfo();
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();	
};

class CWndCoupleTabSkill : public CWndNeuz 
{
public:
	CWndText* m_pText;
	CTexture* m_pSkillBgTexture;

public: 
	CWndCoupleTabSkill(); 
	virtual ~CWndCoupleTabSkill();
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate();
	virtual void OnDraw(C2DRender* p2DRender);
};

class CWndCoupleManager : public CWndNeuz 
{
public:
	CWndCoupleTabInfo	m_wndCoupleTabInfo;
	CWndCoupleTabSkill	m_wndCoupleTabSkill;
public: 
	CWndCoupleManager(); 
	virtual ~CWndCoupleManager();
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate();
};
#endif //__CSC_VER13_2

#ifdef __FUNNY_COIN
class CWndFunnyCoinConfirm : public CWndNeuz 
{
public:
	DWORD m_dwItemId;
	CItemElem* m_pItemElem;

public: 
	CWndFunnyCoinConfirm(); 
	virtual ~CWndFunnyCoinConfirm(); 
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate(); 
	void SetInfo(DWORD dwItemId, CItemElem* pItemElem);
};
#endif //__FUNNY_COIN

#if __VER >= 14 // __SMELT_SAFETY

class CWndSmeltSafety : public CWndNeuz
{
public:
#if __VER >= 15 // __15_5TH_ELEMENTAL_SMELT_SAFETY
	enum WndMode { WND_NORMAL, WND_ACCESSARY, WND_PIERCING, WND_ELEMENT };
#else // __15_5TH_ELEMENTAL_SMELT_SAFETY
	enum WndMode {WND_NORMAL, WND_ACCESSARY, WND_PIERCING};
#endif // __15_5TH_ELEMENTAL_SMELT_SAFETY
	enum { SMELT_MAX = 10 };
	enum { ENCHANT_TIME = 2 };
	enum { EXTENSION_PIXEL = 32, HALF_EXTENSION_PIXEL = EXTENSION_PIXEL / 2 };
#if __VER >= 15 // __15_5TH_ELEMENTAL_SMELT_SAFETY
	enum { GENERAL_NON_USING_SCROLL2_LEVEL = 7, ELEMENTAL_NON_USING_SCROLL2_LEVEL = 10 };
#endif // __15_5TH_ELEMENTAL_SMELT_SAFETY

private:
	WndMode m_eWndMode;
	CItemElem* m_pItemElem;
	CTexture* m_pItemTexture;
	CTexture* m_pNowGaugeTexture;
	CTexture* m_pSuccessTexture;
	CTexture* m_pFailureTexture;
	int m_nMaterialCount;
	int m_nScroll1Count;
	int m_nScroll2Count;
	int m_nResultCount;
	BOOL m_bStart;
	bool m_bResultSwitch;
	DWORD m_dwEnchantTimeStart;
	DWORD m_dwEnchantTimeEnd;
	float m_fGaugeRate;
	int m_nValidSmeltCounter;
	int m_nCurrentSmeltNumber;
	int m_nResultStaticID[SMELT_MAX];
	GENMATDIEINFO m_Material[SMELT_MAX];
	GENMATDIEINFO m_Scroll1[SMELT_MAX];
	GENMATDIEINFO m_Scroll2[SMELT_MAX];
	bool m_bResultStatic[SMELT_MAX];
	LPDIRECT3DVERTEXBUFFER9 m_pVertexBufferGauge;
	LPDIRECT3DVERTEXBUFFER9 m_pVertexBufferSuccessImage;
	LPDIRECT3DVERTEXBUFFER9 m_pVertexBufferFailureImage;
#if __VER >= 15 // __15_5TH_ELEMENTAL_SMELT_SAFETY
	ItemProp* m_pSelectedElementalCardItemProp;
#endif // __15_5TH_ELEMENTAL_SMELT_SAFETY

public:
	CWndSmeltSafety(WndMode eWndMode);
	virtual ~CWndSmeltSafety();

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual	void OnInitialUpdate();
	virtual BOOL Process();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point = 0 );
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point);
	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();

	void SetItem(CItemElem* pItemElem);
	void RefreshInformation(void);
	void RefreshText(void);
	void RefreshValidSmeltCounter(void);
	void StopSmelting(void);
	void DisableScroll2(void);
	void ResetData(void);
	void AddListItem(GENMATDIEINFO* pListItem, CItemElem* pItemElem);
	void SubtractListItem(GENMATDIEINFO* pListItem);
	void DrawListItem(C2DRender* p2DRender);
	BOOL IsDropMaterialZone(CPoint point);
	BOOL IsDropScroll1Zone(CPoint point);
	BOOL IsDropScroll2Zone(CPoint point);
	BOOL IsAcceptableMaterial(ItemProp* pItemProp);
	BOOL IsAcceptableScroll1(ItemProp* pItemProp);
	BOOL IsAcceptableScroll2(ItemProp* pItemProp);
	int GetNowSmeltValue(void);
	int GetDefaultMaxSmeltValue(void);
	void SetResultSwitch(bool bResultSwitch) { m_bResultSwitch = bResultSwitch; }
	void SetCurrentSmeltNumber(int nCurrentSmeltNumber) { m_nCurrentSmeltNumber = nCurrentSmeltNumber; }
	void SetResultStatic(bool bResultStatic, int nIndex) { m_bResultStatic[nIndex] = bResultStatic; }
	CItemElem* GetItemElement(void) const { return m_pItemElem; }
	bool GetResultSwitch(void) const { return m_bResultSwitch; }
	int GetCurrentSmeltNumber(void) const { return m_nCurrentSmeltNumber; }
	int GetResultStaticID(int nIndex) const { return m_nResultStaticID[nIndex]; }
};

class CWndSmeltSafetyConfirm : public CWndNeuz
{
public:
	enum ErrorMode { WND_ERROR1, WND_ERROR2, WND_ERROR3 };

private:
	ErrorMode m_eErrorMode;
	CItemElem* m_pItemElem;

public:
	CWndSmeltSafetyConfirm(ErrorMode eErrorMode);
	virtual ~CWndSmeltSafetyConfirm();

public:
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual	void OnInitialUpdate();
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual void OnDestroy( void );

	void SetWndMode(CItemElem* pItemElem);
};

#endif //__SMELT_SAFETY

#if __VER >= 14 // __EQUIP_BIND
class CWndEquipBindConfirm : public CWndNeuz
{
public:
	enum EquipAction { EQUIP_DOUBLE_CLICK, EQUIP_DRAG_AND_DROP };

public:
	CWndEquipBindConfirm(EquipAction eEquipAction);
	virtual ~CWndEquipBindConfirm(void);

public:
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual	void OnInitialUpdate( void );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );

public:
	void SetInformation(CItemBase* pItemBase, DWORD dwObjId);
	void SetInformation(CItemElem* pItemElem);

private:
	void EquipItem( void );

private:
	EquipAction m_eEquipAction;
	CItemBase* m_pItemBase;
	DWORD m_dwObjId;
	CItemElem* m_pItemElem;
};
#endif // __EQUIP_BIND

#if __VER >= 14 // __RESTATE_CONFIRM
class CWndRestateConfirm : public CWndNeuz
{
public:
	CWndRestateConfirm(DWORD dwItemID);
	virtual ~CWndRestateConfirm(void);

public:
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual	void OnInitialUpdate( void );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );

public:
	void SetInformation(DWORD dwItemObjID, OBJID m_ObjID, int nPart = -1);

private:
	DWORD m_dwItemID;
	DWORD m_dwItemObjID;
	OBJID m_ObjID;
	int m_nPart;
};
#endif // __RESTATE_CONFIRM

#if __VER >= 15 // __CAMPUS
class CWndCampusInvitationConfirm : public CWndNeuz
{
public:
	CWndCampusInvitationConfirm( u_long idSender = 0, const CString& rstrSenderName = _T( "" ) );
	virtual ~CWndCampusInvitationConfirm( void );

public:
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual	void OnInitialUpdate( void );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );

private:
	u_long m_idSender;
	CString m_strSenderName;
};

class CWndCampusSeveranceConfirm : public CWndNeuz
{
public:
	CWndCampusSeveranceConfirm( u_long idTarget = 0, const CString& rstrTargetName = _T( "" ) );
	virtual ~CWndCampusSeveranceConfirm( void );

public:
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual	void OnInitialUpdate( void );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );

private:
	u_long m_idTarget;
	CString m_strTargetName;
};
#endif // __CAMPUS

#ifdef __NEW_ITEM_VARUNA
class CWndBarunaNPC : public CWndNeuz
{
public:
	CWndBarunaNPC( );
	virtual ~CWndBarunaNPC( );
	
	virtual void OnDestroy();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL,DWORD dwWndId = 0);
	virtual BOOL Process();
	virtual void SetInfo( DWORD dwItemId );
//	virtual void PaintFrame( C2DRender* p2DRender );

	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	
	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();

	CTexture m_texGauEmptyNormal ;
	CTexture m_texGauEmptySmall  ;
	CTexture m_texGauFillNormal  ;
	CTexture m_texGauFillSmall   ;
	LPDIRECT3DVERTEXBUFFER9 m_pVBEXPGauge;
	BOOL m_bVBEXPGauge;
	int m_nEXPWidth;
	
	void MakeGaugeVertex();

	DWORD m_dwItemId;
protected:
	CWndStatic* m_pLevelStatic;

	int m_nLevel;
	int m_nFailCounter;
	int m_nFailLevelTable[5];

	CModelObject* m_pPetModel; // Pet Model Object.
	float	m_fRadius;
};


class CWndBarunaExtractOffer : public CWndNeuz
{
public:
	CItemElem* m_pItemElem;
	ItemProp* m_pEItemProp;
	CTexture* m_pTexture;
public: 
	CWndBarunaExtractOffer(); 
	virtual ~CWndBarunaExtractOffer(); 

	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point );
};
class CWndBarunaOffer : public CWndNeuz
{
public:
	CItemElem* m_pItemElem;
	ItemProp* m_pEItemProp;
	CTexture* m_pTexture;

	CItemElem* m_pItemElem2;
	ItemProp* m_pEItemProp2;
	CTexture* m_pTexture2;
public: 
	CWndBarunaOffer(); 
	virtual ~CWndBarunaOffer(); 

	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point );
};
class CWndBarunaEnchant : public CWndNeuz
{
public:
	CItemElem* m_pItemElem;
	ItemProp* m_pEItemProp;
	CTexture* m_pTexture;

	CItemElem* m_pItemElem2;
	ItemProp* m_pEItemProp2;
	CTexture* m_pTexture2;

	CItemElem* m_pItemElem3;
	ItemProp* m_pEItemProp3;
	CTexture* m_pTexture3;
public: 
	CWndBarunaEnchant(); 
	virtual ~CWndBarunaEnchant(); 

	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point );
};
class CWndBarunaSeed : public CWndNeuz
{
public:
	CItemElem* m_pItemElem;
	ItemProp* m_pEItemProp;
	CTexture* m_pTexture;

	CItemElem* m_pItemElem2;
	ItemProp* m_pEItemProp2;
	CTexture* m_pTexture2;
public: 
	CWndBarunaSeed(); 
	virtual ~CWndBarunaSeed(); 

	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point );
};
class CWndBarunaOfferSeed : public CWndNeuz
{
public:
	CItemElem* m_pItemElem;
	ItemProp* m_pEItemProp;
	CTexture* m_pTexture;

	CItemElem* m_pItemElem2;
	ItemProp* m_pEItemProp2;
	CTexture* m_pTexture2;

	CItemElem* m_pItemElem3;
	ItemProp* m_pEItemProp3;
	CTexture* m_pTexture3;
public: 
	CWndBarunaOfferSeed(); 
	virtual ~CWndBarunaOfferSeed(); 

	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point );
};
#ifdef __BARUNA_PIERCING
class CWndBarunaPiercing : public CWndNeuz
{
public:
	DWORD	m_dwItemId[4];
	CTexture* m_pTexture[4];
	CItemElem* m_pItemElem[4];
	LPWNDCTRL m_wndCtrl[4];
	
public: 
	CWndBarunaPiercing(); 
	virtual ~CWndBarunaPiercing(); 

	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point );
};
#endif // __BARUNA_PIERCING

#endif // __NEW_ITEM_VARUNA
#ifdef __FASHION_COMBINE
class CWndFashionCombine : public CWndNeuz
{
public:
	CItemElem* m_pItemElem[12];
	CTexture*  m_pTexture[12];

public: 
	CWndFashionCombine(); 
	virtual ~CWndFashionCombine(); 

	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point );
};
#endif // __FASHION_COMBINE
#ifdef __COLOSSEUM
#endif // __COLOSSEUM

#ifdef __TREASUREBOX
class CWndTreasureBox : public CWndNeuz 
{
public:
	
	float m_nSmeltWidth;
	LPDIRECT3DVERTEXBUFFER9 m_pVBSmeltGauge;
	CTexture m_texGauEmptyNormal;
	BOOL m_bVBSmeltGauge;
	BOOL m_bStart;
	DWORD m_dwStartTime;
	DWORD m_dwEndTime;

	int m_nDelay;
	float m_fGaugeRate;

	CWndTreasureBox( DWORD dwObjID ); 
	virtual ~CWndTreasureBox(); 
	
	CItemElem* m_pItemElem;
	CItemElem* m_pBoxItem;
	CTexture* m_pTexture;
	CRect m_nCtrlId;
	CRect m_nBoxRect;

	virtual BOOL    OnDropIcon( LPSHORTCUT pShortcut, CPoint point );

	virtual BOOL	Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL	OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void	OnDraw( C2DRender* p2DRender ); 
	virtual	void	OnInitialUpdate(); 
	virtual void	OnLButtonDblClk( UINT nFlags, CPoint point ); 
	virtual	void	OnDestroy( void );
	virtual BOOL	Process();
	
	HRESULT RestoreDeviceObjects();
	HRESULT InvalidateDeviceObjects();
	HRESULT DeleteDeviceObjects();

	BOOL IsOpenKey( CItemElem* pKey );
}; 
class CWndTreasureRecieve : public CWndNeuz 
{
public:
	DWORD m_dwTreasures[3];
	int	m_nCount[3];
	CRect m_nCtrlId[3];
	CTexture* pTexture[3];

	CWndTreasureRecieve();
	virtual ~CWndTreasureRecieve();
	void InsertItem( DWORD dwID, int idx, int nCount );
	virtual BOOL	Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL	OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void	OnDraw( C2DRender* p2DRender ); 
	virtual	void	OnInitialUpdate(); 

};
#endif//__TREASUREBOX


#endif // !defined(AFX_WNDFIELD_H__A93F3186_63D6_43C1_956F_EC8691E0C7D9__INCLUDED_)