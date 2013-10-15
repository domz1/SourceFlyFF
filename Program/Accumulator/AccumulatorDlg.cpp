// AccumulatorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Accumulator.h"
#include "AccumulatorDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CAccumulatorDlg dialog

CAccumulatorDlg::CAccumulatorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAccumulatorDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAccumulatorDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pdpAccumulator	= NULL;
}

void CAccumulatorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAccumulatorDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAccumulatorDlg, CDialog)
	//{{AFX_MSG_MAP(CAccumulatorDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAccumulatorDlg message handlers

BOOL CAccumulatorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	LOAD_WS2_32_DLL;

	m_pdpAccumulator	= new CDPAccumulator( this );
	if( !m_pdpAccumulator->StartServer( PN_ACCUMULATOR, TRUE ) )
	{
		return FALSE;
	}

	SetTimer( 0, 100, NULL );

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CAccumulatorDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CAccumulatorDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CAccumulatorDlg::OnDestroy() 
{
	CDialog::OnDestroy();

	KillTimer( 0 );

	if( NULL != m_pdpAccumulator )
		m_pdpAccumulator->DeleteDPObject();
	SAFE_DELETE( m_pdpAccumulator );

	SAFE_DELETE( CAr::m_pHeapMng );
	SAFE_DELETE( CClientSock::m_pPool );
	SAFE_DELETE( CClientSockE::m_pPool );
	SAFE_DELETE( CBuffer::m_pPool );
	SAFE_DELETE( CBuffer::m_pHeapMng );

	UNLOAD_WS2_32_DLL;
}

void CAccumulatorDlg::OnTimer(UINT nIDEvent) 
{
	switch( nIDEvent )
	{
		case 0:
			{
				m_pdpAccumulator->ReceiveMessage();
				break;
			}
	}

	CDialog::OnTimer(nIDEvent);
}
