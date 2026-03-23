#pragma once

#include "Common.h"
#include "Animation.h"

enum class EnemyType {
    Slime,
    Snail,
    Bat,
    Fish,
    Bee
};

enum class EnemyState {
    Idle,
    Walking,
    Flying,
    Swimming,
    Dead,
    Hurt
};

class Enemy {
public:
    Enemy();
    Enemy(EnemyType type, float x, float y);
    
    void loadAnimations();
    void update(float dt);
    void render();
    
    // Действия
    void takeDamage(int amount);
    void kill();
    
    // Геттеры
    const AABB& getBounds() const { return m_bounds; }
    glm::vec2 getPosition() const { return glm::vec2(m_bounds.x, m_bounds.y); }
    glm::vec2 getVelocity() const { return m_velocity; }
    EnemyType getType() const { return m_type; }
    EnemyState getState() const { return m_state; }
    int getHealth() const { return m_health; }
    int getScoreValue() const { return m_scoreValue; }
    bool isAlive() const { return m_state != EnemyState::Dead; }
    bool isFacingRight() const { return m_facingRight; }
    
    // Сеттеры
    void setPosition(float x, float y);
    void setPatrolRange(float start, float end);
    
    // Коллизии
    void resolveCollision(const AABB& other);
    
private:
    EnemyType m_type;
    EnemyState m_state;
    AABB m_bounds;
    glm::vec2 m_velocity;
    
    int m_health;
    int m_maxHealth;
    int m_scoreValue;
    
    bool m_facingRight;
    float m_moveSpeed;
    float m_patrolStart;
    float m_patrolEnd;
    float m_deathTimer;
    
    Animation* m_currentAnim;
    
    void updateAnimation();
    void applyAI(float dt);
};
