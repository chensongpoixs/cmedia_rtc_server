@echo off

if "%1" == "" echo need name & goto end

setlocal

cd /d %~dp0


set PROTC_DIR=..\Tools\
set PROTC_EXE=%PROTC_DIR%\protoc.exe


set CPPOUT_DIR=..\..\Server\common\cclient_msg
if not exist %CPPOUT_DIR% MD %CPPOUT_DIR%

%PROTC_EXE% --cpp_out=%CPPOUT_DIR% %1
if %errorlevel% NEQ 0 goto failed

echo success
goto end

:failed
echo failed

:end

