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

all: wifeman.dll wifeman.sym
     -binplace wifeman.dll wifeman.map wifeman.sym

clean: cleanup all

cleanup:
    if exist *.lrf del *.lrf
    if exist *.obj del *.obj
    if exist *.dll del *.dll
    if exist *.map del *.map
    if exist *.sym del *.sym
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

wifeman.res: $*.rc $*.rcv ..\inc\common.ver

!ENDIF
