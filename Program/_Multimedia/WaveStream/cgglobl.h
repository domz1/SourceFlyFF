/*===========================================================================*\
|
|  File:        cgglobl.h
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

#ifndef CGGLOBL_H
#define CGGLOBL_H

//nclude <windows.h>

// NOTE: these scalar values are used in iklowns.gam & cglevel.cpp
typedef enum
{
    MCP_UNKNOWN = 0,
    MCP_386 = 1,
    MCP_486 = 2,
    MCP_PENTIUM = 3
} MC_PROCESSOR;

typedef enum
{
    MCB_UNKNOWN = 0,
    MCB_ISA = 1,
    MCB_EISA = 2,
    MCB_PCI = 3
} MC_BUSTYPE;

typedef enum
{
    MCV_UNKNOWN = 0,
    MCV_DDRAW = 1
} MC_VIDSYS;

typedef struct _MACHINE_CAPS
{
    MC_PROCESSOR processor;
    MC_BUSTYPE bus;
    DWORD sysMemory;
    DWORD vidMemory;
    MC_VIDSYS vidSystem;
} MACHINE_CAPS;

#ifdef __cplusplus
extern "C"
{
#endif

extern HINSTANCE ghInst;
extern BOOL gUse_DDraw;
extern BOOL gDoubleBuffer;

extern int gSoundMode;
extern BOOL gMusicOn;
extern BOOL gActive;

extern char gDataPath[];

extern MACHINE_CAPS gMachineCaps;

#ifdef __cplusplus
}
#endif

#endif // CGGLOBL_H
