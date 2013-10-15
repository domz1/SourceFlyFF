//
// FILE: CMclThread.h
//
// Copyright (c) 1997 by Aaron Michael Cohen and Mike Woodring
//
/////////////////////////////////////////////////////////////////////////
#ifndef __CMCLTHREAD_H__
#define __CMCLTHREAD_H__

#include "CMclGlobal.h"
#include "CMclKernel.h"

// forward declaration for CMclThreadHandler...
class CMclThread;

// CMclThreadHandler encapsulates the thread procedure for a 
// CMclThread object, each instantiation of a thread handler can
// be used by multiple threads at a time but the base CMclThreadHandler
// class does NOT provide any internal synchronization for multiple threads
// using a single instance simultaneously. A derived class could provide
// this internal synchronization however...
class CMclThreadHandler {

public:
    // destructor does nothing, it is simply a placeholder for
    // ensure the destructors of derived classes are virtual...
    virtual ~CMclThreadHandler();

    // This is a pure virtual function with no implementation
    // it must be implemented in a derived class.
    // The "this" object
    // inside ThreadHandlerProc() will be the CMclThreadHandler derived
    // object itself.
    // The procedure should return the exit code of the thread when finished...
    virtual unsigned ThreadHandlerProc(void) = 0;
};

class CMclThread : public CMclKernel {
protected:
    unsigned int m_uiThreadID;
    CMclThreadHandler *m_pcThreadHandler;

public:
    // only the thread handler reference needs to 
    // be supplied since the other arguments have default values...
    CMclThread( CMclThreadHandler *pcThreadHandler, 
                unsigned uInitFlag = 0, LPSECURITY_ATTRIBUTES lpSecurity = NULL, unsigned uStackSize = 0);
    
    // suspend the thread...
    DWORD Suspend(void);

    // resume the thread...
    DWORD Resume(void);

    // terminate the thread...
    BOOL Terminate( DWORD dwExitCode);

    // read a thread's exit code...
    BOOL GetExitCode( DWORD *pdwExitCode);

    // set a thread's priority...
    BOOL SetPriority( int nPriority);

    // read a thread's priority...
    int GetPriority(void);

    // get the internal thread id...
    DWORD GetThreadId(void);

private:
    // this is a static function used to kick-start the thread handler...
    static unsigned _stdcall CallThreadHandlerProc(void *pThreadHandler);
};

#endif
