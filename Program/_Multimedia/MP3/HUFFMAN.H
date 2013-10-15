/* huffman.h

	Adapted from the ISO MPEG Audio Subgroup Software Simulation
   Group's public c source for its MPEG audio decoder. Miscellaneous
   changes by Jeff Tsay (ctsay@pasteur.eecs.berkeley.edu).

   Last modified : 04/19/97 */

/**********************************************************************
Copyright (c) 1991 MPEG/audio software simulation group, All Rights Reserved
huffman.h
**********************************************************************/
/**********************************************************************
 * MPEG/audio coding/decoding software, work in progress              *
 *   NOT for public distribution until verified and approved by the   *
 *   MPEG/audio committee.  For further information, please contact   *
 *   Davis Pan, 508-493-2241, e-mail: pan@3d.enet.dec.com             *
 *                                                                    *
 * VERSION 4.1                                                        *
 *   changes made since last update:                                  *
 *   date   programmers                comment                        *
 *  27.2.92 F.O.Witte (ITT Intermetall)				                   *
 *  8/24/93 M. Iwadare          Changed for 1 pass decoding.          *
 *  7/14/94 J. Koller		useless 'typedef' before huffcodetab  		 *
 *				removed				      											 *
 *********************************************************************/

#ifndef HUFFMAN_H
#define HUFFMAN_H

#include "all.h"
#include "bit_res.h"

#define HUFFBITS uint32
#define HTN	34
#define MXOFF	250

struct huffcodetab {
  char tablename[3];			/* string, containing table_description   */
  uint32 xlen; 				/* max. x-index+			      	         */
  uint32 ylen;	            /* max. y-index+				               */
  uint32 linbits; 			/* number of linbits			               */
  uint32 linmax;				/* max number to be stored in linbits	   */
  int32 ref;					/* a positive value indicates a reference */
  HUFFBITS *table;			/* pointer to array[xlen][ylen]		      */
  unsigned char *hlen;		/* pointer to array[xlen][ylen]		      */
  unsigned char(*val)[2];	/* decoder tree				               */
  uint32 treelen;				/* length of decoder tree		            */
};

extern struct huffcodetab ht[HTN];

int32 huffman_decoder(struct huffcodetab *h, int32 *x, int32 *y, int32 *v,
                      int32 *w, Bit_Reserve *br);

#endif
