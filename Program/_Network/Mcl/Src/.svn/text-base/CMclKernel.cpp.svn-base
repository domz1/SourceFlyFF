//
// FILE: CMclKernel.cpp
//
// Copyright (c) 1997 by Aaron Michael Cohen
//
/////////////////////////////////////////////////////////////////////////

#include "CMclKernel.h"
#include "CMclAutoPtr.h"

CMclKernel::CMclKernel() {
    m_hHandle = NULL;
    m_dwStatus = ERROR_INVALID_HANDLE;
}

CMclKernel::~CMclKernel() {
    if (CMclIsValidHandle(m_hHandle)) {
        ::CloseHandle(m_hHandle);
        m_hHandle = NULL;
    }
}

void CMclKernel::ThrowError( DWORD dwStatus) {
    CMclThrowError(dwStatus);
}

DWORD CMclKernel::Status(void) const {
    return m_dwStatus;
}

DWORD CMclKernel::Wait( DWORD dwMilliseconds) {
    return ::WaitForSingleObject( m_hHandle, dwMilliseconds);
}

// wait on the current object and one other...    
DWORD CMclKernel::WaitForTwo( CMclWaitableObject &rCMclWaitableObject, BOOL bWaitAll, DWORD dwMilliseconds) {
    HANDLE handles[2];

    // the current object...
    handles[0] = m_hHandle;

    // the parameter object...
    handles[1] = rCMclWaitableObject.GetHandle();

    // wait for the objects...
    return ::WaitForMultipleObjects( 2, handles, bWaitAll, dwMilliseconds);
}

HANDLE CMclKernel::GetHandle(void) const {
    if (this != NULL)
        return m_hHandle;
    else
        return NULL;
}

CMclKernel::operator HANDLE() const {
    return GetHandle();
}

