#include "stdafx.h"
#include "NewTexture.h"
#include "targa.h"
#include "..\_UnhandledException\ExceptionHandler.h"


HRESULT DemoApp::LoadResourceBitmap(
    ID2D1RenderTarget *pRenderTarget,
    IWICImagingFactory *pIWICFactory,
    PCWSTR resourceName,
    PCWSTR resourceType,
    ID2D1Bitmap **ppBitmap
    )
{
    HRESULT hr = S_OK;
    IWICBitmapDecoder *pDecoder = NULL;
    IWICBitmapFrameDecode *pSource = NULL;
    IWICStream *pStream = NULL;
    IWICFormatConverter *pConverter = NULL;

    HRSRC imageResHandle = NULL;
    HGLOBAL imageResDataHandle = NULL;
    void *pImageFile = NULL;
    DWORD imageFileSize = 0;

    // Locate the resource.
    imageResHandle = FindResourceW(HINST_THISCOMPONENT, resourceName, resourceType);

    hr = imageResHandle ? S_OK : E_FAIL;
    if (SUCCEEDED(hr))
    {
        // Load the resource.
        imageResDataHandle = LoadResource(HINST_THISCOMPONENT, imageResHandle);

        hr = imageResDataHandle ? S_OK : E_FAIL;
    }

    if (SUCCEEDED(hr))
    {
        // Lock it to get a system memory pointer.
        pImageFile = LockResource(imageResDataHandle);

        hr = pImageFile ? S_OK : E_FAIL;
    }

    if (SUCCEEDED(hr))
    {
        // Calculate the size.
        imageFileSize = SizeofResource(HINST_THISCOMPONENT, imageResHandle);

        hr = imageFileSize ? S_OK : E_FAIL;
    }

    if (SUCCEEDED(hr))
    {
        // Create a WIC stream to map onto the memory.
        hr = pIWICFactory->CreateStream(&pStream);
    }

    if (SUCCEEDED(hr))
    {
        // Initialize the stream with the memory pointer and size.
        hr = pStream->InitializeFromMemory(
            reinterpret_cast<BYTE*>(pImageFile),
            imageFileSize
            );
    }

    if (SUCCEEDED(hr))
    {
        // Create a decoder for the stream.
        hr = pIWICFactory->CreateDecoderFromStream(
            pStream,
            NULL,
            WICDecodeMetadataCacheOnLoad,
            &pDecoder
            );
    }

    if (SUCCEEDED(hr))
    {
        // Create the initial frame.
        hr = pDecoder->GetFrame(0, &pSource);
    }

    if (SUCCEEDED(hr))
    {
        // Convert the image format to 32bppPBGRA
        // (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
        hr = pIWICFactory->CreateFormatConverter(&pConverter);
    }

    if (SUCCEEDED(hr))
    {
        hr = pConverter->Initialize(
            pSource,
            GUID_WICPixelFormat32bppPBGRA,
            WICBitmapDitherTypeNone,
            NULL,
            0.f,
            WICBitmapPaletteTypeMedianCut
            );
    }

    if (SUCCEEDED(hr))
    {
        // Create a Direct2D bitmap from the WIC bitmap.
        hr = pRenderTarget->CreateBitmapFromWicBitmap(
            pConverter,
            NULL,
            ppBitmap
            );
    }

    SafeRelease(&pDecoder);
    SafeRelease(&pSource);
    SafeRelease(&pStream);
    SafeRelease(&pConverter);

    return hr;
}