// ConvertDlg.cpp : implementation file
//

#include "stdafx.h"

#ifdef __H0503
#include "databaseserver.h"
#include "ConvertDlg.h"

#define TIMER		100
#define PROGRESS	101

void CALLBACK EXPORT TimerProc(
							   HWND hWnd,      // handle of CWnd that called SetTimer
							   UINT nMsg,      // WM_TIMER
							   UINT nIDEvent,   // timer identification
							   DWORD dwTime    // system time
							   );
/////////////////////////////////////////////////////////////////////////////
// CConvertDlg dialog


CConvertDlg::CConvertDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConvertDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConvertDlg)
	m_Total = _T("");
	m_strTime = _T("");
	//}}AFX_DATA_INIT
	m_time = CTime::GetCurrentTime();

	m_bInit = FALSE;
}


void CConvertDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConvertDlg)
	DDX_Control(pDX, IDC_PROGRESS, m_Progress);
	DDX_Text(pDX, IDC_STATIC_TOTAL, m_Total);
	DDX_Text(pDX, IDC_STATIC_TIME, m_strTime);
	DDX_Control(pDX, IDC_DATAGRID4, m_grid);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConvertDlg, CDialog)
	//{{AFX_MSG_MAP(CConvertDlg)
	ON_WM_TIMER()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConvertDlg message handlers

void CConvertDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	if (nIDEvent == PROGRESS)
	{
		m_Progress.SetRange32(0, m_cv->m_pRs.GetRecordCount());
		m_Progress.SetPos(m_cv->m_iCursor);

		CString	temp;
		temp.Format("%d/%d", m_cv->m_iCursor, m_cv->m_pRs.GetRecordCount());
		SetDlgItemText(IDC_STATIC_TOTAL, temp);
		temp.Empty();
//*
		if (m_cv->m_pRs.IsOpen() && m_cv->m_iCursor == m_cv->m_pRs.GetRecordCount())
		{
			m_grid.SetCaption("Result");
			m_grid.SetRefDataSource(NULL);
			m_grid.SetRefDataSource((LPUNKNOWN)m_cv->m_pRs.GetRecordset());
			m_grid.Refresh();
		//	m_grid.Scroll(0, m_cv->m_pRs.GetRecordCount());

			KillTimer(TIMER);
			KillTimer(PROGRESS);
		}//*/
	}
	
	CDialog::OnTimer(nIDEvent);
}

BOOL CConvertDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_cv		= new CADOConvert;
	m_cv->DBConnect();

	m_Thread	= new CMclThread(m_cv, THREAD_PRIORITY_NORMAL);

	SetTimer(TIMER, 1000, TimerProc);
	SetTimer(PROGRESS, 1, NULL);

	m_bInit		= TRUE;

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CConvertDlg::OnOK() 
{
	// TODO: Add extra validation here
	
//	CDialog::OnOK();
}

void CConvertDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

void CALLBACK EXPORT TimerProc(
							   HWND hWnd,      // handle of CWnd that called SetTimer
							   UINT nMsg,      // WM_TIMER
							   UINT nIDEvent,   // timer identification
							   DWORD dwTime    // system time
							   )
{
	static CTime time = CTime::GetCurrentTime();

	switch(nIDEvent)
	{
	case TIMER:
		{//*
			CTimeSpan		ts		= CTime::GetCurrentTime() - time;
			CString			strTime = "\0";

			sprintf(const_cast<char*>((LPCTSTR)strTime), "%s", ts.Format("%M:%S"));
			SetDlgItemText(hWnd, IDC_STATIC_TIME, strTime);
			
			strTime.Empty();
		}
		break;
	}
}

void CConvertDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
//*	
	if (m_bInit)
	{
		CRect	rect, rect1;

		GetClientRect(&rect);
		m_grid.GetClientRect(rect1);

		rect1.top		= rect.top + 28;
		rect1.right		= rect.right;
		rect1.bottom	= rect.bottom;

		m_grid.MoveWindow(rect1);
	}//*/
	

//	GetDlgItem(IDC_DATAGRID3)->MoveWindow()
}

#endif	// __H0503
