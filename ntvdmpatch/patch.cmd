@echo off
call basepath.cmd
if not exist "%BASEPATH%" (
  echo Invalid NT Base path %BASEPATH%
  goto fini
)
mkdir %BASEPATH%\mvdm\softpc.new\host\inc\x86\prod
mkdir %BASEPATH%\mvdm\softpc.new\host\inc\x86\pig
for %%I in (gdpvar.h sas4gen.h cpu_c.h CpuInt_c.h evid_c.h) do copy /Y %BASEPATH%\mvdm\softpc.new\base\cvidc\%%I %BASEPATH%\mvdm\softpc.new\host\inc\x86\prod
copy /Y %BASEPATH%\mvdm\softpc.new\host\inc\mips\prod\pigreg_c.h %BASEPATH%\mvdm\softpc.new\host\inc\x86\prod
copy /Y patches\common\fmstubs.c %BASEPATH%\mvdm\softpc.new\obj.vdm
rem MinNT specific fixes
if exist %BASEPATH%\mvdm\v86\monitor\x86\vdpm.c for %%I in (vdpm.c longpath.c) do copy /Y %BASEPATH%\mvdm\v86\monitor\x86\%%I %BASEPATH%\mvdm\softpc.new\host\src
echo Fixing CVID...
for %%I in (%BASEPATH%\mvdm\softpc.new\base\cvidc\s????0*.c) do ..\ntvdmpatch\util\sed.exe -i "s/\tIUH\treturnValue,v1=0,v2=0,v3=0,v4=0;\t\treturnValue\t=\t\(.*\)(v1,v2,v3,v4);\treturn(returnValue);\t/goto \1;/g" %%I
echo Patching NTVDM...
pushd %BASEPATH%
for %%I IN (%PATCHROOT%\patches\common\*.patch %PATCHROOT%\patches\%REPO%\*.patch) do %PATCHROOT%\util\patch.exe -N -p2 -i %%I
popd
REM
REM After patching the files in their original directories, they can be checked in to central directory
copy /Y %BASEPATH%\mvdm\softpc.new\base\ccpu386\cpu4gen.h %BASEPATH%\mvdm\softpc.new\host\inc\x86\prod
copy /Y %BASEPATH%\mvdm\softpc.new\base\ccpu386\ccpusas4.h %BASEPATH%\mvdm\softpc.new\host\inc\x86\prod
copy /Y %BASEPATH%\mvdm\softpc.new\base\cvidc\evidgen.h %BASEPATH%\mvdm\softpc.new\host\inc\x86\prod
COPY /Y %BASEPATH%\mvdm\softpc.new\host\inc\x86\prod\*.* %BASEPATH%\mvdm\softpc.new\host\inc\x86\pig
for %%I in (gdpvar.h evidgen.h) do del %BASEPATH%\mvdm\softpc.new\base\ccpu386\%%I

echo.
echo Prepare for use with new CVIDC...
pushd experimental\cvidc-new
call patch.cmd
popd
echo.
echo Installing HAXM-patches to source...
pushd experimental\haxm
call patch.cmd
popd
echo Installing ADLIB-sound...
pushd experimental\adlib
call patch.cmd
popd
echo Done patching.
:fini
