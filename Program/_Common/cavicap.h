// CAVICAP.H
// (c) Vadim Gorbatenko, 1996-99
// AviCap window
// Started: 11.09.96
//___________________________________________________________________________________________

// On connection action:
//#define	ON_CONNECT_TEST_BIBITSCOUNT		// Testing for available bits resolution
#define	ON_CONNECT_CHECK_DRIVERLIST		// Create full list of installed drivers
//#define	ON_CONNECT_CHECK_VALIDFORMATS	// Create full list of available formats
#define	CAPTURE_PRIORITY_CONTROL	
//#define DEFAULT_FORCE_SINGLE_FRAME		// Skip cached frames for GrabOne

#define	AVICAP_WINDOW_ID	0xffff

#if !defined(_INC_VFW)
    #define	NODRAWDIB
    #define	NOAVIFMT
    #define	NOMMREG
    #define	NOAVIFILE
    #define	NOMCIWND
    #define	NOMSACM
    #include "vfw.h"
#endif

typedef VOID (CALLBACK *PROGRESSCALLBACK)(LPSTR , DWORD);
typedef VOID (CALLBACK *DRAWCALLBACK)(BOOL);

#define	ANY_DRIVER				-1	// default driver(for connection only)
#define	MAXCAPDRIVERS			10	// maximum drivers list size
#define DEFAULT_PREVIEW_RATE	50	// default preview rate = 50 msec
#define	MAX_VALID_BUFFERS_COUNT 10	// maximum internal buffer for Vxd(testing)
#define MIN_FRAME_PERIOD		20	// msec -> 50 frames per second (can't do faster!)

//optional
#ifdef DEFAULT_FORCE_SINGLE_FRAME		//try to get newest frame when single grab performed
 #ifndef ON_CONNECT_CHECK_VALIDFORMATS
  #define ON_CONNECT_CHECK_VALIDFORMATS	//test all supported formats when connected
 #endif
#ifndef ON_CONNECT_TEST_BIBITSCOUNT
  #define ON_CONNECT_TEST_BIBITSCOUNT	//test all supported resolutions when connected
#endif
#endif

//Image Bits resolution
#define	BITS01	0x0001		// B&W Mono
#define	BITS04	0x0002		// 16  Colors
#define	BITS08	0x0004		// 256 Colors
#define	BITS16	0x0008		// 16 bits RGB
#define	BITS24	0x0010		// 24 bits RGB
#define	BITS32	0x0020		// 32 bits RGB

//struct for gathering capture driver information
typedef struct
{
	CHAR	psName[128];				  //name of driver
	DWORD	dwInternalBufCnt;			 //driver cache size
	DWORD	dwCurBiBitCount;			//current resolution
	DWORD	dwBiBitsCountSupported; //see abow for flags
    BOOL	fNTSC;			//this is NTSC videostandard
	DWORD	dwCompressed;		//is compressed
	SIZE	szFrameSize;		//Current frame size
	BOOL	fFormatDlgSup;	//"Format"  dialog supported
	BOOL	fSourceDlgSup;	//"Source"  dialog supported
	BOOL	fDisplayDlgSup;	//"Display" dialog supported
} CAPDRVINFO;

class AviCap: public CWnd
{
public:
	enum { visible = WS_CHILD|WS_VISIBLE,hidden=WS_CHILD};

	AviCap();
	~AviCap();
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AviCap)
	public:
	virtual BOOL	Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, BOOL fAutoSize=TRUE);
	//}}AFX_VIRTUAL
	
	//connection & disconnection
	BOOL	ConnectWithDriver(LPCSTR Name=NULL);
	BOOL	ConnectWithDriver(int indx);
	BOOL	Disconnect();

	//capturing COMMANDs
	BOOL	StartPreview(BOOL offOn);
	BOOL	GetPreviewFlag()	{return _fPreviewIsOn;}
	BOOL	StartOverlay(BOOL offOn);
	BOOL	StartSeq(BOOL offOn);
	BOOL	GrabOneFrame();
	BOOL	Cancel();

	//define capture parameters & control
	BOOL	SetPreviewRate(int Rate);		/*Rate == xx msec*/
	BOOL	SetCapSetup(CAPTUREPARMS *parms);

	//Enhanced control
	void	ResetCache();
	BOOL	SetFormat(LPBITMAPINFO bmpinf);
	BOOL	SetBitResolution(UINT reqBitsCount);
	BOOL	SetFrameSize(int x, int y);
	BOOL	SetFrameSize(CSize sz) {return SetFrameSize(sz.cx, sz.cy);}

inline	BOOL	SavePalette(LPCSTR file) {return (_curDriver==-1) ? FALSE : (BOOL)capPaletteSave(GetSafeHwnd(), file);}
inline	BOOL	LoadPalette(LPCSTR file) {return (_curDriver==-1) ? FALSE : (BOOL)capPaletteOpen(GetSafeHwnd(), file);}
inline	BOOL	PastePalette()			{return (_curDriver==-1) ? FALSE : (BOOL)capPalettePaste(GetSafeHwnd());}
inline	BOOL	CreatePalette(int frames,int colors)  {return (_curDriver==-1) ? FALSE : (BOOL)capPaletteAuto(GetSafeHwnd(), frames, colors);};
inline	BOOL	CopyFrame()				{return (_curDriver==-1) ? FALSE : (BOOL)capEditCopy(GetSafeHwnd());}
inline	BOOL	SetShowFlag(BOOL fsf)	{BOOL os=_fShow; _fShow=fsf; return os;}


		//enhanced formats information
		BOOL			GetDriverInfo(CAPDRVINFO *cdi);
inline	CSize			GetFrameSize()	{return (_curDriver==-1) ? CSize(0,0) : CSize(((LPBITMAPINFOHEADER)_bmpInfo)->biWidth,((LPBITMAPINFOHEADER)_bmpInfo)->biHeight);}
inline	DWORD			GetRequiredBufferSize()		{return (_curDriver==-1) ? FALSE : _calcBufferSize((LPBITMAPINFOHEADER)_bmpInfo);}
inline	LPBITMAPINFO	GetCurrentFormat()	{return _bmpInfo;}
		BOOL			IsSupportedBitsResolution(UINT bitsCount);
inline	DWORD			GetCurrentBitsResolution()	{_getFormat(); return  _bmpInfo ? ((LPBITMAPINFOHEADER)_bmpInfo)->biBitCount:0;}
		BOOL			EnumValidFrameSize(CSize &sz, int indx);
	
	//status & setup information
	BOOL		GetDriverCaps(CAPDRIVERCAPS *caps);
	BOOL		GetCapStatus(CAPSTATUS *status);
	BOOL		GetCapSetup(CAPTUREPARMS *parms);
  	

	//driver misc information
		int		GetDriversList(CStringArray *Names, CStringArray *Vers);
inline	LPCSTR	GetDriverName()	{return (_curDriver==-1) ? NULL : (LPCSTR)_drvNames[_curDriver];}
inline	LPCSTR	GetDriverVer()	{return (_curDriver==-1) ? NULL : (LPCSTR)_drvVers[_curDriver];}
inline	int		GetDriverIndex() {return _curDriver;}

	//std Dialogs
	BOOL	CompressDlg()	{return (_curDriver==-1) ? FALSE : (BOOL)capDlgVideoCompression(GetSafeHwnd());}
	BOOL	DisplayDlg()	{BOOL r = (_curDriver==-1) ? FALSE : (BOOL)capDlgVideoDisplay(GetSafeHwnd());_autoSize();return r;}	
	BOOL	FormatDlg()		{BOOL r = (_curDriver==-1) ? FALSE : (BOOL)capDlgVideoFormat(GetSafeHwnd());_autoSize();return r;}
	BOOL	VSourceDlg()	{BOOL r = (_curDriver==-1) ? FALSE : (BOOL)capDlgVideoSource(GetSafeHwnd());_autoSize();return r;}

	//Setup standard Avicap callbacks
	BOOL		SetFrameCallBack(LPVOID cb);
	BOOL		SetStreamCallBack(LPVOID cb);
	BOOL		SetYiedCallBack(LPVOID cb);
	BOOL		SetErrorCallBack(LPVOID cb);
	
	//Setup specific callbacks for drawing and progress notification
inline	VOID	SetDrawCallBack(DRAWCALLBACK cb) {_drwcb = cb;}
inline 	VOID	SetProgressCallback(LPVOID cb) {_progressCB=(PROGRESSCALLBACK) cb;}

	//other
	void	AdjustPlacement(BOOL fStretch=FALSE)	{_autosize=!fStretch; _autoSize(); }
inline	BOOL	IsReady()	{return (_curDriver!=-1)&&GetSafeHwnd();}
inline	int		GetLastError()		{int ret=iLastError;iLastError=0; return ret;}	
	
	//optional attribute for quick connection(without testing)
	BOOL		m_DoQuickConnection; 

protected:
	CAPDRIVERCAPS	_capDriverCaps;
	CAPSTATUS		_capStatus;
	CAPTUREPARMS	_captureParms;
	LPBITMAPINFO	_bmpInfo;
	CStringArray	_drvNames;
	CStringArray 	_drvVers;
	int			_curDriver;
	BOOL 		_fDialogIsUp;
	BOOL		_fPreviewIsOn;
	BOOL		_fOverlayIsOn;
	BOOL		_fSeqIsOn;
	int			_previewRate;
	DWORD		_biBitsCountSupported;
	CDWordArray	_formats;
	CDWordArray	_cmprs_formats;
	BOOL		_1FrameCallBackInstalled;
	CSize		_smallFrame;
	int			_internalBufCnt;
	BOOL		_fShow;
	int			iLastError;
	BOOL		_autosize;
  	PROGRESSCALLBACK	_progressCB;
	DRAWCALLBACK		_drwcb;

	int			_GetInternalBuffersCount();
	BOOL		_IsImageCompressed();

virtual	void	_notify(LPSTR lp=NULL) {if(_progressCB) (*_progressCB)(lp,100);}
virtual	void	_notify(LPSTR lp, DWORD pc) {if(_progressCB) (*_progressCB)(lp,pc);}
	
	void		_selfInit();
	void		_getDrvList();
	BOOL		_getCaps();
inline	BOOL	_getStatus()	{return capGetStatus (GetSafeHwnd(), &_capStatus, sizeof (CAPSTATUS));}
inline	BOOL	_getSetup()		{return capCaptureGetSetup (GetSafeHwnd(), &_captureParms, sizeof CAPTUREPARMS);}
inline	BOOL	_setSetup()		{return capCaptureSetSetup (GetSafeHwnd(), &_captureParms, sizeof CAPTUREPARMS);}

	BOOL	_getFormat();
	BOOL	_connect2Driver(int indx);
	BOOL	_testBiBitsCount();
	BOOL	_testValidFormats();
	BOOL	_testInternalBuffers();
	BOOL	_testBitsFormat(UINT bits);
	BOOL	_testFrameFormat(int x, int y);

	LPBITMAPINFO	_mk_LPBITMAPINFO(UINT bitsReqCode,int x, int y);
	int			_getBMIsize(UINT bitsReq, UINT bmiColors);
	int			_getBMIsize(LPBITMAPINFO);
	DWORD		_calcBufferSize(LPBITMAPINFOHEADER bih);
	
	void		_pushResolution();
	void		_popResolution();
	void		_autoSize();

inline	int	_totalDrv()	{return _drvNames.GetSize();}
protected:
	//{{AFX_MSG(AviCap)
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//Error codes
#define	CAP_CREATE_DUP				-1	/*can't connect twice!*/
#define	CAP_WINCREATION_FAILED		-2	/*failed to create capture window*/
#define	CAP_NO_CAPDEVICE_INSTALLED	-3	/*no capture device found*/
#define	CAP_DRIVERNAME_NOT_FOUND	-4	/*no capture driver found*/
#define	CAP_ILLEGAL_DRIVERID		-5	/*out of drivers list*/
#define	CAP_CONNECTION_FAILED		-6	/*no comments*/
#define	CAP_INIT_FAILED				-7	/*initialization step failed*/
#define	CAP_TEST_FAILED				-8	/*can't test device*/
#define	CAP_CALLNOWINDOW			-9	/*window not created*/
#define	CAP_CALLNOCONNECTION		-10	/*no connection with capture driver*/


