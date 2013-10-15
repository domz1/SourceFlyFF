#include "StdAfx.h"
#include "MyTrace.h"

CMyTrace::CMyTrace()
{
	m_hfont	= NULL;
	m_uHead		= m_uTail	= 0;
	memset( m_szStatusBar, 0, sizeof(m_szStatusBar) );
}

CMyTrace::~CMyTrace()
{
	if( m_hfont )
		::DeleteObject( m_hfont );
}

int CMyTrace::FindKey( u_long uKey )
{
	if( m_uHead <= m_uTail ) {
		for( int i =m_uHead; i < m_uTail; i++ ) {
			if( m_mts[i].uKey == uKey )
				return i;
		}
	}
	else
	{
		for( int i = m_uHead; i < MAX_LOG; i++ ) {
			if( m_mts[i].uKey == uKey )
				return i;
		}
		for( i = 0; i < m_uTail; i++ ) {
			if( m_mts[i].uKey == uKey )
				return i;
		}
	}
	return -1;
}

void CMyTrace::Add( u_long uKey, BOOL fInverse, LPCTSTR lpszFormat, ... )
{
	static u_long uKeyStatusBar		= CMyTrace::Key( "status" );

	va_list args;
	va_start( args, lpszFormat );
	int nBuf;
	TCHAR szBuffer[512]	= { 0,};
	nBuf	= _vsntprintf( szBuffer, 512, lpszFormat, args );
	va_end( args );

	if( fInverse )
	{
		int cbString	= strlen( szBuffer );
		memset( &szBuffer[cbString], 0x20, 160 - cbString );
		szBuffer[159]	= '\0';
	}

	int nIndex;
	m_AddRemoveLock.Enter();

	if( uKey == uKeyStatusBar ) {
		strcpy( m_szStatusBar, szBuffer );
	}
	else {
		if( uKey == 0 || ( nIndex = FindKey( uKey ) ) < 0 ) {
			m_mts[m_uTail].uKey		= uKey;
			m_mts[m_uTail].fInverse		= fInverse;

			strcpy( m_mts[m_uTail].szText, szBuffer );

			m_uTail		= ( m_uTail + 1 ) % MAX_LOG;
			if( m_uTail == m_uHead ) {
				m_uHead		= ( m_uHead + 1 ) % MAX_LOG;
			}
		}
		else {
			m_mts[nIndex].fInverse	= fInverse;
			strcpy( m_mts[nIndex].szText, szBuffer );
		}
	}

	m_AddRemoveLock.Leave();

	InvalidateRect( m_hWnd, NULL, TRUE );
}

void CMyTrace::AddLine( char ch )
{
	char szLine[160];
	memset( szLine, ch, 159 );
	szLine[159]		= '\0';
	Add( 0, FALSE, szLine );
}

u_long CMyTrace::Key( LPCSTR szKey )
{
	char szLwr[256];
	strcpy( szLwr, szKey );
	strlwr( szLwr );

	u_long uKey		= 0, num;

	for( int i	= strlen( szLwr ), j = 0; i > 0 && j < 4; j++, i-- )
	{
		uKey	= uKey << 8;
		num		= szLwr[i-1];
		uKey |= num;
	}
	return uKey;
}

void CMyTrace::Paint( HDC hDC )
{
	static int nYStatusBar	= 14 * ( MAX_LOG - 1 ) + 5;

	HFONT hfontOld	= (HFONT)SelectObject( hDC, m_hfont );
	SetBkColor( hDC, m_cr );

	int cy	= 0;

	m_AddRemoveLock.Enter();

	if( m_uHead <= m_uTail ) {
		for( int i =m_uHead; i < m_uTail; i++, cy += 14 ) {
			CMyTrace::TextOut( hDC, 4, cy, m_mts[i].szText, m_mts[i].fInverse );
		}
	}
	else
	{
		for( int i = m_uHead; i < MAX_LOG; i++, cy += 14 ) {
			CMyTrace::TextOut( hDC, 4, cy, m_mts[i].szText, m_mts[i].fInverse );
		}
		for( i = 0; i < m_uTail; i++, cy += 14 ) {
			CMyTrace::TextOut( hDC, 4, cy, m_mts[i].szText, m_mts[i].fInverse );
		}
	}

	if( m_szStatusBar[0] != '\0' )
	{
		CMyTrace::TextOut( hDC, 0, nYStatusBar-14, "________________________________________________________________________________________________________________________________________________________________", FALSE );
		CMyTrace::TextOut( hDC, 4, nYStatusBar, m_szStatusBar, FALSE );
	}

	m_AddRemoveLock.Leave();

	SelectObject( hDC, hfontOld );
}

void CMyTrace::TextOut( HDC hDC, int x, int y, LPCTSTR lpString, BOOL fInverse )
{
	SetBkMode( hDC, fInverse? OPAQUE : TRANSPARENT );
	SetTextColor( hDC, fInverse? m_crBk : m_cr );

	int cbString	= strlen( lpString );
	::TextOut( hDC, x, y, lpString, cbString );
}

void CMyTrace::GetText( u_long uKey, LPSTR lpszText )
{
	int nIndex	= FindKey( uKey );
	if( nIndex < 0 ) {

	}
	else {
		strcpy( lpszText, m_mts[nIndex].szText );
	}
}

CMyTrace	g_MyTrace;
