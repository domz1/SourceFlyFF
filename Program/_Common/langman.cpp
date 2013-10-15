#include "stdafx.h"
#include "langman.h"

#ifdef __LANG_1013

CLangMan::CLangMan()
{
}

CLangMan::~CLangMan()
{
}

BOOL CLangMan::Load( LPCTSTR lpszFileName )
{
	CScanner	s;
	if( !s.Load( lpszFileName, TRUE ) ) 
		return FALSE;
	DWORD	dwLang	= s.GetNumber();		// dwLang
	DWORD a, r, g, b;
	while( s.tok != FINISHED )
	{
		ASSERT( dwLang < LANG_MAX );
		PLANG_DATA	pData	= &m_aLangData[dwLang];
		
		s.GetToken();
		s.GetToken();	// szPath
		lstrcpy( pData->szPath, s.token );
		s.GetToken();

		s.GetToken();	//	szFileName
		lstrcpy( pData->szFileName, s.token );

		s.GetToken();	//	szTitle
		lstrcpy( pData->szTitle, s.token );

		s.GetToken();
		s.GetToken();	//	szFontFirst
		lstrcpy( pData->font.lfCaption.szFontFirst, s.token );
		s.GetToken();

		s.GetToken();
		s.GetToken();	//	szFontSecond
		lstrcpy( pData->font.lfCaption.szFontSecond, s.token );
		s.GetToken();

		pData->font.lfCaption.nDivTitle	= s.GetNumber();	// nDivTitle
		pData->font.lfCaption.nDivCaption	= s.GetNumber();	// nDivCaption
		pData->font.fdLang.charset	= s.GetNumber();	// charset
		pData->font.fdLang.wCodePage	= s.GetNumber();	// wCodePage

		s.GetToken();
		s.GetToken();
		lstrcpy( pData->font.fdLang.faceNT, s.token );	// faceNT
		s.GetToken();

		s.GetToken();
		s.GetToken();
		lstrcpy( pData->font.fdLang.face9x, s.token );	// face9x
		s.GetToken();

		s.GetToken();
		s.GetToken();	// szFont0
		lstrcpy( pData->font.afi[0].szFont, s.token );
		s.GetToken();

		s.GetToken();
		s.GetToken();	// szFont1
		lstrcpy( pData->font.afi[1].szFont, s.token );
		s.GetToken();

		s.GetToken();
		s.GetToken();	// szFont2
		lstrcpy( pData->font.afi[2].szFont, s.token );
		s.GetToken();

		s.GetToken();
		s.GetToken();	// szFont3
		lstrcpy( pData->font.afi[3].szFont, s.token );
		s.GetToken();

		a	= s.GetNumber();	// dwBgColor3_a
		r	= s.GetNumber();	// dwBgColor3_r
		g	= s.GetNumber();	// dwBgColor3_g
		b	= s.GetNumber();	// dwBgColor3_b
		pData->font.afi[3].dwBgColor	= D3DCOLOR_ARGB( a, r, g, b );
		pData->font.afi[3].nOutLine	= s.GetNumber();	// nOutLine3
		pData->font.afi[3].dwFlags	= s.GetNumber();	// dwFlags3

		s.GetToken();
		s.GetToken();	// szFont4
		lstrcpy( pData->font.afi[4].szFont, s.token );
		s.GetToken();
		a	= s.GetNumber();	// dwBgColor4_a
		r	= s.GetNumber();	// dwBgColor4_r
		g	= s.GetNumber();	// dwBgColor4_g
		b	= s.GetNumber();	// dwBgColor4_b
		pData->font.afi[4].dwBgColor	= D3DCOLOR_ARGB( a, r, g, b );
		pData->font.afi[4].nOutLine	= s.GetNumber();	// nOutLine4
		pData->font.afi[4].dwFlags	= s.GetNumber();	// dwFlags4

		s.GetToken();
		s.GetToken();	// szFont5
		lstrcpy( pData->font.afi[5].szFont, s.token );
		s.GetToken();

		a	= s.GetNumber();	// dwBgColor5_a
		r	= s.GetNumber();	// dwBgColor5_r
		g	= s.GetNumber();	// dwBgColor5_g
		b	= s.GetNumber();	// dwBgColor5_b
		pData->font.afi[5].dwBgColor	= D3DCOLOR_ARGB( a, r, g, b );
		pData->font.afi[5].nOutLine	= s.GetNumber();	// nOutLine5
		pData->font.afi[5].dwFlags	= s.GetNumber();	// dwFlags5

//#ifdef __NO_SUB_LANG
		s.GetNumber(); // dwFontSize
//#endif // __NO_SUB_LANG			

		dwLang	= s.GetNumber();	// dwLang
	}
	return TRUE;
}

PLANG_DATA	CLangMan::GetLangData( DWORD dwLang )
{
	ASSERT( dwLang < LANG_MAX );
	return &m_aLangData[dwLang];
}

CLangMan*	CLangMan::GetInstance( void )
{
	static	CLangMan	sLocal;
	return &sLocal;
}

#endif	// __LANG_1013