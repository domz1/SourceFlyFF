#ifndef __WNDTUTORIAL__H
#define __WNDTUTORIAL__H

#if __VER >= 12 // __MOD_TUTORIAL
struct TUTORIAL_STRING
{
	CString		strTitle;
	CString		strContents;
};

// typedef map<int, TUTORIAL_STRING>::value_type mgValType;
// typedef map<int, TUTORIAL_STRING>::iterator mgMapItor;

class CWndTutorial : public CWndNeuz 
{ 
public: 

	CString       m_strKeyword;
	map<int, TUTORIAL_STRING>  m_mapTutorial;

	CWndTutorial(); 
	~CWndTutorial(); 

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 

	BOOL LoadTutorial(LPCTSTR lpszFileName);
	BOOL AddToList(int nIndex);
}; 
#endif

#endif
