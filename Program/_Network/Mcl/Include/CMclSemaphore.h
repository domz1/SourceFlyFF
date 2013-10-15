//
// FILE: CMclSemaphore.h
//
// Copyright (c) 1997 by Aaron Michael Cohen and Mike Woodring
//
/////////////////////////////////////////////////////////////////////////
#ifndef __CMCLSEMAPHORE_H__
#define __CMCLSEMAPHORE_H__

#include "CMclGlobal.h"
#include "CMclKernel.h"

class CMclSemaphore : public CMclKernel {
public:
    // constructor creates a semaphore object...
    CMclSemaphore( int nInitialCount, int nMaximumCount, LPCTSTR lpName = NULL, LPSECURITY_ATTRIBUTES lpSemaphoreAttributes = NULL);
    
    // constructor opens an existing named semaphore...
    // you must check the status after using this constructor,
    // it will NOT throw an error exception if the object cannot be opened...
    CMclSemaphore( LPCTSTR lpName, BOOL bInheritHandle = FALSE, DWORD dwDesiredAccess = SEMAPHORE_ALL_ACCESS);

    // increase the count on a semaphore...
    BOOL Release( LONG lReleaseCount, LONG *plPreviousCount = NULL);
};

#endif

