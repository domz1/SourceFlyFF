#ifndef __HOTKEY_H
#define __HOTKEY_H

#define HOTKEY_MAX 2000

// 기능 정의 

#define FUNC_USE1        1
#define FUNC_USE2        2
#define FUNC_USE3        3
#define FUNC_USE4        4
#define FUNC_USE5        5
#define FUNC_USE6        6
#define FUNC_USE7        7
#define FUNC_GUILD       8
#define FUNC_COMBAT      9
#define FUNC_EQUIPMENT   10
#define FUNC_KNOWLEDGE   11
#define FUNC_KNOWSUB     12
#define FUNC_STATUS      13
#define FUNC_INVENTORY   14
#define FUNC_ATTACKPC    15
#define FUNC_SOUND       16
#define FUNC_MAP         17
#define FUNC_FILTER      18
#define FUNC_WISPER      19 
#define FUNC_CHATLOG     20 
#define FUNC_VIEWOBJECT  21
#define FUNC_CLEARMSG	  22 
#define FUNC_LOOKHIDDEN  23 

#define FUNC_STOP        50
#define FUNC_CHAT        51 
#define FUNC_KNOWLEFT    52
#define FUNC_KNOWRIGHT   53
#define FUNC_KNOWTOP     54
#define FUNC_OLDKNOW     55
#define FUNC_EXIT        56
#define FUNC_SCREENCAP   57
#define FUNC_SLOT1       58
#define FUNC_SLOT2       59
#define FUNC_SLOT3       60
#define FUNC_HELP        61
#define FUNC_VIEWPARTY   62
 
typedef struct
{
	BOOL m_bPressed;
	BOOL m_bShift  ;
	BOOL m_bControl;
	int  m_nHotkey ;
} HOTKEY,* LPHOTKEY;

typedef CQueue<HOTKEY>  CQueueHotkey; 

class CHotkey
{
	CQueueHotkey CQueueHotkey; 
	HOTKEY m_HotkeyTable[100];

public:

	CHotkey();
	~CHotkey();

	void SetDefaultHotkey(int nMax);
	void ExecuteFunc( int nFunc );
	BOOL IsAlreadyHotkey( int nChar, BOOL bShift, BOOL bControl );
	BOOL AddHotkey( int nFunc, int nChar, BOOL bShift, BOOL bControl );
	LPHOTKEY GetHotkey( int nFunc);
	LPHOTKEY GetHotkey( int nChar, BOOL bShift, BOOL bControl );
	int  GetHotkeyType( int nChar, BOOL bShift, BOOL bControl );
	BOOL RemoveHotkey( int nChar, BOOL bShift, BOOL bControl );
	void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};
#endif

