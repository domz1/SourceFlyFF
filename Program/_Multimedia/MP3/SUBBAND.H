/*
 *  @(#) subband.h 1.6, last edit: 6/15/94 16:55:36
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

#ifndef SUBBAND_H
#define SUBBAND_H

#include "all.h"
#include "ibitstr.h"
#include "header.h"
#include "synfilt.h"
#include "crc.h"

enum e_channels { both, left, right };

// abstract base class for subband classes of layer I and II:
class Subband
{
public:
  virtual void read_allocation (Ibitstream *, Header *, Crc16 *) = 0;
  virtual void read_scalefactor (Ibitstream *, Header *) = 0;
  virtual BOOL read_sampledata (Ibitstream *) = 0;
  virtual BOOL put_next_sample (e_channels, SynthesisFilter *, SynthesisFilter *) = 0;
};

#endif
