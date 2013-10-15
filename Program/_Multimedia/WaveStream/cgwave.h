/*===========================================================================*\
|
|  File:        cgwave.h
|
|  Description: 
|       
|-----------------------------------------------------------------------------
|
|  Copyright (C) 1995-1996 Microsoft Corporation.  All Rights Reserved.
|
|  Written by Moss Bay Engineering, Inc. under contract to Microsoft Corporation
|
\*===========================================================================*/
/**************************************************************************

    (C) Copyright 1995-1996 Microsoft Corp.  All rights reserved.

    You have a royalty-free right to use, modify, reproduce and 
    distribute the Sample Files (and/or any modified version) in 
    any way you find useful, provided that you agree that 
    Microsoft has no warranty obligations or liability for any 
    Sample Application Files which are modified. 

    we do not recomend you base your game on IKlowns, start with one of
    the other simpler sample apps in the GDK

 **************************************************************************/

#ifndef _CGWAVE_H
#define _CGWAVE_H
//#include <mmsystem.h>

#define WAVEVERSION 1

#ifndef ER_MEM
#define ER_MEM          0xe000
#endif

#ifndef ER_CANNOTOPEN
#define ER_CANNOTOPEN       0xe100
#endif

#ifndef ER_NOTWAVEFILE
#define ER_NOTWAVEFILE      0xe101
#endif

#ifndef ER_CANNOTREAD
#define ER_CANNOTREAD       0xe102
#endif

#ifndef ER_CORRUPTWAVEFILE
#define ER_CORRUPTWAVEFILE  0xe103
#endif

#ifndef ER_CANNOTWRITE
#define ER_CANNOTWRITE      0xe104
#endif


int WaveOpenFile(
	char		*pszPackName,
    char        *pszFileName,
    HMMIO       *phmmioIn,
    WAVEFORMATEX    **ppwfxInfo,
    MMCKINFO    *pckInRIFF
);

int WaveStartDataRead(
    HMMIO       *phmmioIn,
    MMCKINFO    *pckIn,
    MMCKINFO    *pckInRIFF
);

int WaveReadFile(
    HMMIO       hmmioIn,
    UINT        cbRead,
    BYTE        *pbDest,
    MMCKINFO    *pckIn,
    UINT        *cbActualRead
);
#endif
