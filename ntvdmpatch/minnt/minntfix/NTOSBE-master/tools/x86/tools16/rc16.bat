@echo off
rem Work around a MDOS-player bug that doesn't allow DOS executables to
rem be started from within another DOS executable, if the executable is
rem an executable that got converterted by MSDOS-player

msdos32 rc16dos %1 %2 %3 %4 %5 %6 %7 %8 %9
