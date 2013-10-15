/*
 *  @(#) synthesis_filter.cc 1.14, last edit: 6/21/94 11:22:20
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
 *    - compute_new_v() uses a 32 point fast cosine transform as described by
 *      Byeong Gi Lee in IEEE Transactions ASSP-32 Part 2, August 1984,
 *      "A New Algorithm to Compute the Discrete Cosine Transform"
 *      instead of the matrix-vector multiplication in V1.1
 *    - loop unrolling done in compute_pcm_samples()
 *    - if ULAW is defined, the synthesis filter does a downsampling
 *      to 8 kHz by dropping samples and ignoring subbands above 4 kHz
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

#pragma warning(disable : 4305)

#include "stdafx.h"
 
#include <math.h>
#include "all.h" 
#include "synfilt.h"

static const real MY_PI = 3.14159265358979323846;

static const real cos1_64  = 1.0 / (2.0 * cos(MY_PI        / 64.0));
static const real cos3_64  = 1.0 / (2.0 * cos(MY_PI * 3.0  / 64.0));
static const real cos5_64  = 1.0 / (2.0 * cos(MY_PI * 5.0  / 64.0));
static const real cos7_64  = 1.0 / (2.0 * cos(MY_PI * 7.0  / 64.0));
static const real cos9_64  = 1.0 / (2.0 * cos(MY_PI * 9.0  / 64.0));
static const real cos11_64 = 1.0 / (2.0 * cos(MY_PI * 11.0 / 64.0));
static const real cos13_64 = 1.0 / (2.0 * cos(MY_PI * 13.0 / 64.0));
static const real cos15_64 = 1.0 / (2.0 * cos(MY_PI * 15.0 / 64.0));
static const real cos17_64 = 1.0 / (2.0 * cos(MY_PI * 17.0 / 64.0));
static const real cos19_64 = 1.0 / (2.0 * cos(MY_PI * 19.0 / 64.0));
static const real cos21_64 = 1.0 / (2.0 * cos(MY_PI * 21.0 / 64.0));
static const real cos23_64 = 1.0 / (2.0 * cos(MY_PI * 23.0 / 64.0));
static const real cos25_64 = 1.0 / (2.0 * cos(MY_PI * 25.0 / 64.0));
static const real cos27_64 = 1.0 / (2.0 * cos(MY_PI * 27.0 / 64.0));
static const real cos29_64 = 1.0 / (2.0 * cos(MY_PI * 29.0 / 64.0));
static const real cos31_64 = 1.0 / (2.0 * cos(MY_PI * 31.0 / 64.0));
static const real cos1_32  = 1.0 / (2.0 * cos(MY_PI        / 32.0));
static const real cos3_32  = 1.0 / (2.0 * cos(MY_PI * 3.0  / 32.0));
static const real cos5_32  = 1.0 / (2.0 * cos(MY_PI * 5.0  / 32.0));
static const real cos7_32  = 1.0 / (2.0 * cos(MY_PI * 7.0  / 32.0));
static const real cos9_32  = 1.0 / (2.0 * cos(MY_PI * 9.0  / 32.0));
static const real cos11_32 = 1.0 / (2.0 * cos(MY_PI * 11.0 / 32.0));
static const real cos13_32 = 1.0 / (2.0 * cos(MY_PI * 13.0 / 32.0));
static const real cos15_32 = 1.0 / (2.0 * cos(MY_PI * 15.0 / 32.0));
static const real cos1_16  = 1.0 / (2.0 * cos(MY_PI        / 16.0));
static const real cos3_16  = 1.0 / (2.0 * cos(MY_PI * 3.0  / 16.0));
static const real cos5_16  = 1.0 / (2.0 * cos(MY_PI * 5.0  / 16.0));
static const real cos7_16  = 1.0 / (2.0 * cos(MY_PI * 7.0  / 16.0));
static const real cos1_8   = 1.0 / (2.0 * cos(MY_PI        / 8.0));
static const real cos3_8   = 1.0 / (2.0 * cos(MY_PI * 3.0  / 8.0));
static const real cos1_4   = 1.0 / (2.0 * cos(MY_PI / 4.0));
 

const real SynthesisFilter::d[512] = {
  // Note: These values are not in the same order
  // as in Annex 3-B.3 of the ISO/IEC DIS 11172-3
   0.000000000, -0.000442505,  0.003250122, -0.007003784,
   0.031082153, -0.078628540,  0.100311279, -0.572036743,
   1.144989014,  0.572036743,  0.100311279,  0.078628540,
	0.031082153,  0.007003784,  0.003250122,  0.000442505,
  -0.000015259, -0.000473022,  0.003326416, -0.007919312,
   0.030517578, -0.084182739,  0.090927124, -0.600219727,
   1.144287109,  0.543823242,  0.108856201,  0.073059082,
   0.031478882,  0.006118774,  0.003173828,  0.000396729,
  -0.000015259, -0.000534058,  0.003387451, -0.008865356,
	0.029785156, -0.089706421,  0.080688477, -0.628295898,
   1.142211914,  0.515609741,  0.116577148,  0.067520142,
   0.031738281,  0.005294800,  0.003082275,  0.000366211,
  -0.000015259, -0.000579834,  0.003433228, -0.009841919,
   0.028884888, -0.095169067,  0.069595337, -0.656219482,
	1.138763428,  0.487472534,  0.123474121,  0.061996460,
   0.031845093,  0.004486084,  0.002990723,  0.000320435,
  -0.000015259, -0.000625610,  0.003463745, -0.010848999,
   0.027801514, -0.100540161,  0.057617188, -0.683914185,
   1.133926392,  0.459472656,  0.129577637,  0.056533813,
	0.031814575,  0.003723145,  0.002899170,  0.000289917,
  -0.000015259, -0.000686646,  0.003479004, -0.011886597,
	0.026535034, -0.105819702,  0.044784546, -0.711318970,
   1.127746582,  0.431655884,  0.134887695,  0.051132202,
   0.031661987,  0.003005981,  0.002792358,  0.000259399,
  -0.000015259, -0.000747681,  0.003479004, -0.012939453,
	0.025085449, -0.110946655,  0.031082153, -0.738372803,
   1.120223999,  0.404083252,  0.139450073,  0.045837402,
   0.031387329,  0.002334595,  0.002685547,  0.000244141,
  -0.000030518, -0.000808716,  0.003463745, -0.014022827,
   0.023422241, -0.115921021,  0.016510010, -0.765029907,
	1.111373901,  0.376800537,  0.143264771,  0.040634155,
   0.031005859,  0.001693726,  0.002578735,  0.000213623,
  -0.000030518, -0.000885010,  0.003417969, -0.015121460,
	0.021575928, -0.120697021,  0.001068115, -0.791213989,
   1.101211548,  0.349868774,  0.146362305,  0.035552979,
	0.030532837,  0.001098633,  0.002456665,  0.000198364,
  -0.000030518, -0.000961304,  0.003372192, -0.016235352,
   0.019531250, -0.125259399, -0.015228271, -0.816864014,
   1.089782715,  0.323318481,  0.148773193,  0.030609131,
   0.029937744,  0.000549316,  0.002349854,  0.000167847,
  -0.000030518, -0.001037598,  0.003280640, -0.017349243,
	0.017257690, -0.129562378, -0.032379150, -0.841949463,
   1.077117920,  0.297210693,  0.150497437,  0.025817871,
   0.029281616,  0.000030518,  0.002243042,  0.000152588,
  -0.000045776, -0.001113892,  0.003173828, -0.018463135,
	0.014801025, -0.133590698, -0.050354004, -0.866363525,
	1.063217163,  0.271591187,  0.151596069,  0.021179199,
   0.028533936, -0.000442505,  0.002120972,  0.000137329,
  -0.000045776, -0.001205444,  0.003051758, -0.019577026,
   0.012115479, -0.137298584, -0.069168091, -0.890090942,
   1.048156738,  0.246505737,  0.152069092,  0.016708374,
	0.027725220, -0.000869751,  0.002014160,  0.000122070,
  -0.000061035, -0.001296997,  0.002883911, -0.020690918,
   0.009231567, -0.140670776, -0.088775635, -0.913055420,
   1.031936646,  0.221984863,  0.151962280,  0.012420654,
   0.026840210, -0.001266479,  0.001907349,  0.000106812,
  -0.000061035, -0.001388550,  0.002700806, -0.021789551,
	0.006134033, -0.143676758, -0.109161377, -0.935195923,
   1.014617920,  0.198059082,  0.151306152,  0.008316040,
   0.025909424, -0.001617432,  0.001785278,  0.000106812,
  -0.000076294, -0.001480103,  0.002487183, -0.022857666,
   0.002822876, -0.146255493, -0.130310059, -0.956481934,
	0.996246338,  0.174789429,  0.150115967,  0.004394531,
   0.024932861, -0.001937866,  0.001693726,  0.000091553,
  -0.000076294, -0.001586914,  0.002227783, -0.023910522,
  -0.000686646, -0.148422241, -0.152206421, -0.976852417,
   0.976852417,  0.152206421,  0.148422241,  0.000686646,
	0.023910522, -0.002227783,  0.001586914,  0.000076294,
  -0.000091553, -0.001693726,  0.001937866, -0.024932861,
  -0.004394531, -0.150115967, -0.174789429, -0.996246338,
   0.956481934,  0.130310059,  0.146255493, -0.002822876,
   0.022857666, -0.002487183,  0.001480103,  0.000076294,
  -0.000106812, -0.001785278,  0.001617432, -0.025909424,
  -0.008316040, -0.151306152, -0.198059082, -1.014617920,
   0.935195923,  0.109161377,  0.143676758, -0.006134033,
   0.021789551, -0.002700806,  0.001388550,  0.000061035,
  -0.000106812, -0.001907349,  0.001266479, -0.026840210,
  -0.012420654, -0.151962280, -0.221984863, -1.031936646,
	0.913055420,  0.088775635,  0.140670776, -0.009231567,
   0.020690918, -0.002883911,  0.001296997,  0.000061035,
  -0.000122070, -0.002014160,  0.000869751, -0.027725220,
  -0.016708374, -0.152069092, -0.246505737, -1.048156738,
   0.890090942,  0.069168091,  0.137298584, -0.012115479,
	0.019577026, -0.003051758,  0.001205444,  0.000045776,
  -0.000137329, -0.002120972,  0.000442505, -0.028533936,
  -0.021179199, -0.151596069, -0.271591187, -1.063217163,
   0.866363525,  0.050354004,  0.133590698, -0.014801025,
   0.018463135, -0.003173828,  0.001113892,  0.000045776,
  -0.000152588, -0.002243042, -0.000030518, -0.029281616,
  -0.025817871, -0.150497437, -0.297210693, -1.077117920,
   0.841949463,  0.032379150,  0.129562378, -0.017257690,
   0.017349243, -0.003280640,  0.001037598,  0.000030518,
  -0.000167847, -0.002349854, -0.000549316, -0.029937744,
  -0.030609131, -0.148773193, -0.323318481, -1.089782715,
	0.816864014,  0.015228271,  0.125259399, -0.019531250,
   0.016235352, -0.003372192,  0.000961304,  0.000030518,
  -0.000198364, -0.002456665, -0.001098633, -0.030532837,
  -0.035552979, -0.146362305, -0.349868774, -1.101211548,
   0.791213989, -0.001068115,  0.120697021, -0.021575928,
	0.015121460, -0.003417969,  0.000885010,  0.000030518,
  -0.000213623, -0.002578735, -0.001693726, -0.031005859,
  -0.040634155, -0.143264771, -0.376800537, -1.111373901,
   0.765029907, -0.016510010,  0.115921021, -0.023422241,
   0.014022827, -0.003463745,  0.000808716,  0.000030518,
  -0.000244141, -0.002685547, -0.002334595, -0.031387329,
  -0.045837402, -0.139450073, -0.404083252, -1.120223999,
   0.738372803, -0.031082153,  0.110946655, -0.025085449,
   0.012939453, -0.003479004,  0.000747681,  0.000015259,
  -0.000259399, -0.002792358, -0.003005981, -0.031661987,
  -0.051132202, -0.134887695, -0.431655884, -1.127746582,
	0.711318970, -0.044784546,  0.105819702, -0.026535034,
   0.011886597, -0.003479004,  0.000686646,  0.000015259,
  -0.000289917, -0.002899170, -0.003723145, -0.031814575,
  -0.056533813, -0.129577637, -0.459472656, -1.133926392,
   0.683914185, -0.057617188,  0.100540161, -0.027801514,
	0.010848999, -0.003463745,  0.000625610,  0.000015259,
  -0.000320435, -0.002990723, -0.004486084, -0.031845093,
  -0.061996460, -0.123474121, -0.487472534, -1.138763428,
	0.656219482, -0.069595337,  0.095169067, -0.028884888,
	0.009841919, -0.003433228,  0.000579834,  0.000015259,
  -0.000366211, -0.003082275, -0.005294800, -0.031738281,
  -0.067520142, -0.116577148, -0.515609741, -1.142211914,
	0.628295898, -0.080688477,  0.089706421, -0.029785156,
	0.008865356, -0.003387451,  0.000534058,  0.000015259,
  -0.000396729, -0.003173828, -0.006118774, -0.031478882,
  -0.073059082, -0.108856201, -0.543823242, -1.144287109,
	0.600219727, -0.090927124,  0.084182739, -0.030517578,
	0.007919312, -0.003326416,  0.000473022,  0.000015259
};

SynthesisFilter::SynthesisFilter (uint32 channelnumber, real factor)
{
  register real *floatp, *floatp2;

  // initialize v1[] and v2[]:
  for (floatp = v1 + 512, floatp2 = v2 + 512; floatp > v1; )
	 *--floatp = *--floatp2 = 0.0;

  // initialize samples[]:
  for (floatp = samples + 32; floatp > samples; )
	 *--floatp = 0.0;

  channel = channelnumber;
  range_violations = written_samples = 0;
  actual_v = v1;
  actual_write_pos = 15;
  scalefactor = factor;
}


void SynthesisFilter::compute_new_v (void)
{
  real new_v[32];		// new V[0-15] and V[33-48] of Figure 3-A.2 in ISO DIS 11172-3
  register real *x1, *x2, tmp;
  real p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15;
  real pp0, pp1, pp2, pp3, pp4, pp5, pp6, pp7, pp8, pp9, pp10, pp11, pp12, pp13, pp14, pp15;

  // compute new values via a fast cosine transform:
  x1 = samples;
  x2 = samples + 31;
  p0  = *x1++ + *x2;   p1  = *x1++ + *--x2; p2  = *x1++ + *--x2; p3  = *x1++ + *--x2;
  p4  = *x1++ + *--x2; p5  = *x1++ + *--x2; p6  = *x1++ + *--x2; p7  = *x1++ + *--x2;
  p8  = *x1++ + *--x2; p9  = *x1++ + *--x2; p10 = *x1++ + *--x2; p11 = *x1++ + *--x2;
  p12 = *x1++ + *--x2; p13 = *x1++ + *--x2; p14 = *x1++ + *--x2; p15 = *x1   + *--x2;

  pp0  = p0 + p15; pp1 = p1 + p14; pp2 = p2 + p13; pp3 = p3 + p12;
  pp4  = p4 + p11; pp5 = p5 + p10; pp6 = p6 + p9;  pp7 = p7 + p8;
  pp8  = cos1_32  * (p0 - p15);
  pp9  = cos3_32  * (p1 - p14);
  pp10 = cos5_32  * (p2 - p13);
  pp11 = cos7_32  * (p3 - p12);
  pp12 = cos9_32  * (p4 - p11);
  pp13 = cos11_32 * (p5 - p10);
  pp14 = cos13_32 * (p6 - p9);
  pp15 = cos15_32 * (p7 - p8);

  p0  = pp0 + pp7; p1 = pp1 + pp6; p2 = pp2 + pp5; p3 = pp3 + pp4;
  p4  = cos1_16 * (pp0 - pp7);
  p5  = cos3_16 * (pp1 - pp6);
  p6  = cos5_16 * (pp2 - pp5);
  p7  = cos7_16 * (pp3 - pp4);
  p8  = pp8 + pp15; p9 = pp9 + pp14; p10 = pp10 + pp13; p11 = pp11 + pp12;
  p12 = cos1_16 * (pp8  - pp15);
  p13 = cos3_16 * (pp9  - pp14);
  p14 = cos5_16 * (pp10 - pp13);
  p15 = cos7_16 * (pp11 - pp12);

  pp0  = p0 + p3; pp1 = p1 + p2;
  pp2  = cos1_8 * (p0 - p3);
  pp3  = cos3_8 * (p1 - p2);
  pp4  = p4 + p7; pp5 = p5 + p6;
  pp6  = cos1_8 * (p4 - p7);
  pp7  = cos3_8 * (p5 - p6);
  pp8  = p8 + p11; pp9 = p9 + p10;
  pp10 = cos1_8 * (p8 - p11);
  pp11 = cos3_8 * (p9 - p10);
  pp12 = p12 + p15; pp13 = p13 + p14;
  pp14 = cos1_8 * (p12 - p15);
  pp15 = cos3_8 * (p13 - p14);

  p0 = pp0 + pp1;
  p1 = cos1_4 * (pp0 - pp1);
  p2 = pp2 + pp3;
  p3 = cos1_4 * (pp2 - pp3);
  p4 = pp4 + pp5;
  p5 = cos1_4 * (pp4 - pp5);
  p6 = pp6 + pp7;
  p7 = cos1_4 * (pp6 - pp7);
  p8  = pp8 + pp9;
  p9  = cos1_4 * (pp8 - pp9);
  p10 = pp10 + pp11;
  p11 = cos1_4 * (pp10 - pp11);
  p12 = pp12 + pp13;
  p13 = cos1_4 * (pp12 - pp13);
  p14 = pp14 + pp15;
  p15 = cos1_4 * (pp14 - pp15);

  tmp          = p6 + p7;
  new_v[36-17] = -(p5 + tmp);
  new_v[44-17] = -(p4 + tmp);
  tmp          = p11 + p15;
  new_v[10]    = tmp;
  new_v[6]     = p13 + tmp;
  tmp          = p14 + p15;
  new_v[46-17] = -(p8  + p12 + tmp);
  new_v[34-17] = -(p9  + p13 + tmp);
  tmp         += p10 + p11;
  new_v[38-17] = -(p13 + tmp);
  new_v[42-17] = -(p12 + tmp);
  new_v[2]     = p9 + p13 + p15;
  new_v[4]     = p5 + p7;
  new_v[48-17] = -p0;
  new_v[0]     = p1;
  new_v[8]     = p3;
  new_v[12]    = p7;
  new_v[14]    = p15;
  new_v[40-17] = -(p2  + p3);

  x1 = samples;
  x2 = samples + 31;
  p0  = cos1_64  * (*x1++ - *x2);   p1  = cos3_64  * (*x1++ - *--x2);
  p2  = cos5_64  * (*x1++ - *--x2); p3  = cos7_64  * (*x1++ - *--x2);
  p4  = cos9_64  * (*x1++ - *--x2); p5  = cos11_64 * (*x1++ - *--x2);
  p6  = cos13_64 * (*x1++ - *--x2); p7  = cos15_64 * (*x1++ - *--x2);
  p8  = cos17_64 * (*x1++ - *--x2); p9  = cos19_64 * (*x1++ - *--x2);
  p10 = cos21_64 * (*x1++ - *--x2); p11 = cos23_64 * (*x1++ - *--x2);
  p12 = cos25_64 * (*x1++ - *--x2); p13 = cos27_64 * (*x1++ - *--x2);
  p14 = cos29_64 * (*x1++ - *--x2); p15 = cos31_64 * (*x1   - *--x2);

  pp0  = p0 + p15; pp1 = p1 + p14; pp2 = p2 + p13; pp3 = p3 + p12;
  pp4  = p4 + p11; pp5 = p5 + p10; pp6 = p6 + p9;  pp7 = p7 + p8;
  pp8  = cos1_32  * (p0 - p15);
  pp9  = cos3_32  * (p1 - p14);
  pp10 = cos5_32  * (p2 - p13);
  pp11 = cos7_32  * (p3 - p12);
  pp12 = cos9_32  * (p4 - p11);
  pp13 = cos11_32 * (p5 - p10);
  pp14 = cos13_32 * (p6 - p9);
  pp15 = cos15_32 * (p7 - p8);

  p0  = pp0 + pp7; p1 = pp1 + pp6; p2 = pp2 + pp5; p3 = pp3 + pp4;
  p4  = cos1_16 * (pp0 - pp7);
  p5  = cos3_16 * (pp1 - pp6);
  p6  = cos5_16 * (pp2 - pp5);
  p7  = cos7_16 * (pp3 - pp4);
  p8  = pp8  + pp15; p9 = pp9  + pp14; p10 = pp10 + pp13; p11 = pp11 + pp12;
  p12 = cos1_16 * (pp8  - pp15);
  p13 = cos3_16 * (pp9  - pp14);
  p14 = cos5_16 * (pp10 - pp13);
  p15 = cos7_16 * (pp11 - pp12);

  pp0  = p0 + p3; pp1 = p1 + p2;
  pp2  = cos1_8 * (p0 - p3);
  pp3  = cos3_8 * (p1 - p2);
  pp4  = p4 + p7; pp5 = p5 + p6;
  pp6  = cos1_8 * (p4 - p7);
  pp7  = cos3_8 * (p5 - p6);
  pp8  = p8 + p11; pp9 = p9 + p10;
  pp10 = cos1_8 * (p8 - p11);
  pp11 = cos3_8 * (p9 - p10);
  pp12 = p12 + p15; pp13 = p13 + p14;
  pp14 = cos1_8 * (p12 - p15);
  pp15 = cos3_8 * (p13 - p14);

  p0 = pp0 + pp1;
  p1 = cos1_4 * (pp0 - pp1);
  p2 = pp2 + pp3;
  p3 = cos1_4 * (pp2 - pp3);
  p4 = pp4 + pp5;
  p5 = cos1_4 * (pp4 - pp5);
  p6 = pp6 + pp7;
  p7 = cos1_4 * (pp6 - pp7);
  p8  = pp8 + pp9;
  p9  = cos1_4 * (pp8 - pp9);
  p10 = pp10 + pp11;
  p11 = cos1_4 * (pp10 - pp11);
  p12 = pp12 + pp13;
  p13 = cos1_4 * (pp12 - pp13);
  p14 = pp14 + pp15;
  p15 = cos1_4 * (pp14 - pp15);

  tmp          = p13 + p15;
  new_v[1]     = p1 + p9 + tmp;
  new_v[5]     = p5 + p7 + p11 + tmp;
  tmp         += p9;
  new_v[33-17] = -(p1 + p14 + tmp);
  tmp         += p5 + p7;
  new_v[3]     = tmp;
  new_v[35-17] = -(p6 + p14 + tmp);
  tmp          = p10 + p11 + p12 + p13 + p14 + p15;
  new_v[39-17] = -(p2 + p3 + tmp - p12);
  new_v[43-17] = -(p4 + p6 + p7 + tmp - p13);
  new_v[37-17] = -(p5 + p6 + p7 + tmp - p12);
  new_v[41-17] = -(p2 + p3 + tmp - p13);
  tmp          = p8 + p12 + p14 + p15;
  new_v[47-17] = -(p0 + tmp);
  new_v[45-17] = -(p4 + p6 + p7 + tmp);
  tmp          = p11 + p15;
  new_v[11]    = p7  + tmp;
  tmp         += p3;
  new_v[9]     = tmp;
  new_v[7]     = p13 + tmp;
  new_v[13]    = p7 + p15;
  new_v[15]    = p15;

  // insert V[0-15] (== new_v[0-15]) into actual v:
  x1 = new_v;
  x2 = actual_v + actual_write_pos;
  *x2 = *x1++; x2 += 16; *x2 = *x1++; x2 += 16; *x2 = *x1++; x2 += 16; *x2 = *x1++; x2 += 16;
  *x2 = *x1++; x2 += 16; *x2 = *x1++; x2 += 16; *x2 = *x1++; x2 += 16; *x2 = *x1++; x2 += 16;
  *x2 = *x1++; x2 += 16; *x2 = *x1++; x2 += 16; *x2 = *x1++; x2 += 16; *x2 = *x1++; x2 += 16;
  *x2 = *x1++; x2 += 16; *x2 = *x1++; x2 += 16; *x2 = *x1++; x2 += 16; *x2 = *x1;   x2 += 16;
  // V[16] is always 0.0:
  *x2 = 0.0; x2 += 16;
  // insert V[17-31] (== -new_v[15-1]) into actual v:
  *x2 = -*x1;   x2 += 16; *x2 = -*--x1; x2 += 16; *x2 = -*--x1; x2 += 16; *x2 = -*--x1; x2 += 16;
  *x2 = -*--x1; x2 += 16; *x2 = -*--x1; x2 += 16; *x2 = -*--x1; x2 += 16; *x2 = -*--x1; x2 += 16;
  *x2 = -*--x1; x2 += 16; *x2 = -*--x1; x2 += 16; *x2 = -*--x1; x2 += 16; *x2 = -*--x1; x2 += 16;
  *x2 = -*--x1; x2 += 16; *x2 = -*--x1; x2 += 16; *x2 = -*--x1;

  // insert V[32] (== -new_v[0]) into other v:
  x2 = (actual_v == v1 ? v2 : v1) + actual_write_pos;
  *x2 = -*--x1; x2 += 16;
  // insert V[33-48] (== new_v[16-31]) into other v:
  x1 = new_v + 16;
  *x2 = *x1++; x2 += 16; *x2 = *x1++; x2 += 16; *x2 = *x1++; x2 += 16; *x2 = *x1++; x2 += 16;
  *x2 = *x1++; x2 += 16; *x2 = *x1++; x2 += 16; *x2 = *x1++; x2 += 16; *x2 = *x1++; x2 += 16;
  *x2 = *x1++; x2 += 16; *x2 = *x1++; x2 += 16; *x2 = *x1++; x2 += 16; *x2 = *x1++; x2 += 16;
  *x2 = *x1++; x2 += 16; *x2 = *x1++; x2 += 16; *x2 = *x1++; x2 += 16; *x2 = *x1;   x2 += 16;
  // insert V[49-63] (== new_v[30-16]) into other v:
  *x2 = *--x1; x2 += 16; *x2 = *--x1; x2 += 16; *x2 = *--x1; x2 += 16; *x2 = *--x1; x2 += 16;
  *x2 = *--x1; x2 += 16; *x2 = *--x1; x2 += 16; *x2 = *--x1; x2 += 16; *x2 = *--x1; x2 += 16;
  *x2 = *--x1; x2 += 16; *x2 = *--x1; x2 += 16; *x2 = *--x1; x2 += 16; *x2 = *--x1; x2 += 16;
  *x2 = *--x1; x2 += 16; *x2 = *--x1; x2 += 16; *x2 = *--x1;
}


void SynthesisFilter::compute_pcm_samples (Obuffer *buffer)
{
  uint32 i;
  register real floatreg, *vp;
  register const real *dp;
  int32 pcm_sample;

  i = 0;
  dp = d;

  switch (actual_write_pos)
  {
	 case 0:
		vp = actual_v;
		for (; i < 32; ++i)
		{
	floatreg = *vp * *dp++;
	vp += 15;
	floatreg += *vp * *dp++;   floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++;
	pcm_sample = (int)(floatreg * scalefactor);
	if (pcm_sample > 32767)
	{
/*	  ++range_violations;
	  if (floatreg > max_violation)
		 max_violation = floatreg;    */
	  pcm_sample = 32767;
	}
	else if (pcm_sample < -32768)
	{
/*	  ++range_violations;
	  if (-floatreg > max_violation)
		 max_violation = -floatreg; */
	  pcm_sample = -32768;
	}
	buffer->append (channel, (int32)pcm_sample);
	vp += 15;
		}
		break;
	 case 1:
		vp = actual_v + 1;
		for (; i < 32; ++i)
		{

	floatreg = *vp * *dp++;    floatreg += *--vp * *dp++;
	vp += 15;
	floatreg += *vp * *dp++;   floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	pcm_sample = (int)(floatreg * scalefactor);
	if (pcm_sample > 32767)
	{
/*	  ++range_violations;
	  if (floatreg > max_violation)
		 max_violation = floatreg; */
	  pcm_sample = 32767;
	}
	else if (pcm_sample < -32768)
	{
/*	  ++range_violations;
	  if (-floatreg > max_violation)
		 max_violation = -floatreg; */
	  pcm_sample = -32768;
	}
	buffer->append (channel, (int32)pcm_sample);
	vp += 15;
		}
		break;
	 case 2:
		vp = actual_v + 2;
		for (; i < 32; ++i)
		{
	floatreg = *vp * *dp++;    floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++;
	vp += 15;
	floatreg += *vp * *dp++;   floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++;
	pcm_sample = (int)(floatreg * scalefactor);
	if (pcm_sample > 32767)
	{
/*	  ++range_violations;
	  if (floatreg > max_violation)
		 max_violation = floatreg; */
	  pcm_sample = 32767;
	}
	else if (pcm_sample < -32768)
	{
	  ++range_violations;
/*	  if (-floatreg > max_violation)
		 max_violation = -floatreg; */
	  pcm_sample = -32768;
	}
	buffer->append (channel, (int32)pcm_sample);
	vp += 15;
		}
		break;
	 case 3:
		vp = actual_v + 3;
		for (; i < 32; ++i)
		{

	floatreg = *vp * *dp++;    floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	vp += 15;
	floatreg += *vp * *dp++;   floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	pcm_sample = (int)(floatreg * scalefactor);
	if (pcm_sample > 32767)
	{
/*	  ++range_violations;
	  if (floatreg > max_violation)
		 max_violation = floatreg; */
	  pcm_sample = 32767;
	}
	else if (pcm_sample < -32768)
	{
/*	  ++range_violations;
	  if (-floatreg > max_violation)
		 max_violation = -floatreg; */
	  pcm_sample = -32768;
	}
	buffer->append (channel, (int32)pcm_sample);
	vp += 15;
		}
		break;
	 case 4:
		vp = actual_v + 4;
		for (; i < 32; ++i)
		{

	floatreg = *vp * *dp++;    floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++;
	vp += 15;
	floatreg += *vp * *dp++;   floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++;
	pcm_sample = (int)(floatreg * scalefactor);
	if (pcm_sample > 32767)
	{
/*	  ++range_violations;
	  if (floatreg > max_violation)
		 max_violation = floatreg; */
	  pcm_sample = 32767;
	}
	else if (pcm_sample < -32768)
	{
/*	  ++range_violations;
	  if (-floatreg > max_violation)
		 max_violation = -floatreg; */
	  pcm_sample = -32768;
	}
	buffer->append (channel, (int32)pcm_sample);
	vp += 15;
		}
		break;
	 case 5:
		vp = actual_v + 5;
		for (; i < 32; ++i)
		{

	floatreg = *vp * *dp++;    floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	vp += 15;
	floatreg += *vp * *dp++;   floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	pcm_sample = (int)(floatreg * scalefactor);
	if (pcm_sample > 32767)
	{
/*	  ++range_violations;
	  if (floatreg > max_violation)
		 max_violation = floatreg;   */
	  pcm_sample = 32767;
	}
	else if (pcm_sample < -32768)
	{
/*	  ++range_violations;
	  if (-floatreg > max_violation)
		 max_violation = -floatreg;   */
	  pcm_sample = -32768;
	}
	buffer->append (channel, (int32)pcm_sample);
	vp += 15;
		}
		break;
	 case 6:
		vp = actual_v + 6;
		for (; i < 32; ++i)
		{

	floatreg = *vp * *dp++;    floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++;
	vp += 15;
	floatreg += *vp * *dp++;   floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++;
	pcm_sample = (int)(floatreg * scalefactor);
	if (pcm_sample > 32767)
	{
/*	  ++range_violations;
	  if (floatreg > max_violation)
		 max_violation = floatreg; */
	  pcm_sample = 32767;
	}
	else if (pcm_sample < -32768)
	{
/*	  ++range_violations;
	  if (-floatreg > max_violation)
		 max_violation = -floatreg; */
	  pcm_sample = -32768;
	}
	buffer->append (channel, (int32)pcm_sample);
	vp += 15;
		}
		break;
	 case 7:
		vp = actual_v + 7;
		for (; i < 32; ++i)
		{

	floatreg = *vp * *dp++;    floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	vp += 15;
	floatreg += *vp * *dp++;   floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	pcm_sample = (int)(floatreg * scalefactor);
	if (pcm_sample > 32767)
	{
/*	  ++range_violations;
	  if (floatreg > max_violation)
		 max_violation = floatreg;   */
	  pcm_sample = 32767;
	}
	else if (pcm_sample < -32768)
	{
/*	  ++range_violations;
	  if (-floatreg > max_violation)
		 max_violation = -floatreg;   */
	  pcm_sample = -32768;
	}
	buffer->append (channel, (int32)pcm_sample);
	vp += 15;
		}
		break;
	 case 8:
		vp = actual_v + 8;
		for (; i < 32; ++i)
		{

	floatreg = *vp * *dp++;    floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++;
	vp += 15;
	floatreg += *vp * *dp++;   floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++;
	pcm_sample = (int)(floatreg * scalefactor);
	if (pcm_sample > 32767)
	{
/*	  ++range_violations;
	  if (floatreg > max_violation)
		 max_violation = floatreg; */
	  pcm_sample = 32767;
	}
	else if (pcm_sample < -32768)
	{
/*	  ++range_violations;
	  if (-floatreg > max_violation)
		 max_violation = -floatreg; */
	  pcm_sample = -32768;
	}
	buffer->append (channel, (int32)pcm_sample);
	vp += 15;
		}
		break;
	 case 9:
		vp = actual_v + 9;
		for (; i < 32; ++i)
		{

	floatreg = *vp * *dp++;    floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	vp += 15;
	floatreg += *vp * *dp++;   floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	pcm_sample = (int)(floatreg * scalefactor);
	if (pcm_sample > 32767)
	{
	  ++range_violations;
/*	  if (floatreg > max_violation)
		 max_violation = floatreg; */
	  pcm_sample = 32767;
	}
	else if (pcm_sample < -32768)
	{
/*	  ++range_violations;
	  if (-floatreg > max_violation)
		 max_violation = -floatreg; */
	  pcm_sample = -32768;
	}
	buffer->append (channel, (int32)pcm_sample);
	vp += 15;
		}
		break;
	 case 10:
		vp = actual_v + 10;
		for (; i < 32; ++i)
		{

	floatreg = *vp * *dp++;    floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++;
	vp += 15;
	floatreg += *vp * *dp++;   floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++;
	pcm_sample = (int)(floatreg * scalefactor);
	if (pcm_sample > 32767)
	{
/*	  ++range_violations;
	  if (floatreg > max_violation)
		 max_violation = floatreg; */
	  pcm_sample = 32767;
	}
	else if (pcm_sample < -32768)
	{
/*	  ++range_violations;
	  if (-floatreg > max_violation)
		 max_violation = -floatreg; */
	  pcm_sample = -32768;
	}
	buffer->append (channel, (int32)pcm_sample);
	vp += 15;
		}
		break;
	 case 11:
		vp = actual_v + 11;
		for (; i < 32; ++i)
		{

	floatreg = *vp * *dp++;    floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	vp += 15;
	floatreg += *vp * *dp++;   floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	pcm_sample = (int)(floatreg * scalefactor);
	if (pcm_sample > 32767)
	{
/*	  ++range_violations;
	  if (floatreg > max_violation)
		 max_violation = floatreg; */
	  pcm_sample = 32767;
	}
	else if (pcm_sample < -32768)
	{
/*	  ++range_violations;
	  if (-floatreg > max_violation)
		 max_violation = -floatreg; */
	  pcm_sample = -32768;
	}
	buffer->append (channel, (int32)pcm_sample);
	vp += 15;
		}
		break;
	 case 12:
		vp = actual_v + 12;
		for (; i < 32; ++i)
		{

	floatreg = *vp * *dp++;    floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++;
	vp += 15;
	floatreg += *vp * *dp++;   floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++;
	pcm_sample = (int)(floatreg * scalefactor);
	if (pcm_sample > 32767)
	{
/*	  ++range_violations;
	  if (floatreg > max_violation)
		 max_violation = floatreg; */
	  pcm_sample = 32767;
	}
	else if (pcm_sample < -32768)
	{
/*	  ++range_violations;
	  if (-floatreg > max_violation)
		 max_violation = -floatreg; */
	  pcm_sample = -32768;
	}
	buffer->append (channel, (int32)pcm_sample);
	vp += 15;
		}
		break;
	 case 13:
		vp = actual_v + 13;
		for (; i < 32; ++i)
		{

	floatreg = *vp * *dp++;    floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	vp += 15;
	floatreg += *vp * *dp++;   floatreg += *--vp * *dp++;
	pcm_sample = (int)(floatreg * scalefactor);
	if (pcm_sample > 32767)
	{
/*	  ++range_violations;
	  if (floatreg > max_violation)
		 max_violation = floatreg; */
	  pcm_sample = 32767;
	}
	else if (pcm_sample < -32768)
	{
/*	  ++range_violations;
	  if (-floatreg > max_violation)
		 max_violation = -floatreg; */
	  pcm_sample = -32768;
	}
	buffer->append (channel, (int32)pcm_sample);
	vp += 15;
		}
		break;
	 case 14:
		vp = actual_v + 14;
		for (; i < 32; ++i)
		{

	floatreg = *vp * *dp++;    floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++;
	vp += 15;
	floatreg += *vp * *dp++;
	pcm_sample = (int)(floatreg * scalefactor);
	if (pcm_sample > 32767)
	{
/*	  ++range_violations;
	  if (floatreg > max_violation)
		 max_violation = floatreg; */
	  pcm_sample = 32767;
	}
	else if (pcm_sample < -32768)
	{
/*	  ++range_violations;
	  if (-floatreg > max_violation)
		 max_violation = -floatreg; */
	  pcm_sample = -32768;
	}
	buffer->append (channel, (int32)pcm_sample);
	vp += 15;
		}
		break;
	 case 15:
		vp = actual_v + 15;
		for (; i < 32; ++i)
		{

	floatreg = *vp * *dp++;    floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	floatreg += *--vp * *dp++; floatreg += *--vp * *dp++;
	pcm_sample = (int)(floatreg * scalefactor);
	if (pcm_sample > 32767)
	{
/*	  ++range_violations;
	  if (floatreg > max_violation)
		 max_violation = floatreg; */
	  pcm_sample = 32767;
	}
	else if (pcm_sample < -32768)
	{
/*	  ++range_violations;
	  if (-floatreg > max_violation)
		 max_violation = -floatreg; */
	  pcm_sample = -32768;
	}

	buffer->append(channel, (int32)pcm_sample);
	vp += 31;
		}
  }

}

void SynthesisFilter::calculate_pcm_samples (Obuffer *buffer)
{
  compute_new_v ();
  compute_pcm_samples (buffer);

  written_samples += 32;
  if (actual_write_pos < 15)
	 ++actual_write_pos;
  else
	 actual_write_pos = 0;
  actual_v = (actual_v == v1 ? v2 : v1);

  // initialize samples[]:
  for (register real *floatp = samples + 32; floatp > samples; )
	 *--floatp = 0.0;
}
