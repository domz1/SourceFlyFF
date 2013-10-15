// CClipboard Class
// ----------------
// Written by David Terracino <davet@lycosemail.com>
//
// This code is released into the public domain, because
// it's silly easy. If you want to use and expand it, go
// ahead; fine by me. If you do expand this class's
// functionality, please consider sending it back up to
// the MFC Programmer's Sourcebook at www.codeguru.com.
//
// And as always, please give credit where credit is
// due. Don't remove my name from the source.

#include "stdafx.h"
#include "Clipboard.h"

////////////////////////////////////////////////////////////////////
// GetText
// - Retrieves text from the clipboard
////////////////////////////////////////////////////////////////////
//
// Parameters:
//	lpszBuffer - pointer to a string where the text is to be put
//	nBufSize   - allocated length of lpszBuffer
//	hWnd       - window handle to be used by clipboard
//
// Return Values:
//	TRUE       - Text was successfully copied from clipboard
//	FALSE      - No text on the clipboard
//
////////////////////////////////////////////////////////////////////

BOOL CClipboard::GetText (LPSTR lpszBuffer, int nBufSize, HWND hWnd)
{
	HGLOBAL hGlobal;		// Global memory handle
	LPSTR lpszData;			// Pointer to clipboard data
	unsigned long nSize;	// Size of clipboard data

	// First, open the clipboard. OpenClipboard() takes one
	// parameter, the handle of the window that will temporarily
	// be it's owner. If NULL is passed, the current process
	// is assumed.
	OpenClipboard(hWnd);

	// Request a pointer to the text on the clipboard.
	hGlobal = GetClipboardData(CF_TEXT);

	// If there was no text on the clipboard, we have
	// been returned a NULL handle.	
	if (hGlobal == NULL) return FALSE;

	// Now we have a global memory handle to the text
	// stored on the clipboard. We have to lock this global
	// handle so that we have access to it.
	lpszData = (LPSTR)GlobalLock(hGlobal);
	// Now get the size of the text on the clipboard.
	nSize = GlobalSize(hGlobal);

	// Make sure the text on the clipboard is not longer
	// that the buffer that was allocated for it. If it was
	// snip the text on the clipboard so that it fits.
	if(nSize >= (UINT)nBufSize) nSize = nBufSize - 1;

	// Now, copy the text into the return buffer. At the
	// end, we need to add a NULL string terminator.
	for (UINT i = 0; i < nSize; ++i)
		*(lpszBuffer + i) = *(lpszData + i);
	*(lpszBuffer + i) = 0;

	// Now, simply unlock the global memory pointer
	// and close the clipboard.
	GlobalUnlock(hGlobal);
	CloseClipboard();

	return TRUE;
}

////////////////////////////////////////////////////////////////////
// GetTextLength
// - Retrieves length of text on the clipboard
////////////////////////////////////////////////////////////////////
//
// Parameters:
//	pnSize     - pointer to unsigned long that will receive
//               the length of the text on the clipboard.
//               NOTE: Does not include NULL terminator.
//	hWnd       - window handle to be used by clipboard
//
// Return Values:
//	TRUE       - Text length was successfully returned.
//	FALSE      - No text on the clipboard
//
////////////////////////////////////////////////////////////////////

BOOL CClipboard::GetTextLength (DWORD *pnSize, HWND hWnd)
{
	HGLOBAL hGlobal;		// Global memory handle
	DWORD nSize;	// Size of clipboard data
	LPSTR lpszData;			// Pointer to clipboard data

	// First, open the clipboard. OpenClipboard() takes one
	// parameter, the handle of the window that will temporarily
	// be it's owner. If NULL is passed, the current process
	// is assumed.
	OpenClipboard(hWnd);

	// Request a pointer to the text on the clipboard.
	hGlobal = GetClipboardData(CF_TEXT);

	// If there was no text on the clipboard, we have
	// been returned a NULL handle.	
	if (hGlobal == NULL) 
	{
		*pnSize = 0;
		return FALSE;
	}
	// Now we have a global memory handle to the text
	// stored on the clipboard. We have to lock this global
	// handle so that we have access to it.
	lpszData = (LPSTR)GlobalLock(hGlobal);
	// Now get the size of the text on the clipboard.
	nSize = GlobalSize(hGlobal);

	// Now, simply unlock the global memory pointer
	// and close the clipboard.
	GlobalUnlock(hGlobal);
	CloseClipboard();

	// Finally, save the length of the string we found
	// into the pnSize pointer and return.
	*pnSize = nSize;
	return TRUE;
}

////////////////////////////////////////////////////////////////////
// SetText
// - Places text on the clipboard
////////////////////////////////////////////////////////////////////
//
// Parameters:
//	lpszBuffer - pointer to a string where the text is to be put
//	hWnd       - window handle to be used by clipboard
//
// Return Values:
//	TRUE       - Text was successfully copied from clipboard
//	FALSE      - No text on the clipboard
//
////////////////////////////////////////////////////////////////////

int CClipboard::SetText (LPCTSTR lpszBuffer, HWND hWnd)
{
	HGLOBAL hGlobal;		// Global memory handle
	LPSTR lpszData;			// Pointer to clipboard data
	unsigned long nSize;	// Size of clipboard data

	// First, open the clipboard. OpenClipboard() takes one
	// parameter, the handle of the window that will temporarily
	// be it's owner. If NULL is passed, the current process
	// is assumed. After opening, empty the clipboard so we
	// can put our text on it.
	OpenClipboard(hWnd);
	EmptyClipboard();

	// Get the size of the string in the buffer that was
	// passed into the function, so we know how much global
	// memory to allocate for the string.
	nSize = lstrlen(lpszBuffer);

	// Allocate the memory for the string.
	hGlobal = GlobalAlloc(GMEM_ZEROINIT, nSize+1);
	
	// If we got any error during the memory allocation,
	// we have been returned a NULL handle.
	if (hGlobal == NULL) return FALSE;

	// Now we have a global memory handle to the text
	// stored on the clipboard. We have to lock this global
	// handle so that we have access to it.
	lpszData = (LPSTR)GlobalLock(hGlobal);

	// Now, copy the text from the buffer into the allocated
	// global memory pointer.
	for (UINT i = 0; i < nSize + 1; ++i)
		*(lpszData + i) = *(lpszBuffer + i);

	// Now, simply unlock the global memory pointer,
	// set the clipboard data type and pointer,
	// and close the clipboard.
	GlobalUnlock(hGlobal);
	SetClipboardData(CF_TEXT, hGlobal);
	CloseClipboard();

	return TRUE;
}
