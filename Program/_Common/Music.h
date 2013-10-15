#ifndef __MUSIC_H__
#define __MUSIC_H__

class CMusicMng
{
public:
	struct MUSIC
	{
		TCHAR szMusicFileName[ 128 ];
	};
	CFixedArray< MUSIC > m_aMusic;

	BOOL LoadScript( LPCTSTR lpszFileName );
	LPTSTR GetFileName( DWORD dwId );
};
void ProcessFadeMusic();
BOOL PlayMusic( LPCTSTR lpszFileName, int nLoopCount = 1 );
BOOL PlayMusic( DWORD dwIdMusic, int nLoopCount = 1 );
BOOL PlayBGM( DWORD dwIdMusic );
void LockMusic();
void StopMusic();
void SetVolume( FLOAT fVolume );
FLOAT GetVolume();
BOOL IsPlayingMusic();
BOOL IsStopMusic();
void InitCustomSound( BOOL bEnable = TRUE );
void UnInitCustomSound();

extern CMusicMng g_MusicMng;

#endif
