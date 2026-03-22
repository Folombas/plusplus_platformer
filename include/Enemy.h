#pragma once

#include "Common.h"
#include <raylib.h>
#include <string>

enum class EnemyType {
    GROUND,
    FLYING,
    JUMPING,
    BOSS
};

enum class EnemyState {
    PATROL,
    CHASE,
    ATTACK,
    HURT,
    DEAD
};

class Enemy {
public:
    Enemy();
    Enemy(Vector2 pos, EnemyType type);
    ~Enemy();
    
    void Update(float dt, const class TileMap& tileMap, const class Player* player);
    void Draw(const class GameCamera2D& camera);
    void LoadSprites();
    
    Vector2 GetPosition() const { return position; }
    Rectangle GetBounds() const;
    bool IsActive() const { return active; }
    EnemyType GetType() const { return type; }
    EnemyState GetState() const { return state; }
    int GetHealth() const { return health; }
    
    void TakeDamage(int damage, Vector2 hitDirection);
    void Kill();
    void SetPatrolPoints(Vector2 left, Vector2 right);
    
private:
    void UpdateAI(float dt, const Player* player);
    void UpdatePhysics(float dt, const class TileMap& tileMap);
    void UpdateAnimation(float dt);
    void CheckTileCollisions(const class TileMap& tileMap);
    
    Vector2 position;
    Vector2 velocity;
    Vector2 patrolLeft;
    Vector2 patrolRight;
    
    EnemyType type;
    EnemyState state;
    bool active;
    
    int health;
    int maxHealth;
    int damage;
    int scoreValue;
    
    // AI
    float detectionRange;
    float attackRange;
    float attackCooldown;
    float attackTimer;
    bool facingRight;
    
    // Movement
    float moveSpeed;
    float jumpForce;
    bool grounded;
    
    // Animation
    Texture2D spritesheet;
    int currentFrame;
    float frameTimer;
    int frameWidth;
    int frameHeight;
    std::string currentState;
    
    // Visual effects
    float hurtFlashTimer;
    float deathTimer;
    Color hurtColor;
};
