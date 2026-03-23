# 2D Platformer на C++ и OpenGL

Классический 2D-платформер с использованием OpenGL, GLFW и OpenAL.

## Требования

- **Компилятор**: Visual Studio 2019/2022 (MSVC) или MinGW-w64
- **CMake**: 3.16 или выше
- **vcpkg**: для управления зависимостями

## Зависимости

Проект использует vcpkg для автоматической установки зависимостей:
- glfw3
- glew
- glm
- stb
- openal-soft

## Установка vcpkg (если не установлен)

```powershell
# Клонируем vcpkg
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg

# Устанавливаем
.\bootstrap-vcpkg.bat

# Устанавливаем переменную окружения
setx VCPKG_ROOT "%CD%" -m
```

## Сборка проекта

### Вариант 1: Использование vcpkg (рекомендуется)

```powershell
cd D:\Projects\gamedev\plusplus_platformer

# Создаем папку для сборки
mkdir build
cd build

# Конфигурируем с vcpkg
cmake -DCMAKE_TOOLCHAIN_FILE="$env:VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" ..

# Собираем
cmake --build . --config Release
```

### Вариант 2: Ручная установка зависимостей

Если вы предпочитаете установить зависимости вручную:

```powershell
# Через vcpkg
vcpkg install glfw3:x64-windows glew:x64-windows glm:x64-windows stb:x64-windows openal-soft:x64-windows

# Затем собираем
mkdir build && cd build
cmake ..
cmake --build . --config Release
```

## Запуск игры

После успешной сборки исполняемый файл будет находиться в:
```
build\Release\Platformer.exe
```

Или скопируйте папку `assets` в папку с исполняемым файлом.

## Управление

| Клавиша | Действие |
|---------|----------|
| A / ← | Движение влево |
| D / → | Движение вправо |
| Space / W / ↑ | Прыжок |
| S / ↓ | Приседание |
| P | Пауза |
| R | Рестарт |
| ESC | Выход |

## Структура проекта

```
plusplus_platformer/
├── src/
│   ├── main.cpp          # Точка входа
│   ├── Game.h/cpp        # Основной класс игры
│   ├── Player.h/cpp      # Игрок
│   ├── Enemy.h/cpp       # Враги
│   ├── TileMap.h/cpp     # Карта уровня
│   ├── Renderer.h/cpp    # Рендеринг OpenGL
│   ├── Texture.h/cpp     # Управление текстурами
│   ├── Animation.h/cpp   # Система анимаций
│   ├── Audio.h/cpp       # Звук (OpenAL)
│   ├── Input.h/cpp       # Ввод (GLFW)
│   └── Common.h          # Общие определения
├── assets/
│   ├── Base pack/        # Спрайты персонажей и врагов
│   ├── Buildings expansion/ # Тайлы зданий
│   ├── audio/            # Звуковые эффекты
│   └── level.txt         # Файл уровня
├── CMakeLists.txt        # Конфигурация CMake
└── vcpkg.json            # Зависимости vcpkg
```

## Особенности

- **Анимации**: Плавные анимации ходьбы, прыжков, получения урона
- **Враги**: Различные типы врагов (слизни, улитки, летучие мыши, рыбы)
- **Коллекционные предметы**: Монеты и сердца для восстановления здоровья
- **Физика**: Гравитация, коллизии, платформы
- **Звуки**: Эффекты прыжков и получения урона
- **HUD**: Отображение здоровья, монет и очков

## Создание своего уровня

Откройте `assets/level.txt` и используйте следующие символы:

```
' ' - Воздух
'#' - Земля (твердый)
'=' - Платформа (односторонняя)
'B' - Кирпич (твердый)
'X' - Блок (твердый)
'C' - Монета
'H' - Сердце
'P' - Спавн игрока
'S' - Слизень (враг)
'T' - Летучая мышь (враг)
```

## Лицензия

Спрайты и звуки предоставлены Kenney (www.kenney.nl).
