//
// CAVICAP.CPP
// (c) Vadim Gorbatenko, 1996-99 
// gvv@mail.tomsknet.ru
// All rights reserved
//
// AviCap window
// Release: 0998
// Started: 11.09.96
//___________________________________________________________________________________________


#include "stdafx.h"
#include "cavicap.h"

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "vfw32.lib")

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(AviCap, CWnd)
	//{{AFX_MSG_MAP(AviCap)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

typedef LRESULT (CALLBACK *FRAMECALLBACK)(HWND , LPVIDEOHDR);

//optional: specify required priority for capturing
#ifdef CAPTURE_PRIORITY_CONTROL
	#define UP_THR() {oldpr=GetThreadPriority(GetCurrentThread()); \
					SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_TIME_CRITICAL);}

	#define DN_THR() {SetThreadPriority(GetCurrentThread(),oldpr);}
#else
	#define UP_THR()
	#define DN_THR()
#endif

//granularity for testing
#define	DEF_FRAMESIZE_GRAN_X 20 //pixels
#define	DEF_FRAMESIZE_GRAN_Y 15 //pixels


#define NOW()	timeGetTime()
#define	PALSZ	(sizeof(RGBQUAD))
#define _XXXX() (int)HIWORD(_formats[i])

#define	CHECKWIN()	{if(!GetSafeHwnd()) {iLastError=CAP_CALLNOWINDOW;return FALSE;}}
#define	CHECKCNCT()	{if(_curDriver==-1) {iLastError=CAP_CALLNOCONNECTION;return FALSE;}}


//forward definitions
LRESULT FAR PASCAL _1FrameCallbackProc(HWND hWnd, LPVIDEOHDR lpVHdr);
LRESULT FAR PASCAL _defaultFrameCallbackProc(HWND hWnd, LPVIDEOHDR lpVHdr);
LRESULT FAR PASCAL _timerFrameCallbackProc(HWND hWnd, LPVIDEOHDR lpVHdr);
void 		_dumpHdr(LPBITMAPINFOHEADER);


//default notification strings
LPSTR	lpNotify01=	"Testing resolutions...";
LPSTR	lpNotify02=	"Seaching for NTSC formats...";
LPSTR	lpNotify020="Seaching for PAL/SECAM formats...";
LPSTR	lpNotify03=	"Seaching for buffers...";




FRAMECALLBACK	_UserFrameCallBack=NULL;
LPVIDEOHDR		_lpLastVHdr = NULL;
DWORD			_frameStarted	= 0;
DWORD			_frameDelta	= 0;
BOOL			_frameFinished	= FALSE;
DWORD			_bufferSize	= 0;
int 			_InternalBufCnt = 0;
volatile BOOL	capAbort=FALSE;
int		oldpr;


//
// Constructor
//
AviCap::AviCap()
{
_selfInit();
}

AviCap::~AviCap()
{
	Disconnect();
}

BOOL	AviCap::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd,  BOOL fAutoSize)
{
	ASSERT(!GetSafeHwnd());
	
	if(GetSafeHwnd()) {
		iLastError=CAP_CREATE_DUP;
		return FALSE;		//already connected, can't connect twice!
		}
	
	_autosize = fAutoSize;

	HWND hWnd=capCreateCaptureWindow("AviCap_Basic", 
        	dwStyle, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, 
        	pParentWnd->GetSafeHwnd(), AVICAP_WINDOW_ID);

	if(!hWnd)
		{
		TRACE("AviCap Window creation failed\n");
		iLastError=CAP_WINCREATION_FAILED;
		return FALSE;
		}

	//subclass standard window
	SubclassWindow(hWnd);
	::SetClassLong(hWnd, GCL_STYLE, ::GetClassLong(hWnd,GCL_STYLE)|CS_DBLCLKS);


#ifdef	ON_CONNECT_CHECK_DRIVERLIST
	_getDrvList();
#endif

	return TRUE;
}


//
// Connect with capture driver
// ==============================
// Important notes: 
// Unfortunately, Capture drivers are always 16 bit programs.
// Because of this, driver will "locked: in memory after abnormal capture 
// program termination, and we will can't connect with capture driver again.
// There is no standard and simple ways to make sure is this driver installed 
// and "locked" or actually not exists or disabled. :-((((
//
BOOL	AviCap::ConnectWithDriver(LPCSTR Name)
{
	
	capAbort=FALSE;

	if(!Name)	// Connect to default Driver!
				// N 1 in system list
		return _connect2Driver(0);

	//find
	if(!_totalDrv())
		{
		iLastError=CAP_NO_CAPDEVICE_INSTALLED;
		return FALSE;
		}

	for(int i=0;i<_totalDrv();i++)
		if(lstrcmp(Name,_drvNames[i])==0 )
			return _connect2Driver(i);

	iLastError=CAP_DRIVERNAME_NOT_FOUND;
	return FALSE;
}

// Connect with capture driver
// ==============================
// Same as previouse, but use index from system list
BOOL	AviCap::ConnectWithDriver(int indx)
{
	if(indx == ANY_DRIVER)
		indx=0;
	return _connect2Driver(indx);
}

// Get capture device summary
//
BOOL	AviCap::GetDriverInfo(CAPDRVINFO *cdi)
{

	CHECKCNCT();
	if(!cdi)	return FALSE;
	
	lstrcpy(cdi->psName,GetDriverName());
	cdi->dwInternalBufCnt= _internalBufCnt;
	cdi->dwCurBiBitCount = ((LPBITMAPINFOHEADER)_bmpInfo)->biBitCount;
	cdi->dwBiBitsCountSupported=_biBitsCountSupported;
	cdi->dwCompressed =((LPBITMAPINFOHEADER)_bmpInfo)->biCompression;
	cdi->szFrameSize.cx=(int)((LPBITMAPINFOHEADER)_bmpInfo)->biWidth;
	cdi->szFrameSize.cy=(int)((LPBITMAPINFOHEADER)_bmpInfo)->biHeight;

	
	cdi->fFormatDlgSup=_capDriverCaps.fHasDlgVideoFormat;
	cdi->fSourceDlgSup=_capDriverCaps.fHasDlgVideoSource;
	cdi->fDisplayDlgSup=_capDriverCaps.fHasDlgVideoDisplay;
    cdi->fNTSC = FALSE;//default == PAL
	
	//NB: valid NTSC formats : 640x480, 320x240, 160x120 etc.
	//	  valid PAL  formats : 768x576, 384x288, 196x144 etc.
	#define FX	 cdi->szFrameSize.cx
	if(_formats.GetSize())
	{
	for(int i=0;i<_formats.GetSize();i++)
			//any from follow			
			if(_XXXX()==320||_XXXX()==640||_XXXX()==160||_XXXX()==400)
				{cdi->fNTSC =TRUE; break;}
	}
	else
		if(FX==320||FX==640||FX==160||FX==400)
				cdi->fNTSC =TRUE;
	return TRUE;				
}

// Disconnection
// No comments
BOOL	AviCap::Disconnect()
{	

	CHECKWIN();
	CHECKCNCT();
	
	capSetCallbackOnFrame(GetSafeHwnd(), NULL);
	capSetCallbackOnVideoStream(GetSafeHwnd(), NULL);
	StartSeq(FALSE);
	
	if(_curDriver!=-1&&GetSafeHwnd())
		if(capDriverDisconnect(GetSafeHwnd()))
			 _curDriver=-1;

	if(_bmpInfo)	{	safe_delete( _bmpInfo );
			_bmpInfo=NULL;}

	if(_curDriver==-1)
		return TRUE;
	else	return FALSE;
}

// Standard message 
//
BOOL	AviCap::StartPreview(BOOL offOn)
{
	CHECKWIN();
	CHECKCNCT();
	if(_fPreviewIsOn==offOn)	return TRUE;
	if(capPreview(GetSafeHwnd(), offOn))
		{
		_fPreviewIsOn=offOn;
		return TRUE;}
	
	return FALSE;
	
}

// Standard message 
//
BOOL	AviCap::StartOverlay(BOOL offOn)
{
	CHECKWIN();
	CHECKCNCT();
	if(_fOverlayIsOn==offOn)	return TRUE;
	if(capOverlay(GetSafeHwnd(), offOn))
		{_fOverlayIsOn=offOn;return TRUE;}
	return FALSE;
}

// Standard message 
//
BOOL	AviCap::StartSeq(BOOL offOn)
{
	CHECKWIN();
	CHECKCNCT();
	if(offOn)
	{
	if(_fSeqIsOn)	return TRUE;
	_getSetup();
	_captureParms.fYield = TRUE;    // Watch out!
	_setSetup();
	return capCaptureSequenceNoFile(GetSafeHwnd()) ;
	}
	if(_fSeqIsOn)
		capCaptureStop(GetSafeHwnd());
	_fSeqIsOn=offOn;
	return TRUE;
}

// Clean up internal driver cache
//
void	AviCap::ResetCache()
{
	 if(!GetSafeHwnd()||
		_curDriver==-1 ||
		_internalBufCnt>1)	return;
	
	//disable user callback
	FRAMECALLBACK	ofc = _UserFrameCallBack;
	_UserFrameCallBack = NULL;
	 capGrabFrameNoStop(GetSafeHwnd());
	 _UserFrameCallBack =ofc;

}

BOOL	AviCap::GrabOneFrame()
{
	CHECKWIN();
	CHECKCNCT();
	capAbort=FALSE;

#ifdef DEFAULT_FORCE_SINGLE_FRAME

	if(!_UserFrameCallBack)	return FALSE;
	_frameFinished = FALSE;
	_frameStarted  = 0;
	_InternalBufCnt=_internalBufCnt;

	UP_THR()
	for(int i=0;i<MAX_VALID_BUFFERS_COUNT;i++)
		   {
		 if(capAbort)	break;
		 capGrabFrameNoStop(GetSafeHwnd());
		    if(_frameFinished) {
		    			DN_THR();
		    			return TRUE;}
		   }
	DN_THR()
	return FALSE;
#else
	UP_THR()
	BOOL r = capGrabFrameNoStop(GetSafeHwnd());
	DN_THR()
	return r;
#endif


}

// Standard message 
//
BOOL	AviCap::SetPreviewRate(int Rate)
{
	CHECKWIN();
	CHECKCNCT();
	if(Rate<0)	return FALSE;
	if(capPreviewRate(GetSafeHwnd(), Rate))
		{_previewRate=Rate; return TRUE;}

	return FALSE;
}


// Standard message 
//
BOOL	AviCap::SetYiedCallBack(LPVOID cb)
{
	CHECKWIN();
	CHECKCNCT();
	return capSetCallbackOnYield(GetSafeHwnd(), cb);
}

// setup callback for error message processing
BOOL	AviCap::SetErrorCallBack(LPVOID cb)
{
	CHECKWIN();
	CHECKCNCT();
	return capSetCallbackOnError(GetSafeHwnd(), cb);
}


// Standard message 
//
BOOL	AviCap::SetFrameCallBack(LPVOID cb)
{
	CHECKWIN();
	CHECKCNCT();
	_UserFrameCallBack=(FRAMECALLBACK)cb;
	return TRUE;	
}

// Standard message 
//
BOOL	AviCap::SetStreamCallBack(LPVOID cb)
{
	CHECKWIN();
	CHECKCNCT();
	return capSetCallbackOnVideoStream(GetSafeHwnd(), cb);
}

DRAWCALLBACK drwcb = NULL;


// setup required parameters
//
BOOL	AviCap::SetCapSetup(CAPTUREPARMS *parms)
{
	CHECKWIN();
	CHECKCNCT();
	if(!parms)	return FALSE;
	if(!capCaptureSetSetup (GetSafeHwnd(), parms, sizeof CAPTUREPARMS)||
	   !_getSetup()||!_getFormat())
		return FALSE;
	return	TRUE;
}

// setup required image parameters
//
BOOL	AviCap::SetFormat(LPBITMAPINFO bmpinf)
{
	CHECKWIN();
	CHECKCNCT();
	if(!bmpinf)	return FALSE;
	// calculate Palette size ,
	// setup new format & update internals parameters
	return (capSetVideoFormat(GetSafeHwnd(), bmpinf, 
								(WORD)_getBMIsize(bmpinf))&&
								_getFormat());
}

//no comments
BOOL	AviCap::SetBitResolution(UINT bits)
{
	CHECKWIN();
	CHECKCNCT();

	if(!IsSupportedBitsResolution(bits))
						return FALSE;
	if(!_getStatus())	return FALSE;

	LPBITMAPINFO bi=
		_mk_LPBITMAPINFO(bits, (int)_capStatus.uiImageWidth,
							(int)_capStatus.uiImageHeight);
	BOOL ret=SetFormat(bi);
	if(bi) safe_delete( bi );
	return ret;
}

//no comments
BOOL	AviCap::SetFrameSize(int cx, int cy)
{
	CHECKWIN();
	CHECKCNCT();

	if(	((LPBITMAPINFOHEADER)_bmpInfo)->biWidth == cx &&
		((LPBITMAPINFOHEADER)_bmpInfo)->biHeight == cy)
		return TRUE;

	LPBITMAPINFO bi = _mk_LPBITMAPINFO(		0 /*Do not change bits resolution*/,
										cx, cy /*new image size*/
										);
	
	BOOL ret=SetFormat(bi);

	if(bi) safe_delete( bi );

	_getStatus();
	_autoSize();
	return ret;
}

//testing
BOOL	AviCap::EnumValidFrameSize(CSize &sz, int indx)
{
	CHECKWIN();
	CHECKCNCT();
	
	if(_formats.GetSize()==0) //is internal list ready?
	{
		//no. Collect info.
		AfxGetApp( )-> DoWaitCursor( 1 );
		UP_THR()
		_testValidFormats();
		DN_THR()
		AfxGetApp( )-> DoWaitCursor( 0 );

		if(_formats.GetSize()==0)
			return FALSE;
	}
	else
		//yes. Validate from internal list 
	if(indx>=_formats.GetSize()||indx<0)
		return FALSE;		
	
	sz.cx=(int)HIWORD(_formats[indx]);
	sz.cy=(int)LOWORD(_formats[indx]);
	return TRUE;
}

//testing
BOOL	AviCap::IsSupportedBitsResolution(UINT bitsCount)
{
	CHECKWIN();
	CHECKCNCT();
	if(!_biBitsCountSupported) //still not tested, so test it...
	{
		AfxGetApp( )-> DoWaitCursor( 1 );
		UP_THR()
		_testBiBitsCount();
		DN_THR()
		AfxGetApp( )-> DoWaitCursor( 0 );
	}
	
	switch(bitsCount)
	{
	case 1:	return 	(_biBitsCountSupported&BITS01) == BITS01;
	case 4:	return 	(_biBitsCountSupported&BITS04) == BITS04;
	case 8:	return 	(_biBitsCountSupported&BITS08) == BITS08;
	case 16:	return 	(_biBitsCountSupported&BITS16) == BITS16;
	case  24:	return 	(_biBitsCountSupported&BITS24) == BITS24;
	case 32:	return 	(_biBitsCountSupported&BITS32) == BITS32;
	default: break;
	}

	return FALSE;
}

BOOL	AviCap::Cancel()
{
	return capAbort=TRUE;
}


BOOL	AviCap::GetDriverCaps(CAPDRIVERCAPS *caps)
{
	CHECKWIN();
	CHECKCNCT();
	if(!caps)	return FALSE;
	_getCaps();
	memcpy (caps, &_capDriverCaps, sizeof CAPDRIVERCAPS);
	return TRUE;
}

BOOL	AviCap::GetCapStatus(CAPSTATUS *status)
{
	CHECKWIN();
	CHECKCNCT();
	if(status)	return FALSE;
	_getStatus();
	memcpy (status, &_capStatus, sizeof CAPSTATUS);
	return TRUE;
}

BOOL	AviCap::GetCapSetup(CAPTUREPARMS *parms)
{
	CHECKWIN();
	CHECKCNCT();
	if(!parms)	return FALSE;
	_getSetup();
	memcpy (parms, &_captureParms, sizeof CAPTUREPARMS);
	return TRUE;
}

BOOL	AviCap::GetDriversList(CStringArray *Names, CStringArray *Vers)
{
	if(!_totalDrv())
		_getDrvList();

	if(Names)
		Names->RemoveAll();
	if(Vers)
		Vers->RemoveAll();

	for(int i=0;i<_totalDrv();i++)
	{
	if(Names)
		Names->Add((LPCSTR)_drvNames[i]);
	if(Vers)
		Vers->Add((LPCSTR)_drvVers[i]);
	}
	return 	 _totalDrv();
}
int	AviCap::_GetInternalBuffersCount()
{
	CHECKWIN();
	CHECKCNCT();
	_testInternalBuffers();
	return _internalBufCnt;
}
//===== Private ================================

void	AviCap::_selfInit()
{
	_curDriver	= -1;
	_fDialogIsUp	= FALSE;
	_fPreviewIsOn	= FALSE;
	_fOverlayIsOn	= FALSE;
	_fSeqIsOn	= FALSE;
	_previewRate	= DEFAULT_PREVIEW_RATE;
	_bmpInfo	= NULL;
	_biBitsCountSupported=0;
	_UserFrameCallBack	= NULL;
	_internalBufCnt	= 0;
	m_DoQuickConnection	= FALSE;
	_1FrameCallBackInstalled = FALSE;
	_progressCB	= NULL;
	_drwcb		= NULL;
	iLastError	= 0;
	memset (&_capDriverCaps, 0, sizeof CAPDRIVERCAPS);
	memset (&_capStatus, 	 0, sizeof CAPSTATUS);                                                     

	_formats .SetSize(0,1);
	_drvNames.SetSize(0,1);
	_drvVers .SetSize(0,1);
	
	_cmprs_formats.SetSize(0,1);

}

void	AviCap::_getDrvList()
{
	if(_totalDrv())
		{_drvNames.RemoveAll( );
		_drvVers.RemoveAll( );}
	_drvNames.SetSize(0,1);
	_drvVers.SetSize(0,1);

    int j;
    char szName[80];
    char szVer[80];

    for (j =0; j < MAXCAPDRIVERS; j++) 
        if (capGetDriverDescription (j,
                szName, sizeof (szName),
                szVer,  sizeof (szVer))) 
	{
           _drvNames.Add((LPCSTR)szName);
	   _drvVers.Add((LPCSTR)szVer);
        }
    	else
	{
    //   _drvNames.Add((LPCSTR)"");
	//   _drvVers.Add((LPCSTR)"");
	break; // !!!! terminate 
	}
}

BOOL	AviCap::_getFormat()
{
	int vfs=capGetVideoFormatSize(GetSafeHwnd());
	
	if(!vfs)		return FALSE;

	 if(_bmpInfo)	{safe_delete( _bmpInfo );_bmpInfo=NULL;}
	  _bmpInfo =(BITMAPINFO	*)( new char[vfs]);
	
	LPBITMAPINFOHEADER bmpIH=( LPBITMAPINFOHEADER )_bmpInfo;
	 
	 bmpIH->biSize= sizeof BITMAPINFOHEADER;
	  BOOL ret=capGetVideoFormat(GetSafeHwnd(), _bmpInfo, (WORD)vfs);
	
	if(ret && _autosize)
	{
	#define XX	bmpIH->biWidth
	#define	YY	bmpIH->biHeight
	// Adjust size&position now!
	CRect	rc;
	CRect	rcc;
		CWnd  *parent = GetParent();
		if(!parent)  parent=CWnd::GetDesktopWindow( );
		ASSERT(parent);
		GetClientRect(&rcc);
		parent->GetClientRect(&rc);
		
		//if(rcc.Width() != XX ||
		//  rcc.Height() != YY)
		{
		int x = XX>rc.Width() ? 0 : (rc.Width()-XX)/2;
		int y = YY>rc.Height() ? 0 : (rc.Height()-YY)/2;
		SetWindowPos(NULL,
			x, y, XX,YY, SWP_NOZORDER);
		}

	}
	
	  
	 return   ret;

}

BOOL	AviCap::_getCaps()
{
	if (!capDriverGetCaps (GetSafeHwnd(), &_capDriverCaps, sizeof CAPDRIVERCAPS)) 
		{
		Sleep(20);
		if (capDriverGetCaps (GetSafeHwnd(), &_capDriverCaps, sizeof CAPDRIVERCAPS)) 
			return TRUE;
		memset (&_capDriverCaps, 0, sizeof CAPDRIVERCAPS);
		memset (&_capStatus, 	 0, sizeof CAPSTATUS);
		TRACE("capDriverGetCaps failed(wnd=%x)\n",GetSafeHwnd());
		return FALSE;
		}
	return TRUE;		
}

#define checkQuit()	{if(capAbort) {capDriverDisconnect(GetSafeHwnd()); return FALSE;}}

//General connection method
BOOL	AviCap::_connect2Driver(int indx)
{
	CHECKWIN();	
	if(indx<0||indx>_totalDrv())
		{
		TRACE("AviCap Window connection failed: illegal driver index\n");
		iLastError=CAP_ILLEGAL_DRIVERID;
		return FALSE;
		}
	 AfxGetApp( )-> DoWaitCursor( 1 );
	
	//connect via VFW
	BOOL ret=capDriverConnect(GetSafeHwnd(), indx);
	checkQuit();
		
	if(ret)
	{
	//starts getting driver information
	 	_curDriver=indx;
		if(!_getCaps()||
		   !_getSetup()||
		   !_getStatus()||
		   !_getFormat())
		   	{
		   	iLastError=CAP_INIT_FAILED;
		   	ret=FALSE;
			}
		else
			{
			//try to make window unvisible befor testing
			BOOL vsbl=IsWindowVisible();
			if(vsbl)	ShowWindow(SW_HIDE);
	
			//Slow or fast connection?
			if(!m_DoQuickConnection)
			{
			 //we have enough time to test driver :-)
			 UP_THR();

			 #ifdef	ON_CONNECT_TEST_BIBITSCOUNT
				_testBiBitsCount();
			 #endif
			 checkQuit();

			 #ifdef ON_CONNECT_CHECK_VALIDFORMATS
				_testValidFormats();
			 #endif
			 checkQuit();

			 #ifdef DEFAULT_FORCE_SINGLE_FRAME
				_testInternalBuffers();
			 #endif
			 checkQuit();
			 DN_THR();
			}
			//else skip testing
				
		//setup default capture parameters
		#ifdef DEFAULT_FORCE_SINGLE_FRAME
			capSetCallbackOnFrame(GetSafeHwnd(),(LPVOID)_1FrameCallbackProc);
			_1FrameCallBackInstalled = TRUE;
		#else
			capPreviewRate(GetSafeHwnd(), _previewRate);
		#endif
		
		#ifdef DEFAULT_USED_DOSMEMORY
			_captureParms.wNumVideoRequested=1;
			_captureParms.fUsingDOSMemory=TRUE;
		#endif	

		#ifdef DEFAULT_STEP_CAPTUREAT2X
			_captureParms.fStepCaptureAt2x=TRUE;
			_captureParms.wStepCaptureAverageFrames=3;
		#endif
		
		_captureParms.dwRequestMicroSecPerFrame=20000;
		_captureParms.fYield = TRUE;
		capCaptureSetSetup (GetSafeHwnd(), &_captureParms, sizeof CAPTUREPARMS);
		capCaptureGetSetup (GetSafeHwnd(), &_captureParms, sizeof CAPTUREPARMS);		

			if(vsbl)	ShowWindow(SW_SHOW); //restore
			}
		
	}
	else
	{
	AfxGetApp( )-> DoWaitCursor( 0 );
	TRACE("Connection to Capure Driver Failed\n");
	iLastError=CAP_CONNECTION_FAILED;
	return FALSE;
	}
	 
	if(ret) TRACE("Connected to <%s: %s>\n",GetDriverName(),GetDriverVer());

	else	  TRACE("Connection (phase 2) to Capure Driver Failed\n");
	
	if(!ret)	Disconnect();
	
	return ret;
}

// Testing for supported bits resolution.
// See notes for _testFrameFormat
BOOL	AviCap::_testBiBitsCount()
{
	
	_biBitsCountSupported = 0;

//Test for 1 bit image format
	if(_testBitsFormat(1))
		_biBitsCountSupported|=BITS01;
	_notify(lpNotify01,15);
//Test for 4 bit image format
	if(_testBitsFormat(4))
		_biBitsCountSupported|=BITS04;
	_notify(lpNotify01,30);
//Test for 8 bit image format
	if(_testBitsFormat(8))
		_biBitsCountSupported|=BITS08;
	_notify(lpNotify01,45);
//Test for 16 bit image format
	if(_testBitsFormat(16))
		_biBitsCountSupported|=BITS16;
	_notify(lpNotify01,60);
//Test for 24 bit image format
	if(_testBitsFormat(24))
		_biBitsCountSupported|=BITS24;
//Test for 32 bit image format
	_notify(lpNotify01,75);
	if(_testBitsFormat(32))
		_biBitsCountSupported|=BITS32;
	_notify(lpNotify01,90);

//restore old format
	BOOL ret=capSetVideoFormat(GetSafeHwnd(), _bmpInfo, (WORD)_getBMIsize(_bmpInfo));
	_notify(lpNotify01,100);
	return ret;	
}

// See notes for _testFrameFormat
BOOL	AviCap::_testBitsFormat(UINT bits)
{
	LPBITMAPINFO  bmpI;
	BOOL	ret;
	if(!_bmpInfo)
		if(!_getFormat())	return FALSE;

	int x=(int)((LPBITMAPINFOHEADER)_bmpInfo)->biWidth;
	int y=(int)((LPBITMAPINFOHEADER)_bmpInfo)->biHeight;

	
	bmpI=_mk_LPBITMAPINFO(bits,x,y);
	
	ret=capSetVideoFormat(GetSafeHwnd(), bmpI, (WORD)_getBMIsize(bmpI));
	if(ret)	 //check one's more
		{
		WORD req=((LPBITMAPINFOHEADER)bmpI)->biBitCount;
		capGetVideoFormat(GetSafeHwnd(), bmpI, (WORD)_getBMIsize(bmpI));
		
		if(((LPBITMAPINFOHEADER)bmpI)->biBitCount!=req)	ret=FALSE;
		
		}
	safe_delete( bmpI );
	return ret;
}

// Testing for supported frame sizes.
// See notes for _testFrameFormat
// ++Note: Testing only standard sizes(see below)

#define ADDFMT( x, y)	_formats . Add((DWORD)MAKELONG((WORD)y, (WORD)x) )

BOOL	AviCap::_testValidFormats()
{
	 int	xGran=DEF_FRAMESIZE_GRAN_X, x, xS;
	 int	yGran=DEF_FRAMESIZE_GRAN_Y, y, yS;
	
	int minx=2048,ix=-1,i;

	BOOL 	St1=FALSE;
	BOOL 	St2=FALSE;
	BOOL 	St3=FALSE;
	
	_formats .SetSize(0,1);
	
	_notify(lpNotify020,0);

// In first, test some standard PAL formats:
// 384x288, 768x576
	if(_testFrameFormat(384, 288))
		  {ADDFMT( 384, 288); }
	_notify(lpNotify020,50);
	
	if(_testFrameFormat(768,576))
		  {ADDFMT( 768,576); }
	_notify(lpNotify020,100);

	
	_notify(lpNotify02,0);

// In second, test some standard NTSC formats:
// 160x120, 320x240, 640x480
	if(_testFrameFormat(160, 120))
		  {ADDFMT( 160, 120); St1=TRUE;}
	
	_notify(lpNotify02,10);

	if(_testFrameFormat(320, 240))
		  {ADDFMT( 320, 240); St2=TRUE;}
	
	_notify(lpNotify02,20);

	if(_testFrameFormat(640, 480))
		  {ADDFMT(640, 480); St3=TRUE;}
	
	_notify(lpNotify02,30);

	// Now try to found size granularity!
	//
	if(St2)	 // Middle-Resolition Grabber
		{
		xS=x=320; yS=y=240;
		while(1) //Up
			{
			x+=xGran;y+=yGran;
			if(x==640)	break;//nothing 8-(
			
			if(_testFrameFormat(x, y))	goto found;
			}
		
		xS=x=320; yS=y=240;
		while(1) //Down
			{
			x-=xGran;y-=yGran;
			if(x==160)	break;//nothing 8-(
			
			if(_testFrameFormat(x, y))	goto found;
			}
		}
	else	if( St3)	//If High-resolution Grabber - start from this
		     {
			xS=x=640; yS=y=480;
			while(1) //Up
				{
				x+=xGran;y+=yGran;
				if(x>1600)	break;//nothing!
				
				if(_testFrameFormat(x, y))	goto found;
				}
		
			xS=x=640; yS=y=480;
			while(1) //Down
				{
				x-=xGran;y-=yGran;
				if(x==360)	break;//nothing!
				
				if(_testFrameFormat(x, y))	goto found;
				
				}
		      }//end of high resolution
		else	if(St1)	//Low-Resolution Grabber
				{
				xS=x=160; yS=y=120;
				while(1) //Up
					{
					x+=xGran;y+=yGran;
					if(x==320)	break;//nothing!
					if(_testFrameFormat(x, y))	goto found;
					}
		
				xS=x=160; yS=y=120;
				while(1) //Down
					{
					x-=xGran;y-=yGran;
					if(x<=60)	break;//nothing!
						if(_testFrameFormat(x, y))	goto found;
					}
				}//end of low resolution

	//end of testing for frame sizes
	_notify(lpNotify02,90);

	capSetVideoFormat(GetSafeHwnd(), _bmpInfo, (WORD)_getBMIsize(_bmpInfo));
	if(_formats .GetSize()==0&&
	_testFrameFormat(((LPBITMAPINFOHEADER)_bmpInfo)->biWidth, 
			((LPBITMAPINFOHEADER)_bmpInfo)->biHeight))
			
			ADDFMT( ((LPBITMAPINFOHEADER)_bmpInfo)->biWidth,
			((LPBITMAPINFOHEADER)_bmpInfo)->biHeight);
		
	 
	//find for smallest frame	 
	 for(i=0;i<_formats.GetSize();i++)
	       if(minx>_XXXX())	{ix=i; minx=_XXXX();}	
	if(ix!=-1)
		{
		_smallFrame.cx=(int)HIWORD(_formats[ix]);
		_smallFrame.cy=(int)LOWORD(_formats[ix]);
		}

	_notify(lpNotify02,100);
	return TRUE;	

found:
	xGran=abs(x-xS);
	yGran=abs(y-yS);

	// Walk through standard formats UP
	x=320; y=240;
	while(1)
		{
		x+=xGran;y+=yGran;
		if(x>1600)	break;	//Let's stop now!
		if(x==640&&y==480)	continue;
		if(_testFrameFormat(x, y))	ADDFMT( x, y);
		}
	_notify(lpNotify02,50);
	
	// Walk through standard formats DOWN
	x=320; y=240;
	while(1)
		{
		x-=xGran;y-=yGran;
		if(x<60)	break;//nothing!
		if(x==160&&y==120)	continue;
		if(_testFrameFormat(x, y))	ADDFMT( x, y);
		}
	 _notify(lpNotify02,100);
	
	//find smallest frame	 
	 for(i=0;i<_formats.GetSize();i++)
	       if(minx>_XXXX())	{ix=i; minx=_XXXX();}	
	  if(ix!=-1)
		{
		_smallFrame.cx=(int)HIWORD(_formats[ix]);
		_smallFrame.cy=(int)LOWORD(_formats[ix]);
		}

	//test completed
	//restore old format and return
	return 	capSetVideoFormat(GetSafeHwnd(), _bmpInfo, (WORD)_getBMIsize(_bmpInfo));
}

// Notes:
// 1. This may be slow enough
// 2. This may cause of driver's notification, such as 
// "not enough memory..." or "...not for this videostandard..." etc
// 3. There is no warranty is this format is really available :-(
BOOL	AviCap::_testFrameFormat(int x, int y)
{
	LPBITMAPINFO  bmpI;
	BOOL	ret;
	if(!_bmpInfo)
		if(!_getFormat())	return FALSE;

	int biBitCount=(int)((LPBITMAPINFOHEADER)_bmpInfo)->biBitCount;

	switch(biBitCount)
	{
	case 1:	biBitCount=BITS01; break;
	case 4:	biBitCount=BITS04; break;
	case 8:	biBitCount=BITS08; break;
	case 16:biBitCount=BITS16; break;
	case 24:biBitCount=BITS24; break;
	case 32:biBitCount=BITS32; break;
	default: return FALSE;
	}
	
	bmpI=_mk_LPBITMAPINFO(biBitCount,x,y);
	
	ret=capSetVideoFormat(GetSafeHwnd(), bmpI, (WORD)_getBMIsize(bmpI));
	if(ret)	 //check one's more
		{
		#define _XX()	(int)((LPBITMAPINFOHEADER)bmpI)->biWidth
		#define _YY()	(int)((LPBITMAPINFOHEADER)bmpI)->biHeight
		int x=_XX();
		int y=_YY();
		capGetVideoFormat(GetSafeHwnd(), bmpI, (WORD)_getBMIsize(bmpI));
		if(x!=_XX()||y!=_YY())	ret=FALSE;
	
	//Check is compressed
		if(ret)	{
			_bufferSize = 0;
			//set special callback for timing
			capSetCallbackOnFrame(GetSafeHwnd(),(LPVOID)_timerFrameCallbackProc);
			capGrabFrame(GetSafeHwnd());
	
			if(!_1FrameCallBackInstalled)
				capSetCallbackOnFrame(GetSafeHwnd(), _defaultFrameCallbackProc);
			else
				capSetCallbackOnFrame(GetSafeHwnd(),(LPVOID)_1FrameCallbackProc);
		
			_cmprs_formats.Add((BOOL)(_bufferSize!=_calcBufferSize((LPBITMAPINFOHEADER)bmpI)));	
	//end check		
			}
		}
	safe_delete( bmpI );
	return ret;
}


//
// Note: according SDK LPVIDEOHEADER MUST(!) point to UNCOMPRESSED 
// video bufer (lpData). Can we believe or not?
// Much better make sure...
BOOL	AviCap::_IsImageCompressed()	
{
	if(!_bmpInfo)
		if(!_getFormat())	return FALSE;

	_bufferSize = 0;
	//set specila callback
	if(!capSetCallbackOnFrame(GetSafeHwnd(),(LPVOID)_timerFrameCallbackProc))
		return FALSE;
	capGrabFrame(GetSafeHwnd());
	
	if(!_1FrameCallBackInstalled)
		capSetCallbackOnFrame(GetSafeHwnd(), (LPVOID)_defaultFrameCallbackProc);
	else
		capSetCallbackOnFrame(GetSafeHwnd(),(LPVOID)_1FrameCallbackProc);

	//compare resulting buffer sizes
	return (_bufferSize!=_calcBufferSize((LPBITMAPINFOHEADER)_bmpInfo));
	
}

//may be slow enough!
BOOL	AviCap::_testInternalBuffers()
{

//set special callback
	_notify(lpNotify03,0);
	if(!capSetCallbackOnFrame(GetSafeHwnd(),(LPVOID)_timerFrameCallbackProc))
		return (BOOL)(_internalBufCnt=0);
//set lowest resolution
	_pushResolution();

	if(_biBitsCountSupported!=0&&_formats.GetSize()!=0)
	{	
	DWORD biBitsCode;
	if(_biBitsCountSupported&BITS01) 	 biBitsCode=BITS01;
	else   	if(_biBitsCountSupported&BITS04) biBitsCode=BITS04;
		else   if(_biBitsCountSupported&BITS08) biBitsCode=BITS08;
			else   if(_biBitsCountSupported&BITS16) biBitsCode=BITS16;
				else   if(_biBitsCountSupported&BITS24) biBitsCode=BITS24;
				else   biBitsCode=BITS24;
	
	
	LPBITMAPINFO	bi=_mk_LPBITMAPINFO(biBitsCode,_smallFrame.cx,
							_smallFrame.cy);
	 
	 capSetVideoFormat(GetSafeHwnd(), bi, (WORD)_getBMIsize(bi));
	 _notify(lpNotify03,20);
	 safe_delete( bi );
	 }
	Sleep(200);
	_notify(lpNotify03,50);
	for(int i=0;i<MAX_VALID_BUFFERS_COUNT;i++)
	 {
		   _frameStarted = NOW();
		   capGrabFrame(GetSafeHwnd());
		   _notify(lpNotify03,60+i*10);
		    if(_frameDelta>=MIN_FRAME_PERIOD)	break;
	 }
//restore callback
	if(!_1FrameCallBackInstalled)
		capSetCallbackOnFrame(GetSafeHwnd(), (LPVOID)_defaultFrameCallbackProc);
	else
		capSetCallbackOnFrame(GetSafeHwnd(), (LPVOID)_1FrameCallbackProc);

	_InternalBufCnt=
	_internalBufCnt=i;
	TRACE("%d internal buffers found\n",_internalBufCnt);
	_popResolution();
	_notify(lpNotify03,100);
	
	return _internalBufCnt;
}

LPVOID _svRes=NULL;
int	_vfs;
void	AviCap::_pushResolution()
{
	_svRes=NULL;
	int _vfs=capGetVideoFormatSize(GetSafeHwnd());
	if(!_vfs)
		return;

	   _svRes=(LPVOID)( new char[_vfs]);
	
	  capGetVideoFormat(GetSafeHwnd(), _svRes, (WORD)_vfs);

}

void	AviCap::_popResolution()
{


	if(_svRes)
	{
	capSetVideoFormat(GetSafeHwnd(), _svRes, (WORD)_vfs);
	safe_delete( _svRes );
	_svRes = NULL;
	}
}


LPBITMAPINFO	AviCap::_mk_LPBITMAPINFO(UINT biBitCount,int x, int y)
{
	LPBITMAPINFO bmi;
	int sz;
	if(!_bmpInfo)	biBitCount=24;//default

	if(!biBitCount)	biBitCount=(int)((LPBITMAPINFOHEADER)_bmpInfo)->biBitCount;

	sz=_getBMIsize(biBitCount, 0);
	sz+=sizeof BITMAPINFOHEADER;
	bmi = 	(LPBITMAPINFO)(new char[sz+64]);
	LPBITMAPINFOHEADER bH=(LPBITMAPINFOHEADER)bmi;
	memset(bmi,0,sz);

	bH->biSize 	 = sizeof BITMAPINFOHEADER;
	bH->biWidth	 = x;
	bH->biHeight = y;
	bH->biPlanes = 1;
	bH->biBitCount	= biBitCount;
	bH->biCompression=BI_RGB;
	bH->biClrUsed=0;
	return bmi;
}

int	AviCap::_getBMIsize(LPBITMAPINFO bmi)
{
	LPBITMAPINFOHEADER bH=(LPBITMAPINFOHEADER)bmi;

	int sz=(WORD)bH->biSize;
	return (sz+_getBMIsize((UINT)bH->biBitCount,(UINT)bH->biClrUsed));
}

DWORD	AviCap::_calcBufferSize(LPBITMAPINFOHEADER bih)
{
#define _XXX()	(int)bih->biWidth
#define _YYY()	(int)bih->biHeight

	DWORD ret=(_XXX()*_YYY());
	switch(bih->biBitCount)
	{
	case 1:	return ret/8;
	case 4:	return ret/2;
	case 8:	return ret;
	case 16: return ret*2;
	case 24: return ret*3;
	case 32: return ret*4;
	default: return 0;//???
	}

}

int	AviCap::_getBMIsize(UINT bitsReq, UINT bmiColors)
{
	int sz=0;

	if(!bmiColors)
	{
	switch(bitsReq)
		{
		case 1:	sz=2*PALSZ; break;
		case 4:	sz=16*PALSZ; break;
		case 8:	sz=256*PALSZ; break;
		case 16:
		case 32:
		default: break;		/* A 16,24,32 bitcount DIB has no color table */
		}
	
    }
    else sz=bmiColors*PALSZ;

	return sz;
}

//adjust size and position according current frame size & "_autosize" flag
void	AviCap::_autoSize()	
{
	if(_curDriver==-1 || !_getFormat())	return;
	
	LPBITMAPINFOHEADER bmpIH=( LPBITMAPINFOHEADER )_bmpInfo;
	
	{
	#define XX	bmpIH->biWidth
	#define	YY	bmpIH->biHeight
	// Adjust size&position now!
	CRect	rc;
	CRect	rcc;
		CWnd  *parent = GetParent();
		if(!parent)  parent=CWnd::GetDesktopWindow( );
		ASSERT(parent);
		GetClientRect(&rcc);
		parent->GetClientRect(&rc);
		
		if(_autosize)	
		{
		int x = XX>rc.Width() ? 0 : (rc.Width()-XX)/2;
		int y = YY>rc.Height() ? 0 : (rc.Height()-YY)/2;
		
		MapWindowPoints(parent, &rcc);
			
			if(x!=rcc.left || 
				y!=rcc.top ||
			  XX!=rc.Width() || 
			  YY!=rc.Height())
				
				SetWindowPos(NULL,
					x, y, XX, YY, SWP_NOZORDER);
		}
		
		else //just center Window
		{
		int x = rcc.Width()>rc.Width() ? 0 : (rc.Width() - rcc.Width())/2;
		int y = rcc.Height()>rc.Height() ? 0 : (rc.Height() - rcc.Height())/2;
			
		MapWindowPoints(parent, &rcc);
			if(x!=rcc.left || y!=rcc.top)
			SetWindowPos(NULL,
				x, y, 0,0, SWP_NOZORDER|SWP_NOSIZE);
		}
	   
	}
	
}

//////////////////////////
void _dumpHdr(LPBITMAPINFOHEADER bH)
{
	TRACE("===== New LPBITMAPINFO ========\n");
	TRACE("biWidth \t=%u\n",(UINT)bH->biWidth);
	TRACE("biHeight\t=%u\n",(UINT)bH->biHeight);
	TRACE("biPlanes\t=%u\n",(UINT)bH->biPlanes);
	TRACE("biBitCount\t=%u\n",(UINT)bH->biBitCount);
	TRACE("biClrUsed\t=%u\n",(UINT)bH->biClrUsed);
	TRACE("biCompression\t=%u\n",(UINT)bH->biCompression) ;
	TRACE("biSizeImage\t=%u\n",(UINT)bH->biSizeImage) ;
	TRACE("================================\n");
}
///////////////////////////////////////////////////////////////////
// We have to use this scheme if we need EXACTLY newest image buffer:
// skip cached buffers
LRESULT FAR PASCAL _1FrameCallbackProc(HWND hWnd, LPVIDEOHDR lpVHdr)
{
	_lpLastVHdr = lpVHdr; //save ptr for drawing
	if(_UserFrameCallBack)
		{
		if((int)_frameStarted++<_InternalBufCnt)	return TRUE;
		_frameFinished=TRUE;
		 ( *_UserFrameCallBack )(hWnd,lpVHdr);
		}

	return TRUE;
}
//default callback
LRESULT FAR PASCAL _defaultFrameCallbackProc(HWND hWnd, LPVIDEOHDR lpVHdr)
{
	_lpLastVHdr = lpVHdr; //save ptr for drawing
	if(_UserFrameCallBack)
		 ( *_UserFrameCallBack )(hWnd,lpVHdr);
		 return  TRUE;

	return TRUE;
}

LRESULT FAR PASCAL _timerFrameCallbackProc(HWND hWnd, LPVIDEOHDR lpVHdr)
{
	_lpLastVHdr = lpVHdr; //save ptr for drawing
	_frameDelta=NOW()-_frameStarted;
	_bufferSize=lpVHdr->dwBytesUsed;
	return TRUE;
}

void AviCap::OnDestroy() 
{
	//disconnect before destroying
	Disconnect();
	CWnd::OnDestroy();
}


void AviCap::OnTimer(UINT nIDEvent) 
{
	//add additional processing here

	CWnd::OnTimer(nIDEvent);
}


BOOL AviCap::OnEraseBkgnd(CDC* pDC) 
{
	//skip erasing: we don't need blinking!
	return TRUE;	
}


void AviCap::OnLButtonDown(UINT nFlags, CPoint point) 
{
	//add additional processing here
}

void AviCap::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	//add additional processing here
}

void AviCap::OnPaint(void)
{
	//note AVICAP window doesn't performs a stretching,
	//so we have to do this by themself.
	LPBITMAPINFOHEADER bmpIH=( LPBITMAPINFOHEADER )_bmpInfo;

	if(_fShow)//switch off/on
		{
		ASSERT(_bmpInfo);

		#define XX	bmpIH->biWidth
		#define	YY	bmpIH->biHeight
		
		CRect rc;
		GetClientRect(&rc);
		BOOL fNeedStretch = (rc.right==XX && rc.bottom==YY) ? FALSE: TRUE;
		
		if(!fNeedStretch)
			CWnd::OnPaint();
		else 
			if(_lpLastVHdr)//is ready?
			{//stretch bitmap
			CDC *dc = GetDC();
			
			StretchDIBits( dc->GetSafeHdc( ), 
						0, 
						0, 
						rc.Width(), 
						rc.Height(), 
						0, 
						0, 
						((LPBITMAPINFOHEADER)_bmpInfo)->biWidth, 
						((LPBITMAPINFOHEADER)_bmpInfo)->biHeight, 
						(CONST VOID *)_lpLastVHdr->lpData, 
						_bmpInfo, 
						DIB_RGB_COLORS, 
						SRCCOPY );

			ReleaseDC(dc);
			}

			if(_drwcb) //allow user to paint :-)
			{
				(*_drwcb)(fNeedStretch);
				ValidateRect(NULL);
			}
		}
	else	//just validate area and quit
			ValidateRect(NULL);

}
