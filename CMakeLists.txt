cmake_minimum_required(VERSION 3.16)
project(Platformer VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Интеграция с vcpkg
if(DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()

# Поиск зависимостей
find_package(OpenGL REQUIRED)

find_package(glfw3 CONFIG REQUIRED)
find_package(glew CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(stb CONFIG REQUIRED)

# OpenAL для звука
find_package(unofficial-openal-soft CONFIG QUIET)
if(NOT unofficial-openal-soft_FOUND)
    find_package(OpenAL REQUIRED)
endif()

# Исходные файлы
set(SOURCES
    src/main.cpp
    src/Game.cpp
    src/Player.cpp
    src/Enemy.cpp
    src/TileMap.cpp
    src/Renderer.cpp
    src/Texture.cpp
    src/Animation.cpp
    src/Audio.cpp
    src/Input.cpp
)

set(HEADERS
    src/Game.h
    src/Player.h
    src/Enemy.h
    src/TileMap.h
    src/Renderer.h
    src/Texture.h
    src/Animation.h
    src/Audio.h
    src/Input.h
    src/Common.h
)

# Создаем исполняемый файл
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${OPENGL_INCLUDE_DIR}
    src
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    OpenGL::GL
    glfw
    GLEW::GLEW
    glm::glm
    unofficial::openal-soft
)

# Копирование assets в папку сборки
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/assets"
    "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
)
