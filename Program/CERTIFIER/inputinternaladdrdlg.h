#if !defined(AFX_INPUTINTERNALADDRDLG_H__5FA28345_63E9_4288_83B3_A1CA217C17C8__INCLUDED_)
#define AFX_INPUTINTERNALADDRDLG_H__5FA28345_63E9_4288_83B3_A1CA217C17C8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// inputinternaladdrdlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CInputInternalAddrDlg dialog

class CInputInternalAddrDlg : public CDialog
{
// Construction
public:
	CInputInternalAddrDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CInputInternalAddrDlg)
	enum { IDD = IDD_DIALOG_INTERNAL_IP };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInputInternalAddrDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CInputInternalAddrDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INPUTINTERNALADDRDLG_H__5FA28345_63E9_4288_83B3_A1CA217C17C8__INCLUDED_)
