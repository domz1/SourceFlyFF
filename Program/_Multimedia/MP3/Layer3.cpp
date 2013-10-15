/*
Layer III routines adopted from the ISO MPEG Audio Subgroup Software Simulation
Group's public c source for its MPEG audio decoder. These routines were in the
file "decoder.c". Rearrangement of the routines as member functions of a
layer III decoder object, and optimizations by Jeff Tsay
(ctsay@pasteur.eecs.berkeley.edu).

If you want to help, figure out how to implement a 9 point IDCT
in n log n time, and e-mail me.

Last modified : 01/31/97 */

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

#include "stdafx.h"
#include <math.h>

#include "all.h"
#include "l3type.h"
#include "ibitstr.h"
#include "obuffer.h"
#include "bit_res.h"
#include "header.h"
#include "synfilt.h"
#include "huffman.h"
#include "layer3.h"

LayerIII_Decoder::LayerIII_Decoder(char *huffdec_path0,
 									        Ibitstream *stream0,
                                   Header *header0,
                                   SynthesisFilter *filtera,
                                   SynthesisFilter *filterb,
                 						  Obuffer *buffer0)
{
  huffdec_path = huffdec_path0;
  stream = stream0;
  header = header0;
  filter1 = filtera;
  filter2 = filterb;
  buffer = buffer0;

  frame_start = 0;
  channels = (header->mode() == single_channel) ? 1 : 2;

  int32 i,j,k;

  for(i=0;i<2;i++)
  		for(j=0;j<SBLIMIT;j++)
  			for(k=0;k<SSLIMIT;k++)
  				prevblck[i][j][k]=0.0f;

  br = new Bit_Reserve();
  si = new III_side_info_t;
}

LayerIII_Decoder::~LayerIII_Decoder()
{
   safe_delete( br );
   safe_delete( si );
}

void LayerIII_Decoder::seek_notify()
{
	frame_start = 0;

   int32 i,j,k;

  	for(i=0;i<2;i++)
  		for(j=0;j<SBLIMIT;j++)
  			for(k=0;k<SSLIMIT;k++)
  				prevblck[i][j][k]=0.0f;

   safe_delete( br );
   br = new Bit_Reserve;
}


struct {
	int32 l[5];
	int32 s[3];} sfbtable = {{0, 6, 11, 16, 21},
								    {0, 6, 12}};

int32 slen[2][16] = {{0, 0, 0, 0, 3, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4},
                     {0, 1, 2, 3, 0, 1, 2, 3, 1, 2, 3, 1, 2, 3, 2, 3}};

void LayerIII_Decoder::get_scale_factors(int32 ch, int32 gr)
{
	int32 sfb, window;
	gr_info_s *gr_info = &(si->ch[ch].gr[gr]);
   int32 scale_comp = gr_info->scalefac_compress;
   int32 length0 = slen[0][scale_comp];
   int32 length1 = slen[1][scale_comp];

	 if (gr_info->window_switching_flag && (gr_info->block_type == 2)) {
		if (gr_info->mixed_block_flag) { /* MIXED */ /* NEW - ag 11/25 */
			for (sfb = 0; sfb < 8; sfb++)
				scalefac[ch].l[sfb] = br->hgetbits(
					  slen[0][gr_info->scalefac_compress]);
			for (sfb = 3; sfb < 6; sfb++)
				for (window=0; window<3; window++)
					scalefac[ch].s[window][sfb] = br->hgetbits(
					  slen[0][gr_info->scalefac_compress]);
			for (sfb = 6; sfb < 12; sfb++)
				for (window=0; window<3; window++)
					scalefac[ch].s[window][sfb] = br->hgetbits(
					  slen[1][gr_info->scalefac_compress]);
			for (sfb=12,window=0; window<3; window++)
				scalefac[ch].s[window][sfb] = 0;
		}
		else {  /* SHORT*/

         scalefac[ch].s[0][0] = br->hgetbits(length0);
         scalefac[ch].s[1][0] = br->hgetbits(length0);
         scalefac[ch].s[2][0] = br->hgetbits(length0);
         scalefac[ch].s[0][1] = br->hgetbits(length0);
         scalefac[ch].s[1][1] = br->hgetbits(length0);
         scalefac[ch].s[2][1] = br->hgetbits(length0);
         scalefac[ch].s[0][2] = br->hgetbits(length0);
         scalefac[ch].s[1][2] = br->hgetbits(length0);
         scalefac[ch].s[2][2] = br->hgetbits(length0);
         scalefac[ch].s[0][3] = br->hgetbits(length0);
         scalefac[ch].s[1][3] = br->hgetbits(length0);
         scalefac[ch].s[2][3] = br->hgetbits(length0);
         scalefac[ch].s[0][4] = br->hgetbits(length0);
         scalefac[ch].s[1][4] = br->hgetbits(length0);
         scalefac[ch].s[2][4] = br->hgetbits(length0);
         scalefac[ch].s[0][5] = br->hgetbits(length0);
         scalefac[ch].s[1][5] = br->hgetbits(length0);
         scalefac[ch].s[2][5] = br->hgetbits(length0);
         scalefac[ch].s[0][6] = br->hgetbits(length1);
         scalefac[ch].s[1][6] = br->hgetbits(length1);
         scalefac[ch].s[2][6] = br->hgetbits(length1);
         scalefac[ch].s[0][7] = br->hgetbits(length1);
         scalefac[ch].s[1][7] = br->hgetbits(length1);
         scalefac[ch].s[2][7] = br->hgetbits(length1);
         scalefac[ch].s[0][8] = br->hgetbits(length1);
         scalefac[ch].s[1][8] = br->hgetbits(length1);
         scalefac[ch].s[2][8] = br->hgetbits(length1);
         scalefac[ch].s[0][9] = br->hgetbits(length1);
         scalefac[ch].s[1][9] = br->hgetbits(length1);
         scalefac[ch].s[2][9] = br->hgetbits(length1);
         scalefac[ch].s[0][10] = br->hgetbits(length1);
         scalefac[ch].s[1][10] = br->hgetbits(length1);
         scalefac[ch].s[2][10] = br->hgetbits(length1);
         scalefac[ch].s[0][11] = br->hgetbits(length1);
         scalefac[ch].s[1][11] = br->hgetbits(length1);
         scalefac[ch].s[2][11] = br->hgetbits(length1);
         scalefac[ch].s[0][12] = 0;
			scalefac[ch].s[1][12] = 0;
			scalefac[ch].s[2][12] = 0;

		}
	 }
	 else {   /* LONG types 0,1,3 */

        if ((si->ch[ch].scfsi[0] == 0) || (gr == 0)) {
           scalefac[ch].l[0] = br->hgetbits(length0);
           scalefac[ch].l[1] = br->hgetbits(length0);
           scalefac[ch].l[2] = br->hgetbits(length0);
           scalefac[ch].l[3] = br->hgetbits(length0);
           scalefac[ch].l[4] = br->hgetbits(length0);
           scalefac[ch].l[5] = br->hgetbits(length0);
		  }
        if ((si->ch[ch].scfsi[1] == 0) || (gr == 0)) {
           scalefac[ch].l[6] = br->hgetbits(length0);
           scalefac[ch].l[7] = br->hgetbits(length0);
           scalefac[ch].l[8] = br->hgetbits(length0);
           scalefac[ch].l[9] = br->hgetbits(length0);
           scalefac[ch].l[10] = br->hgetbits(length0);
		  }
        if ((si->ch[ch].scfsi[2] == 0) || (gr == 0)) {
           scalefac[ch].l[11] = br->hgetbits(length1);
           scalefac[ch].l[12] = br->hgetbits(length1);
           scalefac[ch].l[13] = br->hgetbits(length1);
           scalefac[ch].l[14] = br->hgetbits(length1);
           scalefac[ch].l[15] = br->hgetbits(length1);
		  }
        if ((si->ch[ch].scfsi[3] == 0) || (gr == 0)) {
           scalefac[ch].l[16] = br->hgetbits(length1);
           scalefac[ch].l[17] = br->hgetbits(length1);
           scalefac[ch].l[18] = br->hgetbits(length1);
           scalefac[ch].l[19] = br->hgetbits(length1);
           scalefac[ch].l[20] = br->hgetbits(length1);
		  }

        scalefac[ch].l[21] = 0;
		  scalefac[ch].l[22] = 0;
	 }
}

struct  {
	int32 l[23];
	int32 s[14];} sfBandIndex[3] =
	{{{0,4,8,12,16,20,24,30,36,44,52,62,74,90,110,134,162,196,238,288,342,418,576},
	  {0,4,8,12,16,22,30,40,52,66,84,106,136,192}},
	 {{0,4,8,12,16,20,24,30,36,42,50,60,72,88,106,128,156,190,230,276,330,384,576},
	  {0,4,8,12,16,22,28,38,50,64,80,100,126,192}},
	 {{0,4,8,12,16,20,24,30,36,44,54,66,82,102,126,156,194,240,296,364,448,550,576},
	  {0,4,8,12,16,22,30,42,58,78,104,138,180,192}}};

void LayerIII_Decoder::hufman_decode(int32 ch, int32 gr)
{
	int32 i, x, y;
	int32 v, w;
   int32 part2_3_end = part2_start + si->ch[ch].gr[gr].part2_3_length;
   int32 num_bits;
	int32 region1Start;
	int32 region2Start;
   int32 sf_index;
   int32 ssindex, sbindex;
 	struct huffcodetab *h;

   static BOOL huffman_init = FALSE;

/*   if (!huffman_init) {
		if (read_decoder_table(huffdec_path))
          huffman_init = TRUE;
      else
          ExitThread(1L);
   } */

   /* Initialize output */
   /* Eliminates need to zero out the rest */

   for (x=0;x<SBLIMIT;x++){
   	is[x][0]  = 0; is[x][1]  = 0; is[x][2]  = 0;
     	is[x][3]  = 0; is[x][4]  = 0; is[x][5]  = 0;
   	is[x][6]  = 0; is[x][7]  = 0; is[x][8]  = 0;
   	is[x][9]  = 0; is[x][10] = 0; is[x][11] = 0;
   	is[x][12] = 0; is[x][13] = 0; is[x][14] = 0;
		is[x][15] = 0; is[x][16] = 0; is[x][17] = 0;
   }

	/* Find region boundary for short block case. */

	if ( (si->ch[ch].gr[gr].window_switching_flag) &&
		  (si->ch[ch].gr[gr].block_type == 2) ) {

		/* Region2. */

		region1Start = 36;  /* sfb[9/3]*3=36 */
		region2Start = 576; /* No Region2 for short block case. */
	}

	else {          /* Find region boundary for long block case. */

   	sf_index = header->sample_frequency();

		region1Start = sfBandIndex[sf_index].l[si->ch[ch].gr[gr].region0_count
                                             + 1]; /* MI */
		region2Start = sfBandIndex[sf_index].l[si->ch[ch].gr[gr].region0_count +
										   si->ch[ch].gr[gr].region1_count + 2]; /* MI */
		}

   sbindex = 0; ssindex = 0;
	/* Read bigvalues area. */
	for (i=0; i<(si->ch[ch].gr[gr].big_values<<1); i+=2) {
		if      (i<region1Start) h = &ht[si->ch[ch].gr[gr].table_select[0]];
		else if (i<region2Start) h = &ht[si->ch[ch].gr[gr].table_select[1]];
			  else                h = &ht[si->ch[ch].gr[gr].table_select[2]];

		huffman_decoder(h, &x, &y, &v, &w, br);

      is[sbindex][ssindex]   = x;
      is[sbindex][ssindex+1] = y;

      ssindex += 2;
      if (ssindex >= SSLIMIT) {
      	ssindex = 0;
         sbindex++;
      }
	}

	/* Read count1 area. */
	h = &ht[(*si).ch[ch].gr[gr].count1table_select+32];
   num_bits = br->hsstell();

	while ((num_bits < part2_3_end) && (sbindex < SBLIMIT)) {

		huffman_decoder(h, &x, &y, &v, &w, br);
      is[sbindex][ssindex] = v;
      is[sbindex][ssindex + 1] = w;

      ssindex += 2;
      if (ssindex >= SSLIMIT) {
      	ssindex = 0;
         sbindex++;
      }

		if (sbindex < SBLIMIT) {
			is[sbindex][ssindex]   = x;
			is[sbindex][ssindex+1] = y;
      }

      ssindex += 2;
      if (ssindex >= SSLIMIT) {
      	ssindex = 0;
         sbindex++;
      }
      num_bits = br->hsstell();
   }

	if (num_bits > part2_3_end)
		br->rewindNbits(num_bits - part2_3_end);

   num_bits = br->hsstell();

	/* Dismiss stuffing Bits */
	if (num_bits < part2_3_end)
   	br->hgetbits(part2_3_end - num_bits);

	/* Zero out rest. */
/*	for (; i<SSLIMIT*SBLIMIT; i++)
		is[i/SSLIMIT][i%SSLIMIT] = 0; */
}

int pretab[22] = {0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,2,2,3,3,3,2,0};

real LayerIII_Decoder::two_pow2(int32 scale, int32 preflag,
                                int32 pretab_offset, int32 l)
{
	int32 index;
	static real two_to_negative_half_pow[40];
   static BOOL two_pow2_init = FALSE;

   if (!two_pow2_init) {
   	int32 i;

      for (i=0;i<40;i++)
      	two_to_negative_half_pow[i] = (real) pow(2.0, -0.5 * (double) i);

      two_pow2_init = TRUE;
   }

   index = l;

   if (preflag)
   	index += pretab_offset;

   index = index << scale;

   return(two_to_negative_half_pow[index]);
}

void LayerIII_Decoder::dequantize_sample(real xr[SBLIMIT][SSLIMIT],
												     int32 ch, int32 gr)
{
	gr_info_s *gr_info = &(si->ch[ch].gr[gr]);

	int32 ss, sb, cb=0, sfreq=header->sample_frequency();
	int32 next_cb_boundary, cb_begin, cb_width;
   real temp;
	int32 index=0, t_index;
   int32 i;
   static BOOL Dequant_init=FALSE;
   static real TO_FOUR_THIRDS[1024];

   if (!Dequant_init) {

   	for(i = 0; i<1024; i++)
	  		TO_FOUR_THIRDS[i] = (real) pow( (double) i, 4.0/3.0);

      Dequant_init = TRUE;
   }

	/* choose correct scalefactor band per block type, initalize boundary */

	if (gr_info->window_switching_flag && (gr_info->block_type == 2) )
		if (gr_info->mixed_block_flag)
			next_cb_boundary=sfBandIndex[sfreq].l[1];  /* LONG blocks: 0,1,3 */
		else {
         cb_width = sfBandIndex[sfreq].s[1];
		   next_cb_boundary= (cb_width << 2) - cb_width;
	 	   cb_begin = 0;
		}
	else
		next_cb_boundary=sfBandIndex[sfreq].l[1];  /* LONG blocks: 0,1,3 */

   /* Compute overall (global) scaling. */

	temp = (real) pow( 2.0 , (0.25 * (gr_info->global_gain - 210.0)));

   for (sb=0; sb < SBLIMIT; sb++)
   	for(ss=0; ss < SSLIMIT; ss+=2) {
			xr[sb][ss]   =  temp * TO_FOUR_THIRDS[abs(is[sb][ss])];
         xr[sb][ss+1] =  temp * TO_FOUR_THIRDS[abs(is[sb][ss+1])];
			if (is[sb][ss]<0)   xr[sb][ss]   = -xr[sb][ss];
         if (is[sb][ss+1]<0) xr[sb][ss+1] = -xr[sb][ss+1];
      }

	/* apply formula per block type */

	for (sb=0 ; sb < SBLIMIT ; sb++) {

		for (ss=0 ; ss < SSLIMIT ; ss++) {

			if (index == next_cb_boundary)  { /* Adjust critical band boundary */
				if (gr_info->window_switching_flag && (gr_info->block_type == 2)) {
					if (gr_info->mixed_block_flag)  {

						if (index == sfBandIndex[sfreq].l[8])  {
                     next_cb_boundary = sfBandIndex[sfreq].s[4];
                     next_cb_boundary = (next_cb_boundary << 2) -
                     			           next_cb_boundary;
							cb = 3;
						   cb_width = sfBandIndex[sfreq].s[4] -
                     			  sfBandIndex[sfreq].s[3];

					      cb_begin = sfBandIndex[sfreq].s[3];
                     cb_begin = (cb_begin << 2) - cb_begin;
						}

						else if (index < sfBandIndex[sfreq].l[8])
							next_cb_boundary = sfBandIndex[sfreq].l[(++cb)+1];
						else {
                  	next_cb_boundary = sfBandIndex[sfreq].s[(++cb)+1];
                     next_cb_boundary = (next_cb_boundary << 2) -
                     				        next_cb_boundary;

                     cb_begin = sfBandIndex[sfreq].s[cb];
							cb_width = sfBandIndex[sfreq].s[cb+1] -
                                cb_begin;
                     cb_begin = (cb_begin << 2) - cb_begin;
						}
					}
					else  {
                  next_cb_boundary = sfBandIndex[sfreq].s[(++cb)+1];
                  next_cb_boundary = (next_cb_boundary << 2) -
                                      next_cb_boundary;

                  cb_begin = sfBandIndex[sfreq].s[cb];
						cb_width = sfBandIndex[sfreq].s[cb+1] -
                               cb_begin;
                  cb_begin = (cb_begin << 2) - cb_begin;
					}
				}
				else /* long blocks */
					next_cb_boundary = sfBandIndex[sfreq].l[(++cb)+1];
			}

			/* Do long/short dependent scaling operations. */

			if (gr_info->window_switching_flag &&
				 (((gr_info->block_type == 2) && (gr_info->mixed_block_flag == 0)) ||
				  ((gr_info->block_type == 2) && gr_info->mixed_block_flag && (sb >= 2)) ))
         {

				t_index = (index - cb_begin) / cb_width;
            xr[sb][ss] *= pow(2.0, ((-2.0 * gr_info->subblock_gain[t_index])
                                    -(0.5 * (1.0 + gr_info->scalefac_scale)
                                      * scalefac[ch].s[t_index][cb])));
			}
			else {   /* LONG block types 0,1,3 & 1st 2 subbands of switched blocks */
/*				xr[sb][ss] *= pow(2.0, -0.5 * (1.0+gr_info->scalefac_scale)
													 * (scalefac[ch].l[cb]
													 + gr_info->preflag * pretab[cb])); */
				xr[sb][ss] *= two_pow2(gr_info->scalefac_scale, gr_info->preflag,
                                   pretab[cb], scalefac[ch].l[cb]);
			}
         index++;
		}
   }

}

void LayerIII_Decoder::reorder (real xr[SBLIMIT][SSLIMIT], int32 ch, int32 gr)
{

	gr_info_s *gr_info = &(si->ch[ch].gr[gr]);
	int32 sfreq=header->sample_frequency();
	int32 sfb, sfb_start, sfb_lines;
	int32 sb, ss, freq, src_line, des_line;

	if (gr_info->window_switching_flag && (gr_info->block_type == 2)) {

		for(sb=0;sb<SBLIMIT;sb++)
	      re_hybridOut[sb][0]  = re_hybridOut[sb][1]  = re_hybridOut[sb][2]  =
         re_hybridOut[sb][3]  = re_hybridOut[sb][4]  = re_hybridOut[sb][5]  =
         re_hybridOut[sb][6]  = re_hybridOut[sb][7]  = re_hybridOut[sb][8]  =
         re_hybridOut[sb][9]  = re_hybridOut[sb][10] = re_hybridOut[sb][11] =
       	re_hybridOut[sb][12] = re_hybridOut[sb][13] = re_hybridOut[sb][14] =
         re_hybridOut[sb][15] = re_hybridOut[sb][16] = re_hybridOut[sb][17] =
         0.0f;

		if (gr_info->mixed_block_flag) {
			/* NO REORDER FOR LOW 2 SUBBANDS */
			   for (ss=0 ; ss < SSLIMIT ; ss+=3) {
					re_hybridOut[0][ss]   = xr[0][ss];
               re_hybridOut[0][ss+1] = xr[0][ss+1];
               re_hybridOut[0][ss+2] = xr[0][ss+2];
            }

            for (ss=0; ss < SSLIMIT ; ss+=3) {
            	re_hybridOut[1][ss]   = xr[1][ss];
               re_hybridOut[1][ss+1] = xr[1][ss+1];
               re_hybridOut[1][ss+2] = xr[1][ss+2];
            }

			/* REORDERING FOR REST SWITCHED SHORT */
				for(sfb=3,sfb_start=sfBandIndex[sfreq].s[3],
	  				 sfb_lines=sfBandIndex[sfreq].s[4] - sfb_start;
	  				 sfb < 13; sfb++,sfb_start=sfBandIndex[sfreq].s[sfb],
				    (sfb_lines=sfBandIndex[sfreq].s[sfb+1] - sfb_start))
            {
					   int32 sfb_start3 = (sfb_start << 2) - sfb_start;

						for(freq=0;freq<sfb_lines;freq++) {
                     int32 freq3 = (freq << 2) - freq;

							src_line = sfb_start3 + freq;
							des_line = sfb_start3 + freq3;
							re_hybridOut[des_line/SSLIMIT][des_line%SSLIMIT] =
												xr[src_line/SSLIMIT][src_line%SSLIMIT];
							src_line += sfb_lines;
							des_line++;
							re_hybridOut[des_line/SSLIMIT][des_line%SSLIMIT] =
												xr[src_line/SSLIMIT][src_line%SSLIMIT];
							src_line += sfb_lines;
							des_line++;
							re_hybridOut[des_line/SSLIMIT][des_line%SSLIMIT] =
												xr[src_line/SSLIMIT][src_line%SSLIMIT];
					   }
            }
		}
		else {  /* pure short */
			for(sfb=0,sfb_start=0,sfb_lines=sfBandIndex[sfreq].s[1];
				sfb < 13; sfb++,sfb_start=sfBandIndex[sfreq].s[sfb],
				(sfb_lines=sfBandIndex[sfreq].s[sfb+1] - sfb_start))
            {
					   int32 sfb_start3 = (sfb_start << 2) - sfb_start;

						for(freq=0;freq<sfb_lines;freq++) {
                     int32 freq3 = (freq << 2) - freq;

							src_line = sfb_start3 + freq;
							des_line = sfb_start3 + freq3;
							re_hybridOut[des_line/SSLIMIT][des_line%SSLIMIT] =
												xr[src_line/SSLIMIT][src_line%SSLIMIT];
							src_line += sfb_lines;
							des_line++;
							re_hybridOut[des_line/SSLIMIT][des_line%SSLIMIT] =
												xr[src_line/SSLIMIT][src_line%SSLIMIT];
							src_line += sfb_lines;
							des_line++;
							re_hybridOut[des_line/SSLIMIT][des_line%SSLIMIT] =
												xr[src_line/SSLIMIT][src_line%SSLIMIT];
					   }
            }
		}
	}
	else {   /*long blocks */
		for (sb=0 ; sb < SBLIMIT ; sb++)
         for (ss = 0; ss < SSLIMIT; ss+=3) {
         	re_hybridOut[sb][ss]   = xr[sb][ss];
            re_hybridOut[sb][ss+1] = xr[sb][ss+1];
            re_hybridOut[sb][ss+2] = xr[sb][ss+2];
         }
	}
}

#define PI   3.141593
#define PI12 0.2617994
#define PI18 0.17453293
#define PI24 0.1308997
#define PI36 0.08726646
#define PI72 0.04363323

real TAN12[16]={0.0, 0.26794919, 0.57735027, 1.0,
                1.73205081, 3.73205081, 9.9999999e10 /*unbounded*/, -3.73205081,
                -1.73205081, -1.0, -0.57735027, -0.26794919,
                0.0, 0.26794919, 0.57735027, 1.0};

void LayerIII_Decoder::stereo(int32 gr)
{
	gr_info_s *gr_info = &(si->ch[0].gr[gr]);
	int32 sfreq = header->sample_frequency();
   int32 mode_ext = header->mode_extension();

	int32 ms_stereo = (header->mode() == joint_stereo) && (mode_ext & 0x2);
	int32 i_stereo  = (header->mode() == joint_stereo) && (mode_ext & 0x1);
	int32 sfb;
	int32 i,j,sb,ss,is_pos[576];
   int32 lines, temp, temp2;
	real is_ratio[576];

	if  (channels == 1) { /* mono , bypass xr[0][][] to lr[0][][]*/

		for(sb=0;sb<SBLIMIT;sb++)
			for(ss=0;ss<SSLIMIT;ss+=3) {
				lr[0][sb][ss]   = ro[0][sb][ss];
            lr[0][sb][ss+1] = ro[0][sb][ss+1];
				lr[0][sb][ss+2] = ro[0][sb][ss+2];
         }

   } else {

 	/* initialization */
   for (i=0; i<576; i+=8)
      is_pos[i] = is_pos[i+1] = is_pos[i+2] = is_pos[i+3] =
      is_pos[i+4] = is_pos[i+5] = is_pos[i+6] = is_pos[i+7] = 7;

	if (i_stereo)
	{  if (gr_info->window_switching_flag && (gr_info->block_type == 2))
		{  if( gr_info->mixed_block_flag )
			{  int32 max_sfb = 0;

				for (j=0; j<3; j++)
				{  int32 sfbcnt;
					sfbcnt = 2;
					for( sfb=12; sfb >=3; sfb-- )
					{
               	i = sfBandIndex[sfreq].s[sfb];
						lines = sfBandIndex[sfreq].s[sfb+1] - i;
                  i = (i << 2) - i + (j+1) * lines - 1;
						while ( lines > 0 )
						{  if ( ro[1][i/SSLIMIT][i%SSLIMIT] != 0.0f )
							{  sfbcnt = sfb;
								sfb = -10;
								lines = -10;
							}
							lines--;
							i--;
						}
					}
					sfb = sfbcnt + 1;

					if (sfb > max_sfb)
						max_sfb = sfb;

					while(sfb < 12)
					{	temp = sfBandIndex[sfreq].s[sfb];
               	sb   = sfBandIndex[sfreq].s[sfb+1] - temp;
                  i    = (temp << 2) - temp + j * sb;

						for ( ; sb > 0; sb--)
						{  is_pos[i] = scalefac[1].s[j][sfb];
							if (is_pos[i] != 7)
                        is_ratio[i] = TAN12[is_pos[i]];

							i++;
						}
						sfb++;
					}
					sfb = sfBandIndex[sfreq].s[10];
               sb  = sfBandIndex[sfreq].s[11] - sfb;
               sfb = (sfb << 2) - sfb + j * sb;
               temp  = sfBandIndex[sfreq].s[11];
               sb = sfBandIndex[sfreq].s[12] - temp;
               i = (temp << 2) - temp + j * sb;

					for ( ; sb > 0; sb-- )
					{  is_pos[i] = is_pos[sfb];
						is_ratio[i] = is_ratio[sfb];
						i++;
					}
				 }
				 if (max_sfb <= 3)
				 {  i = 2;
					 ss = 17;
					 sb = -1;
					 while (i >= 0)
					 {  if (ro[1][i][ss] != 0.0f)
						 {
                   	 sb = (i<<4) + (i<<1) + ss;
							 i = -1;
						 } else
						 {  ss--;
							 if (ss < 0)
							 {  i--;
								 ss = 17;
							 }
						 }
					 }
					 i = 0;
					 while ( sfBandIndex[sfreq].l[i] <= sb )
						 i++;
					 sfb = i;
					 i = sfBandIndex[sfreq].l[i];
					 for (; sfb<8; sfb++)
					 {  sb = sfBandIndex[sfreq].l[sfb+1]-sfBandIndex[sfreq].l[sfb];
						 for (; sb>0; sb--)
						 {  is_pos[i] = scalefac[1].l[sfb];
							 if (is_pos[i] != 7)
                      	 is_ratio[i] = TAN12[is_pos[i]];

							 i++;
						 }
					 }
				}
			} else
			{  for (j=0; j<3; j++)
				{  int32 sfbcnt;
					sfbcnt = -1;
					for( sfb=12; sfb >=0; sfb-- )
					{
						temp = sfBandIndex[sfreq].s[sfb];
                  lines = sfBandIndex[sfreq].s[sfb+1] - temp;
                  i = (temp << 2) - temp + (j+1) * lines - 1;

						while ( lines > 0 )
						{  if (ro[1][i/SSLIMIT][i%SSLIMIT] != 0.0f)
							{  sfbcnt = sfb;
								sfb = -10;
								lines = -10;
							}
							lines--;
							i--;
						}
					}
					sfb = sfbcnt + 1;
					while( sfb<12 )
					{
						temp = sfBandIndex[sfreq].s[sfb];
                  sb   = sfBandIndex[sfreq].s[sfb+1] - temp;
                  i    = (temp << 2) - temp + j * sb;
						for ( ; sb > 0; sb--)
						{  is_pos[i] = scalefac[1].s[j][sfb];
							if (is_pos[i] != 7)
						      is_ratio[i] = TAN12[is_pos[i]];

							i++;
						}
						sfb++;
					}

					temp = sfBandIndex[sfreq].s[10];
               temp2= sfBandIndex[sfreq].s[11];
               sb   = temp2 - temp;
               sfb  = (temp << 2) - temp + j * sb;
               sb   = sfBandIndex[sfreq].s[12] - temp2;
               i    = (temp2 << 2) - temp2 + j * sb;

					for (; sb>0; sb--)
					{  is_pos[i] = is_pos[sfb];
						is_ratio[i] = is_ratio[sfb];
						i++;
					}
				}
			}
		} else // ms-stereo
		{  i = 31;
			ss = 17;
			sb = 0;
			while (i >= 0)
			{  if (ro[1][i][ss] != 0.0f)
				{
            	sb = (i<<4) + (i<<1) + ss;
					i = -1;
				} else
				{  ss--;
					if ( ss < 0 )
					{  i--;
						ss = 17;
					}
				}
			}
			i = 0;
			while ( sfBandIndex[sfreq].l[i] <= sb )
				i++;
			sfb = i;
			i = sfBandIndex[sfreq].l[i];
			for ( ; sfb<21; sfb++ )
			{  sb = sfBandIndex[sfreq].l[sfb+1] - sfBandIndex[sfreq].l[sfb];
				for ( ; sb > 0; sb--)
				{  is_pos[i] = scalefac[1].l[sfb];
					if ( is_pos[i] != 7 )
               	is_ratio[i] = TAN12[is_pos[i]];

					i++;
				}
			}
			sfb = sfBandIndex[sfreq].l[20];
			for ( sb = 576 - sfBandIndex[sfreq].l[21]; sb > 0; sb-- )
			{  is_pos[i] = is_pos[sfb];
				is_ratio[i] = is_ratio[sfb];
				i++;
			}
		}
	}

   	i = 0;
		for(sb=0;sb<SBLIMIT;sb++)
			for(ss=0;ss<SSLIMIT;ss++) {
				if ( is_pos[i] == 7 ) {
					if (ms_stereo) {
						lr[0][sb][ss] = (ro[0][sb][ss]+ro[1][sb][ss])* 0.7071068f;
						lr[1][sb][ss] = (ro[0][sb][ss]-ro[1][sb][ss])* 0.7071068f;
					}
					else {
						lr[0][sb][ss] = ro[0][sb][ss];
						lr[1][sb][ss] = ro[1][sb][ss];
					}
				}
				else if (i_stereo ) {
               lr[1][sb][ss] = ro[0][sb][ss] / (real) (1 + is_ratio[i]);
  				   lr[0][sb][ss] = lr[1][sb][ss] * is_ratio[i];
				}
/*				else {
					printf("Error in stereo processing\n");
				} */
            i++;
			}

    } // channels == 2
}

real Ci[8]={-0.6f,-0.535f,-0.33f,-0.185f,-0.095f,-0.041f,-0.0142f,-0.0037f};

void LayerIII_Decoder::antialias(int32 ch, int32 gr)
{
   gr_info_s *gr_info = &(si->ch[ch].gr[gr]);

	real          bu,bd;  /* upper and lower butterfly inputs */
	int32         ss,sb;
 	static real   ca[8],cs[8];
 	static BOOL   antialias_init = FALSE;

	if (!antialias_init) {
		int32 i;
		real    sq;
		for (i=0;i<8;i++) {
			sq=sqrt(1.0f+Ci[i]*Ci[i]);
			cs[i] = 1.0f/sq;
         ca[i] = Ci[i] * cs[i];
		}
		antialias_init = TRUE;
	}

	/* 31 alias-reduction operations between each pair of sub-bands */
	/* with 8 butterflies between each pair                         */

	if  (gr_info->window_switching_flag && (gr_info->block_type == 2) &&
		 !gr_info->mixed_block_flag ) {

   	for(sb=0;sb<SBLIMIT;sb++) {
        	hybridIn[sb][0] = re_hybridOut[sb][0];
        	hybridIn[sb][1] = re_hybridOut[sb][1];
        	hybridIn[sb][2] = re_hybridOut[sb][2];
        	hybridIn[sb][3] = re_hybridOut[sb][3];
        	hybridIn[sb][4] = re_hybridOut[sb][4];
        	hybridIn[sb][5] = re_hybridOut[sb][5];
        	hybridIn[sb][6] = re_hybridOut[sb][6];
        	hybridIn[sb][7] = re_hybridOut[sb][7];
        	hybridIn[sb][8] = re_hybridOut[sb][8];
        	hybridIn[sb][9] = re_hybridOut[sb][9];
        	hybridIn[sb][10]= re_hybridOut[sb][10];
        	hybridIn[sb][11]= re_hybridOut[sb][11];
        	hybridIn[sb][12]= re_hybridOut[sb][12];
        	hybridIn[sb][13]= re_hybridOut[sb][13];
        	hybridIn[sb][14]= re_hybridOut[sb][14];
        	hybridIn[sb][15]= re_hybridOut[sb][15];
       	hybridIn[sb][16]= re_hybridOut[sb][16];
        	hybridIn[sb][17]= re_hybridOut[sb][17];
      }

	} else if (gr_info->window_switching_flag && gr_info->mixed_block_flag &&
	  (gr_info->block_type == 2)) {

		hybridIn[0][0] = re_hybridOut[0][0];
		hybridIn[0][1] = re_hybridOut[0][1];
		hybridIn[0][2] = re_hybridOut[0][2];
		hybridIn[0][3] = re_hybridOut[0][3];
      hybridIn[0][4] = re_hybridOut[0][4];
		hybridIn[0][5] = re_hybridOut[0][5];
		hybridIn[0][6] = re_hybridOut[0][6];
		hybridIn[0][7] = re_hybridOut[0][7];
		hybridIn[0][8] = re_hybridOut[0][8];
		hybridIn[0][9] = re_hybridOut[0][9];

      for(ss=0;ss<8;ss++) {
			bu = re_hybridOut[0][17-ss];
			bd = re_hybridOut[1][ss];
			hybridIn[0][17-ss] = (bu * cs[ss]) - (bd * ca[ss]);
			hybridIn[1][ss]    = (bd * cs[ss]) + (bu * ca[ss]);
      }

      hybridIn[1][8]  = re_hybridOut[1][8];
      hybridIn[1][9]  = re_hybridOut[1][9];
      hybridIn[1][10] = re_hybridOut[1][10];
      hybridIn[1][11] = re_hybridOut[1][11];
      hybridIn[1][12] = re_hybridOut[1][12];
      hybridIn[1][13] = re_hybridOut[1][13];
      hybridIn[1][14] = re_hybridOut[1][14];
      hybridIn[1][15] = re_hybridOut[1][15];
      hybridIn[1][16] = re_hybridOut[1][16];
      hybridIn[1][17] = re_hybridOut[1][17];

		for(sb=2;sb<SBLIMIT;sb++) {

        	hybridIn[sb][0] = re_hybridOut[sb][0];
        	hybridIn[sb][1] = re_hybridOut[sb][1];
        	hybridIn[sb][2] = re_hybridOut[sb][2];
        	hybridIn[sb][3] = re_hybridOut[sb][3];
        	hybridIn[sb][4] = re_hybridOut[sb][4];
        	hybridIn[sb][5] = re_hybridOut[sb][5];
        	hybridIn[sb][6] = re_hybridOut[sb][6];
        	hybridIn[sb][7] = re_hybridOut[sb][7];
        	hybridIn[sb][8] = re_hybridOut[sb][8];
        	hybridIn[sb][9] = re_hybridOut[sb][9];
        	hybridIn[sb][10]= re_hybridOut[sb][10];
        	hybridIn[sb][11]= re_hybridOut[sb][11];
        	hybridIn[sb][12]= re_hybridOut[sb][12];
        	hybridIn[sb][13]= re_hybridOut[sb][13];
        	hybridIn[sb][14]= re_hybridOut[sb][14];
        	hybridIn[sb][15]= re_hybridOut[sb][15];
        	hybridIn[sb][16]= re_hybridOut[sb][16];
        	hybridIn[sb][17]= re_hybridOut[sb][17];
      }

	} else {

      hybridIn[0][0] = re_hybridOut[0][0];
      hybridIn[0][1] = re_hybridOut[0][1];
      hybridIn[0][2] = re_hybridOut[0][2];
      hybridIn[0][3] = re_hybridOut[0][3];
      hybridIn[0][4] = re_hybridOut[0][4];
      hybridIn[0][5] = re_hybridOut[0][5];
      hybridIn[0][6] = re_hybridOut[0][6];
      hybridIn[0][7] = re_hybridOut[0][7];

		for(sb=0;sb<31;sb++) {
			for(ss=0;ss<8;ss++) {
				bu = re_hybridOut[sb][17-ss];
				bd = re_hybridOut[sb+1][ss];
				hybridIn[sb][17-ss]   = (bu * cs[ss]) - (bd * ca[ss]);
				hybridIn[sb+1][ss]    = (bd * cs[ss]) + (bu * ca[ss]);
			}

         hybridIn[sb][8]   = re_hybridOut[sb][8];
         hybridIn[sb][9]   = re_hybridOut[sb][9];
      }
      hybridIn[31][8]  = re_hybridOut[31][8];
      hybridIn[31][9]  = re_hybridOut[31][9];
      hybridIn[31][10] = re_hybridOut[31][10];
      hybridIn[31][11] = re_hybridOut[31][11];
      hybridIn[31][12] = re_hybridOut[31][12];
      hybridIn[31][13] = re_hybridOut[31][13];
      hybridIn[31][14] = re_hybridOut[31][14];
      hybridIn[31][15] = re_hybridOut[31][15];
      hybridIn[31][16] = re_hybridOut[31][16];
      hybridIn[31][17] = re_hybridOut[31][17];
   }
}

void LayerIII_Decoder::inv_mdct(real *in, real *out, int32 block_type)
{
/*
This uses Byeong Gi Lee's Fast Cosine Transform algorithm, but the
9 point IDCT needs to be reduced further. Unfortunately, I don't
know how to do that, because 9 is not an even number. - Jeff.*/

/*------------------------------------------------------------------*/
/*                                                                  */
/*    Function: Calculation of the inverse MDCT                     */
/*    In the case of short blocks the 3 output vectors are already  */
/*    overlapped and added in this modul.                           */
/*                                                                  */
/*    New layer3                                                    */
/*                                                                  */
/*------------------------------------------------------------------*/

int32   i, six_i, p;
int32   odd_i, two_odd_i, four_odd_i, eight_odd_i;
real    tmp[18], save, sum;
real 	  pp1, pp2;
static  BOOL MDCT_init = FALSE;
static  real  win[4][36];
static  real COS18[138];

	 if(!MDCT_init){

	 /* type 0 */
		for(i=0;i<36;i++)
			win[0][i] = (real) sin( PI36 *(i+0.5) );

	 /* type 1*/
		for(i=0;i<18;i++)
			win[1][i] = (real) sin( PI36 *(i+0.5) );
		for(i=18;i<24;i++)
			win[1][i] = 1.0f;
		for(i=24;i<30;i++)
			win[1][i] = (real) sin( PI12 *(i+0.5-18) );
		for(i=30;i<36;i++)
			win[1][i] = 0.0f;

	 /* type 2 (not needed anymore) */
/*		for(i=0;i<12;i++)
			win[2][i] = (real) sin( PI12*(i+0.5) ) ;
		for(i=12;i<36;i++)
			win[2][i] = 0.0f; */

	 /* type 3*/
		for(i=0;i<6;i++)
			win[3][i] = 0.0f;
		for(i=6;i<12;i++)
			win[3][i] = (real) sin(PI12 * (i+ 0.5 - 6.0));
		for(i=12;i<18;i++)
			win[3][i] =1.0f;
		for(i=18;i<36;i++)
			win[3][i] = (real) sin(PI36 * (i + 0.5));

   	for(i=0;i<138;i++)
      	COS18[i] = (real) cos(PI18 * i);

		MDCT_init = TRUE;
	 }

	 if(block_type == 2){

	   for(p=0;p<36;p+=9) {
	   	out[p]   = out[p+1] = out[p+2] = out[p+3] =
	      out[p+4] = out[p+5] = out[p+6] = out[p+7] =
	      out[p+8] = 0.0f;
	   }

   	six_i = 0;
	   for(i=0;i<3;i++)
   	{

      // 12 point IMDCT

      // Begin 12 point IDCT

   	// Input aliasing for 12 pt IDCT
	   in[15+i] += in[12+i]; in[12+i] += in[9+i]; in[9+i]  +=  in[6+i];
   	in[6+i]  += in[3+i];  in[3+i]  += in[0+i];

	   // Input aliasing on odd indices (for 6 point IDCT)
   	in[15+i] += in[9+i];  in[9+i]  += in[3+i];

	   // 3 point IDCT on even indices

		pp2 = in[12+i] * 0.500000000f;
	   pp1 = in[ 6+i] * 0.866025403f;
	   sum = in[0+i] + pp2;
	   tmp[1] = in[0+i] - in[12+i];
	   tmp[0] = sum + pp1;
	   tmp[2] = sum - pp1;

      // End 3 point IDCT on even indices

	   // 3 point IDCT on odd indices (for 6 point IDCT)

		pp2 = in[15+i] * 0.500000000f;
   	pp1 = in[9+i]  * 0.866025403f;
   	sum = in[3+i] + pp2;
   	tmp[4] = in[3+i] - in[15+i];
   	tmp[5] = sum + pp1;
   	tmp[3] = sum - pp1;

      // End 3 point IDCT on odd indices

   	// Twiddle factors on odd indices (for 6 point IDCT)

   	tmp[3] *= 1.931851653f;
   	tmp[4] *= 0.707106781f;
   	tmp[5] *= 0.517638090f;

   	// Output butterflies on 2 3 point IDCT's (for 6 point IDCT)

   	save = tmp[0];
   	tmp[0] += tmp[5];
   	tmp[5] = save - tmp[5];
	   save = tmp[1];
	   tmp[1] += tmp[4];
	   tmp[4] = save - tmp[4];
	   save = tmp[2];
	   tmp[2] += tmp[3];
	   tmp[3] = save - tmp[3];

   	// End 6 point IDCT

	   // Twiddle factors on indices (for 12 point IDCT)

	   tmp[0]  *=  0.504314480f;
	   tmp[1]  *=  0.541196100f;
	   tmp[2]  *=  0.630236207f;
	   tmp[3]  *=  0.821339815f;
	   tmp[4]  *=  1.306562965f;
	   tmp[5]  *=  3.830648788f;

      // End 12 point IDCT

	   // Shift to 12 point modified IDCT, multiply by window type 2
	   tmp[8]  = -tmp[0] * 0.793353340f;
	   tmp[9]  = -tmp[0] * 0.608761429f;
	   tmp[7]  = -tmp[1] * 0.923879532f;
	   tmp[10] = -tmp[1] * 0.382683432f;
	   tmp[6]  = -tmp[2] * 0.991444861f;
	   tmp[11] = -tmp[2] * 0.130526192f;

	   tmp[0]  =  tmp[3];
	   tmp[1]  =  tmp[4] * 0.382683432f;
	   tmp[2]  =  tmp[5] * 0.608761429f;

	   tmp[3]  = -tmp[5] * 0.793353340f;
   	tmp[4]  = -tmp[4] * 0.923879532f;
	   tmp[5]  = -tmp[0] * 0.991444861f;

	   tmp[0] *= 0.130526192f;

   	out[six_i + 6]  += tmp[0];
		out[six_i + 7]  += tmp[1];
	   out[six_i + 8]  += tmp[2];
		out[six_i + 9]  += tmp[3];
   	out[six_i + 10] += tmp[4];
		out[six_i + 11] += tmp[5];
	   out[six_i + 12] += tmp[6];
		out[six_i + 13] += tmp[7];
	   out[six_i + 14] += tmp[8];
		out[six_i + 15] += tmp[9];
	   out[six_i + 16] += tmp[10];
		out[six_i + 17] += tmp[11];

   	six_i += 6;
   	}

	 } else {

   // 36 point IDCT

   // input aliasing for 36 point IDCT
   in[17]+=in[16]; in[16]+=in[15]; in[15]+=in[14]; in[14]+=in[13];
   in[13]+=in[12]; in[12]+=in[11]; in[11]+=in[10]; in[10]+=in[9];
   in[9] +=in[8];  in[8] +=in[7];  in[7] +=in[6];  in[6] +=in[5];
   in[5] +=in[4];  in[4] +=in[3];  in[3] +=in[2];  in[2] +=in[1];
   in[1] +=in[0];

   // 18 point IDCT for odd indices

   // input aliasing for 18 point IDCT
   in[17]+=in[15]; in[15]+=in[13]; in[13]+=in[11]; in[11]+=in[9];
   in[9] +=in[7];  in[7] +=in[5];  in[5] +=in[3];  in[3] +=in[1];

   // 9 point IDCT on even indices
/*   for(i=0; i<9; i++) {
   	sum = 0.0;

      for(j=0;j<18;j+=2)
      	sum += in[j] * cos(PI36 * (2*i + 1) * j);

      tmp[i] = sum;
   } */

   for(i=0; i<9; i++) {

   	odd_i = (i << 1) + 1;

      sum = in[0];

      two_odd_i = odd_i << 1;
      four_odd_i = odd_i << 2;

      sum += in[2]  * COS18[odd_i];
      sum += in[4]  * COS18[two_odd_i];

      eight_odd_i = two_odd_i << 2;

      sum += in[6]  * COS18[four_odd_i - odd_i];
      sum += in[8]  * COS18[four_odd_i];
      sum += in[10] * COS18[four_odd_i + odd_i];
      sum += in[12] * COS18[four_odd_i + two_odd_i];
      sum += in[14] * COS18[eight_odd_i - odd_i];
      sum += in[16] * COS18[eight_odd_i];

      tmp[i] = sum;
   }

   // End 9 point IDCT on even indices

   // 9 point IDCT on odd indices
/*   for(i=0; i<9; i++) {
   	sum = 0.0;

      for(j=0;j<18;j+=2)
      	sum += in[j+1] * cos(PI36 * (2*i + 1) * j);

      tmp[17-i] = sum;
   } */

   for(i=0; i<9; i++) {

   	odd_i = (i << 1) + 1;

      sum = in[1];

      two_odd_i = odd_i << 1;
      four_odd_i = odd_i << 2;

      sum += in[3]  * COS18[odd_i];
      sum += in[5]  * COS18[two_odd_i];

      eight_odd_i = two_odd_i << 2;

      sum += in[7]  * COS18[four_odd_i - odd_i];
      sum += in[9]  * COS18[four_odd_i];
      sum += in[11] * COS18[four_odd_i + odd_i];
      sum += in[13] * COS18[four_odd_i + two_odd_i];
      sum += in[15] * COS18[eight_odd_i - odd_i];
      sum += in[17] * COS18[eight_odd_i];

      tmp[17-i] = sum;
   }

   // End 9 point IDCT on odd indices

   // Twiddle factors on odd indices

   tmp[9]  *= 5.736856623f;
   tmp[10] *= 1.931851653f;
   tmp[11] *= 1.183100792f;
   tmp[12] *= 0.871723397f;
   tmp[13] *= 0.707106781f;
   tmp[14] *= 0.610387294f;
   tmp[15] *= 0.551688959f;
   tmp[16] *= 0.517638090f;
   tmp[17] *= 0.501909918f;

   // Butterflies on 9 point IDCT's
   for (i=0;i<9;i++) {
   	save = tmp[i];
      tmp[i] += tmp[17-i];
      tmp[17-i] = save - tmp[17-i];
   }
   // end 18 point IDCT

   // twiddle factors for 36 point IDCT

   tmp[0] *=  -0.500476342f;
   tmp[1] *=  -0.504314480f;
   tmp[2] *=  -0.512139757f;
   tmp[3] *=  -0.524264562f;
   tmp[4] *=  -0.541196100f;
   tmp[5] *=  -0.563690973f;
	tmp[6] *=  -0.592844523f;
   tmp[7] *=  -0.630236207f;
   tmp[8] *=  -0.678170852f;
   tmp[9] *=  -0.740093616f;
   tmp[10]*=  -0.821339815f;
   tmp[11]*=  -0.930579498f;
   tmp[12]*=  -1.082840285f;
   tmp[13]*=  -1.306562965f;
   tmp[14]*=  -1.662754762f;
   tmp[15]*=  -2.310113158f;
   tmp[16]*=  -3.830648788f;
   tmp[17]*= -11.46279281f;

   // end 36 point IDCT

   // shift to modified IDCT

   out[0] =-tmp[9]  * win[block_type][0];
   out[1] =-tmp[10] * win[block_type][1];
   out[2] =-tmp[11] * win[block_type][2];
   out[3] =-tmp[12] * win[block_type][3];
   out[4] =-tmp[13] * win[block_type][4];
   out[5] =-tmp[14] * win[block_type][5];
   out[6] =-tmp[15] * win[block_type][6];
   out[7] =-tmp[16] * win[block_type][7];
   out[8] =-tmp[17] * win[block_type][8];

   out[9] = tmp[17] * win[block_type][9];
   out[10]= tmp[16] * win[block_type][10];
   out[11]= tmp[15] * win[block_type][11];
   out[12]= tmp[14] * win[block_type][12];
   out[13]= tmp[13] * win[block_type][13];
   out[14]= tmp[12] * win[block_type][14];
   out[15]= tmp[11] * win[block_type][15];
   out[16]= tmp[10] * win[block_type][16];
   out[17]= tmp[9]  * win[block_type][17];
   out[18]= tmp[8]  * win[block_type][18];
   out[19]= tmp[7]  * win[block_type][19];
   out[20]= tmp[6]  * win[block_type][20];
   out[21]= tmp[5]  * win[block_type][21];
   out[22]= tmp[4]  * win[block_type][22];
   out[23]= tmp[3]  * win[block_type][23];
   out[24]= tmp[2]  * win[block_type][24];
   out[25]= tmp[1]  * win[block_type][25];
   out[26]= tmp[0]  * win[block_type][26];

   out[27]= tmp[0]  * win[block_type][27];
   out[28]= tmp[1]  * win[block_type][28];
   out[29]= tmp[2]  * win[block_type][29];
   out[30]= tmp[3]  * win[block_type][30];
   out[31]= tmp[4]  * win[block_type][31];
   out[32]= tmp[5]  * win[block_type][32];
   out[33]= tmp[6]  * win[block_type][33];
   out[34]= tmp[7]  * win[block_type][34];
   out[35]= tmp[8]  * win[block_type][35];

   }
}

void LayerIII_Decoder::hybrid(int32 ch, int32 gr, int32 sb)
{

	real *fsIn  = hybridIn[sb];
   real *tsOut = re_hybridOut[sb];
   gr_info_s *gr_info = &(si->ch[ch].gr[gr]);

	real rawout[36];
	int32 bt;

	bt = (gr_info->window_switching_flag && gr_info->mixed_block_flag &&
			 (sb < 2)) ? 0 : gr_info->block_type;

	inv_mdct(fsIn, rawout, bt);

	/* overlap addition */
   tsOut[0]             = rawout[0]  + prevblck[ch][sb][0];
   prevblck[ch][sb][0]  = rawout[18];
   tsOut[1]             = rawout[1]  + prevblck[ch][sb][1];
   prevblck[ch][sb][1]  = rawout[19];
   tsOut[2]             = rawout[2]  + prevblck[ch][sb][2];
   prevblck[ch][sb][2]  = rawout[20];
   tsOut[3]             = rawout[3]  + prevblck[ch][sb][3];
   prevblck[ch][sb][3]  = rawout[21];
   tsOut[4]             = rawout[4]  + prevblck[ch][sb][4];
   prevblck[ch][sb][4]  = rawout[22];
   tsOut[5]             = rawout[5]  + prevblck[ch][sb][5];
   prevblck[ch][sb][5]  = rawout[23];
   tsOut[6]             = rawout[6]  + prevblck[ch][sb][6];
   prevblck[ch][sb][6]  = rawout[24];
   tsOut[7]             = rawout[7]  + prevblck[ch][sb][7];
   prevblck[ch][sb][7]  = rawout[25];
   tsOut[8]             = rawout[8]  + prevblck[ch][sb][8];
   prevblck[ch][sb][8]  = rawout[26];
   tsOut[9]             = rawout[9]  + prevblck[ch][sb][9];
   prevblck[ch][sb][9]  = rawout[27];
   tsOut[10]            = rawout[10] + prevblck[ch][sb][10];
   prevblck[ch][sb][10] = rawout[28];
   tsOut[11]            = rawout[11] + prevblck[ch][sb][11];
   prevblck[ch][sb][11] = rawout[29];
   tsOut[12]            = rawout[12] + prevblck[ch][sb][12];
   prevblck[ch][sb][12] = rawout[30];
   tsOut[13]            = rawout[13] + prevblck[ch][sb][13];
   prevblck[ch][sb][13] = rawout[31];
   tsOut[14]            = rawout[14] + prevblck[ch][sb][14];
   prevblck[ch][sb][14] = rawout[32];
   tsOut[15]            = rawout[15] + prevblck[ch][sb][15];
   prevblck[ch][sb][15] = rawout[33];
   tsOut[16]            = rawout[16] + prevblck[ch][sb][16];
   prevblck[ch][sb][16] = rawout[34];
   tsOut[17]            = rawout[17] + prevblck[ch][sb][17];
   prevblck[ch][sb][17] = rawout[35];

}

void LayerIII_Decoder::decode()
{
	 int32 nSlots = header->slots();
	 int32 gr, ch, ss, sb, main_data_end, flush_main;
	 int32 bytes_to_discard;

	 stream->get_side_info(channels, si);

	 for (; nSlots > 0; nSlots--)  // read main data.
		 br->hputbuf((uint32) stream->get_bits(8));

    main_data_end = br->hsstell() >> 3; // of previous frame

    if (flush_main=(br->hsstell() & 0x7)) {
         br->hgetbits(8 - flush_main);
			main_data_end++;
	 }

	 bytes_to_discard = frame_start - main_data_end
							  - si->main_data_begin;
	 if(main_data_end > 4096) {
			frame_start -= 4096;
			br->rewindNbytes(4096);
	 }

	 frame_start += header->slots();

    if (bytes_to_discard < 0)
			return;

	 for (; bytes_to_discard > 0; bytes_to_discard--) br->hgetbits(8);

	 for (gr=0;gr<2;gr++) {

			for (ch=0; ch<channels; ch++) {
           part2_start= br->hsstell();
			  get_scale_factors(ch, gr);
			  hufman_decode(ch, gr);
			  dequantize_sample(ro[ch], ch, gr);
			}

			stereo(gr);

			for (ch=0; ch<channels; ch++) {

					reorder (lr[ch], ch, gr);
					antialias(ch, gr);

					for (sb=0;sb<SBLIMIT;sb++) { // Hybrid synthesis.
						 hybrid(ch, gr, sb);
					}

					for (ss=1;ss<SSLIMIT;ss+=2) // Frequency inversion for polyphase.
						  for (sb=1;sb<SBLIMIT;sb+=2)
								  re_hybridOut[sb][ss] = -re_hybridOut[sb][ss];

					if (ch == 0)
					  for (ss=0;ss<SSLIMIT;ss++) { // Polyphase synthesis
							for (sb=0;sb<SBLIMIT;sb++)
								filter1->input_sample(re_hybridOut[sb][ss], sb);

							filter1->calculate_pcm_samples(buffer);
					  }
					else
					  for (ss=0;ss<SSLIMIT;ss++) { // Polyphase synthesis
							for (sb=0;sb<SBLIMIT;sb++)
								filter2->input_sample(re_hybridOut[sb][ss], sb);

							filter2->calculate_pcm_samples(buffer);
					  }
			}	// channels
	 }	// granule
  	 buffer->write_buffer (1);		// write to stdout
}

