// VerifyPlayerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "databaseserver.h"
#include "VerifyPlayerDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CVerifyPlayerDlg dialog


CVerifyPlayerDlg::CVerifyPlayerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CVerifyPlayerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CVerifyPlayerDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CVerifyPlayerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVerifyPlayerDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CVerifyPlayerDlg, CDialog)
	//{{AFX_MSG_MAP(CVerifyPlayerDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVerifyPlayerDlg message handlers
