/*
 *  @(#) obuffer.cc 1.15, last edit: 6/23/94 13:04:36
 *  @(#) Copyright (C) 1993, 1994 Tobias Bading (bading@cs.tu-berlin.de)
 *  @(#) Berlin University of Technology
 *
 *  Idea and first implementation for u-law output with fast downsampling by
 *  Jim Boucher (jboucher@flash.bu.edu)
 *
 *  LinuxObuffer class written by
 *  Louis P. Kruger (lpkruger@phoenix.princeton.edu)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 *  Changes from version 1.1 to 1.2:
 *    - new LinuxObuffer class, which works with 16 bit soundcards
 *      like Gravis Ultrasound, SoundBlaster 16 or Pro Audio Spectrum 16,
 *      if attached to /dev/dsp
 *    - ShortObuffer renamed to FileObuffer
 *    - If ULAW is defined:
 *      - SparcObuffer feeds mono u-law output to an amd device (u-law format).
 *        The required downsampling to 8 kHz in done in the synthesis filter.
 *      - FileObuffer creates u-law output at 8 kHz instead of 16 bit PCM samples.
 *    - O_NDELAY flag is cleared now after a successful open syscall
 *      on /dev/audio (caused problems under Solaris)
 *    - options -us, -uh and -ul should work now
 *    - FileObuffer can handle incomplete write syscalls now
 */

/* MCIbuffer class added by Jeff Tsay (last edit: 05/18/96)
	Writing to Windows multimedia system with such a small
	buffer won't do, so we increase buffer size, and ignore
	most of the writes. Also we need to have a queue of 3 wave
	headers for smooth playback. The last header and the oldest
	header play while the current one is being filled in.

	Note that this does not support 8-bit soundcards yet! I don't
	know what the data format is to that */

/* Changes since last version: changed GMEM_MOVEABLE to GMEM_FIXED
	which is a bit faster, according to the suggestions of Paul Forgey.
	clear_buffer() added for seeking. Changes int16 parameter to int32
	for minor speed improvement. Last edit: 06/29/96. */

/* More changes: eliminated GlobalLock on the fixed memory, which
   is unnecessary.

   Added a function clear_buffer(), which clears the audio data
   when a seek is made by the user.

   Also added a function set_stop_flag() which
   sets a flag when playback is interrupted by the user. Then
   no attempt is made to unprepare headers after waveReset() is
   called by the main thread.

   Last edit: 10/12/96. */

/* Replaced usage of the PCMWAVEFORMAT structure with the more up
   to date WAVEFORMATEX structure.

   According to the suggestions of Sergey Kuzmin, replaced
   GlobalAlloc and GlobalFree with new and delete, which should
   eliminate memory leaks.

   Last edit: 12/31/96. */

#define STRICT
#define WIN32_LEAN_AND_MEAN
#define NOMCX
#define NOIME
#define NOGDI
#define NOUSER
#define NOSOUND
#define NOCOMM
#define NODRIVERS
#define OEMRESOURCE
#define NONLS
#define NOSERVICE
#define NOKANJI
#define NOMINMAX
#define NOLOGERROR
#define NOPROFILER
#define NOMEMMGR
#define NOLFILEIO
#define NOOPENFILE
#define NORESOURCE
#define NOATOM
#define NOLANGUAGE
#define NOLSTRING
#define NODBCS
#define NOKEYBOARDINFO
#define NOGDICAPMASKS
#define NOCOLOR
#define NOGDIOBJ
#define NODRAWTEXT
#define NOTEXTMETRIC
#define NOSCALABLEFONT
#define NOBITMAP
#define NORASTEROPS
#define NOMETAFILE
#define NOSYSMETRICS
#define NOSYSTEMPARAMSINFO
#define NOMSG
#define NOWINSTYLES
#define NOWINOFFSETS
#define NOSHOWWINDOW
#define NODEFERWINDOWPOS
#define NOVIRTUALKEYCODES
#define NOKEYSTATES
#define NOWH
#define NOMENUS
#define NOSCROLL
#define NOCLIPBOARD
#define NOICONS
#define NOMB
#define NOSYSCOMMANDS
#define NOMDI
#define NOCTLMGR
#define NOWINMESSAGES
#define NOHELP
#define _WINUSER_
#define __oleidl_h__
#define _OLE2_H_

#include "stdafx.h"


#include "obuffer.h"
#include "obuffer5.h"  // Windows addition
#include "header.h"

MCIbuffer::MCIbuffer (uint32 number_of_channels,
							 Header *header,
							 HWAVEOUT *phwo0)
{
  int32 i;
  LPWAVEHDR temp;
  channels= number_of_channels;
  data_size= channels * BUFFERSIZE;

  hdr_size= sizeof(WAVEHDR);
  lpwavehdr_arr=new LPWAVEHDR[3];
  phwo=phwo0;
  fillup=0;

  lpwf = (tWAVEFORMATEX *) new WAVEFORMATEX;

  lpwf->wBitsPerSample=16;  // No 8-bit support yet
  lpwf->wFormatTag=WAVE_FORMAT_PCM;
  lpwf->nChannels=(WORD) channels;
  lpwf->nSamplesPerSec=(DWORD) header->frequency ();
  lpwf->nAvgBytesPerSec=(DWORD) channels * header->frequency() << 1;
  lpwf->nBlockAlign=(WORD) (channels << 1);
  lpwf->cbSize = 0;

  if (waveOutOpen(phwo, WAVE_MAPPER, (const tWAVEFORMATEX *) lpwf,
					 NULL, NULL, WAVE_ALLOWSYNC))
		ExitThread(1L);

  buffer_count=0;

  for(i=0; i<3; i++) {
	  lpwavehdr_arr[i] = (LPWAVEHDR) new WAVEHDR;

	  temp = lpwavehdr_arr[i];
	  if(!temp) ExitThread(1L);

	  temp->lpData=(LPSTR) new char[data_size];

	  if(!temp->lpData) ExitThread(1L);
	  temp->dwBufferLength=data_size;
	  temp->dwBytesRecorded=0L;
	  temp->dwUser=0L;  // If played, dwUser = 1
	  temp->dwLoops=0L;
	  temp->dwFlags=NULL;
  }

  for(i=0; i<channels; i++)
	bufferp[i]=i * channels;

  user_stop = 0;
}

void MCIbuffer::append (uint32 channel, int32 value)
{
  // Need to break up the 32-bit integer into 2 8-bit bytes.
  // (ignore the first two bytes - either 0x0000 or 0xffff)
  // Note that Intel byte order is backwards!!!

  LPSTR temp=lpwavehdr_arr[2]->lpData;
  temp[bufferp[channel]]= (char) (value & 0xff);
  temp[bufferp[channel]+1] = (char) (value >> 8);
  bufferp[channel]+= channels << 1;

  return;
}

//#pragma argsused
void MCIbuffer::write_buffer (int fd)
{
	// Actually write only when buffer is actually full.
	if ((++buffer_count & BIT_SELECT)==0) {

		int32 i;

		buffer_count=0;

		// Wait for 2 completed headers
		if (fillup>1) {

			// Prepare+write newest header
			while (waveOutPrepareHeader(*phwo, lpwavehdr_arr[2],
						hdr_size))
				Sleep(SLEEPTIME);

			while(waveOutWrite(*phwo, lpwavehdr_arr[2], hdr_size))
				Sleep(SLEEPTIME);

			// Header has now been sent
			lpwavehdr_arr[2]->dwUser=1L;

			wave_swap();

			// Unprepare oldest header
			if (lpwavehdr_arr[2]->dwUser) {

				while(waveOutUnprepareHeader(*phwo, lpwavehdr_arr[2],
					hdr_size)==WAVERR_STILLPLAYING)
					Sleep(SLEEPTIME);
			}

		} else {

			if (++fillup == 2) {

					// Write the previously calculated 2 headers
					while (waveOutPrepareHeader(*phwo, lpwavehdr_arr[0],
						hdr_size))
						Sleep(SLEEPTIME);

					while(waveOutWrite(*phwo, lpwavehdr_arr[0], hdr_size))
						Sleep(SLEEPTIME);

					// Header has now been sent
					lpwavehdr_arr[0]->dwUser=1L;

					wave_swap();

					while (waveOutPrepareHeader(*phwo, lpwavehdr_arr[0],
						hdr_size))
						Sleep(SLEEPTIME);

					while(waveOutWrite(*phwo, lpwavehdr_arr[0], hdr_size))
						Sleep(SLEEPTIME);

					// Header has now been sent
					lpwavehdr_arr[0]->dwUser=1L;

			} else
				wave_swap();
		}

		for(i=0; i<channels; i++)
		  bufferp[i]=i * channels;
	}
	return;
}

void MCIbuffer::wave_swap(void)
{
	int32 i;
	LPWAVEHDR temp=lpwavehdr_arr[2];

	for(i=2;i>0; i--)
		lpwavehdr_arr[i]=lpwavehdr_arr[i-1];

	lpwavehdr_arr[0]=temp;
	return;
}

void MCIbuffer::clear_buffer(void)
// Clear all the data in the buffers
{
	int32 i, j;
	LPWAVEHDR temp;

	waveOutReset(*phwo);

	for(i=0; i<3; i++) {

		temp = lpwavehdr_arr[i];

		if (temp->dwUser)
			waveOutUnprepareHeader(*phwo, temp, hdr_size);

		temp->dwUser = 0;

		for(j=0; j<data_size; j++)
			temp->lpData[j] = (char) 0;
	}

	// Reset buffer pointers
	for(i=0; i<channels; i++)
		bufferp[i]=i * channels;

	// Force the buffers to fillup before playing.
	fillup = buffer_count = 0;
}

void MCIbuffer::set_stop_flag(void)
// Set the flag to avoid unpreparing non-existent headers
{
	user_stop = 1;
   return;
}


MCIbuffer::~MCIbuffer(void)
{
	int32 i, j;

	if ((fillup==1) && !user_stop) {

		// Write the last header calculated (at the top of the array).
		while (waveOutPrepareHeader(*phwo, lpwavehdr_arr[0], hdr_size))
			Sleep(SLEEPTIME);

		while(waveOutWrite(*phwo, lpwavehdr_arr[0], hdr_size))
			Sleep(SLEEPTIME);

		// Header has been written.
		lpwavehdr_arr[0]->dwUser=1L;
	}

	if (buffer_count && !user_stop) {

	/* Write the last wave header (probably not be written due to buffer size
		 increase.) */

		for(i= bufferp[channels-1]; i< data_size; i++)
			 lpwavehdr_arr[2]->lpData[i]= (char) 0;

		while (waveOutPrepareHeader(*phwo, lpwavehdr_arr[2], hdr_size))
			Sleep(SLEEPTIME);

		while(waveOutWrite(*phwo, lpwavehdr_arr[2], hdr_size))
			Sleep(SLEEPTIME);

		// Header has been written.
		lpwavehdr_arr[2]->dwUser=1L;

		wave_swap();
	}

	// Unprepare and free the header memory.
	for (j=2; j>=0; j--) {
		if (lpwavehdr_arr[j]->dwUser && !user_stop)
			while (waveOutUnprepareHeader(*phwo, lpwavehdr_arr[j], hdr_size)
         		 ==WAVERR_STILLPLAYING)
				Sleep(SLEEPTIME);

      safe_delete_array( lpwavehdr_arr[j]->lpData );
      safe_delete( lpwavehdr_arr[j] );
	}

	safe_delete_array( lpwavehdr_arr );
	safe_delete( lpwf );

   if (!user_stop)	// Wave device already closed if user_stop
		while(waveOutClose(*phwo))
			Sleep(SLEEPTIME);

	return;
}
