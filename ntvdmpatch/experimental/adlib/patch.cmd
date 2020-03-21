@echo off
call ..\..\basepath.cmd
xcopy /S /Y host %BASEPATH%\mvdm\softpc.new\host\
if "%REPO%"=="minnt" (
  pushd %BASEPATH%
  for %%I IN (%PATCHROOT%\experimental\adlib\*.patch) do %PATCHROOT%\util\patch.exe -N -p2 -i %%I
  popd
) else (
  echo The ADLIB patch only works in MINNT repository
)
:fini
