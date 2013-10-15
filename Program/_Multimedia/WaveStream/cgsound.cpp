/*===========================================================================*\
|
|  File:        cgsound.cpp
|
|  Description: 
|   Sample Immortal Klowns game sound effects routines.
|
|   Sound effects are implemented as an object which are
|   loaded from a wave file (.WAV) and can be mixed with 
|   other sounds as well as being algorithmically altered
|   (panned from left to right, pitch changes, and volume 
|   changes).
|
|   For demonstration purposes, sounds may be played using a
|   variety of methods.
|       1) DirectSound - the preferred method, requires
|           that a supported sound board be present
|           and that the DirectSound drivers be installed.
|           Offers greatest flexibility and lowest latency.
|           Doesn't support compressed wave files.
|
|   If desired, individual sounds may be played using different
|   methods (there's no advantage to this, it's just for demos).
|
|-----------------------------------------------------------------------------
|
|  Copyright (C) 1995-1996 Microsoft Corporation.  All Rights Reserved.
|  Written by Moss Bay Engineering, Inc. under contract to Microsoft Corporation
|
|	 Version Upgrade 3DSound & MultyChannel stream by MUGEN	1999
|  Copyright (C) 1999 MUGEN.
\*===========================================================================*/

//** include files **
#include "stdafx.h"
#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include "linklist.h"
#include "cgwave.h"
#include "cgsound.h"
#include "cgglobl.h"


unsigned long g_nDxVer = 0x500;

//** local definitions **
// Default volume and panning values
#define MINFREQ_TB              0
#define MAXFREQ_TB              512
#define MINPAN_TB               0
#define MAXPAN_TB               127
#define MIDPAN_TB               64
#define MINVOL_TB               0
#define MAXVOL_TB               127

#define INITVOL_TB              0
#define INITPAN_TB              0

//** external functions **
//** external data **
//** public data **
BOOL gbQuiet = FALSE;

//** private data **

// To prevent having more than one instance of the same
// sound in memory, a list of all sounds is kept.
static CLinkedList  *mpWaveList = NULL;

// In order to turn off/on sounds we need a list of actively playing
// sounds.
static CLinkedList  *pNowPlayingList = NULL;

// Pointer to DirectSound object.
LPDIRECTSOUND    gpds = NULL;

static LPDIRECTSOUNDBUFFER  lpDSPrimaryBuffer=NULL;

// Handle of WaveOut device.
static HWAVEOUT     hWaveDevice = NULL;

LPDIRECTSOUND3DLISTENER                 glpDirectSound3DListener;

// ----------------------------------------------------------
// CreatePrimarySoundBuffer - create a primary sound buffer 
//  for direct sound.  Every DirectSound app needs to have
//  one (and only one) DirectSound buffer.
// ----------------------------------------------------------
BOOL CreatePrimarySoundBuffer()
{
//    DSBUFFERDESC   dsbd;        // buffer description struct
//    MMRESULT            mmResult;   // result of sound calls

    // Check if we already have a primary buffer
    if (gpds != NULL)
    {
        return(TRUE);
    }

    // Create the Direct Sound Object
    if (DirectSoundCreate(NULL,&gpds, NULL) != 0)
    {
        return(FALSE);
    }
    if( gpds->SetCooperativeLevel( g_Neuz.GetSafeHwnd(), DSSCL_NORMAL ) != DS_OK )
    {
        return(FALSE);
    }

    PCMWAVEFORMAT pcmwf;
    DSBUFFERDESC dsbdesc;
/*
char strVer[10];
itoa( g_nDxVer, strVer, 10 );
MessageBox( NULL, strVer, "DirectSound Error", MB_OK );
*/	
		if( g_nDxVer>=0x500 ) // direct 5.0 이상?
		{
    memset(&pcmwf, 0, sizeof(PCMWAVEFORMAT));
    pcmwf.wf.wFormatTag = WAVE_FORMAT_PCM;
    pcmwf.wf.nChannels = 2;
    pcmwf.wf.nSamplesPerSec = 22050;
    pcmwf.wf.nBlockAlign = 2;
    pcmwf.wf.nAvgBytesPerSec = pcmwf.wf.nSamplesPerSec * pcmwf.wf.nBlockAlign;
    pcmwf.wBitsPerSample = 8;
    memset(&dsbdesc, 0, sizeof(DSBUFFERDESC));
    dsbdesc.dwSize = sizeof(DSBUFFERDESC);
    dsbdesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRL3D | DSBCAPS_CTRLVOLUME;
    dsbdesc.dwBufferBytes = 0; //dwBufferBytes and lpwfxFormat must be set this way.
    dsbdesc.lpwfxFormat = NULL;

    if ( DS_OK == gpds->CreateSoundBuffer( &dsbdesc, &lpDSPrimaryBuffer, NULL) )
    {
      if ( DS_OK == lpDSPrimaryBuffer->QueryInterface( IID_IDirectSound3DListener, (void **)&glpDirectSound3DListener) )
      {
        //if ( DS_OK == glpDirectSound3DListener->SetPosition( 0.f, 0.f, -1.f, DS3D_DEFERRED) )
				if ( DS_OK == glpDirectSound3DListener->SetPosition( 0.f, 0.f, 0.f, DS3D_DEFERRED) )
        {
          if ( DS_OK == glpDirectSound3DListener->SetDopplerFactor( 9.9f ,DS3D_DEFERRED) )
				//if ( DS_OK == glpDirectSound3DListener->SetDopplerFactor( 4.9f ,DS3D_DEFERRED) )
          {
            if ( DS_OK == glpDirectSound3DListener->SetRolloffFactor( 0.25f ,DS3D_DEFERRED) )
            //if ( DS_OK == glpDirectSound3DListener->SetRolloffFactor( 0.99f ,DS3D_DEFERRED) )
            {
              if ( DS_OK == glpDirectSound3DListener->CommitDeferredSettings() )
              {
                return TRUE;
              }
            }
          }
        }
				//if( lpDSPrimaryBuffer->SetFormat(&pcmwf.wf)!=DS_OK ) return FALSE;
				glpDirectSound3DListener->Release();
      }
      lpDSPrimaryBuffer->Release(); lpDSPrimaryBuffer=NULL;
    }
		}
		else	// Direct x 5.0 이하
		{
			memset(&pcmwf, 0, sizeof(PCMWAVEFORMAT));
			pcmwf.wf.wFormatTag = WAVE_FORMAT_PCM;
			pcmwf.wf.nChannels = 2;
			pcmwf.wf.nSamplesPerSec = 22050;
			pcmwf.wf.nBlockAlign = 2;
			pcmwf.wf.nAvgBytesPerSec = pcmwf.wf.nSamplesPerSec * pcmwf.wf.nBlockAlign;
			pcmwf.wBitsPerSample = 8;
			memset(&dsbdesc, 0, sizeof(DSBUFFERDESC));
			dsbdesc.dwSize = sizeof(DSBUFFERDESC);
			dsbdesc.dwFlags = DSBCAPS_PRIMARYBUFFER;
			dsbdesc.dwBufferBytes = 0; //dwBufferBytes and lpwfxFormat must be set this way.
			dsbdesc.lpwfxFormat = NULL;

			if ( DS_OK == gpds->CreateSoundBuffer( &dsbdesc, &lpDSPrimaryBuffer, NULL) )
			{
				//if( lpDSPrimaryBuffer->SetFormat(&pcmwf.wf)!=DS_OK ) return FALSE;
				lpDSPrimaryBuffer->Release(); lpDSPrimaryBuffer=NULL;
			}
			else
			{
				MessageBox( NULL, "CreateSoundBuffer", "DirectSound Error", MB_OK );
			}
		}
    //gpds->Release();

    return(FALSE);
}

void FreeDSound(void)
{
	if ( pNowPlayingList != NULL ) safe_delete( pNowPlayingList );
	if ( mpWaveList  != NULL ) safe_delete( mpWaveList );
  if ( lpDSPrimaryBuffer!=NULL ) lpDSPrimaryBuffer->Release();
  if ( glpDirectSound3DListener!=NULL ) glpDirectSound3DListener->Release();
  if ( gpds!=NULL ) gpds->Release();
};

// ----------------------------------------------------------
// InitalizeWaveDevice - be sure wave method is ready for use.
// ----------------------------------------------------------
BOOL InitializeWaveDevice(
    int     WaveMode,   // sndPlaySound, waveOut, or DirectSound or DirectSound3D
    LPWAVEFORMATEX  pFormat     // default wave format
)
{
  if (pNowPlayingList == NULL)
  {
    pNowPlayingList = new CLinkedList;      
  }

  // Create a DirectSound primary buffer
  return (CreatePrimarySoundBuffer());
}

LPBYTE			g_lpData;
MMCKINFO    g_ckIn;
UINT        g_cbActualRead;

// ----------------------------------------------------------
// LoadWaveData - read .WAV file information into appropriate
//  memory buffer for playback.
// ----------------------------------------------------------
LPWAVEHDR LoadWaveData(
  CSoundEffect    *pSound,    // sound effect instance
	LPSTR		PackName,
  LPSTR   WaveName,   // .WAV filename
  int     WaveMode    // sndPlaySound, waveOut or DirectSound or DirectSound3D
)
{
	LPBYTE      lpData = NULL;
	LPWAVEHDR   lpWaveHdr = NULL;
	DWORD       dwDataSize = 0;
	LPDIRECTSOUNDBUFFER lpDirectSoundBuffer = NULL;
	LPWAVEFORMATEX  pwfxInfo = NULL;

	// Check to be sure a non-null name was given
	if ((WaveName == NULL) || (*WaveName == '\0'))
	{
		return(NULL);
	}

	HMMIO       hmmio;    
	MMCKINFO    ckInRiff;
	MMCKINFO    ckIn;
	UINT        cbActualRead;
	UINT        cbSize;

	pwfxInfo = NULL;
	cbSize = 0;

	// Use routines in CGWAVE to open the sound file and
	// parse the data in it.    
	if (WaveOpenFile(PackName, WaveName, &hmmio, &pwfxInfo, &ckInRiff) != 0)
	{
		if (pwfxInfo != NULL) GlobalFree(pwfxInfo);
		return(NULL);
	}

	// Be sure we have an output device ready to play the sound
	if (!InitializeWaveDevice(WaveMode, pwfxInfo))
	{
		mmioClose(hmmio, 0);
		if (pwfxInfo != NULL) GlobalFree(pwfxInfo);
		return(NULL);
	}

	// Position the wave file for reading the wave data
	if (WaveStartDataRead(&hmmio, &ckIn, &ckInRiff) != 0)
	{
		mmioClose(hmmio, 0);
		if (pwfxInfo != NULL) GlobalFree(pwfxInfo);
		return(NULL);
	}

	// Ok, size of wave data is in ckIn, allocate that buffer.
	dwDataSize = ckIn.cksize;

	DSBUFFERDESC    dsbd;
	DWORD   dwBSize;
	DWORD   dwWrapBSize;
	LPVOID  lpWrapPtr;
	DWORD           dw;     // size place holder
            
	// Set up the secondary direct sound buffer. 
	memset(&dsbd, 0, sizeof(DSBUFFERDESC));
	dsbd.dwSize = sizeof(DSBUFFERDESC);
  
	if( g_nDxVer>=0x500 ) 
		dsbd.dwFlags = DSBCAPS_CTRL3D | DSBCAPS_STATIC | DSBCAPS_CTRLVOLUME; 
	else 
		dsbd.dwFlags = DSBCAPS_STATIC;
  
	dsbd.dwBufferBytes = dwDataSize;
	dw = pwfxInfo->cbSize + sizeof(WAVEFORMATEX);

	if ((dsbd.lpwfxFormat = (LPWAVEFORMATEX)GlobalAlloc( GMEM_FIXED, dw)) == NULL)
	{
		mmioClose(hmmio, 0);
		return NULL;
	}

	// Setup the format, frequency, volume, etc.
	memcpy( dsbd.lpwfxFormat, pwfxInfo, dw );

	if (gpds->CreateSoundBuffer(&dsbd, &lpDirectSoundBuffer, NULL) != 0)
	{
		GlobalFree(dsbd.lpwfxFormat);
		mmioClose(hmmio, 0);
		return NULL;
	}

	GlobalFree(dsbd.lpwfxFormat);

	// Need to lock the buffer so that we can write to it!
	if (lpDirectSoundBuffer->Lock(
										0,
										dwDataSize,
										(LPVOID*)&lpData,
										&dwBSize,
										&lpWrapPtr,
										&dwWrapBSize,
										0L) != 0)
	{
		mmioClose(hmmio, 0);
		return NULL;
	} 
	else 
	{
		dwDataSize = dwBSize;
	}

	// Now read the actual wave data into the data buffer.
	if (WaveReadFile(hmmio, dwDataSize, lpData, &ckIn, &cbActualRead) != 0)
	{
		// Data didn't get read for some reason!
		if (pwfxInfo != NULL) GlobalFree(pwfxInfo);
		if (lpData != NULL) GlobalFree(lpData);
	}
	
	cbSize = cbActualRead;	// Save info on size of data
	dwDataSize = cbSize;
	mmioClose(hmmio, 0);		// close the wave file

	// If we have a DirectSound buffer, set format & unlock it.
	if (lpDirectSoundBuffer != NULL)
	{
		lpDirectSoundBuffer->Unlock(lpData, cbActualRead, NULL, 0 );
	}

	// Allocate and lock memory for the header. This memory must
	// also be globally allocated with GMEM_MOVEABLE and GMEM_SHARE flags.
	if ((lpWaveHdr = (LPWAVEHDR) GlobalAlloc( GMEM_FIXED, (DWORD)sizeof(WAVEHDR))) == NULL)
	{
		GlobalFree(lpData);
		return(NULL);
	}

	// After allocation, set up and prepare header.  This struct will be
	// used to play the sounds at the appropriate time.
	lpWaveHdr->lpData = (LPSTR) lpData;
	lpWaveHdr->dwBufferLength = dwDataSize;
	lpWaveHdr->dwFlags = DSBCAPS_CTRL3D;
	lpWaveHdr->dwLoops = 0L;

	pSound->pWaveInfo->lpDirectSoundBuffer = lpDirectSoundBuffer;
	pSound->pWaveInfo->pwfxInfo = pwfxInfo;

	if( g_nDxVer>=0x500 )
	{
		pSound->lpDirectSound3DBuffer=NULL;			// <------- 반드시 해줘야 아래줄 에러 안난다. -_-;

		if ( S_OK != lpDirectSoundBuffer->QueryInterface( IID_IDirectSound3DBuffer, (void **)&pSound->lpDirectSound3DBuffer))
		{
			return NULL;
		}
	}
	lpDirectSoundBuffer->Release(); // <--- 요거 안해줘서 자꾸 evermem 이 false 인 wave loading시 리소스가 쌓였다.(조심)
	return(lpWaveHdr);
}

// ----------------------------------------------------------
// CSoundEffect constructor
// ----------------------------------------------------------
CSoundEffect::CSoundEffect(
    LPSTR   WaveName,   // name of wave file
    DWORD   Id,     // object id to allow one buffer per object
    BOOL    fLoopIt,    // loop the sound when played
		BOOL		EverMem			// 메모리 상주여부
)
{
	CreateSoundEffect(WaveName,Id,fLoopIt,EverMem);
}
CSoundEffect::CSoundEffect( void )
{
}
// ----------------------------------------------------------
// CSoundEffect destructor
// ----------------------------------------------------------
CSoundEffect::~CSoundEffect()
{
  if( (pWaveInfo != NULL) && (--pWaveInfo->mRefCount == 0) )
  {
		for( int i=0;i<8;i++ ) Stop(i);
    
		if( lpDirectSoundBuffer[0]==NULL ) GlobalFree(pWaveInfo->pWaveHdr->lpData);
		else
		{
			for( i=0;i<8;i++ )
			{
				if( lpDirectSoundBuffer[i] != NULL )
				{
					lpDirectSoundBuffer[i]->Release();
					lpDirectSoundBuffer[i] = NULL;
				}
			}
		}
		mpWaveList->Remove( pWaveInfo );
		safe_delete( pWaveInfo->mpFileName );
		GlobalFree(pWaveInfo->pwfxInfo);
		GlobalFree(pWaveInfo->pWaveHdr);
    safe_delete( pWaveInfo );       // causes GP fault why?
    pWaveInfo = NULL;
    // !!! should we delete the list if empty?
  }
}
BOOL CSoundEffect::CreateSoundEffect(
    LPSTR   WaveName,   // name of wave file
    DWORD   Id,     // object id to allow one buffer per object
    BOOL    fLoopIt,    // loop the sound when played
	BOOL		EverMem			// 메모리 상주여부
)
{
	pWaveInfo = NULL;
	fLoop = fLoopIt;
	fPlaying = FALSE;
	m_bEverMem = EverMem;
	m_bOnePlayed = FALSE;
	WaveMode = USE_DSOUND;

	for( int i = 0; i < 8; i ++ )	lpDirectSoundBuffer[i] = NULL;

    // Guard against wise-guys (we don't mind)
    if ((WaveName == NULL) || (*WaveName == '\0'))
		return FALSE;

	CSoundEffect::WaveMode = WaveMode;
	curVolume = MAXVOL_TB;
	fMuted = FALSE;

    // initialize wave list if necessary
	if (!mpWaveList)
    {
		mpWaveList = new CLinkedList;
    }
	else
	{
		// Need to search wave list to see if we already have this
		// wave data cached.
		WAVE_ENTRY  *pCurBuffer = (WAVE_ENTRY *) mpWaveList->GetFirst();

		while (pCurBuffer && !pWaveInfo)
		{
			// Do the name and mode match?
			if ((lstrcmpi( WaveName, pCurBuffer->mpFileName ) == 0 )
				&& (pCurBuffer->hObjectId == Id)
				&& (pCurBuffer->WaveMode == WaveMode))
			{
				pCurBuffer->mRefCount++;
				pWaveInfo = pCurBuffer;
			}
			else 
			{
				pCurBuffer = (WAVE_ENTRY *) mpWaveList->GetNext();
			}
		}
    }

    // if we didn't find the wave file in our cache, we need to load it

	if (!pWaveInfo)
    {
		LPWAVEHDR   pWaveHdr;
		pWaveInfo = new WAVE_ENTRY;

		// Load up the wave data            
		if (pWaveHdr = LoadWaveData( this, NULL, WaveName, WaveMode ))
		{
			// Add to list of known sounds
			pWaveInfo->mpFileName = new char [lstrlen(WaveName)+1];
			lstrcpy(pWaveInfo->mpFileName, WaveName);
			pWaveInfo->pWaveHdr = pWaveHdr;
			pWaveInfo->WaveMode = WaveMode;
			pWaveInfo->hObjectId = Id;
			pWaveInfo->mRefCount = 1;
			mpWaveList->Append( pWaveInfo );
			lpDirectSoundBuffer[0] = pWaveInfo->lpDirectSoundBuffer;
		} 
		else 
		{
			safe_delete( pWaveInfo );
			pWaveInfo = NULL;
			return FALSE;
		}
		if( EverMem )
			for( i = 0; i < 8; i ++ )	gpds->DuplicateSoundBuffer( lpDirectSoundBuffer[0], &lpDirectSoundBuffer[i] );
	} 
	else  
	{
		lpDirectSoundBuffer[0] = pWaveInfo->lpDirectSoundBuffer;
		if( EverMem )
			for( i = 0; i < 8; i ++ ) gpds->DuplicateSoundBuffer( lpDirectSoundBuffer[0], &lpDirectSoundBuffer[i] );
	}

	return TRUE;
}
// ----------------------------------------------------------
// Play - 
// ----------------------------------------------------------
void CSoundEffect::Play( int ch )
{
    if ((pWaveInfo == NULL) || (gbQuiet))
    {
        return;
    }

    fPlaying = TRUE;

    switch (WaveMode)
    {
    case USE_DSOUND:
		default:

        if (lpDirectSoundBuffer[ch] != NULL)
        {
           // Add sound to active list if it isn't already there
           CSoundEffect *pTemp = (CSoundEffect *)pNowPlayingList->GetFirst();
           while (pTemp != NULL)
           {
             if (pTemp == this)
             {
               break;
             }
             pTemp = (CSoundEffect *)pNowPlayingList->GetNext();
           }

			if (pTemp == NULL)
			{
				pNowPlayingList->Add(this);
			}

			HRESULT		ret;
			if( fLoop )
			{
				lpDirectSoundBuffer[ch]->SetCurrentPosition( 0 );
				ret = lpDirectSoundBuffer[ch]->Play(0, 0, DSBPLAY_LOOPING);
			}
			else
			{
				lpDirectSoundBuffer[ch]->SetCurrentPosition( 0 );
				ret = lpDirectSoundBuffer[ch]->Play(0, 0, 0);
			}
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
			m_bOnePlayed = TRUE;
        }
        break;
    }
}

// ----------------------------------------------------------
// Stop - 
// ----------------------------------------------------------
void CSoundEffect::Stop( int ch )
{
    if ((pWaveInfo == NULL) || (!fPlaying))
    {
        return;
    }

    fPlaying = FALSE;

    switch (WaveMode)
    {
    case USE_DSOUND:
		default :
        if (lpDirectSoundBuffer[ch] != NULL)
        {
            pNowPlayingList->Remove(this);
            lpDirectSoundBuffer[ch]->Stop();
        }
        break;
    }
}

// ----------------------------------------------------------
// SetPan - 
// ----------------------------------------------------------
void CSoundEffect::SetPan( int PanFactor, int ch )
{
//  if ((pWaveInfo == NULL) || (curPan == PanFactor))
    if (pWaveInfo == NULL)
    {
        return;
    }

    curPan = PanFactor;

    switch (WaveMode)
    {
    case USE_DSOUND:
		default :
        if (lpDirectSoundBuffer[ch] != NULL)
        {
            // parameter pan is 0-127; dsound's pan is -10000 to +10000
            long    absPan;
            absPan = (10000 * ((curPan-64)>>2)) / 128;
                        lpDirectSoundBuffer[ch]->SetPan(absPan);
        }
        break;
    }
}


// ----------------------------------------------------------
// SetVolume - 
// ----------------------------------------------------------
void CSoundEffect::SetVolume( int Volume, int ch )
{
    if ((pWaveInfo == NULL) /* || (curVolume == Volume)*/)
    {
        return;
    }

    curVolume = Volume;
    switch (WaveMode)
    {
    case USE_DSOUND:
		default :
        if ( lpDirectSoundBuffer[ch] != NULL )
        {
			HRESULT	ret;
				// parameter volume is 0-127; dsound's volume is -10000 to 0
			if (Volume)
				ret = lpDirectSoundBuffer[ch]->SetVolume(-((127-Volume)<<4));
			else
				ret = lpDirectSoundBuffer[ch]->SetVolume(-10000);
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
        break;
    }
}


// ----------------------------------------------------------
// SetMute - 
// ----------------------------------------------------------
void CSoundEffect::SetMute( BOOL fMute, int rd )
{

    if ((pWaveInfo == NULL) || (fMuted == fMute))
    {
        return;
    }
    
    fMuted = fMute;

    if (fMuted)
    {
        Stop(rd);
    } else {
        Play(rd);
    }
}

//-----------------------------------------------------------------------------
// playing이 끝났는가?
//-----------------------------------------------------------------------------
BOOL CSoundEffect::ChkEndPlay( void )
{
	DWORD	ret = NULL;

	if( lpDSPrimaryBuffer != NULL )
	{
		if( (lpDirectSoundBuffer[0]->GetStatus(&ret))==DS_OK )
		{
		  if( ret != DSBSTATUS_PLAYING && m_bOnePlayed==TRUE ) 
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}

BOOL CSoundEffect::ChkPlaying( int ch )
{
	DWORD	ret = NULL;

	if( lpDSPrimaryBuffer != NULL )
	{
		if( (lpDirectSoundBuffer[ch]->GetStatus(&ret))==DS_OK )
		{
		  if( ret == DSBSTATUS_PLAYING ) return TRUE;
		}
	}
	return FALSE;
}

void SetSilence( BOOL fQuiet, int ch )
{
  gbQuiet = fQuiet;
  if (lpDSPrimaryBuffer != NULL)
  {
    CSoundEffect *lpSound = (CSoundEffect *)pNowPlayingList->GetFirst();
    while (lpSound != NULL)
    {
      if (fQuiet)
      {
        lpSound->lpDirectSoundBuffer[ch]->Stop();
      } 
			else 
			{
        if( lpSound->fLoop )
        {
				  lpSound->lpDirectSoundBuffer[ch]->Play(0, 0, DSBPLAY_LOOPING);
        } 
				else 
				{
          lpSound->lpDirectSoundBuffer[ch]->Play(0, 0, 0);
        }
				lpSound->m_bOnePlayed = TRUE;
      }
      lpSound = (CSoundEffect *)pNowPlayingList->GetNext();
    }
  }
}
