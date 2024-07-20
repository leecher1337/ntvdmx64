@echo off
if "%1"=="" goto usage
if not exist "%1\SetEnv.cmd" goto usage

if "%2"=="" (
  if exist "%MSVCDir%\bin\VcVars32.bat" goto vcok
  if exist "C:\PROGRA~2\MICROS~1\VC98\bin\VcVars32.bat" (
    call C:\PROGRA~2\MICROS~1\VC98\bin\VcVars32.bat
    goto vcok
  )
  goto usage
) else (
  if exist %2\VcVars32.bat (
    call %2\VcVars32.bat
    goto vcok
  )
  if exist %2\bin\VcVars32.bat (
    call %2\bin\VcVars32.bat
    goto vcok
  )
  goto usage
)
:vcok

del /Q Release\*.*
call %1\setenv /XP32 /RETAIL
nmake -f pifprop.mak
mkdir Release32 2>nul
move /y Release\pifprop.dll Release32\

goto fini
:usage
echo Usage: %0 [Windows 2003 Platform SDK path] [Visual Studio 6 Path]
:fini
