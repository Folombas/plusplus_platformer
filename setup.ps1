# Setup Script for PlusPlus Platformer
# Run this script to download and setup raylib

Write-Host "Setting up PlusPlus Platformer..." -ForegroundColor Green

# Create raylib directory
$raylibDir = Join-Path $PSScriptRoot "raylib"
if (!(Test-Path $raylibDir)) {
    New-Item -ItemType Directory -Path $raylibDir | Out-Null
}

# Download raylib
Write-Host "Downloading raylib..." -ForegroundColor Yellow
$raylibUrl = "https://github.com/raysan5/raylib/releases/download/5.0/raylib-5.0_win64_msvc19.zip"
$raylibZip = Join-Path $raylibDir "raylib.zip"

try {
    Invoke-WebRequest -Uri $raylibUrl -OutFile $raylibZip
    Write-Host "Extracting raylib..." -ForegroundColor Yellow
    
    # Extract
    Expand-Archive -Path $raylibZip -DestinationPath $raylibDir -Force
    
    # Move files to correct locations
    $includeDir = Join-Path $raylibDir "include"
    $libDir = Join-Path $raylibDir "lib"
    
    if (!(Test-Path $includeDir)) {
        New-Item -ItemType Directory -Path $includeDir | Out-Null
    }
    if (!(Test-Path $libDir)) {
        New-Item -ItemType Directory -Path $libDir | Out-Null
    }
    
    # Copy headers
    Get-ChildItem -Path (Join-Path $raylibDir "include\*.h") -ErrorAction SilentlyContinue | 
        Copy-Item -Destination $includeDir -Force
    
    # Copy lib
    Get-ChildItem -Path (Join-Path $raylibDir "lib\raylib.lib") -ErrorAction SilentlyContinue | 
        Copy-Item -Destination $libDir -Force
    
    # Cleanup
    Remove-Item $raylibZip -Force
    Remove-Item (Join-Path $raylibDir "include") -Recurse -Force -ErrorAction SilentlyContinue
    Remove-Item (Join-Path $raylibDir "lib") -Recurse -Force -ErrorAction SilentlyContinue
    
    Write-Host "Setup complete!" -ForegroundColor Green
    Write-Host "You can now open PlusPlusPlatformer.sln in Visual Studio" -ForegroundColor Cyan
}
catch {
    Write-Host "Error downloading raylib: $_" -ForegroundColor Red
    Write-Host "Please download manually from: https://github.com/raysan5/raylib/releases" -ForegroundColor Yellow
}
