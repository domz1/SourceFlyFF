#ifndef __EDITSTRING_H
#define __EDITSTRING_H 

#include "vutil.h"
inline BOOL IsWhite(TCHAR c)
{
	return (iswspace(c) || c == '\r' || c == '\n' || !iswprint(c)) && !IsDBCSLeadByte(c);
}
inline BOOL IsAlphaNum(TCHAR c)
{
	return iswalnum(c);
}
#define ESSTY_UNDERLINE     1
#define ESSTY_BOLD          2 
#define ESSTY_STRIKETHROUGH 4
#define	ESSTY_SHADOW		8
#define	ESSTY_BLOCK			16

class CD3DFont* pFont;


class CEditString : public CString
{
protected:
	void InitWordAlignment();

public:
	BOOL m_bWordAlign;
	CD3DFont* m_pFont;
	int m_nWidth;
	SIZE m_sizeFont;

	CDWordArray m_adwColor;
	CByteArray  m_abyStyle;
	CDWordArray m_adwLineOffset;
	CWordArray  m_awCodePage;

	CEditString();// : CString( );
	CEditString( const CString& stringSrc );// : CString( stringSrc );
	CEditString( const CEditString& stringSrc );// : CString( stringSrc );
	CEditString( TCHAR ch, int nRepeat = 1 );// : CString( ch, nRepeat );
	CEditString( LPCTSTR lpch, int nLength );// : CString( lpch, nLength );
	CEditString( const unsigned char* psz );// : CString( psz );
	CEditString( LPCWSTR lpsz );// : CString( lpsz );
	CEditString( LPCSTR lpsz );// : CString( lpsz );

	~CEditString();

#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	void InitializeValue( void );
#endif // __IMPROVE_QUEST_INTERFACE
	void Init( int nWidth, SIZE sizeFont );
	void Init( CD3DFont* pFont, CRect* pRect );
	void Adjust( int nWidth, SIZE sizeFont );
	void Adjust( CD3DFont* pFont, CRect* pRect );
	void ParsingString( LPCTSTR lpsz, DWORD dwColor, DWORD dwStyle, WORD wCodePage, CString& string, CDWordArray& adwColor, CByteArray& abyStyle, CWordArray& awCodePage, DWORD dwPStyle = 0x00000001 );
	void SetAlpha( DWORD dwColor );
	void SetColor( DWORD dwColor );
	void SetStyle( DWORD dwStyle );
	void SetCodePage( WORD codePage );
	void SetColor( int nPos, int nLength, DWORD dwColor );
	void SetStyle( int nPos, int nLength, DWORD dwStyle );
	void ClearStyle( int nPos, int nLength, DWORD dwStyle );
		
	void GetTextFormat( CString& str );
	void ClearStyle( DWORD dwStyle );
	void SetCodePage( int nPos, int nLength, WORD wCodePage );
	
	// ref-counted copy from another CString
	const CEditString& operator=( const CString& stringSrc ) { SetString( stringSrc ); return *this; } 
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	const CEditString& operator=( const CEditString& stringSrc );
#else // __IMPROVE_QUEST_INTERFACE
	const CEditString& operator=( const CEditString& stringSrc ) { SetEditString( stringSrc ); return *this; } 
#endif // __IMPROVE_QUEST_INTERFACE
	// set string content to single character
	//const CString& operator=(TCHAR ch) { SetString( ch ); return *this; } 
#ifdef _UNICODE
	const CEditString& operator=( char ch ) { SetString( ch ); return *this; } 
#endif
	// copy string content from ANSI string (converts to TCHAR)
	const CEditString& operator=( LPCSTR lpsz )  { SetString( lpsz ); return *this; } 
	// copy string content from UNICODE string (converts to TCHAR)
	//const CString& operator=( LPCWSTR lpsz ) { SetString( lpsz ); return *this; } 
	// copy string content from unsigned chars
	//const CString& operator=( const unsigned char* psz ) { SetString( psz ); return *this; } 

	// string concatenation

	// concatenate from another CString
	const CEditString& operator+=( const CString& string ) { AddString( string ); return *this; } 
	const CEditString& operator+=( const CEditString& string ) { AddEditString( string ); return *this; } 

	// concatenate a single character
	//const CString& operator+=( TCHAR ch ) { AddString( ch ); return *this; } 
#ifdef _UNICODE
	// concatenate an ANSI character after converting it to TCHAR
	const CString& operator+=( char ch ) { SetString( ch ); return *this; } 
#endif
	// concatenate a UNICODE character after converting it to TCHAR
	const CString& operator+= (LPCTSTR lpsz ) { SetString( lpsz ); return *this; } 
	const CString& operator+= (char* lpsz ) { SetString( lpsz ); return *this; } 

	int Find( LPCTSTR pstr, int nStart ) const;

	int Replace( TCHAR pstr, TCHAR pstr2 ) { return CString::Replace( pstr, pstr2 ); }

	int Insert( int nIndex, TCHAR ch, DWORD dwColor = 0xffffffff/*adeilson 0xff000000*/, DWORD dwStyle = 0x00000000, WORD wCodePage = 0 );
	int Insert( int nIndex, LPCTSTR pstr, DWORD dwColor = 0xffffffff/*adeilson 0xff000000*/, DWORD dwStyle = 0x00000000, WORD wCodePage = 0 );
	int Delete( int nIndex, int nCount = 1 );
	int DeleteLine( int nBeginLine, int nCount  = 1 );
	TCHAR GetAt( int nIndex ) const;
	void SetAt( int nIndex, TCHAR ch, DWORD dwColor = 0xffffffff/*adeilson 0xff000000*/, DWORD dwStyle = 0x00000000, WORD wCodePage = 0 );
	void AddParsingString( LPCTSTR lpsz, DWORD dwColor = 0xffffffff/*adeilson 0xff000000*/, DWORD dwStyle = 0x00000000, WORD wCodePage = 0, DWORD dwPStyle = 0x00000001 );
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	void SetParsingString( LPCTSTR lpsz, DWORD dwColor = 0xffffffff/*adeilson 0xff000000*/, DWORD dwStyle = 0x00000000, WORD wCodePage = 0, DWORD dwPStyle = 0x00000001, BOOL bNoInitAlign = FALSE );
#else // __IMPROVE_QUEST_INTERFACE
	void SetParsingString( LPCTSTR lpsz, DWORD dwColor = 0xffffffff/*adeilson 0xff000000*/, DWORD dwStyle = 0x00000000, WORD wCodePage = 0, DWORD dwPStyle = 0x00000001 );
#endif // __IMPROVE_QUEST_INTERFACE
	void AddString( LPCTSTR lpsz, DWORD dwColor = 0xffffffff/*adeilson 0xff000000*/, DWORD dwStyle = 0x00000000, WORD wCodePage = 0 );
	void AddString( char ch, DWORD dwColor = 0xffffffff/*adeilson 0xff000000*/, DWORD dwStyle = 0x00000000, WORD wCodePage = 0 );
	void SetString( LPCTSTR lpsz, DWORD dwColor = 0xffffffff/*adeilson 0xff000000*/, DWORD dwStyle = 0x00000000, WORD wCodePage = 0 );
	void SetString( char* lpsz, DWORD dwColor = 0xffffffff/*adeilson 0xff000000*/, DWORD dwStyle = 0x00000000, WORD wCodePage = 0 );
	void AddEditString( const CEditString& string );
	void SetEditString( const CEditString& string );
	void Empty( );
	//void Reset( CD3DFont* pFont, CRect* rect, BOOL bHScroll = FALSE, BOOL bVScroll = TRUE);
	void Align( CD3DFont* pFont, int nBeginLine = 0 );
	DWORD GetLineCount();
	DWORD GetLineOffset( DWORD dwLine ) ;
	DWORD OffsetToLine( DWORD dwOffset );
	CString GetLine( DWORD dwLine );
	void GetLine( DWORD dwLine, CHAR* pszStr );
	CEditString GetEditString( DWORD dwLine );
};


#endif
