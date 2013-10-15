// AccumulatorDlg.h : header file
//{{AFX_INCLUDES()
#include "richtext.h"
//}}AFX_INCLUDES

#if !defined(AFX_ACCUMULATORDLG_H__0A04461D_FD5B_4EBF_9E61_9FFAA6156D0B__INCLUDED_)
#define AFX_ACCUMULATORDLG_H__0A04461D_FD5B_4EBF_9E61_9FFAA6156D0B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#include "dpaccumulator.h"

/////////////////////////////////////////////////////////////////////////////
// CAccumulatorDlg dialog

class CAccumulatorDlg : public CDialog
{
// Construction
public:
	CAccumulatorDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CAccumulatorDlg)
	enum { IDD = IDD_ACCUMULATOR_DIALOG };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAccumulatorDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	CDPAccumulator*		m_pdpAccumulator;

	// Generated message map functions
	//{{AFX_MSG(CAccumulatorDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ACCUMULATORDLG_H__0A04461D_FD5B_4EBF_9E61_9FFAA6156D0B__INCLUDED_)
