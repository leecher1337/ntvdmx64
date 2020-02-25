@echo off
call ..\basepath.cmd
cd %BASEPATH%\..\..\NTOSBE-master
buildlocaltools.cmd x86
