@echo off
SETLOCAL EnableDelayedExpansion
if "%1"=="" goto usage
if "%2"=="" goto usage

rem --
rem -- Files required from NT 4 sources
rem --
set PROBE_DIRS=old-src\nt\private nt\private private Windows-NT-4.0-Source-master\private
set PIF_FILES=windows\shell\shelldll\doshelp.h windows\shell\common\inc\debug.h
call :extract NT4 %1

rem --
rem -- Files required from Win2k sources
rem --
set PROBE_DIRS=win2k-master\trunk\private private
set PIF_FILES=genx\shell\inc\uastrfnc.h shell\shlwapi\uastrfnc.c 
for %%I in (commobj.c commobj.h defext.c defext.h ids.h pifdat.c piffnt.c piffntp.h pifhot.c pifhotp.h pifinf.c pifinfp.h piflib.c pifmem.c pifmgr.c pifmgrp.h pifmsc.c pifprg.c pifsub.c pifvid.c proxynt.c shellvm.h) do set PIF_FILES=!PIF_FILES! shell\shell32\%%I
for %%I in (ccstock.h debug.h pif.h port32.h shguidp.h) do set PIF_FILES=!PIF_FILES! shell\inc\%%I
call :extract Win2k %2

..\..\util\patch.exe -p0 <pifprop.patch
echo Done.
goto fini

:usage
echo Usage: %0 [NT4 sources] [Win2k sources]
echo.
goto fini

:extract
rem %1 -- Name (NT4/Win2k)
rem %2 -- zip file or directory

SETLOCAL EnableDelayedExpansion

for %%I in (%PIF_FILES%) do (
  set FIRST_FILE=%%I
  set FIRST_BASE=%%~nxI
  goto fil_fnd
)
:fil_fnd
for %%I in (.7z .zip .rar) do if "%~x2"=="%%~I" goto expand_nt
for %%I in (%PROBE_DIRS%) do if exist "%2\%%~I\nul"
(
  set PFX=%%I
  goto copy_nt
)
echo %1 sourcecode path could not be detected, please point me to Path of nt-Directory
goto fini
:copy_nt
for %%I in (%PIF_FILES%) do copy /Y %PFX%\%%I .
goto check_nt

:expand_nt
del %FIRST_BASE% 2>nul
for %%D in (%PROBE_DIRS%) do (
7z l %2 %%D\%FIRST_FILE% | find /I "%FIRST_BASE%"
  if not errorlevel 1 (
    set PFX=%%D
    goto exnt_found
  )
)
echo Unable to detect base path of %1 sources, please adjust PROBE_DIRS in this batch file to base path of your archive
goto fini
:exnt_found
set PFX_FILES=
for %%I in (%PIF_FILES%) do set PFX_FILES=!PFX_FILES! %PFX%\%%I
7z e %2 %PFX_FILES%

:check_nt
for %%I in (%PIF_FILES%) do (
  if not exist %%~nxI (
    echo Files %%~nxI could not be found, failure, exiting
    goto fini
  )
)
exit /b 0


:fini
