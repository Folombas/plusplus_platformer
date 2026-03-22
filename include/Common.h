#pragma once

#include <raylib.h>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>

// Constants
constexpr int SCREEN_WIDTH = 1280;
constexpr int SCREEN_HEIGHT = 720;
constexpr int TILE_SIZE = 48;
constexpr float GRAVITY = 1500.0f;
constexpr float PLAYER_SPEED = 300.0f;
constexpr float PLAYER_JUMP = -650.0f;
constexpr float ENEMY_SPEED = 100.0f;

// Game states
enum class GameState {
    MENU,
    PLAYING,
    PAUSED,
    LEVEL_COMPLETE,
    GAME_OVER,
    VICTORY
};

// Entity types
enum class EntityType {
    PLAYER,
    ENEMY_GROUND,
    ENEMY_FLYING,
    COLLECTIBLE,
    POWERUP,
    PARTICLE
};

// Power-up types
enum class PowerUpType {
    NONE,
    SPEED_BOOST,
    INVINCIBILITY,
    DOUBLE_JUMP,
    SHIELD
};

// Achievement structure
struct Achievement {
    std::string id;
    std::string name;
    std::string description;
    bool unlocked;
    int progress;
    int required;
    Color color;
};

// Particle structure
struct Particle {
    Vector2 position;
    Vector2 velocity;
    float life;
    float maxLife;
    Color color;
    float size;
    bool active;
    bool fadeOut;
    bool shrink;
};

// Collectible structure
struct Collectible {
    Vector2 position;
    std::string type;
    bool collected;
    float bobOffset;
    int value;
};

// Damage number for visual feedback
struct DamageNumber {
    Vector2 position;
    int damage;
    float life;
    Color color;
    bool active;
};

// Utility functions
inline Rectangle GetSpriteSource(const Texture2D& texture, int frameWidth, int frameHeight, int frameIndex, int framesPerRow = -1) {
    if (framesPerRow < 0) framesPerRow = texture.width / frameWidth;
    int row = frameIndex / framesPerRow;
    int col = frameIndex % framesPerRow;
    return { static_cast<float>(col * frameWidth), static_cast<float>(row * frameHeight), 
             static_cast<float>(frameWidth), static_cast<float>(frameHeight) };
}

inline Color TintAlpha(Color color, float alpha) {
    return { color.r, color.g, color.b, static_cast<unsigned char>(color.a * alpha) };
}

inline Color LerpColor(Color from, Color to, float t) {
    return {
        static_cast<unsigned char>(from.r + (to.r - from.r) * t),
        static_cast<unsigned char>(from.g + (to.g - from.g) * t),
        static_cast<unsigned char>(from.b + (to.b - from.b) * t),
        static_cast<unsigned char>(from.a + (to.a - from.a) * t)
    };
}

inline float Lerp(float from, float to, float t) {
    return from + (to - from) * t;
}

inline Vector2 LerpVec2(Vector2 from, Vector2 to, float t) {
    return { Lerp(from.x, to.x, t), Lerp(from.y, to.y, t) };
}

// Additional utility functions for raylib 5.x
inline float Vector2Distance(Vector2 v1, Vector2 v2) {
    return sqrtf((v2.x - v1.x) * (v2.x - v1.x) + (v2.y - v1.y) * (v2.y - v1.y));
}

inline void DrawGradientV(int posX, int posY, int width, int height, Color top, Color bottom) {
    for (int i = 0; i < height; i++) {
        float t = static_cast<float>(i) / height;
        Color c = LerpColor(top, bottom, t);
        DrawLine(posX, posY + i, posX + width, posY + i, c);
    }
}

// Collision helpers
inline bool CheckCircleCollision(Vector2 pos1, float radius1, Vector2 pos2, float radius2) {
    float dx = pos2.x - pos1.x;
    float dy = pos2.y - pos1.y;
    float distance = sqrtf(dx * dx + dy * dy);
    return distance < (radius1 + radius2);
}

inline bool CheckRectCircleCollision(Rectangle rect, Vector2 circlePos, float circleRadius) {
    float closestX = fmaxf(rect.x, fminf(circlePos.x, rect.x + rect.width));
    float closestY = fmaxf(rect.y, fminf(circlePos.y, rect.y + rect.height));
    float dx = circlePos.x - closestX;
    float dy = circlePos.y - closestY;
    return (dx * dx + dy * dy) < (circleRadius * circleRadius);
}

// Screen shake effect
struct ScreenShake {
    float intensity;
    float duration;
    float timer;
    bool active;
    
    ScreenShake() : intensity(0), duration(0), timer(0), active(false) {}
    
    void Start(float intensity, float duration) {
        this->intensity = intensity;
        this->duration = duration;
        this->timer = 0;
        this->active = true;
    }
    
    Vector2 Update(float dt) {
        if (!active) return { 0, 0 };
        
        timer -= dt;
        if (timer <= 0) {
            active = false;
            return { 0, 0 };
        }
        
        float currentIntensity = intensity * (timer / duration);
        return {
            (GetRandomValue(-100, 100) / 100.0f) * currentIntensity,
            (GetRandomValue(-100, 100) / 100.0f) * currentIntensity
        };
    }
};

// Windows-specific functions
#ifdef _WIN32
#include <direct.h>
#endif
#include <cerrno>

static inline bool MyDirectoryExists(const char* path) {
#ifdef _WIN32
    // Simple implementation without windows.h
    FILE* f = fopen(path, "r");
    if (f) { fclose(f); return true; }
    return false;
#else
    (void)path;
    return false;
#endif
}

static inline bool MyMakeDirectory(const char* path) {
#ifdef _WIN32
    return _mkdir(path) == 0 || errno == EEXIST;
#else
    (void)path;
    return false;
#endif
}

#define DirectoryExists MyDirectoryExists
#define MakeDirectory MyMakeDirectory
