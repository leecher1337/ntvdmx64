@echo off
setlocal ENABLEDELAYEDEXPANSION

echo ----------------------------------------------------
echo Patching MINNT for proper operation
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
if not exist %minntfix%\minnt\public\internal\base\inc\splapip.h (
  echo It seems you have not prepared %minntfix% directory yet.
  echo Please run prepare.cmd first.
  pause
  goto fini
)


echo Copying fixed files to minnt repository
xcopy /E /R /Y %minntfix% %BASEPATH%\..\..\
echo #include "winddi_xp.h" >>%BASEPATH%\..\..\minnt\public\oak\inc\winddi.h
ren %BASEPATH%\mvdm\bin86\jpn\_disp.sys %BASEPATH%\mvdm\bin86\jpn\$disp.sys
ren %BASEPATH%\mvdm\bin86\jpn\_ias.sys %BASEPATH%\mvdm\bin86\jpn\$ias.sys
ren %BASEPATH%\mvdm\bin86\jpn\_prnescp.sys %BASEPATH%\mvdm\bin86\jpn\$prnescp.sys
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

echo Patching broken utilities
pushd %BASEPATH%\..\
%PATCHROOT%\util\patch -N -p0 -i %~dp0\tools.patch
cd ..
%PATCHROOT%\util\patch -N -p0 -i %~dp0\be.patch
rem Also ensure .mc file has CRLF line endings
%PATCHROOT%\util\sed -i "s/\r$/\n$/" NTOSBE-master\src\sdktools\rcdll\rcmsgs.mc
popd


echo Done, now your MINNT build environment should be in a working condition.
echo Run sizzle_minnt.cmd in Build environment directory next and do 
echo buildrepoidw.cmd 
pause

:fini