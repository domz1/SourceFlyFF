#ifndef __WIRTEAVI
#define __WIRTEAVI
#include <vfw.h>

#define TEXT_HEIGHT	20
#define AVIIF_KEYFRAME	0x00000010L // this frame is a key frame.
#define BUFSIZE 260

#define CDibPalette void

#ifdef __WRITEAVI
/////////////////////////////////////////////////////////////////////////////
// Class Name : CDibBitmap
// Maker      : Vampyre
// Date       : 
// Remarks    :
//   장치 독립적인 비트맵 클레스 
//
class CDibBitmap : public CBitmap
{
protected:
	BITMAPINFOHEADER m_infoHeader;
	LPBYTE m_pData; // 메모리 포인터 
	int m_nWidth ; // 비트맵 사이즈는 정렬 방식에 따라서 4의 배수로 세팅된다. 
	int m_nHeight; // 비트맵 사이즈는 정렬 방식에 따라서 4의 배수로 세팅된다. 

public:
	CDibBitmap() { m_pData = 0; }
	~CDibBitmap() {	DeleteObject(); }
	void   CreateDIBSection(CDC* pDC, CDibPalette* pPal,int nWidth,int nHeight,WORD nBitCount = 8);
	CSize  GetSize      () { return CSize(m_infoHeader.biWidth,-m_infoHeader.biHeight); }
	LPBYTE GetData      () { return m_pData; }
	int    GetWidth     () { return m_infoHeader.biWidth      ; } // 4배수의 진짜 이미지 사이즈 
	int    GetHeight    () { return abs(m_infoHeader.biHeight); } // 진짜 이미지 사이즈 
	int    GetPgWidth   () { return m_nWidth ; } // 실제 입력한 사이즈 
	int    GetPgHeight  () { return m_nHeight; } // 실제 입력한 사이즈 
	WORD   GetBitCount  () { return m_infoHeader.biBitCount; }
virtual	BOOL Load(LPCSTR lpszFileName) { return TRUE; }
virtual void CreateDib(CDC* pDc,CDibPalette* pPal) { };
};

class CAVIFile : public CObject
{
public:
	CAVIFile(LPCTSTR lpszFileName, int xdim = -1, int ydim = -1);
	virtual ~CAVIFile();

	virtual bool AddFrame(CBitmap& bmp);
	CString GetFName() const             {return FName;};
	virtual bool IsOK() const            {return bOK;};

private:
	CString FName;
	int xDim;
	int yDim;

	AVISTREAMINFO strhdr;
	PAVIFILE pfile;
	PAVISTREAM ps;
	PAVISTREAM psCompressed;
	PAVISTREAM psText;
	AVICOMPRESSOPTIONS opts;
	AVICOMPRESSOPTIONS FAR * aopts[1];
	DWORD dwTextFormat;
	char szText[BUFSIZE];
	int nFrames;
	bool bOK;
};
#endif // __WRITEAVI
#endif