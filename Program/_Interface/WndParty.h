#ifndef __WNDPARTY__H
#define __WNDPARTY__H

#include "WndPartyChangeTroup.h"

#if __VER >= 8 //__CSC_VER8_2
#include "WndPartyQuick.h"
#endif //__CSC_VER8_2

class CWndPartyInfo : public CWndNeuz 
{ 
public: 
	int m_nSelected; // 현재 파티창에서 선택된 놈 인덱스 (위에서부터 zero base)
	CWndPartyInfo(); 
	~CWndPartyInfo(); 

	CTexture m_texGauEmptyNormal ;
	CTexture m_texGauFillNormal  ;
	LPDIRECT3DVERTEXBUFFER9 m_pVBGauge;
	
	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 
class CWndPartySkill : public CWndNeuz 
{ 
public: 
	CTexture* m_atexSkill[ 10 ];
	int m_nSkillSelect;
	
	CWndPartySkill(); 
	~CWndPartySkill(); 

	virtual void OnMouseWndSurface( CPoint point );
	virtual void OnMouseMove(UINT nFlags, CPoint point);
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point);
}; 
class CWndParty : public CWndNeuz 
{ 
public:
	CWndButton* m_pWndLeave;
	CWndButton* m_pWndChange;
	CWndButton* m_pWndTransfer;
#if __VER >= 8 //__CSC_VER8_2
	CWndButton* m_pBtnPartyQuick;
	CWndPartyQuick* m_pWndPartyQuick;
#else
	CWndButton* m_pBtnPartyQuick;
#endif //__CSC_VER8_2
	
public: 
	CWndPartyInfo  m_wndPartyInfo;
	CWndPartySkill m_wndPartySkill;
	CWndPartyChangeTroup* m_WndPartyChangeTroup;
	
	CWndParty(); 
	~CWndParty(); 

	virtual void SerializeRegInfo( CAr& ar, DWORD& dwVersion );
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 

#endif
