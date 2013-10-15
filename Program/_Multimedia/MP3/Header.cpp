/*
 *  @(#) header.cc 1.8, last edit: 6/15/94 16:51:44
 *  @(#) Copyright (C) 1993, 1994 Tobias Bading (bading@cs.tu-berlin.de)
 *  @(#) Berlin University of Technology
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
 *    - iostreams manipulator calls like "cerr << setw (2) << ..." replaced by
 *      "cerr.width (2); ..." due to problems with older GNU C++ releases.
 *    - syncword recognition slightly changed
 */

/* A few layer III and seeking modifications made by Jeff Tsay.

   Last modified : 01/31/97 */

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

#include "header.h"

const uint32 Header::frequencies[3] = { 44100, 48000, 32000 };

BOOL Header::read_header (Ibitstream *stream, Crc16 **crcp)
{
  uint32 headerstring;

  if (!stream->get_header (&headerstring))
	 return(FALSE);

/*  if ((h_layer = (headerstring >> 17) & 3) == 0)
  {
	 cerr << "unknown layer identifier found!\n";
	 exit (1);
  }
  h_layer = 4 - h_layer;		// now 1 means Layer I and 3 means Layer III   */

  h_layer = 4 - (headerstring >> 17) & 3;

  h_protection_bit = (headerstring >> 16) & 1;
  h_bitrate_index  = (headerstring >> 12) & 0xF;

/*  if ((h_bitrate_index = (headerstring >> 12) & 0xF) == 15)
  {
	 cerr << "unknown bitrate index found!\n";
	 exit (1);
  }           */
/*  if (!h_bitrate_index)
  {
	 cerr << "free format not yet implemented!\n";
	 exit (1);
  } */

  h_sample_frequency = (e_sample_frequency) ((headerstring >> 10) & 3);

/*  if ((h_sample_frequency = (e_sample_frequency)((headerstring >> 10) & 3)) == 3)
  {
	 cerr << "unknown sample frequency!\n";
	 exit (1);
  }          */

  h_padding_bit = (headerstring >> 9) & 1;
  h_mode = (e_mode)((headerstring >> 6) & 3);

/*  if (h_layer == 2)
	 // testing validity of mode and bitrate:
	 if ((((h_bitrate_index >= 1 && h_bitrate_index <= 3) || h_bitrate_index == 5) &&
	 h_mode != single_channel) ||
	(h_bitrate_index >= 11 && h_mode == single_channel))
	 {
		cerr << "illegal combination of mode and bitrate in a layer II stream:\n"
			"  mode: " << mode_string ()
		<< "\n  bitrate: " << bitrate_string () << '\n';
		exit (1);
	 } */


  h_mode_extension = (headerstring >> 4) & 3;

  if (h_mode == joint_stereo)
	 h_intensity_stereo_bound = (h_mode_extension << 2) + 4;
  else
	 h_intensity_stereo_bound = 0;		// should never be used

  h_copyright = (BOOL) ((headerstring >> 3) & 1);
  h_original = (BOOL) ((headerstring >> 2) & 1);

  // calculate number of subbands:
  if (h_layer == 1)
	 h_number_of_subbands = 32;
  else
  {
	 uint32 channel_bitrate = h_bitrate_index;

	 // calculate bitrate per channel:
	 if (h_mode != single_channel)
		if (channel_bitrate == 4)
	channel_bitrate = 1;
		else
	channel_bitrate -= 4;

	 if (channel_bitrate == 1 || channel_bitrate == 2)
		if (h_sample_frequency == thirtytwo)
	h_number_of_subbands = 12;
		else
	h_number_of_subbands = 8;
	 else
		if (h_sample_frequency == fourtyeight || (channel_bitrate >= 3 && channel_bitrate <= 5))
	h_number_of_subbands = 27;
		else
	h_number_of_subbands = 30;
  }

  if (h_intensity_stereo_bound > h_number_of_subbands)
	 h_intensity_stereo_bound = h_number_of_subbands;

  // read framedata:
  if (!stream->read_frame (calculate_framesize()))
	 return(FALSE);

  if (!h_protection_bit)
  {
	 // frame contains a crc checksum
	 checksum = (uint16)stream->get_bits (16);
	 if (!crc)
		crc = new Crc16;
	 crc->add_bits (headerstring, 16);
	 *crcp = crc;
  }
  else
	 *crcp = (Crc16 *)0;

  if (h_sample_frequency == fourtyfour_point_one) {

	 if (!stream->offset)
		stream->offset = new int32 [max_number_of_frames(stream)];

	 if (stream->current_frame > 0) {
			if (stream->current_frame == stream->last_frame_number)
			  stream->offset[stream->current_frame] =
					stream->offset[stream->current_frame - 1] + h_padding_bit;
		} else {
		  stream->offset[0] = h_padding_bit;
		}
  }

  return(TRUE);
}


uint32 Header::calculate_framesize ()
/* calculates framesize in bytes excluding header size */
{
  static const int32 bitrates[3][15] = {
	{0 /*free format*/, 32000, 64000, 96000, 128000, 160000, 192000,
	 224000, 256000, 288000, 320000, 352000, 384000, 416000, 448000},
	{0 /*free format*/, 32000, 48000, 56000, 64000, 80000, 96000,
	 112000, 128000, 160000, 192000, 224000, 256000, 320000, 384000},
	{0 /*free format*/, 32000, 40000, 48000, 56000, 64000, 80000,
	 96000, 112000, 128000, 160000, 192000, 224000, 256000, 320000}
  };

  static const samplefrequencies[3] = { 44100, 48000, 32000 };
  uint32 framesize;

  if (h_layer == 1)
  {
	 framesize = (12 * bitrates[0][h_bitrate_index]) / samplefrequencies[h_sample_frequency];
	 if (h_sample_frequency == fourtyfour_point_one && h_padding_bit)
		++framesize;
	 framesize <<= 2;		// one slot is 4 bytes long
  }
  else
  {
	 framesize = (144 * bitrates[h_layer - 1][h_bitrate_index]) / samplefrequencies[h_sample_frequency];
	 if (h_padding_bit) framesize++;

	 // Layer III slots
	 if (h_layer == 3)
		 nSlots = framesize - ((h_mode == single_channel) ? 17 : 32) // side info size
								  -  (h_protection_bit ? 0 : 2) 		       // CRC size
								  - 4; 								             // header size
    else
    	 nSlots = 0;
  }

  return(framesize - 4);		// subtract header size
}

const char *Header::layer_string (void)
{
  switch (h_layer)
  {
	 case 1:
		return "I  ";
	 case 2:
		return "II ";
	 case 3:
		return "III";
  }
  return NULL;			// dummy
}


const char *Header::bitrate_string (void)
{

  static const char *bitrate_str[3][16] = {
	{"free format", "32 kbit/s", "64 kbit/s", "96 kbit/s", "128 kbit/s", "160 kbit/s",
	 "192 kbit/s", "224 kbit/s", "256 kbit/s", "288 kbit/s", "320 kbit/s", "352 kbit/s",
	 "384 kbit/s", "416 kbit/s", "448 kbit/s", "forbidden"},
	{"free format", "32 kbit/s", "48 kbit/s", "56 kbit/s", "64 kbit/s", "80 kbit/s",
	 "96 kbit/s", "112 kbit/s", "128 kbit/s", "160 kbit/s", "192 kbit/s", "224 kbit/s",
	 "256 kbit/s", "320 kbit/s", "384 kbit/s", "forbidden"},
	{"free format", "32 kbit/s", "40 kbit/s", "48 kbit/s", "56 kbit/s", "64 kbit/s",
	 "80 kbit/s" , "96 kbit/s", "112 kbit/s", "128 kbit/s", "160 kbit/s",
	 "192 kbit/s", "224 kbit/s", "256 kbit/s", "320 kbit/s", "forbidden"}
  };

  return(bitrate_str[h_layer - 1][h_bitrate_index]);
}

const char *Header::sample_frequency_string (void)
{
  switch (h_sample_frequency)
  {
	 case thirtytwo:
		return("32 kHz");
	 case fourtyfour_point_one:
		return("44.1 kHz");
	 case fourtyeight:
		return("48 kHz");
  }
  return(NULL);			// dummy
}

const char *Header::mode_string (void)
{
  switch (h_mode)
  {
	 case stereo:
		return("Stereo        ");
	 case joint_stereo:
		return("Joint stereo  ");
	 case dual_channel:
		return("Dual channel  ");
	 case single_channel:
		return("Single channel");
  }
  return(NULL);			// dummy
}

// Stream searching routines
uint32 Header::max_number_of_frames (Ibitstream *stream)
// Returns the maximum number of frames in the stream
{
	  return ( stream->file_size() /
				  (calculate_framesize() + 4 - h_padding_bit) );
}

uint32 Header::min_number_of_frames (Ibitstream *stream)
// Returns the minimum number of frames in the stream
{
	  return ( stream->file_size() /
				  (calculate_framesize() + 5 - h_padding_bit) );
}

static real ms_per_frame_array[3][3] = {{8.707483f,  8.0f, 12.0f},
											       {26.12245f, 24.0f, 36.0f},
                                        {26.12245f, 24.0f, 36.0f}};

real Header::ms_per_frame(void)
{
	return (ms_per_frame_array[h_layer-1][h_sample_frequency]);
}

