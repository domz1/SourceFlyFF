//
// FILE: CMclCritSec.h
//
// Copyright (c) 1997 by Aaron Michael Cohen and Mike Woodring
//
/////////////////////////////////////////////////////////////////////////
#ifndef __CMCLCRITSEC_H__
#define __CMCLCRITSEC_H__

//#include "CMclGlobal.h"

class CMclCritSec {
private:
    CRITICAL_SECTION m_CritSec;

public:
    // constructor creates a CRITICAL_SECTION inside
    // the C++ object...
    CMclCritSec(void)
    {
		::InitializeCriticalSection( &m_CritSec );
	}

    // destructor...
    virtual ~CMclCritSec()
	{
		::DeleteCriticalSection( &m_CritSec );
	}

    // enter the critical section...
    void Enter(void)
	{
		::EnterCriticalSection( &m_CritSec );
	}

    // leave the critical section...
    void Leave(void)
	{
		::LeaveCriticalSection( &m_CritSec );
	}

    // return a pointer to the internal
    // critical section...
    CRITICAL_SECTION *GetCritSec(void)
	{
		return &m_CritSec;
	}
};

#endif
