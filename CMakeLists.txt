cmake_minimum_required(VERSION 3.16)
project(PlusPlusPlatformer VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Fetch raylib
include(FetchContent)
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.0
)
FetchContent_MakeAvailable(raylib)

# Source files
set(SOURCES
    src/main.cpp
    src/Game.cpp
    src/Player.cpp
    src/Enemy.cpp
    src/Level.cpp
    src/ParticleSystem.cpp
    src/AudioManager.cpp
    src/AchievementManager.cpp
    src/Camera2D.cpp
    src/TileMap.cpp
    src/Common.cpp
)

set(HEADERS
    include/Game.h
    include/Player.h
    include/Enemy.h
    include/Level.h
    include/ParticleSystem.h
    include/AudioManager.h
    include/AchievementManager.h
    include/Camera2D.h
    include/TileMap.h
    include/Common.h
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_include_directories(${PROJECT_NAME} PRIVATE include)
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)

# Copy assets to build directory
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})

# Copy external assets from parent directory if they exist
set(EXTERNAL_SPRITES "${CMAKE_SOURCE_DIR}/../sprites")
set(EXTERNAL_SOUNDS "${CMAKE_SOURCE_DIR}/../sounds")
set(EXTERNAL_FONTS "${CMAKE_SOURCE_DIR}/../fonts")
set(EXTERNAL_UI "${CMAKE_SOURCE_DIR}/../UI")

# Copy external assets to the assets folder in build directory
if(EXISTS ${EXTERNAL_SPRITES})
    message(STATUS "Found external sprites, copying to assets...")
    file(COPY ${EXTERNAL_SPRITES} DESTINATION ${CMAKE_BINARY_DIR}/assets/external_resources)
endif()

if(EXISTS ${EXTERNAL_SOUNDS})
    message(STATUS "Found external sounds, copying to assets...")
    file(COPY ${EXTERNAL_SOUNDS} DESTINATION ${CMAKE_BINARY_DIR}/assets/external_resources)
endif()

if(EXISTS ${EXTERNAL_FONTS})
    message(STATUS "Found external fonts, copying to assets...")
    file(COPY ${EXTERNAL_FONTS} DESTINATION ${CMAKE_BINARY_DIR}/assets/external_resources)
endif()

if(EXISTS ${EXTERNAL_UI})
    message(STATUS "Found external UI assets, copying to assets...")
    file(COPY ${EXTERNAL_UI} DESTINATION ${CMAKE_BINARY_DIR}/assets/external_resources)
endif()

# Platform-specific settings
if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE PLATFORM_DESKTOP)
    target_link_libraries(${PROJECT_NAME} PRIVATE opengl32 glu32)
endif()
