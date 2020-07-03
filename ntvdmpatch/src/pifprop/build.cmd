@echo off
if "%1"=="" goto usage
if not exist %1\SetEnv.cmd goto usage
call %1\setenv /XP64
nmake -f pifprop.mak
goto fini
:usage
echo Usage: %0 [Windows 2003 Platform SDK path]
:fini
