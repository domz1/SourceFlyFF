#include "stdafx.h"
#include <direct.h>
#include "xUtil.h"
#include "../../resource/lang.h"
////////////////////////////////////////////////////////////////////////////////////
// global variable
////////////////////////////////////////////////////////////////////////////////////

#ifdef __PROF
CProf			g_Prof;
#endif

int				g_nMaxTri = 0;
static BOOL		g_bShowMsgBox	= TRUE;
LARGE_INTEGER	g_llFreq;
DWORD			g_next;
char			g_szWorkDir[256];		// 워킹폴더
char			g_szExeDir[256];		// 실행파일 경로
OSTYPE			g_osVersion = WINDOWS_UNKNOWN;

static int		g_nLanguage = -1; // LANG_KOR;
static int		g_nSubLanguage = 0;	//LANG_SUB_DEFAULT

#if __VER >= 15 // __2ND_PASSWORD_SYSTEM
static BOOL		g_b2ndPassWord = FALSE;
#endif // __2ND_PASSWORD_SYSTEM

////////////////////////////////////////////////////////////////////////////////////
// global functions
////////////////////////////////////////////////////////////////////////////////////

/*
DWORD	xRand( void )
{
	g_next = g_next * 1103515245 + 12345;// + nRandomSeed[ i++ ];
	return g_next;	// 억단위 확률이 필요해서바꿈.
}

DWORD xRandom( DWORD num )
{
	return  xRand() % num;
}
*/

DWORD xRandom( DWORD min, DWORD max /* max : max -1 */ )
{
	if( max > min )
		return min + xRandom( max - min );
	else
		return min;
}


float xRandomF( float num )
{
	return ((float)rand() / (float)RAND_MAX) * (float)num;
}

void	xSRand( DWORD seed )
{
   g_next = seed;
}

#ifdef __CLIENT
LPCTSTR Error( LPCTSTR strFormat, ... )
{
    char szBuff[8192];
    static char szStr[8192];

    va_list args;
    va_start(args, strFormat);
    int n = _vsntprintf( szBuff, 8191, strFormat, args );
    va_end(args);

	if( n > 0 )
	{
		#if defined(_DEBUG)
			if( g_bShowMsgBox && MessageBox( NULL, szBuff, "오류", MB_OKCANCEL) == IDCANCEL )
				g_bShowMsgBox	= FALSE;		
		#else
			SYSTEMTIME time;
			GetLocalTime( &time );
			sprintf( szStr, "%d/%2d/%2d   %02d:%02d:%02d   %s\n", 
				time.wYear, time.wMonth, time.wDay,
				time.wHour, time.wMinute, time.wSecond, 
				szBuff );		
			
			DEBUGOUT2( szStr );	// 파일로 쓰기 	
		#endif
	}
	else
		szStr[0] = '\0';

	return szStr;
}

#else // __CLIENT
LPCTSTR Error( LPCTSTR strFormat, ... )
{
    char szBuff[8192];
    va_list args;
    va_start(args, strFormat);
    int n = _vsntprintf( szBuff, 8191, strFormat, args );
    va_end(args);

	if( n > 0 )
	{
#if defined(_DEBUG) && defined(__XUZHU)
		if( g_bShowMsgBox && MessageBox( NULL, szBuff, "오류", MB_OKCANCEL) == IDCANCEL )
			g_bShowMsgBox	= FALSE;		
#endif
		char szStr[8192];
		SYSTEMTIME time;
		GetLocalTime( &time );
		sprintf( szStr, "%d/%2d/%2d   %02d:%02d:%02d   %s\n", 
			time.wYear, time.wMonth, time.wDay,
			time.wHour, time.wMinute, time.wSecond, 
			szBuff );		
		
		DEBUGOUT2( szStr );	
	}

	return "";
}
#endif // !__CLIENT



// 파일타이틀과 확장자를 리턴  ex) test.wri
/*
LPCTSTR		GetFileName( LPCTSTR szSrc )
{
	int		len, i;

	// 파일명만 추출.
	memset( _szTempBuff, 0, 256 );
	len = strlen( szSrc );		// 풀네임 길이 읽음
	i = len;
	while( i-- )
	{	// 스트링 뒤에서 부터 훑으며 \가 나오는곳을 찾는다.
		if( szSrc[i] == '\\' )
		{
			strcpy( _szTempBuff, szSrc + i + 1 );	// \가 나온 다음 칸부터 파일명으로 씀
			break;
		}
		if( i == 0 )		// 끝까지 검색했는데도 \가 나오지 않았으면 그대로 씀
			strcpy( _szTempBuff, szSrc );
	}

	return _szTempBuff;
}
*/
void	GetFileName( LPCTSTR szSrc, LPTSTR szFileName )
{
	int	len, i;
	// 파일명만 추출.
	i	= len	= strlen( szSrc );		// 풀네임 길이 읽음
	while( i-- )
	{	// 스트링 뒤에서 부터 훑으며 \가 나오는곳을 찾는다.
		if( szSrc[i] == '\\' )
		{
			lstrcpy( szFileName, szSrc + i + 1 );	// \가 나온 다음 칸부터 파일명으로 씀
			break;
		}
		if( i == 0 )		// 끝까지 검색했는데도 \가 나오지 않았으면 그대로 씀
			lstrcpy( szFileName, szSrc );
	}
}

// 파일타이틀만 임시버퍼에 저장	ex) test
/*
LPCTSTR		GetFileTitle( LPCTSTR szSrc )
{
	int		len, i;
	LPCTSTR	szName;
	char	buff[256];

	szName = GetFileName( szSrc );		// 패스는 떼네고 파일명의 시작 포인터를 리턴
	strcpy( buff, szName );
	szSrc = buff;		// xuzhu.txt

	// 파일명만 추출.
	len = strlen( szSrc );		// 풀네임 길이 읽음
	for( i = 0; i < len; i ++ )
	{
		if( szSrc[i] == '.' )	break;
		_szTempBuff[i] = szSrc[i];
	}
	_szTempBuff[i] = '\0';

	return _szTempBuff;
}
*/
void	GetFileTitle( LPCTSTR szSrc, LPTSTR szFileTitle )
{
	int		len, i;
	GetFileName( szSrc, szFileTitle );		// 패스 분리

	// 파일명 추출.
	len	= strlen( szFileTitle );	// 풀네임 길이 읽음
	for( i = 0; i < len; i ++ )
	{
		if( szFileTitle[i] == '.' )
			break;
	}
	szFileTitle[i]	= '\0';
}


// 확장자만 리턴.
/*
LPCTSTR		GetFileExt( LPCTSTR szSrc )
{
	int		len, i;
	
	// 파일명만 추출.
	
	memset( _szTempBuff, 0, sizeof(_szTempBuff) );
	len = strlen( szSrc );		// 풀네임 길이 읽음
	i = len;
	while( i-- )
	{	// 스트링 뒤에서 부터 훑으며 .이 나오는곳을 찾는다.
		if( szSrc[i] == '.' )
		{
			strcpy( _szTempBuff, szSrc + i + 1 );	// .가 나온 다음 칸부터 확장자로 씀
			break;
		}
	}
	
	return _szTempBuff;
}
*/
void	GetFileExt( LPCTSTR szSrc, LPTSTR szFileExt )
{
	int		len, i;
	len	= strlen( szSrc );		// 풀네임 길이 읽음
	i	= len;
	while( i-- )
	{	// 스트링 뒤에서 부터 훑으며 .이 나오는곳을 찾는다.
		if( szSrc[i] == '.' )
		{
			lstrcpy( szFileExt, szSrc + i + 1 );	// .가 나온 다음 칸부터 확장자로 씀
			break;
		}
	}
}

// 풀네임에서 패스 부분만 리턴함
/*
LPCTSTR		GetFilePath( LPCTSTR szSrc )
{
	int len = strlen( szSrc );		// 풀네임 길이구함

	// 마지막 \찾음
	for( int i = len-1; i >= 0; i -- )
	{
		if( szSrc[i] == '\\' )
			break;
	}
	strncpy( _szTempBuff, szSrc, i+1 );		// 패스 부분만 카피
	_szTempBuff[i+1] = '\0';
	return _szTempBuff;
}
*/

void	GetFilePath( LPCTSTR szSrc, LPTSTR szFilePath )
{
	int len	= strlen( szSrc );	// 풀네임 길이구함
	// 마지막 \찾음
	for( int i = len-1; i >= 0; i -- )
	{
		if( szSrc[i] == '\\' )
			break;
	}
	strncpy( szFilePath, szSrc, i+1 );		// 패스 부분만 카피
	szFilePath[i+1] = '\0';
}


//CPU 정보를 나타낸다.
#ifdef __CLIENT
static	char	_szTempBuff[256];
LPCTSTR GetCPUInfo( void ) 
{
	CRegKey rg;
	DWORD dwCount;

	memset( _szTempBuff, 0, sizeof(_szTempBuff) );
	
/*	if( rg.Open( HKEY_LOCAL_MACHINE, "Hardware\\Description\\System\\CentralProcessor\\0" ) == ERROR_SUCCESS ) 
	{
		if( rg.QueryValue( _szTempBuff, "ProcessorNameString", &dwCount ) != ERROR_SUCCESS )
			if( rg.QueryValue( _szTempBuff, "Identifier", &dwCount ) != ERROR_SUCCESS )
				strcpy( _szTempBuff, "unknown" );
	} else
	{
		strcpy( _szTempBuff, "unknown" );
	}
		
	rg.Close();
*/
	HKEY hKey;
	if( RegOpenKeyEx( HKEY_LOCAL_MACHINE, "Hardware\\Description\\System\\CentralProcessor\\0", 0, KEY_READ, &hKey ) == ERROR_SUCCESS )
	{
		dwCount = sizeof( _szTempBuff );
		if ( RegQueryValueEx( hKey, "ProcessorNameString", NULL, NULL, (LPBYTE)_szTempBuff, &dwCount ) != ERROR_SUCCESS )
			if ( RegQueryValueEx( hKey, "Identifier", NULL, NULL, (LPBYTE)_szTempBuff, &dwCount ) != ERROR_SUCCESS )
				strcpy( _szTempBuff, "unknown" );
	}
	else
		strcpy( _szTempBuff, "unknown" );

	RegCloseKey( hKey );

	OSVERSIONINFO versionInformation;
	versionInformation.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	
	BOOL b = GetVersionEx( &versionInformation );
	if( b )
	{
		strcat( _szTempBuff, " " );
		switch( versionInformation.dwMajorVersion ) 
		{
		case 3:
			strcat( _szTempBuff, "Windows NT 3.51 " );
			g_osVersion = WINDOWS_NT351;
			break;
		case 4:
			switch( versionInformation.dwMinorVersion )
			{
			case 0:		
				if( versionInformation.dwPlatformId == VER_PLATFORM_WIN32_NT )
				{
					strcat( _szTempBuff, "Windows NT 4.0 " );
					g_osVersion = WINDOWS_NT;
				}
				else
				{
					strcat( _szTempBuff, "Windows 95 " );	
					g_osVersion = WINDOWS_95;
				}
				break;
			case 10:	strcat( _szTempBuff, "Windows 98 " );	g_osVersion = WINDOWS_98;	break;
			case 90:	strcat( _szTempBuff, "Windows Me " );	g_osVersion = WINDOWS_ME;	break;
			}
			break;
		case 5:
			switch( versionInformation.dwMinorVersion )
			{
			case 0:		strcat( _szTempBuff, "Windows 2000 " );	g_osVersion = WINDOWS_2000;	break;
			case 1:		strcat( _szTempBuff, "Windows XP " );	g_osVersion = WINDOWS_XP;	break;
			case 2:		strcat( _szTempBuff, "Windows Server 2003 family " );	g_osVersion = WINDOWS_SERVER_2003;	break;
				
			}
			break;
		}
		if( versionInformation.szCSDVersion[0] )
			strcat( _szTempBuff, versionInformation.szCSDVersion );

	}
	return _szTempBuff;
}
#endif // client

int StringFind( const char* string, int c )
{
	char* pdest;
	return ( pdest = strchr( string, c ) ) ? pdest - string : -1;
}

void StringTrimRight( char* szString )
{
	// find beginning of trailing spaces by starting at beginning (DBCS aware)
	LPTSTR lpsz = szString;
	LPTSTR lpszLast = NULL;

	while (*lpsz != '\0')
	{
		if (_istspace(*lpsz))
		{
			if (lpszLast == NULL)
				lpszLast = lpsz;
		}
		else
			lpszLast = NULL;
		lpsz = _tcsinc(lpsz);
	}

	if (lpszLast != NULL)
	{
		// truncate at trailing space start
		*lpszLast = '\0';
	}
}

CString GetNumberFormatEx( LPCTSTR szNumber )
{
	ASSERT( szNumber );

	CString str;

	int nLength = lstrlen( szNumber );	
	for( int i=nLength-1; i>=0; --i)
	{
		str.Insert(0, szNumber[i]);
		if( ((nLength-i) % 3) == 0 && i != 0 )
			str.Insert(0, ',' );
	}
	return str;
}

void MakeEven( long& x )
{
	x ++;
	x &= (~0x01);
}



void SetLanguageInfo( int nLanguage, int nSubLanguage )
{
	g_nLanguage = nLanguage;
	g_nSubLanguage = nSubLanguage;
}

int GetLanguage()
{
	ASSERT( g_nLanguage >= 0 );
	return g_nLanguage;
}

int GetSubLanguage()
{
	return g_nSubLanguage;
}

#if __VER >= 15 // __2ND_PASSWORD_SYSTEM
void SetUse2ndPassWord( BOOL bUse )
{
	g_b2ndPassWord = bUse;
}

BOOL IsUse2ndPassWord()
{
	return g_b2ndPassWord;
}
#endif // __2ND_PASSWORD_SYSTEM