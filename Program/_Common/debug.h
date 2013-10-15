#ifndef __DEBUG_H
#define __DEBUG_H


#ifdef __IDC
	extern void DEBUGOUT2( LPCTSTR lpszStr, LPCTSTR szFileName = _T( "..\\error.txt" ) );
#else
	extern void DEBUGOUT2( LPCTSTR lpszStr, LPCTSTR szFileName = _T( "error.txt" ) );
#endif

extern void FILEOUT (LPCTSTR lpszFileName,LPCTSTR lpszFormat, ...);
extern void WriteLog( LPCTSTR lpszFormat, ... );
extern void WriteError( LPCTSTR lpszFormat, ... );

extern void OUTPUTDEBUGSTRING( LPCTSTR lpszFormat, ... );

extern const char* MakeFileNameDate( const char* lpszFileName );

#endif