# Microsoft Developer Studio Project File - Name="Mixere" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Mixere - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Mixere.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Mixere.mak" CFG="Mixere - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Mixere - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Mixere - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Mixere - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 audiere.lib shlwapi.lib version.lib htmlhelp.lib /nologo /subsystem:windows /machine:I386
# SUBTRACT LINK32 /profile /debug
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=if exist audiere.dll copy audiere.dll Release	if exist mixere.chm copy mixere.chm Release
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Mixere - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 audiere.lib shlwapi.lib version.lib htmlhelp.lib /nologo /subsystem:windows /profile /debug /machine:I386
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=if exist audiere.dll copy audiere.dll Debug	if exist mixere.chm copy mixere.chm Debug
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "Mixere - Win32 Release"
# Name "Mixere - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AccelTable.cpp
# End Source File
# Begin Source File

SOURCE=.\AccelTable.h
# End Source File
# Begin Source File

SOURCE=.\Audio.cpp
# End Source File
# Begin Source File

SOURCE=.\Audio.h
# End Source File
# Begin Source File

SOURCE=.\AutoDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\AutoDlg.h
# End Source File
# Begin Source File

SOURCE=.\AutoInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\AutoInfo.h
# End Source File
# Begin Source File

SOURCE=.\AutoSliderCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\AutoSliderCtrl.h
# End Source File
# Begin Source File

SOURCE=.\AutoSliderDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\AutoSliderDlg.h
# End Source File
# Begin Source File

SOURCE=.\AutoSliderDlgBar.cpp
# End Source File
# Begin Source File

SOURCE=.\AutoSliderDlgBar.h
# End Source File
# Begin Source File

SOURCE=.\AutoTriggerCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\AutoTriggerCtrl.h
# End Source File
# Begin Source File

SOURCE=.\ChanAudio.cpp
# End Source File
# Begin Source File

SOURCE=.\ChanAudio.h
# End Source File
# Begin Source File

SOURCE=.\ChanInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\ChanInfo.h
# End Source File
# Begin Source File

SOURCE=.\Channel.cpp
# End Source File
# Begin Source File

SOURCE=.\Channel.h
# End Source File
# Begin Source File

SOURCE=.\ChildFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\ChildFrm.h
# End Source File
# Begin Source File

SOURCE=.\ChildToolTip.cpp
# End Source File
# Begin Source File

SOURCE=.\ChildTooltip.h
# End Source File
# Begin Source File

SOURCE=.\ClickSliderCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\ClickSliderCtrl.h
# End Source File
# Begin Source File

SOURCE=.\Clipboard.cpp
# End Source File
# Begin Source File

SOURCE=.\Clipboard.h
# End Source File
# Begin Source File

SOURCE=.\ColorStatusBar.cpp
# End Source File
# Begin Source File

SOURCE=.\ColorStatusBar.h
# End Source File
# Begin Source File

SOURCE=.\ColumnResizer.cpp
# End Source File
# Begin Source File

SOURCE=.\ColumnResizer.h
# End Source File
# Begin Source File

SOURCE=.\CrossfaderBar.cpp
# End Source File
# Begin Source File

SOURCE=.\CrossfaderBar.h
# End Source File
# Begin Source File

SOURCE=.\CtrlResize.cpp
# End Source File
# Begin Source File

SOURCE=.\CtrlResize.h
# End Source File
# Begin Source File

SOURCE=.\DialogBarEx.cpp
# End Source File
# Begin Source File

SOURCE=.\DialogBarEx.h
# End Source File
# Begin Source File

SOURCE=.\DialogEx.cpp
# End Source File
# Begin Source File

SOURCE=.\DialogEx.h
# End Source File
# Begin Source File

SOURCE=.\EditSnapsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\EditSnapsDlg.h
# End Source File
# Begin Source File

SOURCE=.\FileSearchDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\FileSearchDlg.h
# End Source File
# Begin Source File

SOURCE=.\FolderDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\FolderDialog.h
# End Source File
# Begin Source File

SOURCE=.\FormListHeader.cpp
# End Source File
# Begin Source File

SOURCE=.\FormListHeader.h
# End Source File
# Begin Source File

SOURCE=.\FormListInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\FormListInfo.h
# End Source File
# Begin Source File

SOURCE=.\FormListItemDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\FormListItemDlg.h
# End Source File
# Begin Source File

SOURCE=.\FormListView.cpp
# End Source File
# Begin Source File

SOURCE=.\FormListView.h
# End Source File
# Begin Source File

SOURCE=.\HotKeyCtrlEx.cpp
# End Source File
# Begin Source File

SOURCE=.\HotKeyCtrlEx.h
# End Source File
# Begin Source File

SOURCE=.\HotKeyList.cpp
# End Source File
# Begin Source File

SOURCE=.\HotKeyList.h
# End Source File
# Begin Source File

SOURCE=.\Hyperlink.cpp
# End Source File
# Begin Source File

SOURCE=.\Hyperlink.h
# End Source File
# Begin Source File

SOURCE=.\IconButton.cpp
# End Source File
# Begin Source File

SOURCE=.\IconButton.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\MissingFilesDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MissingFilesDlg.h
# End Source File
# Begin Source File

SOURCE=.\Mixere.cpp
# End Source File
# Begin Source File

SOURCE=.\Mixere.h
# End Source File
# Begin Source File

SOURCE=.\Mixere.rc
# End Source File
# Begin Source File

SOURCE=.\MixereDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\MixereDoc.h
# End Source File
# Begin Source File

SOURCE=.\MixereView.cpp
# End Source File
# Begin Source File

SOURCE=.\MixereView.h
# End Source File
# Begin Source File

SOURCE=.\MixerInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\MixerInfo.h
# End Source File
# Begin Source File

SOURCE=.\MSFadeBar.cpp
# End Source File
# Begin Source File

SOURCE=.\MSFadeBar.h
# End Source File
# Begin Source File

SOURCE=.\MultiAutoDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MultiAutoDlg.h
# End Source File
# Begin Source File

SOURCE=.\MultiMix.cpp
# End Source File
# Begin Source File

SOURCE=.\MultiMix.h
# End Source File
# Begin Source File

SOURCE=.\NumEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\NumEdit.h
# End Source File
# Begin Source File

SOURCE=.\OptionsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionsDlg.h
# End Source File
# Begin Source File

SOURCE=.\OptsAppearDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\OptsAppearDlg.h
# End Source File
# Begin Source File

SOURCE=.\OptsGeneralDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\OptsGeneralDlg.h
# End Source File
# Begin Source File

SOURCE=.\OptsSlidersDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\OptsSlidersDlg.h
# End Source File
# Begin Source File

SOURCE=.\Persist.cpp
# End Source File
# Begin Source File

SOURCE=.\Persist.h
# End Source File
# Begin Source File

SOURCE=.\PopupEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\PopupEdit.h
# End Source File
# Begin Source File

SOURCE=.\PropertiesDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PropertiesDlg.h
# End Source File
# Begin Source File

SOURCE=.\ReplaceFilesDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ReplaceFilesDlg.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\SelectSliderCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\SelectSliderCtrl.h
# End Source File
# Begin Source File

SOURCE=.\SmartBuf.cpp
# End Source File
# Begin Source File

SOURCE=.\SmartBuf.h
# End Source File
# Begin Source File

SOURCE=.\Snapshot.cpp
# End Source File
# Begin Source File

SOURCE=.\Snapshot.h
# End Source File
# Begin Source File

SOURCE=.\SnapshotBar.cpp
# End Source File
# Begin Source File

SOURCE=.\SnapshotBar.h
# End Source File
# Begin Source File

SOURCE=.\StaticEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\StaticEdit.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\SubFileFind.cpp
# End Source File
# Begin Source File

SOURCE=.\SubFileFind.h
# End Source File
# Begin Source File

SOURCE=.\TakeSnapDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TakeSnapDlg.h
# End Source File
# Begin Source File

SOURCE=.\TempoBar.cpp
# End Source File
# Begin Source File

SOURCE=.\TempoBar.h
# End Source File
# Begin Source File

SOURCE=.\TimeEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\TimeEdit.h
# End Source File
# Begin Source File

SOURCE=.\Undoable.cpp
# End Source File
# Begin Source File

SOURCE=.\Undoable.h
# End Source File
# Begin Source File

SOURCE=.\UndoManager.cpp
# End Source File
# Begin Source File

SOURCE=.\UndoManager.h
# End Source File
# Begin Source File

SOURCE=.\UndoState.cpp
# End Source File
# Begin Source File

SOURCE=.\UndoState.h
# End Source File
# Begin Source File

SOURCE=.\Version.h
# End Source File
# Begin Source File

SOURCE=.\VersionInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\VersionInfo.h
# End Source File
# Begin Source File

SOURCE=.\VolumeBar.cpp
# End Source File
# Begin Source File

SOURCE=.\VolumeBar.h
# End Source File
# Begin Source File

SOURCE=.\Win32Console.cpp
# End Source File
# Begin Source File

SOURCE=.\Win32Console.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\cfdipless.ico
# End Source File
# Begin Source File

SOURCE=.\res\cflinear.ico
# End Source File
# Begin Source File

SOURCE=.\res\cursor2.cur
# End Source File
# Begin Source File

SOURCE=.\res\dragmulti.cur
# End Source File
# Begin Source File

SOURCE=.\res\dragsingle.cur
# End Source File
# Begin Source File

SOURCE=.\res\loopd.ico
# End Source File
# Begin Source File

SOURCE=.\res\loopu.ico
# End Source File
# Begin Source File

SOURCE=.\res\minutesd.ico
# End Source File
# Begin Source File

SOURCE=.\res\minutesu.ico
# End Source File
# Begin Source File

SOURCE=.\res\Mixere.ico
# End Source File
# Begin Source File

SOURCE=.\res\Mixere.rc2
# End Source File
# Begin Source File

SOURCE=.\res\MixereDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\muted.ico
# End Source File
# Begin Source File

SOURCE=.\res\muteu.ico
# End Source File
# Begin Source File

SOURCE=.\res\paused.ico
# End Source File
# Begin Source File

SOURCE=.\res\pauseu.ico
# End Source File
# Begin Source File

SOURCE=.\res\playd.ico
# End Source File
# Begin Source File

SOURCE=.\res\playu.ico
# End Source File
# Begin Source File

SOURCE=.\res\solod.ico
# End Source File
# Begin Source File

SOURCE=.\res\solou.ico
# End Source File
# Begin Source File

SOURCE=.\res\sortd.bmp
# End Source File
# Begin Source File

SOURCE=.\res\sortu.bmp
# End Source File
# Begin Source File

SOURCE=.\res\stagger.ico
# End Source File
# Begin Source File

SOURCE=.\res\stopd.ico
# End Source File
# Begin Source File

SOURCE=.\res\stopu.ico
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\License.txt
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
# Section Mixere : {318F824D-2CBB-4424-84E7-5889CB14CA15}
# 	2:9:Audio.cpp:Audio.cpp
# 	2:7:Audio.h:Audio.h
# 	2:13:CLASS: CAudio:CAudio
# 	2:19:Application Include:Mixere.h
# End Section
# Section Mixere : {95D18748-B99E-47B2-A2ED-D4C68CFE14FA}
# 	2:17:CLASS: CPopupEdit:CPopupEdit
# 	2:11:PopupEdit.h:PopupEdit.h
# 	2:19:Application Include:mixere.h
# 	2:13:PopupEdit.cpp:PopupEdit.cpp
# End Section
# Section Mixere : {80437918-B3BF-4AB9-92FD-8C5C33EB5538}
# 	2:12:ChildFrm.cpp:ChildFrm.cpp
# 	2:18:CLASS: CChildFrame:CChildFrame
# 	2:19:Application Include:Mixere.h
# 	2:10:ChildFrm.h:ChildFrm.h
# End Section
# Section Mixere : {7E9C64DE-517D-474B-B7E4-79438EB6453B}
# 	2:20:CLASS: CSelectSlider:CSelectSlider
# 	2:14:SelectSlider.h:SelectSlider.h
# 	2:19:Application Include:Mixere.h
# 	2:16:SelectSlider.cpp:SelectSlider.cpp
# End Section
# Section Mixere : {D25A274C-9145-4E39-B953-53DF27F749C4}
# 	2:18:CLASS: CIconButton:CIconButton
# 	2:12:IconButton.h:IconButton.h
# 	2:19:Application Include:Mixere.h
# 	2:14:IconButton.cpp:IconButton.cpp
# End Section
# Section Mixere : {C6996E66-E3A3-4944-B15B-B1394A8782D7}
# 	1:11:IDD_CHANNEL:102
# 	2:16:Resource Include:resource.h
# 	2:11:IDD_CHANNEL:IDD_CHANNEL
# 	2:9:Channel.h:Channel.h
# 	2:11:Channel.cpp:Channel.cpp
# 	2:10:ENUM: enum:enum
# 	2:19:Application Include:Mixere.h
# 	2:15:CLASS: CChannel:CChannel
# End Section
# Section Mixere : {F00DC58C-49C1-4E53-854A-4BDB99679D6F}
# 	2:13:ClickSlider.h:ClickSlider.h
# 	2:15:ClickSlider.cpp:ClickSlider.cpp
# 	2:19:CLASS: CClickSlider:CClickSlider
# 	2:19:Application Include:Mixere.h
# End Section
# Section Mixere : {EA2A32AF-495B-472B-BFDF-97E00714BFF6}
# 	2:10:ENUM: enum:enum
# 	2:16:CLASS: CChanCtrl:CChanCtrl
# 	2:10:ChanCtrl.h:ChanCtrl.h
# 	2:12:ChanCtrl.cpp:ChanCtrl.cpp
# 	2:19:Application Include:Mixere.h
# End Section
# Section Mixere : {D0CA9821-FD85-4EF5-AC35-98003ED8EA3A}
# 	1:11:IDD_CONSOLE:103
# 	2:16:Resource Include:resource.h
# 	2:15:CLASS: CConsole:CConsole
# 	2:9:Console.h:Console.h
# 	2:11:IDD_CONSOLE:IDD_CONSOLE
# 	2:10:ENUM: enum:enum
# 	2:11:Console.cpp:Console.cpp
# 	2:19:Application Include:Mixere.h
# End Section
# Section Mixere : {F0B0AE0F-BE99-4669-8CAA-F79C9714E57A}
# 	2:12:StaticEdit.h:StaticEdit.h
# 	2:14:StaticEdit.cpp:StaticEdit.cpp
# 	2:19:Application Include:mixere.h
# 	2:18:CLASS: CStaticEdit:CStaticEdit
# End Section
# Section Mixere : {76901A2B-69D7-4CEA-8565-F14F7874FF82}
# 	2:16:ColorStatusBar.h:ColorStatusBar.h
# 	2:19:Application Include:Mixere.h
# 	2:18:ColorStatusBar.cpp:ColorStatusBar.cpp
# 	2:22:CLASS: CColorStatusBar:CColorStatusBar
# End Section
