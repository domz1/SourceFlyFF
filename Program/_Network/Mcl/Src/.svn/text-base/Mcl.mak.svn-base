# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

!IF "$(CFG)" == ""
CFG=Mcl - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to Mcl - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Mcl - Win32 Release" && "$(CFG)" != "Mcl - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "Mcl.mak" CFG="Mcl - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Mcl - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Mcl - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project
# PROP Target_Last_Scanned "Mcl - Win32 Debug"
CPP=cl.exe

!IF  "$(CFG)" == "Mcl - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\lib\Release\Mcl.lib"

CLEAN : 
	-@erase "$(INTDIR)\CMclAutoLock.obj"
	-@erase "$(INTDIR)\CMclAutoPtr.obj"
	-@erase "$(INTDIR)\CMclCritSec.obj"
	-@erase "$(INTDIR)\CMclEvent.obj"
	-@erase "$(INTDIR)\CMclGlobal.obj"
	-@erase "$(INTDIR)\CMclKernel.obj"
	-@erase "$(INTDIR)\CMclMailbox.obj"
	-@erase "$(INTDIR)\CMclMonitor.obj"
	-@erase "$(INTDIR)\CMclMutex.obj"
	-@erase "$(INTDIR)\CMclSemaphore.obj"
	-@erase "$(INTDIR)\CMclSharedMemory.obj"
	-@erase "$(INTDIR)\CMclThread.obj"
	-@erase "$(INTDIR)\CMclWaitableCollection.obj"
	-@erase "..\lib\Release\Mcl.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\include" /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /Fp"$(INTDIR)/Mcl.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Mcl.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\Release\Mcl.lib"
LIB32_FLAGS=/nologo /out:"..\lib\Release\Mcl.lib" 
LIB32_OBJS= \
	"$(INTDIR)\CMclAutoLock.obj" \
	"$(INTDIR)\CMclAutoPtr.obj" \
	"$(INTDIR)\CMclCritSec.obj" \
	"$(INTDIR)\CMclEvent.obj" \
	"$(INTDIR)\CMclGlobal.obj" \
	"$(INTDIR)\CMclKernel.obj" \
	"$(INTDIR)\CMclMailbox.obj" \
	"$(INTDIR)\CMclMonitor.obj" \
	"$(INTDIR)\CMclMutex.obj" \
	"$(INTDIR)\CMclSemaphore.obj" \
	"$(INTDIR)\CMclSharedMemory.obj" \
	"$(INTDIR)\CMclThread.obj" \
	"$(INTDIR)\CMclWaitableCollection.obj"

"..\lib\Release\Mcl.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Mcl - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\lib\Debug\Mcl.lib"

CLEAN : 
	-@erase "$(INTDIR)\CMclAutoLock.obj"
	-@erase "$(INTDIR)\CMclAutoPtr.obj"
	-@erase "$(INTDIR)\CMclCritSec.obj"
	-@erase "$(INTDIR)\CMclEvent.obj"
	-@erase "$(INTDIR)\CMclGlobal.obj"
	-@erase "$(INTDIR)\CMclKernel.obj"
	-@erase "$(INTDIR)\CMclMailbox.obj"
	-@erase "$(INTDIR)\CMclMonitor.obj"
	-@erase "$(INTDIR)\CMclMutex.obj"
	-@erase "$(INTDIR)\CMclSemaphore.obj"
	-@erase "$(INTDIR)\CMclSharedMemory.obj"
	-@erase "$(INTDIR)\CMclThread.obj"
	-@erase "$(INTDIR)\CMclWaitableCollection.obj"
	-@erase "..\lib\Debug\Mcl.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /GX /Z7 /Od /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MDd /W3 /GX /Z7 /Od /I "..\include" /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /Fp"$(INTDIR)/Mcl.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\.
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Mcl.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\Debug\Mcl.lib"
LIB32_FLAGS=/nologo /out:"..\lib\Debug\Mcl.lib" 
LIB32_OBJS= \
	"$(INTDIR)\CMclAutoLock.obj" \
	"$(INTDIR)\CMclAutoPtr.obj" \
	"$(INTDIR)\CMclCritSec.obj" \
	"$(INTDIR)\CMclEvent.obj" \
	"$(INTDIR)\CMclGlobal.obj" \
	"$(INTDIR)\CMclKernel.obj" \
	"$(INTDIR)\CMclMailbox.obj" \
	"$(INTDIR)\CMclMonitor.obj" \
	"$(INTDIR)\CMclMutex.obj" \
	"$(INTDIR)\CMclSemaphore.obj" \
	"$(INTDIR)\CMclSharedMemory.obj" \
	"$(INTDIR)\CMclThread.obj" \
	"$(INTDIR)\CMclWaitableCollection.obj"

"..\lib\Debug\Mcl.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

################################################################################
# Begin Target

# Name "Mcl - Win32 Release"
# Name "Mcl - Win32 Debug"

!IF  "$(CFG)" == "Mcl - Win32 Release"

!ELSEIF  "$(CFG)" == "Mcl - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\CMclKernel.h

!IF  "$(CFG)" == "Mcl - Win32 Release"

!ELSEIF  "$(CFG)" == "Mcl - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\CMclKernel.cpp
DEP_CPP_CMCLK=\
	"..\include\CMclAutoPtr.h"\
	"..\include\CMclEvent.h"\
	"..\include\CMclGlobal.h"\
	"..\include\CMclKernel.h"\
	"..\include\CMclMutex.h"\
	"..\include\CMclSemaphore.h"\
	"..\include\CMclThread.h"\
	"..\include\CMclWaitableObject.h"\
	

"$(INTDIR)\CMclKernel.obj" : $(SOURCE) $(DEP_CPP_CMCLK) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\CMclMutex.h

!IF  "$(CFG)" == "Mcl - Win32 Release"

!ELSEIF  "$(CFG)" == "Mcl - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\CMclMutex.cpp
DEP_CPP_CMCLM=\
	"..\include\CMclGlobal.h"\
	"..\include\CMclKernel.h"\
	"..\include\CMclMutex.h"\
	"..\include\CMclWaitableObject.h"\
	

"$(INTDIR)\CMclMutex.obj" : $(SOURCE) $(DEP_CPP_CMCLM) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\CMclCritSec.h

!IF  "$(CFG)" == "Mcl - Win32 Release"

!ELSEIF  "$(CFG)" == "Mcl - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\CMclGlobal.h

!IF  "$(CFG)" == "Mcl - Win32 Release"

!ELSEIF  "$(CFG)" == "Mcl - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\CMclGlobal.cpp
DEP_CPP_CMCLG=\
	"..\include\CMclGlobal.h"\
	

"$(INTDIR)\CMclGlobal.obj" : $(SOURCE) $(DEP_CPP_CMCLG) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\CMclCritSec.cpp
DEP_CPP_CMCLC=\
	"..\include\CMclCritSec.h"\
	"..\include\CMclGlobal.h"\
	

"$(INTDIR)\CMclCritSec.obj" : $(SOURCE) $(DEP_CPP_CMCLC) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\CMclAutoLock.h

!IF  "$(CFG)" == "Mcl - Win32 Release"

!ELSEIF  "$(CFG)" == "Mcl - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\CMclAutoLock.cpp
DEP_CPP_CMCLA=\
	"..\include\CMclAutoLock.h"\
	"..\include\CMclCritSec.h"\
	"..\include\CMclGlobal.h"\
	"..\include\CMclKernel.h"\
	"..\include\CMclMutex.h"\
	"..\include\CMclWaitableObject.h"\
	

"$(INTDIR)\CMclAutoLock.obj" : $(SOURCE) $(DEP_CPP_CMCLA) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\CMclThread.cpp
DEP_CPP_CMCLT=\
	"..\include\CMclGlobal.h"\
	"..\include\CMclKernel.h"\
	"..\include\CMclThread.h"\
	"..\include\CMclWaitableObject.h"\
	

"$(INTDIR)\CMclThread.obj" : $(SOURCE) $(DEP_CPP_CMCLT) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\CMclThread.h

!IF  "$(CFG)" == "Mcl - Win32 Release"

!ELSEIF  "$(CFG)" == "Mcl - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\CMcl.h

!IF  "$(CFG)" == "Mcl - Win32 Release"

!ELSEIF  "$(CFG)" == "Mcl - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\CMclSemaphore.h

!IF  "$(CFG)" == "Mcl - Win32 Release"

!ELSEIF  "$(CFG)" == "Mcl - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\CMclSemaphore.cpp
DEP_CPP_CMCLS=\
	"..\include\CMclGlobal.h"\
	"..\include\CMclKernel.h"\
	"..\include\CMclSemaphore.h"\
	"..\include\CMclWaitableObject.h"\
	

"$(INTDIR)\CMclSemaphore.obj" : $(SOURCE) $(DEP_CPP_CMCLS) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\CMclAutoPtr.cpp
DEP_CPP_CMCLAU=\
	"..\include\CMclAutoPtr.h"\
	"..\include\CMclEvent.h"\
	"..\include\CMclGlobal.h"\
	"..\include\CMclKernel.h"\
	"..\include\CMclMutex.h"\
	"..\include\CMclSemaphore.h"\
	"..\include\CMclThread.h"\
	"..\include\CMclWaitableObject.h"\
	

"$(INTDIR)\CMclAutoPtr.obj" : $(SOURCE) $(DEP_CPP_CMCLAU) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\CMclAutoPtr.h

!IF  "$(CFG)" == "Mcl - Win32 Release"

!ELSEIF  "$(CFG)" == "Mcl - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\CMclEvent.h

!IF  "$(CFG)" == "Mcl - Win32 Release"

!ELSEIF  "$(CFG)" == "Mcl - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\CMclEvent.cpp
DEP_CPP_CMCLE=\
	"..\include\CMclEvent.h"\
	"..\include\CMclGlobal.h"\
	"..\include\CMclKernel.h"\
	"..\include\CMclWaitableObject.h"\
	

"$(INTDIR)\CMclEvent.obj" : $(SOURCE) $(DEP_CPP_CMCLE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\CMclWaitableObject.h

!IF  "$(CFG)" == "Mcl - Win32 Release"

!ELSEIF  "$(CFG)" == "Mcl - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\CMclMonitor.h

!IF  "$(CFG)" == "Mcl - Win32 Release"

!ELSEIF  "$(CFG)" == "Mcl - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\CMclMonitor.cpp
DEP_CPP_CMCLMO=\
	"..\include\CMclEvent.h"\
	"..\include\CMclGlobal.h"\
	"..\include\CMclKernel.h"\
	"..\include\CMclMonitor.h"\
	"..\include\CMclSemaphore.h"\
	"..\include\CMclWaitableObject.h"\
	

"$(INTDIR)\CMclMonitor.obj" : $(SOURCE) $(DEP_CPP_CMCLMO) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\CMclSharedMemory.h

!IF  "$(CFG)" == "Mcl - Win32 Release"

!ELSEIF  "$(CFG)" == "Mcl - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\CMclSharedMemory.cpp
DEP_CPP_CMCLSH=\
	"..\include\CMclGlobal.h"\
	"..\include\CMclSharedMemory.h"\
	

"$(INTDIR)\CMclSharedMemory.obj" : $(SOURCE) $(DEP_CPP_CMCLSH) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\CMclWaitableCollection.cpp
DEP_CPP_CMCLW=\
	"..\include\CMclGlobal.h"\
	"..\include\CMclWaitableCollection.h"\
	"..\include\CMclWaitableObject.h"\
	

"$(INTDIR)\CMclWaitableCollection.obj" : $(SOURCE) $(DEP_CPP_CMCLW) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\CMclWaitableCollection.h

!IF  "$(CFG)" == "Mcl - Win32 Release"

!ELSEIF  "$(CFG)" == "Mcl - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\CMclMailbox.h

!IF  "$(CFG)" == "Mcl - Win32 Release"

!ELSEIF  "$(CFG)" == "Mcl - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\CMclMailbox.cpp
DEP_CPP_CMCLMA=\
	"..\include\CMclAutoPtr.h"\
	"..\include\CMclEvent.h"\
	"..\include\CMclGlobal.h"\
	"..\include\CMclKernel.h"\
	"..\include\CMclMailbox.h"\
	"..\include\CMclMutex.h"\
	"..\include\CMclSemaphore.h"\
	"..\include\CMclSharedMemory.h"\
	"..\include\CMclThread.h"\
	"..\include\CMclWaitableCollection.h"\
	"..\include\CMclWaitableObject.h"\
	

"$(INTDIR)\CMclMailbox.obj" : $(SOURCE) $(DEP_CPP_CMCLMA) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\CMclLinkedLists.h

!IF  "$(CFG)" == "Mcl - Win32 Release"

!ELSEIF  "$(CFG)" == "Mcl - Win32 Debug"

!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
