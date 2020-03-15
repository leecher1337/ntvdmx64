@echo off
call ..\..\basepath.cmd
xcopy /S /Y v86 %BASEPATH%\mvdm\v86\
pushd %BASEPATH%
if "%REPO%"=="minnt" (
  for %%I IN (%PATCHROOT%\experimental\haxm\minnt\*.patch) do %PATCHROOT%\util\patch.exe -N -p2 -i %%I
  %PATCHROOT%\util\patch.exe -N -p2 -i %PATCHROOT%\experimental\haxm\haxm-dpmi.patch
  popd
  ren %BASEPATH%\mvdm\v86\haxm\i386 x86
  del %BASEPATH%\mvdm\v86\haxm\x86\sources
  del %BASEPATH%\mvdm\v86\haxm\makefile
  del %BASEPATH%\mvdm\v86\haxm\sources
  xcopy /S /Y minnt\v86\*.* %BASEPATH%\mvdm\v86\
) else (
  for %%I IN (%PATCHROOT%\experimental\haxm\*.patch) do %PATCHROOT%\util\patch.exe -N -p2 -i %%I
  popd
)
:fini
