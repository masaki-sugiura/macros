@echo off
if "%1" == "" goto END
set LZHTEMP=c:\usertemp\tmc_$$$.lzh
set MACDIR=c:\hidemaru\macros
set LZHDEST=%MACDIR%\tmc_work\tmc_%1_%TMCVER%.lzh
set PKGDIR=%SRCDIR%\tex_mc\%1

if exist %PKGDIR%\ver_up.dat goto NEWEST

if exist %LZHTEMP% del %LZHTEMP%
if exist %LZHDEST% del %LZHDEST%
cd /D %MACDIR%
lha a -axil %LZHTEMP% tex_mc\%1\*.* tex_mc\docs\packages\%1.txt
if "%2"=="" goto CHGNAME
lha a -axil %LZHTEMP% %2 %3 %4 %5 %6 %7 %8 %9
:CHGNAME
move %LZHTEMP% %LZHDEST%
echo THIS FILE IS MADE BY TeX-MC PACKAGE GENERATOR > %PKGDIR%\ver_up.dat
echo VERSION %TMCVER% >> %PKGDIR%\ver_up.dat
goto END

:NEWEST
echo %1 package is not modified.
echo .

:END

