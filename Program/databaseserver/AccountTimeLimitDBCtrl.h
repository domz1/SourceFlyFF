// AccountTimeLimitDBCtrl.h: interface for the CAccountTimeLimitDBCtrl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACCOUNTTIMELIMITDBCTRL_H__E4562E24_E4DE_4E80_8150_E1F1E2F6C4AA__INCLUDED_)
#define AFX_ACCOUNTTIMELIMITDBCTRL_H__E4562E24_E4DE_4E80_8150_E1F1E2F6C4AA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef __VTN_TIMELIMIT
#include "dbcontroller.h"

enum
{
	QUERY_TIMELIMIT_LOAD = 0,
	QUERY_TIMELIMIT_UPDATE,
};

class CAccountTimeLimitDBCtrl : public CDbController  
{
public:
	CAccountTimeLimitDBCtrl();
	virtual ~CAccountTimeLimitDBCtrl();

	virtual	void Handler( LPDB_OVERLAPPED_PLUS pov, DWORD dwCompletionKey );
	virtual void OnTimer();
};


class CAccountTimeLimitDBMng
{
public:
	CAccountTimeLimitDBMng();
	~CAccountTimeLimitDBMng();
	static CAccountTimeLimitDBMng* GetInstance();

	BOOL PostRequest( int nQuery, BYTE* lpBuf = NULL, int nBufSize = 0, DWORD dwCompletionKey = 0 )
	{ return m_AccTimeLimitDBDtrl.PostRequest( nQuery, lpBuf, nBufSize, dwCompletionKey );	}


private:
	CAccountTimeLimitDBCtrl m_AccTimeLimitDBDtrl;
};
#endif // __VTN_TIMELIMIT

#endif // !defined(AFX_ACCOUNTTIMELIMITDBCTRL_H__E4562E24_E4DE_4E80_8150_E1F1E2F6C4AA__INCLUDED_)
