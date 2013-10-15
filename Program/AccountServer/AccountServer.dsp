# Microsoft Developer Studio Project File - Name="AccountServer" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=ACCOUNTSERVER - WIN32 DEBUG
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "AccountServer.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AccountServer.mak" CFG="ACCOUNTSERVER - WIN32 DEBUG"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AccountServer - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "AccountServer - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Neurospace/Program/AccountServer", ZPZBAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "AccountServer - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\Output\AccountServer\Release"
# PROP Intermediate_Dir "..\..\Output\AccountServer\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /Zd /O2 /I "..\..\resource" /I "..\_network" /I "..\_Network\Net2\Include" /I "..\_Network\Misc\Include" /I "." /I "..\_Network\Mcl\Include" /I "..\_DirectX" /I "..\_Common" /I "..\_database" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fr /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "NDEBUG"
# ADD RSC /l 0x412 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 ws2_32.lib winmm.lib d3dx9.lib odbc32.lib odbccp32.lib VERSION.LIB /nologo /subsystem:windows /map /debug /machine:I386
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "AccountServer - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\Output\AccountServer\Debug"
# PROP Intermediate_Dir "..\..\Output\AccountServer\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\resource" /I "..\_network" /I "..\_Network\Net2\Include" /I "..\_Network\Misc\Include" /I "." /I "..\_Network\Mcl\Include" /I "..\_DirectX" /I "..\_Common" /I "..\_database" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "_DEBUG"
# ADD RSC /l 0x412 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib d3dx9.lib ws2_32.lib winmm.lib VERSION.LIB /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"MSVCRT" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "AccountServer - Win32 Release"
# Name "AccountServer - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\account.cpp
# End Source File
# Begin Source File

SOURCE=.\AccountServer.cpp
# End Source File
# Begin Source File

SOURCE=.\AccountServer.rc
# End Source File
# Begin Source File

SOURCE=.\DBManager.cpp
# End Source File
# Begin Source File

SOURCE=.\dpdbsrvr.cpp
# End Source File
# Begin Source File

SOURCE=.\dpsrvr.cpp
# End Source File
# Begin Source File

SOURCE=.\dpwldsrvr.cpp
# End Source File
# Begin Source File

SOURCE=.\globalgiftbox.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\account.h
# End Source File
# Begin Source File

SOURCE=.\AccountServer.h
# End Source File
# Begin Source File

SOURCE=.\DBManager.h
# End Source File
# Begin Source File

SOURCE=.\dpdbsrvr.h
# End Source File
# Begin Source File

SOURCE=.\dpsrvr.h
# End Source File
# Begin Source File

SOURCE=.\dpwldsrvr.h
# End Source File
# Begin Source File

SOURCE=.\globalgiftbox.h
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
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\AccountServer.ico
# End Source File
# Begin Source File

SOURCE=.\AccountServer.ini
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

SOURCE=..\_Network\Misc\Src\heapmng.cpp
# End Source File
# Begin Source File

SOURCE=..\_network\misc\include\heapmng.h
# End Source File
# Begin Source File

SOURCE=..\_network\misc\include\mempooler.h
# End Source File
# Begin Source File

SOURCE=..\_network\misc\include\misc.h
# End Source File
# Begin Source File

SOURCE=..\_network\misc\include\mymap.h
# End Source File
# Begin Source File

SOURCE=..\_network\misc\src\mytrace.cpp
# End Source File
# Begin Source File

SOURCE=..\_network\misc\include\mytrace.h
# End Source File
# End Group
# Begin Group "Net"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\_network\net2\src\buffer.cpp
# End Source File
# Begin Source File

SOURCE=..\_network\net2\include\buffer.h
# End Source File
# Begin Source File

SOURCE=..\_network\net2\include\clientsock.h
# End Source File
# Begin Source File

SOURCE=..\_network\net2\include\clientsocke.h
# End Source File
# Begin Source File

SOURCE=..\_network\net2\src\crc.cpp
# End Source File
# Begin Source File

SOURCE=..\_network\net2\include\crc.h
# End Source File
# Begin Source File

SOURCE=..\_network\net2\include\dpmng.h
# End Source File
# Begin Source File

SOURCE=..\_network\net2\include\dpsock.h
# End Source File
# Begin Source File

SOURCE=..\_network\net2\include\serversock.h
# End Source File
# Begin Source File

SOURCE=..\_network\net2\include\serversocke.h
# End Source File
# Begin Source File

SOURCE=..\_network\net2\src\sock.cpp
# End Source File
# Begin Source File

SOURCE=..\_network\net2\include\sock.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\_network\cmnhdr.h
# End Source File
# Begin Source File

SOURCE=..\_network\msghdr.h
# End Source File
# End Group
# Begin Group "Neuz Files"

# PROP Default_Filter ""
# Begin Group "DX"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\_directx\dxutil.cpp
# End Source File
# Begin Source File

SOURCE=..\_directx\dxutil.h
# End Source File
# End Group
# Begin Group "Cmn"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\_common\buyinginfo.h
# End Source File
# Begin Source File

SOURCE=..\_common\data.h
# End Source File
# Begin Source File

SOURCE=..\_common\debug.cpp
# End Source File
# Begin Source File

SOURCE=..\_common\debug.h
# End Source File
# Begin Source File

SOURCE=..\_common\file.cpp
# End Source File
# Begin Source File

SOURCE=..\_common\file.h
# End Source File
# Begin Source File

SOURCE=..\_Common\memtrace.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\memtrace.h
# End Source File
# Begin Source File

SOURCE=..\_common\scanner.cpp
# End Source File
# Begin Source File

SOURCE=..\_common\scanner.h
# End Source File
# Begin Source File

SOURCE=..\_common\timer.cpp
# End Source File
# Begin Source File

SOURCE=..\_common\timer.h
# End Source File
# Begin Source File

SOURCE=..\_common\vutil.cpp
# End Source File
# Begin Source File

SOURCE=..\_common\vutil.h
# End Source File
# Begin Source File

SOURCE=..\_common\xutil.cpp
# End Source File
# Begin Source File

SOURCE=..\_common\xutil.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\_Common\DefineCommon.h
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

SOURCE=..\_UnhandledException\exceptionhandler.h
# End Source File
# Begin Source File

SOURCE=..\_UnhandledException\neuzd.lib
# End Source File
# End Group
# Begin Group "Billing"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\BillingMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\BillingMgr.h
# End Source File
# Begin Source File

SOURCE=.\BillingMgrJP.cpp
# End Source File
# Begin Source File

SOURCE=.\BillingMgrJP.h
# End Source File
# Begin Source File

SOURCE=.\BillingMgrJP2.cpp
# End Source File
# Begin Source File

SOURCE=.\BillingMgrJP2.h
# End Source File
# Begin Source File

SOURCE=.\BillingMgrTH.cpp
# End Source File
# Begin Source File

SOURCE=.\BillingMgrTH.h
# End Source File
# Begin Source File

SOURCE=.\BillingMgrTW.cpp
# End Source File
# Begin Source File

SOURCE=.\BillingMgrTW.h
# End Source File
# Begin Source File

SOURCE=.\dpadbill.cpp
# End Source File
# Begin Source File

SOURCE=.\dpadbill.h
# End Source File
# Begin Source File

SOURCE=.\dpBilling.cpp
# End Source File
# Begin Source File

SOURCE=.\dpBilling.h
# End Source File
# Begin Source File

SOURCE=.\dpBillingTH.cpp
# End Source File
# Begin Source File

SOURCE=.\dpBillingTH.h
# End Source File
# End Group
# End Target
# End Project
