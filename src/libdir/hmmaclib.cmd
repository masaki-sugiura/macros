@echo off
if "%TEMP%"=="" set TEMP=c:\temp
if "%TMPDIR%"=="" set TMPDIR=%TEMP%
set AWKPATH=c:\hidemaru\macros\src\libdir\script
gawk -f common.awk -f %1 %2 %3 %4 %5 %6 %7 %8 %9

