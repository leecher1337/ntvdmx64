# Microsoft Developer Studio Generated NMAKE File, Based on pificon.dsp
!IF "$(CFG)" == ""
CFG=pificon - Win32 Release
!MESSAGE Keine Konfiguration angegeben. pificon - Win32 Debug wird als Standard verwendet.
!ENDIF 

!IF "$(CFG)" != "pificon - Win32 Release" && "$(CFG)" != "pificon - Win32 Debug"
!MESSAGE UngÅltige Konfiguration "$(CFG)" angegeben.
!MESSAGE Sie kînnen beim AusfÅhren von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "pificon.mak" CFG="pificon - Win32 Debug"
!MESSAGE 
!MESSAGE FÅr die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "pificon - Win32 Release" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "pificon - Win32 Debug" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR Eine ungÅltige Konfiguration wurde angegeben.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "pificon - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\pificon.dll"


CLEAN :
	-@erase "$(INTDIR)\clsfactory.obj"
	-@erase "$(INTDIR)\dllmain.obj"
	-@erase "$(INTDIR)\pificon.obj"
	-@erase "$(INTDIR)\pifprop.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\pificon.dll"
	-@erase "$(OUTDIR)\pificon.exp"
	-@erase "$(OUTDIR)\pificon.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /GS- /W3 /GX /Gz /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "UNICODE" /D "_UNICODE" /Fp"$(INTDIR)\pificon.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\pificon.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=shell32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\pificon.pdb" /def:".\pificon.def" /out:"$(OUTDIR)\pificon.dll" /implib:"$(OUTDIR)\pificon.lib" 
DEF_FILE= \
	".\pificon.def"
LINK32_OBJS= \
	"$(INTDIR)\clsfactory.obj" \
	"$(INTDIR)\dllmain.obj" \
	"$(INTDIR)\pificon.obj" \
	"$(INTDIR)\pifprop.obj"

"$(OUTDIR)\pificon.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "pificon - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\pificon.dll" "$(OUTDIR)\pificon.bsc"


CLEAN :
	-@erase "$(INTDIR)\clsfactory.obj"
	-@erase "$(INTDIR)\clsfactory.sbr"
	-@erase "$(INTDIR)\dllmain.obj"
	-@erase "$(INTDIR)\dllmain.sbr"
	-@erase "$(INTDIR)\pificon.obj"
	-@erase "$(INTDIR)\pificon.sbr"
	-@erase "$(INTDIR)\pifprop.obj"
	-@erase "$(INTDIR)\pifprop.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\pificon.bsc"
	-@erase "$(OUTDIR)\pificon.dll"
	-@erase "$(OUTDIR)\pificon.exp"
	-@erase "$(OUTDIR)\pificon.ilk"
	-@erase "$(OUTDIR)\pificon.lib"
	-@erase "$(OUTDIR)\pificon.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "UNICODE" /D "_UNICODE" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\pificon.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\pificon.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\clsfactory.sbr" \
	"$(INTDIR)\dllmain.sbr" \
	"$(INTDIR)\pificon.sbr" \
	"$(INTDIR)\pifprop.sbr"

"$(OUTDIR)\pificon.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\pificon.pdb" /debug /def:".\pificon.def" /out:"$(OUTDIR)\pificon.dll" /implib:"$(OUTDIR)\pificon.lib" /pdbtype:sept 
DEF_FILE= \
	".\pificon.def"
LINK32_OBJS= \
	"$(INTDIR)\clsfactory.obj" \
	"$(INTDIR)\dllmain.obj" \
	"$(INTDIR)\pificon.obj" \
	"$(INTDIR)\pifprop.obj"

"$(OUTDIR)\pificon.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("pificon.dep")
!INCLUDE "pificon.dep"
!ELSE 
!MESSAGE Warning: cannot find "pificon.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "pificon - Win32 Release" || "$(CFG)" == "pificon - Win32 Debug"
SOURCE=.\clsfactory.c

!IF  "$(CFG)" == "pificon - Win32 Release"


"$(INTDIR)\clsfactory.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pificon - Win32 Debug"


"$(INTDIR)\clsfactory.obj"	"$(INTDIR)\clsfactory.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\dllmain.c

!IF  "$(CFG)" == "pificon - Win32 Release"


"$(INTDIR)\dllmain.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pificon - Win32 Debug"


"$(INTDIR)\dllmain.obj"	"$(INTDIR)\dllmain.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\pificon.c

!IF  "$(CFG)" == "pificon - Win32 Release"


"$(INTDIR)\pificon.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pificon - Win32 Debug"


"$(INTDIR)\pificon.obj"	"$(INTDIR)\pificon.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\pifprop.c

!IF  "$(CFG)" == "pificon - Win32 Release"


"$(INTDIR)\pifprop.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pificon - Win32 Debug"


"$(INTDIR)\pifprop.obj"	"$(INTDIR)\pifprop.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 


!ENDIF 

