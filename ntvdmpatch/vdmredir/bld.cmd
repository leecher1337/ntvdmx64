@echo off
call ..\basepath.cmd
SETLOCAL EnableDelayedExpansion
pushd %BASEPATH%\..
set BASEDIR=%CD%
set BUILD_ALLOW_ALL_WARNINGS=1
chcp 437

rem -- Build SAM lib
pushd ds\sam\client\w7
build
popd


rem -- Build NETRAP and XACTSRV
pushd ds\netapi

pushd netlib\w7
build
popd

pushd rap
build
popd

pushd dosprint
build
popd

pushd rpcxlate\rxcommon
build
popd

pushd rpcxlate\rxapi
build
popd

pushd svcdlls\srvsvc\client\w7
build
popd

pushd svcdlls\browser2\client
build
popd

pushd xactsrv
build
popd

popd

rem -- Build vdmredir.dll
pushd base\mvdm\vdmredir
build
popd

popd
