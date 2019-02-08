@echo off
pushd w:\nt
..\ntvdmpatch\util\patch.exe -N -u -p1 -i ..\ntvdmpatch\updates\003-romemu\romemu.patch
popd
