#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "targa.h"
#include "xutil.h"

CTarga::CTarga()
{
	m_pBuf = NULL;
	m_nTgaDataIdx = 0; 
	m_nReadPixelCount = 0;
	m_pData = NULL;
}
CTarga::~CTarga()
{
	if(m_pBuf) safe_delete( m_pBuf );
}
void CTarga::FreeBuffer()
{
	ASSERT(m_pBuf);
	safe_delete( m_pBuf ); 
	m_pBuf = NULL;
	m_pData = NULL;
}
BOOL CTarga::Load( LPCSTR lpszFileName, LPBYTE* lppData, SIZE* pSize )
{
	CResFile file;
	if( !file.Open( lpszFileName, "rb" ) )
	{
		Error( "CTarga::Load Failed = %s", lpszFileName );
		return FALSE;
	}
	m_nFileSize  = file.GetLength();
	m_pBuf = new BYTE[m_nFileSize];

	file.Read( m_pBuf, m_nFileSize, 1 );

	DWORD * ppPal;
	int pDestBPP = 0;
	m_nTgaDataIdx = 0; 
	m_nReadPixelCount = 0;
	DoDecode((DWORD**)&ppPal,&pDestBPP,(char*)m_pBuf);
	/*
	if(pPal != NULL && m_header.CmapType != 0) 
	{
		RGBQUAD rgb[256];
		for(int i = 0; i < 256; i++)
		{
			rgb[i].rgbRed   = (ppPal)[i] & 0x00ff0000;
			rgb[i].rgbGreen = (ppPal)[i] & 0x0000ff00;
			rgb[i].rgbBlue  = (ppPal)[i] & 0x000000ff;
		}
		pPal->CreateIdPalette(rgb);
	}
	else pPal = NULL;
	*/
	//CreateDIBSection(pDc,pPal,m_header.ImageWidth,-m_header.ImageHeight,pDestBPP);	
	m_pData = new BYTE[ m_header.ImageWidth * m_header.ImageHeight * 4];
	pSize->cx = m_header.ImageWidth;
	pSize->cy = m_header.ImageHeight;
	readImage (&m_header,(DWORD**)&ppPal, pDestBPP,(char*)m_pBuf);
	//DecodeBitmap(m_pData,m_pBuf+sizeof(m_header),GetWidth()-m_header.x2);
	//DecodeBitmap(m_pData,m_pBuf+sizeof(m_header),m_header.ImageWidth-m_header.x2);
	*lppData = m_pData;
	FreeBuffer();
	
	return TRUE;
}
/*
void CTarga::CreateTarga(CDC* pDc,CDibPalette* pPal)
{
	//memcpy(&m_header,m_pBuf,sizeof(m_header));
	DWORD * ppPal;
	int pDestBPP = 0;
	m_nTgaDataIdx = 0; 
	m_nReadPixelCount = 0;
	DoDecode((DWORD**)&ppPal,&pDestBPP,(char*)m_pBuf);
	if(pPal != NULL && m_header.CmapType != 0) 
	{
		RGBQUAD rgb[256];
		for(int i = 0; i < 256; i++)
		{
			rgb[i].rgbRed   = (ppPal)[i] & 0x00ff0000;
			rgb[i].rgbGreen = (ppPal)[i] & 0x0000ff00;
			rgb[i].rgbBlue  = (ppPal)[i] & 0x000000ff;
		}
		pPal->CreateIdPalette(rgb);
	}
	else pPal = NULL;
	CreateDIBSection(pDc,pPal,m_header.ImageWidth,-m_header.ImageHeight,pDestBPP);	
	readImage (&m_header,(DWORD**)&ppPal, pDestBPP,(char*)m_pBuf);
	//DecodeBitmap(m_pData,m_pBuf+sizeof(m_header),GetWidth()-m_header.x2);
	FreeBuffer();
}
*/
BOOL CTarga::DecodeBitmap(LPBYTE pDst,LPBYTE pSrc,int nAlign)
{
	/*
	int nWidthCnt, nHeightCnt, nAlignCnt;
	BYTE arcCnt, imageCode;

	if(nAlign % 2) nAlign--;

	for(nHeightCnt = GetHeight(); nHeightCnt; nHeightCnt--) 
	{
		for(nWidthCnt = GetWidth() - nAlign; nWidthCnt > 0;) 
		{
			arcCnt = *pSrc++;
 			if((arcCnt & 0xc0) != 0xc0)
			{
				*pDst++ = arcCnt;
				nWidthCnt--;
			}
		  else
			{
		    arcCnt &= 0x3f;
		    imageCode = *pSrc++;
				nWidthCnt -= arcCnt;
			  while(arcCnt--)
					*pDst++ = imageCode;
			}
		}
		memset(pDst,0,nAlign);
	}
	*/
	return 1;
}
BOOL CTarga::GetPalette(LPRGBQUAD prgbquad)
{
	LPBYTE buf 
		= &m_pBuf[m_nFileSize-768];
	for(int i=0; i<256; i++)
	{
		prgbquad[i].rgbRed      = (buf[3*i]);
		prgbquad[i].rgbGreen    = (buf[3*i+1]);
		prgbquad[i].rgbBlue     = (buf[3*i+2]);
		prgbquad[i].rgbReserved = 0;
	}
	return TRUE;
}
//////////////////////////// targa original source
//
//
//
//
#define	TRUE	1
#define	FALSE 0
#define RGBA_BLUE   0
#define RGBA_GREEN  1
#define RGBA_RED    2
#define RGBA_ALPHA  3

char	*g_pTgaData = NULL;

inline void SetRGBAPixel( DWORD * pPixel,UCHAR r,UCHAR g,UCHAR b,UCHAR a )
{
  ((UCHAR *)pPixel)[RGBA_RED] = r;
  ((UCHAR *)pPixel)[RGBA_GREEN] = g;
  ((UCHAR *)pPixel)[RGBA_BLUE] = b;
  ((UCHAR *)pPixel)[RGBA_ALPHA] = a;
}
void CTarga::DoDecode(DWORD ** ppPal,int * pDestBPP,char * pDataSrc)
{
//  TGAHEADER m_header;

  readTgaHeader (&m_header, pDataSrc);

  if (*pDestBPP == 0)
  {
    if (m_header.PixelDepth <= 8)
      *pDestBPP = 8;
    else
      *pDestBPP = 32;
  }

  if (m_header.PixelDepth == 16 || m_header.PixelDepth == 32)
    // Image has alpha channel
	{
	  *pDestBPP = 32;
	}
  else
    // Image doesn't have alpha channel
	{
	}

  // Read the color map data (Version 1.0 and 2.0).
  if (m_header.CmapType != 0)
    readPalette (m_header.CmapIndex,m_header.CmapLength,m_header.CmapEntrySize,ppPal,*pDestBPP,pDataSrc);

}


void CTarga::readTgaHeader(TGAHEADER * m_header,char * pDataSrc)
{
  // Read the TGA header (Version 1.0 and 2.0).
	memset( m_header, 0x00, sizeof(TGAHEADER) );
  m_header->IdLength      = *(pDataSrc + m_nTgaDataIdx); m_nTgaDataIdx+=sizeof(char);
  m_header->CmapType      = *(pDataSrc + m_nTgaDataIdx); m_nTgaDataIdx+=sizeof(char);
  m_header->ImageType     = *(pDataSrc + m_nTgaDataIdx); m_nTgaDataIdx+=sizeof(char);
  m_header->CmapIndex     = *(WORD *)(pDataSrc + m_nTgaDataIdx); m_nTgaDataIdx+=sizeof(short);
  m_header->CmapLength    = *(WORD *)(pDataSrc + m_nTgaDataIdx); m_nTgaDataIdx+=sizeof(short);
  m_header->CmapEntrySize = *(pDataSrc + m_nTgaDataIdx); m_nTgaDataIdx+=sizeof(char);
  m_header->X_Origin      = *(WORD *)(pDataSrc + m_nTgaDataIdx); m_nTgaDataIdx+=sizeof(short);
  m_header->Y_Origin      = *(WORD *)(pDataSrc + m_nTgaDataIdx); m_nTgaDataIdx+=sizeof(short);
  m_header->ImageWidth    = *(WORD *)(pDataSrc + m_nTgaDataIdx); m_nTgaDataIdx+=sizeof(short);
  m_header->ImageHeight   = *(WORD *)(pDataSrc + m_nTgaDataIdx); m_nTgaDataIdx+=sizeof(short);
  m_header->PixelDepth    = *(pDataSrc + m_nTgaDataIdx); m_nTgaDataIdx+=sizeof(char);
  m_header->ImagDesc      = *(pDataSrc + m_nTgaDataIdx); m_nTgaDataIdx+=sizeof(char);

  // Skip image ID
	m_nTgaDataIdx+=m_header->IdLength;
}


void CTarga::readPalette(int StartIndex,int Length,int EntrySize,DWORD ** ppPal,int DestBPP,char * pDataSrc)
{
  int   i;

  *ppPal = new DWORD [256];
  if (!(*ppPal)) printf("Out of memory for palette.");

  for (i=StartIndex; i<StartIndex+Length; i++)
  {
    (*ppPal)[i] = readPixel32 (EntrySize, pDataSrc, *ppPal);
  }

  if (DestBPP == 8)
	{
	}
}
void CTarga::setGrayPalette(DWORD ** ppPal)
{
  int i;
  *ppPal = new DWORD [256];
  for (i=0; i<256; i++)
  {
    SetRGBAPixel ((*ppPal)+i, i, i, i, 0xFF);
  }
}
void CTarga::readImage( TGAHEADER * header,DWORD ** ppPal,int DestBPP,char * pDataSrc)
{
  bool bCompressed;

  if (m_header.ImageType == TGA_Mono ||
      m_header.ImageType == TGA_RLEMono)
    setGrayPalette (ppPal);

  switch (m_header.ImageType)
  {
    case TGA_Map:
    case TGA_RGB:
    case TGA_Mono:
      bCompressed = FALSE;
      break;
    case TGA_RLEMap:
    case TGA_RLERGB:
    case TGA_RLEMono:
      bCompressed = TRUE;
      break;
    default:
      printf( "Unknown TGA image type." );
  }
  readData (header, bCompressed, *ppPal, DestBPP, pDataSrc);
}
void CTarga::readData( TGAHEADER * header,bool bCompressed,DWORD * pPal,int DestBPP,char * pDataSrc)
{
	int Width = m_header.ImageWidth;
	int Height = m_header.ImageHeight;
	int bpp = m_header.PixelDepth;
	int nAlign = 0;//GetWidth() - GetPgWidth();

  // Bits 4 & 5 of the Image Descriptor byte control the ordering of
  // the pixels.
  bool bXReversed = ((m_header.ImagDesc & 16) == 16);
  bool bYReversed = ((m_header.ImagDesc & 32) == 32);

  UCHAR * pDest = (UCHAR*) m_pData;
//  UCHAR ** pLineArray = pBmp->GetLineArray();

	if(bYReversed == FALSE)
	{
		switch(DestBPP)
		{
			case  8: pDest += ((m_header.ImageWidth+nAlign) * m_header.ImageHeight    ) - ( m_header.ImageWidth+nAlign     ); break;
			case 32: pDest += ((m_header.ImageWidth+nAlign) * m_header.ImageHeight * 4) - ((m_header.ImageWidth+nAlign) * 4); break;
		}
	}
  for(int y = 0; y < Height; y++)
  {
		/*
	   if (bYReversed)
      pDest = pLineArray[y];
     else
      pDest = pLineArray[Height-y-1];
			*/
    if (!bCompressed)
      expandUncompressedLine (pDest, Width, bXReversed, bpp, pDataSrc, pPal, DestBPP);
     else
      expandCompressedLine (pDest, Width, bXReversed, bpp, pDataSrc, pPal, DestBPP);
		if(bYReversed == TRUE)
		{
			switch(DestBPP)
			{
				case  8: pDest += (m_header.ImageWidth+nAlign)  ; break;
				case 32: pDest += (m_header.ImageWidth+nAlign)*4; break;
			}
			memset(pDest,0,nAlign);
			pDest += nAlign;
		}
		else
		{
			memset(pDest,0,nAlign);
			switch(DestBPP)
			{
				case  8: pDest -= (m_header.ImageWidth+nAlign)  ; break;
				case 32: pDest -= (m_header.ImageWidth+nAlign)*4; break;
			}
		}
  }
}
void CTarga::expandUncompressedLine(UCHAR * pDest,int Width,bool	bReversed,int	bpp,char * pDataSrc,DWORD * pPal,int DestBPP)
{
  int x;

  for (x=0; x<Width; x++)
  {
    if (DestBPP == 32)
    {
      *((DWORD *)pDest) = readPixel32 (bpp, pDataSrc, pPal); m_nReadPixelCount++;
      pDest += 4;
    }
    else
    {
      *pDest = readPixel8 (bpp, pDataSrc);
      pDest ++;
    }
  }
}
void CTarga::expandCompressedLine( UCHAR * pDest,int Width,bool bReversed,int bpp,char * pDataSrc,DWORD * pPal,int DestBPP)
{
  int  x;
  int  i;
  UCHAR Count;

  for (x=0; x<Width; )
  {
    Count = *(pDataSrc + m_nTgaDataIdx); m_nTgaDataIdx+=sizeof(char);
    if (Count & 128)
    { // RLE-Encoded packet
      Count -= 127; // Calculate real repeat count.
      if (DestBPP == 32)
      {
        *((DWORD *)pDest) = readPixel32 (bpp, pDataSrc, pPal); 
				m_nReadPixelCount++;

        for (i=1; i<Count; i++) *((DWORD *)(pDest+i*4)) = *(DWORD *)pDest;
      }
      else
      {
        *pDest = readPixel8 (bpp, pDataSrc);
        for (i=1; i<Count; i++)
          *(pDest+i) = *pDest;
      }
    }
    else
    { // Raw packet
      Count += 1; // Calculate real repeat count.
      for (i=0; i<Count; i++)
      {
        if (DestBPP == 32)
				{
          *((DWORD *)(pDest+i*4)) = readPixel32 (bpp, pDataSrc, pPal); 
					m_nReadPixelCount++;
				}
        else
				{
          *(pDest+i) = readPixel8 (bpp, pDataSrc);
				}
      }
    }
    if (DestBPP == 32) pDest += Count*4; else pDest += Count;
		x += Count;
  }
}


DWORD CTarga::readPixel32( int bpp,char * pDataSrc,DWORD * pPal)
{
  DWORD Dest;
  WORD Src;
  UCHAR pCurEntry[4];
	int						i;

  switch (bpp)
  {
    case 8:
      Dest = pPal[*(pDataSrc + m_nTgaDataIdx)];
			m_nTgaDataIdx+=sizeof(char);
      break;
    case 15:
    case 16:
      Src = *(WORD *)(pDataSrc + m_nTgaDataIdx); m_nTgaDataIdx+=sizeof(short);
      if (bpp == 16)
        SetRGBAPixel (&Dest,
                      (UCHAR)(((Src >> 10) & 31)*8),
                      (UCHAR)(((Src >> 5) & 31)*8),
                      (UCHAR)((Src & 31)*8),
                      (UCHAR)(Src & 32786 >> 8));
       else
        SetRGBAPixel (&Dest,
                      (UCHAR)(((Src >> 10) & 31)*8),
                      (UCHAR)(((Src >> 5) & 31)*8),
                      (UCHAR)((Src & 31)*8),
                      0xFF);
      break;
    case 24:
			for( i=0;i<3;i++ )
			{
			  pCurEntry[i] = *(pDataSrc + m_nTgaDataIdx); m_nTgaDataIdx+=sizeof(char);
			}
      SetRGBAPixel (&Dest, pCurEntry[2],
                    pCurEntry[1], pCurEntry[0], 0xFF);
      break;
    case 32:
			for( i=0;i<4;i++ )
			{
			  pCurEntry[i] = *(pDataSrc + m_nTgaDataIdx); m_nTgaDataIdx+=sizeof(char);
			}

		  if( pCurEntry[3]==1 ) // Alpha value is 1.
			{
				printf( "R: %d G: %d B: %d ALPHA : %d\n",pCurEntry[2], pCurEntry[1], pCurEntry[0], pCurEntry[3] );
				printf( "left : %d right : %d\n", m_nReadPixelCount%640, m_nReadPixelCount%480 );
			}

      SetRGBAPixel (&Dest, pCurEntry[2], pCurEntry[1],
                    pCurEntry[0], pCurEntry[3]);
      break;
  }
  return Dest;
}

UCHAR CTarga::readPixel8( int bpp,char * pDataSrc)
{
  UCHAR Dest;

  if(bpp != 8)
	{
		printf( "8bit disable" );
		return 0;
	}
  Dest = *(pDataSrc + m_nTgaDataIdx); m_nTgaDataIdx+=sizeof(char);

  return Dest;
}





















