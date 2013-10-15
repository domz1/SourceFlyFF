// inputinternaladdrdlg.cpp : implementation file
//

#include "stdafx.h"
#include "certifier.h"
#include "inputinternaladdrdlg.h"

/////////////////////////////////////////////////////////////////////////////
// CInputInternalAddrDlg dialog


CInputInternalAddrDlg::CInputInternalAddrDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInputInternalAddrDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInputInternalAddrDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CInputInternalAddrDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInputInternalAddrDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInputInternalAddrDlg, CDialog)
	//{{AFX_MSG_MAP(CInputInternalAddrDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInputInternalAddrDlg message handlers
