@echo off
setlocal
cd /d "%~dp0"
where g++ >nul 2>nul
if errorlevel 1 (
  echo ERROR: g++ was not found. Install a C++ compiler such as MinGW-w64 and add it to PATH.
  pause
  exit /b 1
)
echo Building the original C++ project test suite...
g++ -std=c++17 tests.cpp Graph.cpp UnionFind.cpp Connectivity.cpp Dijkstra.cpp Kruskal.cpp Scheduler.cpp -o tests.exe
if errorlevel 1 (
  echo ERROR: Original C++ tests did not compile.
  pause
  exit /b 1
)
echo Running original C++ tests...
tests.exe
if errorlevel 1 (
  echo ERROR: Original tests failed.
  pause
  exit /b 1
)
echo.
echo Building the browser integration bridge...
g++ -std=c++17 bridge.cpp Graph.cpp UnionFind.cpp Connectivity.cpp Dijkstra.cpp Kruskal.cpp Scheduler.cpp -o grid-bridge.exe
if errorlevel 1 (
  echo ERROR: Browser bridge did not compile.
  pause
  exit /b 1
)
where node >nul 2>nul
if errorlevel 1 (
  echo ERROR: Node.js was not found. Install Node.js LTS.
  pause
  exit /b 1
)
node --check server.js
if errorlevel 1 (
  echo ERROR: server.js syntax check failed.
  pause
  exit /b 1
)
echo.
echo BUILD AND ORIGINAL TESTS PASSED.
echo Run start.bat to launch the browser simulation.
pause
