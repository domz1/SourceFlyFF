/*
 *  @(#) subband_layer_1.h 1.6, last edit: 6/15/94 16:51:58
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

#ifndef SUBBAND_LAYER_1_H
#define SUBBAND_LAYER_1_H

#include "all.h"
#include "subband.h"
#include "crc.h"


// class for layer I subbands in single channel mode:
class SubbandLayer1 : public Subband
{
protected:
  uint32 subbandnumber;
  uint32 samplenumber;
  uint32 allocation;
  real	 scalefactor;
  uint32 samplelength;
  real	 sample;
  real	 factor, offset;

public:
       SubbandLayer1 (uint32 subbandnumber);
  void read_allocation (Ibitstream *, Header *, Crc16 *);
  void read_scalefactor (Ibitstream *, Header *);
  BOOL read_sampledata (Ibitstream *);
  BOOL put_next_sample (e_channels, SynthesisFilter *, SynthesisFilter *);
};


// class for layer I subbands in joint stereo mode:
class SubbandLayer1IntensityStereo : public SubbandLayer1
{
protected:
  real channel2_scalefactor;

public:
       SubbandLayer1IntensityStereo (uint32 subbandnumber);
  void read_allocation (Ibitstream *stream, Header *header, Crc16 *crc)
  {
    SubbandLayer1::read_allocation (stream, header, crc);
  }
  void read_scalefactor (Ibitstream *, Header *);
  BOOL read_sampledata (Ibitstream *stream)
  {
	 return SubbandLayer1::read_sampledata (stream);
  }
  BOOL put_next_sample (e_channels, SynthesisFilter *, SynthesisFilter *);
};


// class for layer I subbands in stereo mode:
class SubbandLayer1Stereo : public SubbandLayer1
{
protected:
  uint32 channel2_allocation;
  real	 channel2_scalefactor;
  uint32 channel2_samplelength;
  real	 channel2_sample;
  real	 channel2_factor, channel2_offset;

public:
       SubbandLayer1Stereo (uint32 subbandnumber);
  void read_allocation (Ibitstream *, Header *, Crc16 *);
  void read_scalefactor (Ibitstream *, Header *);
  BOOL read_sampledata (Ibitstream *);
  BOOL put_next_sample (e_channels, SynthesisFilter *, SynthesisFilter *);
};

#endif
