/*
 *  @(#) obuffer.h 1.8, last edit: 6/15/94 16:51:56
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

// MCIbuffer class added by Jeff Tsay. Last edit 3/29/96

#ifndef OBUFFER_H
#define OBUFFER_H

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
#include "header.h"

static const uint32 OBUFFERSIZE = 2 * 1152;	// max. 2 * 1152 samples per frame
static const uint32 MAXCHANNELS = 2;		// max. number of channels

// Abstract base class for audio output classes:
class Obuffer
{
public:
  virtual     ~Obuffer (void) {}		// dummy


  virtual void append (uint32 channel, int32 value) = 0;
			 // this function takes a 16 Bit PCM sample
  virtual void write_buffer (int fd) = 0;
			 // this function should write the samples to the filedescriptor
			 // or directly to the audio hardware

  virtual void clear_buffer(void) = 0;
			 // Clears all data in the buffer (for seeking)

  virtual void set_stop_flag(void) = 0;

};

class MCIbuffer : public Obuffer
{
private:
  uint32 bufferp[MAXCHANNELS];
  uint32 channels;
  tWAVEFORMATEX *lpwf;
  LPWAVEHDR* lpwavehdr_arr;
  HWAVEOUT *phwo;
  uint32 buffer_count;
  uint32 hdr_size;
  uint32 fillup;
  DWORD data_size;
  uint32 user_stop;

public:
	MCIbuffer (uint32 number_of_channels, Header *header, HWAVEOUT *phwo0);
	~MCIbuffer (void);

  void	append (uint32 channel, int32 value);
  void	write_buffer (int fd);
  void   clear_buffer(void);
  void   set_stop_flag(void);

private:
  void wave_swap(void);
};

#endif
