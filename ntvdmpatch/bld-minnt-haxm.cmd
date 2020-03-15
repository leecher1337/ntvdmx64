@echo off
call basepath.cmd
pushd %BASEPATH%
set BUILD_ALLOW_ALL_WARNINGS=1
set HAXM=1
chcp 437
build -e
popd
