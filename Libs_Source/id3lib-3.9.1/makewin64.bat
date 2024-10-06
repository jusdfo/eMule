@ECHO OFF

CD /D %~dp0

RMDIR /S /Q obj objd
DEL   /Q *.lib

CALL "%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"

REM $Id: makewin32.bat,v 1.2 2022/01/01 00:00:00 lezy Exp $

if not exist config.h copy config.h.win32 config.h

nmake DEBUG=1 -f makefile.win32
nmake -f makefile.win32
