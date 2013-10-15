# Microsoft Developer Studio Project File - Name="DatabaseServer" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=DatabaseServer - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "DatabaseServer.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DatabaseServer.mak" CFG="DatabaseServer - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DatabaseServer - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "DatabaseServer - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Neurospace/Program/DatabaseServer", TNSAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "DatabaseServer - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\Output\DatabaseServer\Release"
# PROP Intermediate_Dir "..\..\Output\DatabaseServer\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /Zd /O2 /I "." /I "..\_Network\Net\Include" /I "..\_Network\Misc\Include" /I "..\_Network\Mcl\Include" /I "..\_network" /I "..\_DirectX" /I "..\_Common" /I "..\..\resource" /I "..\_database" /I "..\_Interface" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FAcs /Fr /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "NDEBUG"
# ADD RSC /l 0x412 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 ws2_32.lib winmm.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /map /debug /machine:I386 /nodefaultlib:"msvcrt.lib" /libpath:"..\_Network\Mcl\Lib\Release"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "DatabaseServer - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\Output\DatabaseServer\Debug"
# PROP Intermediate_Dir "..\..\Output\DatabaseServer\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "." /I "..\_Network\Net\Include" /I "..\_Network\Misc\Include" /I "..\_Network\Mcl\Include" /I "..\_network" /I "..\_DirectX" /I "..\_Common" /I "..\..\resource" /I "..\_database" /I "..\_Interface" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "_DEBUG"
# ADD RSC /l 0x412 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ws2_32.lib winmm.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"MSVCRT.lib" /pdbtype:sept /libpath:"..\_Network\Mcl\Lib\Release"

!ENDIF 

# Begin Target

# Name "DatabaseServer - Win32 Release"
# Name "DatabaseServer - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\_Database\AccountCacheMgr.cpp
# End Source File
# Begin Source File

SOURCE=..\_Database\AccountCacheMgr.h
# End Source File
# Begin Source File

SOURCE=.\AccountTimeLimitDBCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\CampusDBCtrl.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\Continent.cpp
# End Source File
# Begin Source File

SOURCE=.\couplehelper.cpp
# End Source File
# Begin Source File

SOURCE=.\DatabaseServer.cpp
# End Source File
# Begin Source File

SOURCE=.\dbcontroller.cpp
# End Source File
# Begin Source File

SOURCE=.\dpaccountclient.cpp
# End Source File
# Begin Source File

SOURCE=.\DPCoreSrvr.cpp
# End Source File
# Begin Source File

SOURCE=.\DPLoginSrvr.cpp
# End Source File
# Begin Source File

SOURCE=.\dptrans.cpp
# End Source File
# Begin Source File

SOURCE=.\GuildHouseDBCtrl.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\Housing.cpp
# End Source File
# Begin Source File

SOURCE=.\HousingDBCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\lordskillexecutable.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\memtrace.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\pocket.cpp
# End Source File
# Begin Source File

SOURCE=.\Project.cpp
# End Source File
# Begin Source File

SOURCE=.\QuizDBCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\RainbowRaceDBCtrl.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\rtmessenger.cpp
# End Source File
# Begin Source File

SOURCE=.\SecretRoomDBMng.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=..\_Common\Tax.cpp
# End Source File
# Begin Source File

SOURCE=.\tlord.cpp
# End Source File
# Begin Source File

SOURCE=..\_common\vutil.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AccountTimeLimitDBCtrl.h
# End Source File
# Begin Source File

SOURCE=.\CampusDBCtrl.h
# End Source File
# Begin Source File

SOURCE=.\couplehelper.h
# End Source File
# Begin Source File

SOURCE=.\DatabaseServer.h
# End Source File
# Begin Source File

SOURCE=.\dbcontroller.h
# End Source File
# Begin Source File

SOURCE=.\DefineConMode.h
# End Source File
# Begin Source File

SOURCE=.\dpaccountclient.h
# End Source File
# Begin Source File

SOURCE=.\DPCoreSrvr.h
# End Source File
# Begin Source File

SOURCE=.\DPLoginSrvr.h
# End Source File
# Begin Source File

SOURCE=.\dptrans.h
# End Source File
# Begin Source File

SOURCE=.\GuildHouseDBCtrl.h
# End Source File
# Begin Source File

SOURCE=..\_Common\Housing.h
# End Source File
# Begin Source File

SOURCE=.\HousingDBCtrl.h
# End Source File
# Begin Source File

SOURCE=.\lordskillexecutable.h
# End Source File
# Begin Source File

SOURCE=..\_Common\memtrace.h
# End Source File
# Begin Source File

SOURCE=..\_Common\pocket.h
# End Source File
# Begin Source File

SOURCE=.\project.h
# End Source File
# Begin Source File

SOURCE=.\QuizDBCtrl.h
# End Source File
# Begin Source File

SOURCE=.\RainbowRaceDBCtrl.h
# End Source File
# Begin Source File

SOURCE=..\_Common\rtmessenger.h
# End Source File
# Begin Source File

SOURCE=.\SecretRoomDBMng.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\tlord.h
# End Source File
# Begin Source File

SOURCE=.\VersionCommon.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\DatabaseServer.ico
# End Source File
# Begin Source File

SOURCE=.\databaseserver.ini
# End Source File
# Begin Source File

SOURCE=.\DatabaseServer.rc
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\small.ico
# End Source File
# End Group
# Begin Group "Network Files"

# PROP Default_Filter ""
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
# Begin Group "NetObjects"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\_network\objects\Obj.cpp
# End Source File
# Begin Source File

SOURCE=..\_network\objects\Obj.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\_network\CmnHdr.h
# End Source File
# Begin Source File

SOURCE=..\_network\MsgHdr.h
# End Source File
# End Group
# Begin Group "Neuz Files"

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
# Begin Source File

SOURCE=..\_Common\script.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\script.h
# End Source File
# End Group
# Begin Group "Objects"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\_Common\ObjSerialize.cpp
# End Source File
# Begin Source File

SOURCE=..\_common\ObjSerializeOpt.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\pet.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\pet.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\_Common\authorization.h
# End Source File
# Begin Source File

SOURCE=..\_Common\buff.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\buff.h
# End Source File
# Begin Source File

SOURCE=..\_common\buyinginfo.h
# End Source File
# Begin Source File

SOURCE=..\_Common\Campus.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\Campus.h
# End Source File
# Begin Source File

SOURCE=..\_Common\couple.cpp
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

SOURCE=..\_Common\EventLua.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\EventLua.h
# End Source File
# Begin Source File

SOURCE=..\_common\eveschool.cpp
# End Source File
# Begin Source File

SOURCE=..\_common\eveschool.h
# End Source File
# Begin Source File

SOURCE=..\_Common\file.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\file.h
# End Source File
# Begin Source File

SOURCE=..\_Common\FunnyCoin.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\FunnyCoin.h
# End Source File
# Begin Source File

SOURCE=..\_common\guild.cpp
# End Source File
# Begin Source File

SOURCE=..\_common\guild.h
# End Source File
# Begin Source File

SOURCE=..\_Common\GuildCombat1to1.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\GuildCombat1to1.h
# End Source File
# Begin Source File

SOURCE=..\_Common\GuildHouse.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\GuildHouse.h
# End Source File
# Begin Source File

SOURCE=..\_common\guildquest.cpp
# End Source File
# Begin Source File

SOURCE=..\_common\guildquest.h
# End Source File
# Begin Source File

SOURCE=..\_common\guildwar.cpp
# End Source File
# Begin Source File

SOURCE=..\_common\guildwar.h
# End Source File
# Begin Source File

SOURCE=..\_Common\honor.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\lord.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\lord.h
# End Source File
# Begin Source File

SOURCE=..\_Common\lordskill.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\lordskill.h
# End Source File
# Begin Source File

SOURCE=..\_Common\LuaBase.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\LuaBase.h
# End Source File
# Begin Source File

SOURCE=..\_Common\messenger.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\messenger.h
# End Source File
# Begin Source File

SOURCE=..\_common\party.h
# End Source File
# Begin Source File

SOURCE=..\_common\path.h
# End Source File
# Begin Source File

SOURCE=..\_Common\piercing.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\Piercing.h
# End Source File
# Begin Source File

SOURCE=..\_Common\playerdata.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\playerdata.h
# End Source File
# Begin Source File

SOURCE=..\_Common\post.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\post.h
# End Source File
# Begin Source File

SOURCE=..\_Common\ProjectCmn.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\ProjectCmn.h
# End Source File
# Begin Source File

SOURCE=..\_Common\Quiz.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\Quiz.h
# End Source File
# Begin Source File

SOURCE=..\_Common\serialnumber.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\serialnumber.h
# End Source File
# Begin Source File

SOURCE=..\_Common\SkillInfluence.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\SkillInfluence.h
# End Source File
# Begin Source File

SOURCE=..\_Common\spevent.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\spevent.h
# End Source File
# Begin Source File

SOURCE=..\_Common\timer.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\timer.h
# End Source File
# End Group
# Begin Group "Interface"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\_Interface\UserTaskBar.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\UserTaskBar.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\_Common\DefineCommon.h
# End Source File
# Begin Source File

SOURCE=..\_Common\langman.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\langman.h
# End Source File
# Begin Source File

SOURCE=..\_Common\patch.h
# End Source File
# Begin Source File

SOURCE=..\_Common\path.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\xutil.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\xutil.h
# End Source File
# End Group
# Begin Group "Database Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\_Database\DBBackEndSystem.cpp
# End Source File
# Begin Source File

SOURCE=..\_Database\DBConvert.cpp
# End Source File
# Begin Source File

SOURCE=..\_Database\DBConvertNew.cpp
# End Source File
# Begin Source File

SOURCE=..\_Database\DbItemUpdate.cpp
# End Source File
# Begin Source File

SOURCE=..\_database\DbManager.cpp
# End Source File
# Begin Source File

SOURCE=..\_database\DbManager.h
# End Source File
# Begin Source File

SOURCE=..\_Database\DbManagerFun.cpp
# End Source File
# Begin Source File

SOURCE=..\_Database\DbManagerJoin.cpp
# End Source File
# Begin Source File

SOURCE=..\_Database\DbManagerLog.cpp
# End Source File
# Begin Source File

SOURCE=..\_Database\DbManagerSave.cpp
# End Source File
# Begin Source File

SOURCE=..\_Database\des.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\_Database\des.h
# End Source File
# Begin Source File

SOURCE=..\_database\Query.cpp
# End Source File
# Begin Source File

SOURCE=..\_database\Query.h
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
# End Target
# End Project
# Section DatabaseServer : {BEF6E003-A874-101A-8BBA-00AA00300CAB}
# 	2:5:Class:COleFont
# 	2:10:HeaderFile:font.h
# 	2:8:ImplFile:font.cpp
# End Section
# Section DatabaseServer : {CDE57A52-8B86-11D0-B3C6-00A0C90AEA82}
# 	2:5:Class:CSelBookmarks
# 	2:10:HeaderFile:selbookmarks.h
# 	2:8:ImplFile:selbookmarks.cpp
# End Section
# Section DatabaseServer : {7BF80981-BF32-101A-8BBB-00AA00300CAB}
# 	2:5:Class:CPicture
# 	2:10:HeaderFile:picture.h
# 	2:8:ImplFile:picture.cpp
# End Section
# Section DatabaseServer : {CDE57A41-8B86-11D0-B3C6-00A0C90AEA82}
# 	2:5:Class:CDataGrid
# 	2:10:HeaderFile:datagrid.h
# 	2:8:ImplFile:datagrid.cpp
# End Section
# Section DatabaseServer : {CDE57A50-8B86-11D0-B3C6-00A0C90AEA82}
# 	2:5:Class:CColumns
# 	2:10:HeaderFile:columns.h
# 	2:8:ImplFile:columns.cpp
# End Section
# Section DatabaseServer : {E675F3F0-91B5-11D0-9484-00A0C91110ED}
# 	2:5:Class:CDataFormatDisp
# 	2:10:HeaderFile:dataformatdisp.h
# 	2:8:ImplFile:dataformatdisp.cpp
# End Section
# Section DatabaseServer : {CDE57A54-8B86-11D0-B3C6-00A0C90AEA82}
# 	2:5:Class:CSplit
# 	2:10:HeaderFile:split.h
# 	2:8:ImplFile:split.cpp
# End Section
# Section DatabaseServer : {99FF4676-FFC3-11D0-BD02-00C04FC2FB86}
# 	2:5:Class:CStdDataFormatsDisp
# 	2:10:HeaderFile:stddataformatsdisp.h
# 	2:8:ImplFile:stddataformatsdisp.cpp
# End Section
# Section DatabaseServer : {CDE57A4F-8B86-11D0-B3C6-00A0C90AEA82}
# 	2:5:Class:CColumn
# 	2:10:HeaderFile:column.h
# 	2:8:ImplFile:column.cpp
# End Section
# Section DatabaseServer : {CDE57A43-8B86-11D0-B3C6-00A0C90AEA82}
# 	2:21:DefaultSinkHeaderFile:datagrid.h
# 	2:16:DefaultSinkClass:CDataGrid
# End Section
# Section DatabaseServer : {CDE57A53-8B86-11D0-B3C6-00A0C90AEA82}
# 	2:5:Class:CSplits
# 	2:10:HeaderFile:splits.h
# 	2:8:ImplFile:splits.cpp
# End Section
