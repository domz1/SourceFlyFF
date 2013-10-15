/* Data structures for layer III decoding. Taken from the ISO MPEG Audio
   Subgroup Software Simulation Group's public c source of the MPEG audio
   decoder.

   Last modified : 01/27/97  */


#ifndef L3TYPE_H
#define L3TYPE_H

#include "all.h"

typedef	struct {
				uint32 part2_3_length;
				uint32 big_values;
				uint32 global_gain;
				uint32 scalefac_compress;
				uint32 window_switching_flag;
				uint32 block_type;
				uint32 mixed_block_flag;
				uint32 table_select[3];
				uint32 subblock_gain[3];
				uint32 region0_count;
				uint32 region1_count;
				uint32 preflag;
				uint32 scalefac_scale;
				uint32 count1table_select;
			} gr_info_s;


typedef struct {
	uint32 main_data_begin;
	uint32 private_bits;
	struct {
		 uint32 scfsi[4];
		 gr_info_s gr[2];
		} ch[2];
	} III_side_info_t;


typedef struct {
	int32 l[23];            /* [cb] */
	int32 s[3][13];         /* [window][cb] */
	} III_scalefac_t[2];

#endif