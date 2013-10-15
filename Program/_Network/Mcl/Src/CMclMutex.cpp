//
// FILE: CMclMutex.cpp
//
// Copyright (c) 1997 by Aaron Michael Cohen and Mike Woodring
//
/////////////////////////////////////////////////////////////////////////

#include "CMclMutex.h"

// constructor creates a mutex allowing creation parameters to be specified...
CMclMutex::CMclMutex( BOOL bInitialOwner, LPCTSTR lpName, LPSECURITY_ATTRIBUTES lpMutexAttributes) {
    m_hHandle = ::CreateMutex( lpMutexAttributes, bInitialOwner, lpName);
    if (CMclIsValidHandle(m_hHandle)) {
        if (lpName)
            m_dwStatus = GetLastError();
        else
            m_dwStatus = NO_ERROR;
    }
    else {
        m_dwStatus = GetLastError();
        ThrowError(m_dwStatus);
    }
}

// constructor opens an existing named mutex...
CMclMutex::CMclMutex( LPCTSTR lpName, BOOL bInheritHandle, DWORD dwDesiredAccess) {
    m_hHandle = ::OpenMutex( dwDesiredAccess, bInheritHandle, lpName);
    if (CMclIsValidHandle(m_hHandle)) {
        m_dwStatus = NO_ERROR;
    }
    else {
        m_dwStatus = GetLastError();
    }
}

// release a lock on a mutex...
BOOL CMclMutex::Release(void) {
    return ::ReleaseMutex(m_hHandle);
}


