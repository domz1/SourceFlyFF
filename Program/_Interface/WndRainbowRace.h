#if __VER >= 13 // __RAINBOW_RACE

#ifndef __WNDRAINBOWRACE__H
#define __WNDRAINBOWRACE__H

class CWndRainbowRaceOffer : public CWndNeuz 
{
public:
	CWndText* m_pText;
	int m_nOfferCount;

public: 
	CWndRainbowRaceOffer(); 
	virtual ~CWndRainbowRaceOffer();
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate();

	void SetOfferCount(int nCount) {m_nOfferCount = nCount;};
};

class CWndRainbowRaceInfo : public CWndNeuz 
{
public:
	CWndText* m_pText;

public: 
	CWndRainbowRaceInfo(); 
	virtual ~CWndRainbowRaceInfo();
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate(); 
};

class CWndRainbowRaceRule : public CWndNeuz 
{
public:
	CWndText* m_pText;

public: 
	CWndRainbowRaceRule(); 
	virtual ~CWndRainbowRaceRule();
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate(); 
};

class CWndRainbowRaceRanking : public CWndNeuz 
{
public:
	u_long m_dwPlayerId[5];
	
public: 
	CWndRainbowRaceRanking(); 
	virtual ~CWndRainbowRaceRanking();
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate();
	virtual void OnDraw(C2DRender* p2DRender);

	void SetRankingPlayer(u_long* dwPlayerId);
};

class CWndRainbowRacePrize : public CWndNeuz 
{
public:
	CWndText* m_pText;
	
public: 
	CWndRainbowRacePrize(); 
	virtual ~CWndRainbowRacePrize();
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate(); 
};

class CWndRainbowRaceMiniGameInfo : public CWndNeuz 
{
public:
	CWndText* m_pText;
	CString m_strFileName;

public: 
	CWndRainbowRaceMiniGameInfo(); 
	virtual ~CWndRainbowRaceMiniGameInfo();
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate(); 

	void SetFileName(CString strFileName) {m_strFileName = strFileName;};
};

class CWndRainbowRaceMiniGameButton : public CWndNeuz 
{
public:
	CTexturePack m_BtnTexture;
	BOOL m_bLoadTexMap;
	BOOL m_bFocus;
	BOOL m_bPush;
//	BOOL m_bMove;
	CPoint m_ptPush;
	int m_nAlpha;
	BOOL m_bReverse;

public: 
	CWndRainbowRaceMiniGameButton(); 
	virtual ~CWndRainbowRaceMiniGameButton();
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
//	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate();
	virtual void CWndRainbowRaceMiniGameButton::PaintFrame( C2DRender* p2DRender );
	virtual void OnDraw(C2DRender* p2DRender);
	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();
	virtual void OnLButtonDown( UINT nFlags, CPoint point );
	virtual void OnLButtonUp( UINT nFlags, CPoint point );
//	virtual void OnMouseWndSurface( CPoint point );
//	virtual void OnMouseMove(UINT nFlags, CPoint point);
	virtual BOOL Process();
};

class CWndRainbowRaceMiniGame : public CWndNeuz
{
public:
	CString m_strMiniGameInfo[7];
	CString m_strPathCheck[7];
	int m_TitleStaticID[7];
	int m_CheckStaticID[7];
	int m_nTextID[7];
	DWORD m_dwColor[7];
	BOOL m_bIsCompletedGame[7];
	int m_nCurrentGame;
	BOOL m_bGameInfoRefresh;

	BOOL  m_bReverse;
	float m_fLerp;

	CWndRainbowRaceMiniGameInfo* m_pWndMiniGameInfo;

public: 
	CWndRainbowRaceMiniGame(); 
	virtual ~CWndRainbowRaceMiniGame();
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual void OnLButtonUp( UINT nFlags, CPoint point );
	virtual BOOL OnSetCursor( CWndBase* pWndBase, UINT nHitTest, UINT message );
	virtual BOOL Process();

	void FillRect(C2DRender *p2DRender, CRect rectBg, DWORD dwColorstart, DWORD dwColorend, BOOL bLamp);

public:
	void SetCurrentGameInfo();
};

class CWndRainbowRaceMiniGameEnd : public CWndMessageBox
{ 
public: 
	int m_nGameID;
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD dwWndId = 0 );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual void OnDestroy();

public:
	void SetGameID(int nGameID) {m_nGameID = nGameID;};
};

class CWndRRMiniGameKawiBawiBo : public CWndNeuz 
{
	enum MyPosition {KAWI, BAWI, BO};
public:
	CWndText* m_pText;
	int m_nWinningCount;			//연승 수
	int m_nMyChoice;				//내가 선택한 카드 (가위, 바위, 보)
	int m_nComChoice;				//Com이 선택한 카드
	int m_nCount;
	int m_nDelay;					//Com 상태를 회전시키는 Delay값
	int m_nResult;					//서버로 부터 결과가 온 상태
	int m_nStatus;					//현재의 상태 (Delay관련 적용 위해 사용)
	int m_nItemCount;
	int m_nNextItemCount;
	
	DWORD m_dwRItemId;
	DWORD m_dwRNextItemId;

	CString m_strChoice[3];			//가위, 바위, 보 Text
	
	CWndStatic*	m_pStWinningCount;	//연승 표시
	CWndStatic*	m_pStMyChoice;		//내가 선택한 카드 표시
	CWndStatic*	m_pStComChoice;		//Com이 선택한 카드 표시
	
public: 
	CWndRRMiniGameKawiBawiBo(); 
	virtual ~CWndRRMiniGameKawiBawiBo();
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender );
	virtual	void OnInitialUpdate(); 
	virtual BOOL Process();

	void ReceiveResult(int nResult) {m_nResult = nResult;};
	void RefreshInfo();
	void DisableAllBtn();
	void EnableAllBtn();
	int GetKawiBawiBoComResult(int nMyChoice, int nResult);
}; 

class CWndRRMiniGameDice : public CWndNeuz
{
public:
	CWndText* m_pText;
	int m_nTargetNum;
	int m_nTargetNumBackup;
	int m_nDiceNum1, m_nDiceNum2;
	int m_nResultDiceNum1, m_nResultDiceNum2;	//서버로 부터 받은 결과 값
	CString m_strPathDice[6];
	int m_nResult;
	int m_nCount1, m_nCount2;
	int m_nDelay1, m_nDelay2;					//Com 상태를 회전시키는 Delay값
	int m_nStatus1, m_nStatus2;					//현재의 상태 (Delay관련 적용 위해 사용)
	BOOL m_bFinishStatus1;

	CTexturePack m_texTargetNum;
public:
	CWndRRMiniGameDice(); 
	virtual ~CWndRRMiniGameDice();
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate();
	virtual void OnDraw( C2DRender* p2DRender );
	virtual BOOL Process();
	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();

	void SetTargetNumber(int nNum) {m_nTargetNum = nNum;};
	void ReceiveResult(int nResult, int nDice1, int nDice2);
};

class CWndRRMiniGameArithmeticTimeOver : public CWndMessageBox
{ 
public:
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD dwWndId = 0 );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual void OnDestroy();
};

class CWndRRMiniGameArithmetic : public CWndNeuz 
{
public:
	CWndText* m_pText;
	CString m_strQuestion;
	DWORD m_dwTime;
	int m_nCorrectCount;

	CWndRRMiniGameArithmeticTimeOver* m_pWndRRMiniGameArithmeticTimeOver;

public: 
	CWndRRMiniGameArithmetic(); 
	virtual ~CWndRRMiniGameArithmetic();
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate();

	void SetNextQuestion(CString strQuestion, int nCorrectCount);
	virtual BOOL Process();
};

class CWndRRMiniGameStopWatch : public CWndNeuz 
{
public:
	CWndText* m_pText;
	DWORD m_dwTargetTime;
	DWORD m_dwStartTime;
	DWORD m_dwTime;
	BOOL m_bStart;
	time_t m_timeSec;
	int m_nUSec;
	CTexturePack m_texStopWatchNum;

public: 
	CWndRRMiniGameStopWatch(); 
	virtual ~CWndRRMiniGameStopWatch();
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual BOOL Process();
	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();

public:
	void SetTargetTime(DWORD dwTargetTime);
};

class  CWndRRMiniGameTyping : public CWndNeuz 
{
public:
	CWndText* m_pText;

	int m_QuestionStaticID[3];
	int m_TypingStaticID[3];

public: 
	CWndRRMiniGameTyping(); 
	virtual ~CWndRRMiniGameTyping();
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate();

public:
	void SetQuestion(int nQuestion, CString strQuestion);
	void ResetQuestion();
};

typedef struct __CARD_INFO
{
	CTexture* m_pTexture;
	int m_nCardNum;
	BOOL m_bCheck;
} __CARD_INFO;

class CWndRRMiniGameCard : public CWndNeuz 
{
public:
	CWndText* m_pText;
	CString m_strCard[9];
	CTexture* m_texCardBack;
	__CARD_INFO m_stCard[18];
	int m_nCustomID[18];
	int m_nPair[2];
	DWORD m_dwPenaltyTime;
	int m_nPenaltySec;
	CTexturePack m_texPenaltyNum;

public: 
	CWndRRMiniGameCard(); 
	virtual ~CWndRRMiniGameCard();
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual BOOL Process();
	virtual void OnLButtonUp( UINT nFlags, CPoint point );
	virtual BOOL OnSetCursor( CWndBase* pWndBase, UINT nHitTest, UINT message );
	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();

public:
	void SetQuestion(CString strQuestion);
	void ReceiveResult(int nResult);
};

class CWndRRMiniGameLadderFail : public CWndMessageBox
{ 
public: 
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD dwWndId = 0 );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual void OnDestroy();
};

class CWndRRMiniGameLadder : public CWndNeuz 
{
enum{GO_DOWN, GO_LEFT, GO_RIGHT};
public:
	int m_nResult;
	int m_nTarget;
	int m_nStart;
	int m_nLadder[15][10];
	CWndText* m_pText;
	int m_nStartCustomID[15];
	BOOL m_bStart;
	BOOL m_bMakeLadder;
	BOOL m_bFailMsg;
	CPoint m_ptPoint;
	int m_nGoTo;
	int m_nLevel;
	int m_nPoint;
	int m_nNextPosY;

	CTexture* m_pTexBg1;
	CTexture* m_pTexBg2;
	CTexture* m_pTexHorizon;
	CTexture* m_pTexChoice;
	CTexture* m_pTexPoint;
	CTexture* m_pTexGoal;
	CTexture* m_pTexFail;
	
	CRect m_rectLine[21];
	CRect m_rectPointLine;
	int m_nLineCount;
	BOOL  m_bReverse;
	float m_fLerp;

	CWndRRMiniGameLadderFail* m_pWndRRMiniGameLadderFail;

public: 
	CWndRRMiniGameLadder(); 
	virtual ~CWndRRMiniGameLadder();
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual BOOL Process();
	virtual void OnLButtonUp( UINT nFlags, CPoint point );
	virtual BOOL OnSetCursor( CWndBase* pWndBase, UINT nHitTest, UINT message );

public:
	BOOL MakeLadder();
	void ReceiveResult(int nResult);
	void SetFillRectValue();
	void FillRect(C2DRender *p2DRender, CRect rectBg, DWORD dwColorstart, DWORD dwColorend);
	void Reset();
};
#endif //__WNDRAINBOWRACE__H
#endif //__RAINBOW_RACE