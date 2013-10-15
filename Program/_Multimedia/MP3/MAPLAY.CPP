/* Adapted to Win32 version by Jeff Tsay. Differences: This is a thread
	of the the main program, which is the interface. Header, bitstream, and
	CRC come from the main program. Minimal error checking.
	CRC and range check violations ignored. Playtime not printed. Buffers
	initialized differently. Most global variables have been replaced with
	local ones. Last edit 3/09/96. */

/* Version 1.80 changes: (last edit : 01/31/97)

	Before every frame, maplay check if the user has asked it to
   stop or seek.
   which will be reflected in the maplay_args.

   Eliminated reading of crc from maplay_args.

   Layer III decoder object used for layer III frames. */


/*
 *  @(#) maplay.cc 1.20, last edit: 6/22/94 12:32:55
 *  @(#) Copyright (C) 1993, 1994 Tobias Bading (bading@cs.tu-berlin.de)
 *  @(#) Berlin University of Technology
 *
 *  Many thanks for ideas and implementations to:
 *  -> Jim Boucher (jboucher@flash.bu.edu)
 *     for his idea and first implementation of 8 kHz u-law output
 *  -> Louis P. Kruger (lpkruger@phoenix.princeton.edu)
 *     for his implementation of the LinuxObuffer class
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
 *    - minor changes to create a LinuxObuffer object
 *    - minor changes for a u-law version, which creates 8 kHz u-law output
 *      on an amd device or in stdout mode, if compiled with ULAW defined
 *    - option -amd forces maplay to treat /dev/audio as an amd device
 *      in the u-law version. This is helpful on some SPARC clones.
 *    - iostreams manipulator calls like "cerr << setw (2) << ..." replaced by
 *      "cerr.width (2); ..." due to problems with older GNU C++ releases.
 */


#define STRICT
#define WIN32_LEAN_AND_MEAN
#define NOMCX
#define NOIME
// #define NOGDI             GDI APIs and definitions
// #define NOUSER            USER APIs and definitions
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
// #define NOICONS
// #define NOMB
#define NOSYSCOMMANDS
#define NOMDI
#define NOCTLMGR
#define NOWINMESSAGES
#define NOHELP
// #define _WINUSER_
#define __oleidl_h__
#define _OLE2_H_

#include "stdafx.h"
#include "usrmsg.h"

#define MMNODRV
#define MMNOSOUND
#define MMNOMIDI
#define MMNOAUX
#define MMNOTIMER
#define MMNOJOY
#define MMNOMCI
#define MMNOMMIO
#define MMNOMMSYSTEM
#include <mmsystem.h>
#include "all.h"
#include "crc.h"
#include "header.h"
#include "subband.h"
#include "sublay1.h"
#include "sublay2.h"
#include "synfilt.h"
#include "ibitstr.h"
#include "obuffer.h"
#include "args.h"
#include "bit_res.h"
#include "scalfact.h"
#include "l3type.h"
#include "layer3.h"
//#include "resource.h"

// data extracted from commandline arguments:

SynthesisFilter *filter1 = NULL, *filter2 = NULL;
Obuffer *buffer = NULL;
Header *header = NULL;
Subband *subbands[32] = {NULL, NULL, NULL, NULL,
								 NULL, NULL, NULL, NULL,
                         NULL, NULL, NULL, NULL,
                         NULL, NULL, NULL, NULL,
                         NULL, NULL, NULL, NULL,
                         NULL, NULL, NULL, NULL,
                         NULL, NULL, NULL, NULL,
                         NULL, NULL, NULL, NULL};

LayerIII_Decoder *l3decoder = NULL;

DWORD maplay_Exit (DWORD returncode)
{
  if ((returncode == 1) && buffer) {
  	  buffer->set_stop_flag();
  }

  safe_delete( buffer );
  buffer = NULL;

  safe_delete( filter1 );
  filter1 = NULL;
  safe_delete( filter2 );
  filter2 = NULL;

  safe_delete( l3decoder );
  l3decoder = NULL;

  return(0L);
}

DWORD maplay (MPEG_Args *maplay_args)
{
	enum e_channels which_channels = maplay_args->which_c;

	uint32 layer;
	e_mode mode;
	e_sample_frequency sample_frequency;

	int32 i;
	BOOL read_ready = FALSE, write_ready = FALSE;

	Crc16 *crc = NULL;
	Ibitstream *stream = maplay_args->stream;
	header = maplay_args->MPEGheader;
	HWND hWnd = maplay_args->hWnd;
	HWAVEOUT *phwo = maplay_args->phwo;
	char *huffdec_path = maplay_args->huffdec_path;

	// get info from header of first frame:
	layer = header->layer ();
	if ((mode = header->mode ()) == single_channel)
	   which_channels = left;

	sample_frequency = header->sample_frequency ();

	// create filter(s):
	filter1 = new SynthesisFilter (0);

	if (mode != single_channel && which_channels == both)
	   filter2 = new SynthesisFilter (1);

	// create buffer:

	if (mode == single_channel || which_channels != both)
		buffer = new MCIbuffer (1, header, phwo);	// mono
	else
		buffer = new MCIbuffer (2, header, phwo);	// stereo

	// Layer III : initialize decoder

	if (layer == 3) 
	{
	
		if (mode == single_channel)
			l3decoder = new LayerIII_Decoder(huffdec_path, stream, header, filter1,  NULL, buffer);
		else
			l3decoder = new LayerIII_Decoder(huffdec_path, stream, header, filter1, filter2, buffer);
	}
	do
	{

		if (maplay_args->stop_query()) 
		{
		  maplay_Exit(1L);
		  return(0);
		}
		if (maplay_args->position_change_query()) 
		{
			buffer->clear_buffer();

			if (header->sample_frequency() != fourtyfour_point_one)
				stream->seek(maplay_args->desired_position_query(), header->calculate_framesize());
			else
				stream->seek_pad(maplay_args->desired_position_query(), header->calculate_framesize() - header->padding());

			maplay_args->set_position_change(FALSE);

			if (!header->read_header (stream, &crc))
				goto quit;

			if (l3decoder)
				l3decoder->seek_notify();

			//SendMessage(hWnd, SEEK_ACK, NULL, NULL);
			//SendMessage(hWnd, SCROLL_POS, stream->current_frame, 0);
		}
		// is there a change in important parameters?
		// (bitrate switching is allowed)
		if (header->layer () != layer)
		{
			// layer switching is allowed

			if (header->layer() == 3) 
			{

				if (mode == single_channel)
					l3decoder = new LayerIII_Decoder(huffdec_path, stream, header, filter1, NULL, buffer);
				else
					l3decoder = new LayerIII_Decoder(huffdec_path, stream, header, filter1, filter2, buffer);
			} 
			else if (layer == 3) 
			{
				safe_delete( l3decoder );
				l3decoder = NULL;
			}
			layer = header->layer ();
		}
		if ((mode == single_channel && header->mode () != single_channel) || (mode != single_channel && header->mode () == single_channel))
		{
			MessageBox(hWnd,"Illegal switch from single channel to stereo or vice versa.", "Decoding Error", MB_ICONEXCLAMATION);
			maplay_Exit (0L);
			ExitThread(1L);
		}
		if (header->sample_frequency () != sample_frequency)
		{
			MessageBox(hWnd, "Sorry, can't switch the sample frequency in the middle of the stream.", "Decoding Error", MB_ICONEXCLAMATION);
			maplay_Exit (0L);
			ExitThread(1L);
			return(0L);
		}
		if (header->layer() != 3) 
		{

			// create subband objects:
			if (header->layer () == 1)
			{
				if (header->mode () == single_channel)
					for (i = 0; i < header->number_of_subbands (); ++i)
						subbands[i] = new SubbandLayer1 (i);
				else 
				if (header->mode () == joint_stereo)
				{
					for (i = 0; i < header->intensity_stereo_bound (); ++i)
						subbands[i] = new SubbandLayer1Stereo (i);
					for (; i < header->number_of_subbands (); ++i)
						subbands[i] = new SubbandLayer1IntensityStereo (i);
				}
				else
				for (i = 0; i < header->number_of_subbands (); ++i)
					subbands[i] = new SubbandLayer1Stereo (i);
			}
			else 
			if (header->layer () == 2)
			{
				if (header->mode () == single_channel)
				{
					for (i = 0; i < header->number_of_subbands (); ++i)
						subbands[i] = new SubbandLayer2 (i);
				}
				else 
				if (header->mode () == joint_stereo)
				{
					for (i = 0; i < header->intensity_stereo_bound (); ++i)
						subbands[i] = new SubbandLayer2Stereo (i);
					for (; i < header->number_of_subbands (); ++i)
						subbands[i] = new SubbandLayer2IntensityStereo (i);
				}
				else
				for (i = 0; i < header->number_of_subbands (); ++i)
					subbands[i] = new SubbandLayer2Stereo (i);
			}
			// start to read audio data:
			for (i = 0; i < header->number_of_subbands (); ++i)
				subbands[i]->read_allocation (stream, header, crc);

			if (header->layer () == 2)
			{
				for (i = 0; i < header->number_of_subbands (); ++i)
					((SubbandLayer2 *)subbands[i])->read_scalefactor_selection (stream, crc);
			}
			if (!crc || header->checksum_ok ())
			{
				// no checksums or checksum ok, continue reading from stream:
				for (i = 0; i < header->number_of_subbands (); ++i)
					subbands[i]->read_scalefactor (stream, header);
				do
				{
					for (i = 0; i < header->number_of_subbands (); ++i)
						read_ready = subbands[i]->read_sampledata (stream);

					do
					{
						for (i = 0; i < header->number_of_subbands (); ++i)
							write_ready = subbands[i]->put_next_sample (which_channels,  filter1, filter2);

						filter1->calculate_pcm_samples (buffer);
						if (which_channels == both && header->mode () != single_channel)
							filter2->calculate_pcm_samples (buffer);

					} while (!write_ready);

				} while (!read_ready);

				buffer->write_buffer (1);		// write to stdout
			}
			// Jeff : Don't let user know if crc violated.
			//	 else
			// Sh*t! Wrong crc checksum in frame!
			//		cerr << "WARNING: frame contains wrong crc checksum! (throwing frame away)\n";
			for (i = 0; i < header->number_of_subbands (); ++i) 
			{
				safe_delete( subbands[i] );
				subbands[i] = NULL;
			}
		} 
		else 
		{  // Layer III
			l3decoder->decode();
		}
		// Send notification to the scroll bar every 16 frames
		//if (!(stream->current_frame & 0xf) &&  (!maplay_args->stop_query()))
		//	SendMessage(hWnd, SCROLL_POS, stream->current_frame, 0);

	}
	while (header->read_header (stream, &crc));

	quit:
	// Saturate the scroll bar
	//  if (!maplay_args->stop_query())
	//	  SendMessage(hWnd, SCROLL_POS, stream->current_frame, 0);

	safe_delete( buffer );
	buffer = NULL;

	safe_delete( filter1 );
	filter1 = NULL;
	safe_delete( filter2 );
	filter2 = NULL;

	safe_delete( l3decoder );
	l3decoder = NULL;

	maplay_args->set_done(TRUE);
	SendMessage(hWnd, WM_MUSICTHREADEND, NULL, NULL);
	return(0);
}
