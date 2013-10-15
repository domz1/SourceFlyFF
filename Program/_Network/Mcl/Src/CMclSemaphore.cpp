//
// FILE: CMclSemaphore.cpp
//
// Copyright (c) 1997 by Aaron Michael Cohen and Mike Woodring
//
/////////////////////////////////////////////////////////////////////////

#include "CMclSemaphore.h"

// constructor creates a semaphore object...
CMclSemaphore::CMclSemaphore( int nInitialCount, int nMaximumCount, LPCTSTR lpName, LPSECURITY_ATTRIBUTES lpSemaphoreAttributes) {
    m_hHandle = ::CreateSemaphore( lpSemaphoreAttributes, nInitialCount, nMaximumCount, lpName);
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

// constructor opens an existing named semaphore...
CMclSemaphore::CMclSemaphore( LPCTSTR lpName, BOOL bInheritHandle, DWORD dwDesiredAccess) {
    m_hHandle = ::OpenSemaphore( dwDesiredAccess, bInheritHandle, lpName);
    if (CMclIsValidHandle(m_hHandle)) {
        m_dwStatus = NO_ERROR;
    }
    else {
        m_dwStatus = GetLastError();
    }
}

// increase the count on a semaphore...
BOOL CMclSemaphore::Release( LONG lReleaseCount, LONG *plPreviousCount) {
    LONG lPreviousCount;
    BOOL bStatus = ::ReleaseSemaphore( m_hHandle, lReleaseCount, &lPreviousCount);
    if (bStatus && plPreviousCount) {
        *plPreviousCount = lPreviousCount;
    }

    return bStatus;
}


