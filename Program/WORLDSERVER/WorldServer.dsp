# Microsoft Developer Studio Project File - Name="WorldServer" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=WorldServer - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "WorldServer.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "WorldServer.mak" CFG="WorldServer - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "WorldServer - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "WorldServer - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Neurospace/Program/WorldServer", CDSAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "WorldServer - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\Output\WorldServer\Release"
# PROP Intermediate_Dir "..\..\Output\WorldServer\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /Zi /O2 /I "..\_Network\Net\Include" /I "..\_Network\Misc\Include" /I "." /I "..\_Network\Mcl\Include" /I "..\_DirectX" /I "..\..\resource" /I "..\_Common" /I "..\_network" /I "..\_Interface" /I "..\_AIInterface" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FAcs /Fr /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "NDEBUG"
# ADD RSC /l 0x412 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 d3dx9dt.lib d3d9.lib Dxerr9.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib d3dxof.lib dxguid.lib imm32.lib Dsound.lib pdh.lib VERSION.LIB /nologo /subsystem:windows /map /debug /machine:I386 /nodefaultlib:"msvcrt.lib" /libpath:"..\_Network\Mcl\Lib\Release" /OPT:REF
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "WorldServer - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\Output\WorldServer\Debug"
# PROP Intermediate_Dir "..\..\Output\WorldServer\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\_aiinterface" /I "..\_Network\Net\Include" /I "..\_Network\Misc\Include" /I "." /I "..\_Network\Mcl\Include" /I "..\_DirectX" /I "..\..\resource" /I "..\_Common" /I "..\_network" /I "..\_Interface" /I "..\_AIInterface" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "_DEBUG"
# ADD RSC /l 0x412 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 d3dx9dt.lib d3d9.lib Dxerr9.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib d3dxof.lib dxguid.lib imm32.lib Dsound.lib pdh.lib VERSION.LIB /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"msvcrt.lib" /pdbtype:sept /libpath:"..\_Network\Mcl\Lib\Release"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "WorldServer - Win32 Release"
# Name "WorldServer - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AttackArbiter.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\auction.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\Campus.cpp
# End Source File
# Begin Source File

SOURCE=.\CampusHelper.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\couple.cpp
# End Source File
# Begin Source File

SOURCE=.\couplehelper.cpp
# End Source File
# Begin Source File

SOURCE=.\dpaccountclient.cpp
# End Source File
# Begin Source File

SOURCE=.\DPCoreClient.cpp
# End Source File
# Begin Source File

SOURCE=.\DPDatabaseClient.cpp
# End Source File
# Begin Source File

SOURCE=.\DPSrvr.cpp
# End Source File
# Begin Source File

SOURCE=.\DPSrvrLux.cpp
# End Source File
# Begin Source File

SOURCE=..\_common\flyffevent.cpp
# End Source File
# Begin Source File

SOURCE=..\_common\guildquest.cpp
# End Source File
# Begin Source File

SOURCE=.\ItemScript.cpp
# End Source File
# Begin Source File

SOURCE=.\ItemUpgrade.cpp
# End Source File
# Begin Source File

SOURCE=.\layeredlinkmap.cpp
# End Source File
# Begin Source File

SOURCE=.\LinkMap.cpp
# End Source File
# Begin Source File

SOURCE=.\lordskillexecutable.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\memtrace.cpp
# End Source File
# Begin Source File

SOURCE=.\MiniDumper.cpp
# ADD CPP /Yu
# End Source File
# Begin Source File

SOURCE=.\MiniGameArithmetic.cpp
# End Source File
# Begin Source File

SOURCE=.\MiniGameDiceplay.cpp
# End Source File
# Begin Source File

SOURCE=.\MiniGameGawibawibo.cpp
# End Source File
# Begin Source File

SOURCE=.\MiniGameLadder.cpp
# End Source File
# Begin Source File

SOURCE=.\MiniGamePairgame.cpp
# End Source File
# Begin Source File

SOURCE=.\MiniGameStopwatch.cpp
# End Source File
# Begin Source File

SOURCE=.\MiniGameTyping.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\moverbuff.cpp
# End Source File
# Begin Source File

SOURCE=.\MySplashWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\npchecker.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\pocket.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\post.cpp
# End Source File
# Begin Source File

SOURCE=.\rangda.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\rtmessenger.cpp
# End Source File
# Begin Source File

SOURCE=.\SfxHitArray.cpp
# End Source File
# Begin Source File

SOURCE=.\slord.cpp
# End Source File
# Begin Source File

SOURCE=.\Snapshot.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=..\_Common\testcase.cpp
# End Source File
# Begin Source File

SOURCE=.\ThreadMng.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\ticket.cpp
# End Source File
# Begin Source File

SOURCE=.\User.cpp
# End Source File
# Begin Source File

SOURCE=.\UserLux.cpp
# End Source File
# Begin Source File

SOURCE=.\vmlog.cpp
# End Source File
# Begin Source File

SOURCE=.\WantedListSnapshot.cpp
# End Source File
# Begin Source File

SOURCE=.\WantedListSnapshot.h
# End Source File
# Begin Source File

SOURCE=.\WorldDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\WorldServer.cpp
# End Source File
# Begin Source File

SOURCE=.\WorldServer.rc
# End Source File
# Begin Source File

SOURCE=.\WScript.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AttackArbiter.h
# End Source File
# Begin Source File

SOURCE=..\_Common\auction.h
# End Source File
# Begin Source File

SOURCE=..\_Common\Campus.h
# End Source File
# Begin Source File

SOURCE=.\CampusHelper.h
# End Source File
# Begin Source File

SOURCE=..\_Common\couple.h
# End Source File
# Begin Source File

SOURCE=.\couplehelper.h
# End Source File
# Begin Source File

SOURCE=..\_common\dialogmsg.h
# End Source File
# Begin Source File

SOURCE=.\dpaccountclient.h
# End Source File
# Begin Source File

SOURCE=.\DPCoreClient.h
# End Source File
# Begin Source File

SOURCE=.\DPDatabaseClient.h
# End Source File
# Begin Source File

SOURCE=.\DPSrvr.h
# End Source File
# Begin Source File

SOURCE=..\_common\flyffevent.h
# End Source File
# Begin Source File

SOURCE=.\FunctionsInfo.h
# End Source File
# Begin Source File

SOURCE=.\GlobalTime.h
# End Source File
# Begin Source File

SOURCE=..\_common\guildquest.h
# End Source File
# Begin Source File

SOURCE=.\ItemScript.h
# End Source File
# Begin Source File

SOURCE=.\ItemUpgrade.h
# End Source File
# Begin Source File

SOURCE=.\layeredlinkmap.h
# End Source File
# Begin Source File

SOURCE=.\LinkMap.h
# End Source File
# Begin Source File

SOURCE=.\lordskillexecutable.h
# End Source File
# Begin Source File

SOURCE=..\Lua\lua.hpp
# End Source File
# Begin Source File

SOURCE=..\_Common\memtrace.h
# End Source File
# Begin Source File

SOURCE=.\MiniDumper.h
# End Source File
# Begin Source File

SOURCE=.\MiniGameArithmetic.h
# End Source File
# Begin Source File

SOURCE=.\MiniGameDiceplay.h
# End Source File
# Begin Source File

SOURCE=.\MiniGameGawibawibo.h
# End Source File
# Begin Source File

SOURCE=.\MiniGameLadder.h
# End Source File
# Begin Source File

SOURCE=.\MiniGamePairgame.h
# End Source File
# Begin Source File

SOURCE=.\MiniGameStopwatch.h
# End Source File
# Begin Source File

SOURCE=.\MiniGameTyping.h
# End Source File
# Begin Source File

SOURCE=.\MySplashWnd.h
# End Source File
# Begin Source File

SOURCE=.\npchecker.h
# End Source File
# Begin Source File

SOURCE=..\_Common\pocket.h
# End Source File
# Begin Source File

SOURCE=..\_Common\post.h
# End Source File
# Begin Source File

SOURCE=.\rangda.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=..\_Common\rtmessenger.h
# End Source File
# Begin Source File

SOURCE=.\SfxHitArray.h
# End Source File
# Begin Source File

SOURCE=.\slord.h
# End Source File
# Begin Source File

SOURCE=.\Snapshot.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=..\_Common\testcase.h
# End Source File
# Begin Source File

SOURCE=.\ThreadMng.h
# End Source File
# Begin Source File

SOURCE=..\_Common\ticket.h
# End Source File
# Begin Source File

SOURCE=.\User.h
# End Source File
# Begin Source File

SOURCE=.\UserMacro.h
# End Source File
# Begin Source File

SOURCE=.\VersionCommon.h
# End Source File
# Begin Source File

SOURCE=.\vmlog.h
# End Source File
# Begin Source File

SOURCE=.\WorldDialog.h
# End Source File
# Begin Source File

SOURCE=.\WorldServer.h
# End Source File
# Begin Source File

SOURCE=.\WScript.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\cursor1.cur
# End Source File
# Begin Source File

SOURCE=.\MySplash.bmp
# End Source File
# Begin Source File

SOURCE=.\small.ico
# End Source File
# Begin Source File

SOURCE=.\WorldServer.ico
# End Source File
# Begin Source File

SOURCE=.\WorldServer.ini
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

SOURCE=..\_Network\Net\Src\DPMng.cpp
# End Source File
# Begin Source File

SOURCE=..\_Network\Net\Include\DPMng.h
# End Source File
# Begin Source File

SOURCE=..\_Network\Net\Src\DPSock.cpp
# End Source File
# Begin Source File

SOURCE=..\_Network\Net\Include\DPSock.h
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

SOURCE=..\_Network\Net\Src\Sock.cpp
# End Source File
# Begin Source File

SOURCE=..\_Network\Net\Include\Sock.h
# End Source File
# End Group
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
# Begin Group "Misc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\_network\misc\src\ar.cpp
# End Source File
# Begin Source File

SOURCE=..\_Network\Misc\Include\ar.h
# End Source File
# Begin Source File

SOURCE=..\_network\misc\include\chassert.h
# End Source File
# Begin Source File

SOURCE=..\_Network\Misc\Src\heapmng.cpp
# End Source File
# Begin Source File

SOURCE=..\_Network\Misc\Include\HeapMng.h
# End Source File
# Begin Source File

SOURCE=..\_Network\Misc\Include\MemPool.h
# End Source File
# Begin Source File

SOURCE=..\_network\misc\include\MEMPOOLER.H
# End Source File
# Begin Source File

SOURCE=..\_network\misc\include\Misc.h
# End Source File
# Begin Source File

SOURCE=..\_network\misc\include\MyMap.h
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

SOURCE=..\_Network\ErrorCode.h
# End Source File
# Begin Source File

SOURCE=..\_network\MsgHdr.h
# End Source File
# Begin Source File

SOURCE=..\_Network\tools.cpp
# End Source File
# Begin Source File

SOURCE=..\_Network\tools.h
# End Source File
# End Group
# Begin Group "Neuz Common Files"

# PROP Default_Filter ""
# Begin Group "DirectX Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\_directx\d3dapp.h
# End Source File
# Begin Source File

SOURCE=..\_DirectX\d3dres.h
# End Source File
# Begin Source File

SOURCE=..\_DirectX\d3dutil.cpp
# End Source File
# Begin Source File

SOURCE=..\_DirectX\d3dutil.h
# End Source File
# Begin Source File

SOURCE=..\_DirectX\dsutil.h
# End Source File
# Begin Source File

SOURCE=..\_DirectX\dxutil.cpp
# End Source File
# Begin Source File

SOURCE=..\_DirectX\dxutil.h
# End Source File
# End Group
# Begin Group "Common Files"

# PROP Default_Filter ""
# Begin Group "LOD"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\_Common\landscape.h
# End Source File
# Begin Source File

SOURCE=..\_Common\patch.h
# End Source File
# End Group
# Begin Group "Model"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\_Common\Bone.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\Bone.h
# End Source File
# Begin Source File

SOURCE=..\_Common\Model.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\Model.h
# End Source File
# Begin Source File

SOURCE=..\_Common\ModelMng.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\ModelMng.h
# End Source File
# End Group
# Begin Group "Object"

# PROP Default_Filter ""
# Begin Group "Action"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\_aiinterface\action.cpp
# End Source File
# Begin Source File

SOURCE=..\_aiinterface\action.h
# End Source File
# Begin Source File

SOURCE=..\_aiinterface\actionmover.cpp
# End Source File
# Begin Source File

SOURCE=..\_aiinterface\actionmover.h
# End Source File
# Begin Source File

SOURCE=..\_aiinterface\ActionMoverCollision.cpp
# End Source File
# Begin Source File

SOURCE=..\_AIInterface\ActionMoverDmg.cpp
# End Source File
# Begin Source File

SOURCE=..\_aiinterface\ActionMoverMsg.cpp
# End Source File
# Begin Source File

SOURCE=..\_aiinterface\ActionMoverMsg2.cpp
# End Source File
# Begin Source File

SOURCE=..\_aiinterface\ActionMoverState.cpp
# End Source File
# Begin Source File

SOURCE=..\_AIInterface\ActionMoverState2.cpp
# End Source File
# Begin Source File

SOURCE=..\_AIInterface\ActionShip.cpp
# End Source File
# Begin Source File

SOURCE=..\_AIInterface\ActionShip.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\_common\CommonCtrl.cpp
# End Source File
# Begin Source File

SOURCE=..\_common\CommonCtrl.h
# End Source File
# Begin Source File

SOURCE=..\_Common\CreateObj.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\CreateObj.h
# End Source File
# Begin Source File

SOURCE=..\_Common\ctrl.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\ctrl.h
# End Source File
# Begin Source File

SOURCE=..\_Common\CtrlObjs.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\CtrlObjs.h
# End Source File
# Begin Source File

SOURCE=..\_Common\Item.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\Item.h
# End Source File
# Begin Source File

SOURCE=..\_Common\ItemPet.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\modelobject.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\modelobject.h
# End Source File
# Begin Source File

SOURCE=..\_common\mover.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\Mover.h
# End Source File
# Begin Source File

SOURCE=..\_Common\MoverActEvent.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\MoverAttack.cpp
# End Source File
# Begin Source File

SOURCE=..\_common\MoverEquip.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\MoverItem.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\MoverMove.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\MoverMsg.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\MoverMsg.h
# End Source File
# Begin Source File

SOURCE=..\_Common\MoverParam.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\MoverSkill.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\NpcProperty.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\NpcProperty.h
# End Source File
# Begin Source File

SOURCE=..\_Common\Obj.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\Obj.h
# End Source File
# Begin Source File

SOURCE=..\_Common\object3d.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\object3d.h
# End Source File
# Begin Source File

SOURCE=..\_Common\ObjSerialize.cpp
# End Source File
# Begin Source File

SOURCE=..\_common\ObjSerializeOpt.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\PathObj.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\PathObj.h
# End Source File
# Begin Source File

SOURCE=..\_Common\pet.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\pet.h
# End Source File
# Begin Source File

SOURCE=..\_Common\region.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\region.h
# End Source File
# Begin Source File

SOURCE=..\_Common\Ship.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\Ship.h
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
# Begin Source File

SOURCE=..\_Common\script.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\script.h
# End Source File
# Begin Source File

SOURCE=..\_Common\scriptdialog.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\scriptdialog.h
# End Source File
# Begin Source File

SOURCE=..\_Common\ScriptHelper.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\ScriptHelper.h
# End Source File
# Begin Source File

SOURCE=..\_Common\scriptlib.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=..\_Common\accessory.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\accessory.h
# End Source File
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

SOURCE=..\_Common\Chatting.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\Chatting.h
# End Source File
# Begin Source File

SOURCE=..\_Common\collecting.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\collecting.h
# End Source File
# Begin Source File

SOURCE=..\_Common\Continent.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\Continent.h
# End Source File
# Begin Source File

SOURCE=..\_Common\CooltimeMgr.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\CooltimeMgr.h
# End Source File
# Begin Source File

SOURCE=..\_Common\CreateMonster.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\CreateMonster.h
# End Source File
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

SOURCE=..\_Common\DefineCommon.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\DefineCommon.h
# End Source File
# Begin Source File

SOURCE=..\_Common\Environment.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\Environment.h
# End Source File
# Begin Source File

SOURCE=..\_Common\EventLua.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\EventLua.h
# End Source File
# Begin Source File

SOURCE=..\_Common\EventMonster.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\EventMonster.h
# End Source File
# Begin Source File

SOURCE=..\_common\eveschool.cpp
# End Source File
# Begin Source File

SOURCE=..\_common\eveschool.h
# End Source File
# Begin Source File

SOURCE=..\_UnhandledException\exceptionhandler.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\Exchange.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\Exchange.h
# End Source File
# Begin Source File

SOURCE=..\_Common\file.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\file.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\FuncTextCmd.cpp
# End Source File
# Begin Source File

SOURCE=..\_interface\FuncTextCmd.h
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

SOURCE=..\_common\guildwar.cpp
# End Source File
# Begin Source File

SOURCE=..\_common\guildwar.h
# End Source File
# Begin Source File

SOURCE=..\_Common\honor.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\Housing.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\Housing.h
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

SOURCE=..\_Common\langman.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\langman.h
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

SOURCE=..\_Common\MiniGame.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\MiniGame.h
# End Source File
# Begin Source File

SOURCE=..\_Common\MiniGameBase.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\MiniGameBase.h
# End Source File
# Begin Source File

SOURCE=..\_common\party.cpp
# End Source File
# Begin Source File

SOURCE=..\_common\party.h
# End Source File
# Begin Source File

SOURCE=..\_Common\partyquest.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\partyquest.h
# End Source File
# Begin Source File

SOURCE=..\_Common\path.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\path.h
# End Source File
# Begin Source File

SOURCE=..\_Common\PCBang.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\PCBang.h
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

SOURCE=..\_Common\profile.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\profile.h
# End Source File
# Begin Source File

SOURCE=..\_Common\ProfileInfo.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\ProfileInfo.h
# End Source File
# Begin Source File

SOURCE=..\_Common\Project.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\Project.h
# End Source File
# Begin Source File

SOURCE=..\_Common\ProjectCmn.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\ProjectCmn.h
# End Source File
# Begin Source File

SOURCE=..\_Common\ProjectLux.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\Quiz.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\Quiz.h
# End Source File
# Begin Source File

SOURCE=..\_Common\RainbowRace.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\RainbowRace.h
# End Source File
# Begin Source File

SOURCE=..\_Common\randomoption.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\randomoption.h
# End Source File
# Begin Source File

SOURCE=..\_Common\Respawn.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\Respawn.h
# End Source File
# Begin Source File

SOURCE=..\_Common\SecretRoom.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\SecretRoom.h
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

SOURCE=..\_Common\Tax.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\Tax.h
# End Source File
# Begin Source File

SOURCE=..\_Common\timer.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\timer.h
# End Source File
# Begin Source File

SOURCE=..\_Common\UltimateWeapon.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\UltimateWeapon.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\UserTaskBar.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\UserTaskBar.h
# End Source File
# Begin Source File

SOURCE=..\_Common\vutil.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\vutil.h
# End Source File
# Begin Source File

SOURCE=..\_common\weather.h
# End Source File
# Begin Source File

SOURCE=..\_Common\World.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\World.h
# End Source File
# Begin Source File

SOURCE=..\_Common\worldfile.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\worldintersect.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\worldmng.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\worldmng.h
# End Source File
# Begin Source File

SOURCE=..\_Common\xUtil.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\xUtil.h
# End Source File
# Begin Source File

SOURCE=..\_Common\xutil3d.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\xutil3d.h
# End Source File
# Begin Source File

SOURCE=..\_Common\xuzhu.h
# End Source File
# End Group
# End Group
# Begin Group "AI"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\_AIInterface\AIBear.cpp
# End Source File
# Begin Source File

SOURCE=..\_AIInterface\AIBear.h
# End Source File
# Begin Source File

SOURCE=..\_AIInterface\AIBigMuscle.cpp
# End Source File
# Begin Source File

SOURCE=..\_AIInterface\AIBigMuscle.h
# End Source File
# Begin Source File

SOURCE=..\_AIInterface\AIClockWorks.cpp
# End Source File
# Begin Source File

SOURCE=..\_AIInterface\AIClockWorks.h
# End Source File
# Begin Source File

SOURCE=..\_AIInterface\AIGuard.cpp
# End Source File
# Begin Source File

SOURCE=..\_AIInterface\AIGuard.h
# End Source File
# Begin Source File

SOURCE=..\_aiinterface\aiinterface.cpp
# End Source File
# Begin Source File

SOURCE=..\_aiinterface\aiinterface.h
# End Source File
# Begin Source File

SOURCE=..\_AIInterface\AIKrrr.cpp
# End Source File
# Begin Source File

SOURCE=..\_AIInterface\AIKrrr.h
# End Source File
# Begin Source File

SOURCE=..\_AIInterface\aimeteonyker.cpp
# End Source File
# Begin Source File

SOURCE=..\_AIInterface\aimeteonyker.h
# End Source File
# Begin Source File

SOURCE=..\_aiinterface\aimonster.cpp
# End Source File
# Begin Source File

SOURCE=..\_aiinterface\aimonster.h
# End Source File
# Begin Source File

SOURCE=..\_AIInterface\AIMonster2.cpp
# End Source File
# Begin Source File

SOURCE=..\_AIInterface\AIMonster2.h
# End Source File
# Begin Source File

SOURCE=..\_AIInterface\AIPet.cpp
# End Source File
# Begin Source File

SOURCE=..\_AIInterface\AIPet.h
# End Source File
# Begin Source File

SOURCE=..\_AIInterface\FSM.cpp
# End Source File
# Begin Source File

SOURCE=..\_AIInterface\FSM.h
# End Source File
# Begin Source File

SOURCE=..\_AIInterface\FSMstate.cpp
# End Source File
# Begin Source File

SOURCE=..\_AIInterface\FSMstate.h
# End Source File
# End Group
# End Target
# End Project
