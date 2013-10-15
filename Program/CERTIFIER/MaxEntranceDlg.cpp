// MaxEntranceDlg.cpp : implementation file
//

#include "stdafx.h"
#include "certifier.h"
#include "MaxEntranceDlg.h"
#include "player.h"
extern	CPlayerMng	g_PlayerMng;

/////////////////////////////////////////////////////////////////////////////
// CMaxEntranceDlg dialog


CMaxEntranceDlg::CMaxEntranceDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMaxEntranceDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMaxEntranceDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CMaxEntranceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMaxEntranceDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMaxEntranceDlg, CDialog)
	//{{AFX_MSG_MAP(CMaxEntranceDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMaxEntranceDlg message handlers

void CMaxEntranceDlg::OnOK() 
{
	
	CDialog::OnOK();
}
