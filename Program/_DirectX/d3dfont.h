//-----------------------------------------------------------------------------
// File: D3DFont.h
//
// Desc: Texture-based font class
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef D3DFONT_H
#define D3DFONT_H
#include <tchar.h>
#include <D3D9.h>
#include <d3dx9core.h>

// Font creation flags
#define D3DFONT_BOLD        0x0001
#define D3DFONT_ITALIC      0x0002
#define D3DFONT_ZENABLE     0x0004

// Font rendering flags
#define D3DFONT_CENTERED_X  0x0001
#define D3DFONT_CENTERED_Y  0x0002
#define D3DFONT_TWOSIDED    0x0004
#define D3DFONT_FILTERED    0x0008


// Hangul High : 0xb0 ~ 0xc8
// Hangul Low  : 0xa0 ~ 0xff

//#define EXTRA_HANGUL     0xa4
//#define MIN_HANGUL_HIGH  0xb0
//#define MIN_HANGUL_LOW   0xa0
//#define MAX_HANGUL_HIGH  0xc9
//#define MAX_HANGUL_LOW   0x100
//#define NUM_HANGUL_HIGH  (MAX_HANGUL_HIGH - MIN_HANGUL_HIGH)
//#define NUM_HANGUL_LOW   (MAX_HANGUL_LOW  - MIN_HANGUL_LOW )

//#define MAX_ASCII        96
//#define NUM_HANGUL       (NUM_HANGUL_HIGH * MAX_HANGUL_LOW + 50000)

typedef struct tagFontTexture
{
	//int nTextureCoord;
	LPDIRECT3DTEXTURE9 pTexture;
	FLOAT tx1;
	FLOAT ty1;
	FLOAT tx2;
	FLOAT ty2;
} FONTTEX, * LPFONTTEX;

#if defined( __CLIENT)
#include "EditString.h"
#endif


typedef struct tagGlyphInfo
{
	char text[6];
	WORD wCodePage;
	bool operator < (const tagGlyphInfo& glyphInfo) const { return memcmp(this, &glyphInfo, sizeof(tagGlyphInfo)) < 0; }
} GLYPHINFO;

void    SetCodePage( int nLang );

//-----------------------------------------------------------------------------
// Name: class CD3DFontAPI
// Desc: Texture-based font class for doing text in a 3D scene.
//-----------------------------------------------------------------------------
class CD3DFontAPI
{
    TCHAR   m_strFontName[80];            // Font properties
    DWORD   m_dwFontHeight;
    DWORD   m_dwFontFlags;

	DWORD   m_bCaption;
    LPDIRECT3DDEVICE9       m_pd3dDevice; // A D3DDevice used for rendering
    FLOAT   m_fTextScale;
    DWORD   m_dwSpacing;                  // Character pixel spacing per side

	/////////////////////////////////////////
	LPD3DXFONT    m_pFont;
	//LPD3DXFONT    m_pFont2;
	//LPD3DXFONT    m_pStatsFont;
	LPD3DXMESH    m_pMesh3DText;
	//WCHAR*        m_strTextBuffer;
	
	ID3DXSprite*  m_pD3DXSprite;
	
//	int           m_nFontSize;
	/////////////////////////////////////////

public:
	int   m_nOutLine;
	DWORD m_dwColor;
	DWORD m_dwBgColor;
	DWORD m_dwFlags;
	
public:
	SIZE m_sizeBitmap;

	//LPFONTTEX GetFontOffset( WORD wText ) { if( wText >= m_aFontOffset.GetSize() ) return NULL; return (LPFONTTEX)m_aFontOffset.GetAt( wText ); }
    // 2D and 3D text drawing functions
#if defined( __CLIENT)
    virtual HRESULT DrawText( FLOAT x, FLOAT y, DWORD dwColor, 
		CEditString& editString, int nPos = 0, int nLine = 0, int nLineSpace = 0, DWORD dwFlags=0L );
	virtual HRESULT DrawText( FLOAT x, FLOAT y, FLOAT fXScale, FLOAT fYScale, DWORD dwColor,
		CEditString& editString, int nPos = 0, int nLine = 0, int nLineSpace = 0, DWORD dwFlags=0L  );
#endif 
	
    virtual HRESULT DrawText( FLOAT x, FLOAT y, DWORD dwColor, 
                      const TCHAR* strText, DWORD dwFlags=0L );
	virtual HRESULT DrawText( FLOAT x, FLOAT y, FLOAT fXScale, FLOAT fYScale, DWORD dwColor,
                            const TCHAR* strText, DWORD dwFlags=0L  );
    HRESULT DrawTextScaled( FLOAT x, FLOAT y, FLOAT z, 
                            FLOAT fXScale, FLOAT fYScale, DWORD dwColor, 
                            const TCHAR* strText, DWORD dwFlags=0L );
    HRESULT Render3DText( const TCHAR* strText, DWORD dwColor, DWORD dwFlags=0L );
    
    // Function to get extent of text
    virtual HRESULT GetTextExtent( const TCHAR* strText, SIZE* pSize );

    // Initializing and destroying device-dependent objects
    virtual HRESULT InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
    virtual HRESULT RestoreDeviceObjects();
    virtual HRESULT InvalidateDeviceObjects();
    virtual HRESULT DeleteDeviceObjects();

    // Constructor / destructor
	CD3DFontAPI();
    CD3DFontAPI( const TCHAR* strFontName, DWORD dwHeight, DWORD dwFlags=0L );
    virtual ~CD3DFontAPI();

	// add
	DWORD m_dwMaxHeight;

	DWORD GetMaxHeight() { return m_dwMaxHeight; }
};
class CD3DFont //: public CD3DFontAPI
{
    char    m_strFontName[80];            // Font properties
    DWORD   m_dwFontHeight;
    DWORD   m_dwFontFlags;

	DWORD   m_bCaption;
    LPDIRECT3DDEVICE9       m_pd3dDevice; // A D3DDevice used for rendering
	LPDIRECT3DVERTEXBUFFER9 m_pVB;        // VertexBuffer for rendering text
    DWORD   m_dwTexWidth;                 // Texture dimensions
    DWORD   m_dwTexHeight;
    FLOAT   m_fTextScale;
    DWORD   m_dwSpacing;                  // Character pixel spacing per side

    // Stateblocks for setting and restoring render states
    LPDIRECT3DSTATEBLOCK9 m_pStateBlockSaved;
    LPDIRECT3DSTATEBLOCK9 m_pStateBlockDrawText;

    HRESULT   CreateGDIFont( HDC hDC, HFONT* pFont );
    HRESULT   PaintAlphabet( HDC hDC,DWORD* pBitmapBits, BOOL bMeasureOnly=FALSE );
	void CopyToTexture( LPDIRECT3DTEXTURE9 pTex, CSize size, DWORD dwBgColor );
	HFONT   m_hFont;
	HFONT   m_hFontOld;
	HDC     m_hDC;
	HBITMAP m_hbmBitmap;
	HGDIOBJ m_hbmOld;
	DWORD*  m_pBitmapBits;

	LPDIRECT3DTEXTURE9 m_pCurTexture;
	int m_nCurX,m_nCurY;

	void MakeOutLine( int nWidth, WORD* pDst16 );	
	LPDIRECT3DTEXTURE9 CreateTexture();
public:
	CPtrArray m_apTexture;
    
	DWORD  m_dwColor;
	DWORD  m_dwBgColor;
	DWORD  m_nOutLine;
	DWORD  m_dwFlags;
	
	SIZE m_sizeBitmap;
	
	WORD	m_wCodePage;

	FONTTEX* GetFontTex(const char* begin, const char* end, WORD wCodePage );
	HFONT    GetFont(WORD wCodePage);

	typedef	map<GLYPHINFO,FONTTEX>	font_tex_map;
	typedef	font_tex_map::iterator	font_tex_map_iter;

	font_tex_map	m_fontTex;
	DWORD			m_dwNumTriangles;

	typedef map<WORD, HFONT>		font_map;
	typedef font_map::iterator		font_map_iter;

	font_map		m_fontMap;
	
    HRESULT DrawText( FLOAT x, FLOAT y, DWORD dwColor, 
                      const char* strText, DWORD dwFlags=0L, WORD wCodePage=0 );
	HRESULT DrawText( FLOAT x, FLOAT y, FLOAT fXScale, FLOAT fYScale, DWORD dwColor,
                            const char* strText, DWORD dwFlags=0L, WORD wCodePage=0  );

#if defined( __CLIENT)
    HRESULT DrawText( FLOAT x, FLOAT y, DWORD dwColor, 
		CEditString& editString, int nPos = 0, int nLine = 0, int nLineSpace = 0, DWORD dwFlags=0L );
	HRESULT DrawText( FLOAT x, FLOAT y, FLOAT fXScale, FLOAT fYScale, DWORD dwColor,
		CEditString& editString, int nPos = 0, int nLine = 0, int nLineSpace = 0, DWORD dwFlags=0L  );
	HRESULT GetTextExtent_EditString( CEditString& editString, SIZE* pSize, int nLineBegin = 0, int nLineCount = -1, WORD wCodePage=0 );
	CSize GetTextExtent_EditString( CEditString& editString, int nLineBegin = 0, int nLineCount = -1, WORD wCodePage=0) { SIZE size; GetTextExtent_EditString( editString, &size, nLineBegin, nLineCount, wCodePage ); return size; }
#endif
    // Function to get extent of text
    HRESULT GetTextExtent( const char* strText, SIZE* pSize, WORD wCodePage=0 );
    CSize GetTextExtent( LPCTSTR pszText ) { SIZE size; GetTextExtent( pszText, &size ); return size; }

    // Initializing and destroying device-dependent objects
    HRESULT InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
    HRESULT RestoreDeviceObjects();
    HRESULT InvalidateDeviceObjects();
    HRESULT DeleteDeviceObjects();

    // Constructor / destructor
    CD3DFont( const char* strFontName, DWORD dwHeight, DWORD dwFlags=0L);
    ~CD3DFont();

	// add
	DWORD m_dwMaxHeight;

	DWORD GetMaxHeight() { return m_dwMaxHeight; }
};

class CD3DFontAPIVTN : public CD3DFontAPI
{
public:
	CD3DFont* m_pD3DFont;
	
public:
	CD3DFontAPIVTN( const TCHAR* strFontName, DWORD dwHeight, DWORD dwFlags=0L );
	~CD3DFontAPIVTN();

public:
#if defined( __CLIENT)
    HRESULT DrawText( FLOAT x, FLOAT y, DWORD dwColor, CEditString& editString, int nPos = 0, int nLine = 0, int nLineSpace = 0, DWORD dwFlags=0L );
	HRESULT DrawText( FLOAT x, FLOAT y, FLOAT fXScale, FLOAT fYScale, DWORD dwColor, CEditString& editString, int nPos = 0, int nLine = 0, int nLineSpace = 0, DWORD dwFlags=0L );
#endif
    HRESULT DrawText( FLOAT x, FLOAT y, DWORD dwColor, const TCHAR* strText, DWORD dwFlags=0L );
	HRESULT DrawText( FLOAT x, FLOAT y, FLOAT fXScale, FLOAT fYScale, DWORD dwColor, const TCHAR* strText, DWORD dwFlags=0L );

    // Function to get extent of text
    HRESULT GetTextExtent( const TCHAR* strText, SIZE* pSize );

    // Initializing and destroying device-dependent objects
    HRESULT InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
    HRESULT RestoreDeviceObjects();
    HRESULT InvalidateDeviceObjects();
    HRESULT DeleteDeviceObjects();

	DWORD GetMaxHeight();
};
#endif 










