//
// FILE: CMclGlobal.h
//
// Copyright (c) 1997 by Aaron Michael Cohen and Mike Woodring
//
/////////////////////////////////////////////////////////////////////////
#ifndef __CMCLGLOBAL_H__
#define __CMCLGLOBAL_H__

#include <windows.h>
#include <process.h>
#include <winerror.h>

// forward class declarations to make the compiler happy...
class CMclWaitableObject;
class CMclWaitableCollection;
class CMclKernel;
class CMclMutex;
class CMclSemaphore;
class CMclEvent;
class CMclThread;
class CMclCritSec;
class CMclAutoLock;
class CMclMonitor;
class CMclSharedMemory;
class CMclMailbox;

// defined symbol determines if CMclThrowError throws exceptions
// or just prints debug error messages...
#ifndef __CMCL_THROW_EXCEPTIONS__
#define __CMCL_THROW_EXCEPTIONS__   TRUE
#endif

// for higher level objects which might have to check internal
// object status when exceptions are disabled, these macros can be useful...

// PTR is the smart pointer to check for NULL, 
// STATUS is the variable in which to store an error code if an error is detected...
#if __CMCL_THROW_EXCEPTIONS__
#define CMCL_CHECK_AUTOPTR_OBJECT(PTR,STATUS) if ((PTR).IsNull()) { CMclThrowError(ERROR_OUTOFMEMORY); }
#else
#define CMCL_CHECK_AUTOPTR_OBJECT(PTR,STATUS) if ((PTR).IsNull()) { (STATUS) = ERROR_OUTOFMEMORY; return; }
#endif

// SCODE is the return value to check,
// STATUS is the variable in which to store an error code if an error is detected...
#if __CMCL_THROW_EXCEPTIONS__
#define CMCL_CHECK_CREATION_STATUS(SCODE,STATUS) {}
#else
#define CMCL_CHECK_CREATION_STATUS(SCODE,STATUS) if (((SCODE)!=NO_ERROR)&&((SCODE)!=ERROR_ALREADY_EXISTS)) { STATUS = (SCODE); return; }
#endif

// error handling macro and function...
#define CMclThrowError(dwStatus) CMclInternalThrowError((dwStatus), __FILE__, __LINE__)
extern void CMclInternalThrowError( DWORD dwStatus, LPCTSTR lpFilename, int line);

// check handle for NULL and INVALID_HANDLE
inline BOOL CMclIsValidHandle( HANDLE hHandle) {
    return ((hHandle != NULL) && (hHandle != INVALID_HANDLE_VALUE));
}

// validate wait return codes...
inline BOOL CMclWaitSucceeded( DWORD dwWaitResult, DWORD dwHandleCount) {
    return ((dwWaitResult >= WAIT_OBJECT_0) &&
            (dwWaitResult < WAIT_OBJECT_0 + dwHandleCount));
}

inline BOOL CMclWaitAbandoned( DWORD dwWaitResult, DWORD dwHandleCount) {
    return ((dwWaitResult >= WAIT_ABANDONED_0) &&
            (dwWaitResult < WAIT_ABANDONED_0 + dwHandleCount));
}

inline BOOL CMclWaitTimeout( DWORD dwWaitResult) {
    return (dwWaitResult == WAIT_TIMEOUT);
}
    
inline BOOL CMclWaitFailed( DWORD dwWaitResult) {
    return (dwWaitResult == WAIT_FAILED);
}

// compute object indices for waits...
inline DWORD CMclWaitSucceededIndex( DWORD dwWaitResult) {
    return (dwWaitResult - WAIT_OBJECT_0);
}

inline DWORD CMclWaitAbandonedIndex( DWORD dwWaitResult) {
    return (dwWaitResult - WAIT_ABANDONED_0);
}

#endif


