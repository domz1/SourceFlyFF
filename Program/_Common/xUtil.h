#ifndef		__XUTIL_H__
#define		__XUTIL_H__

extern LARGE_INTEGER	g_llFreq;
extern int				g_nMaxTri;

/////////////////////////////////////////////////////////////////////////////
// random
/*
DWORD		xRand( void );
DWORD		xRandom( DWORD num );
*/
extern	DWORD	g_next;
inline DWORD	xRand( void )
{
	g_next = g_next * 1103515245 + 12345;// + nRandomSeed[ i++ ];
	return g_next;	// 억단위 확률이 필요해서바꿈.
}

inline DWORD xRandom( DWORD num )
{
	return  xRand() % num;
}

DWORD		xRandom( DWORD min, DWORD max );	// min에서 max까지  
float		xRandomF( float num );
void		xSRand( DWORD seed );
inline int	random( int nNum ) { return rand() % nNum; }

/////////////////////////////////////////////////////////////////////////////
// PATH
//LPCTSTR		GetFileName( LPCTSTR szSrc );
//LPCTSTR		GetFileTitle( LPCTSTR szSrc );
//LPCTSTR		GetFileExt( LPCTSTR szSrc );
//LPCTSTR		GetFilePath( LPCTSTR szSrc );
void	GetFileName( LPCTSTR szSrc, LPTSTR szFileName );
void	GetFileTitle( LPCTSTR szSrc, LPTSTR szFileTitle );
void	GetFileExt( LPCTSTR szSrc, LPTSTR szFileExt );
void	GetFilePath( LPCTSTR szSrc, LPTSTR szFilePath );

/////////////////////////////////////////////////////////////////////////////
// ERROR
LPCTSTR		Error( LPCTSTR str, ... );
CString     GetNumberFormatEx( LPCTSTR szNumber );

/////////////////////////////////////////////////////////////////////////////
// ETC
enum  OSTYPE
{
	WINDOWS_UNKNOWN,
	WINDOWS_NT351,
	WINDOWS_95,
	WINDOWS_NT,
	WINDOWS_98,
	WINDOWS_ME,
	WINDOWS_2000,
	WINDOWS_XP,
	WINDOWS_SERVER_2003
};

extern OSTYPE g_osVersion;

LPCTSTR		GetCPUInfo( void ) ;
void		InitLIB( void );
#define		SWAP( A, B )	{ A ^= B; B ^= A; A ^= B; }
inline BOOL IsEmpty( LPCTSTR str )
{
	return (str[0] == '\0') ? TRUE : FALSE;
}

void	MakeEven( long& x );
int		StringFind( const char* string, int c );
void	StringTrimRight( char* szString );

void	SetLanguageInfo( int nLanguage, int nSubLanguage );		
int		GetLanguage();
int		GetSubLanguage();

#if __VER >= 15 // __2ND_PASSWORD_SYSTEM
void	SetUse2ndPassWord( BOOL bUse );
BOOL	IsUse2ndPassWord();
#endif // __2ND_PASSWORD_SYSTEM

/////////////////////////////////////////////////////////////////////////////
// MATH
inline	float xMathCCW( float x1, float y1, float x2, float y2 )
{
	return x1 * y2 - y1 * x2;
}

/////////////////////////////////////////////////////////////////////////////
#ifdef __PROF
#define		MAX_PROF_TIME	256
#define		MAX_PROF_STACK	256

struct	PROF_LIST
{
	int		m_nIdx;
	LARGE_INTEGER	m_lnPrev;
	int		m_nCheckTime;
	char	m_szMsg[64];	// strlen 64의 스트링이 MAX_PROF_TIME개.
};

class CProf
{
	PROF_LIST	m_List[ MAX_PROF_TIME ];
	char	m_szResultLast[1024];		// 최종 결과를 출력하기 위한 버퍼
	int		m_Stack[MAX_PROF_STACK];	// last index stack
	int		m_nMaxStack;
		
	int	m_nMax;
	int	m_nStackCnt;
public:
	char	m_szResult[1024];		// 최종 결과를 출력하기 위한 버퍼

	CProf()
	{
		Init();
	}
	~CProf()
	{
	}
	void	Init()
	{
		QueryPerformanceFrequency( &g_llFreq );
		m_nMax = 0;
		memset( m_List, 0, sizeof(PROF_LIST) * MAX_PROF_TIME );
		memset( m_szResult, 0, sizeof(m_szResult) );
		memset( m_szResultLast, 0, sizeof(m_szResultLast) );
		m_nStackCnt = 0;
		m_nMaxStack = 0;
	}

	// 다시 처음의 메인루프로 돌아올때 함.
	void	Reset()
	{
		m_nMax = 0;		// 체크갯수 초기화
		m_nStackCnt = 0;
		m_nMaxStack = 0;
		strcpy( m_szResultLast, m_szResult );
	}
	void	Check1()
	{
		PROF_LIST	*pList = &m_List[ m_nMax ];
		
		QueryPerformanceCounter( &pList->m_lnPrev );
		pList->m_nIdx = m_nMax;		// Check1이 불리고 다시 Check1이 불려도 max는 계속 증가 하고 스택카운트만 올린다.
		m_nStackCnt ++;
		if( m_nMaxStack >= MAX_PROF_STACK )		Error( "CProf : stack overflow" );
		m_Stack[m_nMaxStack++] = m_nMax;		// 리스트인덱스 푸쉬

		m_nMax ++;
	}
	void	Check2( LPCTSTR szMsg )
	{
		PROF_LIST	*pList;
		LARGE_INTEGER	lnCnt2;
		char	*sz;

		m_nMaxStack --;
		if( m_nMaxStack < 0 )		Error( "CProf : stack under flow" );
		int nLastIdx = m_Stack[ m_nMaxStack ];		// 마지막으로 Check1 했던곳의 인덱스를 POP함.

		pList = &m_List[ nLastIdx ];

		QueryPerformanceCounter( &lnCnt2 );
		pList->m_nCheckTime = (int)(lnCnt2.QuadPart - pList->m_lnPrev.QuadPart);
		sz = pList->m_szMsg;
//		if( IsEmpty(sz) )
		if( strcmp(sz, szMsg) != 0 )
			strcpy( sz, szMsg );
	}
	void	Show( float fFPS )
	{
		Show2( fFPS );
		Error( "%s", m_szResult );
	}
	void	Show2( float fFPS )
	{
		if( fFPS <= 1.0f )	return;
		PROF_LIST	*pList = m_List;;
		int nTotal = (int)(g_llFreq.QuadPart / (int)fFPS);		// 한프레임 토탈 갱신시간
		int		i;
		char	*p = m_szResult;
		int		nLen = 0, nMaxLen = 0;
		
		memset( p, 0, 1024 );
		for( i = 0; i < m_nMax; i ++ )
		{
			if( IsEmpty(pList->m_szMsg) )		continue;	// 메시지가 없으면 출력하지 않음.
			sprintf( p, "%6.2f : %s   %f sec\r\n", ((double)pList->m_nCheckTime / nTotal) * 100, pList->m_szMsg, (double)pList->m_nCheckTime / g_llFreq.QuadPart );
			nLen = strlen(p);
			p += nLen;
			nMaxLen += nLen;
			pList ++;
		}
		sprintf( p, "1/%d : %f,  %f fps, MaxTri = %d", (int)fFPS, (double)g_llFreq.QuadPart / fFPS, fFPS, g_nMaxTri );
		nMaxLen += strlen(p);
	}
	

};
extern CProf	g_Prof;

#endif // prof

#ifdef	__PROF
#define		CHECK1()	g_Prof.Check1();
#define		CHECK2(A)	g_Prof.Check2(A);
#else
#define		CHECK1()	/##/
#define		CHECK2(A)	/##/
#endif

#endif
