@echo off
copy /Y W:\nt\private\mvdm\softpc.new\base\cvidc\gdpvar.h W:\nt\private\mvdm\softpc.new\base\ccpu386
copy /Y W:\nt\private\mvdm\softpc.new\base\cvidc\sas4gen.h W:\nt\private\mvdm\softpc.new\base\ccpu386
copy /Y W:\nt\private\mvdm\softpc.new\base\cvidc\cpu_c.h W:\nt\private\mvdm\softpc.new\base\ccpu386
copy /Y patches\fmstubs.c W:\nt\private\mvdm\softpc.new\obj.vdm
copy /Y W:\nt\private\inc\vdm.h W:\nt\private\mvdm\softpc.new\base\ccpu386
copy /Y W:\nt\private\mvdm\softpc.new\host\inc\mips\prod\pigreg_c.h W:\nt\private\mvdm\softpc.new\base\ccpu386
pushd w:\nt
for %%I IN (..\ntvdmpatch\patches\*.patch) do ..\ntvdmpatch\util\patch.exe -N -p1 -i %%I
popd

