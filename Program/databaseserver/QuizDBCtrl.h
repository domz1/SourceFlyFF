// QuizDBCtrl.h: interface for the CQuizDBCtrl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUIZDBCTRL_H__4019A7AB_9F66_46B7_9116_D45D7338A3D8__INCLUDED_)
#define AFX_QUIZDBCTRL_H__4019A7AB_9F66_46B7_9116_D45D7338A3D8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef __QUIZ
#include "dbcontroller.h"

//////////////////////////////////////////////////////////////////////
// CQuizDBCtrl		Construction/Destruction
//////////////////////////////////////////////////////////////////////
enum
{
	QUERY_QUIZ_OPEN,
	QUERY_QUIZ_STATE,
	QUERY_QUIZ_ENTRANCE,
	QUERY_QUIZ_SELECT,
	QUERY_QUIZ_POST_PRIZE,
	QUERY_QUIZ_CLOSE
};

class CQuizDBCtrl :	public CDbController
{
public:
	CQuizDBCtrl();
	virtual ~CQuizDBCtrl();
	
	virtual void Handler( LPDB_OVERLAPPED_PLUS pov, DWORD dwCompletionKey );
	virtual void OnTimer();

	BOOL		LoadQuizEventId();

	CQuery*		m_pQuery;

private:
	typedef struct _WORLDDATA
	{
		BOOL	bQuizSend;
		int		nQuizEventId;
		int		nChannel;
		int		nState;
		int		nCount;
		DWORD	dwQuizSendCycle;
		DWORD	dwNoticeCycle;
		_WORLDDATA() : bQuizSend( FALSE ), nQuizEventId( 0 ), nChannel( 0 ), nState( 0 ), nCount( 0 ), dwQuizSendCycle( 0 ), dwNoticeCycle( 0 )	{}
	} WORLDDATA;

	void		InitQuizEventId( int nId )	{	m_nQuizEventId = nId;	}
	int			SetQuizEventId()	{	return ++m_nQuizEventId;	}

	void		InitWorldData( DPID dpId, int nChannel, int nState );
	void		DeleteWorldData( DPID dpId, int nState );
	WORLDDATA*	GetWorldData( DPID dpId );
	void		SetState( DPID dpId, int nState);
	int			GetQuizEventId( DPID dpId );
	void		InsertQuizEvent( DPID dpId, int nChannel );
	void		UpdateQuizEvent( DPID dpId, int nChannel, int nWinnerCount, int nQuizCount );
	void		LoadQuizList( DPID dpId );
	void		EntranceQuizEvent( DPID dpId, u_long idPlayer, int nChannel );
	void		SelectQuizExample( DPID dpId, u_long idPlayer, int nChannel, int nQuizId, int nSelect, int nAnswer );
	void		PostPrizeItem( u_long idPlayer, DWORD dwItemId, int nItemNum );


	int		m_nQuizEventId;						// 퀴즈 이벤트 회차
	DWORD	m_dwQuizSendTick;

	map<DPID, WORLDDATA>	m_mapWorldData;
};

//////////////////////////////////////////////////////////////////////
// CQuizDBMng		Construction/Destruction
//////////////////////////////////////////////////////////////////////
class CQuizDBMng
{
public:
	CQuizDBMng();
	~CQuizDBMng();
	static CQuizDBMng* GetInstance();

	BOOL InitQuizEventId();
	CQuery* CreateLogQuery();

	BOOL PostRequest( int nQuery, BYTE* lpBuf = NULL, int nBufSize = 0, DWORD dwCompletionKey = 0 )
	{ return m_QuizDBCtrl.PostRequest( nQuery, lpBuf, nBufSize, dwCompletionKey );	}

private:
	CQuizDBCtrl		m_QuizDBCtrl;
};

#endif // __QUIZ
#endif // !defined(AFX_QUIZDBCTRL_H__4019A7AB_9F66_46B7_9116_D45D7338A3D8__INCLUDED_)
