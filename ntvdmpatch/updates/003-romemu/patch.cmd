@echo off
pushd w:\nt
..\ntvdmpatch\util\patch.exe -N -p1 -i ..\ntvdmpatch\updates\003-romemu\romemu.patch
popd
