#pragma once

#include "Common.h"
#include <raylib.h>
#include <string>

class Player {
public:
    Player();
    ~Player();
    
    void Update(float dt, const class TileMap& tileMap);
    void Draw(const class GameCamera2D& camera);
    void Reset(Vector2 startPosition);
    
    // Getters
    Vector2 GetPosition() const { return position; }
    Vector2 GetVelocity() const { return velocity; }
    void SetVelocity(Vector2 vel) { velocity = vel; }
    Rectangle GetBounds() const;
    bool IsGrounded() const { return grounded; }
    bool IsDead() const { return dead; }
    int GetHealth() const { return health; }
    int GetCoins() const { return coins; }
    PowerUpType GetActivePowerUp() const { return activePowerUp; }
    float GetPowerUpTimer() const { return powerUpTimer; }
    
    // Setters
    void SetSpawnPoint(Vector2 spawn) { spawnPoint = spawn; }
    void AddCoins(int amount) { coins += amount; }
    void AddScore(int amount) { score += amount; }
    int GetScore() const { return score; }
    
    // Combat
    void TakeDamage(int damage, Vector2 hitDirection);
    void Kill();
    void CollectPowerUp(PowerUpType type);
    
    // Actions
    void Jump();
    bool CanJump() const;
    void SetDirection(float dir) { facingRight = dir > 0; }
    bool IsFacingRight() const { return facingRight; }
    
    // State
    bool IsInvincible() const { return activePowerUp == PowerUpType::INVINCIBILITY && powerUpTimer > 0; }
    bool CanDoubleJump() const { return activePowerUp == PowerUpType::DOUBLE_JUMP && powerUpTimer > 0; }
    bool HasShield() const { return activePowerUp == PowerUpType::SHIELD && powerUpTimer > 0; }
    
private:
    void UpdatePhysics(float dt);
    void UpdateAnimation(float dt);
    void CheckTileCollisions(const class TileMap& tileMap);
    void HandleInput();
    
    Vector2 position;
    Vector2 velocity;
    Vector2 spawnPoint;
    
    int health;
    int maxHealth;
    int coins;
    int score;
    
    bool grounded;
    bool facingRight;
    bool dead;
    
    // Animation
    Texture2D spritesheet;
    int currentFrame;
    float frameTimer;
    int framesPerRow;
    int frameWidth;
    int frameHeight;
    
    std::string currentState; // "idle", "run", "jump", "hurt"
    
    // Power-ups
    PowerUpType activePowerUp;
    float powerUpTimer;
    
    // Double jump
    int jumpsLeft;
    bool canDoubleJumpAbility;
    
    // Invincibility frames
    float invincibilityTimer;
    int invincibilityFrames;
    
    // Visual effects
    float squashStretch;
    Vector2 trailPositions[10];
    int trailIndex;
};
