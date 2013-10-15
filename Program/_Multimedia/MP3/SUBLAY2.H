/*
 *  @(#) subband_layer_2.h 1.8, last edit: 6/15/94 16:51:59
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

#ifndef SUBBAND_LAYER_2_H
#define SUBBAND_LAYER_2_H

#include "all.h"
#include "subband.h"
#include "header.h"
#include "crc.h"


// class for layer II subbands in single channel mode:
class SubbandLayer2 : public Subband
{
protected:
  uint32 subbandnumber;

  uint32 allocation;
  uint32 scfsi;
  real	 scalefactor1, scalefactor2, scalefactor3;
  uint32 codelength;
  const real *groupingtable;
  real	 factor;

  uint32 groupnumber;
  uint32 samplenumber;
  real	 samples[3];
  real	 c, d;

  uint32 get_allocationlength (Header *);
  void	 prepare_sample_reading (Header *, uint32, const real **, real *,
				 uint32 *, real *, real *);

public:
		SubbandLayer2 (uint32 subbandnumber);
  void		read_allocation (Ibitstream *, Header *, Crc16 *);
  virtual void	read_scalefactor_selection (Ibitstream *, Crc16 *);
  void		read_scalefactor (Ibitstream *, Header *);
  BOOL		read_sampledata (Ibitstream *);
  BOOL		put_next_sample (e_channels, SynthesisFilter *, SynthesisFilter * = NULL);
};


// class for layer II subbands in joint stereo mode:
class SubbandLayer2IntensityStereo : public SubbandLayer2
{
protected:
  uint32 channel2_scfsi;
  real	 channel2_scalefactor1, channel2_scalefactor2, channel2_scalefactor3;

public:
       SubbandLayer2IntensityStereo (uint32 subbandnumber);
  void read_allocation (Ibitstream *stream, Header *header, Crc16 *crc)
  {
    SubbandLayer2::read_allocation (stream, header, crc);
  }
  void read_scalefactor_selection (Ibitstream *, Crc16 *);
  void read_scalefactor (Ibitstream *, Header *);
  BOOL read_sampledata (Ibitstream *stream)
  {
	 return SubbandLayer2::read_sampledata (stream);
  }
  BOOL put_next_sample (e_channels, SynthesisFilter *, SynthesisFilter *);
};


// class for layer II subbands in stereo mode:
class SubbandLayer2Stereo : public SubbandLayer2
{
protected:
  uint32 channel2_allocation;
  uint32 channel2_scfsi;
  real	 channel2_scalefactor1, channel2_scalefactor2, channel2_scalefactor3;
  BOOL	 channel2_grouping;
  uint32 channel2_codelength;
  const real *channel2_groupingtable;
  real	 channel2_factor;
  real	 channel2_samples[3];
  real	 channel2_c, channel2_d;

public:
       SubbandLayer2Stereo (uint32 subbandnumber);
  void read_allocation (Ibitstream *, Header *, Crc16 *);
  void read_scalefactor_selection (Ibitstream *, Crc16 *);
  void read_scalefactor (Ibitstream *, Header *);
  BOOL read_sampledata (Ibitstream *);
  BOOL put_next_sample (e_channels, SynthesisFilter *, SynthesisFilter *);
};

#endif
