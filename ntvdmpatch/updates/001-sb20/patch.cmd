@echo off
copy /Y inc W:\nt\private\mvdm\softpc.new\host\inc
copy /Y src W:\nt\private\mvdm\softpc.new\host\src
pushd w:\nt
..\ntvdmpatch\util\patch.exe -N -u -p1 -i ..\ntvdmpatch\updates\001-sb20\sb20.patch
popd
