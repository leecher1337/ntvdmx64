@echo off
if "%1"=="" goto usage
if not exist %1\SetEnv.cmd goto usage

del /Q Release\*.*
call %1\setenv /XP64 /RETAIL
nmake -f pificon.mak
mkdir Release64 2>nul
move /y Release\pificon.dll Release64\

goto fini
:usage
echo Usage: %0 [Windows 2003 Platform SDK path]
:fini
