@echo off
call ..\..\basepath.cmd
xcopy /S /Y softpc.new %BASEPATH%\mvdm\softpc.new\
pushd %BASEPATH%
for %%I IN (%PATCHROOT%\experimental\vesa\*.patch) do %PATCHROOT%\util\patch.exe -N -p2 -i %%I
popd
:fini
