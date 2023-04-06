@echo off
setlocal ENABLEDELAYEDEXPANSION

echo ----------------------------------------------------
echo Patching MINNT for proper operation, please wait...
echo ----------------------------------------------------
echo.

REM Setup environment
call ..\basepath.cmd
if not exist %PATCHROOT%\util\sed.exe (
  echo Error: Stream editor not found
  pause
  goto fini
)

set minntfix=minntfix
if not "%1"=="vdmredir" (
if not exist %minntfix%\minnt\public\internal\base\inc\splapip.h (
  echo It seems you have not prepared %minntfix% directory yet.
  echo Please run prepare.cmd first.
  pause
  goto fini
)
)

if exist ..\vdmredir\minntfix\minnt\public\internal\net\inc\icanon.h (
  pushd ..\vdmredir
  call patch.cmd batch
  popd
)

echo Copying fixed files to minnt repository
xcopy /E /R /Y %minntfix% %BASEPATH%\..\..\
if "%1"=="vdmredir" (
  pushd %BASEPATH%\..\
  goto noxpsrc
)
echo #include "winddi_xp.h" >>%BASEPATH%\..\..\minnt\public\oak\inc\winddi.h
move %BASEPATH%\..\..\minnt\public\sdk\inc\commctrl.h %BASEPATH%\..\..\minnt\public\sdk\inc\commctrl.h_
rem Prepend include of sal.h to new header file from SDK
echo #include ^<sal.h^> >%BASEPATH%\..\..\minnt\public\sdk\inc\commctrl.h
type %BASEPATH%\..\..\minnt\public\sdk\inc\commctrl.h_ >>%BASEPATH%\..\..\minnt\public\sdk\inc\commctrl.h
del %BASEPATH%\..\..\minnt\public\sdk\inc\commctrl.h_

rem ... But in makefile, it's with _ ..?
rem ren %BASEPATH%\mvdm\bin86\jpn\_disp.sys $disp.sys
rem ren %BASEPATH%\mvdm\bin86\jpn\_ias.sys $ias.sys
rem ren %BASEPATH%\mvdm\bin86\jpn\_prnescp.sys $prnescp.sys
xcopy /Y %BASEPATH%\mvdm\dos\v86\cmd\backup\kor\makefile %BASEPATH%\mvdm\dos\v86\cmd\backup\jpn\

echo Enabling old DPMI code
move %BASEPATH%\mvdm\dpmi %BASEPATH%\mvdm\dpmi.new
move %BASEPATH%\mvdm\dpmi32 %BASEPATH%\mvdm\dpmi32.new
move %BASEPATH%\mvdm\dpmi.old %BASEPATH%\mvdm\dpmi
move %BASEPATH%\mvdm\dpmi32.old %BASEPATH%\mvdm\dpmi32
move %BASEPATH%\mvdm\inc\dpmi.h %BASEPATH%\mvdm\inc\dpmi.h.new
move %BASEPATH%\mvdm\inc\dpmi.h.old %BASEPATH%\mvdm\inc\dpmi.h
for /d %%i in (%BASEPATH%\mvdm\dpmi.new\*.*) do xcopy /s /e /y "%%i\*.*" "%BASEPATH%\mvdm\dpmi\%%~ni\"
for %%i in (makefile.sub sources.inc) do copy "%BASEPATH%\mvdm\dpmi.new\%%i" "%BASEPATH%\mvdm\dpmi\%%i"
del "%BASEPATH%\mvdm\dpmi\makefile"
copy /y "%BASEPATH%\mvdm\dpmi32.new\dirs" "%BASEPATH%\mvdm\dpmi32\"
copy /y "%BASEPATH%\mvdm\dpmi32.new\sources.inc" "%BASEPATH%\mvdm\dpmi32\"
for /d %%i in (%BASEPATH%\mvdm\dpmi32.new\*.*) do (
  xcopy /s /e "%%i\*.*" "%BASEPATH%\mvdm\dpmi32\%%~ni\"
  copy /y "%BASEPATH%\mvdm\dpmi32\precomp.h" "%BASEPATH%\mvdm\dpmi32\%%~ni\"
)
del "%BASEPATH%\mvdm\dpmi32\makefile"

echo Ensuring that msg files have CRLF
pushd %BASEPATH%\mvdm\dos\v86\messages
for /R %%f in (*.msg) do (
  %PATCHROOT%\util\sed -i "s/\r$/\n$/" %%f 
)
popd

pushd %BASEPATH%\..\
if exist %PATCHROOT%\patches\minnt\wow16.patch.oldsrc ren %PATCHROOT%\patches\minnt\wow16.patch.oldsrc wow16.patch 
if not exist %BASEPATH%\mvdm\wow16\makefile.inc goto noxpsrc
echo Patching WinXP/2k3 WOW16
%PATCHROOT%\util\patch -N -p0 -i %~dp0\wow16-xp.patch 
ren %PATCHROOT%\patches\minnt\wow16.patch wow16.patch.oldsrc

:noxpsrc
echo Updating SDK Headers
%PATCHROOT%\util\patch -N -p0 -i %~dp0\sdk.patch

echo Patching broken utilities
%PATCHROOT%\util\patch -N -p0 -i %~dp0\tools.patch

cd ..
find "qgrep" NTOSBE-master\src\sdktools\dirs >nul
if not errorlevel 1 goto bepatched
%PATCHROOT%\util\patch -N -p0 -i %~dp0\be.patch
rem Also ensure .mc file has CRLF line endings
%PATCHROOT%\util\sed -i "s/\r$/\n$/" NTOSBE-master\src\sdktools\rcdll\rcmsgs.mc
rem Work around broken rc16
del NTOSBE-master\tools\x86\tools16\rc16.exe
:bepatched
popd


echo Done, now your MINNT build environment should be in a working condition.
echo Run sizzle_minnt.cmd in Build environment directory next and do 
echo buildrepoidw.cmd 

if "%1"=="vdmredir" goto fini
if not "%1"=="batch" pause

:fini
