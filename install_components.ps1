# Скрипт установки компонентов для сборки PlusPlus Platformer

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  Установка компонентов для сборки" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Проверка наличия Visual Studio
$vsPath = "C:\Program Files\Microsoft Visual Studio\18\Community"
if (!(Test-Path $vsPath)) {
    $vsPath = "C:\Program Files (x86)\Microsoft Visual Studio\2022\Community"
}

if (!(Test-Path $vsPath)) {
    Write-Host "ERROR: Visual Studio не найдена!" -ForegroundColor Red
    Write-Host ""
    Write-Host "Скачайте Visual Studio 2022 Community:" -ForegroundColor Yellow
    Write-Host "https://visualstudio.microsoft.com/downloads/" -ForegroundColor Cyan
    Write-Host ""
    Write-Host "При установке выберите:" -ForegroundColor Yellow
    Write-Host "  - 'Разработка классических приложений на C++'" -ForegroundColor White
    Write-Host "  - 'Разработка игр на C++' (опционально)" -ForegroundColor White
    Pause
    Exit
}

# Проверка наличия VC++ инструментов
$vcToolsPath = Join-Path $vsPath "VC\Tools\MSVC"
if (!(Test-Path $vcToolsPath)) {
    Write-Host "WARNING: Инструменты C++ не найдены!" -ForegroundColor Yellow
    Write-Host ""
    Write-Host "Необходимо установить компоненты C++ через Visual Studio Installer:" -ForegroundColor Yellow
    Write-Host ""
    Write-Host "1. Откройте 'Visual Studio Installer'" -ForegroundColor White
    Write-Host "2. Нажмите 'Изменить' на вашей версии Visual Studio" -ForegroundColor White
    Write-Host "3. Отметьте 'Разработка классических приложений на C++'" -ForegroundColor White
    Write-Host "4. Нажмите 'Изменить' для установки" -ForegroundColor White
    Write-Host ""
    
    # Попытка запустить установщик
    $installerPath = "C:\Program Files (x86)\Microsoft Visual Studio\Installer\vs_installer.exe"
    if (Test-Path $installerPath) {
        Write-Host "Запуск Visual Studio Installer..." -ForegroundColor Green
        Start-Process $installerPath
    }
    
    Pause
    Exit
}

Write-Host "SUCCESS: Все компоненты найдены!" -ForegroundColor Green
Write-Host ""
Write-Host "Теперь вы можете собрать проект:" -ForegroundColor Cyan
Write-Host "  1. Откройте PlusPlusPlatformer.sln в Visual Studio" -ForegroundColor White
Write-Host "  2. Выберите Release|x64" -ForegroundColor White
Write-Host "  3. Нажмите Ctrl+Shift+B для сборки" -ForegroundColor White
Write-Host "  4. Нажмите F5 для запуска" -ForegroundColor White
Write-Host ""
Pause
