/*
 *  @(#) subband_layer_1.cc 1.7, last edit: 6/15/94 16:51:49
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
 *    - scalefactors itself instead of scalefactor indices are stored in
 *      SubbandLayer1... objects
 *    - check for small values in [-1.0E-7, 1.0E-7] removed, because the
 *      test itself was slower than some SynthesisFilter::input_sample() calls
 *    - check for illegal scalefactor index 63 removed
 */

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

#include "sublay1.h"
#include "scalfact.h"


// factors and offsets for sample requantization:
static const real table_factor[15] = {
  0.0, (1.0/2.0) * (4.0/3.0), (1.0/4.0) * (8.0/7.0), (1.0/8.0) * (16.0/15.0),
  (1.0/16.0) * (32.0/31.0), (1.0/32.0) * (64.0/63.0), (1.0/64.0) * (128.0/127.0),
  (1.0/128.0) * (256.0/255.0), (1.0/256.0) * (512.0/511.0),
  (1.0/512.0) * (1024.0/1023.0), (1.0/1024.0) * (2048.0/2047.0),
  (1.0/2048.0) * (4096.0/4095.0), (1.0/4096.0) * (8192.0/8191.0),
  (1.0/8192.0) * (16384.0/16383.0), (1.0/16384.0) * (32768.0/32767.0)
};

static const real table_offset[15] = {
  0.0, ((1.0/2.0)-1.0) * (4.0/3.0), ((1.0/4.0)-1.0) * (8.0/7.0), ((1.0/8.0)-1.0) * (16.0/15.0),
  ((1.0/16.0)-1.0) * (32.0/31.0), ((1.0/32.0)-1.0) * (64.0/63.0), ((1.0/64.0)-1.0) * (128.0/127.0),
  ((1.0/128.0)-1.0) * (256.0/255.0), ((1.0/256.0)-1.0) * (512.0/511.0),
  ((1.0/512.0)-1.0) * (1024.0/1023.0), ((1.0/1024.0)-1.0) * (2048.0/2047.0),
  ((1.0/2048.0)-1.0) * (4096.0/4095.0), ((1.0/4096.0)-1.0) * (8192.0/8191.0),
  ((1.0/8192.0)-1.0) * (16384.0/16383.0), ((1.0/16384.0)-1.0) * (32768.0/32767.0)
};



/**********************/	// used for single channel mode
/*** Standard Class ***/	// and in derived class for intensity
/**********************/	// stereo mode

SubbandLayer1::SubbandLayer1 (uint32 subbandnumber)
{
  this->subbandnumber = subbandnumber;
  samplenumber = 0;
}


void SubbandLayer1::read_allocation (Ibitstream *stream, Header *, Crc16 *crc)
{
//  if ((allocation = stream->get_bits (4)) == 15) ;
//	 cerr << "WARNING: stream contains an illegal allocation!\n";	// MPEG-stream is corrupted!

  if (crc)
	 crc->add_bits (allocation, 4);
  if (allocation)
  {
	 samplelength = allocation + 1;
	 factor = table_factor[allocation];
    offset = table_offset[allocation];
  }
}


void SubbandLayer1::read_scalefactor (Ibitstream *stream, Header *)
{
  if (allocation)
	 scalefactor = scalefactors[stream->get_bits (6)];
}


BOOL SubbandLayer1::read_sampledata (Ibitstream *stream)
{
  if (allocation)
  {
	 sample = real (stream->get_bits (samplelength));
  }
  if (++samplenumber == 12)
  {
	 samplenumber = 0;
	 return TRUE;
  }
  return FALSE;
}


BOOL SubbandLayer1::put_next_sample (e_channels channels,
					  SynthesisFilter *filter1, SynthesisFilter *)
{
  if (allocation && channels != right)
  {
	 register real scaled_sample = (sample * factor + offset) * scalefactor;
	 filter1->input_sample (scaled_sample, subbandnumber);
  }
  return TRUE;
}


/******************************/
/*** Intensity Stereo Class ***/
/******************************/

SubbandLayer1IntensityStereo::SubbandLayer1IntensityStereo (uint32 subbandnumber)
: SubbandLayer1 (subbandnumber)
{
}


void SubbandLayer1IntensityStereo::read_scalefactor (Ibitstream *stream, Header *)
{
  if (allocation)
  {
	 scalefactor = scalefactors[stream->get_bits (6)];
	 channel2_scalefactor = scalefactors[stream->get_bits (6)];
  }
}


BOOL SubbandLayer1IntensityStereo::put_next_sample (e_channels channels,
	SynthesisFilter *filter1, SynthesisFilter *filter2)
{
  if (allocation)
  {
    sample = sample * factor + offset;		// requantization
	 if (channels == both)
    {
		register real sample1 = sample * scalefactor,
			 sample2 = sample * channel2_scalefactor;
		filter1->input_sample (sample1, subbandnumber);
		filter2->input_sample (sample2, subbandnumber);
	 }
	 else if (channels == left)
	 {
		register real sample1 = sample * scalefactor;
		filter1->input_sample (sample1, subbandnumber);
	 }
	 else
	 {
		register real sample2 = sample * channel2_scalefactor;
		filter1->input_sample (sample2, subbandnumber);
	 }
  }
  return TRUE;
}



/********************/
/*** Stereo Class ***/
/********************/

SubbandLayer1Stereo::SubbandLayer1Stereo (uint32 subbandnumber)
: SubbandLayer1 (subbandnumber)
{
}


void SubbandLayer1Stereo::read_allocation (Ibitstream *stream, Header *, Crc16 *crc)
{
  allocation = stream->get_bits (4);
  channel2_allocation = stream->get_bits (4);
  if (crc)
  {
	 crc->add_bits (allocation, 4);
    crc->add_bits (channel2_allocation, 4);
  }
  if (allocation)
  {
	 samplelength = allocation + 1;
    factor = table_factor[allocation];
    offset = table_offset[allocation];
  }
  if (channel2_allocation)
  {
    channel2_samplelength = channel2_allocation + 1;
	 channel2_factor = table_factor[channel2_allocation];
	 channel2_offset = table_offset[channel2_allocation];
  }
}


void SubbandLayer1Stereo::read_scalefactor (Ibitstream *stream, Header *)
{
  if (allocation)
	 scalefactor = scalefactors[stream->get_bits (6)];
  if (channel2_allocation)
	 channel2_scalefactor = scalefactors[stream->get_bits (6)];
}


BOOL SubbandLayer1Stereo::read_sampledata (Ibitstream *stream)
{
  BOOL returnvalue = SubbandLayer1::read_sampledata (stream);
  if (channel2_allocation)
  {
	 channel2_sample = real (stream->get_bits (channel2_samplelength));
  }
  return returnvalue;
}


BOOL SubbandLayer1Stereo::put_next_sample (e_channels channels,
						SynthesisFilter *filter1, SynthesisFilter *filter2)
{
  SubbandLayer1::put_next_sample (channels, filter1, filter2);
  if (channel2_allocation && channels != left)
  {
	 register float sample2 = (channel2_sample * channel2_factor + channel2_offset) *
				  channel2_scalefactor;
	 if (channels == both)
		filter2->input_sample (sample2, subbandnumber);
	 else
		filter1->input_sample (sample2, subbandnumber);
  }
  return TRUE;
}
