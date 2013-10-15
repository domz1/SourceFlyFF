#ifndef __SCRIPTDIALOG_H
#define __SCRIPTDIALOG_H

#include "script.h"
#include "timer.h"

class CObj;


typedef struct tagScript
{
//	TCHAR  m_szWord[ 64 ];
	TCHAR  m_szKey[ 64 ];
	TCHAR* m_pScript;
//	BOOL   m_bStatus;
//	BOOL   m_bHidden;
} SCRIPT,* LPSCRIPT;

class CScriptDialog // : public CObject
{
public:
	CScriptDialog();
	virtual ~CScriptDialog();

private:
	TCHAR			m_szFileName[ 64 ];

public:
	CTimer			m_aTimer     [MAX_SCRIPT_TIMER];
	int				m_anGlobalVal[MAX_GLOBAL_ARRAY];
	CPtrArray		m_scriptAry;


	void			Free();
	BOOL			IsEmpty();
	BOOL			InterpretScript(CScript& smc);
	BOOL			LoadScript(LPCTSTR lpszFileName); 
	LPSCRIPT		Lookup(LPCTSTR pKey);
//	LPSCRIPT		Lookup(int nId);

	static	char	s_pszName[64];
	static	char	s_pszKey[260];
	static	char	s_pszBeforeRun[260];
	static	char	s_pszAfterRun[260];
	static	void	SetLatestDialog( const char* pszName, const char* pszKey )
	{	
		lstrcpy( s_pszName, pszName );	lstrcpy( s_pszKey, pszKey );	
	}
	static	void	SetBeforeRun( const char* pszBeforeRun )
	{	
		lstrcpy( s_pszBeforeRun, pszBeforeRun );	
	}
	static	void	SetAfterRun( const char* pszAfterRun )
	{	
		lstrcpy( s_pszAfterRun, pszAfterRun );	
	}

	// 처음 시작 
	BOOL Run( LPCTSTR pKey, int* nResult, int nValue = 0, int nDstId = 0, int nSrcId = 0, int nQuestId = 0, int nCaller = WES_DIALOG );
};
#endif 
