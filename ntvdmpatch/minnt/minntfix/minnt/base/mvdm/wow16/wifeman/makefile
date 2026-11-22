# wifeman makefile
#
# Copyright (c) 1991, Microsoft Corporation
#
# History:
#   Created  02-Oct-1992 David Treadwell (davidtr)
#

!IFDEF USEBUILD

# If using BUILD.EXE, edit .\sources. if you want to add a new source
# file to this component.  This file merely indirects to the real make file
# that is shared by all the components of NT OS/2.

!INCLUDE $(NTMAKEENV)\makefile.def

!ELSE

.SUFFIXES:
.SUFFIXES: .c   .asm .h   .inc .obj .lst .sys .exe .com .map .sym .def .lib .dll .rc .res


!ifdef INCLUDE
INCS    =
!else
INCS    = -I..\inc -I..\..\inc
!endif

# DEFINES = -DWOW -DDEBUG $(MVDMFLAGS)
DEFINES = -DWOW -DFE_SB $(MVDMFLAGS)

AOBJ    = -DmemS=1 -w2 $(DEFINES) $(INCS)

CW16    = -AS -G2sw -Os -W3 -Zp $(DEFINES) $(INCS)
CW16B   = $(CW16) -B1 c1l.exe -B2 c2l.exe -B3 c3l.exe

LINK    = /map /align:16

########## Path definition so we find 16 bit tools ##########
# Also works around stupid bug in RC 3.1 that doesn't allow rcpp.err to be
# in a directory that is greater than 128 chars down the path, even if
# rc 3.1 is running as an OS/2 app.

PATH    = $(_NTBINDIR)\private\mvdm\tools16;$(PATH)

!if "$(NTDEBUG)"!="" && "$(NTDEBUG)"!="retail" && "$(NTDEBUG)" != "ntsdnodbg"
AOBJ    = $(AOBJ) -Zd
CW16    = $(CW16) /Od /Oi /Zd
LINK    = $(LINK) /LI
!endif

W16LIBS = ..\lib\sdllcew.lib

#international mods
#note INTL_SRC, INTL_EXE and LANG are external macros set by international.
#wifeman was never part of the international build, so the core\wifeman
#sub-path is ours; the resources themselves are recovered from the shipped
#localized wifeman.dll by ntvdmpatch\src\wow16res.
!IFNDEF LANG
RES_DIR = .\messages\usa
!ELSE
RES_DIR = $(INTL_SRC)\$(LANG)\core\wifeman
EXE_DIR = $(INTL_EXE)
!ENDIF


.asm.obj:
    masm $(AOBJ) $*;

.asm.lst:
    masm $(AOBJ) -l $*,nul,$*.lst;


.c.obj:
    cl16 -c -nologo $(CW16) $*.c

.c.lst:
    cl16 -c -nologo $(CW16) -Fonul -Fc$*.lst $*.c


.def.lib:
    implib $*.lib $*.def

.map.sym:
    mapsym $*

.rc.res:
    rc16 $(INCS) -r $*.rc

!IFNDEF LANG
all: wifeman.dll wifeman.sym
     -binplace wifeman.dll wifeman.map wifeman.sym
!ELSE
all: wifeman.$(LANG)
!ENDIF

clean: cleanup all

cleanup:
    if exist *.lrf del *.lrf
    if exist *.obj del *.obj
    if exist *.dll del *.dll
    if exist *.map del *.map
    if exist *.sym del *.sym
    if exist *.rc  del *.rc
    if exist *.rcv del *.rcv
    if exist *.res del *.res

wifethk.obj: $*.asm ..\..\inc\wow.inc ..\..\inc\wowwife.inc
    masm $(AOBJ) $*;

wifeman.lrf: makefile
    echo $*.obj wifethk.obj libentry.obj fakewife.obj >$@
    echo $*.dll>>$@
    echo $* $(LINK)>>$@
    echo ..\lib\libw.lib /nod>>$@
    echo $*;>>$@

wifeman.dll: wifethk.obj libentry.obj fakewife.obj $*.obj $*.lrf $*.def $*.res
    link16 @$*.lrf;
    rc16 -t $*.res $@

wifeman.rc: $(RES_DIR)\$@
    @copy $(RES_DIR)\$@

wifeman.rcv: $(RES_DIR)\$@
    @copy $(RES_DIR)\$@

wifeman.res: $*.rc $*.rcv ..\inc\common.ver

!IFDEF LANG
wifeman.$(LANG): wifeman.res
    copy $(EXE_DIR)\wifeman.dll wifeman.$(LANG)
    rc16 -t wifeman.res wifeman.$(LANG)
!ENDIF

!ENDIF
