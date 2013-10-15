/*
 *  @(#) crc.h 1.5, last edit: 6/15/94 16:55:32
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

#ifndef CRC_H
#define CRC_H
#include "all.h"

// 16-Bit CRC checksum class:
class Crc16
{
  static const uint16 polynomial;
  uint16 crc;

public:
		 Crc16 (void) { crc = 0xFFFF; }
  void		 add_bits (uint32 bitstring, uint32 length);
		 // feed a bitstring to the crc calculation (0 < length <= 32)
  uint16	checksum (void)
  // return the calculated checksum and erase it for next calls to add_bits()
  {
    register uint16 sum = crc;
    crc = 0xFFFF;
    return sum;
  }
};

#endif
