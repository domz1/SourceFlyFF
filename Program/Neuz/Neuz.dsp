# Microsoft Developer Studio Project File - Name="Neuz" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=NEUZ - WIN32 DEBUG
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Neuz.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Neuz.mak" CFG="NEUZ - WIN32 DEBUG"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Neuz - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Neuz - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "Neuz - Win32 NoGameguard" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Neurospace/Program", NWMAAAAA"
# PROP Scc_LocalPath ".."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Neuz - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\Output\Neuz\Release"
# PROP Intermediate_Dir "..\..\Output\Neuz\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /Zd /O2 /I "." /I "..\..\Resource" /I "..\_Multimedia" /I "..\_DirectX" /I "..\_Common" /I "..\_Interface" /I "..\_network\misc\include" /I "..\_network" /I "..\_network\mcl\include" /I "..\_network\net\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FAcs /Fr /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "NDEBUG"
# ADD RSC /l 0x412 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 d3dx9dt.lib d3d9.lib d3dxof.lib dxerr9.lib dsound.lib winmm.lib dxguid.lib imm32.lib ws2_32.lib VERSION.LIB kernel32.lib user32.lib gdi32.lib /nologo /subsystem:windows /map /debug /machine:I386
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "Neuz - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\Output\Neuz\Debug"
# PROP Intermediate_Dir "..\..\Output\Neuz\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "." /I "..\Neuz" /I "..\..\Resource" /I "..\_Multimedia" /I "..\_DirectX" /I "..\_Common" /I "..\_Interface" /I "..\_network\misc\include" /I "..\_network" /I "..\_network\mcl\include" /I "..\_network\net\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Fr /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 d3dx9dt.lib d3d9.lib d3dxof.lib dxerr9.lib dsound.lib winmm.lib dxguid.lib imm32.lib ws2_32.lib npkcrypt.lib version.lib kernel32.lib user32.lib gdi32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /map /debug /machine:I386 /nodefaultlib:"libcmt.lib"
# SUBTRACT LINK32 /pdb:none /nodefaultlib

!ELSEIF  "$(CFG)" == "Neuz - Win32 NoGameguard"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Neuz___Win32_NoGameguard"
# PROP BASE Intermediate_Dir "Neuz___Win32_NoGameguard"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\Output\Neuz\NoGameguard"
# PROP Intermediate_Dir "..\..\Output\Neuz\NoGameguard"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /Zd /O2 /I "." /I "..\..\Resource" /I "..\_Multimedia" /I "..\_DirectX" /I "..\_Common" /I "..\_Interface" /I "..\_network\misc\include" /I "..\_network" /I "..\_network\mcl\include" /I "..\_network\net\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FAcs /Yu"stdafx.h" /FD /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /MT /W3 /GX /Zi /Od /I "." /I "..\..\Resource" /I "..\_Multimedia" /I "..\_DirectX" /I "..\_Common" /I "..\_Interface" /I "..\_network\misc\include" /I "..\_network" /I "..\_network\mcl\include" /I "..\_network\net\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "NO_GAMEGUARD" /Fr /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "NDEBUG"
# ADD RSC /l 0x412 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 d3dx9dt.lib d3d9.lib d3dxof.lib dxerr9.lib dsound.lib winmm.lib dxguid.lib imm32.lib ws2_32.lib VERSION.LIB kernel32.lib user32.lib gdi32.lib /nologo /subsystem:windows /map /debug /machine:I386
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 d3dx9dt.lib d3d9.lib d3dxof.lib dxerr9.lib dsound.lib winmm.lib dxguid.lib imm32.lib ws2_32.lib VERSION.LIB kernel32.lib user32.lib gdi32.lib /nologo /subsystem:windows /map /debug /machine:I386
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "Neuz - Win32 Release"
# Name "Neuz - Win32 Debug"
# Name "Neuz - Win32 NoGameguard"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "_DirectX"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\_DirectX\d3dapp.cpp
# End Source File
# Begin Source File

SOURCE=..\_DirectX\d3dapp.h
# End Source File
# Begin Source File

SOURCE=..\_DirectX\d3denumeration.cpp
# End Source File
# Begin Source File

SOURCE=..\_DirectX\d3denumeration.h
# End Source File
# Begin Source File

SOURCE=..\_DirectX\d3dfont.cpp
# End Source File
# Begin Source File

SOURCE=..\_DirectX\d3dfont.h
# End Source File
# Begin Source File

SOURCE=..\_DirectX\d3dres.h
# End Source File
# Begin Source File

SOURCE=..\_DirectX\d3dsettings.cpp
# End Source File
# Begin Source File

SOURCE=..\_DirectX\d3dsettings.h
# End Source File
# Begin Source File

SOURCE=..\_DirectX\d3dutil.cpp
# End Source File
# Begin Source File

SOURCE=..\_DirectX\d3dutil.h
# End Source File
# Begin Source File

SOURCE=..\_DirectX\dsutil.cpp
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
# Begin Group "_Interface"

# PROP Default_Filter ""
# Begin Group "WndBase"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\_Common\ResManager.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\ResManager.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndBase.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndBase.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndControl.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndControl.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndEditCtrl.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndEditCtrl.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndItemCtrl.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndItemCtrl.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndTradeCtrl.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndTradeCtrl.h
# End Source File
# End Group
# Begin Group "WndTitle"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\_Interface\WndCredit.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndCredit.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndTitle.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndTitle.h
# End Source File
# End Group
# Begin Group "Misc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\_Interface\Clipboard.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\Clipboard.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\ITheme.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\ITheme.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\tooltip.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\tooltip.h
# End Source File
# End Group
# Begin Group "WndApplet"

# PROP Default_Filter ""
# Begin Group "Communication"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\_interface\WndChatFilter.cpp
# End Source File
# Begin Source File

SOURCE=..\_interface\WndChatFilter.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndCommand.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndCommand.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndCommunication.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndCommunication.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndFriendConFirm.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndFriendConFirm.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndFriendCtrl.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndFriendCtrl.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndGuildCtrl.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndGuildCtrl.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndMessenger.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndMessenger.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndMessengerCtrl.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndMessengerCtrl.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndPartyCtrl.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndPartyCtrl.h
# End Source File
# End Group
# Begin Group "Vendor"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\_interface\WndRegVend.cpp
# End Source File
# Begin Source File

SOURCE=..\_interface\WndRegVend.h
# End Source File
# Begin Source File

SOURCE=..\_interface\WndVendor.cpp
# End Source File
# Begin Source File

SOURCE=..\_interface\WndVendor.h
# End Source File
# Begin Source File

SOURCE=..\_interface\WndVendorBuy.cpp
# End Source File
# Begin Source File

SOURCE=..\_interface\WndVendorBuy.h
# End Source File
# Begin Source File

SOURCE=..\_interface\WndVendorCtrl.cpp
# End Source File
# Begin Source File

SOURCE=..\_interface\WndVendorCtrl.h
# End Source File
# End Group
# Begin Group "Guild"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\_Interface\WndGuild.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndGuild.h
# End Source File
# Begin Source File

SOURCE=..\_interface\WndGuildBank.cpp
# End Source File
# Begin Source File

SOURCE=..\_interface\WndGuildBank.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndGuildCombat1to1.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndGuildCombat1to1.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndGuildHouse.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndGuildHouse.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndGuildMerit.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndGuildMerit.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndGuildName.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndGuildName.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndGuildTabApp.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndGuildTabApp.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndGuildTabInfo.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndGuildTabInfo.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndGuildTabMember.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndGuildTabMember.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndGuildTabPower.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndGuildTabPower.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndGuildTabWar.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndGuildTabWar.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndGuildVote.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndGuildVote.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndGuildWarDecl.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndGuildWarDecl.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndGuildWarGiveUp.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndGuildWarGiveUp.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndGuildWarPeace.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndGuildWarPeace.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndGuildWarPeaceConfirm.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndGuildWarPeaceConfirm.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndGuildWarRequest.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndGuildWarRequest.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndSecretRoom.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndSecretRoom.h
# End Source File
# End Group
# Begin Group "Rank"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\_Interface\WndRankGuild.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndRankGuild.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndRankInfo.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndRankInfo.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndRankWar.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndRankWar.h
# End Source File
# End Group
# Begin Group "Piercing"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\_Interface\WndPiercing.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndPiercing.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\_Interface\Wnd2ndPassword.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\Wnd2ndPassword.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndAwakening.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndAwakening.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndBagEx.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndBagEx.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndBank.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndBank.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndBlessingCancel.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndBlessingCancel.h
# End Source File
# Begin Source File

SOURCE=..\_interface\WndChangeFace.cpp
# End Source File
# Begin Source File

SOURCE=..\_interface\WndChangeFace.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndChangeName.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndChangeName.h
# End Source File
# Begin Source File

SOURCE=..\_interface\WndCloseExistingConnection.cpp
# End Source File
# Begin Source File

SOURCE=..\_interface\WndCloseExistingConnection.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndCollecting.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndCollecting.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndCommItem.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndCommItem.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndDialog.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndDuelConfirm.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndDuelConfirm.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndField.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndField.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndHelp.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndHelp.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndHousing.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndHousing.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndLord.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndLord.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndLvReqDown.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndLvReqDown.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndMap.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndMap.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndMessengerSearch.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndMessengerSearch.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndMiniGame.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndMiniGame.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndMotion.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndMotion.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndNotice.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndNotice.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndOption.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndOption.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndOptionGame.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndOptionGame.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndParty.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndParty.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndPartyChangeTroup.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndPartyChangeTroup.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndPartyConfirm.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndPartyConfirm.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndPartyQuick.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndPartyQuick.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndPetRes.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndPetRes.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndPetSys.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndPetSys.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndPvp.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndPvp.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndQuestQuickInfo.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndQuestQuickInfo.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndRainbowRace.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndRainbowRace.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndRepairItem.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndRepairItem.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndRepairItemCtrl.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndRepairItemCtrl.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndSealChar.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndSealChar.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndSelectCh.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndSelectCh.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndShop.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndShop.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndSummonAngel.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndSummonAngel.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndSummonFriend.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndSummonFriend.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndTaskBar.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndTaskBar.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndText.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndText.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndTutorial.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndTutorial.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndUpgradeBase.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndUpgradeBase.h
# End Source File
# End Group
# Begin Group "Admin"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\_Interface\WndAdminCreateItem.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndAdminCreateItem.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndDebugInfo.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndDebugInfo.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndIndirectTalk.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndIndirectTalk.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\_Interface\AppDefine.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\FuncApplet.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\FuncTextCmd.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\FuncTextCmd.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndGuideSystem.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndGuideSystem.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndInvenRemoveItem.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndInvenRemoveItem.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndManager.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndManager.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndMessageBox.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndMessageBox.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndNeuz.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndNeuz.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndQuest.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndQuest.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndQuitRoom.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndQuitRoom.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndRoomList.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndRoomList.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndSelectVillage.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndSelectVillage.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndWebBox.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndWebBox.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndWorld.cpp
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndWorld.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndWorldControlPlayer.cpp
# End Source File
# End Group
# Begin Group "_Common"

# PROP Default_Filter ""
# Begin Group "LOD"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\_Common\landscape.h
# End Source File
# Begin Source File

SOURCE=..\_Common\lod.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\patch.h
# End Source File
# End Group
# Begin Group "Object"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\_Common\CommonCtrl.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\CommonCtrl.h
# End Source File
# Begin Source File

SOURCE=..\_Common\CreateObj.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\CreateObj.h
# End Source File
# Begin Source File

SOURCE=..\_Common\Ctrl.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\Ctrl.h
# End Source File
# Begin Source File

SOURCE=..\_Common\Item.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\Item.h
# End Source File
# Begin Source File

SOURCE=..\_Common\Light.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\Light.h
# End Source File
# Begin Source File

SOURCE=..\_Common\Mover.cpp
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

SOURCE=..\_Common\MoverEquip.cpp
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

SOURCE=..\_Common\MoverRender.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\MoverSkill.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\Obj.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\Obj.h
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

SOURCE=..\_Common\Region.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\Region.h
# End Source File
# Begin Source File

SOURCE=..\_Common\Sfx.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\Sfx.h
# End Source File
# Begin Source File

SOURCE=..\_Common\sfxbase.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\sfxbase.h
# End Source File
# Begin Source File

SOURCE=..\_Common\SfxObj.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\Ship.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\Ship.h
# End Source File
# Begin Source File

SOURCE=..\_Common\SkillInfluence.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\SkillInfluence.h
# End Source File
# Begin Source File

SOURCE=..\_Common\weather.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\weather.h
# End Source File
# End Group
# Begin Group "Model No. 1"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\_Common\billboard.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\billboard.h
# End Source File
# Begin Source File

SOURCE=..\..\Resource\Blur.fx
# End Source File
# Begin Source File

SOURCE=..\..\Resource\blur.ps
# End Source File
# Begin Source File

SOURCE=..\..\Resource\blur.vs
# End Source File
# Begin Source File

SOURCE=..\..\Resource\Blur2.fx
# End Source File
# Begin Source File

SOURCE=..\_Common\Bone.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\Bone.h
# End Source File
# Begin Source File

SOURCE=..\_Common\Material.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\Material.h
# End Source File
# Begin Source File

SOURCE=..\_Common\Model.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\Model.h
# End Source File
# Begin Source File

SOURCE=..\_Common\ModelGlobal.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\ModelGlobal.h
# End Source File
# Begin Source File

SOURCE=..\_Common\ModelMng.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\ModelMng.h
# End Source File
# Begin Source File

SOURCE=..\_Common\ModelObject.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\ModelObject.h
# End Source File
# Begin Source File

SOURCE=..\_Common\Object3D.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\Object3D.h
# End Source File
# Begin Source File

SOURCE=..\_Common\ParticleMng.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\ParticleMng.h
# End Source File
# Begin Source File

SOURCE=..\_Common\Shader.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\Shader.h
# End Source File
# Begin Source File

SOURCE=..\..\resource\skining.vsh
# End Source File
# Begin Source File

SOURCE=..\..\resource\skining2.vsh
# End Source File
# Begin Source File

SOURCE=..\_Common\TailEffectMng.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\TailEffectMng.h
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

SOURCE=..\_Common\Script.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\Script.h
# End Source File
# End Group
# Begin Group "Contents"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\_common\guild.cpp
# End Source File
# Begin Source File

SOURCE=..\_common\guild.h
# End Source File
# Begin Source File

SOURCE=..\_common\guildquest.cpp
# End Source File
# Begin Source File

SOURCE=..\_common\guildquest.h
# End Source File
# Begin Source File

SOURCE=..\_Common\GuildRender.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\GuildRender.h
# End Source File
# Begin Source File

SOURCE=..\_common\guildwar.cpp
# End Source File
# Begin Source File

SOURCE=..\_common\guildwar.h
# End Source File
# Begin Source File

SOURCE=..\_Common\party.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\party.h
# End Source File
# End Group
# Begin Group "World"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\_Common\Camera.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\Camera.h
# End Source File
# Begin Source File

SOURCE=..\_Common\ContinentDef.h
# End Source File
# Begin Source File

SOURCE=..\_Common\Environment.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\Environment.h
# End Source File
# Begin Source File

SOURCE=..\_Common\Respawn.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\SkyBox.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\SkyBox.h
# End Source File
# Begin Source File

SOURCE=..\_Common\TerrainMng.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\TerrainMng.h
# End Source File
# Begin Source File

SOURCE=..\_Common\World.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\World.h
# End Source File
# Begin Source File

SOURCE=..\_Common\World3D.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\WorldEnvironment.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\WorldFile.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\WorldIntersect.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\worldmng.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\worldmng.h
# End Source File
# End Group
# Begin Group "Low"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\_Common\2DRender.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\2DRender.h
# End Source File
# Begin Source File

SOURCE=..\_Common\data.h
# End Source File
# Begin Source File

SOURCE=..\_Common\EditString.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\EditString.h
# End Source File
# Begin Source File

SOURCE=..\_Common\File.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\File.h
# End Source File
# Begin Source File

SOURCE=..\_Common\Graphic3D.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\Graphic3D.h
# End Source File
# Begin Source File

SOURCE=..\_Common\Music.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\Music.h
# End Source File
# Begin Source File

SOURCE=..\_Common\Music2.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\path.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\path.h
# End Source File
# Begin Source File

SOURCE=..\_Common\targa.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\targa.h
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
# Begin Source File

SOURCE=..\_Common\xUtil3D.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\xUtil3D.h
# End Source File
# End Group
# Begin Group "TextMessage"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\_Common\ClientMsg.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\ClientMsg.h
# End Source File
# Begin Source File

SOURCE=..\_Common\DialogMsg.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\DialogMsg.h
# End Source File
# End Group
# Begin Group "Console"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\_Common\Console.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\Console.h
# End Source File
# Begin Source File

SOURCE=..\_Common\ConsoleCommon.h
# End Source File
# Begin Source File

SOURCE=..\_Common\ConsoleFunc.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=..\_AIInterface\AIInterface.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\authorization.h
# End Source File
# Begin Source File

SOURCE=..\_Common\BsCommon.h
# End Source File
# Begin Source File

SOURCE=..\_Common\buff.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\buff.h
# End Source File
# Begin Source File

SOURCE=..\_Common\Campus.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\Chatting.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\Chatting.h
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

SOURCE=..\_Common\couple.cpp
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

SOURCE=..\_common\eveschool.cpp
# End Source File
# Begin Source File

SOURCE=..\_common\eveschool.h
# End Source File
# Begin Source File

SOURCE=..\_Common\Exchange.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\Exchange.h
# End Source File
# Begin Source File

SOURCE=..\_common\flyffevent.cpp
# End Source File
# Begin Source File

SOURCE=..\_common\flyffevent.h
# End Source File
# Begin Source File

SOURCE=..\_Common\fmod.h
# End Source File
# Begin Source File

SOURCE=..\_Common\fmod_errors.h
# End Source File
# Begin Source File

SOURCE=..\_Common\fmoddyn.h
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

SOURCE=..\_Common\honor.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\honor.h
# End Source File
# Begin Source File

SOURCE=..\_Common\Housing.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\Housing.h
# End Source File
# Begin Source File

SOURCE=..\_Common\HwOption.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\HwOption.h
# End Source File
# Begin Source File

SOURCE=..\_Common\lordskill.cpp
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

SOURCE=..\_Common\moverbuff.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\Particle.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\Particle.h
# End Source File
# Begin Source File

SOURCE=..\_Common\partyquest.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\partyquest.h
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

SOURCE=..\_Common\post.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\post.h
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

SOURCE=..\_Common\RainbowRace.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\randomoption.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\Respawn.h
# End Source File
# Begin Source File

SOURCE=..\_Common\Rijndael.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\Rijndael.h
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

SOURCE=.\TexturePool.cpp
# End Source File
# Begin Source File

SOURCE=.\TexturePool.h
# End Source File
# Begin Source File

SOURCE=..\_Common\ticket.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\ticket.h
# End Source File
# Begin Source File

SOURCE=..\_Common\TreeInformation.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\TreeInformation.h
# End Source File
# Begin Source File

SOURCE=..\_Common\UltimateWeapon.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\UltimateWeapon.h
# End Source File
# Begin Source File

SOURCE=..\_Common\yUtil.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\yUtil.h
# End Source File
# End Group
# Begin Group "_Network"

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

SOURCE=..\_Network\Net\Src\crc.cpp
# End Source File
# Begin Source File

SOURCE=..\_Network\Net\Include\crc.h
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
# Begin Group "Mics"

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

SOURCE=..\_network\misc\include\ObjMap.h
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

SOURCE=..\_network\ggsrv.h
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
# Begin Group "_Multimedia"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\_Multimedia\WRITEAVI.CPP
# End Source File
# Begin Source File

SOURCE=..\_Multimedia\WriteAVI.h
# End Source File
# End Group
# Begin Group "_ObjSubClass"

# PROP Default_Filter ""
# Begin Group "Action"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\_AIInterface\Action.cpp
# End Source File
# Begin Source File

SOURCE=..\_AIInterface\Action.h
# End Source File
# Begin Source File

SOURCE=..\_AIInterface\ActionMover.cpp
# End Source File
# Begin Source File

SOURCE=..\_AIInterface\ActionMover.h
# End Source File
# Begin Source File

SOURCE=..\_AIInterface\ActionMoverCollision.cpp
# End Source File
# Begin Source File

SOURCE=..\_AIInterface\ActionMoverCtrl.cpp
# End Source File
# Begin Source File

SOURCE=..\_AIInterface\ActionMoverDmg.cpp
# End Source File
# Begin Source File

SOURCE=..\_AIInterface\ActionMoverMsg.cpp
# End Source File
# Begin Source File

SOURCE=..\_AIInterface\ActionMoverMsg2.cpp
# End Source File
# Begin Source File

SOURCE=..\_AIInterface\ActionMoverState.cpp
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
# End Group
# Begin Group "_ExceptionHandler"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\_UnhandledException\ExceptionHandler.cpp
# End Source File
# Begin Source File

SOURCE=..\_UnhandledException\ExceptionHandler.h
# End Source File
# Begin Source File

SOURCE=..\_UnhandledException\neuzd.lib
# End Source File
# End Group
# Begin Source File

SOURCE=..\_Common\accessory.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\accessory.h
# End Source File
# Begin Source File

SOURCE=.\CampusHelper.cpp
# End Source File
# Begin Source File

SOURCE=.\clord.cpp
# End Source File
# Begin Source File

SOURCE=.\Cloth.cpp
# End Source File
# Begin Source File

SOURCE=.\CNTREVNT.h
# End Source File
# Begin Source File

SOURCE=..\_Common\collecting.cpp
# End Source File
# Begin Source File

SOURCE=.\couplehelper.cpp
# End Source File
# Begin Source File

SOURCE=.\couplehelper.h
# End Source File
# Begin Source File

SOURCE=.\DPCertified.cpp
# End Source File
# Begin Source File

SOURCE=.\DPCertified.h
# End Source File
# Begin Source File

SOURCE=.\DPClient.cpp
# End Source File
# Begin Source File

SOURCE=.\DPClient.h
# End Source File
# Begin Source File

SOURCE=.\DPClientGuildWar.cpp
# End Source File
# Begin Source File

SOURCE=.\DPClientLux.cpp
# End Source File
# Begin Source File

SOURCE=.\DPCollectClient.cpp
# End Source File
# Begin Source File

SOURCE=.\DPLoginClient.cpp
# End Source File
# Begin Source File

SOURCE=.\DPLoginClient.h
# End Source File
# Begin Source File

SOURCE=..\_Common\langman.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\lord.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\lord.h
# End Source File
# Begin Source File

SOURCE=..\_Common\memtrace.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\memtrace.h
# End Source File
# Begin Source File

SOURCE=..\_Common\mss.h
# End Source File
# Begin Source File

SOURCE=.\Network.cpp
# End Source File
# Begin Source File

SOURCE=.\Network.h
# End Source File
# Begin Source File

SOURCE=.\Neuz.cpp
# End Source File
# Begin Source File

SOURCE=.\Neuz.h
# End Source File
# Begin Source File

SOURCE=.\NeuzEnemy.cpp
# End Source File
# Begin Source File

SOURCE=.\NeuzEnemy.h
# End Source File
# Begin Source File

SOURCE=.\NeuzMsgProc.cpp
# End Source File
# Begin Source File

SOURCE=.\NPGameLib.cpp
# End Source File
# Begin Source File

SOURCE=.\NPGameLib4.h
# End Source File
# Begin Source File

SOURCE=.\npkcrypt.h
# End Source File
# Begin Source File

SOURCE=.\OpenLoadWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\OpenLoadWnd.h
# End Source File
# Begin Source File

SOURCE=..\_Common\playerdata.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\playerdata.h
# End Source File
# Begin Source File

SOURCE=..\_Common\pocket.cpp
# End Source File
# Begin Source File

SOURCE=..\_Common\rtmessenger.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "Neuz - Win32 Release"

# ADD CPP /Yc"stdafx.h"

!ELSEIF  "$(CFG)" == "Neuz - Win32 Debug"

# ADD CPP /ZI /Yc"stdafx.h"

!ELSEIF  "$(CFG)" == "Neuz - Win32 NoGameguard"

# ADD CPP /Yc"stdafx.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TimeLimit.cpp
# End Source File
# Begin Source File

SOURCE=.\TimeLimit.h
# End Source File
# Begin Source File

SOURCE=.\VersionCommon.h
# End Source File
# Begin Source File

SOURCE=.\WebBox.cpp
# End Source File
# Begin Source File

SOURCE=.\WebBox.h
# End Source File
# Begin Source File

SOURCE=.\WebCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\WebCtrl.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndQuizEvent.cpp
# End Source File
# Begin Source File

SOURCE=.\WorldMap.cpp
# End Source File
# Begin Source File

SOURCE=.\WorldMap.h
# End Source File
# Begin Source File

SOURCE=..\_Common\xuzhu.h
# End Source File
# Begin Source File

SOURCE=..\_Common\mss32.lib
# End Source File
# Begin Source File

SOURCE=.\npkcrypt.lib
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\resource\neuz.ini
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\WinMain.rc
# End Source File
# End Group
# Begin Group "Resource Data"

# PROP Default_Filter ""
# Begin Group "Dialog"

# PROP Default_Filter ""
# Begin Group "Madrigal"

# PROP Default_Filter ""
# End Group
# End Group
# Begin Group "Theme"

# PROP Default_Filter ""
# End Group
# Begin Group "Sound"

# PROP Default_Filter ""
# End Group
# Begin Group "Music"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\resource\Music\default.bgm
# End Source File
# Begin Source File

SOURCE="..\..\resource\Music\morrowind title.mp3"
# End Source File
# Begin Source File

SOURCE=..\..\resource\Music\music1007.mp3
# End Source File
# Begin Source File

SOURCE=..\..\resource\Music\track13.wav
# End Source File
# End Group
# Begin Group "Effect"

# PROP Default_Filter ""
# End Group
# Begin Group "Tool"

# PROP Default_Filter ""
# End Group
# Begin Group "Definition Files"

# PROP Default_Filter ""
# End Group
# Begin Group "Client"

# PROP Default_Filter ""
# End Group
# Begin Source File

SOURCE=..\..\resource\credit.txt
# End Source File
# Begin Source File

SOURCE=.\res\cur00001.cur
# End Source File
# Begin Source File

SOURCE=.\res\cur00002.cur
# End Source File
# Begin Source File

SOURCE=.\res\cur00003.cur
# End Source File
# Begin Source File

SOURCE=.\res\curAttack.cur
# End Source File
# Begin Source File

SOURCE=.\res\curBase.cur
# End Source File
# Begin Source File

SOURCE=.\res\curCamera.cur
# End Source File
# Begin Source File

SOURCE=.\res\curControl.cur
# End Source File
# Begin Source File

SOURCE=.\res\curDelay.cur
# End Source File
# Begin Source File

SOURCE=.\res\curGetItem.cur
# End Source File
# Begin Source File

SOURCE=.\res\curHelp.cur
# End Source File
# Begin Source File

SOURCE=.\res\curNo.cur
# End Source File
# Begin Source File

SOURCE=.\res\curNPC.cur
# End Source File
# Begin Source File

SOURCE=.\res\curSelect.cur
# End Source File
# Begin Source File

SOURCE=.\res\cursor2.cur
# End Source File
# Begin Source File

SOURCE=.\res\cursor3.cur
# End Source File
# Begin Source File

SOURCE=.\res\edit.cur
# End Source File
# Begin Source File

SOURCE=.\res\hori.cur
# End Source File
# Begin Source File

SOURCE=.\res\ico00001.ico
# End Source File
# Begin Source File

SOURCE=.\res\main_ico.ico
# End Source File
# Begin Source File

SOURCE=..\..\resource\Masquerade.prj
# End Source File
# Begin Source File

SOURCE=..\..\resource\mss32.dll
# End Source File
# Begin Source File

SOURCE=..\..\resource\mssmp3.asi
# End Source File
# Begin Source File

SOURCE=.\res\resize_h.cur
# End Source File
# Begin Source File

SOURCE=..\..\resource\serverIPList.txt
# End Source File
# Begin Source File

SOURCE=..\..\resource\water.vsh
# End Source File
# End Group
# Begin Source File

SOURCE=.\res\anvil.cur
# End Source File
# Begin Source File

SOURCE=.\CampusHelper.h
# End Source File
# Begin Source File

SOURCE=.\clord.h
# End Source File
# Begin Source File

SOURCE=.\Cloth.h
# End Source File
# Begin Source File

SOURCE=..\_Common\collecting.h
# End Source File
# Begin Source File

SOURCE=.\res\curDoor.cur
# End Source File
# Begin Source File

SOURCE=.\res\curGate.cur
# End Source File
# Begin Source File

SOURCE=.\res\curLever.cur
# End Source File
# Begin Source File

SOURCE=.\DPCollectClient.h
# End Source File
# Begin Source File

SOURCE=..\_Common\langman.h
# End Source File
# Begin Source File

SOURCE=.\res\Omg.ico
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# Begin Source File

SOURCE=..\_Common\rtmessenger.h
# End Source File
# Begin Source File

SOURCE=..\_Interface\WndQuizEvent.h
# End Source File
# Begin Source File

SOURCE=".\남인환매모.txt"
# End Source File
# Begin Source File

SOURCE=".\허저메모.txt"
# End Source File
# End Target
# End Project
