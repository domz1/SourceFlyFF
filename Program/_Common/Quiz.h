// Quiz.h: interface for the CQuiz class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUIZ_H__96B9AD5D_E263_4FAB_AA19_715640D07381__INCLUDED_)
#define AFX_QUIZ_H__96B9AD5D_E263_4FAB_AA19_715640D07381__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef __QUIZ
#if defined(__DBSERVER) || defined(__WORLDSERVER)
#include "ar.h"
#define		REMAIN_QUIZ			30
#endif // defined(__DBSERVER) || defined(__WORLDSERVER)
#define		TYPE_OX				1
#define		TYPE_4C				2
#define		MAX_EXAMPLE			4
#define		MAX_QUIZ_SIZE		1024
#define		LIMIT_USER_COUNT	50


class CQuiz 
{
public:
	enum {	QE_CLOSE, QE_OPEN, QE_CLOSE_WAIT, QE_WATCHINGZONE_OPEN, QE_QUESTION, QE_CORRECT_ANSWER, QE_DROP_OUT, QE_WINNER	};
	
	CQuiz();
	~CQuiz();
	
	static CQuiz* GetInstance();

	void	SetType( int nType )	{	m_nType = nType;	}
	int		GetType()	{	return m_nType;	}

private:
	int			m_nType;		// O/X or multiple choice...
	
#if defined(__DBSERVER) || defined(__WORLDSERVER)
public:
	typedef struct _QUIZLIST
	{
		int		nQuizId;		// 퀴즈의 id
		CString	strQuestion;	// 퀴즈 문제
		int		nCorrect;		// 퀴즈 정답
		int		nItemId;		// 정답을 맞췄을 경우 지급할 아이템 id
		int		nItemNum;		// 아이템 개수
		
		void Serialize( CAr & ar )
		{
			if( ar.IsStoring() )
			{
				ar << nQuizId;
				ar.WriteString( strQuestion );
				ar << nCorrect;
				ar << nItemId;
				ar << nItemNum;
			}
			else
			{
				ar >> nQuizId;
				char szTemp[MAX_QUIZ_SIZE] = {0, };
				ar.ReadString( szTemp, MAX_QUIZ_SIZE );
				strQuestion = szTemp;
				strQuestion.Replace( "|", "\r\n" );
				ar >> nCorrect;
				ar >> nItemId;
				ar >> nItemNum;
			}
		}
	} QUIZLIST;
	typedef vector<QUIZLIST>	VEC_QUIZLIST;
	
	void	SetAutoMode( BOOL bAuto )	{	m_bAuto = bAuto;	}
	BOOL	IsAutoMode()	{	return m_bAuto;		}
	void	SetRun( BOOL bRun )		{	m_bRun = bRun;	}
	BOOL	IsRun()		{	return m_bRun;	}
	DWORD	GetEntranceTime()	{	return m_dwEntranceTime;	}
	void	Clear();
	void	LoadScript();
#endif // defined(__DBSERVER) || defined(__WORLDSERVER)
	
#if defined(__CLIENT) || defined(__WORLDSERVER)
public:
	enum {	ZONE_QUIZ = 100, ZONE_1, ZONE_2, ZONE_3, ZONE_4, ZONE_WATCHING, ZONE_EXIT	};
	
	void	SetState( int nState )	{	m_nState = nState;	}
	int		GetState()	{	return m_nState;	}
	void	InitQuizData( int nType );
	int		GetZoneType( CMover* pMover );
#endif // defined(__CLIENT) || defined(__WORLDSERVER)
	
#ifdef __WORLDSERVER
public:
	void	SetNextTime( DWORD dwNextTime )		{	m_dwNextTime = dwNextTime;	}
	DWORD	GetNextTime()		{	return m_dwNextTime;	}
	
	void	Process();
	void	OpenQuizEvent();
	void	MakeQuizList( QUIZLIST & QL );
	void	SetNPC();
	void	RemoveNPC();
	void	NoticeMessage();
	int		EntranceQuizEvent( CUser* pUser );
	int		TeleportToQuizEvent( CUser* pUser, int nZone );
	D3DXVECTOR3	GetTeleportPos( int nZone );
	void	DropOutWrongUser( int nQuizId, int nCorrect, int nItemId = 0, int nItemNum = 0 );
	BOOL	PtInCorrectZoneRect( CUser* pUser, int nCorrect );
	int		GetUserSelectExample( CUser* pUser );
	void	GoOut( CUser* pUser );
	void	CloseQuizEvent();
private:
	BOOL	IsInQuizEventPlayer( CWorld* pWorld, int nLayer, int nIndex );
#endif // __WORLDSERVER
	
#if defined(__DBSERVER) || defined(__WORLDSERVER)
private:
	BOOL			m_bAuto;						// GM or Auto
	BOOL			m_bRun;
	DWORD			m_dwEntranceTime;				// 입장 시간
	
public:
	int				m_nQuizSize;					// 퀴즈 문제 수
	VEC_QUIZLIST	m_vecQuizList;					// 퀴즈 리스트
#endif // defined(__DBSERVER) || defined(__WORLDSERVER)
	
#if defined(__CLIENT) || defined(__WORLDSERVER)
private:
	int				m_nState;						// 진행 상태
	
	typedef struct _QUIZDATA
	{
		D3DXVECTOR3		vQuizZonePos;				// 퀴즈 존 좌표
		D3DXVECTOR3		vWatchingZonePos;			// 대기 존 좌표
		D3DXVECTOR3		aExamplePos[MAX_EXAMPLE];	// 문제 보기 좌표
		CRect			aExampleRect[MAX_EXAMPLE];	// 문제 보기 구역
		CRect			QuizEventRect;				// 퀴즈 이벤트 존 구역
	} QUIZDATA;
	
public:
	QUIZDATA		m_sQuizData;
	//	CObj*			m_pVObj;
	//	CObj*			m_pHObj;
#endif // defined(__CLIENT) || defined(__WORLDSERVER)
	
#ifdef __WORLDSERVER
private:
	struct QUIZNPC
	{
		OBJID	idNpc;
		DWORD	dwNPCId;
		string	strCharKey;
		D3DXVECTOR3	vPos;
	};

	BOOL			m_bSelectLog;
	int				m_nChannel;						// 진행 채널
	int				m_nQuizCount;					// 풀이한 문제 수
	DWORD			m_dwWaitTime;					// 대기 시간
	DWORD			m_dwQuestionTime;				// 문제 풀이 시간
	DWORD			m_dwAnswerTime;					// 정답 공개 시간
	DWORD			m_dwWatchingZoneOpenTime;		// 대기존 개방 시간
	DWORD			m_dwCloseWaitTime;				// 퀴즈 이벤트 종료 대기 시간
	DWORD			m_dwNextTime;
	DWORD			m_dwPrizeItemId;				// 지급 상품 아이템 ID
	int				m_nPrizeItemNum;				// 지급 상품 아이템 갯수

	QUIZNPC			m_sNPC;
#endif // __WORLDSERVER
	
#ifdef __CLIENT
public:
	string			m_strQuestion;
	int				m_nQCount;
#endif // __CLIENT
};

#endif // __QUIZ
#endif // !defined(AFX_QUIZ_H__96B9AD5D_E263_4FAB_AA19_715640D07381__INCLUDED_)
