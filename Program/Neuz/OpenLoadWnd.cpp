#include "stdafx.h"
#include "OpenLoadWnd.h"

#pragma comment	(lib, "Msimg32.lib")

#ifdef __CSC_VER9_4

COpenLoadWnd g_OpLoadWnd;

COpenLoadWnd::COpenLoadWnd()
{
}

COpenLoadWnd::~COpenLoadWnd()
{
}

LRESULT CALLBACK OpenWndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	HBITMAP hBit, hOBit;
	HDC     MemDC;
	PAINTSTRUCT  ps;
	HDC     hdc;
	CString strPath;
//	BLENDFUNCTION bf;  
	
	switch(uMsg)
	{
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			
			MemDC = CreateCompatibleDC(hdc);
			strPath = MakePath( DIR_THEME, "openning1.image" );
			hBit = (HBITMAP)LoadImage( NULL, strPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE );

			hOBit = (HBITMAP)SelectObject(MemDC, hBit);
			
			//BitBlt(hdc, 0, 0, 640, 480, MemDC, 0, 0, SRCCOPY);
			TransparentBlt(hdc, 0, 0, 640, 480, MemDC, 0, 0, 640, 480, RGB(255, 0, 255));
			SetTextColor(hdc, RGB(255, 255, 255));
			SetBkMode(hdc, TRANSPARENT);

			SelectObject(MemDC, hOBit);
			DeleteObject(hBit);
			DeleteDC(MemDC);
			
			EndPaint(hWnd, &ps);
			break;
		case WM_ERASEBKGND:
			return (LRESULT)1;
	}

    return DefWindowProc( hWnd, uMsg, wParam, lParam );
}

HRESULT COpenLoadWnd::Create( HINSTANCE hInstance )
{
	HRESULT hr = S_OK;

	m_hInstance = hInstance;
    WNDCLASS wndClass = { CS_DBLCLKS, OpenWndProc, 0, 0, hInstance, NULL, NULL,
                         (HBRUSH)GetStockObject(WHITE_BRUSH), NULL, _T("OpenLoadWindow") };
    RegisterClass( &wndClass );
	
	DWORD dwWindowStyle = WS_POPUPWINDOW;
	dwWindowStyle &= ~WS_BORDER;

	m_hWnd = CreateWindow( _T("OpenLoadWindow"), "", dwWindowStyle, CW_USEDEFAULT, CW_USEDEFAULT, 640, 480, 
									0, LoadMenu( hInstance, MAKEINTRESOURCE(IDR_MENU) ), hInstance, 0 );

	if(m_hWnd != NULL)
	{
		RECT openningWndrc;
		SetRect(&openningWndrc, 0, 0, 640, 480);
		
		// rc를 기준으로 윈도우 전체 크기를 계산한다.
		if( AdjustWindowRectEx( &openningWndrc, GetWindowLong(m_hWnd, GWL_STYLE),
			::GetMenu(m_hWnd)?TRUE:FALSE, GetWindowLong(m_hWnd, GWL_EXSTYLE) ) )
		{
			// rc에는 새로 만들어져야 할 윈도우 전체 크기가 들어있게된다.
			// 윈도우를 화면 중앙에 배치하자
			int  x, y;
			
			x = (GetSystemMetrics(SM_CXSCREEN) - (openningWndrc.right-openningWndrc.left)) / 2 - 201;	// 게임가드 스플레쉬화면238x147 만큼 움직임
			y = (GetSystemMetrics(SM_CYSCREEN) - (openningWndrc.bottom-openningWndrc.top)) / 2 - 195;
			if( x < 0 )
				x = 0;
			if( y < 0 )
				y = 0;
			
			::MoveWindow(m_hWnd, x, y, openningWndrc.right-openningWndrc.left, openningWndrc.bottom-openningWndrc.top, TRUE);
		} 

		::ShowWindow(m_hWnd, SW_SHOW);
		UpdateWnd();
	}
	else
		hr = S_FALSE;

    return hr;
}

void COpenLoadWnd::UpdateWnd()
{
	::UpdateWindow(m_hWnd);
}

#endif //__CSC_VER9_4
