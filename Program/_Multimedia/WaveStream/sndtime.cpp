
#include "stdafx.h"	// MFC에서는 항상 "stdafx.h"를 먼저 써줘야 한다(-_-;)
#include <mmsystem.h>
#include "sndtime.h" 

DWORD   sTime;      // System Time
DWORD   gTime;      // Game Time    (Measured from game start)
float   lFps;       // Last Frame per Sec
DWORD   lTpf;       // Last Time per Frame    

DWORD   Timer;      // For Timer Count
DWORD   Interval;   // Timer Interval

TIMERFUNC   TimerFunc;  // Timer Function ptr

void    SetTimer(DWORD interval, TIMERFUNC timerfunc)
{
    Interval = interval;
    TimerFunc = timerfunc;
    Timer = 0;
}

void    InitTime(void)
{
    sTime = timeGetTime();
    gTime = 0;
    srand( (unsigned)time( NULL ) );
}

void    UpdateTime(void)
{
    DWORD   ctime;
    while (1)
    {
        ctime = timeGetTime();
        if (ctime-sTime+lTpf >33) break;
    } 

    lTpf += ctime - sTime;
    sTime = ctime;
    gTime += lTpf;
    lFps = 1000 / (float)lTpf;

    Timer += lTpf;
	if (TimerFunc) {
		while (Timer > Interval)
		{
			TimerFunc();
			Timer -= Interval;
		}
	}
    lTpf = 0;
}

void StopTimer(void)
{
    DWORD   ctime = timeGetTime();

    lTpf += ctime - sTime;
}

void ResumeTimer(void)
{
    sTime = timeGetTime();
}

DWORD   STime(void)
{
    return sTime;
}

DWORD   GTime(void)
{
    return gTime;
}

float   FPS(void)
{
    return lFps;
}

DWORD   TPF(void)
{
    return lTpf;
}

#ifdef  _NPDEBUG

void    RefreshFPS(void)
{
    BackOut(0, 0, "%4.1f", lFps);
}

#endif
