//
// FILE: CMclAutoPtr.h
//
// Copyright (c) 1997 by Aaron Michael Cohen and Mike Woodring
//
/////////////////////////////////////////////////////////////////////////
#ifndef __CMCLAUTOPTR_H__
#define __CMCLAUTOPTR_H__

#include "CMclGlobal.h"
#include "CMclWaitableObject.h"
#include "CMclKernel.h"
#include "CMclThread.h"
#include "CMclMutex.h"
#include "CMclSemaphore.h"
#include "CMclEvent.h"

class CMclKernelAutoPtr {
protected:
    CMclKernel *m_pObjectPtr;

public:
    // constructor...
    CMclKernelAutoPtr(CMclKernel *pObjectPtr = NULL);

    // destructor is pure virtual to make this class abstract...
    virtual ~CMclKernelAutoPtr() = 0;

    // dereferencing operator...
    CMclKernel * operator->() const;

    // indirection operator...
    CMclKernel & operator*() const;

    // get the handle of the internal object...
    HANDLE GetHandle(void) const;

    // read the current status of the internal object...
    DWORD Status(void) const;

    // test the internal pointer for NULL...
    BOOL IsNull(void) const;

protected:
    // Reset member function performs pointer assignment
    // is protected so internal pointer cannot be of a different
    // type for derived classes...
    void Reset(CMclKernel *pObjectPtr);

private:
    // these functions have no implementation since they can 
    // never be called...

    // copying and passing by copy are not allowed...
    // this prevents confusion of internal object ownership...
    CMclKernelAutoPtr(CMclKernelAutoPtr & rhs);

    // assigning one auto pointer to another is not allowed,
    // this prevents confusion of internal object ownership...
    CMclKernelAutoPtr & operator= (CMclKernelAutoPtr & rhs);
};

// all of our auto pointers are instances of this template class
// which is derived from the CMclKernelAutoPtr abstract base class...
template <class T>
class CMclDerivedAutoPtr : public CMclKernelAutoPtr {
public:
    // constructor...
    CMclDerivedAutoPtr(T *pObjectPtr = NULL) : CMclKernelAutoPtr(pObjectPtr) {
    };

    // pointer assignment...
    CMclDerivedAutoPtr<T> & operator= (T *pObjectPtr) {
        Reset(pObjectPtr);
        return *this; 
    };

    // dereferencing operator...
    T * operator->() const {
        // we can safely return a casted pointer because
        // we know the pointer type was checked by the constructor
        // at compile time...
        return static_cast<T *>(m_pObjectPtr); 
    };

    // indirection operator...
    T & operator*() const {
        // we can safely return a casted pointer because
        // we know the pointer type was checked by the constructor
        // at compile time...
        return *(static_cast<T *>(m_pObjectPtr));

    };
};

// typedef's for the autopointers we need in the library...
typedef CMclDerivedAutoPtr<CMclThread> CMclThreadAutoPtr;
typedef CMclDerivedAutoPtr<CMclMutex> CMclMutexAutoPtr;
typedef CMclDerivedAutoPtr<CMclSemaphore> CMclSemaphoreAutoPtr;
typedef CMclDerivedAutoPtr<CMclEvent> CMclEventAutoPtr;

#endif


