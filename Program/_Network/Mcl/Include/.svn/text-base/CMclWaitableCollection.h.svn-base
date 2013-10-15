//
// FILE: CMclWaitableCollection.h
//
// Copyright (c) 1997 by Aaron Michael Cohen and Mike Woodring
//
/////////////////////////////////////////////////////////////////////////
#ifndef __CMCLWAITABLECOLLECTION_H__
#define __CMCLWAITABLECOLLECTION_H__

#include "CMclGlobal.h"
#include "CMclWaitableObject.h"

class CMclWaitableCollection {
private:
    HANDLE m_aObjectHandles[MAXIMUM_WAIT_OBJECTS];
    LONG m_lObjects;

public:
    CMclWaitableCollection();

    // destructor does nothing, simply a placeholder
    // for derived class virtual destructors...
    virtual ~CMclWaitableCollection();

    // copy constructor...
    CMclWaitableCollection(CMclWaitableCollection & rhs);

    // assignment operator...
    CMclWaitableCollection & operator= (CMclWaitableCollection & rhs);

    // get the number of handles in the collection...
    LONG GetCount(void) const;

    // add the handle from a pointer to a waitable object to our collection...
    BOOL AddObject(const CMclWaitableObject *pObject);

    // add the handle from a reference to a waitable object to our collection...
    BOOL AddObject(const CMclWaitableObject & rObject);

    // add a raw handle to our collection...
    BOOL AddObject(const HANDLE hHandle);

    // add collection adds all the objects from the given collection
    // to this collection...
    BOOL AddCollection(const CMclWaitableCollection & rCollection);

    // wait for some handles in the collection to become signaled
    // or the timeout to expire...
    DWORD Wait( BOOL bWaitAll, DWORD dwMilliseconds) const;
};

#endif