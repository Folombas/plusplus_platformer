# Инструкция по сборке для Windows

## Способ 1: Установка через vcpkg (рекомендуется)

1. Установите vcpkg:
```powershell
cd C:\Tools
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
```

2. Установите зависимости:
```powershell
.\vcpkg install glfw3:x64-windows glew:x64-windows glm:x64-windows stb:x64-windows openal-soft:x64-windows
```

3. Установите переменную окружения:
```powershell
setx VCPKG_ROOT "C:\Tools\vcpkg" -m
```

4. Соберите проект:
```powershell
cd D:\Projects\gamedev\plusplus_platformer
mkdir build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE="$env:VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" ..
cmake --build . --config Release
```

## Способ 2: Ручная загрузка библиотек

Если вы не хотите использовать vcpkg, создайте следующую структуру:

```
plusplus_platformer/
├── lib/
│   ├── glfw3/
│   ├── glew/
│   ├── glm/
│   ├── stb/
│   └── openal-soft/
```

Затем отредактируйте CMakeLists.txt для указания путей к библиотекам.

## Быстрая сборка (если vcpkg установлен глобально)

```powershell
cd D:\Projects\gamedev\plusplus_platformer
mkdir build && cd build
cmake ..
cmake --build . --config Release
```

## Запуск

После сборки скопируйте папку assets в папку с исполняемым файлом:
```powershell
copy /Y ..\..\assets .\Release\
```

Или запустите из папки build:
```powershell
cd D:\Projects\gamedev\plusplus_platformer\build\Release
.\Platformer.exe
```
