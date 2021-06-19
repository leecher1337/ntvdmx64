@echo off
rem Automated build script for NTVDMx64
rem -----------------------------------
rem Madantory requirements:
rem 
rem   MinNT-20170416-85fac4faadc77203db8ddc66af280a75c1b717b0.zip
rem   old-src.trunk.r687.20150728.7z
rem
rem Optional environment vars:
rem
rem   PREREQ    Directory, where prerequired files are found. Default is current dir
rem   SRCDIR    Directory, where current ntvdmpatch source folder can be found.
rem             Not useful for end-users, just for automated testing on dev machine
rem   WKDIR     Working directory for build process, default is %CD%\w
rem   HAXBLD    If set to -haxm then HAXM build is built, otherwise normal build
rem   DBGSTP    If set, pause is issued after every step, useful for testing build
rem   SIZ_NTBLD [chk, fre] Checked of free build, default is checked (debug)
rem   NOWIN7    Ignore missing Windows 7 ISO (i.e. build only for Win < 8 or no OLE2)
rem

echo ----------------------------------------------------
echo Autobuild NTVDMx64
echo ----------------------------------------------------
echo.

if exist "%ProgramFiles%\7-Zip" set PATH=%PATH:)=^)%;"%ProgramFiles%\7-Zip"
7z >nul 2>&1
if errorlevel 255 (
for /F "skip=2 tokens=3*" %%r in ('reg query "HKEY_LOCAL_MACHINE\SOFTWARE\7-zip" /v Path') do echo set PATH=%PATH:)=^)%;%%r
7z >nul 2>&1
if errorlevel 255 (
echo Please install 7zip first, then run again
start http://www.7-zip.de
pause
exit /b
)
)

set ABPATH=%CD%
if "%PREREQ%"=="" set PREREQ=%CD%\
if "%WKDIR%"=="" set WKDIR=%CD%\w
if exist "%WKDIR%\nul" (
  echo Working directory %WKDIR% exists.
  echo As it gets deleted after build, please remove it prior to executing this script.
  pause
  exit /B
)
md %WKDIR%
if not exist "%WKDIR%" (
  pause
  exit /B
)

echo Preparing...
call :fetchprq

rem -- Path contains spaces, bad, do subst
set NEEDSUBST=
if not "%WKDIR%"=="%WKDIR: =%" (
  echo [!] Consider using a pathname without space in it
  SET NEEDSUBST=1
)

rem -- Path contains a hyphen, bad, do subst
if not "%WKDIR%"=="%WKDIR:-=%" (
  echo [!] Consider using a pathname without - in it
  SET NEEDSUBST=1
)

rem -- Path is too long, bad, do subst
setlocal enableDelayedExpansion
if not [%WKDIR:~0,-30%]==[] SET NEEDSUBST=1
if not "%NEEDSUBST%"=="" (
  set "drives=WDEFGHIJKLMNOPQRSTUVXYZABC"
  for /f "delims=:" %%A in ('wmic logicaldisk get caption') do set "drives=!drives:%%A=!"
  set "WORKDRV=!drives:~0,1!"

  if "!WORKDRV!"=="" (
    echo No free drive letter found, aborting, sorry...
    pause
    exit /b
  )
  subst !WORKDRV!: %WKDIR%
  set BLDDIR=!WORKDRV!:
) else (
  set BLDDIR=%WKDIR%
)
endlocal & set "WORKDRV=%WORKDRV%" & set BLDDIR=%BLDDIR%

pushd %BLDDIR%
set BLDDIR=
call :setupbe
call :buildit
popd
rmdir /s /q releases 2>nul
move /y %WKDIR%\ntvdmpatch\releases .
if not "%WORKDRV%"=="" subst %WORKDRV%: /d
set WORKDRV=
set ABPATH=
echo Cleaning up...
if not exist releases\nul (
  echo Was unable to move releases directory, please go to %WKDIR%\ntvdmpatch\releases manually and get it from there.
) else (
  rmdir /s /q %WKDIR%
  echo Autobuild completed, check releases-directory
)
pause
exit /b

:buildit
pushd ntvdmpatch\minnt
if "%DBGSTP%"=="" set BATCH=batch
call prepare %BATCH%
call patch %BATCH%
set BATCH=
call mktools
popd
cd NTOSBE-master
call sizzle_minnt.cmd cmdwindow
call buildrepoidw -y 
cd %NTROOT%\..\ntvdmpatch
call patch.cmd
if not "%DBGSTP%"=="" pause
call bld-minnt%HAXBLD%.cmd
if not "%DBGSTP%"=="" pause
call mkrelease-minnt%HAXBLD%.bat
pause
exit /B

:fetchprq
call :dlprq GRMWDK_EN_7600_1.ISO https://download.microsoft.com/download/4/A/2/4A25C7D5-EFBE-4182-B6A9-AE6850409A78/GRMWDK_EN_7600_1.ISO
call :dlprq GRMSDK_EN_DVD.iso https://download.microsoft.com/download/F/1/0/F10113F5-B750-4969-A255-274341AC6BCE/GRMSDK_EN_DVD.iso
call :dlprq NTOSBE-master.zip https://github.com/stephanosio/NTOSBE/archive/master.zip

rem Windows 7 x86:
rem This one would be direct download of en version, but it's very big and therefore slows us down, so better ask for user intervention
rem to download smaller ISO.
rem https://download.microsoft.com/download/C/0/6/C067D0CD-3785-4727-898E-60DC3120BB14/7601.24214.180801-1700.win7sp1_ldr_escrow_CLIENT_PROFESSIONAL_x86FRE_en-us.iso
rem
if not exist %PREREQ%\de_windows_7_professional_with_sp1_x86_dvd_u_677093.iso (
  echo On Windows Versions ^>= 8 when using WOW32, OLE2 support has been removed
  echo from OS. Therefore it is recommended to download Windows 7 ISO as a prereq
  echo to restore OLE2 support in these Windows versions. 
  echo If you plan to use NTVDMx64 only on lower versions of Windows, or not use
  echo WOW32, you can continue without it.
  echo.
  echo By pressing a key, you will be redirected to the Windows 7 ISO download page.
  echo You need to download the Windows 7 Professional 32bit ISO
  echo de_windows_7_professional_with_sp1_x86_dvd_u_677093.iso from there and
  echo place it in this directory.
  echo If you want to continue without it, just press a key afterwards again.
  pause
  start https://winfuture.de/downloadvorschalt,3291.html
  echo.
  echo Now please either download the ISO and put it here and afterwards continue
  echo or continue now to go without it
  pause
  if not exist %PREREQ%\de_windows_7_professional_with_sp1_x86_dvd_u_677093.iso (
    if not exist %PREREQ%\de_windows_7_professional_with_sp1_x64_dvd_u_676919.iso (
      echo OK, Continuing without OLE2 then
      set NOWIN7=1
    )
  )
)

if exist old-src-sr687.7z ren old-src-sr687.7z old-src.trunk.r687.20150728.7z
exit /b

:setupbe
call :unpack NTOSBE-master.zip
if "%DBGSTP%"=="" (
  type NTOSBE-master\buildlocaltools.cmd | findstr /V pause >buildlocaltools.cmd
  move /y buildlocaltools.cmd NTOSBE-master\buildlocaltools.cmd
)
call :unpack MinNT-20170416-85fac4faadc77203db8ddc66af280a75c1b717b0.zip
ren MinNT-master minnt
if "%SRCDIR%"=="" (
  del %PREREQ%\ntvdmx64.zip 2>nul
  call :dlprq ntvdmx64.zip https://github.com/leecher1337/ntvdmx64/archive/master.zip
  7z x %PREREQ%\ntvdmx64.zip
  del %PREREQ%\ntvdmx64.zip
  move /y ntvdmx64-master\ntvdmpatch .
  rmdir /s /q ntvdmx64-master
) else (
  xcopy /s %SRCDIR% ntvdmpatch\
)
call :cpyprq GRMSDK_EN_DVD.iso 
call :cpyprq GRMWDK_EN_7600_1.ISO 
call :cpyprq old-src.trunk.r687.20150728.7z 
copy /y %PREREQ%\de_windows_7_professional_with_sp1*.iso ntvdmpatch\minnt\work\
echo Build environment ready
exit /b

:dlprq
if not exist %PREREQ%\%1 (
  rem -- wget.exe is preferred, as it also works on Windows XP 
  rem -- bitsadmin fails for some urls and thus is the least preferred solution
  rem
  if exist %ABPATH%\wget.exe (
    %ABPATH%\wget %2 -O %PREREQ%\%1
  ) else (
    if exist %ABPATH%\dwnl.exe (
      %ABPATH%\dwnl %2 %PREREQ%\%1
    ) else (
      bitsadmin /transfer %1 /download /priority normal %2 %PREREQ%\%1
    )
  )
)
if not exist %PREREQ%\%1 (
  echo Prerequisite %1 not found in %PREREQ%, FAILED!
  pause
  exit
)
exit /b

:cpyprq
if not exist %PREREQ%\%1 (
  echo Prerequisite %1 not found in %PREREQ%, FAILED!
  pause
  exit
)
copy /y %PREREQ%\%1 ntvdmpatch\minnt\work\
exit /B

:unpack
if not exist %PREREQ%\%1 (
  echo Prerequisite %1 not found in %PREREQ%, FAILED!
  pause
  exit
)
7z x -y %PREREQ%\%1
exit /B
