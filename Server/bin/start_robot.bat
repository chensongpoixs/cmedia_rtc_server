@echo off

setlocal

set para=x64\Debug

if "%1" NEQ "" set para=%1

set Dir=%~dp0%para%\

@ping -n 1 127.0.0.1>nul
start "Robot" /D %Dir% %Dir%Robot.exe ..\..\client.cfg  ..\..\log

REM pause
