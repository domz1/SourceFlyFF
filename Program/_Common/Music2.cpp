
// author : gmpbigsun
// date : 2009_11_02
// music player for ogg and mp3 ( use miles lib )

#include "StdAfx.h"

#if __VER >= 15 // __MUSIC2

#include "Music.h"
#include "mss.h"

CMusicMng g_MusicMng;

static BOOL g_bEnableMusic = TRUE;
//static BOOL g_bLockMusic = FALSE;
static BOOL g_bFadeOutMusic = FALSE;
static TCHAR g_filename[128];
HSTREAM  g_Stream = NULL;
HDIGDRIVER  g_Dig = NULL;
TCHAR g_szFileName[64];

static float g_fMusicVolume = 1.0f;
static float g_fCurrVolume = 1.0f;


BOOL PlayMusic_( LPCTSTR lpszFileName, int nLoopCount )
{
	if( !g_Dig )
		return FALSE;

	if( g_bEnableMusic == FALSE )		// 사운드 카드가 없거나 껐을때
		return FALSE;

	if( _tcscmp( lpszFileName, g_filename ) == 0 && IsPlayingMusic() )
		return FALSE;

	// 아직 음악이 연주중이라면, 볼륨을 서서히 낮춰줄 세팅을 한다.
// 	if( IsPlayingMusic() )
// 	{
// 		_tcscpy( g_szFileName, lpszFileName );
// 		g_bFadeOutMusic = TRUE;
// 		g_fCurrVolume = g_fMusicVolume;
// 
// 		return TRUE;
// 	}

	// 제거 !
	if( g_Stream )
		AIL_close_stream( g_Stream );

	strcpy( g_filename, lpszFileName );
	
	// 새로운 스트림으로!
	g_Stream = AIL_open_stream( g_Dig, g_filename, 0 );

	if( !g_Stream )
	{
		Error( "oh my god... AIL_open_stream error" );
		return FALSE;
	}
	
	if( nLoopCount < 0 )
		nLoopCount = 0;

	AIL_set_stream_loop_count( g_Stream, nLoopCount );
	AIL_start_stream( g_Stream );
	AIL_service_stream( g_Stream, 0 );
	
	// 볼륨은 원래대로
	SetVolume( g_fMusicVolume );

	return TRUE;
}


void ProcessFadeMusic()
{
	/*if( g_bFadeOutMusic )
	{
		g_fCurrVolume -= 0.01f;
		if( g_Dig )
			AIL_set_digital_master_volume_level( g_Dig, g_fCurrVolume );
		if( g_fMusicVolume < 0.0f )
		{
		    StopMusic();
			g_bFadeOutMusic = FALSE;
			g_bLockMusic = FALSE;
			if( PlayMusic_( g_szFileName, 0 ) )
				g_bLockMusic = TRUE;

		}
	}*/
}

BOOL PlayMusic( LPCTSTR lpszFileName, int nLoopCount )
{
	return PlayMusic_( lpszFileName, nLoopCount );
}

BOOL PlayMusic( DWORD dwIdMusic, int nLoopCount )
{
	if( dwIdMusic == NULL_ID || dwIdMusic == 0 ) //BGM_NONE )
		return FALSE;
	LPTSTR lpFileName = g_MusicMng.GetFileName( dwIdMusic );
	if( lpFileName[ 0 ] )
	{	
		if( PlayMusic( lpFileName, nLoopCount ) )
			return TRUE;
	}
	return FALSE;
}

BOOL PlayBGM( DWORD dwIdMusic )
{
	if( dwIdMusic == NULL_ID || dwIdMusic == 0 ) //BGM_NONE )
		return FALSE;
	LPTSTR lpFileName = g_MusicMng.GetFileName( dwIdMusic );
	if( lpFileName[ 0 ] )
		return PlayMusic( lpFileName, 1 );
	
	return FALSE;
}

void LockMusic()
{
	//g_bLockMusic = TRUE;
}

void StopMusic()
{
	if( !g_Stream )
		return;

	AIL_pause_stream( g_Stream, 1 );		//1 pause, 0 palyback
}

void SetVolume( FLOAT fVolume )
{
	g_fMusicVolume = fVolume;	

	AIL_set_digital_master_volume_level( g_Dig, g_fMusicVolume );
}

FLOAT GetVolume()
{
	return g_fMusicVolume;
}

BOOL IsPlayingMusic()
{
///	HSAMPLE hs = AIL_stream_sample_handle( g_Stream );
	if( !g_Stream )
		return FALSE;

	U32 status = AIL_stream_status( g_Stream );
	return ( SMP_PLAYING == status );
}

BOOL IsStopMusic()
{
//	HSAMPLE hs = AIL_stream_sample_handle( g_Stream );
	if( !g_Stream )
		return TRUE;

	U32 status = AIL_stream_status( g_Stream );
	return ( SMP_PLAYING != status );

}

void InitCustomSound( BOOL bEnable )
{
	AIL_startup();

	g_bEnableMusic = bEnable;

	g_Dig = AIL_open_digital_driver( 44100, 16, MSS_MC_USE_SYSTEM_CONFIG, 0 );
	if( !g_Dig )
	{
		MessageBox(0,AIL_last_error(),"Music initialize error",MB_OK);		
		Error( "AIL_open_digital_driver error, Music init error" );
	}

}

void UnInitCustomSound()
{
	AIL_close_stream( g_Stream );
	AIL_close_digital_driver( g_Dig );
	AIL_shutdown();
}



BOOL CMusicMng::LoadScript( LPCTSTR lpszFileName )
{
	CScript script;
	if( script.Load( lpszFileName ) == FALSE )
		return FALSE;
	CString string;
	MUSIC music;
	int id = script.GetNumber();
	do
	{
		script.GetToken(); // fileName
		_tcscpy( music.szMusicFileName, script.token );
		m_aMusic.SetAtGrow( id, &music );
		id = script.GetNumber();
	} while( script.tok != FINISHED );
	m_aMusic.Optimize();

	return TRUE;
}

LPTSTR CMusicMng::GetFileName( DWORD dwId )
{
	MUSIC* pMusic = m_aMusic.GetAt( dwId );
	if( pMusic )
		return (LPTSTR)pMusic->szMusicFileName;
	return "";
}


#endif