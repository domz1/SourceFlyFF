// AudioStream.cpp

//#define VC_EXTRALEAN
//#define STRICT

//#include <mmsystem.h>
#include "stdafx.h"
#include "assert.h"
#include "debug.h"
#include "AudioStream.h" 

// Pointer to DirectSound object.
extern LPDIRECTSOUND    gpds; 

// AudioStreamServices class implementation
//
////////////////////////////////////////////////////////////

// Constructor
AudioStreamServices::AudioStreamServices (void)
{
	DOUT ("AudioStreamServices::AudioStreamServices\n\r");
	m_hwnd = g_Neuz.GetSafeHwnd();

    // It would seem to make sense to initialize DirectSound here,
    // but because there could be an error, it's best done in a
    // separate member function, ::Initialize.
}

// Destructor
AudioStreamServices::~AudioStreamServices (void)
{
    DOUT ("AudioStreamServices::~AudioStreamServices\n\r");
}


//
// AudioStream class implementation
//
////////////////////////////////////////////////////////////

// The following constants are the defaults for our streaming buffer operation.
const UINT DefBufferLength          = 2000; // default buffer length in msec
const UINT DefBufferServiceInterval = 250;  // default buffer service interval in msec

// Constructor
AudioStream::AudioStream (void)
{
    DOUT ("AudioStream::AudioStream\n\r");

    // Initialize data members
    m_pass = NULL;
    m_pwavefile = NULL;
    m_pdsb = NULL;
    m_ptimer = NULL;
    m_fPlaying = m_fCued = FALSE;
    m_lInService = FALSE;
    m_cbBufOffset = 0;
    m_nBufLength = DefBufferLength;
    m_cbBufSize = 0;
    m_nBufService = DefBufferServiceInterval;
    m_nDuration = 0;
    m_nTimeStarted = 0;
    m_nTimeElapsed = 0;
	m_nVolume = 127;
	
}


// Destructor
AudioStream::~AudioStream (void)
{
    DOUT ("AudioStream::~AudioStream\n\r");

    Destroy ();
}

// Create
BOOL AudioStream::Create (LPSTR pszFilename, AudioStreamServices * pass, BOOL Repeat)
{
    BOOL fRtn = SUCCESS;    // assume success
    
    ASSERT (pszFilename);
    ASSERT (pass);

    DOUT ("AudioStream::Create\n\r");

    // pass points to AudioStreamServices object
    m_pass = pass;
		m_Repeat = Repeat;

    if (pszFilename && m_pass && gpds)
    {
        // Create a new WaveFile object
        if (m_pwavefile = new WaveFile)
        {
            // Open given file
            if (m_pwavefile->Open (pszFilename))
            {
                // Calculate sound buffer size in bytes
                // Buffer size is average data rate times length of buffer
                // No need for buffer to be larger than wave data though
                m_cbBufSize = (m_pwavefile->GetAvgDataRate () * m_nBufLength) / 1000;
                m_cbBufSize = (m_cbBufSize > m_pwavefile->GetDataSize ()) ? m_pwavefile->GetDataSize () : m_cbBufSize;

                DOUT1 ("average data rate = %d\n\r", m_pwavefile->GetAvgDataRate ());
                DOUT1 ("m_cbBufSize = %d\n\r", m_cbBufSize);

                // Get duration of sound
                m_nDuration = m_pwavefile->GetDuration ();
                
                // Create sound buffer
                HRESULT hr;
                memset (&m_dsbd, 0, sizeof (DSBUFFERDESC));
                m_dsbd.dwSize = sizeof (DSBUFFERDESC);
                m_dsbd.dwBufferBytes = m_cbBufSize;
			    m_dsbd.dwFlags = DSBCAPS_CTRLVOLUME;
                m_dsbd.lpwfxFormat = m_pwavefile->m_pwfmt;
                hr = gpds->CreateSoundBuffer (&m_dsbd, &m_pdsb, NULL);
                if (hr == DS_OK)
                {
                    // Cue for playback
                    Cue ();
                }
                else
                {
                    // Error, unable to create DirectSound buffer
                    DOUT ("Error, unable to create DirectSound buffer\n\r");
                    if (hr == DSERR_BADFORMAT)
                    {
                        DOUT ("    Bad format (probably ADPCM)\n\r");
                    }
                    
                    fRtn = FALSE;
                }
            }
            else
            {
                // Error opening file
                DOUT1 ("Failed to open wave file: %s\n\r", pszFilename);
                safe_delete( m_pwavefile );
                m_pwavefile = NULL;
                fRtn = FALSE;
            }   
        }
        else
        {
            // Error, unable to create WaveFile object
            DOUT1 ("Failed to create WaveFile object %s\n\r", pszFilename);
            fRtn = FALSE;
        }
    }
    else
    {
        // Error, passed invalid parms
        fRtn = FALSE;
    }

    return (fRtn);
}


// Destroy
BOOL AudioStream::Destroy ( void )
{
  BOOL fRtn = SUCCESS;

  DOUT ("AudioStream::Destroy\n\r");

  // Stop playback
  Stop ();
    
  // Delete WaveFile object
  if (m_pwavefile)
  {
    safe_delete( m_pwavefile );
    m_pwavefile = NULL;
  }

  // Release DirectSound buffer
  if( m_pdsb )
  {
    m_pdsb->Release ();
    m_pdsb = NULL;			// 왜 에러 나지!!!!!!!!!!!!!!!
  }

  return fRtn;
}

// WriteWaveData
//
// Writes wave data to sound buffer. This is a helper method used by Create and
// ServiceBuffer; it's not exposed to users of the AudioStream class.
BOOL AudioStream::WriteWaveData (UINT size)
{
    HRESULT hr;
    LPBYTE lpbuf1 = NULL;
    LPBYTE lpbuf2 = NULL;
    DWORD dwsize1 = 0;
    DWORD dwsize2 = 0;
    DWORD dwbyteswritten1 = 0;
    DWORD dwbyteswritten2 = 0;
    BOOL fRtn = SUCCESS;

    DOUT1 ("AudioStream::WriteWaveData: %d bytes\n\r", size);

//  DWORD dwWriteCursor, dwPlayCursor;
//  m_pdsb->GetCurrentPosition (&dwPlayCursor, &dwWriteCursor);
//  DOUT2 ("    dwPlayCursor = %d, dwWriteCursor = %d\n\r", dwPlayCursor, dwWriteCursor);

    DOUT1 ("m_cbBufOffset = %d\n\r", m_cbBufOffset);
    
    // Lock the sound buffer
    hr = m_pdsb->Lock (m_cbBufOffset, size, (LPVOID*)&lpbuf1, &dwsize1, (LPVOID*)&lpbuf2, &dwsize2, 0);
/*
           if (lpDirectSoundBuffer->Lock(
                    0,
                    dwDataSize,
                    (LPLPVOID)&lpData,
                    &dwBSize,
                    &lpWrapPtr,
                    &dwWrapBSize,
                    0L) != 0)
*/
    if( hr == DS_OK && m_pwavefile )
    {
        // Write data to sound buffer. Because the sound buffer is circular, we may have to
        // do two write operations if locked portion of buffer wraps around to start of buffer.
        ASSERT (lpbuf1);
        if ((dwbyteswritten1 = m_pwavefile->Read (lpbuf1, dwsize1)) == dwsize1)
        {
            // Second write required?
            if (lpbuf2)
            {
                if ((dwbyteswritten2 = m_pwavefile->Read (lpbuf2, dwsize2)) == dwsize2)
                {
                    // Both write operations successful!
                }
                else
                {
                    // Error, didn't read wave data completely
                    DOUT ("Error, read FALSE\n\r");
                    fRtn = FALSE;
                }
            }
        }
        else
        {
            // Error, didn't read wave data completely
            DOUT ("Error, read FALSE\n\r");
            fRtn = FALSE;
        }

        // Update our buffer offset and unlock sound buffer
        m_cbBufOffset = (m_cbBufOffset + dwbyteswritten1 + dwbyteswritten2) % m_cbBufSize;
        m_pdsb->Unlock (lpbuf1, dwbyteswritten1, lpbuf2, dwbyteswritten2);
    }
    else
    {
        // Error locking sound buffer
        DOUT ("Error, unable to lock sound buffer\n\r");
        fRtn = FALSE;
    }

    DOUT1 ("m_cbBufOffset = %d\n\r", m_cbBufOffset);

    return (fRtn);
}


// WriteSilence
//
// Writes silence to sound buffer. This is a helper method used by
// ServiceBuffer; it's not exposed to users of the AudioStream class.
BOOL AudioStream::WriteSilence (UINT size)
{
    HRESULT hr;
    LPBYTE lpbuf1 = NULL;
    LPBYTE lpbuf2 = NULL;
    DWORD dwsize1 = 0;
    DWORD dwsize2 = 0;
    DWORD dwbyteswritten1 = 0;
    DWORD dwbyteswritten2 = 0;
    BOOL fRtn = SUCCESS;

    DOUT1 ("AudioStream::WriteSilence: %d bytes\n\r", size);

    // Lock the sound buffer
    hr = m_pdsb->Lock (m_cbBufOffset, size, (LPVOID*)&lpbuf1, &dwsize1, (LPVOID*)&lpbuf2, &dwsize2, 0);
    if (hr == DS_OK)
    {
        // Get silence data for this file format. Although word sizes vary for different
        // wave file formats, ::Lock will always return pointers on word boundaries.
        // Because silence data for 16-bit PCM formats is 0x0000 or 0x00000000, we can
        // get away with writing bytes and ignoring word size here.
        BYTE bSilence = m_pwavefile->GetSilenceData ();
        
        // Write silence to sound buffer. Because the sound buffer is circular, we may have to
        // do two write operations if locked portion of buffer wraps around to start of buffer.
        memset (lpbuf1, bSilence, dwsize1);
        dwbyteswritten1 = dwsize1;
            
        // Second write required?
        if (lpbuf2)
        {
            memset (lpbuf1, bSilence, dwsize1);
            dwbyteswritten2 = dwsize2;
        }
            
        // Update our buffer offset and unlock sound buffer
        m_cbBufOffset = (m_cbBufOffset + dwbyteswritten1 + dwbyteswritten2) % m_cbBufSize;
        m_pdsb->Unlock (lpbuf1, dwbyteswritten1, lpbuf2, dwbyteswritten2);
    }
    else
    {
        // Error locking sound buffer
        DOUT ("Error, unable to lock sound buffer\n\r");
        fRtn = FALSE;
    }

    return (fRtn);
}


// GetMaxWriteSize
//
// Helper function to calculate max size of sound buffer write operation, i.e. how much
// free space there is in buffer.
DWORD AudioStream::GetMaxWriteSize (void)
{
    DWORD dwWriteCursor, dwPlayCursor, dwMaxSize;

    DOUT ("AudioStream::GetMaxWriteSize\n\r");

    // Get current play position
    if (m_pdsb->GetCurrentPosition (&dwPlayCursor, &dwWriteCursor) == DS_OK)
    {
        if (m_cbBufOffset <= dwPlayCursor)
        {
            // Our write position trails play cursor
            dwMaxSize = dwPlayCursor - m_cbBufOffset;
        }

        else // (m_cbBufOffset > dwPlayCursor)
        {
            // Play cursor has wrapped
            dwMaxSize = m_cbBufSize - m_cbBufOffset + dwPlayCursor;
        }

    }
    else
    {
        // GetCurrentPosition call failed
        ASSERT (0);
        dwMaxSize = 0;
    }

    DOUT2 ("dwPlayCursor = %d, m_cbBufOffset = %d\n\r", dwPlayCursor, m_cbBufOffset);
    DOUT1 ("max write size = %d\n\r", dwMaxSize);
    
    return (dwMaxSize);
}


// ServiceBuffer
//
// Routine to service buffer requests initiated by periodic timer.
//
// Returns TRUE if buffer serviced normally; otherwise returns FALSE.
BOOL AudioStream::ServiceBuffer (void)
{
    BOOL fRtn = TRUE;

    DOUT ("AudioStream::ServiceBuffer\n\r");

    // Check for reentrance
    if (InterlockedExchange (&m_lInService, TRUE) == FALSE)
    {
        // Not reentered, proceed normally

        // Maintain elapsed time count
        m_nTimeElapsed = timeGetTime () - m_nTimeStarted;
//      DOUT1 ("m_nTimeElapsed = %d msec\n\r", m_nTimeElapsed);

        // Stop if all of sound has played
        if (m_nTimeElapsed < m_nDuration)
        {
            // All of sound not played yet, send more data to buffer
            DWORD dwFreeSpace = GetMaxWriteSize ();

            // Determine free space in sound buffer
            if (dwFreeSpace)
            {
                // See how much wave data remains to be sent to buffer
                DWORD dwDataRemaining = m_pwavefile->GetNumBytesRemaining ();
                if (dwDataRemaining == 0)
                {
                    // All wave data has been sent to buffer
                    // Fill free space with silence
                    if (WriteSilence (dwFreeSpace) == FALSE)
                    {
                        // Error writing silence data
                        fRtn = FALSE;
                        ASSERT (0);
                        DOUT ("WriteSilence failed\n\r");
                    }
                }
                else if (dwDataRemaining >= dwFreeSpace)
                {
                    // Enough wave data remains to fill free space in buffer
                    // Fill free space in buffer with wave data
                    if (WriteWaveData (dwFreeSpace) == FALSE)
                    {
                        // Error writing wave data
                        fRtn = FALSE;
                        ASSERT (0);
                        DOUT ("WriteWaveData failed\n\r");
                    }
                }
                else
                {
                    // Some wave data remains, but not enough to fill free space
                    // Send wave data to buffer, fill remainder of free space with silence
                    if (WriteWaveData (dwDataRemaining) == SUCCESS)
                    {
                        if (WriteSilence (dwFreeSpace - dwDataRemaining) == FALSE)
                        {
                            // Error writing silence data
                            fRtn = FALSE;
                            ASSERT (0);
                            DOUT ("WriteSilence failed\n\r");
                        }
                    }
                    else
                    {
                        // Error writing wave data
                        fRtn = FALSE;
                        ASSERT (0);
                        DOUT ("WriteWaveData failed\n\r");
                    }
                }
            }
            else
            {
                // No free space in buffer for some reason
                fRtn = FALSE;
            }
        }
        else
        {
          // All of sound has played, m_Repeat값이 TRUE면 다시 반복 재생하거나 아니면 stop playback
				  Stop();

					if( m_Repeat==TRUE ) Play();
        }

        // Reset reentrancy semaphore
        InterlockedExchange (&m_lInService, FALSE);
    }
    else
    {
        // Service routine reentered. Do nothing, just return
        fRtn = FALSE;
    }
    return (fRtn);
}


// Cue
void AudioStream::Cue (void)
{
    DOUT ("AudioStream::Cue\n\r");
    
    if (!m_fCued)
    {
        // Reset buffer ptr
        m_cbBufOffset = 0;

        // Reset file ptr, etc
        m_pwavefile->Cue ();

        // Reset DirectSound buffer
        m_pdsb->SetCurrentPosition (0);

        // Fill buffer with wave data
        WriteWaveData (m_cbBufSize);
        
        m_fCued = TRUE;
    }
}


// Play
void AudioStream::Play (void)
{
    DOUT ("AudioStream::Play\n\r");

    if (m_pdsb)
    {
        // If playing, stop
        if (m_fPlaying)
        {
            Stop ();
        }

        // Cue for playback if necessary
        if (!m_fCued)
        {
            Cue ();
        }

        // Begin DirectSound playback
        HRESULT hr = m_pdsb->Play (0, 0, DSBPLAY_LOOPING);
        if (hr == DS_OK)
        {
            m_nTimeStarted = timeGetTime ();
            
            // Kick off timer to service buffer
            m_ptimer = new Timer ();
            if (m_ptimer)
            {
                m_ptimer->Create (m_nBufService, m_nBufService, DWORD (this), TimerCallback);
            }
            else
            {
            }

            // Playback begun, no longer cued
            m_fPlaying = TRUE;
            m_fCued = FALSE;
        }
        else
        {
            DOUT ("Error, play failed\n\r");
        }
    }
}

// Timer callback for Timer object created by ::Play method.
BOOL AudioStream::TimerCallback (DWORD dwUser)
{
    // dwUser contains ptr to AudioStream object
    AudioStream * pas = (AudioStream *) dwUser;

    return (pas->ServiceBuffer ());
}

// Stop
void AudioStream::Stop (void)
{
    DOUT ("AudioStream::Stop\n\r");

    if (m_fPlaying)
    {
        // Stop DirectSound playback
        if( m_pdsb ) m_pdsb->Stop ();

        // Delete Timer object
        safe_delete( m_ptimer );
        m_ptimer = NULL;

        m_fPlaying = FALSE;
    }
}

// Volume
void	AudioStream :: SetVolume( int nVolume )
{
    DOUT ("AudioStream::Volume\n\r");

    if (m_fPlaying)
    {
        if( m_pdsb )
		{
			HRESULT		ret;

			if ( nVolume )
				ret = m_pdsb->SetVolume( -((127-nVolume)<<4) );
			else
				ret = m_pdsb->SetVolume( -10000 );
			
			m_nVolume = nVolume;
#ifdef	_DEBUG
			switch( ret )
			{
			case	DS_OK:		ret = 0;	break;
			case	DSERR_ALLOCATED:		ret = 0;	break;
			case	DSERR_ALREADYINITIALIZED :		ret = 0;	break;
			case	DSERR_BADFORMAT :		ret = 0;	break;
			case	DSERR_BUFFERLOST :		ret = 0;	break;
			case	DSERR_CONTROLUNAVAIL :		ret = 0;	break;
			case	DSERR_GENERIC :		ret = 0;	break;
			case	DSERR_INVALIDCALL :		ret = 0;	break;
			case	DSERR_INVALIDPARAM :		ret = 0;	break;
			case	DSERR_NOAGGREGATION :		ret = 0;	break;
			case	DSERR_NODRIVER :		ret = 0;	break;
			case	DSERR_NOINTERFACE :		ret = 0;	break;
			case	DSERR_OTHERAPPHASPRIO :		ret = 0;	break;
			case	DSERR_OUTOFMEMORY :		ret = 0;	break;
			case	DSERR_PRIOLEVELNEEDED :		ret = 0;	break;
			case	DSERR_UNINITIALIZED :		ret = 0;	break;
			case	DSERR_UNSUPPORTED :		ret = 0;	break;
			}
#endif
		}
    }
}

