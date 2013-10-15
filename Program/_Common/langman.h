#ifndef	__LANGMAN_H__
#define	__LANGMAN_H__

#ifdef __LANG_1013

#include "lang.h"

struct LANGFONT
{
	CHAR	szFontFirst[32];
	CHAR	szFontSecond[32];
	CHAR	nDivTitle;
	CHAR	nDivCaption;
};

struct FONT_DATA
{
	int		charset;
	WORD	wCodePage;
	char	faceNT[32];
	char	face9x[32];
};

typedef	struct	_FONT_INFO
{
	TCHAR	szFont[32];
	DWORD  dwColor;
	DWORD  dwColorV19;
	DWORD  dwBgColor;
	DWORD  nOutLine;
	DWORD  dwFlags;
}	FONT_INFO;

typedef	struct	_NEUZFONT
{
	LANGFONT	lfCaption;	// caption
	FONT_DATA	fdLang;		// language
	FONT_INFO	afi[6];	// "gulim9", "gulim8", "gulim13", "Arial Black9", "FontWorld", "gulim20"
}	NEUZFONT;

typedef	struct	_LANG_DATA
{
	char	szPath[MAX_PATH];	// "Korean\\"
	char	szFileName[MAX_PATH];	// "_TWN.inc"
	char	szTitle[100];	// worldserver
	NEUZFONT	font;
	_LANG_DATA()
		{
			szPath[0]	= '\0';
			szFileName[0]	= '\0';
			szTitle[0]	= '\0';
			memset( &font, 0, sizeof(NEUZFONT) );
		}
}	LANG_DATA,	*PLANG_DATA;

class CLangMan
{
private:
	LANG_DATA	m_aLangData[LANG_MAX];
public:
//	Constructions
	CLangMan();
	virtual	~CLangMan();

	BOOL	Load( LPCTSTR	lpszFileName );
	PLANG_DATA	GetLangData( DWORD dwLang );

	static	CLangMan*	GetInstance( void );
};
#endif	// __LANG_1013

#endif	// __LANGMAN_H__