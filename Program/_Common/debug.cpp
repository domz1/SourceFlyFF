#include "stdafx.h"
#include "Debug.h"


#ifdef __IDC
	static const char g_szERROR[] = "..\\error.log";
	static const char g_szEH[]    = "..\\eh.log";
#else	// __IDC
	static const char g_szERROR[] = "error.log";
	static const char g_szEH[]    = "eh.log";
#endif	

enum LOG_TYPE
{
	LOG_SIMPLE,		
	LOG_DATE,
};

static void LogFile( LPCTSTR szString, LPCTSTR szFileName, LOG_TYPE type )
{
	FILE* fp;
	fp = fopen( MakeFileNameDate( szFileName ), "a" );
	if( fp == NULL )
		return;

	if( type == LOG_DATE )
	{
		SYSTEMTIME	time;
		GetLocalTime( &time );

		char szTime[32];
		sprintf( szTime, 
				 "%d/%2d/%2d %02d:%02d:%02d\n", 
				 time.wYear, time.wMonth, time.wDay, 
				 time.wHour, time.wMinute, time.wSecond );

		fprintf( fp, szTime );
	}

	fprintf( fp,"%s\n", szString );
	fclose(fp);
}

static void FileOutV( LOG_TYPE logType, LPCTSTR lpszFileName, LPCTSTR lpszFormat, va_list args )
{
	TCHAR szBuffer[4096];

	int n = _vsntprintf(szBuffer, 4095, lpszFormat, args);

	if( n > 0 )
		LogFile( szBuffer, lpszFileName, logType );
}

void DEBUGOUT2( LPCTSTR szString, LPCTSTR szFileName )
{
	LogFile( szString, szFileName, LOG_SIMPLE );	
}

void FILEOUT(LPCTSTR lpszFileName,LPCTSTR lpszFormat, ...)
{
	va_list args;
	va_start(args, lpszFormat);

	FileOutV( LOG_SIMPLE, lpszFileName, lpszFormat, args );

	va_end(args);
}


void WriteLog( LPCTSTR lpszFormat, ... )
{
	va_list args;
	va_start(args, lpszFormat);

	FileOutV( LOG_DATE, g_szERROR, lpszFormat, args );

	va_end(args);
}

void WriteError( LPCTSTR lpszFormat, ... )
{
	va_list args;
	va_start(args, lpszFormat);

	FileOutV( LOG_DATE, g_szEH, lpszFormat, args );

	va_end(args);
}

void OUTPUTDEBUGSTRING( LPCTSTR lpszFormat, ... )
{
	char lpString[1024]		= { 0,};
	va_list	args;
	va_start( args, lpszFormat );
	int n	= _vsntprintf( lpString, 1024, lpszFormat, args );
	va_end( args );
	if( n > 0 )
		::OutputDebugString( lpString );
}

const char* MakeFileNameDate( const char* lpszFileName )
{
	static char szFullFileName[_MAX_PATH+_MAX_FNAME];
	::memset( szFullFileName, 0, sizeof( szFullFileName ) );

#ifdef __CLIENT
	::GetCurrentDirectory( _MAX_PATH, szFullFileName );
	_tcscat( szFullFileName, "\\Log\\" );
	::CreateDirectory( szFullFileName, NULL );
	_tcscat( szFullFileName, lpszFileName );
#else // __CLIENT
	_tcscpy( szFullFileName, lpszFileName );
#endif // __CLIENT

	char* ptr = _tcsrchr( szFullFileName, '.' );
	if( ptr != NULL )
	{
		char szExt[5] = {0, };
		_tcscpy( szExt, ptr );
		*ptr = '\0';
		char szDate[10] = {0, };
		SYSTEMTIME	time;
		GetLocalTime( &time );
		_stprintf( szDate, "_%d%02d%02d", time.wYear, time.wMonth, time.wDay );
		_tcscat( szFullFileName, szDate );
		_tcscat( szFullFileName, szExt );

		return szFullFileName;
	}
	return lpszFileName;
}






