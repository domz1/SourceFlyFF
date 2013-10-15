#ifndef __HELPER_WEB_BOX_H__
#define __HELPER_WEB_BOX_H__

#ifdef __NEW_WEB_BOX
#ifdef __CLIENT

class CHelperWebBox
{
public:
	enum { HELPER_WEB_SIZE_NORMAL_X = 790, HELPER_WEB_SIZE_NORMAL_Y = 570 };
	enum { HELPER_WEB_SIZE_LARGE_X = 1014, HELPER_WEB_SIZE_LARGE_Y = 738 };

public:
	CHelperWebBox( void );
	virtual ~CHelperWebBox( void );

public:
	static CHelperWebBox* GetInstance( void );

public:
	void Initialize( void );
	void Process( void );
	void Release( void );

public:
	void InitializeWebInformation( void );
	void Close( void );

public:
	void GoFirstPage( void );

public:
	const CString& GetHelperWebAddress( void ) const;
	const HWND& GetParentHandle( void ) const;
	const HWND& GetDialogHandle( void ) const;
	const HWND& GetDialogBoxHandle( void ) const;

private:
	CString m_strHelperWebAddress;
	HINSTANCE m_hInstance;
	HWND m_hParent;
	HWND m_hDialog;
	HWND m_hDialogBox;
	CRect m_rectHelperWeb;
};

#endif // __CLIENT
#endif // __NEW_WEB_BOX

#endif // __HELPER_WEB_BOX_H__