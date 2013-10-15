#include "stdafx.h"
#include <wingdi.h>
#include <tchar.h>
#include <stdio.h>
#include "D3DX9.h"
#include "File.h"
#include "Debug.h"
#if !defined(__SERVER)
	#include "targa.h"
#endif

int GetCharLen( const CHAR* pStr )
{
	char* pNext = CharNext( pStr );
	return (int) (pNext - pStr );
}

// 외국어포함 문장의 길이를 잘라준다.
int GetStrCut( const CHAR* pSrc, CHAR* pDest, int nCount )
{
	int nOffset = 0;
	int nLen;
	
	while( nCount )
	{
		nLen = GetCharLen( &pSrc[ nOffset ]  );
		nOffset += nLen;
		nCount--;
	}
	
	memcpy( pDest, pSrc, sizeof(char)*nOffset);
	
	return 1;
}	

// 외국어포함 문자 갯수를 알아냄
int GetStrLen( const CHAR* pSrc )
{
	int nCount = 0;
	int nOffset = 0;
	int nLen;
	
	while( pSrc[ nOffset ] != NULL )
	{
		nLen = GetCharLen( &pSrc[ nOffset ]  );
		nOffset += nLen;
		nCount++;
	}
	
	return nCount;
}	


int CopyChar( const char* pSrc, char* pDest )
{
	const char* pNext = CharNextEx(pSrc);

	memcpy(pDest, pSrc, pNext - pSrc);

	return pNext - pSrc;
}

BOOL IsMultiByte( const char* pSrc )
{
	return ( CharNextEx(pSrc) - pSrc ) > 1;
}

BOOL IsNative( LPCTSTR lpszStr )
{
	ASSERT( g_codePage != 0 );

	LPCWSTR pwSrc = (LPCWSTR) lpszStr;

	if( g_codePage == 874 ) // 타이 
	{
		return (BYTE)*lpszStr >= 0xa1 && (BYTE)*lpszStr <= 0xfb;
	}
	else
	if( g_codePage == 949 ) // 한글 
	{
		return IsHangul( *pwSrc );
	}
	else
	if( g_codePage == 932 ) // 일본 
	{
		return IsDBCSLeadByte( (BYTE)( *pwSrc ) );
	}
	else
	if( g_codePage == 936 ) // 한자 : 중국
	{
		return IsDBCSLeadByte( (BYTE)( *pwSrc ) );
	}
	else
	if( g_codePage == 950 ) // 한자 : 대만 
	{
//		return IsDBCSLeadByte( *pwSrc );

		if( ((BYTE)*lpszStr >= 0xCA && (BYTE)*lpszStr <= 0xFD) && ( (BYTE)*lpszStr+1 >= 0xA1 ) && ( (BYTE)*lpszStr+1 <= 0xFE) )
		{
			return TRUE;
		}
		else
		if ( ( ( (BYTE)*lpszStr >= 0x41 ) && 
		 	 ( (BYTE)*lpszStr <= 0x5A ) ) || 
		   	( ( (BYTE)*lpszStr >= 0x61 ) && ( (BYTE)*lpszStr <= 0x7A) ) ) 	
		{ 
			return TRUE;			
		} 
		else
		if( isdigit2( (BYTE)*lpszStr ) )
			return TRUE;
		
	}
	return FALSE;
}

BOOL IsHangul( WORD word )
{
	BYTE l = word & 0xff;
	BYTE h = word >> 8;

	// 특수 문자, 일어 기타 언어 FALSE
	if( h >= 0xa1 && h <= 0xac && l >= 0xa0 && l <= 0xff )
		return FALSE;
	// 확장 완성형  
	if( h >= 0x81 && h <= 0xc6 && l >= 0x41 && l <= 0xfe )
		return TRUE;
	// 완성형 코드 
	if( h >= 0xb0 && h <= 0xc8 && l >= 0xa1 && l <= 0xfe )
		return TRUE;
	return FALSE;
} 

void SetStrNull( TCHAR* lpStr, int nNullLength )
{
	int nLength = strlen( lpStr );
	// 0을 넣을 포지션이 실제 스트링 길이보다 길면 실제 스트링 길이로 맞출 필요가 있음
	if( nNullLength > nLength )
		nNullLength = nLength;
	for( int i = 0; i < nNullLength; )
	{
#ifdef __CLIENT
		if( ::GetLanguage() == LANG_THA && g_codePage == 874 ) // 타이 
			i++;
		else if(::GetLanguage() == LANG_VTN && g_codePage == 1258)
			i++;
		else
#endif//__CLIENT
		if( IsDBCSLeadByte( lpStr[ i ] ) )
			i+=2;
		else
			i++;
	}
	// i가 nLength 보다 크다면 Word캐릭터일 것이고, 끝부분이 깨져서 오차가 생긴 것이다.
	if( i > nNullLength )
		lpStr[ i - 2 ] = 0;
	else
		lpStr[ i ] = 0;
}
void SetStrNull( CString& string, int nNullLength )
{
	int nLength = string.GetLength();
	// 0을 넣을 포지션이 실제 스트링 길이보다 길면 실제 스트링 길이로 맞출 필요가 있음
	if( nNullLength > nLength )
		nNullLength = nLength;
	for( int i = 0; i < nNullLength; )
	{
#ifdef __CLIENT
		if( ::GetLanguage() == LANG_THA && g_codePage == 874 ) // 타이 
			i++;
		else if(::GetLanguage() == LANG_VTN && g_codePage == 1258)
			i++;
		else
#endif//__CLIENT
		if( IsDBCSLeadByte( string[ i ] ) )
			i+=2;
		else
			i++;
	}
	// i가 nLength 보다 크다면 Word캐릭터일 것이고, 끝부분이 깨져서 오차가 생긴 것이다.
	if( i > nNullLength )
		string = string.Left( i - 2 );
	else
		string = string.Left( i );
}

#if	defined (__WORLDSERVER) || defined (__CLIENT)

HRESULT LoadTextureFromRes( 
	LPDIRECT3DDEVICE9 pd3dDevice, 
	LPCTSTR strTexture,
	LPDIRECT3DTEXTURE9* ppTexture,
	DWORD MipFilter,
	D3DFORMAT d3dFormat 
	)
{

    return LoadTextureFromRes( pd3dDevice, strTexture, 
		D3DX_DEFAULT, D3DX_DEFAULT, MipFilter, 0, d3dFormat, 
		D3DPOOL_MANAGED, D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, 
		D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, 0, NULL, NULL, ppTexture );
}

HRESULT LoadTextureFromRes( 
	LPDIRECT3DDEVICE9 pDevice,
	LPCTSTR pFileName,
	UINT Width,
	UINT Height,
	UINT MipLevels,
	DWORD Usage,
	D3DFORMAT Format,
	D3DPOOL Pool,
	DWORD Filter,
	DWORD MipFilter,
	D3DCOLOR ColorKey,
	D3DXIMAGE_INFO *pSrcInfo,
	PALETTEENTRY *pPalette,
	LPDIRECT3DTEXTURE9 *ppTexture
	)
{
	HRESULT hr = E_FAIL;
	CResFile file;
	if( file.Open( pFileName, "rb" ) == FALSE )
	{
		LPCTSTR szErr = Error( "::LoadTextureFromRes : %s not found", pFileName );
		ADDERRORMSG( szErr );
		return E_FAIL;
	}
	
	int nSrcDataSize = file.GetLength();
	LPBYTE pSrcData = new BYTE[ nSrcDataSize ];
	if( file.Read( pSrcData, nSrcDataSize ) >= 1 )
	{
		hr = D3DXCreateTextureFromFileInMemoryEx( pDevice,
			   pSrcData, nSrcDataSize, Width, Height, MipLevels, Usage, Format, Pool, Filter, MipFilter, ColorKey, pSrcInfo, pPalette, ppTexture );
		if( FAILED( hr ) )
		{
			LPCTSTR szErr = Error( "D3DXCreateTextureFromFileInMemoryEx %s %s", pFileName, DXGetErrorString9( hr ) );
			ADDERRORMSG( szErr );
		}
	}
	SAFE_DELETE_ARRAY( pSrcData );
	return hr;
}

char g_szCrashClient[1024];

void PaintTexture( LPVOID pDestData, LPIMAGE pImage, CPoint pt, CSize sizeSurface, D3DFORMAT d3dFormat )
{
	memset( g_szCrashClient, 0, sizeof(char) * 1024 );

	strcat( g_szCrashClient, "\nPaintTexture - IN" );

	SIZE sizeTexture = pImage->size;
	DWORD dwOffsetSrc = 0;
	DWORD dwOffsetDest = ( pt.y * sizeSurface.cx + pt.x );
	DWORD dwLine = sizeSurface.cx - sizeTexture.cx;
	int nSizeSurface = sizeSurface.cx * sizeSurface.cy;

	static char szstrError[128];

	sprintf( szstrError, "\nSurface %d %d, Pt %d %d IMG_BIT = %d, D3DFORMAT = %d", sizeSurface.cx, sizeSurface.cy, pt.x, pt.y, pImage->nImgBit, d3dFormat );
	strcat( g_szCrashClient, szstrError );

	if( d3dFormat == D3DFMT_A4R4G4B4 )
	{
		LPWORD pDest = (LPWORD) pDestData;
		LPBYTE pSrc = pImage->lpData;
		if( pImage->nImgBit == IMGBIT_24 )
		{
			for( int y = pt.y; y < pt.y + sizeTexture.cy; y++, dwOffsetDest += dwLine )
			{
				for( int x = pt.x; x < pt.x + sizeTexture.cx; x++, dwOffsetSrc += 3, dwOffsetDest++ )
				{
					if( dwOffsetDest >= 0 && (DWORD)dwOffsetDest < (DWORD)nSizeSurface )
					{
						DWORD bitcount = pImage->nImgBit / 8;
//						if( dwOffsetSrc >= pImage->dwSize )
						if( pImage->dwSize != pImage->dwSizeBak )
						{
							
							char szFile[2048] = { 0 };

							sprintf( szFile, "D3DFMT_A4R4G4B4 ERROR IMGBIT_24\n Current = %d   Tex X = %d, Y = %d  [%d,%d]\n LINE = %d\n Size = %d", dwOffsetSrc, sizeTexture.cx, sizeTexture.cy, sizeSurface.cx, sizeSurface.cy, dwLine, pImage->dwSize );
							//if( byData4 )
							//	pDest[ dwOffsetDest ] = ( byData4 << 12 ) | ( byData3 << 8 ) | ( byData2 << 4 ) | byData1;
							AfxMessageBox(szFile, MB_OKCANCEL);

//							dwOffsetSrc = si
							break;
						}

						BYTE byData1 = pSrc[ dwOffsetSrc + 0 ] >> 4; 
						BYTE byData2 = pSrc[ dwOffsetSrc + 1 ] >> 4; 
						BYTE byData3 = pSrc[ dwOffsetSrc + 2 ] >> 4; 
						// 원래 있던 배경을 보존하기 위해서 ff00ff면 찍지 않는다.
						if( byData1 != 0xf || byData2 != 0 || byData3 != 0xf )
							pDest[ dwOffsetDest ] = 0xf000 | ( byData3 << 8 ) | ( byData2 << 4 ) | byData1;
					}
				}
			}
		}
		else
		if( pImage->nImgBit == IMGBIT_32 )
		{
			for( int y = pt.y; y < pt.y + sizeTexture.cy; y++, dwOffsetDest += dwLine )
			{
				for( int x = pt.x; x < pt.x + sizeTexture.cx; x++, dwOffsetSrc += 4, dwOffsetDest++ )
				{
					if( dwOffsetDest >= 0 && (DWORD)dwOffsetDest < (DWORD)nSizeSurface )
					{
						if( dwOffsetSrc >= (DWORD)( (sizeTexture.cx * sizeTexture.cy * 4) ) )
						{
							//if( byData4 )
							//	pDest[ dwOffsetDest ] = ( byData4 << 12 ) | ( byData3 << 8 ) | ( byData2 << 4 ) | byData1;
							AfxMessageBox(" D3DFMT_A4R4G4B4 ERROR IMGBIT_32", MB_OK );
							break;
						}

						BYTE byData1 = pSrc[ dwOffsetSrc + 0 ] >> 4; 
						BYTE byData2 = pSrc[ dwOffsetSrc + 1 ] >> 4; 
						BYTE byData3 = pSrc[ dwOffsetSrc + 2 ] >> 4; 
						

						BYTE byData4 = pSrc[ dwOffsetSrc + 3 ] >> 4; 
						// 원래 있던 배경을 보존하기 위해서 알파값이 전혀 없으면 찍지 않는다.
						if( byData4 )
							pDest[ dwOffsetDest ] = ( byData4 << 12 ) | ( byData3 << 8 ) | ( byData2 << 4 ) | byData1;
					}
				}
			}
		}
	}
	else
	if( d3dFormat == D3DFMT_A8R8G8B8 )
	{
		LPDWORD pDest = (LPDWORD) pDestData;
		if( pImage->nImgBit == IMGBIT_24 )
		{
			LPBYTE pSrc = pImage->lpData;
			for( int y = pt.y; y < pt.y + sizeTexture.cy; y++, dwOffsetDest += dwLine )
			{
				for( int x = pt.x; x < pt.x + sizeTexture.cx; x++, dwOffsetSrc += 3, dwOffsetDest++ )
				{
					if( dwOffsetDest >= 0 && (DWORD)dwOffsetDest < (DWORD)nSizeSurface )
					{
						if( dwOffsetSrc >= (DWORD)( (sizeTexture.cx * sizeTexture.cy * 2 ) ) )
						{
							//if( byData4 )
							//	pDest[ dwOffsetDest ] = ( byData4 << 12 ) | ( byData3 << 8 ) | ( byData2 << 4 ) | byData1;
							AfxMessageBox(" D3DFMT_A8R8G8B8 ERROR IMGBIT_24", MB_OK );
							break;
						}

						BYTE byData1 = pSrc[ dwOffsetSrc + 0 ]; 
						BYTE byData2 = pSrc[ dwOffsetSrc + 1 ]; 
						BYTE byData3 = pSrc[ dwOffsetSrc + 2 ]; 
						// 원래 있던 배경을 보존하기 위해서 ff00ff면 찍지 않는다.
						if( byData1 != 0xff || byData2 != 0 || byData3 != 0xff )
							pDest[ dwOffsetDest ] = 0xf000 | ( byData3 << 16 ) | ( byData2 << 8 ) | byData1;
					}
				}
			}
		}
		else
		if( pImage->nImgBit == IMGBIT_32 )
		{
			LPDWORD pSrc = (LPDWORD)pImage->lpData;
			for( int y = pt.y; y < pt.y + sizeTexture.cy; y++, dwOffsetDest += dwLine )
			{
				for( int x = pt.x; x < pt.x + sizeTexture.cx; x++, dwOffsetSrc++, dwOffsetDest++ )
				{
					if( dwOffsetDest >= 0 && (DWORD)dwOffsetDest < (DWORD)nSizeSurface )
					{
						// 원래 있던 배경을 보존하기 위해서 알파값이 전혀 없으면 찍지 않는다.
						if( pSrc[ dwOffsetSrc ] & 0xff000000 )
							pDest[ dwOffsetDest ] = pSrc[ dwOffsetSrc ];//yData4 << 24 ) | ( byData3 << 16 ) | ( byData2 << 8 ) | byData1;
					}
				}
			}
		}
	}

	strcat( g_szCrashClient, "\nPaintTexture - OUT" );
}

void AdjustSize( SIZE* pSize )
{
	// 사이즈를 2의 자승으로 바꾸기 
	WORD dwTemp = 0x8000;
	for( int i = 0; i < 16; i++ )
	{
		if( pSize->cx & dwTemp )
		{
			if( pSize->cx % dwTemp )
			{
				dwTemp <<= 1;
				pSize->cx = dwTemp;
			}
			break;
		}
		dwTemp >>= 1;
	}
	dwTemp = 0x8000;
	for( i = 0; i < 16; i++ )
	{
		if( pSize->cy & dwTemp )
		{
			if( pSize->cy % dwTemp )
			{
				dwTemp <<= 1;
				pSize->cy = dwTemp;
			}
			break;
		}
		dwTemp >>= 1;
	}
	//return size;
}
BOOL LoadImage( LPCTSTR lpszFileName, LPIMAGE lpImage ) //LPBYTE* lppData, SIZE* pSize, int* pnImgBit )
{
	TCHAR szFileName[ MAX_PATH ];
	strcpy( szFileName, lpszFileName );
	strlwr( szFileName );
	if( strstr( szFileName, ".tga" ) )
	{
		return LoadTGA( lpszFileName, lpImage );
	}
	else
	if( strstr( szFileName, ".bmp" ) )
	{
		return LoadBMP( lpszFileName, lpImage );
	}				
	return FALSE;
}
BOOL LoadTGA( LPCTSTR lpszFileName, LPIMAGE lpImage ) //LPBYTE* lppData, SIZE* pSize )
{
#if !defined(__SERVER)
	lpImage->nImgBit = IMGBIT_32;
	CTarga targa;
	return targa.Load( lpszFileName, &lpImage->lpData, &lpImage->size );
#endif
	return FALSE;
}

BOOL LoadBMP( LPCTSTR lpszFileName, LPIMAGE lpImage ) //LPBYTE* lppData, SIZE* pSize )
{
	CResFile file;
	if( file.Open( lpszFileName, "rb" ) == FALSE )
	{
		Error( "LoadBMP Failed = %s", lpszFileName );
		return FALSE;
	}

	BITMAPINFOHEADER infoHeader;
	LPBYTE lpData = (LPBYTE)file.Read();
	LPBYTE lpDataDelete = lpData;
	lpData += sizeof( BITMAPFILEHEADER );
	memcpy( &infoHeader, lpData, sizeof( BITMAPINFOHEADER ) );
	lpData += sizeof( BITMAPINFOHEADER );

	int bitCount = 1;
	if( infoHeader.biBitCount < 8)
	{
		lpImage->size.cx = 0;
		lpImage->size.cy = 0;
		lpImage->lpData = NULL;
		lpImage->nImgBit = IMGBIT_24;
		safe_delete_array( lpDataDelete );
		return FALSE;
	}
	if( infoHeader.biBitCount > 8)
	{
		bitCount = infoHeader.biBitCount / 8;
	}
	if( infoHeader.biCompression)
	{
		lpImage->size.cx = 0;
		lpImage->size.cy = 0;
		lpImage->lpData = NULL;
		lpImage->nImgBit = IMGBIT_24;
		safe_delete_array( lpDataDelete );
		return FALSE;
	}
	int nLgHeight = infoHeader.biHeight;
	int nLgWidth  = infoHeader.biWidth;

	lpImage->size.cx = nLgWidth;
	lpImage->size.cy = nLgHeight;

	if( bitCount == 1 ) 
	{
		if( infoHeader.biClrUsed == 0 )
			infoHeader.biClrUsed = 256;
		lpData += infoHeader.biClrUsed * 4;
	}

	LPBYTE lpNewData = new BYTE[ nLgWidth * nLgHeight * bitCount * 2 ]; // 버그 때문에 2 곱했음. 사이즈가 4의 배수가 아니어서 문제 생겨서 * 2 했음 
	lpImage->dwSizeBak	= lpImage->dwSize = nLgWidth * nLgHeight * bitCount * 2;

	int nPgHeight = abs( infoHeader.biHeight );
	int nPgWidth  = infoHeader.biWidth;

	nPgWidth *= bitCount;
	nLgWidth *= bitCount;
	nLgWidth += ( ( nLgWidth % 4 ) ? ( 4 - nLgWidth % 4 ) : 0 );

	LPBYTE pTempDst = lpNewData + ( nPgWidth * ( nPgHeight - 1 ) );

	for( int i = 0; i < nPgHeight; i++ )
	{
		memcpy( pTempDst, lpData, nLgWidth );
		lpData += nLgWidth;
		pTempDst -= nPgWidth;
	}
	safe_delete_array( lpDataDelete );
	lpImage->lpData = lpNewData;
	lpImage->nImgBit = IMGBIT_24;
	return TRUE;
}

BOOL SaveBMP( LPCTSTR lpszFileName, LPBYTE lpData, SIZE size )
{
	int i, j;
	BITMAPFILEHEADER BMPheader;
	BITMAPINFOHEADER header = { 40,640,-480,1,24,0,0,5904,5904,0,0 };
	header.biBitCount = 24;
	header.biWidth  = size.cx;
	header.biHeight = -abs(size.cy);

	LPBYTE pBMP = NULL;
	LPBYTE pp;

	LPBYTE pSurface = (LPBYTE) lpData;

	int nBit = 24 >> 3;//m_infoHeader.biBitCount >> 3;

	BMPheader.bfType = 'M';
	BMPheader.bfType <<= 8;
	BMPheader.bfType += 'B';
	BMPheader.bfReserved1 = 0; 
	BMPheader.bfReserved2 = 0;
	// 16비트(nBit==2)라도 팔레트가 있는 것 처럼 안하면 그림이 밀려서 출력한다. 아케인 캡춰 관련 버그 
	if(nBit == 1 || nBit == 2)
	{
		BMPheader.bfSize = ( sizeof(BITMAPFILEHEADER ) + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 256 + header.biWidth * abs(header.biHeight) * nBit );
		BMPheader.bfOffBits = sizeof(BITMAPFILEHEADER ) + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 256;
	}
	else
	{
		BMPheader.bfSize = ( sizeof(BITMAPFILEHEADER ) + sizeof(BITMAPINFOHEADER) + header.biWidth * abs(header.biHeight) * nBit );
		BMPheader.bfOffBits = sizeof(BITMAPFILEHEADER ) + sizeof(BITMAPINFOHEADER);
	}
 	pBMP = new BYTE[BMPheader.bfSize];
	if( pBMP == NULL )	return	FALSE;
	pp = pBMP;

	header.biHeight = abs( header.biHeight);

	memcpy( pp, &BMPheader, sizeof(BITMAPFILEHEADER) ); pp += sizeof(BITMAPFILEHEADER);
	memcpy( pp, &header, sizeof(BITMAPINFOHEADER) ); pp += sizeof(BITMAPINFOHEADER);

	if(nBit == 1 || nBit == 2)
	{
	}

	j = header.biWidth * abs(header.biHeight) * nBit;


	for( i = 0; i < abs(header.biHeight); i ++ )
	{
		j  -= header.biWidth * nBit;
		memcpy( pp, pSurface + j, header.biWidth * nBit );		
		pp += header.biWidth * nBit;
	}

	CFile file;
	if(file.Open(lpszFileName,CFile::modeWrite|CFile::modeCreate))
	{
		file.Write(pBMP,(int)(pp - pBMP));
		safe_delete_array( pBMP );
		return TRUE;
	}

	safe_delete_array( pBMP );
	return FALSE; 
}

void GetPickRay(CRect rect,POINT ptCursor,D3DXMATRIX* pmatProj,D3DXMATRIX* pmatView,D3DXVECTOR3* pvPickRayOrig,D3DXVECTOR3* pvPickRayDir)
{
	D3DXVECTOR3 v;
	v.x =  ( ( ( 2.0f * ptCursor.x ) / rect.Width()  ) - 1 ) / pmatProj->_11;
	v.y = -( ( ( 2.0f * ptCursor.y ) / rect.Height() ) - 1 ) / pmatProj->_22;
	v.z =  1.0f;

	// Get the inverse view matrix
	D3DXMATRIX m;
	D3DXMatrixInverse( &m, NULL, pmatView );

	// Transform the screen space pick ray into 3D space
	pvPickRayDir->x  = v.x*m._11 + v.y*m._21 + v.z*m._31;
	pvPickRayDir->y  = v.x*m._12 + v.y*m._22 + v.z*m._32;
	pvPickRayDir->z  = v.x*m._13 + v.y*m._23 + v.z*m._33;
	pvPickRayOrig->x = m._41;
	pvPickRayOrig->y = m._42;
	pvPickRayOrig->z = m._43;
}

void GetRayEnd(D3DXVECTOR3* pvPickRayOrig,D3DXVECTOR3* pvPickRayDir,D3DXVECTOR3* pvPickRayEnd)
{
	FLOAT fyOrig = pvPickRayOrig->y;
	FLOAT fyDir  = -pvPickRayDir->y;
	pvPickRayDir->x = fyOrig * pvPickRayDir->x / fyDir; // vecY : posY = vecX : posX
	pvPickRayDir->z = fyOrig * pvPickRayDir->z / fyDir; // vecY : posY = vecZ : posZ
	pvPickRayDir->y = -fyOrig; // 아래 *pvPickRayDir + *pvPickRayOrig에서 y가 0이 나오게 하려면 -fyOrig를 넣어야 한다.
	*pvPickRayEnd = *pvPickRayDir + *pvPickRayOrig;
}


BOOL IntersectTriangle( D3DXVECTOR3& v0, D3DXVECTOR3& v1, D3DXVECTOR3& v2,
			            const D3DXVECTOR3& orig, const D3DXVECTOR3& dir, 
						D3DXVECTOR3* pIntersect, FLOAT* pfDist )
{
	FLOAT fU,fV;
	if(D3DXIntersectTri( &v0, &v1, &v2, &orig, &dir, &fU, &fV, pfDist ) == TRUE )
	{
		*pIntersect = orig + *pfDist * dir;
		return TRUE;
	}
	return FALSE;
}

#endif	//defined (__WORLDSERVER) || defined (__CLIENT) 

#ifdef __CLIENT
static int CALLBACK FontEnumProc(const LOGFONT*, const TEXTMETRIC*, DWORD, LPARAM lParam)
{
	if (lParam != NULL)
	{
		*(BOOL*)lParam = TRUE;
	}
	return 0;
}
BOOL IsFontInstalled( LPCTSTR pszFace )
{
	BOOL bInstalled;
	HDC hDC;
	LOGFONT lf;

	memset(&lf, 0, sizeof(lf));

	lstrcpy(lf.lfFaceName, pszFace);
	lf.lfCharSet = DEFAULT_CHARSET;

	bInstalled = FALSE;
	hDC = ::GetDC(NULL);
	if (hDC != NULL)
	{
		::EnumFontFamiliesEx(hDC, &lf, FontEnumProc, (LPARAM)&bInstalled, 0);
		::ReleaseDC(NULL, hDC);
	}

	return bInstalled;
}
#endif // __CLIENT