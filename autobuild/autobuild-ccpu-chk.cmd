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
rem   KEEPPAT   Internal use, does not demand refresh of ntvdmpatch
rem   WKDIR     Working directory for build process, default is %CD%\w
rem   HAXBLD    If set to -haxm then HAXM build is built, otherwise normal build
rem   DBGSTP    If set, pause is issued after every step, useful for testing build
rem   SIZ_NTBLD [chk, fre] Checked of free build, default is checked (debug)
rem   NOWIN7    Ignore missing Windows 7 ISO (i.e. build only for Win < 8 or no OLE2)
rem   NOW2K3    Ignore missing Windows Server 2003 sourcecode (use std. vdmredir.dll)
rem   NOPAUSE   Do not pause before cleanup
rem   LANG      Only build this language (for a complete list, see documentation)
rem   KEEPWD    Keep working directory so that subsequent builds will run through 
rem             faster when run with KEEPPAT
rem

echo ----------------------------------------------------
echo Autobuild NTVDMx64
echo ----------------------------------------------------
echo.

rem There seem to be some strange constellations where even on syswow64
rem cmd.exe, PROCESSOR_ARCHITECTURE is AMD64 for unknown reasons. Overwrite it
if not "%PROCESSOR_ARCHITECTURE%"=="x86" set PROCESSOR_ARCHITECTURE=x86

setlocal enableDelayedExpansion
if exist "%ProgramFiles%\7-Zip" set PATH=%PATH:)=^)%;"%ProgramFiles%\7-Zip"
7z >nul 2>&1
if errorlevel 255 (
for /F "tokens=2*" %%r in ('reg query "HKEY_LOCAL_MACHINE\SOFTWARE\7-zip" /v Path 2^>nul') do set ZPATH=%%s
if "!ZPATH!"=="" for /F "tokens=2*" %%r in ('reg query "HKEY_CURRENT_USER\SOFTWARE\7-zip" /v Path 2^>nul') do set ZPATH=%%s
if not "!ZPATH!"=="" set PATH=%PATH:)=^)%;!ZPATH!
7z >nul 2>&1
if errorlevel 255 (
echo Please install 7zip first, then run again
start http://www.7-zip.de
pause
exit /b
)
)
endlocal & set PATH=%PATH%

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
  set WORKDRV=
  for /f "tokens=1* delims=:" %%A in ('subst') do (
    for /f "tokens=2 delims=>" %%C in ("%%B") do (
      for /f "tokens=* delims= " %%D in ("%%C") do (
        set "MappedPath=%%D"
        if /i "!MappedPath!"=="%WKDIR%" (
          set "WORKDRV=%%A"
        )
      )
    )
  )

  if "!WORKDRV!"=="" (  
    set "drives=WDEFGHIJKLMNOPQRSTUVXYZABC"
    for /f "delims=:" %%A in ('wmic logicaldisk get caption') do set "drives=!drives:%%A=!"
    set "WORKDRV=!drives:~0,1!"

    if "!WORKDRV!"=="" (
      echo No free drive letter found, aborting, sorry...
      pause
      exit /b
    )
    subst !WORKDRV!: %WKDIR%
  )
  set BLDDIR=!WORKDRV!:
) else (
  set BLDDIR=%WKDIR%
)
endlocal & set "WORKDRV=%WORKDRV%" & set BLDDIR=%BLDDIR%

pushd %BLDDIR%
set BLDDIR=
if "%KEEPPAT%"=="" (
  call :setupbe
  call :buildit
) else (
  call :dlntvdmx64
  rmdir /s /q minnt 2>nul
  call :unpack MinNT-20170416-85fac4faadc77203db8ddc66af280a75c1b717b0.zip
  ren MinNT-master minnt
  pushd ntvdmpatch\minnt
  if "%DBGSTP%"=="" (call patch batch) else call patch
  popd
  call :buildthis
)
popd
rmdir /s /q releases 2>nul
move /y %WKDIR%\ntvdmpatch\releases .
if not "%WORKDRV%"=="" subst %WORKDRV%: /d
set WORKDRV=
set ABPATH=
set CCPU=
set HAXM=
set HAXBLD=
if not exist releases\nul (
  echo Was unable to move releases directory, please go to %WKDIR%\ntvdmpatch\releases manually and get it from there.
  pause
) else (
  if "%NOPAUSE%"=="" pause
  echo Cleaning up...
  if "%KEEPWD%"=="" (rmdir /s /q %WKDIR%) else rmdir /s /q %WKDIR%\ntvdmpatch\minnt\work 2>nul
  echo Autobuild completed, check releases-directory
)
exit /b

:buildit
pushd ntvdmpatch\minnt
if "%DBGSTP%"=="" set BATCH=batch
call prepare %BATCH%
call patch %BATCH%
set BATCH=
call mktools
popd
:buildthis
cd NTOSBE-master
call sizzle_minnt.cmd cmdwindow
call buildrepoidw -y 
cd %NTROOT%\..\ntvdmpatch
call patch.cmd
if not "%DBGSTP%"=="" pause
call bld-minnt%HAXBLD%.cmd
if not "%DBGSTP%"=="" pause
call mkrelease-minnt%HAXBLD%.bat %LANG%
exit /B

:fetchprq
call :dlprq GRMWDK_EN_7600_1.ISO https://download.microsoft.com/download/4/A/2/4A25C7D5-EFBE-4182-B6A9-AE6850409A78/GRMWDK_EN_7600_1.ISO
call :dlprq GRMSDK_EN_DVD.iso https://download.microsoft.com/download/F/1/0/F10113F5-B750-4969-A255-274341AC6BCE/GRMSDK_EN_DVD.iso
call :dlprq NTOSBE-master.zip http://web.archive.org/web/20210804144408/https://codeload.github.com/stephanosio/NTOSBE/zip/master

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
  echo This also applies to Windows 11 ^>= 23H3, as WOW32 support has been damaged
  echo irreparably in these versions of Windows and therefore you cannot use it 
  echo there anyway.
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

if not "%NOW2K3%"=="" goto w2k3chkd
:chkw2k3
set W2K3SRC=
if exist %PREREQ%\nt5src.7z set W2K3SRC=1
if exist %PREREQ%\Win2K3.7z set W2K3SRC=1
if exist %PREREQ%\3790src2.cab (
  if exist %PREREQ%\3790src4.cab set W2K3SRC=1
)
if not "%W2K3SRC%"=="1" (
  if not "%W2K3SRCMSG%"=="1" (
    echo Windows 7 and above only ship with a crippled VDMREDIR.DLL that i.e. doesn't
    echo support mounting network shares via MS-DOS calls.
    echo This repository contains code that is able to partly restore Windows XP
    echo Networking functions of VDMREDIR.DLL, however the leaked Windows Server 2003
    echo is needed for that.
    echo So it is recommended to place the leaked sourceode as a prerequisite here too.
    echo It can have one of these names:
    echo * nt5src.7z
    echo * Win2K3.7z
    echo * 3790src2.cab, 3790src4.cab
    echo.
    echo Now please either download them and put it here and afterwards continue
    echo or continue now to go without it ^(you will get the limited vdmredir.dll^)
    pause
    set W2K3SRCMSG=1
    goto chkw2k3
  ) else (
    echo OK, Continuing without enhanced VDMREDIR.DLL then
  )
)
:w2k3chkd

if exist old-src-sr687.7z ren old-src-sr687.7z old-src.trunk.r687.20150728.7z
exit /b

:setupbe
call :unpack NTOSBE-master.zip
if "%DBGSTP%"=="" (
  type NTOSBE-master\buildlocaltools.cmd | findstr /V pause >buildlocaltools.cmd
  move /y buildlocaltools.cmd NTOSBE-master\buildlocaltools.cmd
)
rmdir /s /q minnt 2>nul
call :unpack MinNT-20170416-85fac4faadc77203db8ddc66af280a75c1b717b0.zip
ren MinNT-master minnt
call :dlntvdmx64
call :cpyprq GRMSDK_EN_DVD.iso 
call :cpyprq GRMWDK_EN_7600_1.ISO 
call :cpyprq old-src.trunk.r687.20150728.7z 
copy /y %PREREQ%\de_windows_7_professional_with_sp1*.iso ntvdmpatch\minnt\work\
for %%I in (nt5src.7z Win2K3.7z 3790src2.cab 3790src4.cab) do if exist %PREREQ%\%%I call :cpyprq %%I
echo Build environment ready
exit /b

:dlntvdmx64
if not "%KEEPPAT%"=="" exit /B
if "%SRCDIR%"=="" (
  del %PREREQ%\ntvdmx64.zip 2>nul
  rmdir /s /q ntvdmpatch 2>nul
  call :dlprq ntvdmx64.zip https://github.com/leecher1337/ntvdmx64/archive/master.zip
  7z x -y  %PREREQ%\ntvdmx64.zip
  del %PREREQ%\ntvdmx64.zip
  move /y ntvdmx64-master\ntvdmpatch .
  rmdir /s /q ntvdmx64-master
) else (
  xcopy /s /Y %SRCDIR% ntvdmpatch\
)
exit /B

:dlprq
if not exist %PREREQ%\%1 (
  rem -- wget.exe is preferred, as it also works on Windows XP 
  rem -- bitsadmin fails for some urls and thus is the least preferred solution
  rem
  if exist %ABPATH%\wget.exe (
    %ABPATH%\wget --no-check-certificate %2 -O %PREREQ%\%1
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
  if not "%WORKDRV%"=="" subst %WORKDRV%: /d
  exit
)
exit /b

:cpyprq
if not exist %PREREQ%\%1 (
  echo Prerequisite %1 not found in %PREREQ%, FAILED!
  pause
  if not "%WORKDRV%"=="" subst %WORKDRV%: /d
  exit
)
copy /y %PREREQ%\%1 ntvdmpatch\minnt\work\
exit /B

:unpack
if not exist %PREREQ%\%1 (
  echo Prerequisite %1 not found in %PREREQ%, FAILED!
  pause
  if not "%WORKDRV%"=="" subst %WORKDRV%: /d
  exit
)
7z x -y %PREREQ%\%1
exit /B
