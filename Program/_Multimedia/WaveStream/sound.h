#ifndef     _SOUNDWAVE_H_
#define     _SOUNDWAVE_H_

//
//  Sound.cpp
//
#ifndef _GAMESERVER

#include "cgsound.h"
#include "AudioStream.h"

#define MAXSEFSLOT 32

#define		ASND_ALL			0
#define		ASND_SOUND_ONLY		1
#define		ASND_MUSIC_ONLY		2
#define		ASND_OFF			3

class CWaveStream
{
	BOOL m_bBGMCreateFailure;
	CString m_strMusicName;
	int m_nMusicId;
 public :
	 CWaveStream( void );
	 ~CWaveStream( void );
	void Initialize();
	void Free();

void SetSefVolumeMain(long level);
void SetSefVolumeLocal(long level);

//  Sef stuff...
void InitSef(void);
void OffSef(void);

long	LoadFSef( LPCTSTR fn, BOOL  repeat = FALSE, BOOL EverMen = TRUE );
void	PlayFSef( LPCTSTR );
void	PlayFSef3D( LPCTSTR, int, int );
void	StopFSef( LPCTSTR );
void	OffOneFSef( LPCTSTR );
void  SetListenerPoint(CPoint pt);

int GetMusicId() { return m_nMusicId; }

// Code by Lestat //////////////////////////////////////////////////////////////////////

BOOL PlayMusic   (int nId,BOOL bRepeat = TRUE);
BOOL PlayMusic   (LPCTSTR lpszFileName,BOOL bRepeat = TRUE);
BOOL PlayEffect  (int nId);
BOOL PlayEffect3D(int nId,CPoint pt);
BOOL PlayEffect  (LPCTSTR lpszFileName,BOOL bEverMem = FALSE);
BOOL PlayEffect3D(LPCTSTR lpszFileName,CPoint pt,BOOL bEverMem = FALSE);
BOOL IsPlayingMusic() { return m_pasCurrent ? m_pasCurrent->IsPlaying() : FALSE; }

////////////////////////////////////////////////////////////////////////////////////////

	void	InitBGM( void );
	BOOL	PlayBGM(LPCTSTR , BOOL );
	void	StopBGM( void );
	void	SetVolumeBGM( int nVolume );
	void	CloseBGM( void );
	void	Process( void );

 private :

long            SefMain;
long            SefLocal;
long            BGMMain;
long            BGMLocal;
int							m_nListenerClientX;
int							m_nListenerClientY;

 AudioStreamServices * m_pass;   // ptr to AudioStreamServices object
 AudioStream *m_pasCurrent;      // ptr to current AudioStream object
};

extern CWaveStream g_WaveStream;

#endif
inline BOOL PlayMusic(int nId,BOOL bRepeat = TRUE)
{
#ifdef _CLIENT
	return g_sound.PlayMusic(nId,bRepeat);
#endif
	return TRUE;
}
inline BOOL PlayMusic(LPCTSTR lpszFileName,BOOL bRepeat = TRUE)
{
#ifdef _CLIENT
	return g_sound.PlayMusic(lpszFileName,bRepeat);
#endif
	return TRUE;
}
inline BOOL PlayEffect(int nId)
{
#ifdef _CLIENT
	return g_sound.PlayEffect(nId);
#endif
	return TRUE;
}
inline BOOL PlayEffect3D(int nId,CPoint pt)
{
#ifdef _CLIENT
	return g_sound.PlayEffect3D(nId,pt);
#endif
	return TRUE;
}
inline BOOL PlayEffect(LPCTSTR lpszFileName,BOOL bEverMem = FALSE)
{
#ifdef _CLIENT
	return g_sound.PlayEffect(lpszFileName,bEverMem);
#endif
	return TRUE;
}
inline BOOL PlayEffect3D(LPCTSTR lpszFileName,CPoint pt,BOOL bEverMem = FALSE)
{
#ifdef _CLIENT
	return g_sound.PlayEffect3D(lpszFileName,pt,bEverMem);
#endif
	return TRUE;
}
inline void StopMusic()
{
#ifdef _CLIENT
	g_sound.StopBGM();
#endif
}
inline void SetVolumeMusic( int nVolume )
{
#ifdef _CLIENT
	g_sound.SetVolumeBGM( nVolume );
#endif
}
#endif





