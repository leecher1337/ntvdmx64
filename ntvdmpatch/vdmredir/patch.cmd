@echo off
setlocal ENABLEDELAYEDEXPANSION

echo Patching VDMREDIR...

REM Setup environment
set REDIRPATCHROOT=%CD%
call ..\basepath.cmd
set minntfix=minntfix
if not exist %minntfix%\minnt\public\internal\ds\inc\dosprint.h (
  echo It seems you have not prepared %minntfix% directory yet.
  echo Please run prepare.cmd first.
  pause
  goto fini
)

echo Copying fixed files to minnt repository
xcopy /E /R /Y %minntfix% %BASEPATH%\..\..\

echo Patching for Win7+ compatibility...
pushd %BASEPATH%\..
for %%I IN (%REDIRPATCHROOT%\patches\*.patch) do %PATCHROOT%\util\patch.exe -N -p0 -i %%I
popd

echo Done, now your MINNT build environment should be in a working condition.
if not "%1"=="batch" pause

:fini
