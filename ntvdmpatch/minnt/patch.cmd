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

echo Ensuring that msg files have CRLF
pushd %BASEPATH%\mvdm\dos\v86\messages
for /R %%f in (*.msg) do (
  more /p <%%f >tmp.msg
  %PATCHROOT%\util\sed -i "s/^        /\t/g" tmp.msg
  move tmp.msg %%f
)
popd

echo Patching broken utilities
pushd %BASEPATH%\..\
%PATCHROOT%\util\patch -N -p0 -i %~dp0\tools.patch
cd ..
%PATCHROOT%\util\patch -N -p0 -i %~dp0\be.patch
popd

echo Done, now your MINNT build environment should be in a working condition.
echo Run sizzle_minnt.cmd in Build environment directory next and do 
echo buildrepoidw.cmd 

:fini