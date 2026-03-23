@echo off
echo ========================================
echo   PlusPlus Platformer - Manual Build
echo ========================================
echo.

REM Set up Visual Studio environment
call "C:\Program Files\Microsoft Visual Studio\18\Community\Common7\Tools\VsDevCmd.bat" -arch=amd64 -host_arch=amd64

if %ERRORLEVEL% neq 0 (
    echo ERROR: Failed to initialize Visual Studio environment
    pause
    exit /b 1
)

echo Compiling with cl.exe...
echo.

REM Create output directory
if not exist "bin\Manual" mkdir "bin\Manual"

REM Source files
set SOURCES=^
    src\main.cpp ^
    src\Game.cpp ^
    src\Player.cpp ^
    src\Enemy.cpp ^
    src\Level.cpp ^
    src\ParticleSystem.cpp ^
    src\AudioManager.cpp ^
    src\AchievementManager.cpp ^
    src\Camera2D.cpp ^
    src\TileMap.cpp ^
    src\Common.cpp

REM Compile - Use /MD for dynamic CRT (matches raylib)
cl.exe /EHsc /O2 /std:c++17 /MD ^
    /I "include" ^
    /I "raylib\include" ^
    /D "PLATFORM_DESKTOP" ^
    /D "NDEBUG" ^
    %SOURCES% ^
    /link ^
    /OUT:"bin\Manual\PlusPlusPlatformer.exe" ^
    "raylib\lib\raylib.lib" ^
    opengl32.lib glu32.lib ^
    winmm.lib user32.lib gdi32.lib shell32.lib ^
    kernel32.lib advapi32.lib ole32.lib oleaut32.lib ^
    comdlg32.lib uuid.lib ^
    legacy_stdio_definitions.lib ^
    /SUBSYSTEM:CONSOLE

if %ERRORLEVEL% equ 0 (
    echo.
    echo ========================================
    echo   BUILD SUCCESSFUL!
    echo ========================================
    echo.
    echo Executable: bin\Manual\PlusPlusPlatformer.exe
    echo.
    
    REM Copy assets
    echo Copying assets...
    xcopy /E /I /Y "assets" "bin\Manual\assets"
    
    echo.
    echo Run the game? (Y/N)
    set /p runGame=
    if /i "%runGame%"=="Y" (
        start "" "bin\Manual\PlusPlusPlatformer.exe"
    )
) else (
    echo.
    echo ========================================
    echo   BUILD FAILED!
    echo ========================================
)

echo.
pause
