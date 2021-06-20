@echo off
call basepath.cmd
SETLOCAL EnableDelayedExpansion
pushd %BASEPATH%
set BUILD_ALLOW_ALL_WARNINGS=1
if "%HAXM%"=="" set CCPU=1

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
rem Currently, wow16 does'nt contain language dependent resources.
rem Therefore we have to work around and null out LANG so that build script
rem doesn't search for specific langauge yet.
set LANGX=%LANG%
set LANG=
:nospeclang

chcp 437
build -e

if not "%LANGX%"=="" set LANG=%LANGX%
if not "%LANG%"=="" for %%I in (!LNGDIRS!) do if exist %%I\dirs.lng move /y %%I\dirs.lng %%I\dirs

popd
