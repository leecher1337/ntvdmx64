@echo off
xcopy /S /Y dos W:\nt\private\mvdm\dos
pushd w:\nt
..\ntvdmpatch\util\patch.exe -N -u -p1 -i ..\ntvdmpatch\updates\002-demlfn\demlfn.patch
popd
