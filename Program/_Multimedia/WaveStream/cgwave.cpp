/*===========================================================================*\
|
|  File:        cgwave.cpp
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

//** include files **
//#ifndef __WATCOMC__
//#define WIN32_LEAN_AND_MEAN
//#endif
#include "stdafx.h"
#include <mmsystem.h>
#include "cgwave.h"

//** local definitions **
//** external functions **
//** external data **
//** public data **
//** private data **
//** public functions **
//** private functions **

HMMIO	PmmioOpen(LPSTR szFilename, LPMMIOINFO lpmmioinfo, DWORD dwOpenFlags)
{
	HMMIO	hmmio;
//	CHAR	err[MAX_PATH];
	
		hmmio = mmioOpen( szFilename, lpmmioinfo, dwOpenFlags);
		if (hmmio==NULL) {
			TRACE("%s open error!", szFilename );
			//sprintf( err, "%s open error!", szFilename );
			//AfxMessageBox(err);
			return NULL;
		}

	FILE *tp = fopen( szFilename, "rb" );
	mmioSeek(hmmio, 0, SEEK_SET);
	fclose(tp);
	
	return hmmio;
}

// ----------------------------------------------------------
// WaveOpenFile - 
//  This function will open a wave input file and prepare 
//  it for reading, so the data can be easily read with 
//  WaveReadFile. Returns 0 if successful, the error code 
//  if not.
//      pszFileName     - Input filename to load.
//      phmmioIn    - Pointer to handle which will 
//              be used for further mmio routines.
//      ppwfxInfo   - Ptr to ptr to WaveFormatEx 
//              structure with all info about the file.
// ----------------------------------------------------------
int WaveOpenFile(
	char		*pszPackName,
    char        *pszFileName,
    HMMIO       *phmmioIn,
    WAVEFORMATEX    **ppwfxInfo,
    MMCKINFO    *pckInRIFF
)
{

    HMMIO       hmmioIn;
    MMCKINFO    ckIn;       // chunk info. for general use.
    PCMWAVEFORMAT   pcmWaveFormat;  // Temporary PCM format structure to load in.   
    WORD        cbExtraAlloc;   // Extra bytes for waveformatex structure for weird formats.
    int         nError;     // Return value.


    // Initialization...
    *ppwfxInfo = NULL;
    nError = 0;
    hmmioIn = NULL;
    
    if ((hmmioIn = PmmioOpen( pszFileName, NULL, MMIO_ALLOCBUF | MMIO_READ)) == NULL)
    {
        nError = ER_CANNOTOPEN;
        goto ERROR_READING_WAVE;
    }

    if ((nError = (int)mmioDescend(hmmioIn, pckInRIFF, NULL, 0)) != 0)
    {
        goto ERROR_READING_WAVE;
    }


    if ((pckInRIFF->ckid != FOURCC_RIFF) || (pckInRIFF->fccType != mmioFOURCC('W', 'A', 'V', 'E')))
    {
        nError = ER_NOTWAVEFILE;
        goto ERROR_READING_WAVE;
    }
            
    /* Search the input file for for the 'fmt ' chunk.     */
    ckIn.ckid = mmioFOURCC('f', 'm', 't', ' ');
    if ((nError = (int)mmioDescend(hmmioIn, &ckIn, pckInRIFF, MMIO_FINDCHUNK)) != 0)
    {
        goto ERROR_READING_WAVE;        
    }
                    
    /* Expect the 'fmt' chunk to be at least as large as <PCMWAVEFORMAT>;
    * if there are extra parameters at the end, we'll ignore them */
    
    if (ckIn.cksize < (long) sizeof(PCMWAVEFORMAT))
    {
        nError = ER_NOTWAVEFILE;
        goto ERROR_READING_WAVE;
    }
                                                                
    /* Read the 'fmt ' chunk into <pcmWaveFormat>.*/     
    if (mmioRead(hmmioIn, (HPSTR) &pcmWaveFormat, (long) sizeof(pcmWaveFormat)) != (long) sizeof(pcmWaveFormat))
    {
        nError = ER_CANNOTREAD;
        goto ERROR_READING_WAVE;
    }
                            

    // Ok, allocate the waveformatex, but if its not pcm
    // format, read the next word, and thats how many extra
    // bytes to allocate.
    if (pcmWaveFormat.wf.wFormatTag == WAVE_FORMAT_PCM)
        cbExtraAlloc = 0;               
                            
    else
    {
        // Read in length of extra bytes.
        if (mmioRead(hmmioIn, (LPSTR) &cbExtraAlloc,
        (long) sizeof(cbExtraAlloc)) != (long) sizeof(cbExtraAlloc))
        {
            nError = ER_CANNOTREAD;
            goto ERROR_READING_WAVE;
        }

    }
                            
    // Ok, now allocate that waveformatex structure.
    if ((*ppwfxInfo = (LPWAVEFORMATEX)GlobalAlloc(GMEM_FIXED, sizeof(WAVEFORMATEX)+cbExtraAlloc)) == NULL)
    {
        nError = ER_MEM;
        goto ERROR_READING_WAVE;
    }

    // Copy the bytes from the pcm structure to the waveformatex structure
    memcpy(*ppwfxInfo, &pcmWaveFormat, sizeof(pcmWaveFormat));
    (*ppwfxInfo)->cbSize = cbExtraAlloc;

    // Now, read those extra bytes into the structure, if cbExtraAlloc != 0.
    if (cbExtraAlloc != 0)
    {
        if (mmioRead(hmmioIn, (LPSTR) (((BYTE*)&((*ppwfxInfo)->cbSize))+sizeof(cbExtraAlloc)),
        (long) (cbExtraAlloc)) != (long) (cbExtraAlloc))
        {
            nError = ER_NOTWAVEFILE;
            goto ERROR_READING_WAVE;
        }
    }

    /* Ascend the input file out of the 'fmt ' chunk. */                                
    if ((nError = mmioAscend(hmmioIn, &ckIn, 0)) != 0)
    {
        goto ERROR_READING_WAVE;
    }
    

    goto TEMPCLEANUP;       

ERROR_READING_WAVE:
    if (*ppwfxInfo != NULL)
    {
        GlobalFree(*ppwfxInfo);
        *ppwfxInfo = NULL;
    }       

    if (hmmioIn != NULL)
        {
            mmioClose(hmmioIn, 0);
        hmmioIn = NULL;
    }
    
TEMPCLEANUP:
    *phmmioIn = hmmioIn;

    return(nError);

}


// ----------------------------------------------------------
// WaveStartDataRead - 
//  This routine has to be called before WaveReadFile as 
//  it searchs for the chunk to descend into for reading, 
//  that is, the 'data' chunk.  For simplicity, this used 
//  to be in the open routine, but was taken out and moved 
//  to a separate routine so there was more control on the 
//  chunks that are before the data chunk, such as 'fact', 
//  etc...
// ----------------------------------------------------------
int WaveStartDataRead(
    HMMIO       *phmmioIn,
    MMCKINFO    *pckIn,
    MMCKINFO    *pckInRIFF
)
{
    int nError;

    nError = 0;

    // Do a nice little seek...
    if ((nError = mmioSeek(*phmmioIn, pckInRIFF->dwDataOffset 
    + sizeof(FOURCC), SEEK_SET)) == -1)
    {
        //Assert(FALSE);
        goto ERROR_READING_WAVE;
    }

    nError = 0;
    //  Search the input file for for the 'data' chunk.
    pckIn->ckid = mmioFOURCC('d', 'a', 't', 'a');
    if ((nError = mmioDescend(*phmmioIn, pckIn, pckInRIFF, MMIO_FINDCHUNK)) != 0)
    {
        goto ERROR_READING_WAVE;
    }

    goto CLEANUP;

ERROR_READING_WAVE:

CLEANUP:    
    return(nError);
}


// ----------------------------------------------------------
// WaveReadFile - 
//  This will read wave data from the wave file.  Make 
//  sure we're descended into the data chunk, else this 
//  will fail bigtime!
//      hmmioIn     - Handle to mmio.
//      cbRead      - # of bytes to read.   
//      pbDest      - Destination buffer to put bytes.
//      cbActualRead    - # of bytes actually read.
// ----------------------------------------------------------
int WaveReadFile(
    HMMIO       hmmioIn,
    UINT        cbRead,
    BYTE        *pbDest,
    MMCKINFO    *pckIn,
    UINT        *cbActualRead
)
{

    MMIOINFO    mmioinfoIn;         // current status of <hmmioIn>
    int     nError;
    UINT        cT, cbDataIn;

    nError = 0;

    if (nError = mmioGetInfo(hmmioIn, &mmioinfoIn, 0) != 0)
    {
        goto ERROR_CANNOT_READ;
    }
                
    cbDataIn = cbRead;
    if (cbDataIn > pckIn->cksize) 
        cbDataIn = pckIn->cksize;   

    pckIn->cksize -= cbDataIn;
    
    for (cT = 0; cT < cbDataIn; cT++)
    {
        // Copy the bytes from the io to the buffer.
        if (mmioinfoIn.pchNext == mmioinfoIn.pchEndRead)
        {
            if ((nError = mmioAdvance(hmmioIn, &mmioinfoIn, MMIO_READ)) != 0)
            {
                        goto ERROR_CANNOT_READ;
            } 
            if (mmioinfoIn.pchNext == mmioinfoIn.pchEndRead)
            {
                nError = ER_CORRUPTWAVEFILE;
                        goto ERROR_CANNOT_READ;
            }
        }
        
        // Actual copy.
        *((BYTE*)pbDest+cT) = *((BYTE*)mmioinfoIn.pchNext++);
    }

    if ((nError = mmioSetInfo(hmmioIn, &mmioinfoIn, 0)) != 0)
    {
        goto ERROR_CANNOT_READ;
    }

    *cbActualRead = cbDataIn;
    goto FINISHED_READING;

ERROR_CANNOT_READ:
    *cbActualRead = 0;

FINISHED_READING:
    return(nError);

}

// ----------------------------------------------------------
// WaveCloseFile - 
//  This will close the wave file openned with WaveOpenFile.  
//      phmmioIn - Pointer to the handle to input MMIO.
//      ppwfxSrc - Pointer to pointer to WaveFormatEx structure.
//
//  Returns 0 if successful, non-zero if there was a warning.
// ----------------------------------------------------------
int WaveCloseReadFile(
    HMMIO       *phmmio,
    WAVEFORMATEX    **ppwfxSrc
)
{

    if (*ppwfxSrc != NULL)
    {
        GlobalFree(*ppwfxSrc);
        *ppwfxSrc = NULL;
    }

    if (*phmmio != NULL)
    {
        mmioClose(*phmmio, 0);
        *phmmio = NULL;
    }

    return(0);

}
