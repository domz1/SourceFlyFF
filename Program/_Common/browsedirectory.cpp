///////////////////////////////////////////////////////////////////
// Implementation File BrowseDirectory.cpp

#include "stdafx.h"
#include <io.h>
#include "BrowseDirectory.h"


///////////////////////////////////////////////////////////////////
// Default messages
static const TCHAR DEFAULT_TITLE  [] = _T("Select a Directory");
static const TCHAR FILES_FOUND    [] = _T("Files exist");
static const TCHAR FILES_NOT_FOUND[] = _T("Files don't exist");

///////////////////////////////////////////////////////////////////
CWizBrowseDirectory::~CWizBrowseDirectory() {
}


///////////////////////////////////////////////////////////////////
void CWizBrowseDirectory::InitBrowseInfo(BROWSEINFO& bi, CWnd* pFromWnd,
LPCTSTR Title) {
        memset(&bi, 0, sizeof(bi));
    bi.hwndOwner = pFromWnd->GetSafeHwnd();
    bi.pidlRoot = NULL;
    bi.lpszTitle = (NULL != Title) ? Title : DEFAULT_TITLE;
    bi.ulFlags = BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS;
    bi.lpfn = NULL;
    bi.lParam = 0;
}

///////////////////////////////////////////////////////////////////
CString CWizBrowseDirectory::Do (CWnd* pFromWnd, LPCTSTR Title) {
        CString retStr;
        BrowseDir(retStr, pFromWnd, Title);
        return retStr;
}

///////////////////////////////////////////////////////////////////
void CWizBrowseDirectory::BrowseDir(CString& retStr, CWnd* pFromWnd, LPCTSTR
Title) {
    LPMALLOC pMalloc;
    /* Get's the Shell's default allocator */
    if (NOERROR != ::SHGetMalloc(&pMalloc)) AfxThrowMemoryException();

        BROWSEINFO   bi;
    TCHAR        pszBuffer[MAX_PATH];
    LPITEMIDLIST pidl;

    InitBrowseInfo(bi, pFromWnd, Title);
    bi.pszDisplayName = pszBuffer;

    // This next call issues the dialog box 
  __try {
                if ((pidl = ::SHBrowseForFolder(&bi)) != NULL) {
          __try {
                if (::SHGetPathFromIDList(pidl, pszBuffer)) { 
                    //At this point pszBuffer contains the selected path */
                    retStr = pszBuffer;
                                }
                        }
          __finally {
                                // Free the PIDL allocated by SHBrowseForFolder
                pMalloc->Free(pidl);
            }
        }
    }
  __finally {
                // Release the shell's allocator 
        pMalloc->Release();
        pMalloc = NULL;
        }
}

///////////////////////////////////////////////////////////////////
CWizBrowseDirForFiles::CWizBrowseDirForFiles(LPCTSTR FileWldcrt, LPCTSTR
MsgFilesExist, LPCTSTR MsgFilesNotExist)
                : m_strFileWldcrt(FileWldcrt), m_MsgFilesExist
(MsgFilesExist), m_MsgFilesNotExist(MsgFilesNotExist) {
        if (NULL == MsgFilesExist)    m_MsgFilesExist = FILES_FOUND;
        if (NULL == MsgFilesNotExist) m_MsgFilesNotExist = FILES_NOT_FOUND;
}

///////////////////////////////////////////////////////////////////
int CALLBACK CWizBrowseDirForFiles_CallBack(HWND hwnd, UINT uMsg, LPARAM
lParam, LPARAM lpData) {
        return ((CWizBrowseDirForFiles *)lpData)->CallBack(hwnd, uMsg,lParam);
}

///////////////////////////////////////////////////////////////////
void CWizBrowseDirForFiles::InitBrowseInfo(BROWSEINFO& bi, CWnd* pFromWnd,
LPCTSTR Title) {
        CWizBrowseDirectory::InitBrowseInfo(bi, pFromWnd,Title);
        bi.lpfn = CWizBrowseDirForFiles_CallBack;
        bi.lParam = LPARAM(this);
    bi.ulFlags |= BIF_STATUSTEXT;
}

///////////////////////////////////////////////////////////////////
int CWizBrowseDirForFiles::CallBack(HWND hwnd, UINT uMsg, LPARAM lParam) {
        WPARAM w = 0;
        if (BFFM_SELCHANGED == uMsg) {
                TCHAR Buffer[MAX_PATH*2];
                LPITEMIDLIST pidl = LPITEMIDLIST(lParam);
                if (::SHGetPathFromIDList(pidl, Buffer)) {
                        TRACE("\nDir is %s", Buffer);
                        w = TestFile(Buffer);
                }
                else {
                        TRACE("\nError in SHGetPathFromIDList from CallBack lParam=%uld %p ", lParam, LPVOID(lParam));
                }
        }

        FileExists(hwnd, w);
        return 0;       
}

///////////////////////////////////////////////////////////////////
int CWizBrowseDirForFiles::TestFile(TCHAR* pStr) {
        // Caution here!
        // pStr is only MAX_PATH*2 long!
   _tcscat(pStr, _T("\\"));
   _tcscat(pStr, m_strFileWldcrt);
        WIN32_FIND_DATA fd;
        HANDLE h = ::FindFirstFile(pStr, &fd);
        if(INVALID_HANDLE_VALUE == h) return 0;

        FindClose(h);
        return 1;
}

///////////////////////////////////////////////////////////////////
void CWizBrowseDirForFiles::FileExists(HWND hwnd, WPARAM w) {
        //      @#$$% Microsoft! The documentation is wrong!    
  ::SendMessage(hwnd, BFFM_ENABLEOK, w, w);
  ::SendMessage(hwnd, BFFM_SETSTATUSTEXT, 0, LPARAM((w) ? m_MsgFilesExist :
m_MsgFilesNotExist));
}
