@echo off
call ..\..\basepath.cmd
xcopy /S /Y v86 %BASEPATH%\mvdm\v86\
pushd %BASEPATH%
if "%REPO%"=="minnt" (
  for %%I IN (%PATCHROOT%\experimental\hyperv\minnt\*.patch) do %PATCHROOT%\util\patch.exe -N -p2 -i %%I
  popd
  xcopy /S /Y v86\*.* %BASEPATH%\mvdm\v86\
) else (
  echo Only minnt is supported
  popd
)
:fini
