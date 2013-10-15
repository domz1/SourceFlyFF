#ifndef __WNDHOUSING__H
#define __WNDHOUSING__H
#pragma once

#if __VER >= 13 // __HOUSING
#include "Housing.h"

struct HOUSING_ITEM
{
	CString		m_strName;
	CString		m_strDesc;
	time_t		m_dwTime;
	BOOL		m_bDeploy;
	int			m_nType;
	int			m_nIndex;
	DWORD		dwItemId;		// 아이템 ID

#if __VER >= 15 // __GUILD_HOUSE
	int			m_nSlotIndex;		//서버에서 관리하는 가구벡터 인덱스 
	D3DXVECTOR3 m_vPos;
	float		m_fAngle;
	int			m_nTeleIndex;
#endif

	HOUSING_ITEM() {Init();};
	~HOUSING_ITEM() {};

	void Init() {m_strName.Empty(); m_strDesc.Empty(); m_dwTime = m_bDeploy = m_nType = m_nIndex = dwItemId = 0;};
};


class CWndHousing : public CWndNeuz 
{
private:
	// 서버에서 주는 정보
	vector<HOUSINGINFO>		m_vecItem;
	// 실제 창에서 쓰일 정보들
	vector<HOUSING_ITEM>	m_mapItem;
	// 소팅 타입
	int						m_nSortType;
	BOOL					m_bIsGreater;
	int						m_nSelected;
public: 
	CWndHousing(); 
	virtual ~CWndHousing(); 
	
	virtual void SerializeRegInfo( CAr& ar, DWORD& dwVersion );
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual BOOL OnMouseWheel( UINT nFlags, short zDelta, CPoint pt );
	
	void RefreshItemList( );
	void Sort();
	
	
}; 

#endif // __HOUSING

#if __VER >= 15 // __GUILD_HOUSE

class CWndGHouseShowOneUnit : public CWndNeuz			
{
	// 설치 재설치중 대상정보를 출력해주는 ...
public:
	CWndGHouseShowOneUnit( );
	virtual ~CWndGHouseShowOneUnit( );

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual	void OnInitialUpdate(); 
	virtual void OnDraw( C2DRender* p2DRender ); 

	void SetItem( const HOUSING_ITEM& kItem )	{ m_kItem = kItem; }

protected:
	HOUSING_ITEM m_kItem;
};


static const int GH_MAX_VIEW_CAPACITY = 7;	//리스트에서 보여줄 최대 허용량

class CWndGuildHousing : public CWndNeuz
{
public:
	enum GH_SECTION		// 구분 ( 해당 속성만 리스트에서 보여준다. )
	{
		GS_ALL, 
		GS_FURNITURE,
		GS_TELEPORTER,
	};
		
	CWndGuildHousing( );
	virtual ~CWndGuildHousing( );

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual	void OnInitialUpdate(); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point );
	virtual	BOOL Process();

	void RefreshItemList( );
	void Sort();
	BOOL SetSelectedByObjID( OBJID objID );
	void SetSection( GH_SECTION eSection )		{ m_eSection = eSection; }
	BOOL IsSection( GH_SECTION eSection )		{ return ( eSection == m_eSection ); }
	BOOL InitBySection( GH_SECTION eSection );
	int  GetWndItemIndexByObjID( OBJID objID );

protected:
	void UpdateIRButton( );			//IR: Install/Recall
	void UpdateSortTextColor( int oldType, int newType );
	void SetEnableInstallBtns( BOOL bEnable );
	void CheckChannel( );	// 설치 해체등이 가능한 채널인지 검사 & 출력 
	void FixScrollBar( const int nSelected );
	void AutoAddingComboItems( );

protected:
	vector<HOUSING_ITEM>	m_cWndItems;

	CWndButton	m_wndButton[ GH_MAX_VIEW_CAPACITY ];	

	int			m_nSortType;
	BOOL		m_bIsGreater;
	int			m_nSelectedSort;

	CTexture  m_texUp;
	CTexture  m_texDown;
	
	int			m_nSelected;
	
	GH_SECTION	m_eSection;
	BOOL		m_bDeploying;

	BOOL		m_bOnFocusEffect;

	CWndGHouseShowOneUnit* m_pGHShowOne;
	DWORD m_dwComboCurrIK3;
};

#ifdef __GUILD_HOUSE_MIDDLE

struct GHBidData	//길드하우스 입찰 데이터
{
	GHBidData( ) { Init(); }
	void Init() { _id = 0, _name.Empty(), _cGuildList.clear(), _nBidMinPenya = 0; }

	OBJID _id;
	CString _name;

	vector< DWORD > _cGuildList;	//입찰한 길드목록
	int _nBidMinPenya;				//최소 입찰금액
};

typedef vector<GHBidData>				GHBidDataContainer;
typedef GHBidDataContainer::iterator	GHBidDataIter;

class CWndGuildHouseBid : public CWndNeuz			
{
	//길드하우스 입찰 
public:
	CWndGuildHouseBid( );
	virtual ~CWndGuildHouseBid( );

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual	void OnInitialUpdate(); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 

	void SetEnableWindow_Apply( BOOL bEnable, BOOL bWait = FALSE );
	void ResetInputMoneyWindows( );				//입력창 "0"으로 초기화
	void RequestCurrHouseInfo( );				//현재 보고있는 하우스 세부정보 요청

	void RefreshWnd_HouseList( );	//메인정보 갱신 및 그 외의 윈도우 초기화
	void RefreshWnd_HouseInfo( );	//메인정보 이외의 정보 갱신
	void UpdateData_HouseList( OBJID houseID, const char* szName );
	void UpdateData_HouseInfo( OBJID houseID, const int nMinPenya, __int64 nTnederPenya, vector< DWORD >& guildIDs );

	GHBidData* FindData( OBJID houseID );

	void MakeMoneyStyle( CString& str );

protected:
	GHBidDataContainer _cBidDatas;
	__int64 m_n64TenderPenya;	//내 길드의 입찰금 
	BOOL m_bMaster;

	BOOL m_bWaitResult;
	DWORD m_dwWaitTime;
};

#endif //__GUILD_HOUSE_MIDDLE


#endif //__GUILD_HOUSE

#endif
