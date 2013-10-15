# Microsoft Developer Studio Project File - Name="CoreServer" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=CORESERVER - WIN32 DEBUG
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "CoreServer.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "CoreServer.mak" CFG="CORESERVER - WIN32 DEBUG"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "CoreServer - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "CoreServer - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Neurospace/Program/CoreServer", MISAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "CoreServer - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\Output\CoreServer\Release"
# PROP Intermediate_Dir "..\..\Output\CoreServer\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /Zd /O2 /I "." /I "..\_Network\Net\Include" /I "..\_Network\Misc\Include" /I "..\_Network\Mcl\Include" /I "..\_network" /I "..\_DirectX" /I "..\_Common" /I "..\..\resource" /I "..\_Interface" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "NDEBUG"
# ADD RSC /l 0x412 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib winmm.lib pdh.lib VERSION.LIB /nologo /subsystem:windows /map /debug /machine:I386 /nodefaultlib:"msvcrt.lib" /libpath:"..\_Network\Mcl\Lib\Release"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "CoreServer - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\Output\CoreServer\Debug"
# PROP Intermediate_Dir "..\..\Output\CoreServer\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "." /I "..\_Network\Net\Include" /I "..\_Network\Misc\Include" /I "..\_Network\Mcl\Include" /I "..\_network" /I "..\_DirectX" /I "..\_Common" /I "..\..\resource" /I "..\_Interface" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "_DEBUG"
# ADD RSC /l 0x412 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib winmm.lib pdh.lib VERSION.LIB /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"msvcrt.lib" /pdbtype:sept /libpath:"..\_Network\Mcl\Lib\Release"

!ENDIF 

# Begin Target

# Name "CoreServer - Win32 Release"
# Name "CoreServer - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\CoreServer.cpp
# End Source File
# Begin Source File

SOURCE=.\CoreServer.rc
# End Source File
# Begin Source File

SOURCE=.\DPCacheSrvr.cpp
# End Source File
# Begin Source File

SOURCE=.\DPCoreSrvr.cpp
# End Source File
# Begin Source File

SOURCE=.\DPDatabaseClient.cpp
# End Source File
# Begin Source File

SOURCE=.\dploginsrvr.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\memtrace.cpp
# End Source File
# Begin Source File

SOURCE=.\Player.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\playerdata.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\rtmessenger.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\WantedList.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\CoreServer.h
# End Source File
# Begin Source File

SOURCE=.\DPCacheSrvr.h
# End Source File
# Begin Source File

SOURCE=.\DPCoreSrvr.h
# End Source File
# Begin Source File

SOURCE=.\DPDatabaseClient.h
# End Source File
# Begin Source File

SOURCE=.\dploginsrvr.h
# End Source File
# Begin Source File

SOURCE=..\_Common\memtrace.h
# End Source File
# Begin Source File

SOURCE=.\Player.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\VersionCommon.h
# End Source File
# Begin Source File

SOURCE=.\WantedList.h
# End Source File
# End Group
# Begin Group "Exception"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\_UnhandledException\exceptionhandler.cpp
# End Source File
# Begin Source File

SOURCE=..\_UnhandledException\exceptionhandler.h
# End Source File
# Begin Source File

SOURCE=..\_UnhandledException\neuzd.lib
# End Source File
# End Group
# Begin Group "Neuz Common Files"

# PROP Default_Filter ""
# Begin Group "DirectX Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\_DirectX\dxutil.cpp
# End Source File
# Begin Source File

SOURCE=..\_DirectX\dxutil.h
# End Source File
# End Group
# Begin Group "Common Files"

# PROP Default_Filter ""
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

SOURCE=..\_common\buyinginfo.h
# End Source File
# Begin Source File

SOURCE=..\_Common\data.h
# End Source File
# Begin Source File

SOURCE=..\_common\debug.cpp
# End Source File
# Begin Source File

SOURCE=..\_common\debug.h
# End Source File
# Begin Source File

SOURCE=..\_Common\Environment.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\Environment.h
# End Source File
# Begin Source File

SOURCE=..\_common\file.cpp
# End Source File
# Begin Source File

SOURCE=..\_common\file.h
# End Source File
# Begin Source File

SOURCE=..\_common\guild.cpp
# End Source File
# Begin Source File

SOURCE=..\_common\guild.h
# End Source File
# Begin Source File

SOURCE=..\_common\guildwar.cpp
# End Source File
# Begin Source File

SOURCE=..\_common\guildwar.h
# End Source File
# Begin Source File

SOURCE=..\_Common\InstanceDungeonBase.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\InstanceDungeonBase.h
# End Source File
# Begin Source File

SOURCE=..\_Common\InstanceDungeonParty.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\InstanceDungeonParty.h
# End Source File
# Begin Source File

SOURCE=..\_Common\messenger.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\messenger.h
# End Source File
# Begin Source File

SOURCE=..\_Common\party.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\party.h
# End Source File
# Begin Source File

SOURCE=..\databaseserver\project.h
# End Source File
# Begin Source File

SOURCE=..\_common\projectcmn.h
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

SOURCE=..\_common\xutil.cpp
# End Source File
# Begin Source File

SOURCE=..\_common\xutil.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\_common\authorization.h
# End Source File
# Begin Source File

SOURCE=..\_Common\DefineCommon.h
# End Source File
# End Group
# Begin Group "Network Files"

# PROP Default_Filter ""
# Begin Group "Net"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\_network\net\src\buffer.cpp
# End Source File
# Begin Source File

SOURCE=..\_network\net\include\buffer.h
# End Source File
# Begin Source File

SOURCE=..\_network\net\src\clientsock.cpp
# End Source File
# Begin Source File

SOURCE=..\_network\net\include\clientsock.h
# End Source File
# Begin Source File

SOURCE=..\_network\net\src\clientsocke.cpp
# End Source File
# Begin Source File

SOURCE=..\_network\net\include\clientsocke.h
# End Source File
# Begin Source File

SOURCE=..\_network\net\src\dpmng.cpp
# End Source File
# Begin Source File

SOURCE=..\_network\net\include\dpmng.h
# End Source File
# Begin Source File

SOURCE=..\_network\net\src\dpsock.cpp
# End Source File
# Begin Source File

SOURCE=..\_network\net\include\dpsock.h
# End Source File
# Begin Source File

SOURCE=..\_network\net\src\serversock.cpp
# End Source File
# Begin Source File

SOURCE=..\_network\net\include\serversock.h
# End Source File
# Begin Source File

SOURCE=..\_network\net\src\serversocke.cpp
# End Source File
# Begin Source File

SOURCE=..\_network\net\include\serversocke.h
# End Source File
# Begin Source File

SOURCE=..\_network\net\src\sock.cpp
# End Source File
# Begin Source File

SOURCE=..\_network\net\include\sock.h
# End Source File
# End Group
# Begin Group "Mcl"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\_network\mcl\include\cmcl.h
# End Source File
# Begin Source File

SOURCE=..\_network\mcl\include\cmclautolock.h
# End Source File
# Begin Source File

SOURCE=..\_network\mcl\include\cmclcritsec.h
# End Source File
# End Group
# Begin Group "Misc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\_network\misc\src\ar.cpp
# End Source File
# Begin Source File

SOURCE=..\_network\misc\include\ar.h
# End Source File
# Begin Source File

SOURCE=..\_network\misc\include\chassert.h
# End Source File
# Begin Source File

SOURCE=..\_Network\Misc\Src\heapmng.cpp
# End Source File
# Begin Source File

SOURCE=..\_network\misc\include\heapmng.h
# End Source File
# Begin Source File

SOURCE=..\_network\misc\include\mempool.h
# End Source File
# Begin Source File

SOURCE=..\_network\misc\include\MEMPOOLER.H
# End Source File
# Begin Source File

SOURCE=..\_network\misc\include\memspy.h
# End Source File
# Begin Source File

SOURCE=..\_network\misc\include\Misc.h
# End Source File
# Begin Source File

SOURCE=..\_network\misc\include\MyMap.h
# End Source File
# Begin Source File

SOURCE=..\_network\misc\src\MyTrace.cpp
# End Source File
# Begin Source File

SOURCE=..\_network\misc\include\MyTrace.h
# End Source File
# Begin Source File

SOURCE=..\_network\misc\include\ObjMap.h
# End Source File
# Begin Source File

SOURCE=..\_network\misc\src\ServerDesc.cpp
# End Source File
# Begin Source File

SOURCE=..\_network\misc\include\ServerDesc.h
# End Source File
# Begin Source File

SOURCE=..\_network\misc\include\tickcount.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\_network\CmnHdr.h
# End Source File
# Begin Source File

SOURCE=..\_network\MsgHdr.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=".\BoA!!!.txt"
# End Source File
# Begin Source File

SOURCE=.\CoreServer.ico
# End Source File
# Begin Source File

SOURCE=.\CoreServer.ini
# End Source File
# Begin Source File

SOURCE=.\small.ico
# End Source File
# End Group
# End Target
# End Project
