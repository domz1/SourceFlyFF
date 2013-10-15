/*
 *  @(#) ibitstream.h 1.5, last edit: 6/15/94 16:55:34
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

#ifndef BITSTREAM_H
#define BITSTREAM_H

#include "all.h"
#include "l3type.h"

const uint32 bufferintsize = 433;
	// max. 1730 bytes per frame: 144 * 384kbit/s / 32000 Hz + 2 Bytes CRC


// Class to extract bitstrings from files:
class Ibitstream
{
//  int		fd;
  HWND hWnd;
  HANDLE FH;
  SECURITY_ATTRIBUTES security;
  uint32	buffer[bufferintsize];
  uint32	framesize;		// number of valid bytes in buffer
  uint32	*wordpointer;		// position of next unsigned int for get_bits()
  uint32	bitindex;		// number (0-31, from MSB to LSB) of next bit for get_bits()


public:

		Ibitstream (const char *filename, HWND hWnd0);
		~Ibitstream (void);

  BOOL		get_header (uint32 *);
		// get next 32 bits from bitstream in an unsigned int,
		// returned value False => end of stream
  BOOL		read_frame (uint32 bytesize);
		// fill buffer with data from bitstream, returned value False => end of stream
  uint32	get_bits (uint32 number_of_bits);
		// read bits (1 <= number_of_bits <= 16) from buffer into the lower bits
		// of an unsigned int. The LSB contains the latest read bit of the stream.


  // Layer 3 routines
  BOOL   get_side_info(uint32 channels, III_side_info_t *side_info);


  // Stream searching routines (Jeff Tsay)

  DWORD  file_size();
		// Returns the size, in bytes, of the input file.

  DWORD  seek(int32 frames, int32 frame_size);
		// Seeks frames for 32 kHz and 48 kHz (non-padded) files

  DWORD  seek_pad(int32 frames, int32 frame_size);
		// Seeks frames for 44.1 kHz (padded) files

  int32 *offset; 		// offset from predicted frame location
  int32 current_frame;
  int32 last_frame_number;

};

#endif
