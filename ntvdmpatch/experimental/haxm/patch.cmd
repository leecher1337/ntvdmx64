@echo off
xcopy /S /Y v86 W:\nt\private\mvdm\v86\
pushd w:\nt
for %%I IN (..\ntvdmpatch\experimental\haxm\*.patch) do ..\ntvdmpatch\util\patch.exe -N -p1 -i %%I
popd
