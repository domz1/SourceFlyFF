
//Guild house windows
//date : 2009_10_26
//author : gmpbigsun

#pragma  once

#if __VER >= 15 // __GUILD_HOUSE

#include "WndHousing.h"
#include "GuildHouse.h"

class CWndGHTeleporter;
class CWndGHUpkeep;
class CWndGHMainMenu : public CWndNeuz			//GH : Guild House 
{
	// 길드하우스 메인메뉴 윈도우
public:
	CWndGHMainMenu( );
	virtual ~CWndGHMainMenu( );

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
//	virtual void OnMouseWndSurface( CPoint point );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual	BOOL Process();

	void RefreshWndTelelpoter( );

protected:
	CWndButton* m_pSetupPower;
	CWndButton* m_pFurniture;
	CWndButton* m_pStorage;
	CWndButton* m_pTeleporter;
	CWndButton* m_pUpkeep;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndGHTeleporter
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define MAX_TELEPORTER_SLOT  (5)

class CWndGHTeleporter : public CWndNeuz
{
	// 텔레포터 , 
	// 2009_11_17 개발도중 길드가구창으로 편입댐 , 현재 쓰이지 않음 
public:
	CWndGHTeleporter( );
	virtual ~CWndGHTeleporter( );

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual	void OnInitialUpdate(); 
//	virtual void OnMouseWndSurface( CPoint point );

	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual void OnMouseWndSurface( CPoint point );
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point );
	virtual void OnLButtonDown( UINT nFlags, CPoint point );
	virtual void OnDraw(C2DRender* p2DRender);
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point);

	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();

	
	int GetSlotIndexByPoint( const CPoint& point );
	void RefreshItems( );		// 서버정보를 기반으로 텔레포터정보만을 빼와서 클라데이터로 재구성 
	void UpdateButtons( );		// 현재 선택한 슬롯에 의거 버튼 활성화 상태 갱신 

protected:
	void DrawSlotItems( C2DRender* p2DRender );
	GH_Fntr_Info* GetInstalledItem( OUT int& serverSlot );		//골때린다.정말  서버데이터에서 찾은 설치된 텔레포터 얻기 out:서버데이터 인덱스 

	int GetCountListUp( );	//리스트업댄  총 갯수얻기 

protected:
	CWndButton* m_pOK;
	CWndButton* m_pFire;

	int			m_nCtrlId[MAX_TELEPORTER_SLOT];
	CTexture*	m_pTexture[MAX_TELEPORTER_SLOT];
	vector<HOUSING_ITEM>	m_cWndItems;
	int			m_nSelectedSlot;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//CWndGHUpkeep
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CWndGHUpkeep : public CWndNeuz
{ 
public: 
	CWndGHUpkeep(); 
	virtual ~CWndGHUpkeep(); 

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 

protected:
	void		OnOK();
	void		OnChangeBuyCount( DWORD dwBuy );

public: 
	CWndEdit*		m_pEdit;
	CWndStatic*		m_pStatic;
	CWndStatic*		m_pStaticGold;
#if __VER >= 11 // __CSC_VER11_3
	int				m_nBuyType;
#endif //__CSC_VER11_3
};


#endif 