//#define _SOUND_CPP

/*
|	 CSound Class library
|	 Version Upgrade 3DSound & MultyChannel stream by MUGEN	1999
|  Copyright (C) 1999 MUGEN.
*/

#include "stdafx.h"
#include "sound.h"
#include "sndtime.h"
#include "linklist.h"

 extern LPDIRECTSOUND									gpds;				// from cgsound.cpp
 BOOL		CreatePrimarySoundBuffer( void );					// from cgsound.cpp
 void		FreeDSound( void );												// from cgsound.cpp
 extern LPDIRECTSOUND3DLISTENER       glpDirectSound3DListener;
 extern unsigned long g_nDxVer;

BOOL		g_bBGMPlaying;
CTimer	g_SndTimer;
CWaveStream	g_WaveStream;
CLinkedList  *g_SefList = NULL;

CWaveStream::CWaveStream( void )
{
	// return; // fuck
  SefMain = 127;
  SefLocal = 127;
  BGMMain = 127;
  BGMLocal = 127;
	m_pasCurrent = NULL;
	m_pass = NULL;
	m_nListenerClientX = 1024/2; // 화면 중앙
	m_nListenerClientY = 768/2; // 화면 중앙
	m_bBGMCreateFailure = FALSE;
	m_nMusicId = -1;
/*
  InitSef();
	InitBGM();
*/
}

 CWaveStream::~CWaveStream( void )
{
/*
	StopBGM();
  CloseBGM();
  OffSef();
  FreeDSound();
*/
}

 void CWaveStream::Initialize( void )
{
	 //return; // fuck
  InitSef();
	InitBGM();
}

 void CWaveStream::Free( void )
{
	 //return; // fuck
	StopBGM();
  CloseBGM();
  //OffSef();
	//FreeDSound();
}

 void    CWaveStream::SetSefVolumeMain(long level)
{
	 //return; // fuck
	int j;

  if (level<0) level = 0;
  if (level>127) level = 127;
  SefMain = level;

	CSoundEffect	*sef;
	void	*node = NULL;

	node = g_SefList->GetFirst();
	while( node )
	{
		sef = (CSoundEffect *)node;
		for( j=0;j<8;j++ ) sef->SetVolume( SefMain * SefLocal /127, j );
		node = g_SefList->GetNext();
	}
}

void     CWaveStream::SetSefVolumeLocal(long level)
{
	//return; // fuck
	int j;

  if (level<0) level = 0;
  if (level>127) level = 127;
  SefLocal = level;

	CSoundEffect	*sef;
	void	*node = NULL;

	node = g_SefList->GetFirst();
	while( node )
	{
		sef = (CSoundEffect *)node;
		for( j=0;j<8;j++ ) sef->SetVolume( SefMain * SefLocal /127, j );
		node = g_SefList->GetNext();
	}
}

void     CWaveStream::InitSef(void)
{
	//return; // fuck
	g_SefList = new CLinkedList;
	if( !CreatePrimarySoundBuffer() ) return;
}

void     CWaveStream::OffSef(void)
{
	//return; // fuck
	CSoundEffect	*sef;
	void	*node = NULL;

	node = g_SefList->GetFirst();
	while( node )
	{
		sef = (CSoundEffect *)node;
		node = g_SefList->GetNext();
		safe_delete( sef );
	}
	safe_delete( g_SefList );
}

long	 CWaveStream::LoadFSef( LPCTSTR fn, BOOL repeat, BOOL EverMem )
{
	//return 1; // fuck
	int i, j;
	CSoundEffect	*sef;
	void	*node = NULL;

	node = g_SefList->GetFirst();
  while( node )
	{
  	sef = (CSoundEffect *)node;
	  if( !strcmp( sef->pWaveInfo->mpFileName, fn ) ) return 1;
	  node = g_SefList->GetNext();
	} 

	sef = new CSoundEffect; i = 0;
	if(sef->CreateSoundEffect( (LPSTR)fn, i+100, repeat, EverMem ))
	{
		for( j = 0; j < 8; j ++ )	
			sef->SetVolume( SefMain * SefLocal /127, j );
		g_SefList->Append(sef);
		return 1;
	}
	else
	{
		safe_delete( sef );
		sef = NULL;
		return -1;
	}

	return -1;
}

void	 CWaveStream::PlayFSef( LPCTSTR fn )
{
//	return; // fuck
	int j = 0;
	CSoundEffect	*sef;
	void	*node = NULL;

	node = g_SefList->GetFirst();	
  while( node )
	{
	  sef = (CSoundEffect *)node;
	  if( !strcmp( sef->pWaveInfo->mpFileName, fn ) ) break;
	  node = g_SefList->GetNext();
	}
  if( !node ) return;

	if( sef->m_bEverMem )
	{
		for( j=0;j<4;j++ ) // 수련장같은데서 사운드 다수 발생에 의한 속도저하를 막기위해 최대 4채널로 한다.
		{
			if( sef->ChkPlaying(j)==FALSE ) break;
		}
		if( j==4 ) return;
	}
	else
	{
		if( sef->ChkPlaying(0)==TRUE ) return; // 임시 메모리를 차지하는 wav는 1채널만 지원한다(랙방지)
		j=0;
	}

	sef->Stop(j);
	sef->SetVolume( SefMain * SefLocal /127, j );
	sef->Play(j);
}

 void  CWaveStream::PlayFSef3D( LPCTSTR fn, int x, int y )
{
	 //return; // fuck
	int j;  
	CSoundEffect	*sef;
	void	*node = NULL; 

	node = g_SefList->GetFirst();
  while( node )
	{
	  sef = (CSoundEffect *)node;
	  if( !strcmp( sef->pWaveInfo->mpFileName, fn ) ) break;
	  node = g_SefList->GetNext();
	}
  if( !node ) return;

	if( sef->m_bEverMem )
	{
		for( j=0;j<4;j++ ) // 수련장같은데서 사운드 다수 발생에 의한 속도저하를 막기위해 최대 4채널로 한다.
		{
			if( sef->ChkPlaying(j)==FALSE ) break;
		}
		if( j==4 ) return;
	}
	else
	{
		if( sef->ChkPlaying(0)==TRUE ) return; // 임시 메모리를 차지하는 wav는 1채널만 지원한다(랙방지)
		j=0;
	}

	sef->Stop(j);
	sef->lpDirectSound3DBuffer->SetPosition( P2M(x-m_nListenerClientX), 0.0, P2M(m_nListenerClientY-y), DS3D_DEFERRED);
  glpDirectSound3DListener->CommitDeferredSettings();

  sef->SetVolume( SefMain * SefLocal /127, j );
  sef->Play(j);
}

void	 CWaveStream::StopFSef( LPCTSTR fn )
{
	//return; // fuck
	CSoundEffect	*sef;
	void	*node = NULL;

	node = g_SefList->GetFirst();
  while( node )
	{
	  sef = (CSoundEffect *)node;
	  if( !strcmp( sef->pWaveInfo->mpFileName, fn ) ) break;
	  node = g_SefList->GetNext();
	}
  if( !node ) return;

	for( int i=0;i<8;i++ ) sef->Stop(i);
}

void	 CWaveStream::OffOneFSef( LPCTSTR fn )
{
//	return; // fuck
	CSoundEffect	*sef;
	void	*node = NULL;

	node = g_SefList->GetFirst();
  while( node )
	{
	  sef = (CSoundEffect *)node;
	  if( !strcmp( sef->pWaveInfo->mpFileName, fn ) ) break;
	  node = g_SefList->GetNext();
	}
  if( !node ) return;

	g_SefList->Remove(sef);
	safe_delete( sef );
}

 void CWaveStream::Process( void )
{	
//	 return; // fuck
	CSoundEffect	*sef;
	void	*node = NULL;

	node = g_SefList->GetFirst();
	while( node )
	{
		sef = (CSoundEffect *)node;
		if( !sef->m_bEverMem && sef->ChkEndPlay()==TRUE )
		{
		  g_SefList->Remove(sef);
			safe_delete( sef );
			node = g_SefList->GetCurrent();
		}
		else
			node = g_SefList->GetNext();
	}
}

///////////////////////////////////////////////////////////////////////////////

 void    CWaveStream::InitBGM( void )
{
//	 return; // fuck
  // Create and initialize AudioStreamServices object.
  // This must be done once and only once for each window that uses
  // streaming services.
  m_pass = new AudioStreamServices;		
  
	// Initialize ptr to current AudioStream object
  m_pasCurrent = NULL;

  // Delete current AudioStream object
  if (m_pasCurrent)
  {
    safe_delete( m_pasCurrent );
  }
  // Initialize ptr to current AudioStream object
  m_pasCurrent = NULL;

  // Delete current AudioStream object
  if (m_pasCurrent)
  {
    safe_delete( m_pasCurrent );
  }
	// Create new AudioStream object
  m_pasCurrent = new AudioStream;
}

BYTE	g_bOffVolume = 0;

BOOL  CWaveStream::PlayBGM( LPCTSTR fn, BOOL Repeat )
{	
	// return; // fuck
  // Destroy current AudioStream object
 	if( m_pasCurrent && m_strMusicName == fn && (m_pasCurrent->IsPlaying() || m_bBGMCreateFailure == TRUE)) 
		return FALSE;
	if( g_bBGMPlaying) 
		StopBGM();


	g_SndTimer(100);

	if (m_pasCurrent)
	{
		m_strMusicName == fn;
		m_pasCurrent->Destroy();
		m_bBGMCreateFailure = !m_pasCurrent->Create ((CHAR*) fn, m_pass, Repeat );
		if(m_bBGMCreateFailure == FALSE)
		{
			m_pasCurrent->Play ();
			m_pasCurrent->SetVolume( m_pasCurrent->m_nVolume );
			g_bBGMPlaying = TRUE;
			m_strMusicName = fn;
			if( m_pasCurrent->m_nVolume == 0 )
				g_bOffVolume = 4;
			return TRUE;
		}
	}
	return FALSE;
}

void	CWaveStream :: SetVolumeBGM( int nVolume )
{
//	 return; // fuck
	if (m_pasCurrent)
	{
		m_pasCurrent->SetVolume( nVolume );
	}
}

 void  CWaveStream::StopBGM( void )
{
//	 return; // fuck
	g_bBGMPlaying = FALSE;
	m_strMusicName = "";
	m_nMusicId = -1;
	m_bBGMCreateFailure = FALSE;
  if (m_pasCurrent)
  {
    m_pasCurrent->Stop ();
  }
}

 void CWaveStream::CloseBGM( void )
{
//	 return; // fuck
  // Destroy current AudioStream object
  if (m_pasCurrent)
  {
    m_pasCurrent->Destroy();
    safe_delete( m_pasCurrent );
  }

  // Destroy AudioStreamServices object
  if (m_pass)
  {
    safe_delete( m_pass );
  }
}

// Code by Lestat /////////////////////////////////////////////////////////////////////////

BOOL CWaveStream::PlayMusic(int nId,BOOL bRepeat)
{
	/*
	//return 1;
	if(nId <= 0) return FALSE;
	if(prj.GetMusicFile(nId).IsEmpty()) return FALSE;
	CString strFullPath = prj.MakeFullPath(prj.m_strDirMusic,prj.GetMusicFile(nId));
	if(::GetFileAttributes(strFullPath) == 0xFFFFFFFF)
	{
		// 음악 파일이 존재하지 않으면 프로퍼티에서 아예 삭제한다.
		prj.ClearMusicFile(nId);
		return FALSE;
	}
	m_nMusicId = nId;
	PlayBGM(strFullPath,bRepeat);
	*/
	return TRUE;
}
BOOL CWaveStream::PlayMusic(LPCTSTR lpszFileName,BOOL bRepeat)
{
	if(strlen(lpszFileName) == 0) return FALSE;
	//CString strFullPath = prj.MakeFullPath(prj.m_strDirMusic,lpszFileName);
	PlayBGM(lpszFileName,bRepeat);
	return TRUE;
}
BOOL CWaveStream::PlayEffect(int nId)
{
	/*
	if(nId < 0) return FALSE;
	SoundProp* pProp = prj.GetSoundProp(nId);
	if(pProp->name.IsEmpty()) return FALSE;
	CString strFullPath = prj.MakeFullPath(prj.m_strDirSound,pProp->name);
	if(LoadFSef(strFullPath,FALSE,pProp->bLoad) != -1)
	{
		PlayFSef(strFullPath);
		return TRUE;
	}
	*/
	return FALSE;
}
BOOL CWaveStream::PlayEffect3D(int nId,CPoint pt)
{
	/*
	if(nId < 0) return FALSE;
  if( pt.x<-100 || pt.x>CLIENT_WIDTH+100 || pt.y<-100 || pt.y>CLIENT_HEIGHT+100 ) return TRUE;

	SoundProp* pProp = prj.GetSoundProp(nId);
	if(pProp->name.IsEmpty()) return FALSE;
	CString strFullPath = prj.MakeFullPath(prj.m_strDirSound,pProp->name);
	if(LoadFSef(strFullPath,FALSE,pProp->bLoad) != -1)
	{
		if( g_nDxVer>=0x500 ) PlayFSef3D(strFullPath,pt.x,pt.y); else PlayFSef(strFullPath);
		return TRUE;
	}
	*/
	return FALSE;
}
BOOL CWaveStream::PlayEffect(LPCTSTR lpszFileName,BOOL bEverMem)
{
	/*
	if(strlen(lpszFileName) == 0) return FALSE;
	CString strFullPath = prj.MakeFullPath(prj.m_strDirSound,lpszFileName);
	if(LoadFSef(strFullPath,FALSE,bEverMem) != -1)
	{
		PlayFSef(strFullPath);
		return TRUE;
	}
	*/
	return FALSE;
}
BOOL CWaveStream::PlayEffect3D(LPCTSTR lpszFileName,CPoint pt,BOOL bEverMem)
{
	/*
	if(strlen(lpszFileName) == 0) return FALSE;
	CString strFullPath = prj.MakeFullPath(prj.m_strDirSound,lpszFileName);
	if(LoadFSef(strFullPath,FALSE,bEverMem) != -1)
	{
		if( g_nDxVer>=0x500 ) PlayFSef3D(strFullPath,pt.x,pt.y); else PlayFSef(strFullPath);
		return TRUE;
	}
	*/
	return FALSE;
}
/*
BOOL CWaveStream::StopEffect( LPCTSTR lpszFileName, BOOL bEverMem )
{
	//return 1;
	CString strFullPath = prj.MakeFullPath(prj.m_strDirSound,lpszFileName);
	if(LoadFSef(strFullPath,FALSE,bEverMem) != -1)
	{
		StopFSef( strFullPath )
		if( g_nDxVer>=0x500 ) PlayFSef3D(strFullPath,pt.x,pt.y); else PlayFSef(strFullPath);
		return TRUE;
	}
	return FALSE;
}
*/
void CWaveStream::SetListenerPoint(CPoint point)
{
	m_nListenerClientX = point.x, m_nListenerClientY = point.y;
}