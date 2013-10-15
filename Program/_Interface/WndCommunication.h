#ifndef __COMMUNICATION_H
#define __COMMUNICATION_H

#ifdef __ITEM_LINK
#define COLOR_ITEMLINK (DWORD)0xff002fff
#endif //__ITEM_LINK

class CWndCommand;
class CWndChatFilter;
//////////////////////////////////////////////////////////////////////////////////////
// 채팅 윈도 
//
class CWndEditChat : public CWndEdit
{
public:
	CWndEditChat();
	~CWndEditChat();
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnSetFocus(CWndBase* pOldWnd);
	virtual void OnKillFocus(CWndBase* pNewWnd);
};
class CWndMacroChat : public CWndButton
{
public:
	CTexture* m_pTexMacro;
	CTexture m_texMacroChat;
	CWndMacroChat();
	~CWndMacroChat();
	virtual void OnDraw( C2DRender* p2DRender );
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnMouseMove(UINT nFlags, CPoint point);
	virtual	void OnInitialUpdate();
};
class CWndTextChat : public CWndText
{
public:
	CWndTextChat();
	~CWndTextChat();
	virtual BOOL IsPickupSpace(CPoint point); 
};

#if __VER >= 8 //__Y_CHAT_SYSTEM_8
class CWndChatLog : public CWndNeuz
{
	CWndMenu	 m_wndMenuPlace;
	CWndTextChat m_wndText;

public:
	CWndChatLog();   
	virtual ~CWndChatLog(); 
	void  PutString( LPCTSTR lpszString, DWORD dwColor = 0xffffffff, DWORD dwPStyle = 0x00000001 ); //CObj* pObj );
#ifdef __ITEM_LINK
	void  SetLinkedItem( CItemElem* pLinkedItem );
#endif //__ITEM_LINK
//	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL,DWORD dwWndId = 0);
	// message
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL );
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
//	virtual BOOL OnEraseBkgnd(C2DRender* p2DRender);
	virtual void OnDestroy();
	virtual void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void OnDestroyChildWnd( CWndBase* pWndChild );
	virtual void OnSetFocus(CWndBase* pOldWnd);
	virtual void OnKillFocus(CWndBase* pNewWnd);
	virtual void AdditionalSkinTexture( LPWORD pDest, CSize size, D3DFORMAT d3dFormat );
	virtual void OnRButtonUp(UINT nFlags, CPoint point);
	virtual void OnRButtonDown(UINT nFlags, CPoint point);
	virtual void OnMouseMove(UINT nFlags, CPoint point);
	virtual void SetWndRect( CRect rectWnd, BOOL bOnSize = TRUE);
	virtual BOOL Process ();	

#ifdef __ITEM_LINK
private:
	CItemElem* m_pLinkedItem;
#endif //__ITEM_LINK
};
#endif //__Y_CHAT_SYSTEM_8

class CWndChat : public CWndNeuz
{
	IMAGE m_wndChatEdit[3];
	//CSize m_sizeWndChatEdit[3];

	CWndMacroChat m_wndMacroChat;
	CTimer m_timerDobe;
	CTexture m_texEdit;
	CTimer m_timerInsMsg;
#ifdef __CSC_GAME_GRADE
	CTimer m_timerAlertGGrade;
#endif //__CSC_GAME_GRADE
	int m_nInsMsgCnt;
	int				m_nHistoryIndex;
	vector<CString> m_strHistory;
	CTimer m_timerInputTimeOut;

public:
	BOOL m_bChatLock;
#if __VER >= 8 //__CSC_VER8_1
	BOOL m_bChatLog;
#endif //CSC_VER8_1
	BOOL m_bMoveLock;
	static int m_nChatChannel;
	CWndChatFilter* m_pWndChatFilter;
	CString m_strCharName;
	CWndTextChat m_wndText;
	CStringArray m_strArray;
	CWndCommand* m_pWndCommand;
//	CWndButton m_wndMenu;
	//CWndText m_wndText;
	//CWndEdit m_wndBeginning;
	CWndEditChat m_wndEdit;
	//CWndText m_wndReceiver;

//	CWndButton m_wndShout;
	//CWndButton m_wndParty;

	CWndChat();   
	virtual ~CWndChat(); 
	void SerializeRegInfo( CAr& ar, DWORD& dwVersion );
	void  Parsing( CString string );
	void  PutString( LPCTSTR lpszString, DWORD dwColor = 0xffffffff, DWORD dwPStyle = 0x00000001 ); //CObj* pObj );
//	virtual CItem* GetFocusItem() { return NULL; }
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL,DWORD dwWndId = 0);
	// message
	void SetChannel();
	virtual	void SetWndRect( CRect rectWnd, BOOL bOnSize = TRUE);
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL );
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnEraseBkgnd(C2DRender* p2DRender);
	virtual void OnDestroy();
	virtual void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void OnDestroyChildWnd( CWndBase* pWndChild );
	virtual void OnSetFocus(CWndBase* pOldWnd);
	virtual void OnKillFocus(CWndBase* pNewWnd);
	virtual void AdditionalSkinTexture( LPWORD pDest, CSize size, D3DFORMAT d3dFormat );
	virtual void OnRButtonUp(UINT nFlags, CPoint point);
	virtual void OnRButtonDown(UINT nFlags, CPoint point);
	virtual void OnMouseMove(UINT nFlags, CPoint point);
	virtual BOOL Process ();

};

#endif
