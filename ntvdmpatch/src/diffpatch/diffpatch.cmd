@echo off

rem Configuration section:
rem
rem Ensure that patch and diff are in path:
if "%ProgramW6432%"=="" set ProgramW6432=%ProgramFiles%
set PATH=%PATH%;%ProgramW6432%\git\usr\bin

rem
rem Set to directory where current patches are located
set PATCHROOT=f:\Windows.Source.Code.w2k.nt4.wxp\ntvdmpatch\ntvdmpatch

rem
rem Set to path where original unpatched MINNT source is located
set ORIGSRC=f:\Windows.Source.Code.w2k.nt4.wxp\minnt\q160765803-minint4-85fac4faadc7\base

if not exist %PATCHROOT%\patches\minnt\ccpu.patch (
echo Please setup PATCHROOT and retry.
exit /B
)

if not exist %ORIGSRC%\mvdm\dirs (
echo Please setup ORIGSRC and retry.
exit /B
)

if "%1"=="" goto usage:

rem Here we put the unified diffs that get generated temporarily
rmdir /s /q unified 2>nul
md unified 2>nul

setlocal enabledelayedexpansion


rem
rem Create old-Directory with template from new directory
rem
rmdir /s /q old 2>nul
md old 2>nul

xcopy /s /e /i /Y /Q new old 
cd old
for /r %%f in (*) do (
  set B=%%f
  xcopy /s /e /i /Y /Q %ORIGSRC%\!B:%CD%\=! !B:%CD%\=!
)
cd ..

rmdir /s /q cur 2>nul
xcopy /s /e /i /Y /Q old cur 
cd cur

set PATFILE=%1


rem
rem Create a list of patches that are relevant
rem
set n=0
for %%I IN (%PATCHROOT%\patches\common\*.patch %PATCHROOT%\patches\minnt\*.patch %PATCHROOT%\experimental\haxm\minnt\*.patch %PATCHROOT%\experimental\haxm\haxm-dpmi.patch %PATCHROOT%\experimental\adlib\*.patch %PATCHROOT%\experimental\vesa\*.patch) do (
for /r %%f in (*) do (
  set B=%%f
  if not "%LASTFILE%"=="%%I" (
    find /i "!B:%CD%\=!" %%I >nul
    if not errorlevel 1 (
      set /A n+=1
      set patfiles[!n!]=%%I
      set basefiles[!n!]=%%~nxI
      set LASTFILE=%%I
    )
  )
)
)


rem
rem Now create a "current" version for the files and create a unified version of the patches
rem
set hit=
for /L %%I in (1,1,%n%) do (
  if !hit!==1 (
    cd..
    rmdir /s /q comp 2>nul
    xcopy /s /e /i /Y /Q cur comp
    cd cur 
    patch.exe --no-backup-if-mismatch -s -t -N -p2 -i !patfiles[%%I]! 2>nul >nul
    diff -Naur --strip-trailing-cr ..\comp ..\cur >..\unified\!basefiles[%%I]!
  ) else (
    patch.exe --no-backup-if-mismatch -s -t -N -p2 -i !patfiles[%%I]! 2>nul >nul
    if "!basefiles[%%I]!"=="%PATFILE%" set hit=1
  )
)


if not !hit!==1 (
  echo Did not find %PATFILE% in patch list
  cd..
  goto cleanup:
)


rem
rem Next, create unified diff of current files with latest master
rem
del ..\changes.diff  2>nul
for /r %%f in (*) do (
  set B=%%f
  diff -Naur --strip-trailing-cr ..\cur\!B:%CD%\=! ..\new\!B:%CD%\=! >>..\changes.diff
)


rem
rem Now start with a fresh copy and apply patches until we found our existing patch file
rem
cd ..
rmdir /s /q result 2>nul
md result 2>nul
rmdir /s /q cur 2>nul
xcopy /s /e /i /Y /Q old cur 
cd cur
set hit=
for /L %%I in (1,1,%n%) do (
  if !hit! == 1 (
    rem Apply unified patch
    patch.exe --no-backup-if-mismatch -s -t -N -p2 -i ..\unified\!basefiles[%%I]! 2>nul >nul
    cd..
    diff -Nar --strip-trailing-cr comp cur >result\!basefiles[%%I]!_
    rem CRLF conversion
    type result\!basefiles[%%I]!_ | FIND "" /V >result\!basefiles[%%I]!
    del result\!basefiles[%%I]!_
    rmdir /s /q comp 2>nul
    xcopy /s /e /i /Y /Q cur comp
    cd cur
  ) else (    
    if "!basefiles[%%I]!"=="%PATFILE%" (
      rem If unified diffs previously crated can be applied, we are safe to create the rest
      set hit=1

      rem Save current state for comparison
      cd..
      rmdir /s /q comp 2>nul
      xcopy /s /e /i /Y /Q cur comp
      cd cur 

      rem Apply old changes and new patches
      patch.exe --no-backup-if-mismatch -s -t -N -p2 -i !patfiles[%%I]! 2>nul >nul
      patch.exe --no-backup-if-mismatch -s -t -N -p2 -i ..\changes.diff
      cd ..
      diff -Nar --strip-trailing-cr comp cur >result\!basefiles[%%I]!_
      type result\!basefiles[%%I]!_ | FIND "" /V >result\!basefiles[%%I]!
      del result\!basefiles[%%I]!_
      rmdir /s /q comp 2>nul
      xcopy /s /e /i /Y /Q cur comp
      cd cur
    ) else (
      patch.exe --no-backup-if-mismatch -s -t -N -p2 -i !patfiles[%%I]! 2>nul >nul
    )
  )
)

cd ..


diff -Nar --strip-trailing-cr cur new >nul
if errorlevel 1 (
echo -------------------------------------------------
echo WARNING WARNING WARNING
echo Differences found, not all patchsets applied correctly?
echo Result may be unusable
echo -------------------------------------------------
) else (
:cleanup
rmdir /s /q unified 2>nul
rmdir /s /q comp 2>nul
rmdir /s /q old 2>nul
rmdir /s /q cur 2>nul
del changes.diff 2>nul
)

exit /B

:usage
echo Usage: %0 [patchfile where to incorporate]
echo.
echo Follow these steps:
echo 1) Set up "new" directory with directory structure starting with mvdm
echo    that contains the patched new files that need to be integrated.
echo    For multiple merges, they need to contain just the desired merge.
echo 2) Call this script with the patch file where to inpororate the
echo    patched files from new-directory.
echo 3) Result files will be in result\ directory. These are the diffs for the
echo    files that need to be merged into patchfiles source manually.
echo.
echo Example: You have patched nt_graph.c and want to incorporate it into
echo          ega_crash.patch:
echo 1) Place the following file in new:
echo    new\mvdm\softpc.new\host\src\nt_graph.c
echo 2) Run script with:
echo    %0 ega_crash.patch
echo.
