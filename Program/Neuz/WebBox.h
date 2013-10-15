#ifndef __WEBBOX_H__
#define __WEBBOX_H__


#include "WebCtrl.h"

//#define  WEBPAGE_DEFAULT_X	790
//#define  WEBPAGE_DEFAULT_Y	570

#define  WEB_DEFAULT_X	790
#define  WEB_DEFAULT_Y	570

#define  WEB_STATUS_STORE	0x02
 
void InitWebGlobalVar();

class CWebBox
{
public:
	CWebBox();
	virtual ~CWebBox();
public:
//	bool	Invali( void ) ; 
	void	Init( HINSTANCE hInst, HWND hWnd, char * WebAddress, RECT WebRect );
	HWND	Begin( BOOL StaticContent = NULL );
	void	Close( void );
//	void	Encrypt( char* original, int seed, char* encrypt );
//	unsigned char*	Decyrpt( unsigned char* original, int seed );
	void	Destroy();	
	BOOL	ChangeWebAddress( char* address, char *postdata = NULL, char *header = NULL );
	BOOL	IsActive( void )	{ return( m_hDlgBox != NULL );	}
//	BOOL	IsStatic()	{	return( m_hbWebBMP != NULL );	}
	void	SetWebStatus( DWORD status )	{	m_Status	= status;	}

//	unsigned long	shifter( unsigned long i, bool bForward );
//	void LongToNts( unsigned long nValue, unsigned char *szValue );
//	void NtsToLong( unsigned char* szValue,unsigned long& nValue );
	void	Show( BOOL type );
//	bool CopyTexture( HDC *pDC );
//	void ShowWeb() ; 
//	void Draw() ; 
	void	Set_Rect( RECT rc );
	void	UpDate( HDC dc );
//	BOOL	InitInstance( HINSTANCE hInstance, int nCmdShow );
	static	CWebBox*	GetInstance( void );
private:
	BOOL	m_bItemShop;
public:
	void	SetItemShop( BOOL bItemShop )	{	m_bItemShop	= bItemShop;	}
	LPCTSTR	GetAddress( void );
public:
	/*
	CSprite2 m_BackImage ; 
	BPTexture m_BackTex  ; 
	D3DXVECTOR3 m_BackImagePos ; 


	CSprite2 m_BackImage_Store ; 
	BPTexture m_BackTex_Store  ; 
	D3DXVECTOR3 m_BackImagePos_Store ; //*/

//	void Create(float x, float y ) ; 
//	void Render_BackImage() ; 

	HWND		m_hDlgBox;
//	HWND		m_hSubWindow ; 
	HWND		m_hDlg;
//	bool        m_bLClick    ; 	
//	bool        m_bRefresh   ; 
	HWND	GetParent( void )	{	return m_hParent;	}

#ifdef __EVENT_1101
#ifdef __EVENT_1101_2
	__int64 m_nEventFlag;
#else // __EVENT_1101_2
	DWORD	m_dwEventFlag;
#endif // __EVENT_1101_2
#endif	// __EVENT_1101

#ifdef __YWEBSKIN
	CBitmap* m_bmpSkin;

	void LoadSkin( char* filename );
	BOOL BitmapRgn( char *filename, COLORREF TansColor );
	HRGN BitmapToRegion(HBITMAP hBmp, COLORREF cTransparentColor/* = 0*/, COLORREF cTolerance/* = 0x101010*/);
	BOOL OnEraseBkgnd(CDC* pDC);
		
#endif //__YWEBSKIN
	
private:
	
	RECT	    m_WebRect;	
	HINSTANCE	m_hInst;
	HWND		m_hParent;
//	HBITMAP		m_hbWebBMP;
	char*		m_AddressName;
//	SIZE	    m_SurfaceSize;
//	BOOL	    m_bNonGDI;
	
	bool        m_bShow  ; 
//	bool        m_WebCount ; 

	DWORD       m_Status ; 

	//          시작관련 
	bool        m_bStartWeb  ; 
	bool        m_bStart ;
	bool        m_bEnd   ;
//	int			m_nStartWebCount ; 
//	short       m_BackColor     ; 

//	char        m_U_ID[128]  ; 
//	char        m_U_PW[128]  ; 

	char	m_szUser[128];
	int		m_nPlayer;
	int		m_nServer;
	int		m_nLevel;
	int		m_nJob;
	int		m_nSex;
	char	m_szName[128];

//	HDC         m_hBackDC    ; 
//	HDC         m_hParentDC  ; 
//	HGDIOBJ		m_Bitmap  ,m_Oldbmp   ; 

//	bool        m_bSmallSize ; 


public:
	RECT Get_WebRect(){return m_WebRect ; }
	bool Get_Start() ; 
	void Set_Start() ; 
	void Set_End()   ; 
	bool Get_Show(){return m_bShow ;}
	void Set_Show(bool type){m_bShow = type ; }		
//	BOOL Get_WebFlag(){return m_WebCount ; }
	BOOL Get_WebPlay(){return m_bStart ;}	
//	void Ch_WebFlag(){m_WebCount = !m_WebCount ;}
	bool Start_WebBox(HWND hwnd,HINSTANCE hinstance,float x, float y , char *address) ;
	bool End_WebBox()  ;
	bool Process(HWND hWnd,HINSTANCE hInstance, char* user_id, u_long player_id, DWORD server_index, int nLevel, int nJob, int nSex, const char* szName );
	void Refresh_Web() ; 
public:
//	LPDIRECT3DTEXTURE8  m_WebTex ; 
};

#endif	// __WEBBOX_H__
