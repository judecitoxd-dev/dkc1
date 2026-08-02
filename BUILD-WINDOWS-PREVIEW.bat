@echo off
setlocal
cd /d "%~dp0"
powershell.exe -NoLogo -NoProfile -ExecutionPolicy Bypass -File "%~dp0tools\build_windows_preview.ps1" %*
set "DK1_BUILD_EXIT=%ERRORLEVEL%"
if not "%DK1_BUILD_EXIT%"=="0" (
    echo.
    echo DK1 Windows preview build failed with exit code %DK1_BUILD_EXIT%.
    pause
    exit /b %DK1_BUILD_EXIT%
)
echo.
echo DK1 Windows preview build completed successfully.
pause
exit /b 0
