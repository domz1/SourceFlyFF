/* Layer III decoder object declarations. Rearrangement
   of layer III routines as an object by Jeff Tsay.

   Last modified : 01/31/97 */

#ifndef LAYERIII_H
#define LAYERIII_H

#include "all.h"
#include "l3type.h"
#include "ibitstr.h"
#include "obuffer.h"
#include "bit_res.h"
#include "header.h"
#include "synfilt.h"

#define SSLIMIT 18
#define SBLIMIT 32

class LayerIII_Decoder {

public:

	LayerIII_Decoder(char *huffdec_path0, Ibitstream *stream0, Header *header0,
   	           	  SynthesisFilter *filtera, SynthesisFilter *filterb,
                    Obuffer *buffer0);

   ~LayerIII_Decoder();

   // Notify decoder that a seek is being made
   void seek_notify();

   // Decode one frame, filling the buffer with the output samples
	void decode();


private:

	// private data

	int32 is[SBLIMIT][SSLIMIT];
   real  ro[2][SBLIMIT][SSLIMIT];
   real  lr[2][SBLIMIT][SSLIMIT];
	real  hybridIn[SBLIMIT][SSLIMIT];
   real  re_hybridOut[SBLIMIT][SSLIMIT];
   real prevblck[2][SBLIMIT][SSLIMIT];

   char *huffdec_path;
	Ibitstream *stream;
   Header *header;
   SynthesisFilter *filter1, *filter2;
   Obuffer *buffer;
   Bit_Reserve *br;
   III_side_info_t *si;
   III_scalefac_t scalefac;

   int32 frame_start;
   int32 part2_start;
   uint32 channels;

   // private member functions

	void get_scale_factors(int32 ch, int32 gr);
   void hufman_decode(int32 ch, int32 gr);
	real two_pow2(int32 scale, int32 preflag, int32 pretab_offset, int32 l);
	void dequantize_sample(real xr[SBLIMIT][SSLIMIT], int32 ch, int32 gr);
	void reorder(real xr[SBLIMIT][SSLIMIT], int32 ch, int32 gr);
	void stereo(int32 gr);
	void antialias(int32 ch, int32 gr);
   void inv_mdct(real *in, real *out, int32 block_type);
	void hybrid(int32 ch, int32 gr, int32 sb);

};

#endif
