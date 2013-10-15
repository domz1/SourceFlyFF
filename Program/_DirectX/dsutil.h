//-----------------------------------------------------------------------------
// File: DSUtil.h
//
// Desc: 
//
// Copyright (c) 1999-2001 Microsoft Corp. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef DSUTIL_H
#define DSUTIL_H

#include <windows.h>
#include <mmsystem.h>
#include <mmreg.h>
#include <dsound.h>
#include <control.h>
#include <strmif.h>
#include "..\_Common\data.h"
#include "..\_Common\path.h"


//-----------------------------------------------------------------------------
// Classes used by this header
//-----------------------------------------------------------------------------
class CSoundManager;
class CSound;
class CStreamingSound;
class CWaveFile;




//-----------------------------------------------------------------------------
// Typing macros 
//-----------------------------------------------------------------------------
#define WAVEFILE_READ   1
#define WAVEFILE_WRITE  2

#define DSUtil_StopSound(s)         { if(s) s->Stop(); }
#define DSUtil_PlaySound(s)         { if(s) s->Play( 0, 0 ); }
#define DSUtil_PlaySoundLooping(s)  { if(s) s->Play( 0, DSBPLAY_LOOPING ); }


//-----------------------------------------------------------------------------
// Name: class CSoundManager
// Desc: 
//-----------------------------------------------------------------------------
class CSoundManager
{
protected:
    LPDIRECTSOUND8 m_pDS;
	D3DVECTOR m_vOrientFront; 
	D3DVECTOR m_vOrientTop;
	D3DVECTOR m_vPosListener;
	FLOAT m_fTheta;
	BOOL  m_bSuccess;

public:
	int m_nSoundVolume;
	int m_nOldSoundVolume;
	CSoundManager();
    ~CSoundManager();

    HRESULT Initialize( HWND hWnd, DWORD dwCoopLevel, DWORD dwPrimaryChannels, DWORD dwPrimaryFreq, DWORD dwPrimaryBitRate );
    inline  LPDIRECTSOUND8 GetDirectSound() { return m_pDS; }
    HRESULT SetPrimaryBufferFormat( DWORD dwPrimaryChannels, DWORD dwPrimaryFreq, DWORD dwPrimaryBitRate );
    HRESULT Get3DListenerInterface( LPDIRECTSOUND3DLISTENER* ppDSListener );

    HRESULT Create( CSound** ppSound, LPTSTR strWaveFileName, DWORD dwCreationFlags = 0, GUID guid3DAlgorithm = GUID_NULL, DWORD dwNumBuffers = 1 );
    HRESULT CreateFromMemory( CSound** ppSound, BYTE* pbData, ULONG ulDataSize, LPWAVEFORMATEX pwfx, DWORD dwCreationFlags = 0, GUID guid3DAlgorithm = GUID_NULL, DWORD dwNumBuffers = 1 );
    HRESULT CreateStreaming( CStreamingSound** ppStreamingSound, LPTSTR strWaveFileName, DWORD dwCreationFlags, GUID guid3DAlgorithm, DWORD dwNotifyCount, DWORD dwNotifySize, HANDLE hNotifyEvent );
//////////////////////////////////////////////
// custom
private:
		CMapStringToPtr         m_map;
		LPDIRECTSOUND3DLISTENER m_pDSListener;      // 3D listener object
		DS3DLISTENER            m_dsListenerParams; // Listener properties

public:
//		IGraphBuilder *m_pGraphBuilder  ;
		IMediaControl *m_pMediaControl  ;
		IMediaSeeking *m_pMediaSeeking  ;
		IBaseFilter   *m_pSourceCurrent ;
		IBaseFilter   *m_pSourceNext;
		TCHAR          m_szCurrentFile[128];
		HWND           m_hwndApp;

public:
		BOOL Play( LPTSTR pszFileName, D3DXVECTOR3* pvPosition = NULL, D3DXVECTOR3* pvVelocity = NULL, BOOL bIsAutoplay = TRUE, BOOL bIsVoice = FALSE );
		BOOL Play( LPCTSTR pszFileName, D3DXVECTOR3* pvPosition = NULL, D3DXVECTOR3* pvVelocity = NULL, BOOL bIsAutoplay = TRUE,  BOOL bIsVoice = FALSE ) { return Play((LPTSTR)pszFileName,pvPosition,pvVelocity, bIsAutoplay, bIsVoice ); }
		BOOL Play( int nId, D3DXVECTOR3* pvPosition = NULL, D3DXVECTOR3* pvVelocity = NULL, BOOL bIsAutoplay = TRUE,  BOOL bIsVoice = FALSE ); 
		CSound* GetSound( LPTSTR pszFileName );
		void Stop();
		void SetListener( D3DXVECTOR3 vecPos, FLOAT fAngle );

		HRESULT PlayMusic( LPCTSTR pszFileName );

		// 이하부터 사운드 프로퍼티를 위한 처리들 
		struct MUSIC
		{
			TCHAR szMusicFileName[ 128 ];
		}; 
		CFixedArray< MUSIC > m_aMusic;
		BOOL LoadScript( LPCTSTR lpszFileName );
		LPTSTR GetFileName( int nId );
};




//-----------------------------------------------------------------------------
// Name: class CSound
// Desc: Encapsulates functionality of a DirectSound buffer.
//-----------------------------------------------------------------------------
class CSound
{
protected:
    LPDIRECTSOUNDBUFFER* m_apDSBuffer;
    DWORD                m_dwDSBufferSize;
    CWaveFile*           m_pWaveFile;
    DWORD                m_dwNumBuffers;

    HRESULT RestoreBuffer( LPDIRECTSOUNDBUFFER pDSB, BOOL* pbWasRestored );

public:
		LPDS3DBUFFER           m_pBufferParams;   // 3D buffer properties
		LPDIRECTSOUND3DBUFFER* m_apDS3DBuffer;

    CSound( LPDIRECTSOUNDBUFFER* apDSBuffer, DWORD dwDSBufferSize, DWORD dwNumBuffers, CWaveFile* pWaveFile );
    virtual ~CSound();

    HRESULT Get3DBufferInterface();
    HRESULT Get3DBufferInterface( DWORD dwIndex );
    HRESULT Get3DBufferInterface( DWORD dwIndex, LPDIRECTSOUND3DBUFFER* ppDS3DBuffer );
    HRESULT FillBufferWithSound( LPDIRECTSOUNDBUFFER pDSB, BOOL bRepeatWavIfBufferLarger );

		DWORD GetFreeBufferIndex();
    LPDIRECTSOUNDBUFFER GetFreeBuffer();
    LPDIRECTSOUNDBUFFER GetBuffer( DWORD dwIndex );

    HRESULT Play( DWORD dwPriority = 0, DWORD dwFlags = 0, LPDWORD pIndex = NULL);
    HRESULT Stop();
    HRESULT Reset();
    BOOL    IsSoundPlaying();
};




//-----------------------------------------------------------------------------
// Name: class CStreamingSound
// Desc: Encapsulates functionality to play a wave file with DirectSound.  
//       The Create() method loads a chunk of wave file into the buffer, 
//       and as sound plays more is written to the buffer by calling 
//       HandleWaveStreamNotification() whenever hNotifyEvent is signaled.
//-----------------------------------------------------------------------------
class CStreamingSound : public CSound
{
protected:
    DWORD m_dwLastPlayPos;
    DWORD m_dwPlayProgress;
    DWORD m_dwNotifySize;
    DWORD m_dwNextWriteOffset;
    BOOL  m_bFillNextNotificationWithSilence;

public:
    CStreamingSound( LPDIRECTSOUNDBUFFER pDSBuffer, DWORD dwDSBufferSize, CWaveFile* pWaveFile, DWORD dwNotifySize );
    ~CStreamingSound();

    HRESULT HandleWaveStreamNotification( BOOL bLoopedPlay );
    HRESULT Reset();
};




//-----------------------------------------------------------------------------
// Name: class CWaveFile
// Desc: Encapsulates reading or writing sound data to or from a wave file
//-----------------------------------------------------------------------------
class CWaveFile
{
public:
    WAVEFORMATEX* m_pwfx;        // Pointer to WAVEFORMATEX structure
    HMMIO         m_hmmio;       // MM I/O handle for the WAVE
    MMCKINFO      m_ck;          // Multimedia RIFF chunk
    MMCKINFO      m_ckRiff;      // Use in opening a WAVE file
    DWORD         m_dwSize;      // The size of the wave file
    MMIOINFO      m_mmioinfoOut;
    DWORD         m_dwFlags;
    BOOL          m_bIsReadingFromMemory;
    BYTE*         m_pbData;
    BYTE*         m_pbDataCur;
    ULONG         m_ulDataSize;
    CHAR*         m_pResourceBuffer;

protected:
    HRESULT ReadMMIO();
    HRESULT WriteMMIO( WAVEFORMATEX *pwfxDest );

public:
    CWaveFile();
    ~CWaveFile();

    HRESULT Open( LPTSTR strFileName, WAVEFORMATEX* pwfx, DWORD dwFlags );
    HRESULT OpenFromMemory( BYTE* pbData, ULONG ulDataSize, WAVEFORMATEX* pwfx, DWORD dwFlags );
    HRESULT Close();

    HRESULT Read( BYTE* pBuffer, DWORD dwSizeToRead, DWORD* pdwSizeRead );
    HRESULT Write( UINT nSizeToWrite, BYTE* pbData, UINT* pnSizeWrote );

    DWORD   GetSize();
    HRESULT ResetFile();
    WAVEFORMATEX* GetFormat() { return m_pwfx; };
};

#if	defined(__CLIENT)
extern CSoundManager g_SoundMng;
#include "..\_common\Path.h"
inline BOOL PLAYSND( LPCTSTR pszFileName, D3DXVECTOR3* pvPosition = NULL, BOOL bIsVoice = FALSE ) 
{ 
	return g_SoundMng.Play( MakePath( DIR_SOUND, pszFileName ), pvPosition, NULL, TRUE, bIsVoice ); 
}
inline BOOL PLAYSND( int nId, D3DXVECTOR3* pvPosition = NULL, BOOL bIsVoice = FALSE ) 
{ 
	return g_SoundMng.Play( nId, pvPosition, NULL, TRUE, bIsVoice ); 
}
#else
inline BOOL PLAYSND( LPCTSTR pszFileName, D3DXVECTOR3* pvPosition = NULL )	{	return TRUE;	}
inline BOOL PLAYSND( int nId, D3DXVECTOR3* pvPosition = NULL )	{	return TRUE;	}
#endif

#endif // DSUTIL_H
