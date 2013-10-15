#include "stdafx.h"
#include <mmsystem.h>
//#include <fstream.h>
#include <fstream>
using namespace std;
#include "usrmsg.h"

#include "music.h"

// Thread functions
DWORD maplay(MPEG_Args *);
DWORD mciplay(MCI_Args *);
DWORD maplay_Exit (DWORD returncode);

CMusic::CMusic()
{
	m_nMusicVolume = 1;
}
CMusic::~CMusic()
{
	Uninit();
}


void CMusic::Init(HWND hWnd)
{
	m_hWnd = hWnd;

	//m_bMPEG = FALSE;
	m_bRepeat = TRUE;
	m_bCMDLINE = FALSE;
	m_bpaused = FALSE;
	m_bcan_play = FALSE;
	m_bcan_pause = FALSE;
	m_bcan_stop = FALSE;
	m_bplaying = FALSE;

	/* thread */
	m_hMPEG_Thread=NULL;
	m_hMCI_Thread = NULL;

	/* argument */
	m_pmaplay_args = &m_maplay_args;
	m_pmci_args = &m_mci_args;

	/* wave out */
	phwo=&hwo;

	/* scroll */
	//m_scroll_range = 0;

	/* temp path */
	//GetWindowsDirectory(m_table_path, 256);
  //	lstrcat(m_table_path, "\\huffdec");
     
	/* enum e_channels m_mode */
	m_mode=both;

	/* icon */
//	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}


// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.



void CMusic::ClearSound()
{
//	m_bscrolling = FALSE;

	if (m_bMPEG) 
	{
		TRACE("ClearSound 2\n");
		if (m_hMPEG_Thread) 
		{
			TRACE("ClearSound 3\n");
			if (!m_pmaplay_args->done_query()) 
			{
				TRACE("ClearSound 4\n");
				m_pmaplay_args->set_stop(TRUE);
				TRACE("ClearSound 5 = %p\n",m_hMPEG_Thread);
		        if (WaitForSingleObject(m_hMPEG_Thread, 500) == WAIT_TIMEOUT) 
				{
					TRACE("ClearSound 6\n");
		         	TerminateThread(m_hMPEG_Thread, 0L); 
		        }
			}
			TRACE("ClearSound 7\n");
		    CloseHandle(m_hMPEG_Thread);
		}
		TRACE("ClearSound 8\n");
		m_hMPEG_Thread=NULL; // ensures next call to clear_sound() will not
		  					   // attempt to kill thread
		waveOutReset(*phwo);
		waveOutClose(*phwo);
		TRACE("ClearSound 9\n");
		//maplay_Exit(0L);
   	
		safe_delete( m_pmaplay_args->stream );
		m_pmaplay_args->stream = NULL;

		safe_delete( m_pmaplay_args->MPEGheader );
		m_pmaplay_args->MPEGheader = NULL;
	} 
	else 
	{
   		if (m_hMCI_Thread) 
		{
			TRACE("ClearSound 10\n");
			if (!m_pmci_args->done_query()) 
			{
				TRACE("ClearSound 11\n");
		        m_pmci_args->set_stop(TRUE);
				TRACE("ClearSound 12\n");
			    if(WaitForSingleObject(m_hMCI_Thread, 500)==WAIT_TIMEOUT)
				{
					TRACE("ClearSound 13\n");
				 	TerminateThread(m_hMCI_Thread, 0L);
				}
   			}
			TRACE("ClearSound 14\n");
			CloseHandle(m_hMCI_Thread);
		}
		m_hMCI_Thread=NULL; // ensures next call to clear_sound() will not
      					  // attempt to kill thread
		TRACE("ClearSound 15\n");
		::mciSendString ((LPSTR)"stop sounder", NULL, 0, NULL);
		TRACE("ClearSound 16\n");
		::mciSendString ((LPSTR)"close sounder", NULL, 0, NULL);
	}
//:SendMessage(m_status_bar, SB_SETTEXT, 2, (LPARAM) "000:00");
	return;
}



BOOL CMusic::File_Init()
{
	return (WaveP(m_szName) ? reinit_MCI() : reinit_MPEG());
}


void CMusic::ok_play()
{
	m_bcan_play=TRUE;
	m_bcan_pause=FALSE;
	m_bplaying=FALSE;
	return;
}

void CMusic::no_stop()
{
	m_bcan_stop=FALSE;
	return;
}

BOOL CMusic::WaveP(LPSTR t) // Check if the file is a format the Windows multimedia system supports.
{
	return ((lstrcmp(t + lstrlen(t)-3, "WAV")==0) ||
			  (lstrcmp(t + lstrlen(t)-3, "wav")==0) ||
			  (lstrcmp(t + lstrlen(t)-3, "MID")==0) ||
			  (lstrcmp(t + lstrlen(t)-3, "mid")==0));
}

BOOL CMusic::reinit_MCI()
{
	char Buffer[1024];    	 // MCI response string

	m_bMPEG=FALSE;

	lstrcpy(m_command, "open ");
	lstrcat(m_command, m_szName);
	lstrcat(m_command, " alias sounder wait");
	mciSendString ((LPSTR)m_command, Buffer, 1024, NULL);
	mciSendString("set sounder time format ms wait", Buffer, 1024, NULL);
	mciSendString("status sounder length wait", Buffer, 1024, NULL);

//	m_scroll_range = my_atoi(Buffer);
//	m_line_size = m_scroll_range >> 3;

	m_args = m_pmci_args;
	m_pmci_args->playing = FALSE;
	m_pmci_args->set_position_change(FALSE);
	m_pmci_args->set_desired_position(0);
	m_pmci_args->hWnd = m_hWnd;
	m_pmci_args->set_stop(FALSE);
	m_pmci_args->set_done(FALSE);

	return (TRUE);
}

BOOL CMusic::reinit_MPEG()
{
	m_bMPEG = TRUE;
	m_args = m_pmaplay_args;

	m_pmaplay_args->stream = new Ibitstream(m_szName, m_hWnd);
	m_pmaplay_args->MPEGheader = new Header;
	m_pmaplay_args->phwo = phwo;
	m_pmaplay_args->hWnd = m_hWnd;
	m_pmaplay_args->huffdec_path = m_table_path;
	m_pmaplay_args->which_c = m_mode;
	m_pmaplay_args->set_position_change(FALSE);
	m_pmaplay_args->set_desired_position(0);
	m_pmaplay_args->set_stop(FALSE);
	m_pmaplay_args->set_done(FALSE);

	if (!m_pmaplay_args->MPEGheader->read_header (m_pmaplay_args->stream,&m_pCrc)){
	//	MessageBox("No header found!","File format error", MB_OK | MB_ICONSTOP);
		return(FALSE);
	}

//	m_scroll_range = m_pmaplay_args->MPEGheader->min_number_of_frames(m_pmaplay_args->stream);
//	m_line_size = m_scroll_range >> 3;

	return(TRUE);
}
BOOL CMusic::Play( LPCTSTR lpszFileName, BOOL bRepeat )
{
	if( m_nMusicVolume == 0 )
		return FALSE;
	TRACE("Play 1 \n");
	m_bRepeat = bRepeat;
	strcpy(m_szName,lpszFileName);
	TRACE("Play 2 \n");
	ClearSound();
	TRACE("Play 3 \n");
	m_bListMode=FALSE;
	TRACE("Play 4 \n");
	//if (!File_Init()) return FALSE;
	TRACE("Play 5 \n");
	ok_play();
	TRACE("Play 6 \n");
	no_stop();
	TRACE("Play 7 \n");
	Play();
	return TRUE;
}

void CMusic::Play()
{
	if( m_nMusicVolume == 0 )
		return;
	if (m_bcan_play) 
	{
		TRACE("Play 8 \n");
		no_play();
		TRACE("Play 9 \n");
		File_Init();

		if (m_bMPEG)
		{
			TRACE("Play 10 \n");
			if (m_bpaused && m_hMPEG_Thread!=NULL) 
			{
				TRACE("Play 11 \n");
				m_bpaused=FALSE;
				waveOutRestart(*phwo);
				TRACE("Play 12 \n");
				ResumeThread(m_hMPEG_Thread);
			} 
			else
			{
				TRACE("Play 13 \n");
				MPEG_play();
			}
		} 
		else 
		{
			TRACE("Play 14 \n");
			MCI_play();
			TRACE("Play 15 \n");
		}
	//	::SendMessage(m_status_bar,SB_SETTEXT, 0,(LPARAM) "Playing...");
	}
	return;
}
void CMusic::Pause()
{
	TRACE("Pause 1 \n");
    if (m_bcan_pause) {

      m_bcan_pause = FALSE;
        m_bpaused    = TRUE;
       // CMDLINE   = FALSE;

		TRACE("Pause 2 \n");
        if (m_bMPEG) {
			TRACE("Pause 3 \n");
            SuspendThread(m_hMPEG_Thread);
			TRACE("Pause 4 \n");
            waveOutPause(*phwo);
        } else {
            m_pmci_args->playing = FALSE;
			TRACE("Pause 5 \n");
            mciSendString ((LPSTR)"pause sounder", NULL, 0, NULL);
        }
		TRACE("Pause 6 \n");
        ok_play();
        
        return;
   }
}
void CMusic::Stop()
{
	TRACE("Stop 1 \n");
	Pause();
	TRACE("Stop 2 \n");
    if (m_bcan_stop) {

		TRACE("Stop 3 \n");
        ClearSound();

      m_bcan_stop  = FALSE; 
      m_bCMDLINE   = FALSE;
      m_bpaused    = FALSE;
      //save_mode = FALSE;
	  TRACE("Stop 4 \n");
        File_Init();
		TRACE("Stop 5 \n");
        ok_play();
		TRACE("Stop 6 \n");
        no_stop();
   }
}

BOOL CMusic::MCI_play()
{
	char Buffer[1024];    	 // MCI response string
	
	m_pmci_args->playing = TRUE;

	mciSendString ((LPSTR)"play sounder notify", Buffer, 1024, m_hWnd);

	m_hMCI_Thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) mciplay,
									  m_pmci_args, 0, &m_dwThreadId);
	return(TRUE);
}

void CMusic::no_play()
{
	m_bcan_play=FALSE;
	m_bcan_pause=TRUE;
	m_bcan_stop=TRUE;
	m_bplaying=TRUE;
	return;
}

BOOL CMusic::MPEG_play() // Assumes stream, MPEGheader, crc are already initialized
{
	BOOL t;
	m_hMPEG_Thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) maplay,
										m_pmaplay_args, 0, &m_dwThreadId);
	t=SetThreadPriority(m_hMPEG_Thread, THREAD_PRIORITY_ABOVE_NORMAL);
    return(t);
}

void CMusic::Uninit() 
{
	TRACE("Debug 1\n");
	Stop();
	ClearSound();
	TRACE("Debug 10\n");

	/*
	if( m_pmaplay_args->stream )
		delete m_pmaplay_args->stream;
	if( m_pmaplay_args->MPEGheader ) 
		delete m_pmaplay_args->MPEGheader;
	*/
}
