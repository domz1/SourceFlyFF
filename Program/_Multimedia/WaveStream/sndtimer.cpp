// Timer.cpp

//#define VC_EXTRALEAN
//#define STRICT

#include "stdafx.h"		// MFC에서는 항상 "stdafx.h"를 먼저 써줘야 한다(-_-;)
#include "assert.h"
#include "debug.h"
#include <mmsystem.h>
#include "sndTimer.h"
int g_nTimer;
// constructor
Timer::Timer (void)
{

    DOUT ("Timer::Timer\n\r");

    m_nIDTimer = NULL;
}


// Destructor
Timer::~Timer (void)
{
    DOUT ("Timer::~Timer\n\r");

    if (m_nIDTimer)
    {
        if(timeKillEvent (m_nIDTimer) != TIMERR_NOERROR )
				{
					ASSERT(0);
				}
				g_nTimer--;
    }
}


// Create
BOOL Timer::Create (UINT nPeriod, UINT nRes, DWORD dwUser, TIMERCALLBACK pfnCallback)
{
    BOOL bRtn = SUCCESS;    // assume success
    
    DOUT ("Timer::Create\n\r");

    ASSERT (pfnCallback);
    ASSERT (nPeriod > 10);
    ASSERT (nPeriod >= nRes);

    m_nPeriod = nPeriod;
    m_nRes = nRes;
    m_dwUser = dwUser;
    m_pfnCallback = pfnCallback;
		g_nTimer++;
    if ((m_nIDTimer = timeSetEvent (m_nPeriod, m_nRes, TimeProc, (DWORD) this, TIME_PERIODIC)) == NULL)
    {
        bRtn = FALSE;
    }

    return (bRtn);
}


// Timer proc for multimedia timer callback set with timeSetTime().
//
// Calls procedure specified when Timer object was created. The 
// dwUser parameter contains "this" pointer for associated Timer object.
// 
void CALLBACK Timer::TimeProc(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
	if(!g_nTimer) return;
    // dwUser contains ptr to Timer object
    Timer * ptimer = (Timer *) dwUser;

    // Call user-specified callback and pass back user specified data
    (ptimer->m_pfnCallback) (ptimer->m_dwUser);
}
