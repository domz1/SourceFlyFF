#ifndef __MUSIC_H
#define __MUSIC_H

#include "crc.h"
#include "ibitstr.h"
#include "header.h"
#include "args.h"
#include "subband.h"
#include "my_str_lib.h"

class CMusic
{
	HWND m_hWnd;

	/* Argument containers */
	MPEG_Args m_maplay_args, *m_pmaplay_args;
	MCI_Args m_mci_args, *m_pmci_args;
	Args_* m_args;

	/*  WaveOut handle and its address */
	HWAVEOUT hwo, *phwo;

	// Dummy CRC pointer
	Crc16 *m_pCrc;


	/* file names */
	char m_szName[256];    	// Name of sound file

	char m_table_path[256];   // Name and path of huffman decoder table
	char scratch[64];

	/* status variable */
	BOOL m_bListMode;
	BOOL m_bMPEG ;
	BOOL m_bRepeat;
	BOOL m_bCMDLINE;
	BOOL m_bpaused;
	BOOL m_bcan_play;
	BOOL m_bcan_pause;
	BOOL m_bcan_stop;
	BOOL m_bplaying;
	//BOOL m_bscrolling;

	/* thread */
	HANDLE m_hMPEG_Thread;
	HANDLE m_hMCI_Thread;
	DWORD m_dwThreadId;

	char m_command[384];   // MCI command string
	enum e_channels m_mode;
public:
	void ClearSound(void);
	BOOL File_Init();
	void ok_play();
	void no_stop();
	BOOL WaveP(LPSTR t);
	BOOL reinit_MPEG();
	BOOL reinit_MCI();
	void audio_Info();
	void clear_sound();
	BOOL MCI_play();
	void no_play();
	BOOL MPEG_play();

public:
	int m_nMusicVolume;
	CMusic();
	~CMusic();
	void Uninit();
	void Init(HWND hWnd);

	BOOL Play( LPCTSTR lpszFileName, BOOL bRepeat = TRUE );
	void Play();
	void Pause();
	void Stop();


};

#endif