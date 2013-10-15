// AudioStream.h

#ifndef _INC_AUDIOSTREAM
#define _INC_AUDIOSTREAM

#include <dsound.h>
#include "sndtimer.h"
#include "wavefile.h"

// Constants
#ifndef SUCCESS
#define SUCCESS TRUE        // Error returns for all member functions
#define FAILURE FALSE
#endif // SUCCESS


// Classes

// AudioStreamServices
//
// DirectSound apportions services on a per-window basis to allow
// sound from background windows to be muted. The AudioStreamServices
// class encapsulates the initialization of DirectSound services.
//
// Each window that wants to create AudioStream objects must
// first create and initialize an AudioStreamServices object. 
// All AudioStream objects must be destroyed before the associated 
// AudioStreamServices object is destroyed.
class AudioStreamServices
{
public:
    AudioStreamServices (void);
    ~AudioStreamServices (void);
protected:
    HWND m_hwnd;
};


// AudioStream
//
// Audio stream interface class for playing WAV files using DirectSound.
// Users of this class must create AudioStreamServices object before
// creating an AudioStream object.
//
// Public Methods:
//
// Public Data:
//
class AudioStream
{
public:
    AudioStream (void);
    ~AudioStream (void);
    BOOL Create (LPSTR pszFilename, AudioStreamServices * pass, BOOL Repeat);
    BOOL Destroy ( void );
    void Play (void);
    void Stop (void);
	void	SetVolume( int nVolume );
	BOOL IsPlaying() { return m_fPlaying; };

	int		m_nVolume;

protected:
    void Cue (void);
    BOOL WriteWaveData (UINT cbSize);
    BOOL WriteSilence (UINT cbSize);
    DWORD GetMaxWriteSize (void);
    BOOL ServiceBuffer (void);
    static BOOL TimerCallback (DWORD dwUser);
    AudioStreamServices * m_pass;  // ptr to AudioStreamServices object
    LPDIRECTSOUNDBUFFER m_pdsb;    // sound buffer
    WaveFile * m_pwavefile;        // ptr to WaveFile object
    Timer * m_ptimer;              // ptr to Timer object
    BOOL m_fCued;                  // semaphore (stream cued)
    BOOL m_fPlaying;               // semaphore (stream playing)
    DSBUFFERDESC m_dsbd;           // sound buffer description
    LONG m_lInService;             // reentrancy semaphore
    UINT m_cbBufOffset;            // last write position
    UINT m_nBufLength;             // length of sound buffer in msec
    UINT m_cbBufSize;              // size of sound buffer in bytes
    UINT m_nBufService;            // service interval in msec
    UINT m_nDuration;              // duration of wave file
    UINT m_nTimeStarted;           // time (in system time) playback started
    UINT m_nTimeElapsed;           // elapsed time in msec since playback started

	BOOL m_Repeat;								 // 재생 끝나도 재반복 재생할지 여부
};

#endif // _INC_AUDIOSTREAM 