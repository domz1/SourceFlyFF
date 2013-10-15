#include "stdafx.h"
#include "WebBox.h"
#define WININC
#include "resource.h"
#include "dpcertified.h"
#include "tools.h"

/////////////////////////////////////////////////////////////////////////////////////////
// gloabl variable
/////////////////////////////////////////////////////////////////////////////////////////

#define	WEB_HEADER	"Content-Type: application/x-www-form-urlencoded\r\nContent-length: %d\r\n"

LPTSTR WEB_ADDRESS_DEFAULT	= NULL;
LPTSTR WEB_POSTDATA = NULL;
static LPCTSTR WEB_ADDRESS_QOODO	= "www.omg.com.tw/qoodo02/default.aspx";
static LPCTSTR WEB_POSTDATA_QOODO	= "account=%s&check=%s";

#ifdef __EVENT_1101
	#ifdef __EVENT_1101_2
		static LPCTSTR WEB_ADDRESS_CALLTHEROLL	= "http://www.flyff.com/event/20070718_event_popup_attandance_character.asp";
//		static LPCTSTR WEB_ADDRESS_CALLTHEROLL	= "http://renewal.flyff.com/event/20070718_event_popup_attandance_character.asp";
	#else // __EVENT_1101_2
		static LPCTSTR WEB_ADDRESS_CALLTHEROLL	= "http://renewal.flyff.com/event/20061114_event_popup_attend.asp";
	#endif // __EVENT_1101_2
static LPCTSTR WEB_POSTDATA_CALLTHEROLL	= "CName=%s&dwEventFlag=%s";
#endif	// __EVENT_1101
/////////////////////////////////////////////////////////////////////////////////////////
// global function
/////////////////////////////////////////////////////////////////////////////////////////

BOOL	CALLBACK	WebDlgProc( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam );

void InitWebGlobalVar()
{
	switch( ::GetLanguage() )
	{
	case LANG_JAP:
		WEB_ADDRESS_DEFAULT = "http://www.flyff.jp/market/in_client/index.asp";
		WEB_POSTDATA = "user_id=%s&m_idPlayer=%d&server_index=%d&md5=%s";
		break;
	case LANG_TWN:
		WEB_ADDRESS_DEFAULT	= "http://bill4.omg.com.tw/charge/flyffitemlist.asp";
		WEB_POSTDATA = "server_index=%d&m_idPlayer=%d&user_id=%s&m_szName=%s&md5=%s";
		break;
	case LANG_THA:
		WEB_ADDRESS_DEFAULT	= "http://bill.ini3.co.th/Charge/Flyff/ItemShopMain.asp";
//		WEB_POSTDATA = "user_id=%s&m_idPlayer=%d&server_index=%d&md5=%s";
		WEB_POSTDATA	= "user_id=%s&m_idPlayer=%d&server_index=%d&md5=%s&check=%s";
		break;
	case LANG_KOR:
		#if defined( __MAINSERVER )
//			WEB_ADDRESS_DEFAULT	= "http://billflex.flyff.com/itemshop_main_main.asp";
			WEB_ADDRESS_DEFAULT	= "http://billflex.flyff.com/checkid.aspx";
			//WEB_ADDRESS_DEFAULT	= "http://119.207.77.51/checkid.aspx";
		#elif defined( __TESTSERVER )
			WEB_ADDRESS_DEFAULT	= "http://itemdev.flyff.com/itemshop_main_main.asp";
		#elif defined( __INTERNALSERVER )
			WEB_ADDRESS_DEFAULT	= "http://itemdev.flyff.com/itemshop_main_main.asp";
		#endif	// __TESTSERVER
		WEB_POSTDATA = "user_id=%s&m_idPlayer=%d&server_index=%d&m_nLevel=%d&m_nJob=%d&m_dwSex=%d&m_szName=%s";
		break;
#ifdef __NO_SUB_LANG
	case LANG_USA:
		{
			WEB_ADDRESS_DEFAULT = "http://billing.gpotato.com/Charge/FlyffItemList.asp";
		}
		WEB_POSTDATA = "user_id=%s&m_idPlayer=%d&server_index=%d&md5=%s&check=%s";
		break;
#else // __NO_SUB_LANG
	case LANG_USA:
		if( ::GetSubLanguage() == LANG_SUB_PHP )
		{
			WEB_ADDRESS_DEFAULT = "http://bill.netgames.ph/Charge/Flyff/ItemShopMain.asp";
//			WEB_ADDRESS_DEFAULT		= "https://my.levelupgames.ph/GameWeb/Charge/Flyff/ItemShopMain.asp";
		}
		else if( ::GetSubLanguage() == LANG_SUB_IND )
		{
			WEB_ADDRESS_DEFAULT = "about:blank";
		}
		else 
		{
			WEB_ADDRESS_DEFAULT = "http://billing.gpotato.com/Charge/FlyffItemList.asp";
		}
		WEB_POSTDATA = "user_id=%s&m_idPlayer=%d&server_index=%d&md5=%s&check=%s";
		break;
#endif // __NO_SUB_LANG
	case LANG_GER:
		WEB_ADDRESS_DEFAULT = "http://billing.gpotato.eu/Charge/FlyffItemList.asp";
//		WEB_ADDRESS_DEFAULT = "about:blank";
		WEB_POSTDATA = "user_id=%s&m_idPlayer=%d&server_index=%d&md5=%s&check=%s";
		break;
	case LANG_SPA:
		//	mulcom	BEGIN100318	ƒ•∑π æ∆¿Ã≈€º• URL ∫Ø∞Ê
		WEB_ADDRESS_DEFAULT = "http://billing.es.gpotato.com/charge/FlyffItemList.asp";
		//	mulcom	END100318	ƒ•∑π æ∆¿Ã≈€º• URL ∫Ø∞Ê
		WEB_POSTDATA = "user_id=%s&m_idPlayer=%d&server_index=%d&md5=%s&check=%s";
		break;
	case LANG_POR:
		WEB_ADDRESS_DEFAULT = "http://billing.gpotato.com.br/Charge/FlyffItemList.asp";
		WEB_POSTDATA = "user_id=%s&m_idPlayer=%d&server_index=%d&md5=%s&check=%s";
		break;
	case LANG_FRE:
		WEB_ADDRESS_DEFAULT	= "http://billing.gpotato.eu/Charge/FlyffItemList.asp";
		WEB_POSTDATA = "user_id=%s&m_idPlayer=%d&server_index=%d&md5=%s&check=%s&language=FR";
//		WEB_ADDRESS_DEFAULT = "about:blank";
//		WEB_POSTDATA = "user_id=%s&m_idPlayer=%d&server_index=%d&md5=%s&check=%s";
		break;
	case LANG_HK:
		WEB_ADDRESS_DEFAULT = "http://flyff.urgame.com.hk/cashitemshop/";
		WEB_POSTDATA = "user_id=%s&m_idPlayer=%d&server_index=%d&md5=%s&check=%s";
		break;
	case LANG_VTN:
		WEB_ADDRESS_DEFAULT = "http://shop.flyff.vn/Default.aspx";
		WEB_POSTDATA = "user_id=%s&m_idPlayer=%d&server_index=%d&md5=%s&check=%s";
		break;
	case LANG_RUS:
		WEB_ADDRESS_DEFAULT = "http://www.flyffgame.ru/client/mall/";
		WEB_POSTDATA = "user_id=%s&m_idPlayer=%d&server_index=%d&md5=%s&check=%s";
		break;
	}
}

CWebBox::CWebBox()
{
	m_bStart	= false;
	m_hParent	= NULL;
	m_hDlg	= NULL;
	m_hDlgBox	= NULL;
//	m_hSubWindow	= NULL ; 
	m_hInst		= NULL;
	ZeroMemory( &m_WebRect, sizeof(RECT) );
	m_AddressName	= NULL;
	m_bShow		= FALSE;
	m_bEnd	= false;
	m_bStartWeb		= false;
//	m_bLClick	= false;
//	m_bRefresh	= false;
//	m_Status	= WEB_STATUS_STORE;
//	m_WebTex	= NULL;
//	m_WebCount	= FALSE;
//	m_hbWebBMP	= NULL;
//	m_SurfaceSize.cx	= 0;
//	m_SurfaceSize.cy	= 0;
//	m_bNonGDI	= FALSE;
//	m_nStartWebCount	= 0;
//	m_BackColor	= BACK_IMAGE_INIT_COLOR;
//	m_bSmallSize	= false;
	ZeroMemory( m_szUser, sizeof(m_szUser) );
	m_nPlayer	= m_nServer		= 0;
	m_nLevel	= m_nJob	= m_nSex	= 0;
	ZeroMemory( m_szName, sizeof(m_szName) );
	m_bItemShop		= TRUE;

#ifdef __EVENT_1101
#ifdef __EVENT_1101_2
	m_nEventFlag = 0;
#else // __EVENT_1101_2
	m_dwEventFlag	= 0;
#endif // __EVENT_1101_2
#endif	// __EVENT_1101
}

CWebBox::~CWebBox()
{
	Destroy();
//	m_BackTex.Destroy();
//	m_BackImage.Destroy();
//	m_BackTex_Store.Destroy();
//	m_BackImage_Store.Destroy();
}

/*
static HBITMAP	CreateDibBMP(HDC hdc, int Width,int Height, int bitCount)
{
	LPVOID					lpBits;
	struct			
	{
		BITMAPINFOHEADER	bi;
		DWORD				ct[256];
	} dib;

	dib.bi.biSize = sizeof(BITMAPINFOHEADER);
	dib.bi.biWidth = Width;
	dib.bi.biHeight = Height;
	dib.bi.biBitCount = bitCount;
	dib.bi.biPlanes = 1;
	dib.bi.biCompression = 0;
	dib.bi.biSizeImage = 0;
	dib.bi.biClrUsed = 0;

	switch(bitCount)
	{
	case 16:
		dib.bi.biCompression =BI_BITFIELDS;
		dib.ct[0] = 0xF800;
		dib.ct[1] = 0x07E0;
		dib.ct[2] = 0x001F;
		break;
	case 15:
		dib.bi.biBitCount = 16;
		break;
	}
	return CreateDIBSection(hdc, (LPBITMAPINFO) & dib, DIB_RGB_COLORS, &lpBits,
                            NULL, 0);
}
*/

void CWebBox::Init( HINSTANCE hInst, HWND hWnd, char * WebAddress, RECT WebRect )
{
	m_hParent	= hWnd;
	m_hInst		= hInst;
	m_AddressName	= WebAddress;
	m_WebRect	= WebRect;
//	m_bNonGDI	= TRUE;

//	LoadSkin("testskin.bmp");
}

/*
bool CWebBox::Invali()
{
	RECT rc;
	GetWindowRect( m_hDlgBox, &rc );
	::InvalidateRect( m_hDlgBox, &rc, TRUE );
	return TRUE;
}
*/

HWND CWebBox::Begin(BOOL StaticContent)
{
	if( m_hDlgBox != NULL)	return NULL;
	else
	{
		/*
		m_hParentDC	= GetDC( m_hParent );
		m_hBackDC	= CreateCompatibleDC( m_hParentDC );
		m_Bitmap	= CreateCompatibleBitmap( m_hParentDC, g_nGameWidth, g_nGameHeight );
	
		m_Oldbmp	= SelectObject( m_hBackDC, m_Bitmap );
		BitBlt( m_hBackDC, 0, 0, g_nGameWidth, g_nGameHeight, m_hParentDC, 0, 0, SRCCOPY );			

		WNDCLASS wc;
	    HINSTANCE hinst	= GetModuleHandle( NULL );
		memset( &wc, 0, sizeof(wc) );
//		wc.style	= CS_DBLCLKS | CS_GLOBALCLASS | CS_NOCLOSE | CS_OWNDC;
		wc.style	= CS_HREDRAW | CS_VREDRAW |CS_DBLCLKS;
		wc.lpfnWndProc	= SubWindowProc;
		wc.hInstance	= hinst;
		wc.hCursor	= NULL;
		wc.lpszClassName	= "SubWindow";
		RegisterClass( &wc );
   
		m_hSubWindow	= CreateWindowEx( 0, "SubWindow", "SubWindow", WS_CHILDWINDOW,
		0, 0, g_nGameWidth, g_nGameHeight, m_hParent, NULL, m_hInst, NULL);
		ShowWindow( m_hSubWindow, SW_SHOW );
		
		HDC subdc	= GetDC( m_hSubWindow );
		UpDate( subdc );
		*/
		HINSTANCE	hInst	= GetModuleHandle( NULL );
		m_hDlg	= CreateDialog( hInst, MAKEINTRESOURCE( IDD_WEBDLG )
												, m_hParent, WebDlgProc );
//		ShowWindow( m_hDlg, SW_SHOW );

		m_hDlgBox	= CreateWindow( WC_WEBCTRL, m_AddressName, WS_EX_TOPMOST | WS_CHILDWINDOW
									,m_WebRect.left, m_WebRect.top, m_WebRect.right, m_WebRect.bottom
									,m_hDlg, NULL, m_hInst, NULL );
	}

	if( m_hDlgBox == NULL)
		return NULL;

	return m_hDlgBox;
}

void CWebBox::Show( BOOL type )
{
	if( type )
	{
		ShowWindow( m_hDlgBox, SW_SHOW );
		SetFocus( m_hDlgBox );
		m_bStart	= true;
	}
	else
	{
		ShowWindow( m_hDlgBox, SW_HIDE );
		SetFocus( m_hParent );
		m_bStart	= false;
	}
}

void CWebBox::Destroy()
{	
	if(m_hDlgBox)
	{	
		DestroyWindow( m_hDlgBox );
		m_hDlgBox	= NULL;
	}

	if( m_hDlg )
	{
		DestroyWindow( m_hDlg );
		m_hDlg	= NULL;
	}
}

BOOL CWebBox::ChangeWebAddress(char* address , char *postdata , char *header)
{
	SendMessage( m_hDlgBox, WEB_CTRL_ADDRESS, NULL, (LPARAM)address );
	SendMessage( m_hDlgBox, WEB_CTRL_POST, NULL, (LPARAM)postdata );
	SendMessage( m_hDlgBox, WEB_CTRL_HEADER, NULL, (LPARAM)header );
	SendMessage( m_hDlgBox, WEB_CTRL_NAVIGATE, NULL, NULL );

	return TRUE;
}

void CWebBox::Close()
{
	Destroy();
	m_bEnd	= true;
	m_bStart	= false;
	m_bStartWeb		= false;
}

/*
void CWebBox::Encrypt(char* original, int seed, char* encrypt)
{
	int pos = 0;
	int szpos = 0;
	char m_id[256];
	unsigned long part;
	unsigned long origin;
	static unsigned char nts[256];
	int length;
	
	memset(m_id, '\0', sizeof(m_id));
	strcpy(m_id, original);
	m_id[strlen(m_id)] = '\0';
	length =strlen(m_id);

	while (pos < length) {
		origin = *(unsigned long*)(m_id+pos);
		origin = origin + seed;
		part = shifter(origin,true);
		LongToNts(part,nts+szpos);
		pos += 4;
		szpos+=7;
	}
	strcpy(encrypt,(char*)nts);
	return;
}

unsigned char* CWebBox::Decyrpt(unsigned char* original,int seed)
{
	int pos = 0;
	int szpos = 0;
	unsigned long part;
	unsigned long origin;
	unsigned static char nts[256];
	int length;

	memset(nts,0,sizeof(nts));
	length =strlen((char*)original);
	while (szpos < length) {
		NtsToLong(original+szpos,origin);
		part = shifter(origin,false);
		part = part -seed;
		memcpy(nts+pos,(char*)&part,4);
		pos+=4;
		szpos+=7;
	}
	return nts;
}

unsigned long CWebBox::shifter(unsigned long i, bool bForward)
{
	int shiftTable[32]={ 26,31,17,10,30,16,24, 2,29, 8,20,15,28,11,13, 4,19,23, 0,12,14,27, 6,18,21, 3, 9, 7,22, 1,25, 5};
	int reverseTable[32]={ 18,29,07,25,15,31,22,27, 9,26, 3,13,19,14,20,11, 5, 2,23,16,10,24,28,17, 6,30, 0,21,12, 8, 4, 1};

	unsigned long result=0;
	int pos = 0;
	int value;
	while (i>0) {
		value = i - ((i >> 1) << 1);
		i = i >> 1;
		if (value) {
			if (bForward)
			{	result += value << shiftTable[pos];
			}
			else 
			{	result += value << reverseTable[pos];
			}
		}
		pos++;
	}
	return result;
}

void CWebBox::LongToNts(unsigned long nValue, unsigned char *szValue)
{
	unsigned char charTable[37]={ "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"};
	int pos=0;
	int pV;
	while (pos <7)
	{
		  pV = nValue - nValue / 36*36;
		  szValue[pos]=charTable[pV];
		  pos++;
		  nValue = nValue/36;
	}
	szValue[pos]='\0';
}

void CWebBox::NtsToLong(unsigned char* szValue,unsigned long& nValue)
{
	int pos=0;
	int posV;
	int charV;
	nValue = 0;
	while(pos<7) {
		posV=1;
		for (int i=0;i<pos;i++)
			posV=posV*36;
		
		if (szValue[pos]>='0' && szValue[pos]<='9')
			charV=szValue[pos]-'0';
		else
			charV=szValue[pos]-'A'+10;
		nValue =nValue+charV*posV;
		pos++;
	}
}		
*/

void CWebBox::Set_Rect( RECT rc )
{
	SetWindowPos( m_hDlgBox, HWND_TOPMOST, rc.left, rc.top, rc.right, rc.bottom, SWP_HIDEWINDOW );
}

/*
BOOL CWebBox::InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;   

   hWnd = CreateWindow("", "", WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}
*/

bool CWebBox::Start_WebBox( HWND hwnd, HINSTANCE hinstance, float x, float y, char *address )
{
//	InitWebCtrl();	
//	int xsize =0, ysize=0 ; 
//	D3DXVECTOR2 stsize , stinc , vi; 
	
//	if(x == 800 && y == 600)
//		m_bSmallSize = true ; 
/*
	if(m_Status == WEB_STATUS_BLUG)
	{
		xsize = (x - WEBPAGE_DEFAULT_X)/2 + WEB_PAGE_INC_X; 
		ysize = (y - WEBPAGE_DEFAULT_Y)/2 + WEB_PAGE_INC_Y; 
	}
//	else if(m_Status == WEB_STATUS_STORE)
	{
*/
//		if(!m_bSmallSize)
//		{		
//			xsize = (int)( (x - STORE_DEFAULT_X)/2 + STORE_PAGE_INC_X ); 
//			ysize = (int)( (y - STORE_DEFAULT_Y)/2 + STORE_PAGE_INC_Y ); 
//		}
//		else
//		{
//			xsize = (x - 800)/2 + STORE_PAGE_INC_X; 
//			ysize = (y - 600)/2 + STORE_PAGE_INC_Y; 
//		}

//	}
	RECT rc	= {	-1, 0, WEB_DEFAULT_X+5, WEB_DEFAULT_Y	};
//	rc.left = xsize ; 
//	rc.top = ysize ; 
/*
	if(m_Status == WEB_STATUS_BLUG)
	{
		rc.right = WEBPAGE_DEFAULT_X ; 
		rc.bottom = WEBPAGE_DEFAULT_Y ; 
	}
	else
	{
		if(!m_bSmallSize)
		{
			rc.right = STORE_DEFAULT_X ; 
			rc.bottom = STORE_DEFAULT_Y ;
		}
		else
		{
			rc.right = 800 ; 
			rc.bottom = 600-STORE_PAGE_INC_Y;
		}
	}
*/
//	rc.right	= 800;
//	rc.bottom	= 600;

	Init(hinstance,hwnd, NULL, rc);	
	Destroy();
	m_AddressName = address;
//	InitWebCtrl();	
	Begin() ;		
	Show(FALSE) ; 

	return true; 
}
bool CWebBox::End_WebBox() 
{
	Show( FALSE );
	Close();
	UninitWebCtrl();
	SetFocus( m_hParent );
	return true; 
}

/*
void CWebBox::Create(float x, float y)
{
//	m_BackImagePos.x = (x - WEB_BACK_IMAGE_X)/2 ; 
//	m_BackImagePos.y = (y - WEB_BACK_IMAGE_Y)/2 ; 

//	m_BackTex.CreateTextureToTex(g_Info,".\\Texture\\WebImage\\webblog.tex",".\\Texture\\WebImage\\") ; 
//	m_BackImage.Create(&m_BackTex,"Webblog0000.MAK",12,LEFT_TOP|DRAW_MATRIX,4) ; 
	DWORD color = D3DCOLOR_ARGB(m_BackColor,255,255,255) ; 	
//	m_BackImage.SetPos(m_BackImagePos) ; 
//	m_BackImage.SetVertexData(color) ; 


	if(g_nGameWidth == 800)
		m_bSmallSize = true ; 

	if(!m_bSmallSize)
	{
//		m_BackImagePos_Store.x = (x - STORE_BACK_IMAGE_X)/2 ; 
//		m_BackImagePos_Store.y = (y - STORE_BACK_IMAGE_Y)/2 ; 
	}
	else
	{
//		m_BackImagePos_Store.x = 0 ;
//		m_BackImagePos_Store.y = 0 ;

	}

//	m_BackTex_Store.CreateTextureToTex(g_Info,".\\Texture\\WebImage\\MallTex.TEX",".\\Texture\\WebImage\\") ; 
//	m_BackImage_Store.Create(&m_BackTex_Store,"mall0000.MAK",12,LEFT_TOP|DRAW_MATRIX,4) ; 
//	m_BackImage_Store.SetPos(m_BackImagePos_Store) ; 
//	m_BackImage_Store.SetVertexData(color) ; 
}

void CWebBox::Render_BackImage()
{
	if(m_bStart)
	{
		m_BackColor += BACK_IMAGE_INC_COLOR ; 
		if(m_BackColor >= 255)
			m_BackColor = 255 ; 

		DWORD color = D3DCOLOR_ARGB(m_BackColor,255,255,255) ; 

		if(m_Status == WEB_STATUS_BLUG)
		{
		//	m_BackImage.SetVertexData(color) ; 
			
		//	m_BackImage.SetPos(m_BackImagePos) ; 
		//	m_BackImage.Draw(); 
		}
		else
		{
			if(m_bSmallSize)
			{
				D3DXVECTOR2 vi ; 
				vi.x = 800.f / 1024.f ; vi.y = 600.f / 768.f ; 
		//		m_BackImage_Store.SetScale(vi) ; 
			}
		//	m_BackImage_Store.SetVertexData(color) ; 			
		//	m_BackImage_Store.SetPos(m_BackImagePos_Store) ; 
		//	m_BackImage_Store.Draw()  ; 
		}

		if(m_nStartWebCount > BACK_IMAGE_COUNT)
		{
			m_nStartWebCount = 0 ; 
			m_BackColor      = BACK_IMAGE_INIT_COLOR ; 
			m_bStartWeb = true  ; 	
		}
		else
			m_nStartWebCount ++ ; 
		
	}
}
*/

void CWebBox::Refresh_Web()
{
	char address[512], postdata[WEB_STR_LEN], header[WEB_STR_LEN];

	ZeroMemory( address, 512 );
//	wsprintf( address, WEB_ADDRESS_DEFAULT );
	wsprintf( address, GetAddress() );
#ifdef __NO_SUB_LANG
	if( ::GetLanguage() == LANG_JAP || ::GetLanguage() == LANG_THA || ::GetLanguage() == LANG_USA || GetLanguage() == LANG_TWN || ::GetLanguage() == LANG_GER || ::GetLanguage() == LANG_SPA || ::GetLanguage() == LANG_FRE || GetLanguage() == LANG_HK || ::GetLanguage() == LANG_POR || ::GetLanguage() == LANG_VTN || ::GetLanguage() == LANG_RUS )
#else // __NO_SUB_LANG
	if( ::GetLanguage() == LANG_JAP || ::GetLanguage() == LANG_THA || ::GetLanguage() == LANG_USA || GetLanguage() == LANG_TWN || ::GetLanguage() == LANG_GER || ::GetLanguage() == LANG_SPA || ::GetLanguage() == LANG_FRE || GetLanguage() == LANG_HK || ::GetLanguage() == LANG_POR || ::GetLanguage() == LANG_VTN || ::GetLanguage() == LANG_RUS )
#endif // __NO_SUB_LANG
	{
		char out[260]	= { 0, };
		char in[260]	= { 0, };
		if( ::GetLanguage() == LANG_JAP || ::GetLanguage() == LANG_THA )
		{
			sprintf( in, "%s%d%dBlzq88rBc", m_szUser, m_nPlayer, m_nServer );
		}
		else if( GetLanguage() == LANG_TWN )
		{
			if( m_bItemShop )
				sprintf( in, "%d%d%s%saozmfhdnpf", m_nServer, m_nPlayer, m_szUser, g_pPlayer->GetName() );
			else
				sprintf( in, "6936%s5874", m_szUser );
		}
		else if( GetLanguage() == LANG_GER )
		{
			sprintf( in, "%s%d%dgjgusdbfjq", m_szUser, m_nPlayer, m_nServer );
		}
		else if( GetLanguage() == LANG_SPA )
		{
			sprintf( in, "%s%d%drlawnsgud", m_szUser, m_nPlayer, m_nServer );
		}
		else if( GetLanguage() == LANG_POR )
		{
			sprintf( in, "%s%d%drlawnsgud", m_szUser, m_nPlayer, m_nServer );
		}
		else if( GetLanguage() == LANG_HK )
		{
			sprintf( in, "%s%d%dtpgk", m_szUser, m_nPlayer, m_nServer );
		}
		else if( GetLanguage() == LANG_FRE )
		{
			sprintf( in, "%s%d%dwjdgPdnjs", m_szUser, m_nPlayer, m_nServer );
		}
		else if( GetLanguage() == LANG_VTN )
		{
			sprintf( in, "%s%d%ddlrjsdn", m_szUser, m_nPlayer, m_nServer );
		}
		else if( GetLanguage() == LANG_RUS )
		{
			sprintf( in, "%s%d%drlaansrms", m_szUser, m_nPlayer, m_nServer );
		}
#ifdef __NO_SUB_LANG
		else if( GetLanguage() == LANG_PHP )
		{
			sprintf( in, "%s%d%dthdgustjr", m_szUser, m_nPlayer, m_nServer );
		}
		else if( GetLanguage() == LANG_USA )
		{
			sprintf( in, "%s%d%ddbstkddl", m_szUser, m_nPlayer, m_nServer );
		}
		else
		{
			sprintf( in, "%s%d%ddldhsdlseh", m_szUser, m_nPlayer, m_nServer );
		}		
#else // __NO_SUB_LANG
		else 
		{
			if( ::GetSubLanguage() == LANG_SUB_PHP ) 
				sprintf( in, "%s%d%dthdgustjr", m_szUser, m_nPlayer, m_nServer );
			else if( ::GetSubLanguage() == LANG_SUB_USA )
				sprintf( in, "%s%d%ddbstkddl", m_szUser, m_nPlayer, m_nServer );
			else if( ::GetSubLanguage() == LANG_SUB_IND )
			{
				sprintf( in, "%s%d%ddldhsdlseh", m_szUser, m_nPlayer, m_nServer );
			}
		}
#endif // __NO_SUB_LANG

		::md5( out, in );
#ifdef __NO_SUB_LANG
		if( ::GetLanguage() == LANG_USA || ::GetLanguage() == LANG_SPA || ::GetLanguage() == LANG_THA ||::GetLanguage() == LANG_HK || ::GetLanguage() == LANG_POR || ::GetLanguage() == LANG_VTN || ::GetLanguage() == LANG_RUS )
#else // __NO_SUB_LANG
		if( ::GetLanguage() == LANG_USA || ::GetLanguage() == LANG_SPA || ::GetLanguage() == LANG_THA ||::GetLanguage() == LANG_HK || ::GetLanguage() == LANG_POR || ::GetLanguage() == LANG_VTN || ::GetLanguage() == LANG_RUS )
#endif // __NO_SUB_LANG
		{
			wsprintf( postdata, WEB_POSTDATA, m_szUser, m_nPlayer, m_nServer, out, g_Neuz.m_szPassword );
		}
		else if( ::GetLanguage() == LANG_GER || ::GetLanguage() == LANG_FRE )
		{
			#ifdef __GPAUTH_02
			wsprintf( postdata, WEB_POSTDATA, m_szUser, m_nPlayer, m_nServer, out, g_Neuz.m_szCheck );
//			OutputDebugString( postdata );
			#else	// __GPAUTH_02
			wsprintf( postdata, WEB_POSTDATA, m_szUser, m_nPlayer, m_nServer, out, g_Neuz.m_szPassword );
			#endif	// __GPAUTH_02
		}
		else if( GetLanguage() == LANG_TWN )
		{
			if( m_bItemShop )
				wsprintf( postdata, WEB_POSTDATA, m_nServer, m_nPlayer, m_szUser, g_pPlayer->GetName(), out );
			else
				wsprintf( postdata, WEB_POSTDATA_QOODO, m_szUser, out );
		}
		else
		{
			wsprintf( postdata, WEB_POSTDATA, m_szUser, m_nPlayer, m_nServer, out );
		}
	}
	else if( ::GetLanguage() == LANG_KOR )
	{
#ifdef __EVENT_1101
		if( m_bItemShop )
		{
			wsprintf( postdata, WEB_POSTDATA, m_szUser, m_nPlayer, m_nServer, m_nLevel, m_nJob, m_nSex, m_szName );
		}
		else
		{
			char szEventFlag[128]	= { 0,};
#ifdef __EVENT_1101_2
			szEventFlag[0] = '0';
			for( int i = 0; i < 63; i++ )
			{
				if( m_nEventFlag & (__int64)0x4000000000000000 >> i )
					lstrcat( szEventFlag, "1" );
				else
					lstrcat( szEventFlag, "0" );
			}
#else // __EVENT_1101_2
			for( int i = 0; i < 32; i++ )
			{
				if( m_dwEventFlag & 0x80000000 >> i )
					lstrcat( szEventFlag, "1" );
				else
					lstrcat( szEventFlag, "0" );
			}
#endif // __EVENT_1101_2

			wsprintf( postdata, WEB_POSTDATA_CALLTHEROLL, m_szName, szEventFlag );
		}
#else	// __EVENT_1101
		wsprintf( postdata, WEB_POSTDATA, m_szUser, m_nPlayer, m_nServer, m_nLevel, m_nJob, m_nSex, m_szName );
#endif	// __EVENT_1101
	}
	else
	{
		wsprintf( postdata, WEB_POSTDATA, m_szUser, m_nPlayer, m_nServer );
	}
	wsprintf( header, WEB_HEADER, lstrlen( postdata ) );
	ChangeWebAddress( address, postdata, header );
}

bool CWebBox::Process(HWND hWnd,HINSTANCE hInstance, char* szUser, u_long nPlayer, DWORD nServer, int nLevel, int nJob, int nSex, const char* szName )
{
	char address[512], postdata[WEB_STR_LEN], header[WEB_STR_LEN];
	ZeroMemory( address, 512 );
	ZeroMemory( postdata, WEB_STR_LEN );
	ZeroMemory( header, WEB_STR_LEN );

	if( m_bStart && m_bStartWeb )
	{
		lstrcpy( m_szUser, szUser );
		m_nPlayer	= nPlayer;
		m_nServer	= nServer;
		m_nLevel	= nLevel;
		m_nJob	= nJob;
		m_nSex	= nSex;
		lstrcpy( m_szName, szName );

		D3DDEVICE->SetDialogBoxMode( TRUE );
		Start_WebBox( hWnd, hInstance, WEB_DEFAULT_X, WEB_DEFAULT_Y, NULL );

//		wsprintf( address, WEB_ADDRESS_DEFAULT );
		wsprintf( address, GetAddress() );
#ifdef __NO_SUB_LANG
		if( ::GetLanguage() == LANG_JAP || ::GetLanguage() == LANG_THA || ::GetLanguage() == LANG_USA || GetLanguage() == LANG_TWN || ::GetLanguage() == LANG_GER || ::GetLanguage() == LANG_SPA || ::GetLanguage() == LANG_FRE || GetLanguage() == LANG_HK || ::GetLanguage() == LANG_POR || ::GetLanguage() == LANG_VTN || ::GetLanguage() == LANG_RUS )
#else // __NO_SUB_LANG
		if( ::GetLanguage() == LANG_JAP || ::GetLanguage() == LANG_THA || ::GetLanguage() == LANG_USA || GetLanguage() == LANG_TWN || ::GetLanguage() == LANG_GER || ::GetLanguage() == LANG_SPA || ::GetLanguage() == LANG_FRE || GetLanguage() == LANG_HK || ::GetLanguage() == LANG_POR || ::GetLanguage() == LANG_VTN || ::GetLanguage() == LANG_RUS )
#endif // __NO_SUB_LANG
		{
			char out[260]	= { 0, };
			char in[260]	= { 0, };
			if( ::GetLanguage() == LANG_JAP || ::GetLanguage() == LANG_THA )
			{
				sprintf( in, "%s%d%dBlzq88rBc", m_szUser, m_nPlayer, m_nServer );
			}
			else if( GetLanguage() == LANG_TWN )
			{
				if( m_bItemShop )
					sprintf( in, "%d%d%s%saozmfhdnpf", m_nServer, m_nPlayer, m_szUser, g_pPlayer->GetName() );
				else
					sprintf( in, "6936%s5874", m_szUser );
			}
			else if( ::GetLanguage() == LANG_GER )
			{
				sprintf( in, "%s%d%dgjgusdbfjq", m_szUser, m_nPlayer, m_nServer );
			}
			else if( ::GetLanguage() == LANG_SPA )
			{
				sprintf( in, "%s%d%drlawnsgud", m_szUser, m_nPlayer, m_nServer );
			}
			else if( ::GetLanguage() == LANG_POR )
			{
				sprintf( in, "%s%d%drlawnsgud", m_szUser, m_nPlayer, m_nServer );
			}
			else if( ::GetLanguage() == LANG_FRE )
			{
				sprintf( in, "%s%d%dwjdgPdnjs", m_szUser, m_nPlayer, m_nServer );
			}
			else if( ::GetLanguage() == LANG_HK )
			{
				sprintf( in, "%s%d%dtpgk", m_szUser, m_nPlayer, m_nServer );
			}
#ifdef __NO_SUB_LANG
			else if( ::GetLanguage() == LANG_USA )
			{
				sprintf( in, "%s%d%ddbstkddl", m_szUser, m_nPlayer, m_nServer );
			}
			else if( ::GetLanguage() == LANG_PHP )
			{
				sprintf( in, "%s%d%dthdgustjr", m_szUser, m_nPlayer, m_nServer );
			}
			else if( ::GetLanguage() == LANG_ID )
			{
				sprintf( in, "%s%d%ddldhsdlseh", m_szUser, m_nPlayer, m_nServer );
			}
#else // __NO_SUB_LANG
			else if( ::GetLanguage() == LANG_USA )
			{
				if( ::GetSubLanguage() == LANG_SUB_PHP )
					sprintf( in, "%s%d%dthdgustjr", m_szUser, m_nPlayer, m_nServer );
				else if(::GetSubLanguage() == LANG_SUB_USA )
					sprintf( in, "%s%d%ddbstkddl", m_szUser, m_nPlayer, m_nServer );
				else if(::GetSubLanguage() == LANG_SUB_IND )
				{
					sprintf( in, "%s%d%ddldhsdlseh", m_szUser, m_nPlayer, m_nServer );
				}
			}
#endif // __NO_SUB_LANG
			else if( ::GetLanguage() == LANG_VTN )
			{
				sprintf( in, "%s%d%ddlrjsdn", m_szUser, m_nPlayer, m_nServer );
			}
			else if( ::GetLanguage() == LANG_RUS )
			{
				sprintf( in, "%s%d%drlaansrms", m_szUser, m_nPlayer, m_nServer );
			}
			
			::md5( out, in );
#ifdef __NO_SUB_LANG
			if( ::GetLanguage() == LANG_USA || ::GetLanguage() == LANG_SPA || ::GetLanguage() == LANG_THA || ::GetLanguage() == LANG_HK || ::GetLanguage() == LANG_POR || ::GetLanguage() == LANG_VTN || ::GetLanguage() == LANG_RUS )
#else // __NO_SUB_LANG
			if( ::GetLanguage() == LANG_USA || ::GetLanguage() == LANG_SPA || ::GetLanguage() == LANG_THA || ::GetLanguage() == LANG_HK || ::GetLanguage() == LANG_POR || ::GetLanguage() == LANG_VTN || ::GetLanguage() == LANG_RUS )
#endif // __NO_SUB_LANG
			{
				wsprintf( postdata, WEB_POSTDATA, m_szUser, m_nPlayer, m_nServer, out, g_Neuz.m_szPassword );
			}
			else if( ::GetLanguage() == LANG_GER || ::GetLanguage() == LANG_FRE )
			{
				#ifdef __GPAUTH_02
				wsprintf( postdata, WEB_POSTDATA, m_szUser, m_nPlayer, m_nServer, out, g_Neuz.m_szCheck );
//				OutputDebugString( postdata );
				#else	// __GPAUTH_02
				wsprintf( postdata, WEB_POSTDATA, m_szUser, m_nPlayer, m_nServer, out, g_Neuz.m_szPassword );
				#endif	// __GPAUTH_02
			}
			else if( GetLanguage() == LANG_TWN )
			{
				if( m_bItemShop )
					wsprintf( postdata, WEB_POSTDATA, m_nServer, m_nPlayer, m_szUser, g_pPlayer->GetName(), out );
				else
					wsprintf( postdata, WEB_POSTDATA_QOODO, m_szUser, out );
			}
			else
			{
				wsprintf( postdata, WEB_POSTDATA, m_szUser, m_nPlayer, m_nServer, out );
			}
		}
		else if( ::GetLanguage() == LANG_KOR )
		{
#ifdef __EVENT_1101
		if( m_bItemShop )
		{
			wsprintf( postdata, WEB_POSTDATA, m_szUser, m_nPlayer, m_nServer, m_nLevel, m_nJob, m_nSex, m_szName );
		}
		else
		{
			char szEventFlag[128]	= { 0,};
#ifdef __EVENT_1101_2
			szEventFlag[0] = '0';
			for( int i = 0; i < 63; i++ )
			{
				if( m_nEventFlag & (__int64)0x4000000000000000 >> i  )
					lstrcat( szEventFlag, "1" );
				else
					lstrcat( szEventFlag, "0" );
			}
#else // __EVENT_1101_2
			for( int i = 0; i < 32; i++ )
			{
				if( m_dwEventFlag & 0x80000000 >> i )
					lstrcat( szEventFlag, "1" );
				else
					lstrcat( szEventFlag, "0" );
			}
#endif // __EVENT_1101_2
			wsprintf( postdata, WEB_POSTDATA_CALLTHEROLL, m_szName, szEventFlag );
		}
#else	// __EVENT_1101
		wsprintf( postdata, WEB_POSTDATA, m_szUser, m_nPlayer, m_nServer, m_nLevel, m_nJob, m_nSex, m_szName );
#endif	// __EVENT_1101
		}
		else
		{
			wsprintf( postdata, WEB_POSTDATA, m_szUser, m_nPlayer, m_nServer );
		}

		wsprintf( header, WEB_HEADER, lstrlen( postdata ) );
		ChangeWebAddress( address, postdata, header );
		Show( TRUE );
		m_bStart	= false;
		m_bEnd	= false;
		return true;
	}
	else if( m_bEnd )
	{
		End_WebBox();
		m_bEnd	= false;
		m_bStart	= false;
		m_bStartWeb	= false;
		return false;
	}
	else if( m_bStartWeb )
	{
		if( GetAsyncKeyState( VK_F5 ) )
			Refresh_Web();
	}

	return false;
}

bool CWebBox::Get_Start()
{
	if( m_bStartWeb )
		return true;
	return false;
}

void CWebBox::Set_Start()
{
//	m_hBackDC	= NULL;
//	m_hParentDC	= NULL;
//	m_Bitmap	= NULL;
	m_bStart	= true;
	m_bEnd	= false;
	m_bStartWeb		= true;
//	m_bStartWeb		= false;
}

void CWebBox::Set_End()
{
	if( m_bStartWeb )
	{
		m_bEnd	= true;
		m_bStart	= false;
		m_bStartWeb		= false;
		ShowWindow( m_hDlg, SW_HIDE );
	}
}

void CWebBox::UpDate( HDC hdc )
{	
//	HDC subdc	= GetDC( m_hSubWindow );
//	BitBlt( hdc, 0, 0, g_nGameWidth, g_nGameHeight, m_hBackDC, 0, 0, SRCCOPY );
//	ReleaseDC( m_hSubWindow, hdc );
}

/*
LRESULT CALLBACK SubWindowProc( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam )
{	
	CWebBox* pwb	= CWebBox::GetInstance();
	switch ( Msg )
	{
		case WM_PAINT:
			{
				PAINTSTRUCT ps;
				ps.rcPaint	= pwb->Get_WebRect();
				HDC hdc	= BeginPaint( hWnd, &ps );
				pwb->UpDate( hdc );
				EndPaint( hWnd, &ps );
			}
			break;  
		case WM_LBUTTONDOWN:
			pwb->m_bLClick	= true;
			break;
	}
	
	return DefWindowProc( hWnd, Msg, wParam, lParam );
}
*/	

BOOL	CALLBACK	WebDlgProc( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam )
{
	CWebBox* pWebBox	= CWebBox::GetInstance();

	switch( Msg )
	{
		case WM_INITDIALOG:
			{
				CRect rcCenter;
				GetWindowRect( pWebBox->GetParent(), &rcCenter );
				
				int xLeft = (rcCenter.left + rcCenter.right) / 2 - WEB_DEFAULT_X / 2;
				int yTop = (rcCenter.top + rcCenter.bottom) / 2 - WEB_DEFAULT_Y / 2;

				DWORD CapH = 0;
				
				if(  g_Neuz.IsWindow() )
					CapH = GetSystemMetrics( SM_CYCAPTION );
				else
					CapH = 9;


				if( g_Neuz.m_bFakeFullMode )
				{
					if( g_Option.m_bStartFullScreen )
						CapH = 5;
					else
						CapH = 20;				
				}

				SetWindowPos( hWnd, NULL
										, xLeft, yTop+CapH, WEB_DEFAULT_X, WEB_DEFAULT_Y
										, SWP_NOZORDER | SWP_NOACTIVATE );
				break;
			}
		case WM_COMMAND:
			{
				switch( wParam )
				{
					case IDCANCEL:
						CWebBox::GetInstance()->Set_End();
						break;
				}
				break;
			}
/*
		case WM_ERASEBKGND:
			{
				CDC pDC;
				HDC hdc;
				hdc = GetDC(CWebBox::GetInstance()->m_hDlg);
				pDC.FromHandle( hdc );
				pDC.Attach(hdc);

				CWebBox::GetInstance()->OnEraseBkgnd(&pDC);
				break;
			}
*/
			
	}
	return FALSE;
}

LPCTSTR CWebBox::GetAddress( void )
{
	if( GetLanguage() == LANG_TWN && m_bItemShop == FALSE )
		return WEB_ADDRESS_QOODO;
#ifdef __EVENT_1101
	else if( GetLanguage() == LANG_KOR && m_bItemShop == FALSE )
		return WEB_ADDRESS_CALLTHEROLL;
#endif	// __EVENT_1101
	return WEB_ADDRESS_DEFAULT;
}

#ifdef __YWEBSKIN
void CWebBox::LoadSkin(char *filename)
{
	HBITMAP hBitmap;
	
    hBitmap = (HBITMAP)LoadImage(NULL, filename, IMAGE_BITMAP, 0,0, LR_LOADFROMFILE | LR_CREATEDIBSECTION | LR_DEFAULTSIZE);
	
	m_bmpSkin = CBitmap::FromHandle(hBitmap);          
	
	

	//m_bmpSkin.LoadBitmap( filename );

	BITMAP bmp;
	m_bmpSkin->GetBitmap( &bmp );
	
	::SetWindowPos( m_hDlg, NULL, 0, 0, bmp.bmWidth, bmp.bmHeight, SWP_NOZORDER );
	BitmapRgn( filename, RGB( 0,0,0 ) );
}

BOOL CWebBox::BitmapRgn( char *filename, COLORREF TansColor )
{
	HBITMAP			m_hBack;
	m_hBack = (HBITMAP)LoadBitmap(m_hInst, filename );
	::SetWindowRgn( m_hDlg, BitmapToRegion( m_hBack, TansColor, TansColor ),TRUE);
	
	return TRUE;
}

HRGN CWebBox::BitmapToRegion(HBITMAP hBmp, COLORREF cTransparentColor/* = 0*/, COLORREF cTolerance/* = 0x101010*/)
{
	HRGN hRgn = NULL;

	if (hBmp)
	{
		// Create a memory DC inside which we will scan the bitmap content
		HDC hMemDC = CreateCompatibleDC(NULL);
		if (hMemDC)
		{
			// Get bitmap size
			BITMAP bm;
			GetObject(hBmp, sizeof(bm), &bm);

			// Create a 32 bits depth bitmap and select it into the memory DC 
			BITMAPINFOHEADER RGB32BITSBITMAPINFO = {	
					sizeof(BITMAPINFOHEADER),	// biSize 
					bm.bmWidth,					// biWidth; 
					bm.bmHeight,				// biHeight; 
					1,							// biPlanes; 
					32,							// biBitCount 
					BI_RGB,						// biCompression; 
					0,							// biSizeImage; 
					0,							// biXPelsPerMeter; 
					0,							// biYPelsPerMeter; 
					0,							// biClrUsed; 
					0							// biClrImportant; 
			};
			VOID * pbits32; 
			HBITMAP hbm32 = CreateDIBSection(hMemDC, (BITMAPINFO *)&RGB32BITSBITMAPINFO, DIB_RGB_COLORS, &pbits32, NULL, 0);
			if (hbm32)
			{
				HBITMAP holdBmp = (HBITMAP)SelectObject(hMemDC, hbm32);

				// Create a DC just to copy the bitmap into the memory DC
				HDC hDC = CreateCompatibleDC(hMemDC);
				if (hDC)
				{
					// Get how many bytes per row we have for the bitmap bits (rounded up to 32 bits)
					BITMAP bm32;
					GetObject(hbm32, sizeof(bm32), &bm32);
					while (bm32.bmWidthBytes % 4)
						bm32.bmWidthBytes++;

					// Copy the bitmap into the memory DC
					HBITMAP holdBmp = (HBITMAP)SelectObject(hDC, hBmp);
					BitBlt(hMemDC, 0, 0, bm.bmWidth, bm.bmHeight, hDC, 0, 0, SRCCOPY);

					// For better performances, we will use the ExtCreateRegion() function to create the
					// region. This function take a RGNDATA structure on entry. We will add rectangles by
					// amount of ALLOC_UNIT number in this structure.
					#define ALLOC_UNIT	100
					DWORD maxRects = ALLOC_UNIT;
					HANDLE hData = GlobalAlloc(GMEM_MOVEABLE, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects));
					RGNDATA *pData = (RGNDATA *)GlobalLock(hData);
					pData->rdh.dwSize = sizeof(RGNDATAHEADER);
					pData->rdh.iType = RDH_RECTANGLES;
					pData->rdh.nCount = pData->rdh.nRgnSize = 0;
					SetRect(&pData->rdh.rcBound, MAXLONG, MAXLONG, 0, 0);

					// Keep on hand highest and lowest values for the "transparent" pixels
					BYTE lr = GetRValue(cTransparentColor);
					BYTE lg = GetGValue(cTransparentColor);
					BYTE lb = GetBValue(cTransparentColor);
					BYTE hr = min(0xff, lr + GetRValue(cTolerance));
					BYTE hg = min(0xff, lg + GetGValue(cTolerance));
					BYTE hb = min(0xff, lb + GetBValue(cTolerance));

					// Scan each bitmap row from bottom to top (the bitmap is inverted vertically)
					BYTE *p32 = (BYTE *)bm32.bmBits + (bm32.bmHeight - 1) * bm32.bmWidthBytes;
					for (int y = 0; y < bm.bmHeight; y++)
					{
						// Scan each bitmap pixel from left to right
						for (int x = 0; x < bm.bmWidth; x++)
						{
							// Search for a continuous range of "non transparent pixels"
							int x0 = x;
							LONG *p = (LONG *)p32 + x;
							while (x < bm.bmWidth)
							{
								BYTE b = GetRValue(*p);
								if (b >= lr && b <= hr)
								{
									b = GetGValue(*p);
									if (b >= lg && b <= hg)
									{
										b = GetBValue(*p);
										if (b >= lb && b <= hb)
											// This pixel is "transparent"
											break;
									}
								}
								p++;
								x++;
							}

							if (x > x0)
							{
								// Add the pixels (x0, y) to (x, y+1) as a new rectangle in the region
								if (pData->rdh.nCount >= maxRects)
								{
									GlobalUnlock(hData);
									maxRects += ALLOC_UNIT;
									hData = GlobalReAlloc(hData, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects), GMEM_MOVEABLE);
									pData = (RGNDATA *)GlobalLock(hData);
								}
								RECT *pr = (RECT *)&pData->Buffer;
								SetRect(&pr[pData->rdh.nCount], x0, y, x, y+1);
								if (x0 < pData->rdh.rcBound.left)
									pData->rdh.rcBound.left = x0;
								if (y < pData->rdh.rcBound.top)
									pData->rdh.rcBound.top = y;
								if (x > pData->rdh.rcBound.right)
									pData->rdh.rcBound.right = x;
								if (y+1 > pData->rdh.rcBound.bottom)
									pData->rdh.rcBound.bottom = y+1;
								pData->rdh.nCount++;

								// On Windows98, ExtCreateRegion() may fail if the number of rectangles is too
								// large (ie: > 4000). Therefore, we have to create the region by multiple steps.
								if (pData->rdh.nCount == 2000)
								{
									HRGN h = ExtCreateRegion(NULL, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects), pData);
									if (hRgn)
									{
										CombineRgn(hRgn, hRgn, h, RGN_OR);
										DeleteObject(h);
									}
									else
										hRgn = h;
									pData->rdh.nCount = 0;
									SetRect(&pData->rdh.rcBound, MAXLONG, MAXLONG, 0, 0);
								}
							}
						}

						// Go to next row (remember, the bitmap is inverted vertically)
						p32 -= bm32.bmWidthBytes;
					}

					// Create or extend the region with the remaining rectangles
					HRGN h = ExtCreateRegion(NULL, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects), pData);
					if (hRgn)
					{
						CombineRgn(hRgn, hRgn, h, RGN_OR);
						DeleteObject(h);
					}
					else
						hRgn = h;

					// Clean up
					GlobalFree(hData);
					SelectObject(hDC, holdBmp);
					DeleteDC(hDC);
				}

				DeleteObject(SelectObject(hMemDC, holdBmp));
			}

			DeleteDC(hMemDC);
		}	
	}

	return hRgn;
}

BOOL CWebBox::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	CRect rc;
	GetClientRect(m_hDlg, &rc);
	
	CDC MemDC;	
	MemDC.CreateCompatibleDC(pDC);
	
	CBitmap* pbmpOld = MemDC.SelectObject( m_bmpSkin );		
	pDC->BitBlt(0, 0, rc.right, rc.bottom, &MemDC, 0, 0, SRCCOPY);
	MemDC.SelectObject( pbmpOld );
	ReleaseDC(m_hDlg, pDC->GetSafeHdc());
	
	return TRUE;
}

#endif //__YWEBSKIN

CWebBox* CWebBox::GetInstance( void )
{
	static CWebBox	sWebBox;
	return &sWebBox;
}