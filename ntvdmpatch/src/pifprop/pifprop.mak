# Microsoft Developer Studio Generated NMAKE File, Based on pifprop.dsp
!IF "$(CFG)" == ""
CFG=pifprop - Win32 Release
!MESSAGE Keine Konfiguration angegeben. pifprop - Win32 Release wird als Standard verwendet.
!ENDIF 

!IF "$(CFG)" != "pifprop - Win32 Release" && "$(CFG)" != "pifprop - Win32 Debug"
!MESSAGE UngÅltige Konfiguration "$(CFG)" angegeben.
!MESSAGE Sie kînnen beim AusfÅhren von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "pifprop.mak" CFG="pifprop - Win32 Debug"
!MESSAGE 
!MESSAGE FÅr die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "pifprop - Win32 Release" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "pifprop - Win32 Debug" (basierend auf  "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "pifprop - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\pifprop.dll"


CLEAN :
	-@erase "$(INTDIR)\commobj.obj"
	-@erase "$(INTDIR)\defext.obj"
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\pifdat.obj"
	-@erase "$(INTDIR)\piffnt.obj"
	-@erase "$(INTDIR)\pifhot.obj"
	-@erase "$(INTDIR)\pifinf.obj"
	-@erase "$(INTDIR)\piflib.obj"
	-@erase "$(INTDIR)\pifmem.obj"
	-@erase "$(INTDIR)\pifmgr.obj"
	-@erase "$(INTDIR)\pifmsc.obj"
	-@erase "$(INTDIR)\pifprg.obj"
	-@erase "$(INTDIR)\pifsub.obj"
	-@erase "$(INTDIR)\pifvid.obj"
	-@erase "$(INTDIR)\proxynt.obj"
	-@erase "$(INTDIR)\uastrfnc.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\pifprop.dll"
	-@erase "$(OUTDIR)\pifprop.exp"
	-@erase "$(OUTDIR)\pifprop.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /GS- /W3 /GX /Gz /O2 /I "." /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "UNICODE" /Fp"$(INTDIR)\pifprop.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\pifprop.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=shell32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib ole32.lib oleaut32.lib uuid.lib shlwapi.lib comctl32.lib /nologo /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\pifprop.pdb" /def:".\pifprop.def" /force /out:"$(OUTDIR)\pifprop.dll" /implib:"$(OUTDIR)\pifprop.lib" 
DEF_FILE= \
	".\pifprop.def"
LINK32_OBJS= \
	"$(INTDIR)\proxynt.obj" \
	"$(INTDIR)\main.obj" \
	"$(INTDIR)\pifdat.obj" \
	"$(INTDIR)\piffnt.obj" \
	"$(INTDIR)\pifhot.obj" \
	"$(INTDIR)\pifinf.obj" \
	"$(INTDIR)\piflib.obj" \
	"$(INTDIR)\pifmem.obj" \
	"$(INTDIR)\pifmgr.obj" \
	"$(INTDIR)\pifmsc.obj" \
	"$(INTDIR)\pifprg.obj" \
	"$(INTDIR)\pifsub.obj" \
	"$(INTDIR)\pifvid.obj" \
	"$(INTDIR)\uastrfnc.obj" \
	"$(INTDIR)\commobj.obj" \
	"$(INTDIR)\defext.obj"

"$(OUTDIR)\pifprop.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "pifprop - Win32 Debug"

OUTDIR=.\pifprop___Win32_Debug
INTDIR=.\pifprop___Win32_Debug
# Begin Custom Macros
OutDir=.\pifprop___Win32_Debug
# End Custom Macros

ALL : "$(OUTDIR)\pifprop.dll"


CLEAN :
	-@erase "$(INTDIR)\commobj.obj"
	-@erase "$(INTDIR)\defext.obj"
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\pifdat.obj"
	-@erase "$(INTDIR)\piffnt.obj"
	-@erase "$(INTDIR)\pifhot.obj"
	-@erase "$(INTDIR)\pifinf.obj"
	-@erase "$(INTDIR)\piflib.obj"
	-@erase "$(INTDIR)\pifmem.obj"
	-@erase "$(INTDIR)\pifmgr.obj"
	-@erase "$(INTDIR)\pifmsc.obj"
	-@erase "$(INTDIR)\pifprg.obj"
	-@erase "$(INTDIR)\pifsub.obj"
	-@erase "$(INTDIR)\pifvid.obj"
	-@erase "$(INTDIR)\proxynt.obj"
	-@erase "$(INTDIR)\uastrfnc.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\pifprop.dll"
	-@erase "$(OUTDIR)\pifprop.exp"
	-@erase "$(OUTDIR)\pifprop.ilk"
	-@erase "$(OUTDIR)\pifprop.lib"
	-@erase "$(OUTDIR)\pifprop.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "." /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "UNICODE" /Fp"$(INTDIR)\pifprop.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\pifprop.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib ole32.lib oleaut32.lib uuid.lib shlwapi.lib comctl32.lib shell32.lib /nologo /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\pifprop.pdb" /debug /def:".\pifprop.def" /force /out:"$(OUTDIR)\pifprop.dll" /implib:"$(OUTDIR)\pifprop.lib" /pdbtype:sept 
DEF_FILE= \
	".\pifprop.def"
LINK32_OBJS= \
	"$(INTDIR)\proxynt.obj" \
	"$(INTDIR)\main.obj" \
	"$(INTDIR)\pifdat.obj" \
	"$(INTDIR)\piffnt.obj" \
	"$(INTDIR)\pifhot.obj" \
	"$(INTDIR)\pifinf.obj" \
	"$(INTDIR)\piflib.obj" \
	"$(INTDIR)\pifmem.obj" \
	"$(INTDIR)\pifmgr.obj" \
	"$(INTDIR)\pifmsc.obj" \
	"$(INTDIR)\pifprg.obj" \
	"$(INTDIR)\pifsub.obj" \
	"$(INTDIR)\pifvid.obj" \
	"$(INTDIR)\uastrfnc.obj" \
	"$(INTDIR)\commobj.obj" \
	"$(INTDIR)\defext.obj"

"$(OUTDIR)\pifprop.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("pifprop.dep")
!INCLUDE "pifprop.dep"
!ELSE 
!MESSAGE Warning: cannot find "pifprop.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "pifprop - Win32 Release" || "$(CFG)" == "pifprop - Win32 Debug"
SOURCE=.\commobj.c

"$(INTDIR)\commobj.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\defext.c

"$(INTDIR)\defext.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\main.c

"$(INTDIR)\main.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\pifdat.c

"$(INTDIR)\pifdat.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\piffnt.c

"$(INTDIR)\piffnt.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\pifhot.c

"$(INTDIR)\pifhot.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\pifinf.c

"$(INTDIR)\pifinf.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\piflib.c

"$(INTDIR)\piflib.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\pifmem.c

"$(INTDIR)\pifmem.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\pifmgr.c

"$(INTDIR)\pifmgr.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\pifmsc.c

"$(INTDIR)\pifmsc.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\pifprg.c

"$(INTDIR)\pifprg.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\pifsub.c

"$(INTDIR)\pifsub.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\pifvid.c

"$(INTDIR)\pifvid.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\proxynt.c

"$(INTDIR)\proxynt.obj" : $(SOURCE) "$(INTDIR)"

SOURCE=.\uastrfnc.c

"$(INTDIR)\uastrfnc.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

