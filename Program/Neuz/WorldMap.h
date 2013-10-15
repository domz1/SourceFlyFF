// WorldMap.h: interface for the CWorldMap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WORLDMAP_H__D1974F1A_A118_43E5_91D0_588850AE6D13__INCLUDED_)
#define AFX_WORLDMAP_H__D1974F1A_A118_43E5_91D0_588850AE6D13__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if __VER >= 13 // __CSC_VER13_1
typedef struct __MONSTER_INFO
{
	int		m_nMonCnt;
	DWORD	m_dwMonsterId[5];
	DWORD	m_dwDropItemId;
	CRect	m_rectPos;
} __MONSTER_INFO;

class CMonsterInfoPack : public CTexturePack
{
public:
	int m_nMap;
	vector<__MONSTER_INFO>	m_vecMonsterInfo;
public:
	CMonsterInfoPack();
	~CMonsterInfoPack();
	BOOL LoadScript( LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR pFileName, int nMap );
};
#endif //__CSC_VER13_1

#if __VER >= 13 // __RAINBOW_RACE
typedef struct __RAINBOW_NPC
{
	int		m_nMap;
	CRect	m_rectTotalMapPos;
	CRect	m_rectPos;
} __RAINBOW_NPC;

class CRainbowNPCPack : public CTexturePack
{
public:
	vector<__RAINBOW_NPC>	m_vecRainbowNPC;
public:
	BOOL LoadScript( LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR pFileName );
};
#endif //__RAINBOW_RACE

class CWorldMap
{
#if __VER >= 13
#if __VER >= 14 // __NEW_CONTINENT

#if __VER >= 15 // __NEW_CONTINENT15
	enum {MAX_BUTTON = 9};
#else
	enum {MAX_BUTTON = 8};
#endif //__NEW_CONTINENT15

#else //__NEW_CONTINENT
	enum {MAX_BUTTON = 7};
#endif //__NEW_CONTINENT
#endif //__VER >= 13
	CString	   m_strViewMapString;
	CBillboard m_billArrow[2];
	CTexture   m_texArrow[2];
	BILLBOARD  m_billboard[2];
	
	BOOL		m_bRender;
	CTexture*	m_pTexWorldMap;

#if __VER >= 14 // __NEW_CONTINENT
	CString		m_strMapStringList[MAX_BUTTON - 1];
#else //__NEW_CONTINENT
	CString		m_strMapStringList[6];
#endif //__NEW_CONTINENT

#if __VER < 14 // __NEW_CONTINENT
	multimap< CString, vector<CPoint> >   m_mapView;
#endif // __NEW_CONTINENT
	int			m_nMap;
	
	CPoint			m_cPos;
	CTexturePack	m_texMapButton;
#if __VER >= 13 // __CSC_VER13_1
#if __VER >= 14 // __NEW_CONTINENT
	CString				m_strMonScript[MAX_BUTTON - 2];
#else //__NEW_CONTINENT
	CString				m_strMonScript[5];
#endif //__NEW_CONTINENT
	CMonsterInfoPack	m_MonsterInfo;
	int					m_nSelMon;
	BOOL				m_bShowMonsterInfo;
	CRect				m_cRect[MAX_BUTTON];
	int					m_nDrawMenu[MAX_BUTTON];
#else //__CSC_VER13_1
	CRect			m_cRect[6];
	int				m_nDrawMenu[6];
#endif //__CSC_VER13_1

#if __VER >= 13 // __RAINBOW_RACE
	CRainbowNPCPack		m_RainbowNPC;
#endif //__RAINBOW_RACE
	FLOAT			m_fRate;
	CPoint			m_cpScreen;
	CPoint			m_cOffset;

public:
	CWorldMap();
	virtual ~CWorldMap();

	static	CWorldMap* GetInstance( void );

#if __VER < 14 // __NEW_CONTINENT
	int GetMapNo( D3DXVECTOR3 vPos );
#endif // __NEW_CONTINENT

#if __VER >= 15 // __TELEPORTER	
	struct WMapIcon		//Wnd Map Icon
	{
		int _index;
		CRect _rect;
	};
	OBJID	m_idTeleporter;
	BOOL	m_bTeleportMode;
	vector< WMapIcon > m_cTelPosRects;
	vector< D3DXVECTOR3 > m_cWorldPos;
	CTexture* m_pTelPosTexture;

	void ProcessingTeleporter( CMover* pFocusMover );			// 월드맵을 열고, 좌표에 대한 버튼 생성/업데이트
	void RenderTelPos( C2DRender *p2DRender );
	void SetTelMode( BOOL bMode );
	BOOL WorldPosToMapPos( const D3DXVECTOR3& vPos, OUT CPoint& cPos );
	void UpdateTeleportWorld( );
#endif
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
private:
	CRect m_rectDestination;
	BOOL m_bDestinationMode;
	CTexture* m_pDestinationPositionTexture;

	enum { MINIMUM_ALPHA = 0, MAXIMUM_ALPHA = 255 };
	int m_nDestinationTextureAlpha;
	DWORD m_tmOld;
	BOOL m_bAlphaSwitch;

public:
	void SetDestinationMode( BOOL bDestinationMode );
	void UpdateDestinationPosition( const D3DXVECTOR3& rPosition );
	void RenderDestinationPosition( C2DRender* p2DRender );
#endif // __IMPROVE_QUEST_INTERFACE

#ifdef __IMPROVE15_WORLDMAP
	CWndListBox* m_pWndList;
	int m_nSelected;
#endif
	
	void Init();
	void Process();
	void RenderWorldMap( C2DRender *p2DRender );
	void RenderPlayer( C2DRender *p2DRender, BOOL bMyPlayer, D3DXVECTOR3 vPos, const TCHAR* szName );
	BOOL LoadWorldMap();
	void DeleteWorldMap();

	void DeleteDeviceObjects();
	void RestoreDeviceObjects(); 
	void InvalidateDeviceObjects();
	BOOL IsRender() { return m_bRender; }
	void OnLButtonDown();

#if __VER >= 13 // __CSC_VER13_1
	CPoint GetCpScreen() {return m_cpScreen;}
	CPoint GetCpOffset() {return m_cOffset;}
#endif //__CSC_VER13_1
};

#endif // !defined(AFX_WORLDMAP_H__D1974F1A_A118_43E5_91D0_588850AE6D13__INCLUDED_)
