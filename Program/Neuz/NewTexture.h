// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved

#pragma once

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER              // Allow use of features specific to Windows 7 or later.
#define WINVER 0x0700       // Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT        // Allow use of features specific to Windows 7 or later.
#define _WIN32_WINNT 0x0700 // Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef UNICODE
#define UNICODE
#endif

#define WIN32_LEAN_AND_MEAN     // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <wchar.h>
#include <math.h>

#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
//#include <wincodec.h>

/******************************************************************
*                                                                 *
*  Macros                                                         *
*                                                                 *
******************************************************************/

template<class Interface>
inline void
SafeRelease(
    Interface **ppInterfaceToRelease
    )
{
    if (*ppInterfaceToRelease != NULL)
    {
        (*ppInterfaceToRelease)->Release();

        (*ppInterfaceToRelease) = NULL;
    }
}

#ifndef Assert
#if defined( DEBUG ) || defined( _DEBUG )
#define Assert(b) do {if (!(b)) {OutputDebugStringA("Assert: " #b "\n");}} while(0)
#else
#define Assert(b)
#endif //DEBUG || _DEBUG
#endif


#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif


/******************************************************************
*                                                                 *
*  DemoApp                                                        *
*                                                                 *
******************************************************************/

class DemoApp
{
public:
    DemoApp();
    ~DemoApp();

    HRESULT Initialize();

    void RunMessageLoop();

private:
    HRESULT CreateDeviceIndependentResources();
    void DiscardDeviceIndependentResources();
    HRESULT CreateDeviceResources();
    void DiscardDeviceResources();

    HRESULT OnRender();

    HRESULT LoadResourceBitmap(
        ID2D1RenderTarget *pRT,
        IWICImagingFactory *pIWICFactory,
        PCWSTR resourceName,
        PCWSTR resourceType,
        __deref_out ID2D1Bitmap **ppBitmap
        );

    void OnResize(
        UINT width,
        UINT height
        );

    static LRESULT CALLBACK WndProc(
        HWND hWnd,
        UINT message,
        WPARAM wParam,
        LPARAM lParam
        );

private:
    HWND m_hwnd;

    ID2D1Factory *m_pDirect2DFactory;
    ID2D1HwndRenderTarget *m_pRenderTarget;
    ID2D1SolidColorBrush *m_pBlackBrush;

	// Rectangle Geometry
	ID2D1RectangleGeometry *m_pRectGeo;    

    // WIC and Bitmaps
    IWICImagingFactory *m_pWICFactory;

    ID2D1Bitmap *m_pBitmapMask;

    ID2D1Bitmap *m_pFernBitmap;
    ID2D1Bitmap *m_pLinearFadeFlowersBitmap;
    ID2D1Bitmap *m_pRadialFadeFlowersBitmap;

    // Gradients
    ID2D1LinearGradientBrush *m_pLinearGradientBrush;
    ID2D1RadialGradientBrush *m_pRadialGradientBrush;

    // Bitmap brushes
    ID2D1BitmapBrush *m_pBitmapMaskBrush;
    ID2D1BitmapBrush *m_pFernBitmapBrush;
    ID2D1BitmapBrush *m_pLinearFadeFlowersBitmapBrush;
    ID2D1BitmapBrush *m_pRadialFadeFlowersBitmapBrush;

    IDWriteFactory *m_pDirectWriteFactory;
};
