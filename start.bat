@echo off
setlocal
cd /d "%~dp0"
if not exist grid-bridge.exe (
  echo grid-bridge.exe not found. Run build.bat first.
  pause
  exit /b 1
)
node server.js
pause
