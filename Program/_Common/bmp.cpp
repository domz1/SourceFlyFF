#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "..\library\graphic.h"
#include "bmp.h"


IMPLEMENT_SERIAL(CBmp,CObject,0)

CBmp::CBmp()
{
	m_pBuffer = NULL;
	m_pCurBuf = NULL;
}
CBmp::~CBmp()
{
	if(m_pBuffer) safe_delete( m_pBuffer );
}
void CBmp::FreeBuffer()
{
	ASSERT(m_pBuffer);
	safe_delete( m_pBuffer ); 
	m_pBuffer = NULL;
	m_pCurBuf = NULL;
}

void CBmp::Serialize(CArchive& ar)
{
//	ar.Flush();
	if(ar.IsStoring())
		;
	else
	{
		ReadChunk(ar);
	}
}
BOOL CBmp::Load(LPCSTR lpszFileName)
{
	CFile file;
	if(!file.Open(lpszFileName,CFile::modeRead/*|CFile::shareDenyNone*/))
		return FALSE;
	CArchive ar(&file,CArchive::load);
	Serialize(ar);
	return TRUE;
}
BOOL CBmp::ReadChunk(CArchive& ar)
{
	m_nFileSize  = ar.GetFile()->GetLength();
	m_pCurBuf = m_pBuffer = new BYTE[m_nFileSize];
	ar.Read(m_pBuffer,m_nFileSize);
	return 0;
}
void CBmp::CreateBmp(CDC* pDc,CDibPalette* pPal)
{
	m_pCurBuf += sizeof(BITMAPFILEHEADER);
	memcpy(&m_infoHeader,m_pCurBuf,sizeof(BITMAPINFOHEADER));
	m_pCurBuf += sizeof(BITMAPINFOHEADER);

	int bitCount = 1;

	if(m_infoHeader.biBitCount < 8)
	{
		FreeBuffer();
		return;
	}
	if(m_infoHeader.biBitCount > 8)
	{
		bitCount = m_infoHeader.biBitCount / 8;
	}
	if(m_infoHeader.biCompression)
	{
		FreeBuffer();
		return;
	}
	int nLgHeight = m_infoHeader.biHeight;
	int nLgWidth  = m_infoHeader.biWidth;

	if(bitCount == 1) 
	{
		if(m_infoHeader.biClrUsed == 0)
			m_infoHeader.biClrUsed = 256;
		if(pPal != NULL) 
		{
			RGBQUAD rgbquad[256];
			memcpy(rgbquad,m_pCurBuf,m_infoHeader.biClrUsed*4);
			pPal->CreateIdPalette(rgbquad,m_infoHeader.biClrUsed);
		}
		m_pCurBuf += m_infoHeader.biClrUsed*4;
	}
	else
		pPal = NULL;

	CreateDIBSection(pDc,pPal,nLgWidth,-nLgHeight,m_infoHeader.biBitCount);	

	// CreateDibSection을 거치면서 m_infoHeader.biWidth는 물리적 좌표로 변환한다.
	int nPgHeight = abs(m_infoHeader.biHeight);
	int nPgWidth  = m_infoHeader.biWidth;
	nPgWidth *= bitCount;
	nLgWidth *= bitCount;
	nLgWidth += ((nLgWidth % 4) ? (4 - nLgWidth % 4) : 0);
	LPBYTE pTempDst = m_pData + (nPgWidth * (nPgHeight - 1));
	for(int i = 0; i < nPgHeight; i++)
	{
		memcpy(pTempDst,m_pCurBuf,nLgWidth);
		m_pCurBuf += nLgWidth;
		pTempDst -= nPgWidth;
	}
	FreeBuffer();
}
BOOL CBmp::DecodeBitmap(LPBYTE pDst,LPBYTE pSrc,int nAlign)
{
	return 1;
}
BOOL CBmp::GetPalette(LPRGBQUAD prgbquad)
{
	return TRUE;
}

#define _CLIENT_WIDTH  640
#define _CLIENT_HEIGHT 480

BOOL CBmp::Save(LPCTSTR lpszFileName,RGBQUAD* rgbQuad)
{
	int      i, j;
	BITMAPFILEHEADER		BMPheader;
	LPBYTE pBMP = NULL;
	LPBYTE pp,pp2;

	LPBYTE pSurface = (LPBYTE) m_pData;

	int nBit = m_infoHeader.biBitCount >> 3;

	BMPheader.bfType = 'M';
	BMPheader.bfType <<= 8;
	BMPheader.bfType += 'B';
	BMPheader.bfReserved1 = 0; 
	BMPheader.bfReserved2 = 0;
	// 16비트(nBit==2)라도 팔레트가 있는 것 처럼 안하면 그림이 밀려서 출력한다. 아케인 캡춰 관련 버그 
	if(nBit == 1 || nBit == 2)
	{
		BMPheader.bfSize = ( sizeof(BITMAPFILEHEADER ) + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 256 + m_infoHeader.biWidth * abs(m_infoHeader.biHeight) * nBit );
		BMPheader.bfOffBits = sizeof(BITMAPFILEHEADER ) + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 256;
	}
	else
	{
		BMPheader.bfSize = ( sizeof(BITMAPFILEHEADER ) + sizeof(BITMAPINFOHEADER) + m_infoHeader.biWidth * abs(m_infoHeader.biHeight) * nBit );
		BMPheader.bfOffBits = sizeof(BITMAPFILEHEADER ) + sizeof(BITMAPINFOHEADER);
	}
 	pBMP = new BYTE[BMPheader.bfSize];
	if( pBMP == NULL )	return	FALSE;
	pp = pBMP;

	m_infoHeader.biHeight = abs( m_infoHeader.biHeight);

	memcpy( pp, &BMPheader   , sizeof(BITMAPFILEHEADER) ); pp += sizeof(BITMAPFILEHEADER);
	memcpy( pp, &m_infoHeader, sizeof(BITMAPINFOHEADER) ); pp += sizeof(BITMAPINFOHEADER);

	if(nBit == 1 || nBit == 2)
	{
		if(rgbQuad)
			memcpy(pp,rgbQuad,sizeof(RGBQUAD)*256); 
		pp += (sizeof(RGBQUAD) * 256);
	}

	j = m_infoHeader.biWidth * abs(m_infoHeader.biHeight) * nBit;


	for( i = 0; i < abs(m_infoHeader.biHeight); i ++ )
	{
		j  -= m_infoHeader.biWidth * nBit;
		memcpy( pp, pSurface + j, m_infoHeader.biWidth * nBit );		
		pp += m_infoHeader.biWidth * nBit;
	}
	//memset(pp2,0xff,800*2);
	//memset(pp2+800*2,0xff,800*2);
	
	CFile file;
	if(file.Open(lpszFileName,CFile::modeWrite|CFile::modeCreate))
	{
		file.Write(pBMP,(int)(pp - pBMP));
		safe_delete( pBMP );
		return TRUE;
	}
	safe_delete( pBMP );
	return FALSE;
}
