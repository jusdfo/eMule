@ECHO OFF

CD /D %~dp0

CD eMule
@REM git pull

CD /D %~dp0
START "" eMule\emule.sln
