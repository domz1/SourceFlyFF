#ifndef __CBMP_H
#define __CBMP_H

#include "..\library\graphic.h"

class CBmp : public CDibBitmap
{
	DECLARE_SERIAL(CBmp)

	// Decoding

	BOOL GetPalette(LPRGBQUAD rgb);
	BOOL DecodeBitmap(LPBYTE pDst,LPBYTE pSrc,int nAlign);
	BOOL ReadChunk(CArchive& ar);

	LPBYTE m_pBuffer;
	LPBYTE m_pCurBuf;
	DWORD  m_nFileSize;

public:
	CBmp();
	~CBmp();
	void FreeBuffer();
	void Serialize(CArchive& ar);
	void CreateBmp(CDC* pDc,CDibPalette* pPal);
	BOOL Save(LPCTSTR lpszFileName,RGBQUAD* rgbQuad);
virtual	BOOL Load(LPCSTR lpszFileName);
virtual void CreateDib(CDC* pDc,CDibPalette* pPal)
	{ CreateBmp(pDc,pPal); }
};
#endif