#ifndef __TARGA_H
#define __TARGA_H

#pragma pack(1)
typedef struct _TgaHeader
{
  UCHAR			IdLength;            // Image ID Field Length
  UCHAR			CmapType;            // Color Map Type
  UCHAR			ImageType;           // Image Type

  DWORD		CmapIndex;           // First Entry Index
  DWORD		CmapLength;          // Color Map Length
  UCHAR			CmapEntrySize;       // Color Map Entry Size

  DWORD		X_Origin;            // X-origin of Image
  DWORD		Y_Origin;            // Y-origin of Image
  DWORD		ImageWidth;          // Image Width
  DWORD		ImageHeight;         // Image Height
  UCHAR			PixelDepth;          // Pixel Depth
  UCHAR			ImagDesc;            // Image Descriptor
} TGAHEADER;

// Definitions for image types.
#define TGA_Null 0
#define TGA_Map 1
#define TGA_RGB 2
#define TGA_Mono 3
#define TGA_RLEMap 9
#define TGA_RLERGB 10
#define TGA_RLEMono 11
#define TGA_CompMap 32
#define TGA_CompMap4 33

#pragma pack()

class CTarga
{
	TGAHEADER m_header;
	LPBYTE m_pData;

	// Decoding

	BOOL GetPalette(LPRGBQUAD rgb);
	BOOL DecodeBitmap(LPBYTE pDst,LPBYTE pSrc,int nAlign);
	BOOL ReadChunk(CArchive& ar);

	LPBYTE m_pBuf;
	DWORD  m_nFileSize;
	DWORD	 m_nTgaDataIdx; 
	int		 m_nReadPixelCount;

	//////////////////////////// targa original source

  void DoDecode( DWORD ** ppPal,int * pDestBPP,char * pDataSrc );
  // Fills TGAHEADER structure with the information in the file.
  void readTgaHeader(TGAHEADER * pTgaHead,char * pDataSrc);
  // Reads the TGA palette and creates a windows palette.
  void readPalette(int StartIndex,int Length,int EntrySize,DWORD ** ppPal,int DestBPP,char * pDataSrc);
  // Creates a standalone RGBA grayscale palette.
  void setGrayPalette( DWORD ** ppPal);
  // Determines compression type and calls readData.
  void readImage( TGAHEADER * pTgaHead,DWORD ** ppPal,int DestBPP,char * pDataSrc);
  // Reads image data line-by-line.
  void readData( TGAHEADER * pTgaHead,bool bCompressed,DWORD * pPal,int DestBPP,char * pDataSrc);
  // Decodes one line of uncompressed image data.
  void expandUncompressedLine( UCHAR * pDest,int Width,bool	bReversed,int	bpp,char * pDataSrc,DWORD * pPal,int DestBPP );
  // Decodes one line of compressed image data.
  void expandCompressedLine( UCHAR * pDest,int Width, bool	bReversed,int	bpp, char * pDataSrc,DWORD * pPal,int DestBPP);
  // Reads one image pixel and returns it in RGBA format.
  DWORD readPixel32( int bpp,char * pDataSrc,DWORD * pPal);
  // Reads one image pixel and returns it in 8-bit format.
  UCHAR readPixel8( int bpp,char * pDataSrc);

	////////////////////////////////////////////////////////////////
	void CreateTarga();// CDC* pDc,CDibPalette* pPal);

public:
	CTarga();
	~CTarga();
	void FreeBuffer();
	//void CreateTarga( CDC* pDc,CDibPalette* pPal);
	BOOL Load( LPCSTR lpszFileName, LPBYTE* lppData, SIZE* pSize );
};
#endif