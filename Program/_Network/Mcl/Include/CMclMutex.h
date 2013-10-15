//
// FILE: CMclMutex.h
//
// Copyright (c) 1997 by Aaron Michael Cohen and Mike Woodring
//
/////////////////////////////////////////////////////////////////////////
#ifndef __CMCLMUTEX_H__
#define __CMCLMUTEX_H__

#include "CMclGlobal.h"
#include "CMclKernel.h"

class CMclMutex : public CMclKernel {
public:
    // constructors create a mutex object...
    CMclMutex( BOOL bInitialOwner = FALSE, LPCTSTR lpName = NULL, LPSECURITY_ATTRIBUTES lpMutexAttributes = NULL);
    
    // constructor opens an existing named mutex...
    // you must check the status after using this constructor,
    // it will NOT throw an error exception if the object cannot be opened...
    CMclMutex( LPCTSTR lpName, BOOL bInheritHandle = FALSE, DWORD dwDesiredAccess = MUTEX_ALL_ACCESS);

    // release a lock on a mutex...
    BOOL Release(void);
};

#endif


