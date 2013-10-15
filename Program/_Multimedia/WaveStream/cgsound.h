/*===========================================================================*\
|
|  File:        cgsound.h
|
|  Description: 
|       
|-----------------------------------------------------------------------------
|
|  Copyright (C) 1995-1996 Microsoft Corporation.  All Rights Reserved.
|
|  Written by Moss Bay Engineering, Inc. under contract to Microsoft Corporation
|
|	 <<<<<<<< Add 3DSound lib by MUGEN	1999.3 >>>>>>>>>
\*===========================================================================*/
/**************************************************************************

    (C) Copyright 1995-1996 Microsoft Corp.  All rights reserved.

    You have a royalty-free right to use, modify, reproduce and 
    distribute the Sample Files (and/or any modified version) in 
    any way you find useful, provided that you agree that 
    Microsoft has no warranty obligations or liability for any 
    Sample Application Files which are modified. 

    we do not recomend you base your game on IKlowns, start with one of
    the other simpler sample apps in the GDK

 **************************************************************************/

#ifndef _CGSOUND_H
#define _CGSOUND_H

//extern "C" {
    #include <dsound.h>
#include <d3dtypes.h>
#include <mmsystem.h>
//};

#define USE_DSOUND		0

/**************** CONVERSION MACROS **************************
These would be faster if hard-coded, but are left like this
for readability.
**************************************************************/
#define SCREEN_METERS D3DVAL(6)         //(How many meters wide is the screen?)
#define PPM    D3DVAL(640/SCREEN_METERS)  //How many pixels per meter?
#define MPP    D3DVAL(SCREEN_METERS/640.f)  //How many meters per pixel?
#define P2M(x) D3DVAL(D3DVAL(x) * MPP)   //converts pixels to meters
#define M2P(x) D3DVAL(D3DVAL(x) * PPM)   //converts meters to pixels

// Master wave information structure -- one for each unique
// sound (not each instance).
typedef struct _WAVE_ENTRY
{
    LPSTR           mpFileName;
    DSBUFFERDESC        *dsbd;
    LPDIRECTSOUNDBUFFER lpDirectSoundBuffer;
	  LPDIRECTSOUND3DBUFFER	lpDirectSound3DBuffer;
    LPWAVEFORMATEX      pwfxInfo;
    LPWAVEHDR       pWaveHdr;
    DWORD           hObjectId;
    int         WaveMode;
    int         mRefCount;
} WAVE_ENTRY, *LPWAVE_ENTRY;


class CSoundEffect
{
public:
    LPWAVE_ENTRY    pWaveInfo;  // ptr to master wav info
    DSBUFFERDESC    *dsbd;      // instance DS buffer
    LPDIRECTSOUNDBUFFER     lpDirectSoundBuffer[8];
    LPDIRECTSOUND3DBUFFER   lpDirectSound3DBuffer;
    int     WaveMode;   // method for playing
    int     curVolume;  // 0-127
    int     curPan;     // 0=left, 127=right
    BOOL    fLoop;
    BOOL    fPlaying;
    BOOL    fMuted;
    int     curChannel;
		BOOL		m_bEverMem;
		BOOL		m_bOnePlayed;

		CSoundEffect( void );
    CSoundEffect(LPSTR WaveFile, DWORD UniqueId=0, BOOL fLoopIt=FALSE, BOOL EverMem=TRUE); 
    ~CSoundEffect();

		BOOL CreateSoundEffect(LPSTR WaveFile, DWORD UniqueId=0, BOOL fLoopIt=FALSE, BOOL EverMem=TRUE);
    void Play( int ch );
    void Stop( int ch );
    void SetPan( int PanFactor, int ch );
    void SetVolume( int Volume, int ch );
    void SetMute( BOOL, int );
    void SetFreq();
		BOOL ChkEndPlay( void );
		BOOL ChkPlaying( int ch );

    friend LPWAVEHDR LoadWaveData(CSoundEffect *, LPSTR, int);
    friend void SetSilence( BOOL, int );
};

#endif
