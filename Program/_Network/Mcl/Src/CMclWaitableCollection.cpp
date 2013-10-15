//
// FILE: CMclWaitableCollection.cpp
//
// Copyright (c) 1997 by Aaron Michael Cohen and Mike Woodring
//
/////////////////////////////////////////////////////////////////////////

#include "CMclWaitableCollection.h"

CMclWaitableCollection::CMclWaitableCollection() {
    m_lObjects = 0;
}

CMclWaitableCollection::~CMclWaitableCollection() {
    // must implement virtual destructor...
    return;
}

CMclWaitableCollection::CMclWaitableCollection(CMclWaitableCollection & rhs) {
    // copy constructor uses assignment operator...
    *this = rhs;
}

CMclWaitableCollection & CMclWaitableCollection::operator= (CMclWaitableCollection & rhs) { 
    for (int i = 0; i < rhs.m_lObjects; i++) {
        m_aObjectHandles[i] = rhs.m_aObjectHandles[i];
    }
    m_lObjects = rhs.m_lObjects;
    return *this; 
}

LONG CMclWaitableCollection::GetCount(void) const {
    return m_lObjects;
}

BOOL CMclWaitableCollection::AddObject(const CMclWaitableObject *pObject) {
    // make sure that we are not full...
    if (m_lObjects == MAXIMUM_WAIT_OBJECTS)
        return FALSE;

    // add the object's HANDLE to our wait list...
    m_aObjectHandles[m_lObjects++] = pObject->GetHandle();

    return TRUE;
}

BOOL CMclWaitableCollection::AddObject(const CMclWaitableObject & rObject) {
    // make sure that we are not full...
    if (m_lObjects == MAXIMUM_WAIT_OBJECTS)
        return FALSE;

    // add the object's HANDLE to our wait list...
    m_aObjectHandles[m_lObjects++] = rObject.GetHandle();

    return TRUE;
}

BOOL CMclWaitableCollection::AddObject(const HANDLE hHandle) {
    // make sure that we are not full...
    if (m_lObjects == MAXIMUM_WAIT_OBJECTS)
        return FALSE;

    // add the HANDLE to our wait list...
    m_aObjectHandles[m_lObjects++] = hHandle;

    return TRUE;
}

BOOL CMclWaitableCollection::AddCollection( const CMclWaitableCollection & rCollection) {
    for (int i = 0; i < rCollection.m_lObjects; i++) {
        if (!AddObject(rCollection.m_aObjectHandles[i]))
            return FALSE;
    }
    return TRUE;
}

DWORD CMclWaitableCollection::Wait( BOOL bWaitAll, DWORD dwMilliseconds) const {
    // check the number of objects...
    if (m_lObjects == 0) {
        return WAIT_FAILED;
    }
    
    // wait for the objects...
    return ::WaitForMultipleObjects( m_lObjects, m_aObjectHandles, bWaitAll, dwMilliseconds);
}

