#ifndef __CFileIO_H
#define __CFileIO_H

#include <TCHAR.H>
#include <stdio.h>
#include <io.h>


class CFileIO
{
public:
	FILE* fp;

	CFileIO( LPCTSTR lpszFileName, TCHAR* mode ) { Open( lpszFileName, mode ); }
	CFileIO() { fp = NULL; } 
	
	virtual ~CFileIO() { Close(); }
	virtual BOOL  Open( LPCTSTR lpszFileName, TCHAR* mode );
	virtual BOOL Close();
	virtual int	 Flush()  { return fflush( fp ); }
	virtual char  GetC()  { return (char)getc( fp ); }
	virtual WORD  GetW()  { return getwc( fp ); } 
	virtual DWORD GetDW() { DWORD dw; fread( &dw, sizeof( dw ), 1, fp ); return dw; }
	virtual long Tell()   { return ftell(fp); }
	virtual int  Seek( long offset, int whence ) { return fseek( fp, offset, whence ); }
	virtual long GetLength() { return _filelength( Handle() ); }
	virtual size_t Read( void *ptr, size_t size, size_t n = 1 )	{ return fread( ptr, size, n, fp ); }
	virtual LPVOID Read();

	int  Handle() { return fileno( fp ); }
	int  Error()  { return ferror( fp ); }
	int  Eof() { return feof( fp ); }
	int  PutC( char c ) { return putc( c, fp ); }
	int  PutW( WORD w ) { return putw( w, fp ); }
	int  PutDW( DWORD dw ) { return fwrite( &dw, sizeof( dw ), 1, fp ); }
	size_t Write( LPVOID ptr, size_t size, size_t n = 1 ) { return fwrite( ptr, size, n, fp ); }
	int PutString( LPCTSTR lpszString ) { return _ftprintf( fp, lpszString ); }
	int PutWideString( LPCTSTR lpszString );
};

#if defined( __CLIENT )
	struct RESOURCE
	{
		TCHAR szResourceFile[ 128 ];
		DWORD dwOffset;
		DWORD dwFileSize;
		BYTE  byEncryptionKey;
		bool  bEncryption;
	}; 
	class CResFile : public CFileIO
	{
	public:
		CResFile( LPCTSTR lpszFileName, TCHAR *mode );
		CResFile();

	private:
		CFile m_File;
		bool m_bResouceInFile;
		char m_szFileName[_MAX_FNAME];
		DWORD m_nFileSize;
		DWORD m_nFileBeginPosition; // 시작 위치
		DWORD m_nFileCurrentPosition; // 현재 위치
		DWORD m_nFileEndPosition; // 끝 위치
		bool m_bEncryption;
		BYTE m_byEncryptionKey;
		static void AddResource( TCHAR* lpResName );

	public:

#ifdef __SECURITY_0628
		static	char	m_szResVer[100];
		static	map<string, string>	m_mapAuth;
		static	void	LoadAuthFile( void );
#endif	// __SECURITY_0628

		static CMapStringToPtr m_mapResource;

		static BYTE Encryption( BYTE byEncryptionKey, BYTE byData )
		{
			byData = ( byData << 4 ) | ( byData >> 4 );
			return ( ~byData ) ^ byEncryptionKey;
		}
		static BYTE Decryption( BYTE byEncryptionKey, BYTE byData )
		{
			byData = ~byData ^ byEncryptionKey;
			return ( byData << 4 ) | ( byData >> 4 );
		}

		BOOL IsEncryption() { return m_bEncryption; }
		virtual ~CResFile() { Close(); }
		virtual BOOL Open( LPCTSTR lpszFileName, TCHAR *mode );
		virtual BOOL Close( void );
		
		virtual LPVOID Read();
		virtual size_t Read( void *ptr, size_t size, size_t n = 1 );
		virtual long GetLength();
		virtual int  Seek( long offset, int whence );
		virtual long Tell();
		
		virtual char  GetC();
		virtual WORD  GetW();
		virtual DWORD GetDW();
		
		virtual int	 Flush();
		
		//BOOL FindFile(char *szSerchPath, LPCTSTR lpszFileName, TCHAR *mode, int f );
		//BOOL FindFileFromResource( char *filepath, LPCTSTR lpszFileName );

		static void ScanResource( LPCTSTR lpszRootPath );
		static void FreeResource();
	};
#else
	// 클라이언트가 아니면 
	#define CResFile CFileIO
#endif


class CFileFinder
{
	POSITION m_pos;
	long m_lHandle;
	CHAR m_szFilespec[ MAX_PATH ];
	BOOL m_bResFile;
public:
	CFileFinder();
	~CFileFinder();
	BOOL WildCmp( LPCTSTR lpszWild, LPCTSTR lpszString );

	BOOL FindFirst( LPCTSTR lpFilespec, struct _finddata_t *fileinfo );
	BOOL FindNext( struct _finddata_t *fileinfo );
	void FindClose();
};


#endif
