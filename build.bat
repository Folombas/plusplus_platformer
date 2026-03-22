@echo off
echo ========================================
echo   PlusPlus Platformer - Build Script
echo ========================================
echo.

REM Check if Visual Studio is installed
where msbuild >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo ERROR: MSBuild not found. Please install Visual Studio 2022.
    echo.
    echo Download from: https://visualstudio.microsoft.com/
    pause
    exit /b 1
)

echo Building PlusPlus Platformer...
echo.

REM Build Release configuration
msbuild PlusPlusPlatformer.sln /p:Configuration=Release /p:Platform=x64 /v:quiet

if %ERRORLEVEL% equ 0 (
    echo.
    echo ========================================
    echo   BUILD SUCCESSFUL!
    echo ========================================
    echo.
    echo Executable: bin\x64\Release\PlusPlusPlatformer.exe
    echo.
    echo Run the game? (Y/N)
    set /p runGame=
    if /i "%runGame%"=="Y" (
        start "" "bin\x64\Release\PlusPlusPlatformer.exe"
    )
) else (
    echo.
    echo ========================================
    echo   BUILD FAILED!
    echo ========================================
    echo.
    echo Please check the error messages above.
    echo Make sure you have:
    echo   1. Visual Studio 2022 installed
    echo   2. Run setup.ps1 to download raylib
)

echo.
pause
