@echo off
setlocal EnableExtensions EnableDelayedExpansion
rem OpenXP configure/build entry point for Windows.
rem Usage: configure.cmd [compiler] [architecture]
rem   compiler: auto, vs, clang, mingw
rem   architecture: auto, i386, amd64

set "ROOT=%~dp0"
set "COMPILER=%~1"
set "ARCH=%~2"
if not defined COMPILER set "COMPILER=auto"
if not defined ARCH set "ARCH=auto"

if /I "%COMPILER%"=="auto" (
    where cl.exe >nul 2>&1
    if not errorlevel 1 (set "COMPILER=vs") else (
        where clang-cl.exe >nul 2>&1
        if not errorlevel 1 (set "COMPILER=clang") else (
            where gcc.exe >nul 2>&1
            if not errorlevel 1 (set "COMPILER=mingw") else (
                echo Error: no supported compiler found in PATH.
                echo Run this script from a Visual Studio Developer Command Prompt,
                echo or pass a compiler explicitly: configure.cmd vs amd64
                exit /b 1
            )
        )
    )
)

if /I "%ARCH%"=="auto" (
    if defined PROCESSOR_ARCHITEW6432 (set "HOST_ARCH=%PROCESSOR_ARCHITEW6432%") else (set "HOST_ARCH=%PROCESSOR_ARCHITECTURE%")
    if /I "!HOST_ARCH!"=="AMD64" (set "ARCH=amd64") else if /I "!HOST_ARCH!"=="ARM64" (set "ARCH=amd64") else (set "ARCH=i386")
)

if /I not "%COMPILER%"=="vs" if /I not "%COMPILER%"=="clang" if /I not "%COMPILER%"=="mingw" (
    echo Error: unsupported compiler "%COMPILER%".
    exit /b 1
)
if /I not "%ARCH%"=="i386" if /I not "%ARCH%"=="amd64" (
    echo Error: unsupported architecture "%ARCH%".
    exit /b 1
)

where cmake.exe >nul 2>&1
if errorlevel 1 (echo Error: cmake.exe was not found in PATH.& exit /b 1)
where ninja.exe >nul 2>&1
if errorlevel 1 (echo Error: ninja.exe was not found in PATH.& exit /b 1)

set "OUTPUT=%ROOT%output-%COMPILER%-%ARCH%"
set "BUILD=%OUTPUT%\build"
if exist "%OUTPUT%" rmdir /s /q "%OUTPUT%"
mkdir "%BUILD%"

echo ==============================================
echo OpenXP configure/build
echo ==============================================
echo Compiler:     %COMPILER%
echo Architecture: %ARCH%
echo Generator:    Ninja
echo Output:       %OUTPUT%
echo ==============================================

if /I "%COMPILER%"=="vs" (
    if not defined VSCMD_VER if not defined VSINSTALLDIR (
        echo Error: Visual Studio environment is not initialized.
        echo Run from a Visual Studio Developer Command Prompt or use the CI setup step.
        exit /b 1
    )
    cmake -S "%ROOT%" -B "%BUILD%" -G Ninja -DWRK_ARCH=%ARCH% -DCMAKE_BUILD_TYPE=Release
) else if /I "%COMPILER%"=="clang" (
    set "CC=clang-cl"
    set "CXX=clang-cl"
    cmake -S "%ROOT%" -B "%BUILD%" -G Ninja -DWRK_ARCH=%ARCH% -DCMAKE_C_COMPILER=!CC! -DCMAKE_CXX_COMPILER=!CXX! -DCMAKE_BUILD_TYPE=Release
) else (
    if /I "%ARCH%"=="i386" (set "CC=i686-w64-mingw32-gcc"& set "CXX=i686-w64-mingw32-g++"& set "FLAGS=-m32") else (set "CC=x86_64-w64-mingw32-gcc"& set "CXX=x86_64-w64-mingw32-g++"& set "FLAGS=-m64")
    cmake -S "%ROOT%" -B "%BUILD%" -G Ninja -DWRK_ARCH=%ARCH% -DCMAKE_C_COMPILER=!CC! -DCMAKE_CXX_COMPILER=!CXX! -DCMAKE_C_FLAGS=!FLAGS! -DCMAKE_CXX_FLAGS=!FLAGS! -DCMAKE_BUILD_TYPE=Release
)
if errorlevel 1 exit /b %errorlevel%

cmake --build "%BUILD%" --parallel
if errorlevel 1 exit /b %errorlevel%

echo.
echo Build completed successfully.
echo Output directory: %OUTPUT%
endlocal
