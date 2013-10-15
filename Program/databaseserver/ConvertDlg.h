//{{AFX_INCLUDES()
#include "datagrid.h"
//}}AFX_INCLUDES
#if !defined(AFX_CONVERTDLG_H__5E87454E_DF2B_49FF_966F_46EFEE3242B7__INCLUDED_)
#define AFX_CONVERTDLG_H__5E87454E_DF2B_49FF_966F_46EFEE3242B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ConvertDlg.h : header file
//
#include "adoconvert.h"
#include "afxdb.h"

/////////////////////////////////////////////////////////////////////////////
// CConvertDlg dialog

class CConvertDlg : public CDialog
{
public:
	CADOConvert*		m_cv;
	CTime				m_time;
	CMclThread*			m_Thread;
	CMclCritSec			m_cs;
	BOOL				m_bInit;


	
// Construction
public:
	CConvertDlg(CWnd* pParent = NULL);   // standard constructor

	friend UINT Proc(LPVOID p_Param);

// Dialog Data
	//{{AFX_DATA(CConvertDlg)
	enum { IDD = IDD_DIALOG_CONVERT };
	CProgressCtrl	m_Progress;
	CString			m_Total;
	CString			m_strTime;
	CDataGrid	m_grid;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConvertDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CConvertDlg)
	afx_msg void OnTimer(UINT nIDEvent);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONVERTDLG_H__5E87454E_DF2B_49FF_966F_46EFEE3242B7__INCLUDED_)
