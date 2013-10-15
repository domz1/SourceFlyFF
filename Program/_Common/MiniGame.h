// MiniGame.h: interface for the CMiniGame class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MINIGAME_H__79AC4FCB_D41A_4407_ACF3_0FDF796F2ED1__INCLUDED_)
#define AFX_MINIGAME_H__79AC4FCB_D41A_4407_ACF3_0FDF796F2ED1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef __EVE_MINIGAME

class CUser;
class CMiniGame  
{
public:
	enum { KAWIBAWIBO_WIN, KAWIBAWIBO_LOST, KAWIBAWIBO_DRAW, KAWIBAWIBO_FAILED, KAWIBAWIBO_GETITEM	};
	enum { KAWI, BAWI, BO };
	enum { ALPHABET_NOTENOUGH_MONEY = 998, ALPHABET_FAILED = 999, };
	enum { FIVESYSTEM_BETOK = 996, FIVESYSTEM_FAILED = 997, FIVESYSTEM_OVERMAX = 998, FIVESYSTEM_NOTENOUGH = 999 };

	struct __ITEMGAME
	{
		DWORD	dwItemId;	// 아이디
		int		nItemCount;	// 갯수
		DWORD	dwPer;		// 퍼센트
		__ITEMGAME()
		{
			dwItemId = 0;
			nItemCount = 0;
			dwPer = 0;
		};
	};
	
	struct __ALPHABET
	{
		int		nPublic;	// 공개 글자 위치
		CString strWord;	// 제시 단어
		CString	strQuestion;	// 문제
		vector<__ITEMGAME>	vecItem;	// 지급 아이템
		__ALPHABET():nPublic(1),strWord(_T("")), strQuestion(_T("")) {}
	};
	
	struct __KAWIBAWIBORESULT
	{
		int		nMy;
		int		nYou;
		int		nResult;
	};

	struct __KAWIBAWIBO
	{
		DWORD	dwDraw;		// 비김 확률
		DWORD	dwLost;		// 패배 확률
		DWORD	dwWin;		// 승리 확률
		DWORD	dwItemId;	// 아이디
		int		nItemCount;	// 아이템 갯수
		__KAWIBAWIBO()
		{
			dwDraw = 0;		
			dwLost = 0;	
			dwWin = 0;	
			dwItemId = 0;
			nItemCount = 0;
		};
	};

	struct __FIVESYSTEM		// (오곱) 입찰금 상위 순위별 확률
	{
		DWORD	dwMin;		// Min 확률
		DWORD	dwMax;		// Max 확률
		__FIVESYSTEM()
		{
			dwMin = 0;
			dwMax = 0;
		};
	};



public:
	CMiniGame();
	virtual ~CMiniGame();

	BOOL Load_Alphabet();	// 알파벳 단어 맞추기
	BOOL Load_KawiBawiBo();	// 가위, 바위, 보
	BOOL Load_FiveSystem();	// 오곱
	BOOL Load_Reassemble(); // 퍼즐 
	char GetCharacter( DWORD ItemId ); // 아이템 ID 맞는 알파벳 한 글자 리턴
	int	GetKawibawiboYou( int nMy, int nResult );
#ifdef __WORLDSERVER
	int Result_Kawibawibo( CUser* pUser );
	BOOL ResultItem_Kawibawibo( CUser* pUser );
	__KAWIBAWIBO FindKawibawibo( int nWinCount );
	
	BOOL Result_Reassemble( CUser* pUser, OBJID* objItemId, int nCount );
	BOOL ResultItem_Reassemble( CUser* pUser );
	__ITEMGAME GetReassembleRandomItem();
	void OpenWnd_Reassemble( CUser* pUser );
	
	void OpenWnd_Alphabet( CUser* pUser );
	int Result_Alphabet( CUser* pUser, OBJID* objItemId, int nCount, int nQuestionID );
	BOOL ResultItem_Alphabet( CUser* pUser, int nNum );
	__ITEMGAME GetAlphabetRandomItem( int nNum );
	char GetPublicCharacter( int nQuestionNum, int nPublicAlphabet );
		
	void OpenWnd_FiveSystem( CUser* pUser );
	void DestroyWnd_FiveSystem( CUser* pUser );
	int Bet_FiveSystem( CUser* pUser, int nBetNum, int nBetPenya );
	int Result_FiveSystem( CUser* pUser );
	BOOL ResultPenya_FiveSystem( CUser* pUser, int nResult );
#endif // __WORLDSERVER
	
	int m_nBetMinPenya;				// 오곱 - 최소 입찰 금액
	int m_nBetMaxPenya;				// 오곱 - 최대 입찰 금액
	int m_nMultiple;				// 오곱 - 상금 지급 배수
	
	int m_nPayPenya;				// 알파벳 지출 페냐
	
	int nKawiBawiBo_MaxWin;			// 가위바위보 최대 연승 인덱스

	vector<__ALPHABET>		m_vecAlphabet;	
	map<int, __KAWIBAWIBO>  m_mapKawiBawiBo;
	map<int, __FIVESYSTEM>	m_mapFiveSystem;
	vector<__ITEMGAME>		m_vecReassembleItem;
	vector<DWORD>	m_vecReassembleCard;
	vector<__KAWIBAWIBORESULT> m_vecKawibawiboResult;
};

#endif // __EVE_MINIGAME
#endif // !defined(AFX_MINIGAME_H__79AC4FCB_D41A_4407_ACF3_0FDF796F2ED1__INCLUDED_)
