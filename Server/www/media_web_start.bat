@echo off

setlocal

REM set para=x64\Debug

REM if "%1" NEQ "" set para=%1

REM set Dir=%~dp0%para%\

@ping -n 1 127.0.0.1>nul
start "media-webserver"   node.exe media_server.js

REM  pause