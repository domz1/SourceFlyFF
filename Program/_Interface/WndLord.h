#ifndef __WNDLORD__H
#define __WNDLORD__H
#if __VER >= 12 // __LORD
#include "clord.h"


class CWndLordConfirm : public CWndNeuz 
{ 
private:
	CString		m_pPledge;
public: 
	CWndLordConfirm(CString strStr) {m_pPledge = strStr;}; 
	~CWndLordConfirm() {}; 

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 

class CWndLordPledge : public CWndNeuz 
{ 
private:
	SPC					m_pRanker;
	bool				m_bIsFirst;
	CWndLordConfirm*	m_pWndConfirm;

public: 
	CWndLordPledge(); 
	CWndLordPledge(int nRank); 
	~CWndLordPledge(); 
	
	void SetPledge(LPCTSTR strPledge);
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	BOOL SetRanker(int nRank);
}; 

class CWndLordState : public CWndNeuz 
{ 
private:
	DWORD				m_tmRefresh;
	CString				strName[10];
	CString				strClass[10];
	int					m_nSelect;
public: 
	CWndLordPledge*		m_pWndPledge;

	CWndLordState(); 
	~CWndLordState(); 

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual BOOL Process();
}; 



class CWndLordTender : public CWndNeuz 
{ 
public: 
	CWndLordTender(); 
	~CWndLordTender(); 

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	void	RefreshDeposit();
}; 

class CWndLordVote : public CWndNeuz 
{ 
private:
	DWORD				m_tmRefresh;

public: 
	CWndLordVote(); 
	~CWndLordVote(); 

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual BOOL Process();
}; 

class CWndLordEvent : public CWndNeuz 
{ 
private:
	int m_nEEvent;	// 사용자가 선택한 경험치 이벤트 인덱스, 초기값 0
	int m_nDEvent;	// 사용자가 선택한 드롭률 이벤트 인덱스, 초기값 0
public: 
	CWndLordEvent(); 
	~CWndLordEvent(); 

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 

class CWndLordSkill : public CWndNeuz 
{ 
private:
	BOOL          m_bDrag;
	CTexture*     m_aTexSkill[9];
	int			  m_nCurSelect;
	LPWNDCTRL	  m_aWndCtrl[9];
public: 
	CWndLordSkill(); 
	~CWndLordSkill(); 

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual void OnMouseMove(UINT nFlags, CPoint point);
	virtual void OnMouseWndSurface( CPoint point );
}; 

#define LORDSKILL_RECALL	6
#define LORDSKILL_TELEPORT	7

class CWndLordSkillConfirm : public CWndNeuz 
{ 
private:
	int m_nType;

public: 
	CWndLordSkillConfirm() {m_nType = 0;}; 
	~CWndLordSkillConfirm() {}; 
	CWndLordSkillConfirm(int nType) {m_nType = nType;};

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
};

class CWndLordInfo : public CWndNeuz 
{ 

public: 
	CWndLordInfo()  {}; 
	~CWndLordInfo() {}; 

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
};

class CWndLordRPInfo : public CWndNeuz 
{ 

public: 
	CWndLordRPInfo()  {}; 
	~CWndLordRPInfo() {}; 

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
};

#endif
#endif
