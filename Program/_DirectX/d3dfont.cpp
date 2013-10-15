//-----------------------------------------------------------------------------
// File: D3DFont.cpp
//
// Desc: Texture-based font class
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#define STRICT
#include "stdafx.h"
#include <stdio.h>
#include <tchar.h> 
#include <D3DX9.h>
#include "D3DFont.h"
#include "D3DUtil.h"
#include "DXUtil.h"
#include "..\_UnhandledException\ExceptionHandler.h"
#ifdef __LANG_1013
#include "langman.h"
#endif	// __LANG_1013

//-----------------------------------------------------------------------------
// Name: CD3DFontAPI()
// Desc: Font class constructor
//-----------------------------------------------------------------------------
CD3DFontAPI::CD3DFontAPI()
{
    m_dwFontHeight         = 0;
    m_dwFontFlags          = 0;
    m_dwSpacing            = 0;
    m_pd3dDevice           = NULL;
    m_pFont       = NULL;
    m_pD3DXSprite = NULL;
	m_nOutLine = 0;
	m_dwColor = 0xffffffff;//ADEILSON 0xff000000
	m_dwBgColor = 0xff000000;
}

CD3DFontAPI::CD3DFontAPI( const TCHAR* strFontName, DWORD dwHeight, DWORD dwFlags )
{
    _tcsncpy( m_strFontName, strFontName, sizeof(m_strFontName) / sizeof(TCHAR) );
    m_strFontName[sizeof(m_strFontName) / sizeof(TCHAR) - 1] = _T('\0');
    m_dwFontHeight         = dwHeight;
    m_dwFontFlags          = dwFlags;
    m_dwSpacing            = 0;
    m_pd3dDevice           = NULL;

//_nFontSize  = 12;
    m_pFont       = NULL;
    //m_pFont2      = NULL;
    //m_pStatsFont  = NULL;
    m_pD3DXSprite = NULL;
    //m_strTextBuffer = NULL;
	
	m_nOutLine = 0;

	m_dwColor = 0xffffffff;//ADEILSON 0xff000000
	m_dwBgColor = 0xff000000;
	
}




//-----------------------------------------------------------------------------
// Name: ~CD3DFontAPI()
// Desc: Font class destructor
//-----------------------------------------------------------------------------
CD3DFontAPI::~CD3DFontAPI()
{
    InvalidateDeviceObjects();
    DeleteDeviceObjects();
}
//-----------------------------------------------------------------------------
// Name: InitDeviceObjects()
// Desc: Initializes device-dependent objects, including the vertex buffer used
//       for rendering text and the texture map which stores the font image.
//-----------------------------------------------------------------------------
HRESULT CD3DFontAPI::InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;

	DWORD dwRed   = ( m_dwBgColor & 0x00ff0000 ) >> ( 16 + 4 );
	DWORD dwGreen = ( m_dwBgColor & 0x0000ff00 ) >> ( 8 + 4 );
	DWORD dwBlue  = ( m_dwBgColor & 0x000000ff ) >> 4;
	m_dwBgColor = ( dwRed << 8 ) | ( dwGreen << 4 ) | dwBlue;
	
	// DWORD color을 WORD color로 변환 
	dwRed   = ( m_dwColor & 0x00ff0000 ) >> ( 16 + 4 );
	dwGreen = ( m_dwColor & 0x0000ff00 ) >> ( 8 + 4 );
	dwBlue  = ( m_dwColor & 0x000000ff ) >> 4;
	m_dwColor = ( dwRed << 8 ) | ( dwGreen << 4 ) | dwBlue;


	HRESULT hr;
    int nHeight;
	
    HDC hDC = GetDC( NULL );
    int nLogPixelsY = GetDeviceCaps(hDC, LOGPIXELSY);
    ReleaseDC( NULL, hDC );
	
	int nH = m_dwFontHeight;

	//CString strError;
	//strError.Format( "CD3DFontAPI::InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice ) : 생성 직전\n" );
	//ADDERRORMSG( strError );

    nHeight = -nH * nLogPixelsY / 72;
    hr = D3DXCreateFont( m_pd3dDevice,          // D3D device
		nHeight,               // Height
		0,                     // Width
		FW_MEDIUM,               // Weight
		0,                     // MipLevels, 0 = autogen mipmaps
		FALSE,                 // Italic
		DEFAULT_CHARSET,       // CharSet
		OUT_DEFAULT_PRECIS,    // OutputPrecision
		DEFAULT_QUALITY,       // Quality
		DEFAULT_PITCH | FF_DONTCARE, // PitchAndFamily
		m_strFontName,              // pFaceName
		&m_pFont);              // ppFont

	//strError.Format( "CD3DFontAPI::InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice ) : 생성 후 m_pFont %p ", m_pFont );
	//ADDERRORMSG( strError );

	/*
    nHeight = -nH * nLogPixelsY / 72;
    if( FAILED( hr = D3DXCreateFont( m_pd3dDevice, nHeight, 0, FW_MEDIUM, 0, FALSE, DEFAULT_CHARSET, 
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, 
		TEXT("MS Sans Serif"), &m_pFont2 ) ) )
        return hr;
	
    nHeight = -nH * nLogPixelsY / 72;
    if( FAILED( hr = D3DXCreateFont( m_pd3dDevice, nHeight, 0, FW_MEDIUM, 0, FALSE, DEFAULT_CHARSET, 
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, 
		TEXT("MS Sans Serif"), &m_pStatsFont ) ) )
        return hr;
	*/
    if( FAILED( hr = D3DXCreateSprite( m_pd3dDevice, &m_pD3DXSprite ) ) )
        return hr;

	//strError.Format( "D3DXCreateSprite : 생성 후 m_pD3DXSprite %p ", m_pD3DXSprite );
	//ADDERRORMSG( strError );

	GetTextExtentPoint32(  m_pFont->GetDC(), "활", 2, &m_sizeBitmap );
	m_dwMaxHeight = m_sizeBitmap.cy;//+ ( m_nOutLine * 2 );

	//strError.Format( "GetTextExtentPoint32 : 이후" );
	//ADDERRORMSG( strError );
	
	return S_OK;
}
//-----------------------------------------------------------------------------
// Name: RestoreDeviceObjects()
// Desc:
//-----------------------------------------------------------------------------
HRESULT CD3DFontAPI::RestoreDeviceObjects()
{
	//CString strError;
	//strError.Format( "CD3DFontAPI::RestoreDeviceObjects 1\n" );
	//ADDERRORMSG( strError );
	
    // Restore the fonts
    //m_pStatsFont->OnResetDevice();
	if( m_pFont )
		m_pFont->OnResetDevice();

	//strError.Format( "CD3DFontAPI::RestoreDeviceObjects 2\n" );
	//ADDERRORMSG( strError );

    //m_pFont2->OnResetDevice();
	if( m_pD3DXSprite )
		m_pD3DXSprite->OnResetDevice();


	
    // Restore the textures
/*    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_DITHERENABLE, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0x80808080);
*/	
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: InvalidateDeviceObjects()
// Desc: Destroys all device-dependent objects
//-----------------------------------------------------------------------------
HRESULT CD3DFontAPI::InvalidateDeviceObjects()
{
	//if( m_pStatsFont )
	//	m_pStatsFont->OnLostDevice();

	//CString strError;
	//strError.Format( "CD3DFontAPI::InvalidateDeviceObjects 1" );
	//ADDERRORMSG( strError );

	if( m_pFont )
		m_pFont->OnLostDevice();

	//strError.Format( "CD3DFontAPI::InvalidateDeviceObjects 2" );
	//ADDERRORMSG( strError );

	//if( m_pFont2 )
	//	m_pFont2->OnLostDevice();
	
	if( m_pD3DXSprite )
		m_pD3DXSprite->OnLostDevice();
	
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: DeleteDeviceObjects()
// Desc: Destroys all device-dependent objects
//-----------------------------------------------------------------------------
HRESULT CD3DFontAPI::DeleteDeviceObjects()
{
	SAFE_RELEASE( m_pFont );
    //SAFE_RELEASE( m_pFont2 );
    //SAFE_RELEASE( m_pStatsFont );
    SAFE_RELEASE( m_pD3DXSprite );
	
	//SAFE_DELETE_ARRAY( m_strTextBuffer );
	
    return S_OK;
}

HRESULT CD3DFontAPI::GetTextExtent( const TCHAR* strText, SIZE* pSize )
{
	SIZE size; 
	
	int nOffset  = 0;
	int nOffNext = 0;
	CString str = strText;
	vector<CString> OffsetIndex;
	OffsetIndex.clear();
	
	nOffNext = str.Find( '\n', nOffset );
	
	if( nOffNext == -1 )
		OffsetIndex.push_back( str );
	else
	{
		while( nOffNext != -1 )
		{
			nOffNext = str.Find( '\n', nOffset );
			
			if( nOffNext == -1 )
				OffsetIndex.push_back( str.Mid( nOffset, str.GetLength() -nOffset ) );
			else
				OffsetIndex.push_back( str.Mid( nOffset, nOffNext-nOffset ) );
			
			nOffset = nOffNext+1;
		}
	}
	
	int nIndex  = 0;
	int nMaxLen = 0;
	for( int i=0; i<(int)( OffsetIndex.size() ); i++ )
	{
		if( OffsetIndex[i].GetLength() > nMaxLen )
		{
			nIndex  = i;
			nMaxLen = OffsetIndex[i].GetLength();
		}
	}
	::GetTextExtentPoint32( m_pFont->GetDC(), (LPCTSTR)OffsetIndex[ nIndex ].GetBuffer( 0 ), strlen( OffsetIndex[ nIndex ].GetBuffer( 0 ) ), &size );
	
	size.cy = OffsetIndex.size() * m_dwMaxHeight;

	*pSize = size;

	return S_OK;
}
//-----------------------------------------------------------------------------
// Name: DrawText()
// Desc: Draws 2D text. Note that sx and sy are in pixels
//-----------------------------------------------------------------------------
HRESULT CD3DFontAPI::DrawText( FLOAT sx, FLOAT sy, DWORD dwColor,
                            const TCHAR* strText, DWORD dwFlags )
{
	return DrawText( sx, sy, 1.0f, 1.0f, dwColor, strText, dwFlags );
}
HRESULT CD3DFontAPI::DrawText( FLOAT sx, FLOAT sy, FLOAT fXScale, FLOAT fYScale, DWORD dwColor,
                            const TCHAR* strText, DWORD dwFlags )
{
	if( m_dwFlags & D3DFONT_FILTERED )
    {
        m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
        m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
    }
	RECT rc;
	FLOAT a, r, g, b;

/*	
	a = ((dwColor>>12) & 0x0f) / 16.f;
	r = ((dwColor>>8) & 0x0f) / 16.f;
	g = ((dwColor>>4) & 0x0f)  / 16.f;
	b = ((dwColor) & 0x0f)     / 16.f;
*/
	// 위에 것이 ARGB값이 적용이 안되어 아래 것으로 바꿨음.
	a = ((dwColor>>24) & 0xff) / 255.f;
	r = ((dwColor>>16) & 0xff) / 255.f;
	g = ((dwColor>>8) & 0xff)  / 255.f;
	b = ((dwColor) & 0xff)     / 255.f;


	D3DXMATRIX mScale, m_World;
	D3DXMatrixIdentity( &m_World );
	D3DXMatrixScaling( &mScale, fXScale, fYScale, 0.0f );
	m_pD3DXSprite->SetTransform(&mScale);
	m_pD3DXSprite->Begin( D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE );

	sx /= fXScale;
	sy /= fYScale;
	SetRect( &rc, (int)( sx ), (int)( sy ), 0, 0 );        

	if( m_nOutLine )
	{
		FLOAT a_b, r_b, g_b, b_b;

		a_b = ((m_dwBgColor>>12) & 0xf) / 16.f;
		r_b = ((m_dwBgColor>>8) & 0xf) / 16.f;
		g_b = ((m_dwBgColor>>4) & 0xf)  / 16.f;
		b_b = ((m_dwBgColor) & 0xf)     / 16.f;

		SetRect( &rc, (int)( sx+m_nOutLine ), (int)( sy+m_nOutLine ), 0, 0 );        
		m_pFont->DrawText( m_pD3DXSprite, strText, -1, &rc, DT_NOCLIP, D3DXCOLOR( r_b,g_b,b_b,a ) ); 
		SetRect( &rc, (int)( sx-m_nOutLine ), (int)( sy-m_nOutLine ), 0, 0 );        
		m_pFont->DrawText( m_pD3DXSprite, strText, -1, &rc, DT_NOCLIP, D3DXCOLOR( r_b,g_b,b_b,a ) ); 
		SetRect( &rc, (int)( sx-m_nOutLine ), (int)( sy+m_nOutLine ), 0, 0 );        
		m_pFont->DrawText( m_pD3DXSprite, strText, -1, &rc, DT_NOCLIP, D3DXCOLOR( r_b,g_b,b_b,a ) ); 
		SetRect( &rc, (int)( sx+m_nOutLine ), (int)( sy-m_nOutLine ), 0, 0 );        
		m_pFont->DrawText( m_pD3DXSprite, strText, -1, &rc, DT_NOCLIP, D3DXCOLOR( r_b,g_b,b_b,a ) ); 
		SetRect( &rc, (int)( sx+m_nOutLine ), (int)( sy+0 ), 0, 0 );        
		m_pFont->DrawText( m_pD3DXSprite, strText, -1, &rc, DT_NOCLIP, D3DXCOLOR( r_b,g_b,b_b,a ) ); 
		SetRect( &rc, (int)( sx-m_nOutLine ), (int)( sy+0 ), 0, 0 );        
		m_pFont->DrawText( m_pD3DXSprite, strText, -1, &rc, DT_NOCLIP, D3DXCOLOR( r_b,g_b,b_b,a ) ); 
		SetRect( &rc, (int)( sx+0 ), (int)( sy-m_nOutLine ), 0, 0 );        
		m_pFont->DrawText( m_pD3DXSprite, strText, -1, &rc, DT_NOCLIP, D3DXCOLOR( r_b,g_b,b_b,a ) ); 
		SetRect( &rc, (int)( sx-0 ), (int)( sy+m_nOutLine ), 0, 0 );        
		m_pFont->DrawText( m_pD3DXSprite, strText, -1, &rc, DT_NOCLIP, D3DXCOLOR( r_b,g_b,b_b,a ) ); 

		SetRect( &rc, (int)( sx ), (int)( sy ), 0, 0 );        
		m_pFont->DrawText( m_pD3DXSprite, strText, -1, &rc, DT_NOCLIP, D3DXCOLOR( r,g,b,a ) ); 
	}
	else
	{
		m_pFont->DrawText( m_pD3DXSprite, strText, -1, &rc, DT_NOCLIP, dwColor ); 
	}
	m_pD3DXSprite->End();
	m_pD3DXSprite->SetTransform(&m_World );

	return S_OK;
}

#if defined( __CLIENT) 
HRESULT CD3DFontAPI::DrawText( FLOAT sx, FLOAT sy, DWORD dwColor,
						   CEditString& strEditString, int nPos, int nLine, int nLineSpace, DWORD dwFlags )
{
	return DrawText( sx, sy, 1.0f, 1.0f, dwColor, strEditString, nPos, nLine, nLineSpace, dwFlags );
}
//////////////
HRESULT CD3DFontAPI::DrawText( FLOAT sx, FLOAT sy, FLOAT fXScale, FLOAT fYScale, DWORD dwColor,
                            CEditString& strEditString, int nPos, int nLines, int nLineSpace, DWORD dwFlags )
{
	if( m_dwFlags & D3DFONT_FILTERED )
    {
        m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
        m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
    }
	
	RECT rc;
	FLOAT _fsx = sx;
	FLOAT _fsy = sy;
	

	FLOAT a, r, g, b;
	a = ((dwColor>>24) & 0xff) / 255.f;
	r = ((dwColor>>16) & 0xff) / 255.f;
	g = ((dwColor>>8) & 0xff)  / 255.f;
	b = ((dwColor) & 0xff)     / 255.f;
	
	
	if( nLines == 0 )
		nLines = strEditString.GetLineCount();
	int nMax = nPos + nLines;
	if( nMax > (int)( strEditString.GetLineCount() ) )
		nMax = strEditString.GetLineCount();

	sx -= m_dwSpacing;
	FLOAT fStartX = sx;
	sx = _fsx;
	sy = _fsy;
	
	
	for( int i = nPos; i < nMax; i++ )
	{
		CString string = strEditString.GetLine( i );
		DWORD dwOffset = strEditString.GetLineOffset( i );
		DWORD dwColor = strEditString.m_adwColor[ dwOffset ];
		
		SetRect( &rc, (int)( sx ), (int)( sy ), 0, 0 );  
		m_pFont->DrawText( NULL, string.GetBuffer(0), -1, &rc, DT_NOCLIP, dwColor );//D3DXCOLOR( r, g, b, 1.0f ) ); 
		
		sx = fStartX;
		sy += m_dwMaxHeight + nLineSpace;
		
	}

	return S_OK;
}
#endif // not COLA

#ifndef __LANG_1013
struct FONT_DATA {
	int			charset;
	WORD		wCodePage;
	const char*	faceNT;
	const char*	face9x;
} g_fontData[] = {
	{ HANGUL_CHARSET,		949,	"Gulim",		"굴림" },
	{ ANSI_CHARSET,			1252,	"Arial",	"Arial" },
//	{ ANSI_CHARSET,			1252,	"Arial Black",	"Arial Black" },
	{ SHIFTJIS_CHARSET,		932,	"MS Gothic",	"굃굍 굊긕긘긞긏" },
	{ CHINESEBIG5_CHARSET,	950,	"MingLiU",		"꾄ⁿ톱" },     //대만어
//	{ GB2312_CHARSET,		936,	"SimSun",		"芥竟" },       //중국어
	{ THAI_CHARSET,			874,	"Microsoft Sans Serif",		"Microsoft Sans Serif" },
	{ CHINESEBIG5_CHARSET,	950,	"MingLiU",		"꾄ⁿ톱" },     //대만어
	///	{ THAI_CHARSET,			874,	"Tahoma",		"Tahoma" },
};
#endif	// __LANG_1013


void    SetCodePage( int nLang )
{
#ifdef __LANG_1013
//	g_codePage	= CLangMan::GetInstance()->GetLangData( nLang )->font.fdLang.wCodePage;
#else	// __LANG_1013
	g_codePage = g_fontData[nLang].wCodePage;
#endif	// __LANG_1013
}



int CALLBACK EnumFontFamExProc(CONST LOGFONT* plogFont, CONST TEXTMETRIC* textMetric, DWORD dwWord, LPARAM lParam)
{
	return stricmp((const char*)lParam, plogFont->lfFaceName);
}

int GetCharsetFromCodePage(WORD codePage)
{
	switch( codePage )
	{
	case 932:
		return SHIFTJIS_CHARSET;
	case 949:
		return HANGUL_CHARSET;
	case 936:
		return GB2312_CHARSET;
	case 950:
		return CHINESEBIG5_CHARSET;
	case 1253:
		return GREEK_CHARSET;
	case 1254:
		return TURKISH_CHARSET;
	case 1255:
		return HEBREW_CHARSET;
	case 1256:
		return ARABIC_CHARSET;
	case 1257:
		return BALTIC_CHARSET;
	case 874:
		return THAI_CHARSET;
	case 1250:
		return EASTEUROPE_CHARSET;
	case 1258:
		return VIETNAMESE_CHARSET;
	case 1251:
		return RUSSIAN_CHARSET;
	default:
		return ANSI_CHARSET;
	}
//	1252
}


//-----------------------------------------------------------------------------
// Custom vertex types for rendering text
//-----------------------------------------------------------------------------
#define MAX_NUM_VERTICES 100*6

struct FONT2DVERTEX { D3DXVECTOR4 p;   DWORD color;     FLOAT tu, tv; };

#define D3DFVF_FONT2DVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)

inline FONT2DVERTEX InitFont2DVertex( const D3DXVECTOR4& p, D3DCOLOR color,
                                      FLOAT tu, FLOAT tv )
{
    FONT2DVERTEX v;   v.p = p;   v.color = color;   v.tu = tu;   v.tv = tv;
    return v;
}

CD3DFont::CD3DFont( const char* strFontName, DWORD dwHeight, DWORD dwFlags )// : CD3DFontAPI( strFontName, dwHeight,  dwFlags ) 
{
    _tcsncpy( m_strFontName, strFontName, sizeof(m_strFontName) / sizeof(char) );
    m_strFontName[sizeof(m_strFontName) / sizeof(char) - 1] = _T('\0');
    m_dwFontHeight         = dwHeight;
    m_dwFontFlags          = dwFlags;
	m_nOutLine             = 0;
    m_pd3dDevice           = NULL;
    m_pVB                  = NULL;

    m_pStateBlockSaved     = NULL;
    m_pStateBlockDrawText  = NULL;

	m_nCurX = 0;
	m_nCurY = 0;
	m_pCurTexture = NULL;

	m_hFont = NULL;
	m_hFontOld = NULL;
	m_hDC = NULL;
	m_hbmBitmap = NULL;
	m_hbmOld = NULL;
	m_pBitmapBits = NULL;
	m_dwColor = 0xffffffff;//ADEILSON 0xff000000
	m_dwBgColor = 0xff000000;
	m_wCodePage = g_codePage;
}

//-----------------------------------------------------------------------------
// Name: ~CD3DFont()
// Desc: Font class destructor
//-----------------------------------------------------------------------------
CD3DFont::~CD3DFont()
{
    InvalidateDeviceObjects();
    DeleteDeviceObjects();
}


//-----------------------------------------------------------------------------
// Name: CreateGDIFont
// Desc: Create a font based on the current state of related member variables
//       and return the handle (or null on error)
//-----------------------------------------------------------------------------
HRESULT CD3DFont::CreateGDIFont( HDC hDC, HFONT* pFont )
{
    // Create a font.  By specifying ANTIALIASED_QUALITY, we might get an
    // antialiased font, but this is not guaranteed.
    INT nHeight    = -MulDiv( m_dwFontHeight, 
                              (INT)(GetDeviceCaps(hDC, LOGPIXELSY) * m_fTextScale), 
                              72 );
    DWORD dwBold   = (m_dwFontFlags & D3DFONT_BOLD)   ? FW_BOLD : FW_NORMAL;
    DWORD dwItalic = (m_dwFontFlags & D3DFONT_ITALIC) ? TRUE    : FALSE;
    *pFont         = CreateFont( nHeight, 0, 0, 0, dwBold, dwItalic,
                                 FALSE, FALSE, GetCharsetFromCodePage(g_codePage), OUT_DEFAULT_PRECIS,
                                 CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
                                 VARIABLE_PITCH, m_strFontName );

    if( *pFont == NULL )
        return E_FAIL;

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: InitDeviceObjects()
// Desc: Initializes device-dependent objects, including the vertex buffer used
//       for rendering text and the texture map which stores the font image.
//-----------------------------------------------------------------------------
HRESULT CD3DFont::InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{
    HRESULT hr = S_OK;

    // Keep a local copy of the device
    m_pd3dDevice = pd3dDevice;

    // Assume we will draw fonts into texture without scaling unless the
    // required texture size is found to be larger than the device max
    m_fTextScale  = 1.0f; 

    m_hDC = CreateCompatibleDC( NULL );
    SetMapMode( m_hDC, MM_TEXT );

    hr = CreateGDIFont( m_hDC, &m_hFont );
    if( FAILED(hr) )
        return hr;

    m_hFontOld = (HFONT) SelectObject( m_hDC, m_hFont );

    // Calculate the dimensions for the smallest power-of-two texture which
    // can hold all the printable characters
    m_dwTexWidth = m_dwTexHeight = 512;

    if( FAILED(hr) )
        return hr;
    
    // If requested texture is too big, use a smaller texture and smaller font,
    // and scale up when rendering.
    D3DCAPS9 d3dCaps;
    m_pd3dDevice->GetDeviceCaps( &d3dCaps );
    if( m_dwTexWidth > d3dCaps.MaxTextureWidth )
	{
		m_dwTexWidth = m_dwTexHeight = d3dCaps.MaxTextureWidth;
	}

	if( ::GetLanguage() == LANG_USA )
		GetTextExtentPoint32(  m_hDC, "W", 2, &m_sizeBitmap );
	else if(::GetLanguage() == LANG_VTN)
		GetTextExtentPoint32(  m_hDC, "활", 2, &m_sizeBitmap );
	else
		GetTextExtentPoint32(  m_hDC, "가", 2, &m_sizeBitmap );

	m_sizeBitmap.cy +=2;

	m_dwMaxHeight = m_sizeBitmap.cy + ( m_nOutLine * 2 );

    BITMAPINFO bmi;
    ZeroMemory( &bmi.bmiHeader, sizeof(BITMAPINFOHEADER) );
    bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth       =  (int)m_sizeBitmap.cx;
    bmi.bmiHeader.biHeight      = -(int)m_sizeBitmap.cy;
    bmi.bmiHeader.biPlanes      = 1;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biBitCount    = 32;

    // Create a bitmap for the font
    m_hbmBitmap = CreateDIBSection( m_hDC, &bmi, DIB_RGB_COLORS,
                                  (void**)&m_pBitmapBits, NULL, 0 );

    m_hbmOld = SelectObject( m_hDC, m_hbmBitmap );
    
    // Set text properties
    SetTextColor( m_hDC, RGB(255,000,000) );
    SetBkColor(   m_hDC, 0x00000000 );
    SetTextAlign( m_hDC, TA_TOP );

	// DWORD color을 WORD color로 변환 
	DWORD dwRed   = ( m_dwBgColor & 0x00ff0000 ) >> ( 16 + 4 );
	DWORD dwGreen = ( m_dwBgColor & 0x0000ff00 ) >> ( 8 + 4 );
	DWORD dwBlue  = ( m_dwBgColor & 0x000000ff ) >> 4;
	m_dwBgColor = ( dwRed << 8 ) | ( dwGreen << 4 ) | dwBlue;
	
	// DWORD color을 WORD color로 변환 
	dwRed   = ( m_dwColor & 0x00ff0000 ) >> ( 16 + 4 );
	dwGreen = ( m_dwColor & 0x0000ff00 ) >> ( 8 + 4 );
	dwBlue  = ( m_dwColor & 0x000000ff ) >> 4;
	m_dwColor = ( dwRed << 8 ) | ( dwGreen << 4 ) | dwBlue;
	
	// 텍스춰 한장 만들어 놓기 
	m_pCurTexture = CreateTexture();

	m_apTexture.Add( m_pCurTexture  );

	return hr;
}

LPDIRECT3DTEXTURE9 CD3DFont::CreateTexture()
{
	LPDIRECT3DTEXTURE9 lpTexture;
	m_pd3dDevice->CreateTexture( m_dwTexWidth, m_dwTexHeight, 1, 0, D3DFMT_A4R4G4B4, D3DPOOL_MANAGED, &lpTexture, NULL );
	D3DLOCKED_RECT d3dlr;
	lpTexture->LockRect( 0, &d3dlr, NULL, 0 );
	WORD* pDstRow = (WORD*)d3dlr.pBits;
	int nLength = m_dwTexWidth * m_dwTexHeight;
	for( int i = 0; i < nLength; i++ )
		*pDstRow++ = (WORD)m_dwBgColor;			//gmpbigsun : 이미 변환이 되어있음 DWORD->WORD (::InitDeviceObjects)
	lpTexture->UnlockRect( 0 );
	return lpTexture;
}



//-----------------------------------------------------------------------------
// Name: RestoreDeviceObjects()
// Desc:
//-----------------------------------------------------------------------------
HRESULT CD3DFont::RestoreDeviceObjects()
{
    HRESULT hr;

    // Create vertex buffer for the letters
    int vertexSize = sizeof(FONT2DVERTEX);
    if( FAILED( hr = m_pd3dDevice->CreateVertexBuffer( MAX_NUM_VERTICES * vertexSize,
                                                       D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 0,
                                                       D3DPOOL_DEFAULT, &m_pVB, NULL ) ) )
    {
        return hr;
    }

    bool bSupportsAlphaBlend = true;
    LPDIRECT3D9 pd3d9 = NULL;
    if( SUCCEEDED( m_pd3dDevice->GetDirect3D( &pd3d9 ) ) )
    {
        D3DCAPS9 Caps;
        D3DDISPLAYMODE Mode;
        LPDIRECT3DSURFACE9 pSurf = NULL;
        D3DSURFACE_DESC Desc;
        m_pd3dDevice->GetDeviceCaps( &Caps );
        m_pd3dDevice->GetDisplayMode( 0, &Mode );
        if( SUCCEEDED( m_pd3dDevice->GetRenderTarget( 0, &pSurf ) ) )
        {
            pSurf->GetDesc( &Desc );
            if( FAILED( pd3d9->CheckDeviceFormat( Caps.AdapterOrdinal, Caps.DeviceType, Mode.Format,
                D3DUSAGE_RENDERTARGET | D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING, D3DRTYPE_SURFACE, 
                Desc.Format ) ) )
            {
                bSupportsAlphaBlend = false;
            }
            SAFE_RELEASE( pSurf );
        }
        SAFE_RELEASE( pd3d9 );
    }

    // Create the state blocks for rendering text
    for( UINT which=0; which<2; which++ )
    {
        m_pd3dDevice->BeginStateBlock();

        if ( D3DFONT_ZENABLE & m_dwFontFlags )
            m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
        else
            m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );

        if( bSupportsAlphaBlend )
        {
            m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
            m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,   D3DBLEND_SRCALPHA );
            m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND,  D3DBLEND_INVSRCALPHA );
        }
        else
        {
            m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
        }
        m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  TRUE );
        m_pd3dDevice->SetRenderState( D3DRS_ALPHAREF,         0x08 );
        m_pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC,  D3DCMP_GREATEREQUAL );
        m_pd3dDevice->SetRenderState( D3DRS_FILLMODE,   D3DFILL_SOLID );
        m_pd3dDevice->SetRenderState( D3DRS_CULLMODE,   D3DCULL_CCW );
        m_pd3dDevice->SetRenderState( D3DRS_STENCILENABLE,    FALSE );
        m_pd3dDevice->SetRenderState( D3DRS_CLIPPING,         TRUE );
        m_pd3dDevice->SetRenderState( D3DRS_CLIPPLANEENABLE,  FALSE );
        m_pd3dDevice->SetRenderState( D3DRS_VERTEXBLEND,      D3DVBF_DISABLE );
        m_pd3dDevice->SetRenderState( D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE );
        m_pd3dDevice->SetRenderState( D3DRS_FOGENABLE,        FALSE );
        m_pd3dDevice->SetRenderState( D3DRS_COLORWRITEENABLE,
            D3DCOLORWRITEENABLE_RED  | D3DCOLORWRITEENABLE_GREEN |
            D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
        m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
        m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
        m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
        m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
        m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_NONE );

        if( which==0 )
            m_pd3dDevice->EndStateBlock( &m_pStateBlockSaved );
        else
            m_pd3dDevice->EndStateBlock( &m_pStateBlockDrawText );
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: InvalidateDeviceObjects()
// Desc: Destroys all device-dependent objects
//-----------------------------------------------------------------------------
HRESULT CD3DFont::InvalidateDeviceObjects()
{
    SAFE_RELEASE( m_pVB );
    SAFE_RELEASE( m_pStateBlockSaved );
    SAFE_RELEASE( m_pStateBlockDrawText );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: DeleteDeviceObjects()
// Desc: Destroys all device-dependent objects
//-----------------------------------------------------------------------------
HRESULT CD3DFont::DeleteDeviceObjects()
{
	if( m_hDC )
	{
		SelectObject( m_hDC, m_hbmOld );
		SelectObject( m_hDC, m_hFontOld );
		DeleteObject( m_hbmBitmap );
		DeleteObject( m_hFont );
		DeleteDC( m_hDC );
		m_hDC = NULL;
	}

	for(font_map_iter iter = m_fontMap.begin(); iter != m_fontMap.end(); ++iter)
		DeleteObject(iter->second);
	m_fontMap.clear();
	
	LPDIRECT3DTEXTURE9 pTex;
	for( int i = 0 ; i < m_apTexture.GetSize(); i++ ) 
	{
		pTex = (LPDIRECT3DTEXTURE9)m_apTexture.GetAt( i );
		SAFE_RELEASE( pTex );
	}
    m_pd3dDevice = NULL;
	m_apTexture.RemoveAll();

    return S_OK;
}
// 테이블에 0xf는 넣지 말것.f는 체크 범위에서 벗어나는 값이다.
// f는 기본 폰트만이 사용할 수 있다!!
static DWORD g_adwOutLine1[ 9 ] = 
{
	0x0, 0xe, 0x0,
	0xe, 0xe, 0xe,
	0x0, 0xe, 0x0
};
static DWORD g_adwOutLine2[ 25 ] = 
{
	0x3, 0x7, 0x7, 0x7, 0x3,
	0x7, 0xa, 0xe, 0xa, 0x7,
	0x7, 0xe, 0xe, 0xe, 0x7,
	0x7, 0xa, 0xe, 0xa, 0x7,
	0x3, 0x7, 0x7, 0x7, 0x3
};
static DWORD g_adwOutLine3[ 49 ] = 
{
	0x0, 0x4, 0x4, 0x4, 0x4, 0x4, 0x0,
	0x4, 0x7, 0xe, 0xe, 0xe, 0x7, 0x4,
	0x4, 0xe, 0xe, 0xe, 0xe, 0xe, 0x4,
	0x4, 0xe, 0xe, 0xe, 0xe, 0xe, 0x4,
	0x4, 0xa, 0xe, 0xe, 0xa, 0xe, 0x4,
	0x4, 0x7, 0xe, 0xe, 0xe, 0x7, 0x4,
	0x0, 0x4, 0x4, 0x4, 0x4, 0x4, 0x0
};
static DWORD g_adwOutLine4[ 82 ] = 
{
	0x0, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x0,
	0x4, 0x7, 0xe, 0xe, 0xe, 0xe, 0xe, 0x7, 0x4,
	0x4, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0x4,
	0x4, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0x4,
	0x4, 0xa, 0xe, 0xe, 0xa, 0xe, 0xa, 0xe, 0x4,
	0x4, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0x4,
	0x4, 0xa, 0xe, 0xe, 0xa, 0xe, 0xa, 0xe, 0x4,
	0x4, 0x7, 0xe, 0xe, 0xe, 0xe, 0xe, 0x7, 0x4,
	0x0, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x0
};

void CD3DFont::MakeOutLine( int nWidth, WORD* pDst16 )
{
	int nOffset = 0;
	int nDstOffset;
	WORD* pDst16_,* pDstCur;
	int nCurX = m_nCurX + m_nOutLine ;
	int nCurY = m_nCurY + m_nOutLine ;
	nDstOffset = ( nCurY * nWidth + nCurX );
	pDst16_ = &pDst16[ nDstOffset ] ;//d3dlr.Pitch;

	DWORD* apdwOutLine;
	
	switch( m_nOutLine )
	{
	case 1: apdwOutLine = g_adwOutLine1; break;
	case 2: apdwOutLine = g_adwOutLine2; break;
	case 3: apdwOutLine = g_adwOutLine3; break;
	case 4: apdwOutLine = g_adwOutLine4; break;
	}
	int nOutLineLength = m_nOutLine * 2 + 1;

	for( int y = 0; y < m_sizeBitmap.cy; y++ )
	{
		pDstCur = pDst16_;
		for( int x = 0; x < m_sizeBitmap.cx; x++ )
		{
			if( ( *pDstCur & 0xf000 ) == 0xf000)
			{
				for( int y2 = (int)y + nCurY - m_nOutLine, y3 = 0; y2 <= (int)( ( y + nCurY + m_nOutLine ) ); y2++, y3++ )
				{
					for( int x2 = (int)x + nCurX - m_nOutLine, x3 = 0; x2 <= (int)( ( x + nCurX + m_nOutLine ) ); x2++, x3++ )
					{
						if( x2 >= 0 && x2 < nWidth && y2 >= 0 && y2 < nWidth )
						{
							
							nOffset = ( y2 * nWidth + x2 );
							if( ( pDst16[ nOffset] & 0xf000 ) != 0xf000 )
							{
								DWORD dwColor = apdwOutLine[ y3 * nOutLineLength + x3 ] << 12;
								if( dwColor )
								{
									if( (DWORD)( ( pDst16[ nOffset] & 0xf000 ) ) < dwColor )///&& ( pDst16[ nOffset] & 0xf000 ) != 0xf000 )
										pDst16[ nOffset] = (WORD)( ( pDst16[ nOffset] & 0x0fff ) | dwColor );
								}
							}
						}
					}
				}
			}
			pDstCur++;
		}
        pDst16_ += nWidth;
	}
}
void CD3DFont::CopyToTexture( LPDIRECT3DTEXTURE9 pTex, CSize size, DWORD dwBgColor )
{
    D3DLOCKED_RECT d3dlr;

    HRESULT hr = pTex->LockRect( 0, &d3dlr, NULL, 0 );
	if( hr == D3DERR_INVALIDCALL )
	{
		ADDERRORMSG( "CD3Dfont::CopyToTexture Lock실패" );
		return;
	}
    BYTE* pDstRow = (BYTE*)d3dlr.pBits;
    WORD* pDst16;
    DWORD dwColor; // 4-bit measure of pixel intensity

	int nOffset = 0;
	int nDstOffset;
	BYTE* pTemp  = pDstRow;
	nDstOffset = ( ( m_nCurY + m_nOutLine ) * d3dlr.Pitch + ( ( m_nCurX + m_nOutLine ) * 2 ) );
	pDstRow = &pDstRow[ nDstOffset ] ;//d3dlr.Pitch;

	// DWORD color을 WORD color로 변환 
	DWORD dwRed   = ( dwBgColor & 0x00ff0000 ) >> ( 16 + 4 );
	DWORD dwGreen = ( dwBgColor & 0x0000ff00 ) >> ( 8 + 4 );
	DWORD dwBlue  = ( dwBgColor & 0x000000ff ) >> 4;
	dwBgColor = ( dwRed << 8 ) | ( dwGreen << 4 ) | dwBlue;

	if( size.cx > m_sizeBitmap.cx ) size.cx = m_sizeBitmap.cx;
	if( size.cy > m_sizeBitmap.cy ) size.cy = m_sizeBitmap.cy;

	int nNext = m_sizeBitmap.cx - size.cx;

	for( int y = 0, y2 = m_nCurY + m_nOutLine; y < size.cy; y++, y2++ )
	{
        pDst16 = (WORD*)pDstRow;//[ nDstOffset ] ;
		for( int x = 0, x2 = m_nCurX + m_nOutLine; x < size.cx; x++, nOffset++, x2++ )
		{
			if( (int)pDst16 - (int)pTemp < ( 1024 * 512 ) )
			{
				dwColor = m_pBitmapBits[ nOffset ] & 0x00ffffff;
				if( dwColor )
				{
					*pDst16++ = (WORD) m_dwColor | 0xf000;
				}
				else
				{
					pDst16++;
				}
			}
		}
        pDstRow += d3dlr.Pitch;
		nOffset += nNext;
	}
	if( m_nOutLine )
	{
		MakeOutLine( d3dlr.Pitch / 2, (WORD*) d3dlr.pBits );
	}

	pTex->UnlockRect(0);
}
/*----------------------------------------------------------------------------*/ 
FONTTEX* CD3DFont::GetFontTex(const char* begin, const char* end, WORD wCodePage )
{
	int len = end-begin;
	// 좆같은 태국어
	if( wCodePage == 874)
	{
		if( (BYTE)*end == 0xd3 )
		{
			len++;
		}
	}
	GLYPHINFO glyphInfo;

	memcpy(glyphInfo.text, begin, len);
	memset(glyphInfo.text+len, 0, sizeof(glyphInfo.text)-len);
	glyphInfo.wCodePage = wCodePage;

	font_tex_map_iter iter = m_fontTex.find(glyphInfo);

	if(iter != m_fontTex.end()) {
		return &iter->second;
	} else {
		iter = m_fontTex.insert(font_tex_map::value_type(glyphInfo, FONTTEX())).first;
	}

	LPFONTTEX lpFontTex = &iter->second;

	lpFontTex->pTexture = NULL;

    // Set the starting point for the drawing
	if( (DWORD)( m_nCurX + m_sizeBitmap.cx + ( m_nOutLine * 2 ) ) > m_dwTexWidth )
	{
		m_nCurX = 0;
		m_nCurY += m_dwMaxHeight;
		// 폰트가 텍스춰를 초과했다. 다음 텍스춰 준비 
		if( m_nCurY + m_dwMaxHeight > m_dwTexWidth ) 
		{
			m_nCurY = 0;
			m_pCurTexture = CreateTexture();

			if( m_pCurTexture == NULL )
			{
				return lpFontTex;
			}
			m_apTexture.Add( m_pCurTexture );
		}
	}
	
	// Clear Font Buffer
	ZeroMemory( m_pBitmapBits, m_sizeBitmap.cy * m_sizeBitmap.cx * sizeof( DWORD ) ); 

	// Font를 선택한다
	SelectObject(m_hDC, GetFont(wCodePage));
	// 현재 선택된 텍스춰에 쓴다.
    SIZE size;
	GetTextExtentPoint32( m_hDC, begin, end-begin, &size );
	ExtTextOut( m_hDC, 0, 0, ETO_OPAQUE, NULL, begin, len, NULL );
	size.cy = min( size.cy,  (int)( m_dwMaxHeight ) );
	CopyToTexture( m_pCurTexture, size,0x008080ff );

	
	// 기록한다.
	lpFontTex->pTexture = m_pCurTexture;
	lpFontTex->tx1 = ( ( FLOAT )( m_nCurX ) ) / m_dwTexWidth ;
	lpFontTex->ty1 = ( ( FLOAT )( m_nCurY ) ) / m_dwTexHeight;
	lpFontTex->tx2 = ( ( FLOAT )( m_nCurX + size.cx + ( m_nOutLine * 2 ) ) ) / m_dwTexWidth ;
	lpFontTex->ty2 = ( ( FLOAT )( m_nCurY + size.cy + ( m_nOutLine * 2 ) ) ) / m_dwTexHeight;
	m_nCurX += size.cx + ( m_nOutLine * 2 );

	return lpFontTex;
}
HFONT CD3DFont::GetFont(WORD wCodePage)
{
	if(wCodePage != g_codePage)
	{
		font_map_iter iter = m_fontMap.find(wCodePage);

		if(iter != m_fontMap.end())
		{
			return iter->second;
		}
		else
		{
#ifdef __LANG_1013
			PLANG_DATA	pLangData	= CLangMan::GetInstance()->GetLangData( ::GetLanguage() );
			const char* fontFace = m_strFontName;

			LOGFONT logFont;
			memset(&logFont, 0, sizeof(logFont));
			logFont.lfCharSet = pLangData->font.fdLang.charset;

			if(EnumFontFamiliesEx(GetDC(NULL), &logFont, (FONTENUMPROC)EnumFontFamExProc, (LONG)pLangData->font.fdLang.faceNT, 0) == 0) 
			{
				fontFace = pLangData->font.fdLang.faceNT;
			} 
			else if(EnumFontFamiliesEx(GetDC(NULL), &logFont, (FONTENUMPROC)EnumFontFamExProc, (LONG)pLangData->font.fdLang.face9x, 0) == 0) 
			{
				fontFace = pLangData->font.fdLang.face9x;
			}

			INT nHeight    = -MulDiv( m_dwFontHeight, 
									  (INT)(GetDeviceCaps(m_hDC, LOGPIXELSY) * m_fTextScale), 
									  72 );
			DWORD dwBold   = (m_dwFontFlags & D3DFONT_BOLD)   ? FW_BOLD : FW_NORMAL;
			DWORD dwItalic = (m_dwFontFlags & D3DFONT_ITALIC) ? TRUE    : FALSE;

			HFONT hFont = CreateFont( nHeight, 0, 0, 0, dwBold, dwItalic,
							 FALSE, FALSE, pLangData->font.fdLang.charset, OUT_DEFAULT_PRECIS,
							 CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
							 VARIABLE_PITCH, fontFace );

			m_fontMap.insert(font_map::value_type(wCodePage, hFont));

				return hFont;
#else	// __LANG_1013
			for( int i = 0; i < sizeof(g_fontData)/sizeof(FONT_DATA); ++i )
			{
				if(g_fontData[i].wCodePage == wCodePage)
				{
					const char* fontFace = m_strFontName;

					LOGFONT logFont;
					memset(&logFont, 0, sizeof(logFont));
					logFont.lfCharSet = g_fontData[i].charset;

					if(EnumFontFamiliesEx(GetDC(NULL), &logFont, (FONTENUMPROC)EnumFontFamExProc, (LONG)g_fontData[i].faceNT, 0) == 0) 
					{
						fontFace = g_fontData[i].faceNT;
					} 
					else if(EnumFontFamiliesEx(GetDC(NULL), &logFont, (FONTENUMPROC)EnumFontFamExProc, (LONG)g_fontData[i].face9x, 0) == 0) 
					{
						fontFace = g_fontData[i].face9x;
					}

					INT nHeight    = -MulDiv( m_dwFontHeight, 
											  (INT)(GetDeviceCaps(m_hDC, LOGPIXELSY) * m_fTextScale), 
											  72 );
					DWORD dwBold   = (m_dwFontFlags & D3DFONT_BOLD)   ? FW_BOLD : FW_NORMAL;
					DWORD dwItalic = (m_dwFontFlags & D3DFONT_ITALIC) ? TRUE    : FALSE;

					HFONT hFont = CreateFont( nHeight, 0, 0, 0, dwBold, dwItalic,
									 FALSE, FALSE, g_fontData[i].charset, OUT_DEFAULT_PRECIS,
									 CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
									 VARIABLE_PITCH, fontFace );

					m_fontMap.insert(font_map::value_type(wCodePage, hFont));

					return hFont;
				}
			}
#endif	// __LANG_1013
		}
	}

	return m_hFont;
}
//-----------------------------------------------------------------------------
// Name: DrawText()
// Desc: Draws 2D text. Note that sx and sy are in pixels
//-----------------------------------------------------------------------------
HRESULT CD3DFont::DrawText( FLOAT sx, FLOAT sy, DWORD dwColor,
                            const char* strText, DWORD dwFlags, WORD wCodePage )
{
	return DrawText( sx, sy, 1.0f, 1.0f, dwColor, strText, dwFlags, wCodePage );
}
HRESULT CD3DFont::DrawText( FLOAT sx, FLOAT sy, FLOAT fXScale, FLOAT fYScale, DWORD dwColor,
                            const char* strText, DWORD dwFlags, WORD wCodePage )
{
    if( m_pd3dDevice == NULL )
        return E_FAIL;

	wCodePage = wCodePage ? wCodePage : m_wCodePage;

    // Setup renderstate
    m_pStateBlockSaved->Capture();
    m_pStateBlockDrawText->Apply();
    m_pd3dDevice->SetFVF( D3DFVF_FONT2DVERTEX );
	m_pd3dDevice->SetVertexShader( NULL );
    m_pd3dDevice->SetStreamSource( 0, m_pVB, 0, sizeof(FONT2DVERTEX) );

    // Set filter states
    if( dwFlags & D3DFONT_FILTERED )
    {
        m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
        m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
    }
	LPDIRECT3DTEXTURE9 pTexture = NULL;
    // Adjust for character spacing
    FLOAT fStartX = sx;
	FLOAT _fsx = sx;
	FLOAT _fsy = sy;

    // Fill vertex buffer
    FONT2DVERTEX* pVertices = NULL;
	LPFONTTEX lpFontTex = NULL;

	for( int t = 0; t < m_apTexture.GetSize(); t++ )
	{
	    m_dwNumTriangles = 0;
		pTexture = (LPDIRECT3DTEXTURE9)m_apTexture.GetAt( t );
		m_pd3dDevice->SetTexture( 0, pTexture );
		pVertices = NULL;
		m_pVB->Lock( 0, 0, (void**)&pVertices, D3DLOCK_DISCARD );
		sx = _fsx;
		sy = _fsy;
		const char* iter = strText;

		while( *iter )
		{
			if( *iter == _T('\n') )
			{
			  sx = fStartX;
			  sy += m_dwMaxHeight;
			}

			const char* next = CharNextEx( iter, wCodePage );

			lpFontTex = GetFontTex( iter, next, wCodePage );

			FLOAT tx1 = lpFontTex->tx1; 
			FLOAT ty1 = lpFontTex->ty1; 
			FLOAT tx2 = lpFontTex->tx2; 
			FLOAT ty2 = lpFontTex->ty2; 

			FLOAT w = (tx2-tx1) * m_dwTexWidth  * fXScale;
			FLOAT h = (ty2-ty1) * m_dwTexHeight * fYScale;
			
			if( *(BYTE*)iter > ' ' ) 
			{
				if( pTexture == lpFontTex->pTexture )
				{
					*pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+0-0.5f,sy+h-0.5f,0.9f,1.0f), dwColor, tx1, ty2 );
					*pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+0-0.5f,sy+0-0.5f,0.9f,1.0f), dwColor, tx1, ty1 );
					*pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+w-0.5f,sy+h-0.5f,0.9f,1.0f), dwColor, tx2, ty2 );
					*pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+w-0.5f,sy+0-0.5f,0.9f,1.0f), dwColor, tx2, ty1 );
					*pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+w-0.5f,sy+h-0.5f,0.9f,1.0f), dwColor, tx2, ty2 );
					*pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+0-0.5f,sy+0-0.5f,0.9f,1.0f), dwColor, tx1, ty1 );
					m_dwNumTriangles += 2;

					if( m_dwNumTriangles * 3 > ( MAX_NUM_VERTICES - 6 ) )
					{
						// Unlock, render, and relock the vertex buffer
						m_pVB->Unlock();
						m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, m_dwNumTriangles );
						pVertices = NULL;
						m_pVB->Lock( 0, 0, (void**)&pVertices, D3DLOCK_DISCARD );
						m_dwNumTriangles = 0L;
					}
				}
			}

			sx += w - ( m_nOutLine * 2 );
			iter = next;
		}
		// Unlock and render the vertex buffer
		m_pVB->Unlock();
		if( m_dwNumTriangles > 0 )
			m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, m_dwNumTriangles );
	}
    // Restore the modified renderstates
    m_pStateBlockSaved->Apply();

    return S_OK;
}

#if defined( __CLIENT)
						
HRESULT CD3DFont::DrawText( FLOAT sx, FLOAT sy, DWORD dwColor,
						   CEditString& strEditString, int nPos, int nLine, int nLineSpace, DWORD dwFlags )
{
	return DrawText( sx, sy, 1.0f, 1.0f, dwColor, strEditString, nPos, nLine, nLineSpace, dwFlags );
}

HRESULT CD3DFont::DrawText( FLOAT sx, FLOAT sy, FLOAT fXScale, FLOAT fYScale, DWORD dwColor,
                            CEditString& strEditString, int nPos, int nLines, int nLineSpace, DWORD dwFlags )
{
    if( m_pd3dDevice == NULL )
        return E_FAIL;

    // Setup renderstate
    m_pStateBlockSaved->Capture();
    m_pStateBlockDrawText->Apply();
    m_pd3dDevice->SetFVF( D3DFVF_FONT2DVERTEX );
	m_pd3dDevice->SetVertexShader( NULL );
    m_pd3dDevice->SetStreamSource( 0, m_pVB, 0, sizeof(FONT2DVERTEX) );

    // Set filter states
    if( dwFlags & D3DFONT_FILTERED )
    {
        m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
        m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
    }
	LPDIRECT3DTEXTURE9 pTexture = NULL;
    // Adjust for character spacing
    FLOAT fStartX = sx;
	FLOAT _fsx = sx;
	FLOAT _fsy = sy;

    // Fill vertex buffer
    FONT2DVERTEX* pVertices = NULL;
	LPFONTTEX lpFontTex = NULL;
	
	if( nLines == 0 )
		nLines = strEditString.GetLineCount();
	int nMax = nPos + nLines;
	if( nMax > (int)( strEditString.GetLineCount() ) )
		nMax = strEditString.GetLineCount();

	for( int t = 0; t < m_apTexture.GetSize(); t++ )
	{
	    m_dwNumTriangles = 0;
		pTexture = (LPDIRECT3DTEXTURE9)m_apTexture.GetAt( t );
		m_pd3dDevice->SetTexture( 0, pTexture );
		pVertices = NULL;
		m_pVB->Lock( 0, 0, (void**)&pVertices, D3DLOCK_DISCARD );
		FLOAT fStartX = sx;
		sx = _fsx;
		sy = _fsy;
		//for( int i = nPos; i < nPos + nLine; i++ )
		for( int i = nPos; i < nMax; i++ )
		{
			CString string = strEditString.GetLine( i );
			DWORD dwOffset = strEditString.GetLineOffset( i );
			int nLength = string.GetLength();

			const char* begin = string;
			const char* end = begin + nLength;
			const char* iter = begin;

			int nLang	= GetLanguage();

			while(*iter)
			{
				DWORD dwColor = strEditString.m_adwColor[ dwOffset + (iter - begin) ];
				WORD wCodePage = strEditString.m_awCodePage[ dwOffset + (iter - begin) ];
				if( nLang == LANG_THA )
					wCodePage	= m_wCodePage;

				const char* next = CharNextEx( iter, wCodePage );

				lpFontTex = GetFontTex(iter, next, wCodePage);

				FLOAT tx1 = lpFontTex->tx1; 
				FLOAT ty1 = lpFontTex->ty1; 
				FLOAT tx2 = lpFontTex->tx2; 
				FLOAT ty2 = lpFontTex->ty2; 
				
				FLOAT w = (tx2-tx1) * m_dwTexWidth  * fXScale;
				FLOAT h = (ty2-ty1) * m_dwTexHeight * fYScale;

				if( *(BYTE*)iter > ' ' )
				{
					if( pTexture == lpFontTex->pTexture )
					{
						*pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+0-0.5f,sy+h-0.5f,0.9f,1.0f), dwColor, tx1, ty2 );
						*pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+0-0.5f,sy+0-0.5f,0.9f,1.0f), dwColor, tx1, ty1 );
						*pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+w-0.5f,sy+h-0.5f,0.9f,1.0f), dwColor, tx2, ty2 );
						*pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+w-0.5f,sy+0-0.5f,0.9f,1.0f), dwColor, tx2, ty1 );
						*pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+w-0.5f,sy+h-0.5f,0.9f,1.0f), dwColor, tx2, ty2 );
						*pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+0-0.5f,sy+0-0.5f,0.9f,1.0f), dwColor, tx1, ty1 );
						m_dwNumTriangles += 2;
						
						if( m_dwNumTriangles * 3 > ( MAX_NUM_VERTICES - 6 ) )
						{
							// Unlock, render, and relock the vertex buffer
							m_pVB->Unlock();
							m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, m_dwNumTriangles );
							pVertices = NULL;
							m_pVB->Lock( 0, 0, (void**)&pVertices, D3DLOCK_DISCARD );
							m_dwNumTriangles = 0L;
						}
					}
				}				
				sx += w - ( m_nOutLine * 2 );
				iter = next;
			}
			sx = fStartX;
			sy += m_dwMaxHeight + nLineSpace;
		}
		// Unlock and render the vertex buffer
		m_pVB->Unlock();
		if( m_dwNumTriangles > 0 )
			m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, m_dwNumTriangles );
	}
    // Restore the modified renderstates
    m_pStateBlockSaved->Apply();

    return S_OK;
}
#endif
//-----------------------------------------------------------------------------
// Name: GetTextExtent()
// Desc: Get the dimensions of a text string
//-----------------------------------------------------------------------------
#if defined( __CLIENT)
HRESULT CD3DFont::GetTextExtent_EditString( CEditString& editString, SIZE* pSize, int nLineBegin, int nLineCount, WORD wCodePage )
{
	if( NULL == pSize )
        return E_FAIL;

    FLOAT fRowWidth  = 0.0f;
    FLOAT fRowHeight = (FLOAT)( m_dwMaxHeight );//( m_fTexCoords[ 0 ][ 3 ] - m_fTexCoords[ 0 ][ 1 ] ) * m_dwTexHeight;
    FLOAT fWidth     = 0.0f;
    FLOAT fHeight    = 0.0f;//fRowHeight;
	LPFONTTEX lpFontTex = NULL;
	wCodePage = wCodePage ? wCodePage : m_wCodePage;

	CHAR szBuff[ 1024 ];
	const char* iter;

	if( nLineCount == -1 )
		nLineCount = editString.GetLineCount();
	if( nLineBegin >= (int)( editString.GetLineCount() ) )
		nLineBegin = editString.GetLineCount() - 1;
	if( nLineBegin + nLineCount > (int)( editString.GetLineCount() ) )
		nLineCount = editString.GetLineCount() - nLineBegin;

	for( int i = nLineBegin; i < nLineBegin + nLineCount; i++ )
	{
		editString.GetLine( i, szBuff );
		iter = szBuff;
		fRowWidth = 0.0f;
		while( *iter )
		{
			/*
			if( *iter == _T('\n') )
			{
				fRowWidth = 0.0f;
				fHeight  += fRowHeight;
			}
			*/

			const char* next = CharNextEx( iter, wCodePage );

			lpFontTex = GetFontTex(iter, next, wCodePage );

			FLOAT tx1 = lpFontTex->tx1;
			FLOAT tx2 = lpFontTex->tx2;

			fRowWidth += ( ( tx2 - tx1 ) * m_dwTexWidth ) - ( m_nOutLine * 2 );

			if( fRowWidth > fWidth )
				fWidth = fRowWidth;

			iter = next;
		}		
		fHeight += fRowHeight;
	}
/*
	const char* iter = strText;

    while( *iter )
    {
        if( *iter == _T('\n') )
        {
            fRowWidth = 0.0f;
            fHeight  += fRowHeight;
        }

		const char* next = CharNextEx( iter, wCodePage );

		lpFontTex = GetFontTex(iter, next, wCodePage );

        FLOAT tx1 = lpFontTex->tx1;
        FLOAT tx2 = lpFontTex->tx2;

        fRowWidth += ( ( tx2 - tx1 ) * m_dwTexWidth ) - ( m_nOutLine * 2 );

        if( fRowWidth > fWidth )
            fWidth = fRowWidth;

		iter = next;
    }
*/
    pSize->cx = (int)fWidth;
    pSize->cy = (int)fHeight;
    return S_OK;
}
#endif
HRESULT CD3DFont::GetTextExtent( const char* strText, SIZE* pSize, WORD wCodePage )
{
	if( NULL == strText || NULL == pSize )
        return E_FAIL;
	/*
	int nLen = strlen( strText );
	if( nLen == 2 )
	{
		int a;
		a++;
	}
	if( (BYTE)strText[0] == 0xcd &&  (BYTE)strText[1] == 0xe9 )
	{
		int b;
		b++;

	}
	*/

    FLOAT fRowWidth  = 0.0f;
    FLOAT fRowHeight = (FLOAT)( m_dwMaxHeight );//( m_fTexCoords[ 0 ][ 3 ] - m_fTexCoords[ 0 ][ 1 ] ) * m_dwTexHeight;
    FLOAT fWidth     = 0.0f;
    FLOAT fHeight    = fRowHeight;
	LPFONTTEX lpFontTex = NULL;
	wCodePage = wCodePage ? wCodePage : m_wCodePage;

	const char* iter = strText;

    while( *iter )
    {
        if( *iter == _T('\n') )
        {
            fRowWidth = 0.0f;
            fHeight  += fRowHeight;
        }

		const char* next = CharNextEx( iter, wCodePage );

		lpFontTex = GetFontTex(iter, next, wCodePage );

        FLOAT tx1 = lpFontTex->tx1;
        FLOAT tx2 = lpFontTex->tx2;

        fRowWidth += ( ( tx2 - tx1 ) * m_dwTexWidth ) - ( m_nOutLine * 2 );

        if( fRowWidth > fWidth )
            fWidth = fRowWidth;

		iter = next;
    }

    pSize->cx = (int)fWidth;
    pSize->cy = (int)fHeight;
    return S_OK;
}
/*----------------------------------------------------------------------------*/ 
/*----------------------------------------------------------------------------*/ 
/*----------------------------------------------------------------------------*/ 
/*----------------------------------------------------------------------------*/ 
/*----------------------------------------------------------------------------*/ 

CD3DFontAPIVTN::CD3DFontAPIVTN(const TCHAR* strFontName, DWORD dwHeight, DWORD dwFlags)
{
	m_pD3DFont = new CD3DFont(strFontName, dwHeight, dwFlags);
}

CD3DFontAPIVTN::~CD3DFontAPIVTN()
{
	if(m_pD3DFont != NULL)
		SAFE_DELETE(m_pD3DFont);
}

#if defined( __CLIENT) 
HRESULT CD3DFontAPIVTN::DrawText( FLOAT x, FLOAT y, DWORD dwColor, CEditString& editString, int nPos, int nLine, int nLineSpace, DWORD dwFlags )
{
	if(m_pD3DFont != NULL)
		return m_pD3DFont->DrawText(x, y, dwColor, editString, nPos, nLine, nLineSpace, dwFlags);

	return S_FALSE;
}

HRESULT CD3DFontAPIVTN::DrawText( FLOAT x, FLOAT y, FLOAT fXScale, FLOAT fYScale, DWORD dwColor, CEditString& editString, int nPos, int nLine, int nLineSpace, DWORD dwFlags )
{
	if(m_pD3DFont != NULL)
		return m_pD3DFont->DrawText(x, y, fXScale, fYScale, dwColor, editString, nPos, nLine, nLineSpace, dwFlags);

	return S_FALSE;
}
#endif

HRESULT CD3DFontAPIVTN::DrawText( FLOAT x, FLOAT y, DWORD dwColor, const TCHAR* strText, DWORD dwFlags )
{
	if(m_pD3DFont != NULL)
		return m_pD3DFont->DrawText(x, y, dwColor, strText, dwFlags, g_codePage);

	return S_FALSE;
}

HRESULT CD3DFontAPIVTN::DrawText( FLOAT x, FLOAT y, FLOAT fXScale, FLOAT fYScale, DWORD dwColor, const TCHAR* strText, DWORD dwFlags )
{
	if(m_pD3DFont != NULL)
	{
		FLOAT a, r, g, b;
		a = ((dwColor>>24) & 0xff) / 255.f;
		r = ((dwColor>>16) & 0xff) / 255.f;
		g = ((dwColor>>8) & 0xff)  / 255.f;
		b = ((dwColor) & 0xff)     / 255.f;

		if( m_nOutLine )
		{
			FLOAT a_b, r_b, g_b, b_b;

			a_b = ((m_dwBgColor>>12) & 0xf) / 16.f;
			r_b = ((m_dwBgColor>>8) & 0xf) / 16.f;
			g_b = ((m_dwBgColor>>4) & 0xf)  / 16.f;
			b_b = ((m_dwBgColor) & 0xf)     / 16.f;

			m_pD3DFont->DrawText(x+m_nOutLine, y+m_nOutLine, fXScale, fYScale, D3DXCOLOR( r_b,g_b,b_b,a ), strText, dwFlags, g_codePage);
			m_pD3DFont->DrawText(x-m_nOutLine, y-m_nOutLine, fXScale, fYScale, D3DXCOLOR( r_b,g_b,b_b,a ), strText, dwFlags, g_codePage);
			m_pD3DFont->DrawText(x-m_nOutLine, y+m_nOutLine, fXScale, fYScale, D3DXCOLOR( r_b,g_b,b_b,a ), strText, dwFlags, g_codePage);
			m_pD3DFont->DrawText(x+m_nOutLine, y-m_nOutLine, fXScale, fYScale, D3DXCOLOR( r_b,g_b,b_b,a ), strText, dwFlags, g_codePage);
			m_pD3DFont->DrawText(x+m_nOutLine, y, fXScale, fYScale, D3DXCOLOR( r_b,g_b,b_b,a ), strText, dwFlags, g_codePage);
			m_pD3DFont->DrawText(x-m_nOutLine, y, fXScale, fYScale, D3DXCOLOR( r_b,g_b,b_b,a ), strText, dwFlags, g_codePage);
			m_pD3DFont->DrawText(x, y-m_nOutLine, fXScale, fYScale, D3DXCOLOR( r_b,g_b,b_b,a ), strText, dwFlags, g_codePage);
			m_pD3DFont->DrawText(x, y+m_nOutLine, fXScale, fYScale, D3DXCOLOR( r_b,g_b,b_b,a ), strText, dwFlags, g_codePage);
			m_pD3DFont->DrawText(x, y, fXScale, fYScale, D3DXCOLOR( r,g,b,a ), strText, dwFlags, g_codePage);
		}
		else
		{
			m_pD3DFont->DrawText(x, y, fXScale, fYScale, D3DXCOLOR( r,g,b,a ), strText, dwFlags, g_codePage);
		}

		return S_OK;
	}

	return S_FALSE;
}

HRESULT CD3DFontAPIVTN::GetTextExtent( const TCHAR* strText, SIZE* pSize )
{
	if(m_pD3DFont != NULL)
		return m_pD3DFont->GetTextExtent(strText, pSize);

	return S_FALSE;
}

HRESULT CD3DFontAPIVTN::InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{
	if(m_pD3DFont != NULL)
	{
		m_pD3DFont->m_dwBgColor = m_dwBgColor;
		m_pD3DFont->m_dwColor = m_dwColor;

		DWORD dwRed   = ( m_dwBgColor & 0x00ff0000 ) >> ( 16 + 4 );
		DWORD dwGreen = ( m_dwBgColor & 0x0000ff00 ) >> ( 8 + 4 );
		DWORD dwBlue  = ( m_dwBgColor & 0x000000ff ) >> 4;
		m_dwBgColor = ( dwRed << 8 ) | ( dwGreen << 4 ) | dwBlue;
		
		// DWORD color을 WORD color로 변환 
		dwRed   = ( m_dwColor & 0x00ff0000 ) >> ( 16 + 4 );
		dwGreen = ( m_dwColor & 0x0000ff00 ) >> ( 8 + 4 );
		dwBlue  = ( m_dwColor & 0x000000ff ) >> 4;
		m_dwColor = ( dwRed << 8 ) | ( dwGreen << 4 ) | dwBlue;

		m_pD3DFont->InitDeviceObjects(pd3dDevice);
	}

	return S_FALSE;
}

HRESULT CD3DFontAPIVTN::RestoreDeviceObjects()
{
	if(m_pD3DFont != NULL)
		return m_pD3DFont->RestoreDeviceObjects();

	return S_FALSE;
}

HRESULT CD3DFontAPIVTN::InvalidateDeviceObjects()
{
	if(m_pD3DFont != NULL)
		return m_pD3DFont->InvalidateDeviceObjects();

	return S_FALSE;
}

HRESULT CD3DFontAPIVTN::DeleteDeviceObjects()
{
	if(m_pD3DFont != NULL)
		return m_pD3DFont->DeleteDeviceObjects();

	return S_FALSE;
}

DWORD CD3DFontAPIVTN::GetMaxHeight()
{
	if(m_pD3DFont != NULL)
		return m_pD3DFont->GetMaxHeight();

	return S_FALSE;	
}

