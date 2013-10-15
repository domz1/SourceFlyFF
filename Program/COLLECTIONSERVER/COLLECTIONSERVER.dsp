# Microsoft Developer Studio Project File - Name="COLLECTIONSERVER" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=COLLECTIONSERVER - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "COLLECTIONSERVER.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "COLLECTIONSERVER.mak" CFG="COLLECTIONSERVER - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "COLLECTIONSERVER - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "COLLECTIONSERVER - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "COLLECTIONSERVER - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\..\resource" /I "..\_network" /I "..\_Network\Net\Include" /I "..\_Network\Misc\Include" /I "." /I "..\_Network\Mcl\Include" /I "..\_DirectX" /I "..\_Common" /I "..\_database" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "NDEBUG"
# ADD RSC /l 0x412 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib d3dx9.lib winmm.lib /nologo /subsystem:windows /map /debug /machine:I386 /libpath:"..\_Network\Mcl\Lib\Release"

!ELSEIF  "$(CFG)" == "COLLECTIONSERVER - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\resource" /I "..\_network" /I "..\_Network\Net\Include" /I "..\_Network\Misc\Include" /I "." /I "..\_Network\Mcl\Include" /I "..\_DirectX" /I "..\_Common" /I "..\_database" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "_DEBUG"
# ADD RSC /l 0x412 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib d3dx9.lib winmm.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"MSVCRT" /pdbtype:sept /libpath:"..\_Network\Mcl\Lib\Release"

!ENDIF 

# Begin Target

# Name "COLLECTIONSERVER - Win32 Release"
# Name "COLLECTIONSERVER - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\COLLECTIONSERVER.cpp
# End Source File
# Begin Source File

SOURCE=.\COLLECTIONSERVER.rc
# End Source File
# Begin Source File

SOURCE=.\DbManager.cpp
# End Source File
# Begin Source File

SOURCE=.\DPCollectionSrvr.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\memtrace.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\COLLECTIONSERVER.h
# End Source File
# Begin Source File

SOURCE=.\DbManager.h
# End Source File
# Begin Source File

SOURCE=.\DPCollectionSrvr.h
# End Source File
# Begin Source File

SOURCE=..\_Common\memtrace.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\COLLECTIONSERVER.ico
# End Source File
# Begin Source File

SOURCE=.\small.ico
# End Source File
# End Group
# Begin Group "Network Files"

# PROP Default_Filter ""
# Begin Group "Mcl"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\_Network\Mcl\Include\CMcl.h
# End Source File
# Begin Source File

SOURCE=..\_Network\Mcl\Include\CMclAutoLock.h
# End Source File
# Begin Source File

SOURCE=..\_Network\Mcl\Include\CMclCritSec.h
# End Source File
# End Group
# Begin Group "Net"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\_Network\Net\Src\buffer.cpp
# End Source File
# Begin Source File

SOURCE=..\_Network\Net\Include\buffer.h
# End Source File
# Begin Source File

SOURCE=..\_Network\Net\Src\clientsock.cpp
# End Source File
# Begin Source File

SOURCE=..\_Network\Net\Include\clientsock.h
# End Source File
# Begin Source File

SOURCE=..\_Network\Net\Src\clientsocke.cpp
# End Source File
# Begin Source File

SOURCE=..\_Network\Net\Include\clientsocke.h
# End Source File
# Begin Source File

SOURCE=..\_Network\Net\Src\crc.cpp
# End Source File
# Begin Source File

SOURCE=..\_Network\Net\Include\crc.h
# End Source File
# Begin Source File

SOURCE=..\_Network\Net\Src\dpmng.cpp
# End Source File
# Begin Source File

SOURCE=..\_Network\Net\Include\dpmng.h
# End Source File
# Begin Source File

SOURCE=..\_Network\Net\Src\dpsock.cpp
# End Source File
# Begin Source File

SOURCE=..\_Network\Net\Include\dpsock.h
# End Source File
# Begin Source File

SOURCE=..\_Network\Net\Src\serversock.cpp
# End Source File
# Begin Source File

SOURCE=..\_Network\Net\Include\serversock.h
# End Source File
# Begin Source File

SOURCE=..\_Network\Net\Src\serversocke.cpp
# End Source File
# Begin Source File

SOURCE=..\_Network\Net\Include\serversocke.h
# End Source File
# Begin Source File

SOURCE=..\_Network\Net\Src\sock.cpp
# End Source File
# Begin Source File

SOURCE=..\_Network\Net\Include\sock.h
# End Source File
# End Group
# Begin Group "Misc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\_Network\Misc\Src\ar.cpp
# End Source File
# Begin Source File

SOURCE=..\_Network\Misc\Include\ar.h
# End Source File
# Begin Source File

SOURCE=..\_Network\Misc\Src\DoS.cpp
# End Source File
# Begin Source File

SOURCE=..\_Network\Misc\Include\DoS.h
# End Source File
# Begin Source File

SOURCE=..\_Network\Misc\Src\dos2.cpp
# End Source File
# Begin Source File

SOURCE=..\_Network\Misc\Include\dos2.h
# End Source File
# Begin Source File

SOURCE=..\_Network\Misc\Src\heapmng.cpp
# End Source File
# Begin Source File

SOURCE=..\_Network\Misc\Include\heapMng.h
# End Source File
# Begin Source File

SOURCE=..\_Network\Misc\Include\mempool.h
# End Source File
# Begin Source File

SOURCE=..\_Network\Misc\Include\MEMPOOLER.H
# End Source File
# Begin Source File

SOURCE=..\_Network\Misc\Include\Misc.h
# End Source File
# Begin Source File

SOURCE=..\_Network\Misc\Include\mymap.h
# End Source File
# Begin Source File

SOURCE=..\_Network\Misc\Src\mytrace.cpp
# End Source File
# Begin Source File

SOURCE=..\_Network\Misc\Include\mytrace.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\_Network\CmnHdr.h
# End Source File
# Begin Source File

SOURCE=..\_Network\MsgHdr.h
# End Source File
# End Group
# Begin Group "Neuz Common Files"

# PROP Default_Filter ""
# Begin Group "DX"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\_DirectX\dxutil.cpp
# End Source File
# Begin Source File

SOURCE=..\_DirectX\dxutil.h
# End Source File
# End Group
# Begin Group "Script"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\_Common\scanner.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\scanner.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\_Common\data.h
# End Source File
# Begin Source File

SOURCE=..\_Common\debug.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\debug.h
# End Source File
# Begin Source File

SOURCE=..\_Common\DefineCommon.h
# End Source File
# Begin Source File

SOURCE=..\_Common\file.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\file.h
# End Source File
# Begin Source File

SOURCE=..\_Common\timer.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\timer.h
# End Source File
# Begin Source File

SOURCE=..\_Common\vutil.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\vutil.h
# End Source File
# Begin Source File

SOURCE=..\_Common\xUtil.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\xUtil.h
# End Source File
# End Group
# Begin Group "ODBC Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\_Database\des.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\_Database\des.h
# End Source File
# Begin Source File

SOURCE=..\_Database\Query.cpp
# End Source File
# Begin Source File

SOURCE=..\_Database\Query.h
# End Source File
# End Group
# Begin Group "Exception"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\_UnhandledException\exceptionhandler.cpp
# End Source File
# Begin Source File

SOURCE=..\_UnhandledException\ExceptionHandler.h
# End Source File
# Begin Source File

SOURCE=..\_UnhandledException\neuzd.lib
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
