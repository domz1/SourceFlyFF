//
// FILE: CMclWaitableObject.h
//
// Copyright (c) 1997 by Aaron Michael Cohen and Mike Woodring
//
/////////////////////////////////////////////////////////////////////////
#ifndef __CMCLWAITABLEOBJECT_H__
#define __CMCLWAITABLEOBJECT_H__

#include "CMclGlobal.h"

class CMclWaitableObject {
    // class has no member data, it's only purpose is to provide
    // a base class for waitable objects which have internal HANDLES
    // and Status...

    // class needs no construtor, since it has no members...

public:
    // get the internal handle...
    // this member function is virtual to assure
    // this function appears in all derived classes
    // and pure (= 0) so that this class cannot be instantiated...
    virtual HANDLE GetHandle(void) const = 0;

    // get the internal object status...
    // this member function is virtual to assure
    // this function appears in all derived classes
    // and pure (= 0) so that this class cannot be instantiated...
    virtual DWORD Status(void) const = 0;
};

#endif
