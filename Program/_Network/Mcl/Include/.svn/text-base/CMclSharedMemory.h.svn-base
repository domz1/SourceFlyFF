//
// FILE: CMclSharedMemory.h
//
// Copyright (c) 1997 by Aaron Michael Cohen and Mike Woodring
//
/////////////////////////////////////////////////////////////////////////
#ifndef __CMCLSHAREDMEMORY_H__
#define __CMCLSHAREDMEMORY_H__

#include "CMclGlobal.h"

class CMclSharedMemory {
private:
    void *m_lpSharedMemory;
    HANDLE m_hMapping;
    BOOL m_bIsCreator;
    DWORD m_dwStatus;
    
public:
    // constructor requires a size with an optional name, it will
    // create the shared memory area if it doesn't already exist,
    // if it does exist and the requested size is larger than the current
    // size, the size of the shared memory area will grow to the requested size...
    CMclSharedMemory( LONG lSize, LPCTSTR lpName, LPSECURITY_ATTRIBUTES lpSharedMemoryAttributes = NULL);
    
    // constructor requires just a name, opens the shared memory
    // area if it exists, you must check the status of this object to
    // see if it succeeded or failed, it will NOT throw an exception if
    // the named shared memory area does not exist...
    CMclSharedMemory( LPCTSTR lpName);

    // destructor is virtual to allow easy use of
    // derived classes...
    virtual ~CMclSharedMemory();

    // query for status after construction if exceptions are disabled...
    DWORD Status(void) const;

    // basic access function returns a void pointer
    // to the shared memory, derived classes can add functions
    // to return pointers to higher level structures or structure
    // members by first getting the base pointer through this function...
    void *GetPtr(void);

    // function to tell if this object created the shared memory area...
    BOOL IsCreator(void);
};

#endif
