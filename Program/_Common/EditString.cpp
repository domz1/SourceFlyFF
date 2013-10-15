#include "stdafx.h"


#define CODE_PAGE(wCodePage) ( wCodePage ? wCodePage : g_codePage )

void CEditString::InitWordAlignment()
{
	m_bWordAlign = TRUE;
}

CEditString::CEditString() : CString( )
{
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	InitializeValue();
#else // __IMPROVE_QUEST_INTERFACE
	m_nWidth = 100;
	m_sizeFont.cx = 12;
	m_sizeFont.cy = 12;
	m_pFont = NULL;
#endif // __IMPROVE_QUEST_INTERFACE
#ifdef __IMPROVE_MAP_SYSTEM
	SetColor( 0xffffffff );//ADEILSON 0xff000000
	SetStyle( 0 );
	SetCodePage( 0 );
#endif // __IMPROVE_MAP_SYSTEM
	InitWordAlignment();
}
CEditString::CEditString( const CString& stringSrc ) : CString( stringSrc )
{
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	InitializeValue();
#endif // __IMPROVE_QUEST_INTERFACE
#ifdef __IMPROVE_MAP_SYSTEM
	SetColor( 0xffffffff );//ADEILSON 0xff000000
	SetStyle( 0 );
	SetCodePage( 0 );
#endif // __IMPROVE_MAP_SYSTEM
	InitWordAlignment();
}

CEditString::CEditString( const CEditString& stringSrc ) : CString( stringSrc )
{
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	m_bWordAlign = stringSrc.m_bWordAlign;
	m_pFont = stringSrc.m_pFont;
	Init( stringSrc.m_nWidth, stringSrc.m_sizeFont );
#endif // __IMPROVE_QUEST_INTERFACE
	m_adwColor.Append( stringSrc.m_adwColor );
	m_abyStyle.Append( stringSrc.m_abyStyle );
	m_awCodePage.Append( stringSrc.m_awCodePage );
	InitWordAlignment();
}

CEditString::CEditString( TCHAR ch, int nRepeat ) : CString( ch, nRepeat )
{
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	InitializeValue();
	SetColor( 0xffffffff );//ADEILSON 0xff000000
#else // __IMPROVE_QUEST_INTERFACE
	SetColor( 0xffffffff );
#endif // __IMPROVE_QUEST_INTERFACE
	SetStyle( 0 );
	SetCodePage( 0 );
	InitWordAlignment();
}

CEditString::CEditString( LPCTSTR lpch, int nLength ) : CString( lpch, nLength )
{
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	InitializeValue();
	SetColor( 0xffffffff );//ADEILSON 0xff000000
#else // __IMPROVE_QUEST_INTERFACE
	SetColor( 0xffffffff );
#endif // __IMPROVE_QUEST_INTERFACE
	SetStyle( 0 );
	SetCodePage( 0 );
	InitWordAlignment();
}

CEditString::CEditString( const unsigned char* psz ) : CString( psz )
{
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	InitializeValue();
	SetColor( 0xffffffff );//ADEILSON 0xff000000
#else // __IMPROVE_QUEST_INTERFACE
	SetColor( 0xffffffff );
#endif // __IMPROVE_QUEST_INTERFACE
	SetStyle( 0 );
	SetCodePage( 0 );
	InitWordAlignment();
}

CEditString::CEditString( LPCWSTR lpsz ) : CString( lpsz )
{
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	InitializeValue();
	SetColor( 0xffffffff );//ADEILSON 0xff000000
#else // __IMPROVE_QUEST_INTERFACE
	SetColor( 0xffffffff );
#endif // __IMPROVE_QUEST_INTERFACE
	SetStyle( 0 );
	SetCodePage( 0 );
	InitWordAlignment();
}

CEditString::CEditString( LPCSTR lpsz ) : CString( lpsz )
{
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	InitializeValue();
	SetColor( 0xffffffff );//ADEILSON 0xff000000
#else // __IMPROVE_QUEST_INTERFACE
	SetColor( 0xffffffff );
#endif // __IMPROVE_QUEST_INTERFACE
	SetStyle( 0 );
	SetCodePage( 0 );
	InitWordAlignment();
}

CEditString::~CEditString()
{
}
void CEditString::SetAlpha( DWORD dwAlpha )
{
	for( int i = 0; i < GetLength(); i++)
	{
		m_adwColor.SetAt( 
			i ,
			( m_adwColor.GetAt( i ) & 0x00ffffff ) | ( dwAlpha << 24 )
		);
	}
}
void CEditString::SetColor( DWORD dwColor )
{
	m_adwColor.SetSize( GetLength() );
	for( int i = 0; i < GetLength(); i++)
		m_adwColor.SetAt( i , dwColor );
}
void CEditString::SetStyle( DWORD dwStyle )
{
	m_abyStyle.SetSize( GetLength() );
	for( int i = 0; i < GetLength(); i++)
		m_abyStyle.SetAt( i , (BYTE)( dwStyle ) );
}
void CEditString::SetCodePage( WORD wCodePage )
{
	m_awCodePage.SetSize( GetLength() );
	for( int i = 0; i < GetLength(); i++)
		m_awCodePage.SetAt( i , CODE_PAGE(wCodePage) );
}
void CEditString::SetColor( int nPos, int nLength, DWORD dwColor )
{
	for( int i = nPos; i < nPos + nLength; i++)
		m_adwColor.SetAt( i, dwColor );
}
void CEditString::SetStyle( int nPos, int nLength, DWORD dwStyle )
{
	for( int i = nPos; i < nPos + nLength; i++)
		m_abyStyle.SetAt( i, (BYTE)( m_abyStyle.GetAt( i ) | dwStyle ) );
}
void CEditString::ClearStyle( int nPos, int nLength, DWORD dwStyle )
{
	for( int i = nPos; i < nPos + nLength; i++)
		m_abyStyle.SetAt( i , (BYTE)( m_abyStyle.GetAt( i ) & ~dwStyle ) );
}

void CEditString::ClearStyle( DWORD dwStyle )
{
	m_abyStyle.SetSize( GetLength() );
	for( int i = 0; i < GetLength(); i++)
		m_abyStyle.SetAt( i , (BYTE)( m_abyStyle.GetAt( i ) & ~dwStyle ) );
}

void CEditString::SetCodePage( int nPos, int nLength, WORD wCodePage )
{
	for( int i = nPos; i < nPos + nLength; i++)
		m_awCodePage.SetAt( i, CODE_PAGE(wCodePage) );
}
void CEditString::Empty( )
{
	CString::Empty();
	m_adwLineOffset.RemoveAll();
	m_adwColor.RemoveAll();
	m_abyStyle.RemoveAll();
	m_awCodePage.RemoveAll();
	m_adwLineOffset.Add( 0 );
	//m_adwColor.Add( 0 );
	//m_abyStyle.Add( 0 );
	//m_adwLineOffset.Add( 0 );
}

void CEditString::GetTextFormat( CString& str )
{
	int nLen = CString::GetLength();

	if( nLen )
	{
		int   nColorMarkCnt = 0;
		int   nSMarkCnt = 0;
		int   nBMarkCnt = 0;
		int   nUMarkCnt = 0;
		DWORD dwColor;
		DWORD dwStyle;
		DWORD dwStyleS;
		DWORD dwStyleB;
		DWORD dwStyleU;
		DWORD dwOldColor = 0xffffffff;//ADEILSON 0xff000000
		DWORD dwOldStyleS = 0;
		DWORD dwOldStyleB = 0;
		DWORD dwOldStyleU = 0;
		str.Empty();

		for( int i=0; i<nLen; i++ )
		{
			dwColor = m_adwColor.GetAt(i);
			dwStyle = m_abyStyle.GetAt(i);

			dwStyleU = dwStyle & ESSTY_UNDERLINE;
			dwStyleB = dwStyle & ESSTY_BOLD;
			dwStyleS = dwStyle & ESSTY_STRIKETHROUGH;

			if( dwColor != dwOldColor )
			{
				if( nColorMarkCnt )
				{
					str += "#nc";
					nColorMarkCnt--;
				}

				if( dwColor != 0xffffffff )//ADEILSON 0xff000000 )
				{
					CString strColor;
					strColor.Format( "#c%x", dwColor );
					
					str += strColor;	
					nColorMarkCnt++;
				}
			}

			if( dwStyleU != dwOldStyleU )
			{
				if( nUMarkCnt )
				{
					str += "#nu";
					nUMarkCnt--;
				}

				if( dwStyleU & ESSTY_UNDERLINE )
				{
					str += "#u";
					nUMarkCnt++;
				}
			}

			if( dwStyleB != dwOldStyleB )
			{
				if( nBMarkCnt )
				{
					str += "#nb";
					nBMarkCnt--;
				}
				
				if( dwStyleB & ESSTY_BOLD )
				{
					str += "#b";
					nBMarkCnt++;
				}
			}
			
			if( dwStyleS != dwOldStyleS )
			{
				if( nSMarkCnt )
				{
					str += "#ns";
					nSMarkCnt--;
				}
				
				if( dwStyleS & ESSTY_STRIKETHROUGH )
				{
					str += "#s";
					nSMarkCnt++;
				}
			}
			
			str += CString::GetAt(i);

			dwOldColor = dwColor;

			dwOldStyleS = dwStyleS;
			dwOldStyleB = dwStyleB;
			dwOldStyleU = dwStyleU;
		}

		if( nColorMarkCnt )
			str += "#nc";

		if( nSMarkCnt )
			str += "#ns";

		if( nBMarkCnt )
			str += "#nb";

		if( nUMarkCnt )
			str += "#nu";	
	}
}

void CEditString::ParsingString( LPCTSTR lpsz, DWORD dwColor, DWORD dwStyle, WORD wCodePage, CString& string, CDWordArray& adwColor, CByteArray& abyStyle, CWordArray& awCodePage, DWORD dwPStyle )
{
	ASSERT( wCodePage );
	// #cffffffff // 칼라 
	// #u // underline
	// #b // bold
	DWORD dwCurColor = dwColor;
	DWORD dwCurStyle = dwStyle;
	DWORD wCurCodePage = wCodePage;
	CString strTemp;
	TCHAR szColor[9];
	int nLen = strlen( lpsz );
	
	for( int i = 0; i < nLen; i++ )
	{
		if( lpsz[ i ] == '#' ) // 인식 코드
		{
			if( ++i >= nLen )
				break;
			switch( lpsz[ i ] )
			{
			case 'c':
				{
					if( ++i >= nLen )
						break;

					if( dwPStyle & PS_USE_MACRO )
					{
						memcpy( szColor, &lpsz[ i ], 8 );
						szColor[ 8 ] = 0;
						DWORDLONG dwlNumber = 0;
						DWORD dwMulCnt = 0;
						CHAR cVal;
						for( int j = 7; j >= 0; j--)
						{
							cVal = szColor[ j ];
							if( cVal >= 'a' )
								cVal = ( cVal - 'a' ) + 10;
							else cVal -= '0';
							dwlNumber |= (DWORDLONG) cVal << dwMulCnt;
							dwMulCnt += 4;
						}
						dwCurColor = (DWORD)dwlNumber;
					}

					i += 7;
				}
				break;
			case 'u':
				if( dwPStyle & PS_USE_MACRO )
					dwCurStyle |= ESSTY_UNDERLINE;
				break;
			case 'b':
				if( dwPStyle & PS_USE_MACRO )
					dwCurStyle |= ESSTY_BOLD;
				break;
			case 's':
				if( dwPStyle & PS_USE_MACRO )
					dwCurStyle |= ESSTY_STRIKETHROUGH;
				break;

			case 'l':
				{
					if(++i >= nLen)
						break;

					if( dwPStyle & PS_USE_MACRO )
					{						
						memcpy( szColor, &lpsz[ i ], 4 );
						szColor[ 4 ] = 0;
						wCurCodePage = CODE_PAGE( atoi( szColor ) );
					}
					i += 3;
				}				
				break;
			case 'n':
				if( ++i >= nLen )
					break;

				if( dwPStyle & PS_USE_MACRO )
				{					
					switch( lpsz[ i ] )
					{
					case 'c':
						dwCurColor = dwColor;
						break;
					case 'b':
						dwCurStyle &= ~ESSTY_BOLD; 
						break;
					case 'u':
						dwCurStyle &= ~ESSTY_UNDERLINE; 
						break;
					case 's':
						dwCurStyle &= ~ESSTY_STRIKETHROUGH; 
						break;
					case 'l':
						wCurCodePage = wCodePage;
						break;
					}
				}
				break;
			default: // 명령코드를 발견 못했을 경우 
				if( dwPStyle & PS_USE_MACRO )
				{
					// #코드를 넣어준다
					strTemp += lpsz[ i - 1 ];
					adwColor.Add( dwCurColor );
					abyStyle.Add( (BYTE)( dwCurStyle ) );			
					awCodePage.Add( (WORD)( wCurCodePage ) );
					// 다음 문자를 넣어준다.
					strTemp += lpsz[ i ];
					adwColor.Add( dwCurColor );
					abyStyle.Add( (BYTE)( dwCurStyle ) );
					awCodePage.Add( (WORD)( wCurCodePage ) );
				}
				break;
			}
		}
		else
		{
			if( lpsz[ i ] == '\\' && lpsz[ i+1 ] == 'n' )
			{
				strTemp += '\n';
				adwColor.Add( dwCurColor );
				abyStyle.Add( (BYTE)( dwCurStyle ) );
				awCodePage.Add( (WORD)( wCurCodePage ) );

				i+=1;
			}
			else
			{
				strTemp += lpsz[ i ];
				int nlength = strTemp.GetLength();
				adwColor.Add( dwCurColor );
				abyStyle.Add( (BYTE)( dwCurStyle ) );
				awCodePage.Add( (WORD)( wCurCodePage ) );
			}
		}
	}
	string += strTemp;
	//*((CString*)this) = string; 
	//m_adwColor.RemoveAll();
	//m_adwColor.Append( adwColor );
}

void CEditString::AddParsingString( LPCTSTR lpsz, DWORD dwColor, DWORD dwStyle, WORD wCodePage, DWORD dwPStyle )
{ 
	int nLine = GetLineCount() - 1;
	ParsingString( lpsz, dwColor, dwStyle, CODE_PAGE(wCodePage),(CString&)*this, m_adwColor, m_abyStyle, m_awCodePage, dwPStyle );
	/*
	int nBegin = GetLength();
	int nEnd = nBegin + _tcslen( lpsz );
	*((CString*)this) += lpsz; 
	for( int i = nBegin; i < nEnd; i++ )
		m_adwColor.SetAtGrow( i, dwColor ); 
	for( i = nBegin; i < nEnd; i++ )
		m_abyStyle.SetAtGrow( i, dwStyle ); 
		*/
	Align( m_pFont, nLine );
}
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
void CEditString::SetParsingString( LPCTSTR lpsz, DWORD dwColor, DWORD dwStyle, WORD wCodePage, DWORD dwPStyle, BOOL bNoInitAlign )
{ 
	Empty();
	ParsingString( lpsz, dwColor, dwStyle, CODE_PAGE(wCodePage), (CString&)*this, m_adwColor, m_abyStyle, m_awCodePage, dwPStyle );
	if( bNoInitAlign == FALSE )
		Align( m_pFont, 0 );
}
#else // __IMPROVE_QUEST_INTERFACE
void CEditString::SetParsingString( LPCTSTR lpsz, DWORD dwColor, DWORD dwStyle, WORD wCodePage, DWORD dwPStyle )
{ 
	Empty();
	ParsingString( lpsz, dwColor, dwStyle, CODE_PAGE(wCodePage), (CString&)*this, m_adwColor, m_abyStyle, m_awCodePage, dwPStyle );
	//*((CString*)this) = lpsz; 
	//SetColor( dwColor );
	//SetStyle( dwStyle );
	//ParsingString( lpsz, dwColor );
	Align( m_pFont, 0 );
}
#endif // __IMPROVE_QUEST_INTERFACE
void CEditString::AddString( LPCTSTR lpsz, DWORD dwColor, DWORD dwStyle, WORD wCodePage )
{ 
	int nLine = GetLineCount() - 1;
	int nBegin = GetLength();
	int nEnd = nBegin + _tcslen( lpsz );
	*((CString*)this) += lpsz; 
	for( int i = nBegin; i < nEnd; i++ )
		m_adwColor.SetAtGrow( i, dwColor ); 
	for( i = nBegin; i < nEnd; i++ )
		m_abyStyle.SetAtGrow( i, (BYTE)( dwStyle ) ); 
	for( i = nBegin; i < nEnd; i++ )
		m_awCodePage.SetAtGrow( i, CODE_PAGE(wCodePage) ); 
	Align( m_pFont, nLine );
}
void CEditString::AddString( char ch, DWORD dwColor, DWORD dwStyle, WORD wCodePage )
{ 
	int nLine = GetLineCount() - 1;
	*((CString*)this) += ch; 
	m_adwColor.Add( dwColor ); 
	m_abyStyle.Add( (BYTE)( dwStyle ) ); 
	m_awCodePage.Add( CODE_PAGE(wCodePage) );
	Align( m_pFont, nLine );
}
void CEditString::SetString( LPCTSTR lpsz, DWORD dwColor, DWORD dwStyle, WORD wCodePage )
{ 
	Empty();
	*((CString*)this) = lpsz; 
	SetColor( dwColor );
	SetStyle( dwStyle );
	SetCodePage( CODE_PAGE(wCodePage) );
	Align( m_pFont, 0 );
	//ParsingString( lpsz, dwColor );
}
void CEditString::SetString( char* lpsz, DWORD dwColor , DWORD dwStyle, WORD wCodePage )
{
	Empty();
	*((CString*)this) = lpsz; 
	SetColor( dwColor );
	SetStyle( dwStyle );
	SetCodePage( CODE_PAGE(wCodePage) );
	Align( m_pFont, 0 );
}
void CEditString::AddEditString( const CEditString& string )
{
	int nLine = GetLineCount() - 1;
	int nBegin = GetLength();
	int nEnd = nBegin + string.GetLength();
	*((CString*)this) += string; 
	m_adwColor.Append( string.m_adwColor );
	m_abyStyle.Append( string.m_abyStyle );
	m_awCodePage.Append( string.m_awCodePage );
	Align( m_pFont, nLine );
}
void CEditString::SetEditString( const CEditString& string )
{
	Empty();
	*((CString*)this) = string; 
	m_adwColor.RemoveAll();
	m_adwColor.Append( string.m_adwColor );
	m_abyStyle.RemoveAll();
	m_abyStyle.Append( string.m_abyStyle );
	m_awCodePage.RemoveAll();
	m_awCodePage.Append( string.m_awCodePage );
	Align( m_pFont, 0 );
}
int CEditString::Insert( int nIndex, TCHAR ch, DWORD dwColor, DWORD dwStyle, WORD wCodePage )
{
	int nLine = GetLineCount() - 1;
	CString::Insert( nIndex, ch );
	m_adwColor.InsertAt( nIndex, dwColor );
	m_abyStyle.InsertAt( nIndex, (BYTE)( dwStyle ) );
	m_awCodePage.InsertAt( nIndex, CODE_PAGE(wCodePage) );
	Align( m_pFont, 0 );//nLine );
	return TRUE;
}
int CEditString::Insert( int nIndex, LPCTSTR pstr, DWORD dwColor, DWORD dwStyle, WORD wCodePage )
{
	int nLine = GetLineCount() - 1;
	CString::Insert( nIndex, pstr );
	m_adwColor.InsertAt( nIndex, dwColor, _tcslen( pstr) );
	m_abyStyle.InsertAt( nIndex, (BYTE)( dwStyle ), _tcslen( pstr) );
	m_awCodePage.InsertAt( nIndex, CODE_PAGE(wCodePage), _tcslen( pstr) );
	Align( m_pFont, 0 );//nLine );
	return TRUE;
}
int CEditString::Delete( int nIndex, int nCount )
{
	int nLine = GetLineCount() - 1;
	CString::Delete( nIndex, nCount );
	m_adwColor.RemoveAt( nIndex, nCount );
	m_abyStyle.RemoveAt( nIndex, nCount );
	m_awCodePage.RemoveAt( nIndex, nCount );
	Align( m_pFont, 0 );//nLine );
	return TRUE;
}
int CEditString::DeleteLine( int nBeginLine, int nCount )
{
	int nMax = nBeginLine + nCount;
	int nBeginOffset, nEndOffset;
	int nLengthCount = 0;
	if( nMax > m_adwLineOffset.GetSize() )
	{
		nCount -= nMax - m_adwLineOffset.GetSize();
		nMax = m_adwLineOffset.GetSize();
	}
	for( int i = nBeginLine; i < nMax; i++ )
	{
		nBeginOffset = GetLineOffset( i );
		nEndOffset = GetLineOffset( i + 1 );
		nLengthCount += nEndOffset - nBeginOffset;
	}
	nBeginOffset = GetLineOffset( nBeginLine );
	CString::Delete( nBeginOffset, nLengthCount );
	m_adwColor.RemoveAt( nBeginOffset, nLengthCount );
	m_abyStyle.RemoveAt( nBeginOffset, nLengthCount );
	m_awCodePage.RemoveAt( nBeginOffset, nLengthCount );
	m_adwLineOffset.RemoveAt( nBeginLine, nCount ); 

	for( int j = nBeginLine; j < m_adwLineOffset.GetSize(); j++ )
	{
		int nNewOffset = m_adwLineOffset.GetAt( j ) - nLengthCount;
		m_adwLineOffset.SetAt( j, nNewOffset );
	}

	//ign( m_pFont, 0 );//nLine );
	return TRUE;
}
TCHAR CEditString::GetAt( int nIndex ) const
{
	return CString::GetAt( nIndex );
}

int CEditString::Find( LPCTSTR pstr, int nStart ) const
{
	return CString::Find( pstr, nStart );
}


void CEditString::SetAt( int nIndex, TCHAR ch, DWORD dwColor, DWORD dwStyle, WORD wCodePage )
{
	int nLine = GetLineCount() - 1;
	CString::SetAt( nIndex, ch );
	m_adwColor.SetAt( nIndex, dwColor );
	m_abyStyle.SetAt( nIndex, (BYTE)( dwStyle ) );
	m_awCodePage.SetAt( nIndex, CODE_PAGE(wCodePage) );
	Align( m_pFont, 0 );//nLine );
}
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
void CEditString::InitializeValue( void )
{
	m_nWidth = 100;
	m_sizeFont.cx = 12;
	m_sizeFont.cy = 12;
	m_pFont = NULL;
}
#endif // __IMPROVE_QUEST_INTERFACE
void CEditString::Init( int nWidth, SIZE sizeFont )
{
	m_nWidth = nWidth;
	m_sizeFont = sizeFont;
}
void CEditString::Init( CD3DFont* pFont, CRect* pRect )
{
	TCHAR str[ 3 ] = _T( "가" );

	m_sizeFont = pFont->GetTextExtent(str);

	BOOL bAlign = FALSE;
	// 사이즈와 폰트 둘중에 하나라도 기존것과 다르다면 정렬을 해주어야한다.
	if( m_nWidth != pRect->Width() || m_pFont != pFont )
		bAlign = TRUE;
	m_nWidth = pRect->Width();
	m_pFont = pFont;
	if( bAlign )
		Align( m_pFont, 0 );//nLine );
}
void CEditString::Adjust( int nWidth, SIZE sizeFont )
{
	m_nWidth = nWidth;
	m_sizeFont = sizeFont;
}
void CEditString::Adjust( CD3DFont* pFont, CRect* pRect )
{
	TCHAR str[ 3 ] = _T( "가" );
	m_sizeFont = pFont->GetTextExtent(str);
	BOOL bAlign = FALSE;
	// 사이즈와 폰트 둘중에 하나라도 기존것과 다르다면 정렬을 해주어야한다.
	if( m_nWidth != pRect->Width() || m_pFont != pFont )
		bAlign = TRUE;
	m_nWidth = pRect->Width();
	m_pFont = pFont;
	if( bAlign )
		Align( m_pFont, 0 );//nLine );
}

void CEditString::Align( CD3DFont* pFont, int nBeginLine )
{
	if( pFont == NULL )
		return;
	int nWidth  = m_nWidth;

	if( nBeginLine < 0 ) nBeginLine = 0;
	CSize sizeString = m_sizeFont;
	int nLength = GetLength();
	if( nLength == 0 || nBeginLine == 0 )
	{
		m_adwLineOffset.RemoveAll();
		if( nLength == 0 )
			return;
	}
	CString strTemp;
	int nOffset = 0;
	if( GetLineCount() )
	{
		nOffset = GetLineOffset( nBeginLine );
		//  라인옵셋 값이 이상 값이라면 
		if( nOffset < 0 || nOffset >= nLength ) 
			Error( "CEditString::Align : pos = %c, LineCount = %d, Length = %d", nOffset, GetLineCount(), nLength );
	}
	char chr = 0; 
	int nLineHeight = sizeString.cy;
	LPCTSTR lpszString = *this;
	CSize sizeTemp;	
	CString strAdd;
	int nSpace = 0;
	int nOffAdd = 0;

	BOOL bWordAlign		= FALSE;
	// GetLanguage()를 constructor에서 호출 할 수 없기에 여기서 한다.
	int nLang	= ::GetLanguage();
	switch( nLang )
	{
#ifdef __NO_SUB_LANG
		case LANG_USA:
#else // __NO_SUB_LANG
		case LANG_USA:
#endif // __NO_SUB_LANG
		case LANG_GER:
		case LANG_SPA:
		case LANG_POR:
		case LANG_FRE:
		case LANG_VTN:
		case LANG_RUS:
			bWordAlign	= m_bWordAlign;
			break;
	}

	do
	{
		m_adwLineOffset.SetAtGrow( nBeginLine++,  nOffset );
		strTemp = _T("");
		sizeTemp = CSize( 0, 0 );
		strAdd.Empty();
		nSpace = 0;
		do 
		{
			nOffAdd = 0;
			chr = GetAt( nOffset + nOffAdd );
			nOffAdd++;
			if( strchr( " +-*^/%=;(),':{}.", chr ) ) // chr == ' ' )
				nSpace = nOffset + 1;
			if( chr == '\n' )
			{
				nOffset += nOffAdd;
				break;
			}
			if( chr == '\r' )
				nOffset += nOffAdd;
			else
			{
				if( nOffset + nOffAdd < nLength && IsDBCSLeadByte( chr ) )
				{
					strAdd += chr;
					chr = GetAt( nOffset + nOffAdd );
					nOffAdd++;
				}
				strAdd += chr;
				CSize sizeAdd;
				pFont->GetTextExtent( strAdd, &sizeAdd );
				sizeTemp.cx += sizeAdd.cx;
				if( strAdd[ 0 ] != ' ' && sizeTemp.cx > nWidth ) 
				{
					if( bWordAlign && nSpace )
						nOffset = nSpace;
					break;
				}		
				else
				{
					strTemp += strAdd;
					nOffset += nOffAdd;
				}
				strAdd.Empty();
			}
		} while( nOffset < nLength && chr != '\0' && chr != '\n' );

		sizeString.cy += nLineHeight;
	} 
	while( chr != '\0' && nOffset < nLength );
}


DWORD CEditString::GetLineCount() 
{
	return (DWORD) m_adwLineOffset.GetSize();//- 1;
}
DWORD CEditString::GetLineOffset( DWORD dwLine ) 
{
	if ( dwLine >= (DWORD)m_adwLineOffset.GetSize() ) 
		return GetLength();
	if ( dwLine < 0 ) return 0;
	return m_adwLineOffset[ dwLine ];
}
void CEditString::GetLine( DWORD dwLine, CHAR* szStr )
{
	CString retval = _T("");

	if( dwLine >= 0 && dwLine < GetLineCount() ) 
	{
		LONG begin = GetLineOffset( dwLine );
		LONG size;
		if( dwLine < GetLineCount() ) 
			size = GetLineOffset( dwLine + 1 ) - begin ;//- 1;
		else 
			size = GetLength() - begin;

#ifdef __VS2003
		PCXSTR pData = GetString();
		memcpy( ( VOID* )szStr, ( VOID* )&pData[ begin ], size );
#else //__VS2003
		memcpy( (VOID*)szStr, (VOID*)&m_pchData[ begin ], size );
#endif // __VS2003

		szStr[ size ] = 0;
	}
}

CString CEditString::GetLine(DWORD dwLine)
{
	CString retval = _T("");

	if( dwLine >= 0 && dwLine < GetLineCount() ) 
	{
		LONG begin = GetLineOffset( dwLine );
		LONG size;
		if( dwLine < GetLineCount() ) 
			size = GetLineOffset( dwLine + 1 ) - begin ;//- 1;
		else 
			size = GetLength() - begin;
		retval = Mid( begin, size ); 
		//retval.Remove('\r');
		//retval.Remove('\n');
	}
	return retval;
}
CEditString CEditString::GetEditString( DWORD dwLine )
{
	CEditString retval = _T("");
	
	if( dwLine >= 0 && dwLine < GetLineCount() ) 
	{
		LONG begin = GetLineOffset( dwLine );
		LONG size;
		if( dwLine < GetLineCount() ) 
			size = GetLineOffset( dwLine + 1 ) - begin;
		else 
			size = GetLength() - begin;
		retval = Mid( begin, size ); 
		for( int i = 0; i < size; i++ )
		{
			retval.m_adwColor[ i ] = m_adwColor[ begin + i ];
			retval.m_abyStyle[ i ] = m_abyStyle[ begin + i ];
			retval.m_awCodePage[ i ] = m_awCodePage[ begin + i ];
		}
	}
	return retval;
}
DWORD CEditString::OffsetToLine(DWORD dwOffset)
{
	CString retval = _T("");

	DWORD dwLineCount = GetLineCount();

	for(DWORD i = 0; i < dwLineCount; i++) 
	{
		DWORD dwBegin = GetLineOffset( i );
		DWORD dwNext = GetLineOffset( i + 1 );

		if(dwOffset >= dwBegin && dwOffset < dwNext)
			return i;
	}
	if( dwOffset == (DWORD)GetLength() )
	{
		if( dwOffset > 0 && GetAt( dwOffset - 1 ) == '\n')
			return dwLineCount;
		return dwLineCount - 1;
	}
	return 0;
}

#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
const CEditString& CEditString::operator=( const CEditString& stringSrc )
{
	m_bWordAlign = stringSrc.m_bWordAlign;
	m_pFont = stringSrc.m_pFont;
	Init( stringSrc.m_nWidth, stringSrc.m_sizeFont );
	SetEditString( stringSrc ); return *this;
}
#endif // __IMPROVE_QUEST_INTERFACE