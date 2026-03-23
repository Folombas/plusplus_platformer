# Script to extract all asset ZIP files into the project's assets folder

$projectRoot = Split-Path -Parent $PSScriptRoot
$assetsDir = "$PSScriptRoot\assets"

# Add System.IO.Compression.FileSystem for ZIP extraction
Add-Type -AssemblyName System.IO.Compression.FileSystem

function Extract-Zip {
    param(
        [string]$zipPath,
        [string]$destPath
    )
    
    if (Test-Path $zipPath) {
        Write-Host "Extracting: $zipPath"
        
        # Create destination if it doesn't exist
        if (-not (Test-Path $destPath)) {
            New-Item -ItemType Directory -Force -Path $destPath | Out-Null
        }
        
        try {
            $zip = [System.IO.Compression.ZipFile]::OpenRead($zipPath)
            foreach ($entry in $zip.Entries) {
                # Skip directories and system files
                if ($entry.Name -eq "" -or $entry.Name.StartsWith("__MACOSX")) {
                    continue
                }
                
                $fullPath = Join-Path $destPath $entry.FullName
                
                # Create directory if needed
                $dir = Split-Path -Parent $fullPath
                if ($dir -and -not (Test-Path $dir)) {
                    New-Item -ItemType Directory -Force -Path $dir | Out-Null
                }
                
                # Extract file
                [System.IO.Compression.ZipFileExtensions]::ExtractToFile($entry, $fullPath, $true)
            }
            $zip.Dispose()
            Write-Host "  -> Extracted to: $destPath" -ForegroundColor Green
        }
        catch {
            Write-Host "  -> Error extracting: $_" -ForegroundColor Red
        }
    }
}

Write-Host "======================================" -ForegroundColor Cyan
Write-Host "Extracting Asset ZIP Files" -ForegroundColor Cyan
Write-Host "======================================" -ForegroundColor Cyan
Write-Host ""

# Extract Sprites
Write-Host "--- SPRITES ---" -ForegroundColor Yellow
$spritesDir = "$projectRoot\sprites"
if (Test-Path $spritesDir) {
    $spriteZips = Get-ChildItem -Path $spritesDir -Filter "*.zip"
    foreach ($zip in $spriteZips) {
        $destName = $zip.BaseName
        Extract-Zip -zipPath $zip.FullName -destPath "$assetsDir\sprites\$destName"
    }
} else {
    Write-Host "Sprites folder not found: $spritesDir" -ForegroundColor Red
}
Write-Host ""

# Extract Sounds
Write-Host "--- SOUNDS ---" -ForegroundColor Yellow
$soundsDir = "$projectRoot\sounds"
if (Test-Path $soundsDir) {
    $soundZips = Get-ChildItem -Path $soundsDir -Filter "*.zip"
    foreach ($zip in $soundZips) {
        $destName = $zip.BaseName
        Extract-Zip -zipPath $zip.FullName -destPath "$assetsDir\audio\$destName"
    }
} else {
    Write-Host "Sounds folder not found: $soundsDir" -ForegroundColor Red
}
Write-Host ""

# Extract Fonts
Write-Host "--- FONTS ---" -ForegroundColor Yellow
$fontsDir = "$projectRoot\fonts"
if (Test-Path $fontsDir) {
    $fontZips = Get-ChildItem -Path $fontsDir -Filter "*.zip"
    foreach ($zip in $fontZips) {
        $destName = $zip.BaseName
        Extract-Zip -zipPath $zip.FullName -destPath "$assetsDir\fonts\$destName"
    }
} else {
    Write-Host "Fonts folder not found: $fontsDir" -ForegroundColor Red
}
Write-Host ""

# Extract UI
Write-Host "--- UI ---" -ForegroundColor Yellow
$uiDir = "$projectRoot\UI"
if (Test-Path $uiDir) {
    $uiZips = Get-ChildItem -Path $uiDir -Filter "*.zip"
    foreach ($zip in $uiZips) {
        $destName = $zip.BaseName
        Extract-Zip -zipPath $zip.FullName -destPath "$assetsDir\ui\$destName"
    }
} else {
    Write-Host "UI folder not found: $uiDir" -ForegroundColor Red
}
Write-Host ""

Write-Host "======================================" -ForegroundColor Cyan
Write-Host "Asset Extraction Complete!" -ForegroundColor Green
Write-Host "======================================" -ForegroundColor Cyan
