// WndQuizEvent.h: interface for the CWndQuizEvent class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WNDQUIZEVENT_H__962131ED_62BC_4644_9902_5C9929A95737__INCLUDED_)
#define AFX_WNDQUIZEVENT_H__962131ED_62BC_4644_9902_5C9929A95737__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef __QUIZ

class CWndQuizEventConfirm : public CWndNeuz
{
public:
	CWndQuizEventConfirm( BOOL bEnter );
	virtual ~CWndQuizEventConfirm();

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual	void OnInitialUpdate();
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual void OnDraw( C2DRender* p2DRender );

	void	SetString( const char* lpszMessage );
private:
	BOOL m_bEnter;
};

class CWndQuizEventQuestionOX : public CWndNeuz
{
public:
	CWndQuizEventQuestionOX();
	virtual ~CWndQuizEventQuestionOX();

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual	void OnInitialUpdate();
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual void OnDraw( C2DRender* p2DRender );

	void	UpdateQuestion( const char* lpszQuestion = NULL );

private:
	CTexture m_texChar;
};

class CWndQuizEventQuestion4C : public CWndNeuz
{
public:
	CWndQuizEventQuestion4C();
	virtual ~CWndQuizEventQuestion4C();

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual	void OnInitialUpdate();
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual void OnDraw( C2DRender* p2DRender );

	void	UpdateQuestion( const char* lpszQuestion = NULL );

private:
	CTexture m_texChar;
};

class CWndQuizEventButton : public CWndNeuz 
{
public:
	CWndQuizEventButton();
	virtual ~CWndQuizEventButton();

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual	void OnInitialUpdate();
	virtual void PaintFrame( C2DRender* p2DRender );
	virtual void OnDraw( C2DRender* p2DRender );
	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();
	virtual void OnLButtonDown( UINT nFlags, CPoint point );
	virtual void OnLButtonUp( UINT nFlags, CPoint point );
	virtual BOOL Process();

	CTexturePack m_BtnTexture;
	BOOL m_bLoadTexMap;
	BOOL m_bFocus;
	BOOL m_bPush;

	CPoint m_ptPush;
	int m_nAlpha;
	BOOL m_bReverse;
};

#endif // __QUIZ
#endif // !defined(AFX_WNDQUIZEVENT_H__962131ED_62BC_4644_9902_5C9929A95737__INCLUDED_)
