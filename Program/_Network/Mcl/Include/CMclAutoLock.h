//
// FILE: CMclAutoLock.h
//
// Copyright (c) 1997 by Aaron Michael Cohen and Mike Woodring
//
/////////////////////////////////////////////////////////////////////////
#ifndef __CMCLAUTOLOCK_H__
#define __CMCLAUTOLOCK_H__


#include "CMclCritSec.h"

class CMclAutoLock {
private:
    CMclCritSec *m_pcCritSec;

public:
    // constructors...
    CMclAutoLock( CMclCritSec & rCMclCritSec )
	{
		m_pcCritSec = &rCMclCritSec;
		m_pcCritSec->Enter();
	}

    // destructor...
    ~CMclAutoLock(void)
	{
		m_pcCritSec->Leave();
	}
};

#endif

