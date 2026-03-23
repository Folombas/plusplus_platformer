#pragma once

#include "Common.h"
#include "Animation.h"
#include <string>

class Player {
public:
    Player();
    
    void loadAnimations();
    void update(float dt);
    void render();
    
    // Управление
    void moveLeft();
    void moveRight();
    void stopHorizontal();
    void jump();
    void setCrouching(bool crouching);
    
    // Состояние
    void takeDamage(int amount);
    void heal(int amount);
    void kill();
    void respawn(const glm::vec2& position);
    
    // Геттеры
    const AABB& getBounds() const { return m_bounds; }
    glm::vec2 getPosition() const { return glm::vec2(m_bounds.x, m_bounds.y); }
    glm::vec2 getVelocity() const { return m_velocity; }
    PlayerState getState() const { return m_state; }
    int getHealth() const { return m_health; }
    int getMaxHealth() const { return m_maxHealth; }
    int getCoins() const { return m_coins; }
    int getScore() const { return m_score; }
    bool isOnGround() const { return m_onGround; }
    bool isFacingRight() const { return m_facingRight; }
    
    // Сеттеры
    void setPosition(float x, float y);
    void setVelocity(float vx, float vy);
    void addCoins(int amount);
    void addScore(int amount);
    
    // Коллизии
    void resolveCollision(const AABB& other, bool isPlatform = false);
    
private:
    AABB m_bounds;
    glm::vec2 m_velocity;
    PlayerState m_state;
    
    int m_health;
    int m_maxHealth;
    int m_coins;
    int m_score;
    
    bool m_onGround;
    bool m_facingRight;
    bool m_crouching;
    float m_invincibilityTimer;
    
    Animation* m_currentAnim;
    
    void updateAnimation();
    void applyPhysics(float dt);
};
