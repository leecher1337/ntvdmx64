@echo off
mkdir W:\nt\private\mvdm\softpc.new\host\inc\x86\prod
mkdir W:\nt\private\mvdm\softpc.new\host\inc\x86\pig
for %%I in (gdpvar.h sas4gen.h cpu_c.h CpuInt_c.h evid_c.h) do copy /Y W:\nt\private\mvdm\softpc.new\base\cvidc\%%I W:\nt\private\mvdm\softpc.new\host\inc\x86\prod
copy /Y W:\nt\private\mvdm\softpc.new\host\inc\mips\prod\pigreg_c.h W:\nt\private\mvdm\softpc.new\host\inc\x86\prod
copy /Y patches\fmstubs.c W:\nt\private\mvdm\softpc.new\obj.vdm
echo Fixing CVID...
for %%I in (W:\nt\private\mvdm\softpc.new\base\cvidc\s????0*.c) do ..\ntvdmpatch\util\sed.exe -i "s/\tIUH\treturnValue,v1=0,v2=0,v3=0,v4=0;\t\treturnValue\t=\t\(.*\)(v1,v2,v3,v4);\treturn(returnValue);\t/goto \1;/g" %%I
echo Patching NTVDM...
pushd w:\nt
for %%I IN (..\ntvdmpatch\patches\*.patch) do ..\ntvdmpatch\util\patch.exe -N -p1 -i %%I
popd
REM
REM After patching the files in their original directories, they can be checked in to central directory
copy /Y W:\nt\private\mvdm\softpc.new\base\ccpu386\cpu4gen.h W:\nt\private\mvdm\softpc.new\host\inc\x86\prod
copy /Y W:\nt\private\mvdm\softpc.new\base\ccpu386\ccpusas4.h W:\nt\private\mvdm\softpc.new\host\inc\x86\prod
copy /Y W:\nt\private\mvdm\softpc.new\base\cvidc\evidgen.h W:\nt\private\mvdm\softpc.new\host\inc\x86\prod
COPY /Y W:\nt\private\mvdm\softpc.new\host\inc\x86\prod\*.* W:\nt\private\mvdm\softpc.new\host\inc\x86\pig
for %%I in (gdpvar.h evidgen.h) do del W:\nt\private\mvdm\softpc.new\base\ccpu386\%%I

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
echo Done patching.
