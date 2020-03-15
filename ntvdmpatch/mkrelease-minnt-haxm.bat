@echo off
call mkrelease-minnt.bat
md release\haxm
copy /Y haxm release\haxm\
REM
REM DPMI currently not supported by HAXM build, remove in improbable case this works some day...
REM
SETLOCAL ENABLEDELAYEDEXPANSION
for /d %%L in (releases\*.*) do (
  util\sed -i "s/lh %%SystemRoot%%\\system32\\dosx/REM lh %%SystemRoot%%\\system32\\dosx/g" %%L\dos\autoexec.nt
)
