//
// FILE: CMclThread.cpp
//
// Copyright (c) 1997 by Aaron Michael Cohen and Mike Woodring
//
/////////////////////////////////////////////////////////////////////////

#include "CMclThread.h"

// destructor does nothing, it is simply a placeholder for
// ensure the destructors of derived classes are virtual...
CMclThreadHandler::~CMclThreadHandler() {
    return;
}

// constructor creates a thread C++ object and
// creates the kernel thread object which begins executing
// at the ThreadHandlerProc of the cThreadHandler object...
CMclThread::CMclThread( CMclThreadHandler *pcThreadHandler, 
                        unsigned uInitFlag, 
                        LPSECURITY_ATTRIBUTES lpSecurity, 
                        unsigned uStackSize) : m_pcThreadHandler(pcThreadHandler) {
        
    // create the thread, saving the thread HANDLE, identifier, and
    // setting the status, call the class static function to forward
    // the thread on to the thread handler...
    m_hHandle = (HANDLE) _beginthreadex( lpSecurity, uStackSize, CallThreadHandlerProc, m_pcThreadHandler, uInitFlag, &m_uiThreadID);
    if (CMclIsValidHandle(m_hHandle)) {
        m_dwStatus = NO_ERROR;
    }
    else {
        // throw thread creation error...
        m_dwStatus = errno;
        ThrowError(m_dwStatus);
    }      
}

unsigned _stdcall CMclThread::CallThreadHandlerProc(void *pThreadHandler) {
    // the constructor calls us here to start the thread, we need to
    // use the argument and convert it to a CMclThreadHandler so that
    // we can call the CMclThreadHandler::ThreadHandlerProc() as a member
    // function...
    CMclThreadHandler *pcHandler = static_cast<CMclThreadHandler *>(pThreadHandler);
    
    // call the handler procedure and return the exit code...
    return pcHandler->ThreadHandlerProc();    
}
            
// suspend the thread...
DWORD CMclThread::Suspend(void) {
    return ::SuspendThread( m_hHandle);
}

// resume the thread...
DWORD CMclThread::Resume(void) {
    return ::ResumeThread( m_hHandle);
}

// terminate the thread...
BOOL CMclThread::Terminate( DWORD dwExitCode) {
    return ::TerminateThread( m_hHandle, dwExitCode);
}

// read a thread's exit code...
BOOL CMclThread::GetExitCode( DWORD *pdwExitCode) {
    return ::GetExitCodeThread( m_hHandle, pdwExitCode);
}

// set a thread's priority...
BOOL CMclThread::SetPriority( int nPriority) {
    return ::SetThreadPriority( m_hHandle, nPriority);
}

// read a thread's priority...
int CMclThread::GetPriority( void) {
    return ::GetThreadPriority( m_hHandle);
}

// return the thread's identifier...
DWORD CMclThread::GetThreadId(void) {
    return static_cast<DWORD>(m_uiThreadID);
}
