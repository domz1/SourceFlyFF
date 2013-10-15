/*
 *  @(#) crc.cc 1.5, last edit: 6/15/94 16:55:30
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
#define STRICT
#define WIN32_LEAN_AND_MEAN
#define INC_OLE2
#define NOSERVICE
#define NOMCX     
#define NOIME

#include "stdafx.h"
#include "crc.h"


// generator polinomial:
const uint16 Crc16::polynomial = 0x8005;


void Crc16::add_bits (uint32 bitstring, uint32 length)
{
  uint32 bitmask = 1 << (length - 1);
  do
	 if (!(crc & 0x8000) ^ !(bitstring & bitmask))
	 {
		crc <<= 1;
		crc ^= polynomial;
	 }
	 else
		crc <<= 1;
  while (bitmask >>= 1);
}
