// Neuz.cpp : Defines the entry point for the application.
//
#include "stdafx.h"
#include "..\_UnhandledException\ExceptionHandler.h"
#include "lang.h"

#ifdef __LANG_1013
#include "langman.h"
#endif	// __LANG_1013

/*
static TCHAR g_szResourcePath[ MAX_PATH ];

void SetResourcePath()
{
	DWORD dwRet = ::GetCurrentDirectory( MAX_PATH, g_szResourcePath );
	if( dwRet == 0 )
	{
		LPCTSTR szErr = Error( "SetResourcePath : ¿¡·¯ %08x", GetLastError() );
		ADDERRORMSG( szErr );
	}
	_tcscat( g_szResourcePath, "\\" );
}
LPCTSTR GetResourcePath()
{
	return  g_szResourcePath;
}

CString MakePath( LPCTSTR lpszFileName )
{
	CString strFullPath;
	strFullPath += lpszFileName;
	return strFullPath;
}
*/
void MakePath( CString& strFullPath, LPCTSTR lpszDirName, LPCTSTR lpszFileName )
{
	strFullPath = lpszDirName;
	strFullPath += lpszDirName;
	strFullPath += lpszFileName;
}

CString MakePath( LPCTSTR lpDir, DWORD dwLan, LPCTSTR lpFile )
{
	CString	strFullPath( lpDir );

#ifdef __LANG_1013
	strFullPath		+= CLangMan::GetInstance()->GetLangData( dwLan )->szPath;
#else	// __LANG_1013
	switch( dwLan )
	{
		case LANG_KOR:
			strFullPath += "Korean\\";
			break;
		case LANG_USA:
			strFullPath += "English\\";
			break;
		case LANG_JAP:
			strFullPath += "Japanese\\";
			break;
		case LANG_CHI:
			strFullPath += "Chinese\\";
			break;
		case LANG_THA:
			strFullPath += "Thailand\\";
			break;
		case LANG_TWN:
		case LANG_HK:
			strFullPath += "Taiwan\\";
			break;
		case LANG_VTN:
			strFullPath += "Vietnam\\";
			break;
		case LANG_RUS:
			strFullPath += "Russian\\";
			break;
		default:
			AfxMessageBox( "unknown language setting" );
			break;
	}
#endif	// __LANG_1013

	strFullPath		+= lpFile;
	return strFullPath;
}


CString MakePath( LPCTSTR lpszDirName, LPCTSTR lpszFileName )
{
	CString strFullPath( lpszDirName );
	strFullPath += lpszFileName;
	return strFullPath;
}

void MakePath( TCHAR* lpszFullPath, LPCTSTR lpszDirName, LPCTSTR lpszFileName )
{
	_tcscpy( lpszFullPath, lpszDirName );
	_tcscat( lpszFullPath, lpszFileName );
}
