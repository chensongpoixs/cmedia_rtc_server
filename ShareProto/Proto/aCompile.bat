@echo off

setlocal

setlocal enabledelayedexpansion

cd /d %~dp0

set PROTC_DIR=..\Tools\
set PROTC_EXE=%PROTC_DIR%protoc.exe
set DIFF_EXE=%PROTC_DIR%diff.exe


set TMP_DIR=tmp
if not exist %TMP_DIR% MD %TMP_DIR%

call:gen_cpp
if errorlevel 1 goto failed


echo success
goto end

:gen_cpp
echo c plus plus

set CPPOUT_DIR=..\..\Server\Server\rtc_sdp
if not exist %CPPOUT_DIR% MD %CPPOUT_DIR%

echo %CPPOUT_DIR%
for /f %%i in ('dir /b *.proto') do (
echo compile %%~nxi 
%PROTC_EXE% --cpp_out=%TMP_DIR% %%~nxi
if !errorlevel! NEQ 0 goto:eof
call:diff_copy %TMP_DIR% %CPPOUT_DIR% %%~ni.pb.h
if !errorlevel! NEQ 0 goto:eof
call:diff_copy %TMP_DIR% %CPPOUT_DIR% %%~ni.pb.cc
if !errorlevel! NEQ 0 goto:eof
)
goto:eof


REM src_dir dst_dir filename
:diff_copy
%DIFF_EXE% %1\%3 %2\%3
if %errorlevel% == 1 echo diff error & goto:eof
if %errorlevel% == 2 echo %3 & copy /y %1\%3 %2\%3 1>nul
goto:eof

:failed
echo failed

:end

rd /s /q %TMP_DIR%
pause