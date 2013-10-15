#ifndef CNTREVNT_H
#define CNTREVNT_H

#include <shlobj.h>
#include <EXDISPID.H> 
#include <ATLBASE.H>

class CWebEventSink : public IDispatch
{
public:
 
   CWebEventSink()
   {
      m_cRef = 0;
   }

   // *** IUnknown ***
   STDMETHOD(QueryInterface)( REFIID riid, PVOID *ppv )
   {
      if ( IsEqualIID( riid, IID_IDispatch ) )
         *ppv = (IDispatch *) this;
      else if ( IsEqualIID( riid, IID_IUnknown ) )
         *ppv = this;
      else
      {
         *ppv = NULL;
         return E_NOINTERFACE;
      }

      AddRef();

      return S_OK;
   }

   STDMETHOD_(ULONG, AddRef)(void)
   {
      return InterlockedIncrement( &m_cRef );
   }

   STDMETHOD_(ULONG, Release)(void)
   {
      ULONG cRef = InterlockedDecrement( &m_cRef );

      if ( cRef == 0 )
		safe_delete( this );

      return cRef;
   }

   // *** IDispatch ***
   STDMETHOD (GetIDsOfNames)( REFIID, OLECHAR **, unsigned int, LCID, DISPID *pdispid )
   {
      *pdispid = DISPID_UNKNOWN;
      return DISP_E_UNKNOWNNAME;
   }

   STDMETHOD (GetTypeInfo)( unsigned int, LCID, ITypeInfo ** )
   {
      return E_NOTIMPL;
   }

   STDMETHOD (GetTypeInfoCount)( unsigned int * )
   {
      return E_NOTIMPL;
   }

   STDMETHOD (Invoke)( DISPID, REFIID, LCID, WORD, DISPPARAMS *, VARIANT *, EXCEPINFO *, unsigned int * );
   /*
   {
      return S_OK;
   }*/

private:

   LONG m_cRef;
};

class CWebContainer : public IOleClientSite, 
                      public IOleInPlaceSite,
                      public IOleInPlaceFrame,
                      public IOleControlSite,
                      public IDispatch
{
public:

   CWebContainer( HWND hwnd );
   ~CWebContainer();

   // *** IUnknown ***
   STDMETHOD(QueryInterface)( REFIID riid, PVOID *ppv )
   {
      if ( IsEqualIID( riid, IID_IOleClientSite ) )
         *ppv = (IOleClientSite *) this;
      else if ( IsEqualIID( riid, IID_IOleInPlaceSite ) )
         *ppv = (IOleInPlaceSite *) this;
      else if ( IsEqualIID( riid, IID_IOleInPlaceFrame ) )
         *ppv = (IOleInPlaceFrame *) this;
      else if ( IsEqualIID( riid, IID_IOleInPlaceUIWindow ) )
         *ppv = ( IOleInPlaceUIWindow *) this;
      else if ( IsEqualIID( riid, IID_IOleControlSite ) )
         *ppv = (IOleControlSite *)this;
      else if ( IsEqualIID( riid, IID_IOleWindow ) )
         *ppv = this;
      else if ( IsEqualIID( riid, IID_IDispatch ) )
         *ppv = (IDispatch *) this;
      else if ( IsEqualIID( riid, IID_IUnknown ) )
         *ppv = this;
      else
      {
         *ppv = NULL;
         return E_NOINTERFACE;
      }

      AddRef();

      return S_OK;
   }

   STDMETHOD_(ULONG, AddRef)(void)
   {
      return InterlockedIncrement( &m_cRef );
   }

   STDMETHOD_(ULONG, Release)(void)
   {
      ULONG cRef = InterlockedDecrement( &m_cRef );

      if ( cRef == 0 )
		safe_delete( this );

      return cRef;
   }

   // *** IOleClientSite ***
   STDMETHOD (SaveObject)()
   {
      return E_NOTIMPL;
   }

   STDMETHOD (GetMoniker)( DWORD, DWORD, LPMONIKER * )
   {
      return E_NOTIMPL;
   }

   STDMETHOD (GetContainer)( LPOLECONTAINER * )
   {
      return E_NOINTERFACE;
   }

   STDMETHOD (ShowObject)()
   {
      return S_OK;
   }

   STDMETHOD (OnShowWindow)( BOOL )
   {
      return S_OK;
   }

   STDMETHOD (RequestNewObjectLayout)()
   {
      return E_NOTIMPL;
   }

   // *** IOleWindow ***
   STDMETHOD (GetWindow)( HWND *phwnd )
   {
      *phwnd = m_hWnd;
      return S_OK;
   }

   STDMETHOD (ContextSensitiveHelp)( BOOL )
   {
      return E_NOTIMPL;
   }

   // *** IOleInPlaceSite ***
   STDMETHOD (CanInPlaceActivate)(void)
   {
      return S_OK;
   }

   STDMETHOD (OnInPlaceActivate) (void)
   {
      return S_OK;
   }

   STDMETHOD (OnUIActivate) (void)
   {
      return S_OK;
   }

   STDMETHOD (GetWindowContext)(
      IOleInPlaceFrame    **ppFrame, 
      IOleInPlaceUIWindow **ppIIPUIWin,
      LPRECT                prcPosRect,
      LPRECT                prcClipRect,
      LPOLEINPLACEFRAMEINFO pFrameInfo )
   {
      *ppFrame = (IOleInPlaceFrame *) this;
      *ppIIPUIWin = NULL;

      RECT rc;

      GetClientRect( m_hWnd, &rc );

      prcPosRect->left   = 0;
      prcPosRect->top    = 0;
      prcPosRect->right  = rc.right;
      prcPosRect->bottom = rc.bottom;

      CopyRect( prcClipRect, prcPosRect );

      pFrameInfo->cb             = sizeof(OLEINPLACEFRAMEINFO);
      pFrameInfo->fMDIApp        = FALSE;
      pFrameInfo->hwndFrame      = m_hWnd;
      pFrameInfo->haccel         = NULL;
      pFrameInfo->cAccelEntries  = 0;

      (*ppFrame)->AddRef();

      return S_OK;
   }

   STDMETHOD (Scroll)( SIZE )
   {
      return E_NOTIMPL;
   }

   STDMETHOD (OnUIDeactivate)( BOOL )
   {
      return E_NOTIMPL;
   }

   STDMETHOD (OnInPlaceDeactivate)(void)
   {
      return S_OK;
   }

   STDMETHOD (DiscardUndoState)(void)
   {
      return E_NOTIMPL;
   }

   STDMETHOD (DeactivateAndUndo)(void)
   {
      return E_NOTIMPL;
   }

   STDMETHOD (OnPosRectChange)( LPCRECT )
   {
      return S_OK;
   }

   // *** IOleInPlaceUIWindow ***
   STDMETHOD (GetBorder)( LPRECT )
   {
      return E_NOTIMPL;
   }

   STDMETHOD (RequestBorderSpace)( LPCBORDERWIDTHS )
   {
      return E_NOTIMPL;
   }

   STDMETHOD (SetBorderSpace)( LPCBORDERWIDTHS )
   {
      return E_NOTIMPL;
   }

   STDMETHOD (SetActiveObject)( IOleInPlaceActiveObject *, LPCOLESTR )
   {
      return E_NOTIMPL;
   }

   // *** IOleInPlaceFrame ***
   STDMETHOD (InsertMenus)( HMENU, LPOLEMENUGROUPWIDTHS )
   {
      return E_NOTIMPL;
   }

   STDMETHOD (SetMenu)( HMENU, HOLEMENU, HWND )
   {
      return E_NOTIMPL;
   }

   STDMETHOD (RemoveMenus)( HMENU )
   {
      return E_NOTIMPL;
   }

   STDMETHOD (SetStatusText)( LPCOLESTR )
   {
      return E_NOTIMPL;
   }

   STDMETHOD (EnableModeless)( BOOL )
   {
      return E_NOTIMPL;
   }

   STDMETHOD (TranslateAccelerator)( LPMSG, WORD )
   {
      return S_OK;
   }

   // *** IOleControlSite ***
   STDMETHOD (OnControlInfoChanged)(void)
   {
      return E_NOTIMPL;
   }

   STDMETHOD (LockInPlaceActive)( BOOL )
   {
      return E_NOTIMPL;
   }

   STDMETHOD (GetExtendedControl)( IDispatch ** )
   {
      return E_NOTIMPL;
   }

   STDMETHOD (TransformCoords)( POINTL *, POINTF *, DWORD )
   {
      return E_NOTIMPL;
   }

   STDMETHOD (TranslateAccelerator)( LPMSG, DWORD )
   {
      return E_NOTIMPL;
   }

   STDMETHOD (OnFocus)( BOOL )
   {
      return E_NOTIMPL;
   }

   STDMETHOD (ShowPropertyFrame)(void)
   {
      return E_NOTIMPL;
   }

   // *** IDispatch ***
   STDMETHOD (GetIDsOfNames)( REFIID, OLECHAR **, unsigned int, LCID, DISPID *pdispid )
   {
      *pdispid = DISPID_UNKNOWN;
      return DISP_E_UNKNOWNNAME;
   }

   STDMETHOD (GetTypeInfo)( unsigned int, LCID, ITypeInfo ** )
   {
      return E_NOTIMPL;
   }

   STDMETHOD (GetTypeInfoCount)( unsigned int * )
   {
      return E_NOTIMPL;
   }

   STDMETHOD (Invoke)( DISPID, REFIID, LCID, WORD, DISPPARAMS *, VARIANT *, EXCEPINFO *, unsigned int * )
   {
      return DISP_E_MEMBERNOTFOUND;
   }

   void setLocation( int, int, int, int );
   void setVisible( bool );
   void setFocus( bool );
   void add();
   void remove();
   HRESULT getWeb( IWebBrowser2 ** );

private:

   void ConnectEvents();
   void DisconnectEvents();

   IConnectionPoint *GetConnectionPoint( REFIID );

   LONG           m_cRef;
   HWND           m_hWnd;
   IUnknown      *m_punk;
   RECT           m_rect;
   CWebEventSink *m_pEvent;
   DWORD          m_eventCookie;
};

#endif

