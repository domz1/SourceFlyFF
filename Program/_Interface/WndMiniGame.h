#ifdef __EVE_MINIGAME

#ifndef __WNDMINIGAME__H
#define __WNDMINIGAME__H

class CWndKawiBawiBoGameWin;

class CWndKawiBawiBoGame : public CWndNeuz 
{
	enum MyPosition {KAWI, BAWI, BO};
public:
	int m_nWinningCount;			//연승 수
	int m_nWinningMaxCount;			//연승 최대치
	int m_nMyChoice;				//내가 선택한 카드 (가위, 바위, 보)
	int m_nComChoice;				//Com이 선택한 카드
	int m_nCount;
	int m_nDelay;					//Com 상태를 회전시키는 Delay값
	int m_nResult;					//서버로 부터 결과가 온 상태
	int m_nPrevResult;
	int m_nStatus;					//현재의 상태 (Delay관련 적용 위해 사용)
	int m_nItemCount;
	int m_nNextItemCount;
	BOOL m_bCheckCoupon;			//가위바위보 쿠폰을 필요로하는지 확인
	
	DWORD m_dwRItemId;
	DWORD m_dwRNextItemId;

	CString m_strChoice[3];			//가위, 바위, 보 Text
	
	CWndStatic*	m_pStWinningCount;	//연승 표시
	CWndStatic*	m_pStMyChoice;		//내가 선택한 카드 표시
	CWndStatic*	m_pStComChoice;		//Com이 선택한 카드 표시
	
	CWndKawiBawiBoGameWin* m_pWndGameWin;
public: 
	CWndKawiBawiBoGame(); 
	virtual ~CWndKawiBawiBoGame();
	
	virtual void OnDestroy();
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual BOOL Process ();

	void ReceiveResult(int nResult, int nItemCount, int nNextItemCount, DWORD dwItemId, DWORD dwNextItemId);
	void CreateWinningWnd();
	void RefreshInfo();
	void DisableAllBtn();
	void EnableAllBtn();
}; 

class CWndKawiBawiBoGameWin : public CWndNeuz 
{
public:
	CWndKawiBawiBoGame* m_pWndGame;

	int m_nItemCount;
	int m_nNextItemCount;
	DWORD m_dwItemID;
	DWORD m_dwNextItemID;
	
public: 
	CWndKawiBawiBoGameWin(); 
	virtual ~CWndKawiBawiBoGameWin(); 
	
	virtual void OnDestroy();
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point );
	virtual void OnMouseWndSurface( CPoint point );

	void SetInfo(int nItemCount, int nNextItemCount, DWORD dwItemId, DWORD dwNextItemId, int nWinningCount, int nWinningMaxCount);
}; 

class CWndKawiBawiBoGameConfirm : public CWndNeuz 
{	
public:
	CWndButton m_wndButton1;
	CWndButton m_wndButton2;
protected:
	CString m_strText;
	CWndText m_wndText;
	
public:
	CWndKawiBawiBoGameConfirm();
	~CWndKawiBawiBoGameConfirm();
	
	virtual void OnDestroy();
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD dwWndId = 0 );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 

class CWndFindWordGame : public CWndNeuz 
{
public:
	int m_nPublic;				//힌트로 주어지는 글자의 위치
	OBJID m_firstwordID;		//힌트로 주어지는 첫번째 글자
	OBJID m_itemID[5];			//각 글자의 Object ID
	CItemElem* m_pItemElem[5];	//아이템 정보 저장.
	int m_pStaticID[5];			//5개의 WndID -> Picture (ondropicon 사용을 위해)
	int m_nSelectCtrl;			//선택된 Wnd 기억.
	CWndText* m_pText;
	BOOL m_bGetFirst;			//힌트로 주어지는 글자를 받았다.
	BOOL m_bStart;
	int m_nQuestionID;			// 문제 번호
	
	CWndInventory* m_pWndInventory;
public: 
	CWndFindWordGame();
	virtual ~CWndFindWordGame(); 
	
	virtual void OnDestroy();
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point );
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point );
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point );
	virtual BOOL Process ();

	int HitTest( CPoint point );
	void SetFirstWord(int nPublic, char nChar);
	void SetWord(CItemElem* pItemElem);
	void SetDescription(CHAR* szChar);
	void ReceiveResult(int nResult, DWORD dwItemId, int nItemCount);
}; 

class CWndDiceGameTender;
class CWndDiceGameMessage;

class CWndDiceGame : public CWndNeuz 
{
	enum DiceNum {DICE_ONE, DICE_TWO, DICE_THREE, DICE_FOUR, DICE_FIVE, DICE_SIX};
public:
	int m_nPenya[6];		//각 번호에 입찰한 금액
	int m_nSelectCtrl;		//선택한 번호
	int m_nDiceChoiceNum;	//선택된 주사위의 결과값
	int m_nDiceNum;			//주사위 번호
	int m_pBtnID[6];		//Penya 올라갈 Ctrl의 ID
	int m_nCount;
	int m_nDelay;			//Com 상태를 회전시키는 Delay값
	int m_nStatus;			//현재의 상태 (Delay관련 적용 위해 사용)
	int m_nMaxPenya;		//최대 입찰금액
	int m_nMinPenya;		//최소 입찰금액
	int m_nMultiple;		//상금지급 배수
	int m_nPrizePenya;		//상금지급 액수
	int m_pStaticID[6];
	int m_pStaticRID[6];
	int m_pStaticNum[6];
	int m_nAlpha;

	BOOL m_bSendStart;		//서버로 결과를 요청 함
	BOOL m_bReciveResult;	//서버로 부터 결과를 받았는지 확인
	BOOL m_bEnd;			//주사위 게임이 끝난 시점
	BOOL m_bFlsh;

	CString m_strPathDice[6];
	CString m_strPathDiceNum[6];
	
	CWndText* m_pCost[6];	//Penya 올릴 Ctrl
	CWndText* m_pRCost[6];	//Penya 결과 Ctrl
	CWndText* m_pText;
	
	CWndDiceGameTender* m_pWndDiceTender;
	CWndDiceGameMessage* m_pWndMessageBox;
	
public: 
	CWndDiceGame(); 
	virtual ~CWndDiceGame(); 
	
	virtual void OnDestroy();
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual BOOL Process ();
	
	void RefreshInfo();
	void SetMinMaxPenya(int nMinPenya, int nMaxPenya, int nMultiple);	//최소/최대 입찰금액, 상품지급 배수를 적용
	void CheckPenyaUsuable(int nPenya, int nSelect);	//입찰되는 금액이 가능한 금액인지를 확인
	void SetTenderPenya(int nSelect, int nPenya);		//가능한 금액이라면 적용한다.
	void ReceiveResult(int nDiceNum, int nPenya);		//서버로 부터 주사위의 결과 값을 받음
	void SetDescription(CHAR* szChar);
	void RefreshCtrl();
	void ResetPenya();
}; 

class CWndDiceGameTender : public CWndNeuz 
{
public:
	CWndDiceGame* m_pDiceGame;
	int m_nTenderPenya;
	int m_nSelect;
	int m_nMaxPenya;
	int m_nMinPenya;
	
public: 
	CWndDiceGameTender(); 
	virtual ~CWndDiceGameTender(); 
	
	virtual void OnDestroy();
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point );

	void SetMinMaxPenya(int nMinPenya, int nMaxPenya);
	void SetSelectNum(int nSelect);
};

class CWndDiceGameMessage : public CWndNeuz
{
public:
	CWndButton m_wndButton;
protected:
	CString m_strText;
	CWndText m_wndText;
	
public:
	CWndDiceGameMessage();
	~CWndDiceGameMessage();
	
	virtual void OnDestroy();
	virtual BOOL Initialize( LPCTSTR lpszMessage, CWndBase* pWndParent, DWORD nType = MB_OK );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
};

class CWndPuzzleGame : public CWndNeuz 
{
public:
	DWORD m_itemID[9];			//각 글자의 Object ID
	OBJID m_objItemID[9];
	CItemElem* m_pItemElem[9];	//아이템 정보 저장.
	int m_pStaticID[9];			//5개의 WndID -> Picture (ondropicon 사용을 위해)
	int m_nSelectCtrl;			//선택된 Wnd 기억.
	CWndText* m_pText;
	BOOL m_bStart;

	CWndInventory* m_pWndInventory;
public: 
	CWndPuzzleGame(); 
	virtual ~CWndPuzzleGame(); 
	
	virtual void OnDestroy();
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point );
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point );
	virtual BOOL Process ();
	
	int HitTest( CPoint point );
	void SetPuzzle(vector<DWORD> vPuzzle);
	void SetPicture(CItemElem* pItemElem);
	void ReceiveResult();
	void SetDescription(CHAR* szChar);
}; 

#endif //__WNDMINIGAME__H
#endif //__EVE_MINIGAME