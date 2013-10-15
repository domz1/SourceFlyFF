// SndTime.cpp

typedef void (*TIMERFUNC)(void);

void    InitTime(void);
void    UpdateTime(void);
DWORD   STime(void);
DWORD   GTime(void);
float   FPS(void);
DWORD   TPF(void);

void    SetTimer(DWORD interval, TIMERFUNC timerfunc);

#ifdef  _NPDEBUG
void    RefreshFPS(void);
#endif

void StopTimer(void);
void ResumeTimer(void);
//
//
//
