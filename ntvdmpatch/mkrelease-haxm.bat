@echo off
call mkrelease.bat
md release\haxm
copy /Y haxm release\haxm\
