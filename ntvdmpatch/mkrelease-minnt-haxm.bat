@echo off
call mkrelease-minnt.bat
REM
REM DPMI currently not supported by HAXM build, remove in improbable case this works some day...
REM
SETLOCAL ENABLEDELAYEDEXPANSION
for /d %%L in (releases\*.*) do (
  md %%L\haxm >nul
  copy /Y haxm %%L\haxm
  util\sed -i "s/lh %%SystemRoot%%\\system32\\dosx/REM lh %%SystemRoot%%\\system32\\dosx/g" %%L\dos\autoexec.nt
)
