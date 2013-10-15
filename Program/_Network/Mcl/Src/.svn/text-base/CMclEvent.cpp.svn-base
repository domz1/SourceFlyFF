//
// FILE: CMclEvent.cpp
//
// Copyright (c) 1997 by Aaron Michael Cohen
//
/////////////////////////////////////////////////////////////////////////

#include "CMclEvent.h"

// constructor creates an event object...
CMclEvent::CMclEvent( BOOL bManualReset, BOOL bInitialState, LPCTSTR lpName, 
                        LPSECURITY_ATTRIBUTES lpEventAttributes) {
    m_hHandle = ::CreateEvent( lpEventAttributes, bManualReset, bInitialState, lpName);
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

// constructor opens an existing named event...
CMclEvent::CMclEvent( LPCTSTR lpName, BOOL bInheritHandle, DWORD dwDesiredAccess) {
    m_hHandle = ::OpenEvent( dwDesiredAccess, bInheritHandle, lpName);
    if (CMclIsValidHandle(m_hHandle)) {
        m_dwStatus = NO_ERROR;
    }
    else {
        m_dwStatus = GetLastError();
    }
}

// operations on event object...
BOOL CMclEvent::Set(void) {
    return ::SetEvent(m_hHandle);
}

BOOL CMclEvent::Reset(void) {
    return ::ResetEvent(m_hHandle);
}

BOOL CMclEvent::Pulse(void) {
    return ::PulseEvent(m_hHandle);
}

