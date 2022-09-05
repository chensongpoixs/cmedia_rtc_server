@echo off

setlocal

set para=x64\Debug

if "%1" NEQ "" set para=%1

set Dir=%~dp0%para%\

@ping -n 1 127.0.0.1>nul
start "media_rtc_server" /D %Dir% %Dir%media_rtc_server.exe ..\..\server.cfg  ..\..\log

REM pause
