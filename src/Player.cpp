#include "Player.h"
#include "Texture.h"
#include "Renderer.h"
#include "Audio.h"
#include <iostream>

Player::Player()
    : m_bounds{0, 0, 40, 60}
    , m_velocity(0, 0)
    , m_state(PlayerState::Idle)
    , m_health(3)
    , m_maxHealth(3)
    , m_coins(0)
    , m_score(0)
    , m_onGround(false)
    , m_facingRight(true)
    , m_crouching(false)
    , m_invincibilityTimer(0.0f)
    , m_currentAnim(nullptr)
{}

void Player::loadAnimations() {
    auto& texMgr = TextureManager::getInstance();
    auto& animMgr = AnimationManager::getInstance();
    
    // Загружаем текстуры для ходьбы (p2 - красный персонаж)
    std::vector<int> walkTextures;
    for (int i = 1; i <= 11; i++) {
        char path[256];
        snprintf(path, sizeof(path), "assets/Base pack/Player/p2_walk/PNG/p2_walk%02d.png", i);
        int texId = texMgr.loadTexture("p2_walk" + std::to_string(i), path);
        walkTextures.push_back(texId);
    }
    
    // Анимация ходьбы
    Animation walkAnim("walk", 12.0f);
    for (int texId : walkTextures) {
        walkAnim.addFrame(texId, 1.0f / 12.0f);
    }
    animMgr.addAnimation("walk", std::move(walkAnim));
    
    // Загружаем остальные текстуры
    int standTex = texMgr.loadTexture("p2_stand", "assets/Base pack/Player/p2_stand.png");
    int jumpTex = texMgr.loadTexture("p2_jump", "assets/Base pack/Player/p2_jump.png");
    int hurtTex = texMgr.loadTexture("p2_hurt", "assets/Base pack/Player/p2_hurt.png");
    int duckTex = texMgr.loadTexture("p2_duck", "assets/Base pack/Player/p2_duck.png");
    int frontTex = texMgr.loadTexture("p2_front", "assets/Base pack/Player/p2_front.png");
    
    // Анимация стойки
    Animation idleAnim("idle", 1.0f);
    idleAnim.addFrame(standTex, 0.5f);
    animMgr.addAnimation("idle", std::move(idleAnim));
    
    // Анимация прыжка
    Animation jumpAnim("jump", 1.0f);
    jumpAnim.addFrame(jumpTex, 0.1f);
    animMgr.addAnimation("jump", std::move(jumpAnim));
    
    // Анимация получения урона
    Animation hurtAnim("hurt", 5.0f);
    hurtAnim.addFrame(hurtTex, 0.2f);
    animMgr.addAnimation("hurt", std::move(hurtAnim));
    
    // Анимация приседания
    Animation duckAnim("duck", 1.0f);
    duckAnim.addFrame(duckTex, 0.1f);
    animMgr.addAnimation("duck", std::move(duckAnim));
    
    m_currentAnim = animMgr.getAnimation("idle");
}

void Player::update(float dt) {
    applyPhysics(dt);
    
    if (m_invincibilityTimer > 0) {
        m_invincibilityTimer -= dt;
    }
    
    updateAnimation();
    
    if (m_currentAnim) {
        m_currentAnim->update(dt);
    }
    
    // Границы мира
    m_bounds.x = Utils::clamp(m_bounds.x, 0.0f, 10000.0f - m_bounds.width);
}

void Player::applyPhysics(float dt) {
    // Гравитация
    m_velocity.y += GRAVITY * dt;
    
    // Ограничение скорости падения
    m_velocity.y = Utils::clamp(m_velocity.y, -1000.0f, 1000.0f);
    
    // Применяем скорость
    m_bounds.x += m_velocity.x * dt;
    m_bounds.y += m_velocity.y * dt;
    
    // Проверка на землю (базовая)
    if (m_bounds.y + m_bounds.height >= 600) {
        m_bounds.y = 600 - m_bounds.height;
        m_velocity.y = 0;
        m_onGround = true;
    }
}

void Player::updateAnimation() {
    auto& animMgr = AnimationManager::getInstance();
    
    if (m_state == PlayerState::Hurt) {
        m_currentAnim = animMgr.getAnimation("hurt");
        if (m_currentAnim->isFinished()) {
            m_state = PlayerState::Idle;
        }
    } else if (m_state == PlayerState::Dead) {
        return;
    } else if (!m_onGround) {
        m_currentAnim = animMgr.getAnimation("jump");
    } else if (m_crouching) {
        m_currentAnim = animMgr.getAnimation("duck");
    } else if (std::abs(m_velocity.x) > 0.1f) {
        m_currentAnim = animMgr.getAnimation("walk");
    } else {
        m_currentAnim = animMgr.getAnimation("idle");
    }
    
    if (m_currentAnim && !m_currentAnim->isPlaying()) {
        m_currentAnim->play();
    }
}

void Player::render() {
    if (m_state == PlayerState::Dead) return;
    
    // Мигание при неуязвимости
    if (m_invincibilityTimer > 0 && static_cast<int>(m_invincibilityTimer * 20) % 2 == 0) {
        return;
    }
    
    float x = m_bounds.x;
    float y = m_bounds.y;
    float w = m_bounds.width;
    float h = m_bounds.height;
    
    if (m_crouching) {
        h = m_bounds.height / 2.0f;
        y = m_bounds.y + m_bounds.height / 2.0f;
    }
    
    // Отражение по горизонтали
    glm::vec4 color = m_facingRight ? glm::vec4(1.0f) : glm::vec4(-1.0f, 1.0f, 1.0f, 1.0f);
    
    if (m_currentAnim) {
        int texId = m_currentAnim->getCurrentFrameTexture();
        int srcX, srcY, srcW, srcH;
        m_currentAnim->getCurrentFrameRect(srcX, srcY, srcW, srcH);
        
        if (srcW > 0 && srcH > 0) {
            Renderer::getInstance().drawSpriteRect(texId, x, y, w, h, srcX, srcY, srcW, srcH, 0.0f, color);
        } else {
            Renderer::getInstance().drawSprite(texId, x, y, w, h, 0.0f, color);
        }
    }
}

void Player::moveLeft() {
    if (m_state == PlayerState::Dead || m_state == PlayerState::Hurt) return;
    m_velocity.x = -PLAYER_SPEED;
    m_facingRight = false;
    if (m_state == PlayerState::Idle) m_state = PlayerState::Running;
}

void Player::moveRight() {
    if (m_state == PlayerState::Dead || m_state == PlayerState::Hurt) return;
    m_velocity.x = PLAYER_SPEED;
    m_facingRight = true;
    if (m_state == PlayerState::Idle) m_state = PlayerState::Running;
}

void Player::stopHorizontal() {
    m_velocity.x = 0;
    if (m_state == PlayerState::Running) m_state = PlayerState::Idle;
}

void Player::jump() {
    if (m_state == PlayerState::Dead || m_state == PlayerState::Hurt) return;
    if (m_onGround) {
        m_velocity.y = -PLAYER_JUMP_FORCE;
        m_onGround = false;
        m_state = PlayerState::Jumping;
        AudioManager::getInstance().playSound("jump");
    }
}

void Player::setCrouching(bool crouching) {
    if (m_state == PlayerState::Dead || m_state == PlayerState::Hurt) return;
    m_crouching = crouching && m_onGround;
}

void Player::takeDamage(int amount) {
    if (m_invincibilityTimer > 0 || m_state == PlayerState::Dead) return;
    
    m_health -= amount;
    AudioManager::getInstance().playSound("hit");
    
    if (m_health <= 0) {
        kill();
    } else {
        m_state = PlayerState::Hurt;
        m_invincibilityTimer = 2.0f;
        auto anim = AnimationManager::getInstance().getAnimation("hurt");
        if (anim) anim->reset();
    }
}

void Player::heal(int amount) {
    m_health = std::min(m_health + amount, m_maxHealth);
}

void Player::kill() {
    m_state = PlayerState::Dead;
    m_velocity.y = -PLAYER_JUMP_FORCE / 2.0f;
}

void Player::respawn(const glm::vec2& position) {
    m_bounds.x = position.x;
    m_bounds.y = position.y;
    m_velocity = glm::vec2(0, 0);
    m_health = m_maxHealth;
    m_state = PlayerState::Idle;
    m_invincibilityTimer = 0.0f;
    m_crouching = false;
}

void Player::setPosition(float x, float y) {
    m_bounds.x = x;
    m_bounds.y = y;
}

void Player::setVelocity(float vx, float vy) {
    m_velocity.x = vx;
    m_velocity.y = vy;
}

void Player::addCoins(int amount) {
    m_coins += amount;
    m_score += amount * 100;
}

void Player::addScore(int amount) {
    m_score += amount;
}

void Player::resolveCollision(const AABB& other, bool isPlatform) {
    // Простая AABB коллизия
    if (!m_bounds.intersects(other)) return;
    
    // Вычисляем перекрытия
    float overlapLeft = (m_bounds.x + m_bounds.width) - other.x;
    float overlapRight = (other.x + other.width) - m_bounds.x;
    float overlapTop = (m_bounds.y + m_bounds.height) - other.y;
    float overlapBottom = (other.y + other.height) - m_bounds.y;
    
    // Находим минимальное перекрытие
    float minOverlap = std::min({overlapLeft, overlapRight, overlapTop, overlapBottom});
    
    if (isPlatform) {
        // Платформы - только сверху
        if (m_velocity.y > 0 && overlapTop < overlapBottom && 
            m_bounds.y + m_bounds.height - m_velocity.y * 0.016f <= other.y) {
            m_bounds.y = other.y - m_bounds.height;
            m_velocity.y = 0;
            m_onGround = true;
            if (m_state == PlayerState::Falling) m_state = PlayerState::Idle;
        }
    } else {
        // Твердые блоки
        if (minOverlap == overlapTop) {
            m_bounds.y = other.y - m_bounds.height;
            m_velocity.y = 0;
            m_onGround = true;
            if (m_state == PlayerState::Falling) m_state = PlayerState::Idle;
        } else if (minOverlap == overlapBottom) {
            m_bounds.y = other.y + other.height;
            m_velocity.y = 0;
        } else if (minOverlap == overlapLeft) {
            m_bounds.x = other.x - m_bounds.width;
            m_velocity.x = 0;
        } else if (minOverlap == overlapRight) {
            m_bounds.x = other.x + other.width;
            m_velocity.x = 0;
        }
    }
}
