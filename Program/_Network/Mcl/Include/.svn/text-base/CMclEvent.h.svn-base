//
// FILE: CMclEvent.h
//
// Copyright (c) 1997 by Aaron Michael Cohen and Mike Woodring
//
/////////////////////////////////////////////////////////////////////////
#ifndef __CMCLEVENT_H__
#define __CMCLEVENT_H__

#include "CMclGlobal.h"
#include "CMclKernel.h"

class CMclEvent : public CMclKernel {
public:
    // constructor creates an event object...
    CMclEvent( BOOL bManualReset = FALSE, BOOL bInitialState = FALSE, LPCTSTR lpName = NULL, LPSECURITY_ATTRIBUTES lpEventAttributes = NULL);
    
    // constructor opens an existing named event,
    // you must check the status after using this constructor,
    // it will NOT throw an error exception if the object cannot be opened...
    CMclEvent( LPCTSTR lpName, BOOL bInheritHandle = FALSE, DWORD dwDesiredAccess = EVENT_ALL_ACCESS);

    // operations on event object...
    BOOL Set(void);
    BOOL Reset(void);
    BOOL Pulse(void);
};

#endif


