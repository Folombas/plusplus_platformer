#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <random>
#include <cstring>

// Константы игры
constexpr float GRAVITY = 2000.0f;
constexpr float PLAYER_SPEED = 300.0f;
constexpr float PLAYER_JUMP_FORCE = 700.0f;
constexpr float TILE_SIZE = 64.0f;
constexpr int WINDOW_WIDTH = 1280;
constexpr int WINDOW_HEIGHT = 720;

// Состояния игрока
enum class PlayerState {
    Idle,
    Running,
    Jumping,
    Falling,
    Hurt,
    Dead
};

// Типы тайлов
enum class TileType {
    Air,
    Ground,
    Platform,
    Brick,
    Block,
    Spike,
    Coin,
    Heart,
    Lava,
    Water
};

// Структура для тайла
struct Tile {
    TileType type;
    int textureId;
    bool solid;
    bool animated;
};

// AABB коллизия
struct AABB {
    float x, y, width, height;
    
    bool intersects(const AABB& other) const {
        return x < other.x + other.width &&
               x + width > other.x &&
               y < other.y + other.height &&
               y + height > other.y;
    }
    
    glm::vec2 center() const {
        return glm::vec2(x + width / 2.0f, y + height / 2.0f);
    }
};

// Утилиты
namespace Utils {
    inline float clamp(float value, float min, float max) {
        return std::max(min, std::min(max, value));
    }
    
    inline bool pointInRect(float px, float py, float rx, float ry, float rw, float rh) {
        return px >= rx && px <= rx + rw && py >= ry && py <= ry + rh;
    }
}

// Объявления классов для устранения циклических зависимостей
class Player;
class Enemy;
class TileMap;
class Game;
