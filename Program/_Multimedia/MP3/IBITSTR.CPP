/*
 *  @(#) ibitstream.cc 1.8, last edit: 6/15/94 16:51:45
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
 *    - third argument in open syscall added
 *    - minor bug in get_header() fixed
 */

/* Modified for Windows file handles by Jeff Tsay. Last edit 1/26/96. */

/* Added syncword detection for compatibility with streams produced by
   DALET. Changed at the request of Wilfried Solbach, thanks for the
   donation in advance! However Layer I MPP files playback jerkily.
   Last edit : 11/29/96. */

/* Added a routine to read layer III side info. Also not mentioned
   previously are several routines that allow seeking in a stream.
   Last edit : 01/31/97. */

#define STRICT
#define WIN32_LEAN_AND_MEAN
#define NOMCX
#define NOIME
// #define NOGDI
// #define NOUSER
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

#include "all.h"
#include "ibitstr.h"
#include "header.h"
#include "l3type.h"

#define swap_int32(int_32) (((int_32) << 24) | (((int_32) << 8) & 0x00ff0000) | \
				(((int_32) >> 8) & 0x0000ff00) | ((int_32) >> 24))

Ibitstream::Ibitstream (const char *filename, HWND hWnd0)		// constructor
{

  char bad_file_msg[256];
  hWnd=hWnd0;

  security.nLength=sizeof(SECURITY_ATTRIBUTES);
  security.lpSecurityDescriptor=NULL;
  security.bInheritHandle=FALSE;

  FH=CreateFile(filename, GENERIC_READ, FILE_SHARE_READ,
					 &security, OPEN_EXISTING, NULL, NULL);

  if (FH==INVALID_HANDLE_VALUE) {
	lstrcpy(bad_file_msg, "Error opening file: ");
	MessageBox(hWnd, strcat(bad_file_msg, filename) , "Invalid handle", MB_OK);
  }

  wordpointer = buffer;
  bitindex = 0;

  // Seeking variables
  offset = NULL;
  current_frame = -1;
  last_frame_number = -1;
}

Ibitstream::~Ibitstream (void)				// destructor
{
  safe_delete_array( offset );
  CloseHandle(FH);
}

BOOL Ibitstream::get_header (uint32 *headerstring)
{
  BOOL result, sync;
  DWORD numread;

  do {

   // This part assumes idiotic Intel byte order
   result=(BOOL) ReadFile(FH, (char*)headerstring, 4, &numread, NULL);

   if (!(result && (numread==4)))
      return (FALSE);

   sync = ((*headerstring & 0xF8FF) == 0xF8FF);

   if (!sync)
      result = (BOOL) SetFilePointer(FH, -3, NULL, FILE_CURRENT);

  } while (!sync && result);

  if (!result)
     return (FALSE);

  register uint32 header = *headerstring;
  *headerstring = swap_int32 (header);

  if (last_frame_number < ++current_frame)
	last_frame_number = current_frame;

  return (TRUE);
}

BOOL Ibitstream::read_frame (uint32 bytesize)
{

	DWORD numread;
	BOOL result=ReadFile(FH, buffer, bytesize, &numread, NULL);

/*  if (bytesize > (bufferintsize << 2))
  {
	 cerr << "Internal error: framelength > bufferlength?!\n";
	 exit (1);
  } */

  wordpointer = buffer;
  bitindex = 0;
  framesize = bytesize;
// #ifdef DAMN_INTEL_BYTE_ORDER
  register uint32 *wordp, word;
  for (wordp = buffer + ((bytesize - 1) >> 2); wordp >= buffer; --wordp)
  {
	 word = *wordp;
	 *wordp = swap_int32 (word);
  }
// #endif

  return ((BOOL) (result && (numread==framesize)));
}

uint32 Ibitstream::get_bits (uint32 number_of_bits)
{
  static uint32 bitmask[17] =
  {
	 0,	// dummy
	 0x00000001, 0x00000003, 0x00000007, 0x0000000F,
	 0x0000001F, 0x0000003F, 0x0000007F, 0x000000FF,
	 0x000001FF, 0x000003FF, 0x000007FF, 0x00000FFF,
	 0x00001FFF, 0x00003FFF, 0x00007FFF, 0x0000FFFF
  };
  uint32 returnvalue;
  uint32 sum = bitindex + number_of_bits;

  if (sum <= 32)
  {
	 // all bits contained in *wordpointer
	 returnvalue = (*wordpointer >> (32 - sum)) & bitmask[number_of_bits];
	 if ((bitindex += number_of_bits) == 32)
	 {
		bitindex = 0;

/*		if ((char *)++wordpointer > (char *)buffer + framesize)
		{
	cerr << "Ibitstream::get_bits(): no more bits in buffer!\n";
	exit (1);
		} */
		wordpointer++; // added by me!
	 }
	 return returnvalue;
  }

  *((int16 *)&returnvalue + 1) = *(int16 *)wordpointer;

  wordpointer++; // Added by me!
  *(int16 *)&returnvalue = *((int16 *)wordpointer + 1);

  returnvalue >>= 48 - sum;	// returnvalue >>= 16 - (number_of_bits - (32 - bitindex))
  returnvalue &= bitmask[number_of_bits];
  bitindex = sum - 32;
  return returnvalue;
}


BOOL Ibitstream::get_side_info(uint32 channels,
										 III_side_info_t *side_info)
// Reads the side info from the stream, assuming the entire
// frame has been read already. Adopted from the public c code.

// Mono   : 136 bits (= 17 bytes)
// Stereo : 256 bits (= 32 bytes)

{
	int ch, gr;

	side_info->main_data_begin = get_bits(9);
	if (channels == 1)
		side_info->private_bits = get_bits(5);
	else side_info->private_bits = get_bits(3);

	for (ch=0; ch<channels; ch++) {
		side_info->ch[ch].scfsi[0] = get_bits(1);
		side_info->ch[ch].scfsi[1] = get_bits(1);
		side_info->ch[ch].scfsi[2] = get_bits(1);
		side_info->ch[ch].scfsi[3] = get_bits(1);
   }

	for (gr=0; gr<2; gr++) {
		for (ch=0; ch<channels; ch++) {
			side_info->ch[ch].gr[gr].part2_3_length = get_bits(12);
			side_info->ch[ch].gr[gr].big_values = get_bits(9);
			side_info->ch[ch].gr[gr].global_gain = get_bits(8);
			side_info->ch[ch].gr[gr].scalefac_compress = get_bits(4);
			side_info->ch[ch].gr[gr].window_switching_flag = get_bits(1);
			if (side_info->ch[ch].gr[gr].window_switching_flag) {
				side_info->ch[ch].gr[gr].block_type = get_bits(2);
				side_info->ch[ch].gr[gr].mixed_block_flag = get_bits(1);

				side_info->ch[ch].gr[gr].table_select[0] = get_bits(5);
				side_info->ch[ch].gr[gr].table_select[1] = get_bits(5);

				side_info->ch[ch].gr[gr].subblock_gain[0] = get_bits(3);
				side_info->ch[ch].gr[gr].subblock_gain[1] = get_bits(3);
				side_info->ch[ch].gr[gr].subblock_gain[2] = get_bits(3);

				/* Set region_count parameters since they are implicit in this case. */

				if (side_info->ch[ch].gr[gr].block_type == 0) {
/*					printf("Side info bad: block_type == 0 in split block.\n");
					exit(0); */
					return(FALSE);
				} else if (side_info->ch[ch].gr[gr].block_type == 2
							&& side_info->ch[ch].gr[gr].mixed_block_flag == 0)
					side_info->ch[ch].gr[gr].region0_count = 8; /* MI 9; */
				else side_info->ch[ch].gr[gr].region0_count = 7; /* MI 8; */
				side_info->ch[ch].gr[gr].region1_count = 20 -
						side_info->ch[ch].gr[gr].region0_count;
				}
			else {

				side_info->ch[ch].gr[gr].table_select[0] = get_bits(5);
				side_info->ch[ch].gr[gr].table_select[1] = get_bits(5);
				side_info->ch[ch].gr[gr].table_select[2] = get_bits(5);
				side_info->ch[ch].gr[gr].region0_count = get_bits(4);
				side_info->ch[ch].gr[gr].region1_count = get_bits(3);
				side_info->ch[ch].gr[gr].block_type = 0;
				}
			side_info->ch[ch].gr[gr].preflag = get_bits(1);
			side_info->ch[ch].gr[gr].scalefac_scale = get_bits(1);
			side_info->ch[ch].gr[gr].count1table_select = get_bits(1);
			}
		}
		return(TRUE);
}

DWORD Ibitstream::file_size()
{
	return (GetFileSize(FH, NULL));
}

DWORD Ibitstream::seek(int32 frame, int32 frame_size)
{
	current_frame = frame - 1;
	return (SetFilePointer(FH, frame * (frame_size + 4), NULL, FILE_BEGIN));
}

DWORD Ibitstream::seek_pad(int32 frame, int32 base_frame_size)
{
	// base_frame_size is the frame size _without_ padding.

	Header header;
	Crc16 *crc;

	int32 total_frame_size = base_frame_size + 4;
	int32 diff;

	if (last_frame_number < frame - 1) {
		diff = (last_frame_number >= 0) ?  offset[last_frame_number] : 0;
		SetFilePointer(FH, (last_frame_number + 1) * total_frame_size +
							diff, NULL, FILE_BEGIN);

		current_frame = last_frame_number;

		do {
			if (!header.read_header(this, &crc)) // will increment last_frame_number
				return(0L);
		} while (last_frame_number < frame -1);


	} else {
		diff = (frame > 0) ? offset[frame - 1] : 0;
		SetFilePointer(FH, frame * total_frame_size + diff, NULL, FILE_BEGIN);
		current_frame = frame - 1;
	}

	return (1L);
}


