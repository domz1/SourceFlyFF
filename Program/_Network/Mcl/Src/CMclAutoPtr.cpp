//
// FILE: CMclAutoPtr.cpp
//
// Copyright (c) 1997 by Aaron Michael Cohen
//
/////////////////////////////////////////////////////////////////////////

#include "CMclAutoPtr.h"

/////////////////////////////////////////////////////////////////////////
// CMclKernelAutoPtr
/////////////////////////////////////////////////////////////////////////

// can construct a kernel auto pointer with a valid internal object
// or a NULL...
CMclKernelAutoPtr::CMclKernelAutoPtr(CMclKernel *pObjectPtr) { 
    m_pObjectPtr = pObjectPtr; 
}

// deleting the auto pointer deletes what the internal pointer points to...
CMclKernelAutoPtr::~CMclKernelAutoPtr() {
    // no check needed since deleting a NULL
    // pointer is okay in C++...
    safe_delete( m_pObjectPtr ); 
}

void CMclKernelAutoPtr::Reset(CMclKernel *pObjectPtr) {
    // we don't need to check for NULL in C++...
    if (m_pObjectPtr != pObjectPtr)
        safe_delete( m_pObjectPtr );

    m_pObjectPtr = pObjectPtr; 
}

// dereferencing operator...
CMclKernel * CMclKernelAutoPtr::operator->() const { 
    return m_pObjectPtr; 
}

// indirection operator...
CMclKernel & CMclKernelAutoPtr::operator*() const { 
    return *m_pObjectPtr; 
}

// get the handle of the internal object...
HANDLE CMclKernelAutoPtr::GetHandle(void) const {
    return m_pObjectPtr->GetHandle();
}

// read the current status of the internal object...
DWORD CMclKernelAutoPtr::Status(void) const {
    return m_pObjectPtr->Status();
}

BOOL CMclKernelAutoPtr::IsNull(void) const {
    return (m_pObjectPtr == NULL);
}








