#ifndef __WNDDIALOG2__H
#define __WNDDIALOG2__H

class CWndAnswer : public CWndButton
{
public:
	LPVOID m_pWordButton;
};

class CWndDialog : public CWndNeuz 
{ 
	CWndAnswer* m_apWndAnswer[ 6 ];
public: 
	CTimer m_timer;
	CTexture m_texChar;
	BOOL m_bWordButtonEnable;
	int m_nWordButtonNum;
	int m_nKeyButtonNum;
	int m_nContextButtonNum;
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	int m_nNewQuestListNumber;
	int m_nCurrentQuestListNumber;
	int m_nSelectKeyButton;
#endif // __IMPROVE_QUEST_INTERFACE
	CUIntArray m_aContextMark[ 32 ];
	struct WORDBUTTON 
	{
		BOOL bStatus;
		CRect rect; 
		TCHAR szWord[ 64 ];
		TCHAR szKey[ 64 ];
		DWORD dwParam;
		DWORD dwParam2;
		int nLinkIndex; // 줄바꿈으로 단어가 끊어진 경우 연결하기 위한 인덱스 
	};
	WORDBUTTON m_aWordButton[ 32 ];
	WORDBUTTON m_aKeyButton[ 32 ];
	WORDBUTTON m_aContextButton[ 32 ];
	CEditString m_string;

	DWORD m_dwQuest;// context 버튼에서 사용함 
	BOOL m_bSay;
	int m_nCurArray;
	CPtrArray m_strArray;
	OBJID m_idMover;
	CMapStringToString m_mapWordToOriginal;
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
private:
	CWndListBox m_WndNewQuestListBox;
	CWndListBox m_WndCurrentQuestListBox;
	CTexture* m_pNewQuestListIconTexture;
	CTexture* m_pExpectedQuestListIconTexture;
	CTexture* m_pCurrentQuestListIconTexture;
	CTexture* m_pCompleteQuestListIconTexture;
public:
#endif // __IMPROVE_QUEST_INTERFACE
	 
	CWndDialog(); 
	~CWndDialog(); 

	void RemoveAllKeyButton();
	void RemoveKeyButton( LPCTSTR lpszKey );
	void AddAnswerButton( LPCTSTR lpszWord, LPCTSTR lpszKey, DWORD dwParam, DWORD dwQuest );
	void AddKeyButton( LPCTSTR lpszWord, LPCTSTR lpszKey, DWORD dwParam, DWORD dwQuest );
	void ParsingString( LPCTSTR lpszString );
	void Say( LPCTSTR lpszString, DWORD dwQuest );
	void EndSay();
	void BeginText();
	void MakeKeyButton();
	void MakeContextButton();
	void MakeAnswerButton();
	void UpdateButtonEnable();
	BOOL OnChildNotify(UINT message,UINT nID,LRESULT* pLResult);
	void RunScript( const char* szKey, DWORD dwParam, DWORD dwQuest );
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	void AddNewQuestList( const LPCTSTR lpszWord, const LPCTSTR lpszKey, const DWORD dwParam, const DWORD dwQuest );
	void AddCurrentQuestList( const LPCTSTR lpszWord, const LPCTSTR lpszKey, const DWORD dwParam, const DWORD dwQuest );
	void MakeQuestKeyButton( const CString& rstrKeyButton );

private:
	void RenderNewQuestListIcon( C2DRender* p2DRender );
	void RenderCurrentQuestListIcon( C2DRender* p2DRender );
	void AddQuestList( CWndListBox& pWndListBox, int& nQuestListNumber, const LPCTSTR lpszWord, const LPCTSTR lpszKey, const DWORD dwParam, const DWORD dwQuest );

public:
#endif // __IMPROVE_QUEST_INTERFACE

	virtual BOOL Process();
	virtual BOOL OnSetCursor( CWndBase* pWndBase, UINT nHitTest, UINT message );
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual void OnMouseWndSurface( CPoint point);
}; 
#endif
