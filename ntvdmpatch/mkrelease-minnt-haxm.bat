@echo off
call mkrelease-minnt.bat
SETLOCAL ENABLEDELAYEDEXPANSION
for /d %%L in (releases\*.*) do (
  md %%L\haxm >nul
  copy /Y haxm %%L\haxm
)
