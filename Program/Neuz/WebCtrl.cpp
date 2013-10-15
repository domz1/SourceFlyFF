// WebCtrl.cpp: implementation of the CWebCtrl class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "resdata.h"
#include "CNTREVNT.H"
#include "WebCtrl.h"

#define WININC
#include <EXDISPID.H>
#include <ATLBASE.H> 
#include <ATLCONV.H>

#include "webbox.h"

#ifdef __NEW_WEB_BOX
#include "HelperWebBox.h"
#endif // __NEW_WEB_BOX



#define THIS_PROP   "this"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CWebContainer::CWebContainer( HWND hwnd )
{
   m_cRef = 0;
   m_hWnd = hwnd;
   m_punk = NULL;

   SetRectEmpty( &m_rect );

   m_pEvent = new CWebEventSink();

   m_pEvent->AddRef();
}

CWebContainer::~CWebContainer()
{
   m_pEvent->Release();

   m_punk->Release();
}

void CWebContainer::add()
{
   CLSID clsid;

   CLSIDFromString( L"Shell.Explorer", &clsid );

   CoCreateInstance( clsid, NULL, CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER, IID_IUnknown, (PVOID *) &m_punk );

   IOleObject *pioo;

   m_punk->QueryInterface( IID_IOleObject, (PVOID *) &pioo );

   pioo->SetClientSite( this );

   pioo->Release();

   IPersistStreamInit *ppsi;

   m_punk->QueryInterface( IID_IPersistStreamInit, (PVOID *) &ppsi );

   ppsi->InitNew();
   ppsi->Release();

   ConnectEvents();

   setVisible( true );

   setFocus( true );
}

void CWebContainer::remove()
{
   IOleObject *pioo;
   
   m_punk->QueryInterface( IID_IOleObject, (PVOID *) &pioo );

   pioo->Close( OLECLOSE_NOSAVE );

   pioo->SetClientSite( NULL );

   pioo->Release();

   IOleInPlaceObject *pipo;

   m_punk->QueryInterface( IID_IOleInPlaceObject, (PVOID *) &pipo );

   pipo->UIDeactivate();

   pipo->InPlaceDeactivate();

   pipo->Release();

   DisconnectEvents();
}

void CWebContainer::setLocation( int x, int y, int cx, int cy )
{
   m_rect.left   = x;
   m_rect.top    = y;
   m_rect.right  = cx;
   m_rect.bottom = cy;

   IOleInPlaceObject *pipo;

   m_punk->QueryInterface( IID_IOleInPlaceObject, (PVOID *) &pipo );

   pipo->SetObjectRects( &m_rect, &m_rect );

   pipo->Release();
}

void CWebContainer::setVisible( bool bVisible )
{
   IOleObject *pioo;

   m_punk->QueryInterface( IID_IOleObject, (PVOID *) &pioo );

   if ( bVisible )
   {
      pioo->DoVerb( OLEIVERB_INPLACEACTIVATE, NULL, this, 0, m_hWnd, &m_rect );
      pioo->DoVerb( OLEIVERB_SHOW, NULL, this, 0, m_hWnd, &m_rect );
   }
   else
      pioo->DoVerb( OLEIVERB_HIDE, NULL, this, 0, m_hWnd, NULL );

   pioo->Release();
}

void CWebContainer::setFocus( bool bFocus )
{
   IOleObject *pioo;

   if ( bFocus )
   {
      m_punk->QueryInterface( IID_IOleObject, (PVOID *) &pioo );

      pioo->DoVerb( OLEIVERB_UIACTIVATE, NULL, this, 0, m_hWnd, &m_rect );

      pioo->Release();
   }
}

void CWebContainer::ConnectEvents()
{
   IConnectionPoint *pcp = GetConnectionPoint( DIID_DWebBrowserEvents2 );

   pcp->Advise( m_pEvent, &m_eventCookie );

   pcp->Release();
}

void CWebContainer::DisconnectEvents()
{
   IConnectionPoint *pcp;

   pcp = GetConnectionPoint( DIID_DWebBrowserEvents2 );

   pcp->Unadvise( m_eventCookie );

   pcp->Release();
}

STDMETHODIMP CWebEventSink::Invoke(DISPID dispidMember, REFIID riid,
                                LCID lcid, WORD wFlags,
                                DISPPARAMS* pDispParams,
                                VARIANT* pvarResult,
                                EXCEPINFO*  pExcepInfo, UINT* puArgErr)
{
   USES_CONVERSION;
   
   if (!pDispParams)
      return E_INVALIDARG;

   switch (dispidMember)
   {
      // The parameters for this DISPID are as follows:
      // [0]: Cancel flag  - VT_BYREF|VT_BOOL
      // [1]: HTTP headers - VT_BYREF|VT_VARIANT
      // [2]: Address of HTTP POST data  - VT_BYREF|VT_VARIANT 
      // [3]: Target frame name - VT_BYREF|VT_VARIANT 
      // [4]: Option flags - VT_BYREF|VT_VARIANT
      // [5]: URL to navigate to - VT_BYREF|VT_VARIANT
      // [6]: An object that evaluates to the top-level or frame
      //      WebBrowser object corresponding to the event. 
      //
      case DISPID_BEFORENAVIGATE2:
         if (pDispParams->cArgs >= 5 &&
             pDispParams->rgvarg[5].vt == (VT_BYREF|VT_VARIANT))
         {
         /*   CComVariant varURL(*pDispParams->rgvarg[5].pvarVal);
            varURL.ChangeType(VT_BSTR);
			
			//OLE2T(varURL.bstrVal);

			char TempChar[256];


			
			//strcpy(TempChar, (char*)(LPSTR)OLE2T(varURL.bstrVal) );

			char temp[120];
			memset(temp, 0, 120);
			memcpy(temp, varURL.bstrVal, 60);
			for(int  i = 1; i < 60; i++)
				temp[ i ] = temp[ i * 2];
			int tempsize = strlen(temp);
            memset(&temp[ tempsize], 0, 120 - tempsize);

			
			if( !strcmp(TempChar, "loading_start.htm") )
				//g_MainGame->m_GameData->m_bLoadingView = false;*/
         }
         break;

      // The parameters for this DISPID:
      // [0]: URL navigated to - VT_BYREF|VT_VARIANT
      // [1]: An object that evaluates to the top-level or frame
      //      WebBrowser object corresponding to the event. 
      //
		case DISPID_NAVIGATECOMPLETE2:
			{
				if(g_WndMng.GetWndBase(APP_WEBBOX))
					g_WndMng.GetWndBase(APP_WEBBOX)->SetVisible(TRUE);
				if(g_WndMng.GetWndBase(APP_WEBBOX2))
						g_WndMng.GetWndBase(APP_WEBBOX2)->SetVisible(TRUE);
			
				ShowWindow( CWebBox::GetInstance()->m_hDlg, SW_SHOW );

#ifdef __NEW_WEB_BOX
				ShowWindow( CHelperWebBox::GetInstance()->GetDialogHandle(), SW_SHOW );
#endif // __NEW_WEB_BOX

				/*if (pDispParams->rgvarg[0].vt == (VT_BYREF|VT_VARIANT))
				{
				CComVariant varURL(*pDispParams->rgvarg[0].pvarVal);
				varURL.ChangeType(VT_BSTR);

				char TempChar[512];
				strcpy(TempChar,OLE2A((const unsigned short*)pDispParams->rgvarg[0].pvarVal));

				if( !strcmp(TempChar, "loading_end.htm") )
				g_MainGame->m_GameData->m_bLoadingView = true;
				}*/
				break;
			}
      default:
         break;
   }

   return S_OK;
}

IConnectionPoint* CWebContainer::GetConnectionPoint( REFIID riid )
{
   IConnectionPointContainer *pcpc;
   
   m_punk->QueryInterface( IID_IConnectionPointContainer, (PVOID *) &pcpc );

   IConnectionPoint *pcp;

   pcpc->FindConnectionPoint( riid, &pcp );

   pcpc->Release();

   return pcp;
}

HRESULT CWebContainer::getWeb( IWebBrowser2 **ppweb )
{
   return m_punk->QueryInterface( IID_IWebBrowser2, (PVOID *) ppweb );
}

CWebCtrl::CWebCtrl( HWND hwnd )
{
   m_hWnd = hwnd;
   m_cxScroll = GetSystemMetrics( SM_CXHSCROLL ) + 2;
   m_cyScroll = GetSystemMetrics( SM_CYVSCROLL ) + 2;
       
}



static LRESULT CALLBACK WebCtrlProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
   CWebCtrl *pthis = (CWebCtrl *) GetProp( hwnd, THIS_PROP );

   if ( !pthis )
   {
      if ( uMsg != WM_CREATE )
         return DefWindowProc( hwnd, uMsg, wParam, lParam );

      pthis = new CWebCtrl( hwnd );

      SetProp( hwnd, THIS_PROP, (HANDLE) pthis );
   }

   LRESULT lResult = pthis->WebCtrlProc( uMsg, wParam, lParam );

   if ( uMsg == WM_DESTROY )
   {
      RemoveProp( hwnd, THIS_PROP );
	  safe_delete( pthis );
   }

   return lResult;
}

void CWebCtrl::Navigate( char* szURL, char* szPostData, char* szHeaders )
{
	VARIANT *pvarURL, *pvarPostData, *pvarHeaders;
	VARIANT varURL, varPostData, varHeaders;

	pvarURL		= NULL;
	pvarPostData	= NULL;
	pvarHeaders		= NULL;

	if( !szURL )
		return;

	if( !*szURL )
		return;

	int len = lstrlen( szURL ) + 1;

	WCHAR *pszW;

#ifndef UNICODE

	pszW	= new WCHAR[len];

	MultiByteToWideCharEx( CP_ACP, 0, szURL, -1, pszW, len );

#else	// UNICODE

	pszW = szURL;

#endif	// UNICODE

	VariantInit( &varURL );

	varURL.vt	= VT_BSTR;
	varURL.bstrVal	= SysAllocString( pszW );

#ifndef UNICODE

	safe_delete_array( pszW );

#endif	// UNICODE

	pvarURL	= &varURL;

	len	= lstrlen ( szHeaders );
	if( 0 < len )
	{
		len++;

#ifndef UNICODE

		pszW	= new WCHAR[len];

		MultiByteToWideCharEx( CP_ACP, 0, szHeaders, -1, pszW, len );

#else	// UNICODE

		pszW	= szURL;

#endif	// UNICODE

		VariantInit( &varHeaders );
		varHeaders.vt	= VT_BSTR;
		varHeaders.bstrVal	= SysAllocString( pszW );

#ifndef UNICODE

		safe_delete_array( pszW );

#endif	// UNICODE

		pvarHeaders		= &varHeaders;
	}

	len	= lstrlen ( szPostData );
	if( 0 < len )
	{
		VariantInit( &varPostData );

		SAFEARRAYBOUND rgsabound;
		rgsabound.cElements		= len;
		rgsabound.lLbound	= 0;

		varPostData.parray	= ::SafeArrayCreate( VT_UI1, 1, &rgsabound );
		varPostData.vt	= unsigned short( VT_UI1 | VT_ARRAY );

		void* pvDestData;
		::SafeArrayAccessData( varPostData.parray, &pvDestData );
		memcpy( pvDestData, szPostData, len );
		::SafeArrayUnaccessData( varPostData.parray );

		pvarPostData	= &varPostData;
	}

	m_pweb->Navigate2( pvarURL, NULL, NULL, pvarPostData, pvarHeaders );

	VariantClear( pvarURL );

	if( pvarPostData )
		VariantClear( pvarPostData );

	if (pvarHeaders)
		VariantClear( pvarHeaders );

//	m_pweb->Navigate2( &v, NULL, NULL, NULL, NULL );
}

/*
void CWebCtrl::Navigate( LPTSTR psz )
{
   if ( !psz )
      return;

   if ( !*psz )
      return;

   int len = lstrlen( psz ) + 1;

   WCHAR *pszW;

#ifndef UNICODE

   pszW = new WCHAR[len];

   MultiByteToWideChar( CP_ACP, 0, psz, -1, pszW, len );

#else

   pszW = psz;

#endif

   VARIANT v;

   VariantInit( &v );

   v.vt = VT_BSTR;
   v.bstrVal = SysAllocString( pszW );

#ifndef UNICODE

   delete []pszW;

#endif

   m_pweb->Navigate2( &v, NULL, NULL, NULL, NULL );

   VariantClear( &v );
}
*/
LRESULT CWebCtrl::WebCtrlProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	LRESULT lResult = 0;

	switch ( uMsg )
	{
		case WEB_CTRL_ADDRESS:
			memset(g_address,0,WEB_STR_LEN);
			strcpy(g_address,(char*)lParam);
			break ; 
		case WEB_CTRL_POST:
			memset( g_postdata, 0, WEB_STR_LEN );
			strcpy( g_postdata,(char*)lParam );
			TRACE( "POST: %s\n", g_postdata );
//			{
//				char lpOutputString[100];
//				sprintf( lpOutputString, "POST: %s\n", g_postdata );
//				OutputDebugString( lpOutputString );
//			}
			break;
		case WEB_CTRL_HEADER:
			memset(g_header,0,WEB_STR_LEN);
			strcpy( g_header, (char*)lParam );
			TRACE( "HEADER: %s\n", g_header );
//			{
//				char lpOutputString[100];
//				sprintf( lpOutputString, "HEADER: %s\n", g_header );
//				OutputDebugString( lpOutputString );
//			}
			break;
		case WEB_CTRL_NAVIGATE:
			Navigate( g_address, g_postdata, g_header );
			break ; 
#ifdef __NEW_WEB_BOX
		case WEB_CTRL_GO_BACK:
			{
				m_pweb->GoBack();
				break;
			}
		case WEB_CTRL_GO_FORWARD:
			{
				m_pweb->GoForward();
				break;
			}
		case WEB_CTRL_STOP:
			{
				m_pweb->Stop();
				break;
			}
		case WEB_CTRL_REFRESH:
			{
				m_pweb->Refresh();
				break;
			}
#endif // __NEW_WEB_BOX
		case WM_CREATE:
			m_pContainer = new CWebContainer( m_hWnd );
			m_pContainer->AddRef();
			m_pContainer->add();
			m_pContainer->getWeb( &m_pweb );
			CREATESTRUCT *pcs;
			pcs = (CREATESTRUCT *) lParam;
			Navigate( (LPTSTR) pcs->lpszName );
			break;
		case WM_SETTEXT:
			Navigate( (LPTSTR) lParam );
			break;

		case WM_SIZE:

         m_pContainer->setLocation( -2, -2, LOWORD(lParam) + m_cxScroll, HIWORD(lParam) + m_cyScroll );

         break;
	  case WEB_CTRL_SHOW:
		 m_pContainer->setVisible( true );
		 break;
	  case WEB_CTRL_HIDE:
		 m_pContainer->setVisible( false );
		 break;

	case WM_DESTROY:

		m_pweb->Release();

		m_pContainer->remove();

		m_pContainer->Release();

		break;

	default:

		lResult = DefWindowProc( m_hWnd, uMsg, wParam, lParam );

		break;
	}

	return lResult;
}


bool WINAPI InitWebCtrl()
{
   CoInitialize( NULL );

   WNDCLASS wc;

   HINSTANCE hinst = GetModuleHandle( NULL );

   if ( !GetClassInfo( hinst, WC_WEBCTRL, &wc ) )
   {
      memset( &wc, 0, sizeof(wc) );

      wc.style         = CS_DBLCLKS | CS_GLOBALCLASS | CS_NOCLOSE | CS_OWNDC;
      wc.lpfnWndProc   = ::WebCtrlProc;
      wc.hInstance     = hinst;
      //wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
	  wc.hCursor       = LoadCursor( hinst, /*MAKEINTRESOURCE(IDC_CURSOR1)*/NULL );
      wc.lpszClassName = WC_WEBCTRL;

      return RegisterClass( &wc ) ? true : false;
   }
   
   return true;
}

bool WINAPI UninitWebCtrl()
{
   WNDCLASS wc;

   bool bResult = false;

   if ( GetClassInfo( GetModuleHandle( NULL ), WC_WEBCTRL, &wc ) )
      bResult = UnregisterClass( WC_WEBCTRL, wc.hInstance ) ? true : false;

   CoUninitialize();

   return bResult;
}

/*
HWND WINAPI CreateWebControl( LPCTSTR pszText, DWORD dwStyles, int x, int y, int cx, int cy, HWND hwndParent, UINT uId )
{
   return CreateWindow( WC_WEBCTRL, pszText, dwStyles, x, y, cx, cy, hwndParent, (HMENU) uId, GetModuleHandle( NULL ), NULL );
}
*/

static class AutoInit
{
public:

   AutoInit()
   {
      InitWebCtrl();
   }

   ~AutoInit()
   {
      UninitWebCtrl();
   }

} a;


