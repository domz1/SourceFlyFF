//
// FILE: CMclAutoLock.cpp
//
// Copyright (c) 1997 by Aaron Michael Cohen
//
/////////////////////////////////////////////////////////////////////////

#include "CMclAutoLock.h"

// constructors...
CMclAutoLock::CMclAutoLock( HANDLE hMutexHandle) {
    m_pCritSec = NULL;
    m_pcCritSec = NULL;
    m_pcMutex = NULL;
    m_hMutexHandle = hMutexHandle;
    ::WaitForSingleObject( m_hMutexHandle, INFINITE);
}

CMclAutoLock::CMclAutoLock( CMclMutex & rCMclMutex) {
    m_pCritSec = NULL;
    m_pcCritSec = NULL;
    m_hMutexHandle = NULL;
    m_pcMutex = &rCMclMutex;
    m_pcMutex->Wait(INFINITE);
}

CMclAutoLock::CMclAutoLock( CRITICAL_SECTION * pCritSec) {
    m_hMutexHandle = NULL;
    m_pcMutex = NULL;
    m_pcCritSec = NULL;
    m_pCritSec = pCritSec;
    ::EnterCriticalSection(m_pCritSec);
}

CMclAutoLock::CMclAutoLock( CMclCritSec & rCMclCritSec) {
    m_hMutexHandle = NULL;
    m_pcMutex = NULL;
    m_pCritSec = NULL;
    m_pcCritSec = &rCMclCritSec;
    m_pcCritSec->Enter();
}

// destructor...
CMclAutoLock::~CMclAutoLock(void) {
    BOOL bStatus = TRUE;

    if (m_hMutexHandle) {
        bStatus = ::ReleaseMutex(m_hMutexHandle);
    }
    else if (m_pcMutex) {
        bStatus = m_pcMutex->Release();
    }
    else if (m_pCritSec) {
        ::LeaveCriticalSection(m_pCritSec);
    }
    else {
        m_pcCritSec->Leave();
    }

    if (!bStatus) {
        CMclThrowError( ::GetLastError());
    }
}

