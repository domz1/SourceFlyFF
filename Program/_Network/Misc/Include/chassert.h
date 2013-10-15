#ifndef __CHASSERT_H__
#define	__CHASSERT_H__

#define	MAXBUFF		1024
inline void chASSERTFAIL( LPCSTR file, int line, PCSTR expr, LPSTR szFmt, ... )
{
	char	pszBuffer[MAXBUFF];
	va_list		marker;

	va_start( marker, szFmt );
	if( _vsnprintf( pszBuffer, MAXBUFF, szFmt, marker ) < 0 )
	{
		wsprintf( pszBuffer, _T( "Buffer overflow reporting '%*.*s'" ), 50, 50, pszBuffer );
		return;
	}
	va_end( marker );

	char	pchBuffer[MAXBUFF];
	wsprintfA( pchBuffer, "%s(%d): %s\r\n\t-%s\r\n", file, line, expr, pszBuffer );	// Pointer to a buffer to receive the formatted output. The maximum size of the buffer is 1024 bytes.

	FILE* f;
#ifdef __IDC
	if( !( f = fopen( "..\\assert.log", "a" ) ) )	return;
#else	// __IDC
	if( !( f = fopen( "assert.log", "a" ) ) )	return;
#endif	// __IDC

	fprintf( f, pchBuffer );
	fclose( f );
	fflush( f );
}

#define	chASSERT( x )	if( !( x ) )	{	chASSERTFAIL( __FILE__, __LINE__, #x, "" );		return;		}



#endif	// __CHASSERT_H__
