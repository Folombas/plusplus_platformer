# Script to create placeholder audio files
# Note: These are silent placeholders - add real audio files for best experience

$audioDir = Join-Path $PSScriptRoot "assets\audio"

if (!(Test-Path $audioDir)) {
    New-Item -ItemType Directory -Path $audioDir | Out-Null
}

# Create empty files as placeholders
$soundFiles = @(
    "jump.wav",
    "land.wav", 
    "coin.wav",
    "powerup.wav",
    "hurt.wav",
    "death.wav",
    "enemy_death.wav",
    "attack.wav",
    "level_complete.wav",
    "menu_select.wav",
    "menu_back.wav",
    "achievement.wav",
    "water_splash.wav",
    "music_menu.ogg",
    "music_forest.ogg",
    "music_cave.ogg",
    "music_castle.ogg",
    "music_boss.ogg",
    "music_victory.ogg"
)

foreach ($file in $soundFiles) {
    $path = Join-Path $audioDir $file
    if (!(Test-Path $path)) {
        # Create empty file
        "" | Out-File -FilePath $path -Encoding ASCII
        Write-Host "Created placeholder: $file" -ForegroundColor Yellow
    }
}

Write-Host "`nPlaceholder audio files created." -ForegroundColor Green
Write-Host "Add real .wav and .ogg files to assets/audio/ for sound." -ForegroundColor Cyan
