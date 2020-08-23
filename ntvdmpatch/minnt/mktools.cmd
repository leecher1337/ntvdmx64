@echo off
call ..\basepath.cmd
cd %BASEPATH%\..\..\NTOSBE-master
set PATH=
call buildlocaltools.cmd x86
exit /b
