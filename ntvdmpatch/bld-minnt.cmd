@echo off
call basepath.cmd
pushd %BASEPATH%
set BUILD_ALLOW_ALL_WARNINGS=1
set CCPU=1
build -e
popd
