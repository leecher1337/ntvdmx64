@echo off
call ..\..\basepath.cmd
if "%REPO%"=="minnt" (
  echo HAXM support is currently not available for minnt repo, only old-src
  goto fini
)
xcopy /S /Y v86 %BASEPATH%\mvdm\v86\
pushd %BASEPATH%
for %%I IN (%PATCHROOT%\experimental\haxm\*.patch) do %PATCHROOT%\util\patch.exe -N -p2 -i %%I
popd
:fini
