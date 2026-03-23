# Скрипт сборки для Platformer
# Требует установленного vcpkg

$ErrorActionPreference = "Stop"

Write-Host "=== Platformer Build Script ===" -ForegroundColor Cyan

# Проверка vcpkg
if (-not $env:VCPKG_ROOT) {
    Write-Host "Error: VCPKG_ROOT environment variable not set!" -ForegroundColor Red
    Write-Host "Please install vcpkg and set VCPKG_ROOT:" -ForegroundColor Yellow
    Write-Host "  git clone https://github.com/microsoft/vcpkg.git C:\Tools\vcpkg" -ForegroundColor Yellow
    Write-Host "  cd C:\Tools\vcpkg" -ForegroundColor Yellow
    Write-Host "  .\bootstrap-vcpkg.bat" -ForegroundColor Yellow
    Write-Host "  setx VCPKG_ROOT 'C:\Tools\vcpkg' -m" -ForegroundColor Yellow
    exit 1
}

Write-Host "VCPKG_ROOT: $env:VCPKG_ROOT" -ForegroundColor Green

# Установка зависимостей
Write-Host "`n=== Installing Dependencies ===" -ForegroundColor Cyan
& "$env:VCPKG_ROOT\vcpkg.exe" install glfw3:x64-windows glew:x64-windows glm:x64-windows stb:x64-windows openal-soft:x64-windows

# Создание папки build
$buildDir = Join-Path $PSScriptRoot "build"
if (-not (Test-Path $buildDir)) {
    New-Item -ItemType Directory -Path $buildDir | Out-Null
}

# Конфигурация и сборка
Write-Host "`n=== Configuring with CMake ===" -ForegroundColor Cyan
Set-Location $buildDir
cmake -DCMAKE_TOOLCHAIN_FILE="$env:VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" ..

if ($LASTEXITCODE -ne 0) {
    Write-Host "CMake configuration failed!" -ForegroundColor Red
    exit 1
}

Write-Host "`n=== Building ===" -ForegroundColor Cyan
cmake --build . --config Release

if ($LASTEXITCODE -ne 0) {
    Write-Host "Build failed!" -ForegroundColor Red
    exit 1
}

# Копирование assets
Write-Host "`n=== Copying Assets ===" -ForegroundColor Cyan
$assetsSrc = Join-Path $PSScriptRoot "assets"
$assetsDst = Join-Path $buildDir "Release\assets"

if (Test-Path $assetsDst) {
    Remove-Item -Path $assetsDst -Recurse -Force
}
Copy-Item -Path $assetsSrc -Destination (Join-Path $buildDir "Release") -Recurse

Write-Host "`n=== Build Complete! ===" -ForegroundColor Green
Write-Host "Executable: $buildDir\Release\Platformer.exe" -ForegroundColor Green
