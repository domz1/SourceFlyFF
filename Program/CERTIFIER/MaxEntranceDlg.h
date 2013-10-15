#if !defined(AFX_MAXENTRANCEDLG_H__701D9A00_D901_4B48_84A4_D8D2CCEEAD4F__INCLUDED_)
#define AFX_MAXENTRANCEDLG_H__701D9A00_D901_4B48_84A4_D8D2CCEEAD4F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MaxEntranceDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMaxEntranceDlg dialog

class CMaxEntranceDlg : public CDialog
{
// Construction
public:
	CMaxEntranceDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMaxEntranceDlg)
	enum { IDD = IDD_DIALOG_MAX_ENTRANCE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMaxEntranceDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMaxEntranceDlg)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAXENTRANCEDLG_H__701D9A00_D901_4B48_84A4_D8D2CCEEAD4F__INCLUDED_)
