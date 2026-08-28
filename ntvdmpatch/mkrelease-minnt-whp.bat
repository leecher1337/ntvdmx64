@echo off
call mkrelease-minnt.bat
SETLOCAL ENABLEDELAYEDEXPANSION
for /d %%L in (releases\*.*) do (
  md %%L\hyperv >nul
  copy /Y hyperv %%L\hyperv
)
