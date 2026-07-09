@echo off
setlocal

rem Flow: VS Code task -> this wrapper -> MSVC vcvars64 -> cmake command.
rem Calls: vswhere locates Visual Studio without hardcoding the install drive.

set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
set "VS_INSTALL="

if exist "%VSWHERE%" (
    for /f "usebackq tokens=*" %%i in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do (
        set "VS_INSTALL=%%i"
    )
)

if not defined VS_INSTALL (
    echo Failed to locate Visual Studio with MSVC tools.
    echo Install Visual Studio C++ tools or run this task from a Developer Command Prompt.
    exit /b 1
)

set "VCVARS=%VS_INSTALL%\VC\Auxiliary\Build\vcvars64.bat"
if not exist "%VCVARS%" (
    echo Failed to locate vcvars64.bat:
    echo %VCVARS%
    exit /b 1
)

call "%VCVARS%" >nul
cmake %*
exit /b %ERRORLEVEL%
