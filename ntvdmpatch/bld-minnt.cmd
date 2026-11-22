@echo off
call basepath.cmd
SETLOCAL EnableDelayedExpansion
pushd %BASEPATH%
set BUILD_ALLOW_ALL_WARNINGS=1
rem Multithreaded build caused problems on some machines so disabling multithreaded build:
set BUILD_MULTIPROCESSOR=
set BUILD_CONCURRENT_PROCESSORS=
set NUMBER_OF_PROCESSORS=1
set V86MON=
if "%HAXM%"=="" set CCPU=1
if not "%HAXM%"=="" set V86MON=haxm
if not "%HYPERV%"=="" set V86MON=hyperv

rem
rem Prepare directories with language dependent versions. If user specified language,
rem modify dirs file to just compile wanted language
rem
if "%LANG%"=="" goto nospeclang
cd mvdm
set LNGDIRS=bin86 dos\command dos\dem
for %%J in (cmd dev) do (
  for /D %%I in (dos\v86\%%J\*) do set LNGDIRS=!LNGDIRS! %%I
)
set LNGDIRS=!LNGDIRS! dos\v86\redir dpmi softpc.new\base\bios softpc.new\base\keymouse softpc.new\base\video softpc.new\host\src softpc.new\obj.vdm wow32

for %%I in (!LNGDIRS!) do (
  if exist %%I\dirs.lng move /y %%I\dirs.lng %%I\dirs
  set LNGDONE=
  for %%J in (%LANG% daytona usa) do (
    if "!LNGDONE!"=="" (
      if exist %%I\%%J (
        move /y %%I\dirs %%I\dirs.lng
        echo DIRS=%%J >%%I\dirs
        set LNGDONE=1
      )
    )
  )
)
cd..
rem
rem wow16 is the one component that keys off LANG, and for it LANG does not
rem mean "compile this language" but "do not link, stamp the localized
rem resources into a binary that INTL_EXE already holds".  The 16-bit modules
rem carry no localized code, so they are built usa here and stamped per
rem language later, by mkrelease, which is the pass that has the built
rem binaries to stamp.  Null out LANG so wow16 builds usa.
set LANGX=%LANG%
set LANG=
:nospeclang

if exist ..\public\internal\net\inc\icanon.h (
  pushd ..\..\ntvdmpatch\vdmredir
  call bld.cmd
  popd
)

chcp 437
build -e

if not "%LANGX%"=="" set LANG=%LANGX%
if not "%LANG%"=="" for %%I in (!LNGDIRS!) do if exist %%I\dirs.lng move /y %%I\dirs.lng %%I\dirs

popd

