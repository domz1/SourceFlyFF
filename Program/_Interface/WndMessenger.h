#ifndef __WNDMESSENGER__H
#define __WNDMESSENGER__H

#include "WndFriendCtrl.h"
#include "WndPartyCtrl.h"
#include "WndGuildCtrl.h"
#if __VER >= 11 // __CSC_VER11_4
#include "WndMessengerCtrl.h"
#endif //__CSC_VER11_4

class CWndMessenger : public CWndNeuz 
{ 
public: 
	CWndMessenger(); 
	~CWndMessenger(); 

	CWndMenu		m_menuState;
	CWndFriendCtrl	m_wndFriend;
	CWndPartyCtrl	m_wndParty;
	CWndGuildCtrl	m_wndGuild;
	CString			m_strToolTip;	// 접속표시 툴팁문자열 
	CTexture		m_TexMail;
	int				m_nFlashCounter;
	int				m_nSwitch;

	virtual	BOOL Process();	
	virtual void SetWndRect( CRect rectWnd, BOOL bOnSize );
	virtual void SerializeRegInfo( CAr& ar, DWORD& dwVersion );
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual void OnMouseWndSurface( CPoint point );
};

#if __VER >= 11 // __CSC_VER11_4
class CWndMessengerEx : public CWndNeuz 
{ 
public: 
	CWndMessengerEx(); 
	~CWndMessengerEx(); 

	CWndMenu			m_menuState;
	CWndFriendCtrlEx	m_wndFriend;
	CWndGuildCtrlEx		m_wndGuild;
#if __VER >= 15 // __CAMPUS
	CWndCampus			m_WndCampus;
#endif // __CAMPUS
	CTexture			m_TexMail;
	int					m_nFlashCounter;
	int					m_nSwitch;

	virtual	BOOL Process();
//	virtual void SetWndRect( CRect rectWnd, BOOL bOnSize );
	virtual void SerializeRegInfo( CAr& ar, DWORD& dwVersion );
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase );
//	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point );

	void UpdateFriendList();
	void UpdateGuildMemberList();
#if __VER >= 15 // __CAMPUS
	void UpdateCampusMemberList();
#endif // __CAMPUS
};
#endif //__CSC_VER11_4

class CWndInstantMsg : public CWndNeuz 
{ 
public: 
	CTimer m_timer;
	CString m_strMessage;
	CWndInstantMsg(); 
	~CWndInstantMsg(); 
	
	CString m_strPlayer;
	
	void AddMessageJoin( LPCTSTR lpszJoinName );
	void AddMessage( LPCTSTR lpszFrom, LPCTSTR lpszMessage );
	void AddPostMessage( LPCTSTR lpszSendName );
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 
class CWndMessage : public CWndNeuz 
{ 
public: 
	CWndMessage(); 
	~CWndMessage(); 
	
	CString m_strPlayer;
	
	void InitSize( void );
	void AddMessage( LPCTSTR lpszFrom, LPCTSTR lpszMessage );
	void OnInputString();

	virtual void SetWndRect( CRect rectWnd, BOOL bOnSize );
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 

class CWndMessageNote : public CWndNeuz 
{ 
public:
	TCHAR m_szName[ 64 ];
	DWORD m_dwUserId;

	CWndText* m_pWndText;
	CWndEdit* m_pEdit;
public: 
	CWndMessageNote(); 
	~CWndMessageNote(); 
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 

class CWndMessengerNote : public CWndNeuz 
{ 
public: 
	CWndMessengerNote(); 
	~CWndMessengerNote(); 
	
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
