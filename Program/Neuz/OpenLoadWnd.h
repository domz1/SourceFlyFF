#ifdef __CSC_VER9_4

#ifndef __OPENLOADWND__H
#define __OPENLOADWND__H

class COpenLoadWnd
{
protected:
	HWND		m_hWnd; // The main app window
	HINSTANCE	m_hInstance;
public:
	COpenLoadWnd();
	~COpenLoadWnd();

    HRESULT Create( HINSTANCE hInstance );
	HWND GetSafeHwnd() { return m_hWnd; }
	HINSTANCE GetSafeHInstance() { return m_hInstance; }

	void UpdateWnd();
};

extern COpenLoadWnd g_OpLoadWnd;

#endif //__OPENLOADWND__H
#endif //__CSC_VER9_4