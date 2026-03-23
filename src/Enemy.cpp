#include "Enemy.h"
#include "Texture.h"
#include "Renderer.h"

Enemy::Enemy()
    : m_type(EnemyType::Slime)
    , m_state(EnemyState::Idle)
    , m_bounds{0, 0, 40, 30}
    , m_velocity(0, 0)
    , m_health(1)
    , m_maxHealth(1)
    , m_scoreValue(100)
    , m_facingRight(true)
    , m_moveSpeed(50.0f)
    , m_patrolStart(0)
    , m_patrolEnd(200)
    , m_deathTimer(0.0f)
    , m_currentAnim(nullptr)
{}

Enemy::Enemy(EnemyType type, float x, float y)
    : Enemy()
{
    m_type = type;
    m_bounds.x = x;
    m_bounds.y = y;
    
    switch (type) {
        case EnemyType::Slime:
            m_bounds.width = 40;
            m_bounds.height = 30;
            m_moveSpeed = 40.0f;
            m_scoreValue = 100;
            break;
        case EnemyType::Snail:
            m_bounds.width = 50;
            m_bounds.height = 35;
            m_moveSpeed = 25.0f;
            m_scoreValue = 150;
            break;
        case EnemyType::Bat:
            m_bounds.width = 35;
            m_bounds.height = 25;
            m_moveSpeed = 80.0f;
            m_scoreValue = 200;
            break;
        case EnemyType::Fish:
            m_bounds.width = 40;
            m_bounds.height = 25;
            m_moveSpeed = 60.0f;
            m_scoreValue = 150;
            break;
        case EnemyType::Bee:
            m_bounds.width = 30;
            m_bounds.height = 30;
            m_moveSpeed = 70.0f;
            m_scoreValue = 200;
            break;
    }
    
    m_patrolEnd = x + 150.0f;
    m_patrolStart = x - 150.0f;
}

void Enemy::loadAnimations() {
    auto& texMgr = TextureManager::getInstance();
    auto& animMgr = AnimationManager::getInstance();
    
    std::string prefix;
    std::string filePrefix;
    switch (m_type) {
        case EnemyType::Slime: prefix = "slime"; filePrefix = "slime"; break;
        case EnemyType::Snail: prefix = "snail"; filePrefix = "snail"; break;
        case EnemyType::Bat: prefix = "bat"; filePrefix = "fly"; break; // Используем fly текстуры для летучих врагов
        case EnemyType::Fish: prefix = "fish"; filePrefix = "fish"; break;
        case EnemyType::Bee: prefix = "bee"; filePrefix = "fly"; break;
    }
    
    // Загружаем текстуры из Base pack/Enemies
    std::string basePath = "assets/Base pack/Enemies/";
    
    // Ходьба/полет
    std::string walkPath1 = basePath + filePrefix + "Walk1.png";
    std::string walkPath2 = basePath + filePrefix + "Walk2.png";
    
    int walk1Tex = texMgr.loadTexture(prefix + "_walk1", walkPath1);
    int walk2Tex = texMgr.loadTexture(prefix + "_walk2", walkPath2);
    
    if (walk1Tex >= 0 && walk2Tex >= 0) {
        Animation walkAnim(prefix + "_walk", 8.0f);
        walkAnim.addFrame(walk1Tex, 0.125f);
        walkAnim.addFrame(walk2Tex, 0.125f);
        animMgr.addAnimation(prefix + "_walk", std::move(walkAnim));
    }
    
    // Смерть
    std::string deadPath = basePath + filePrefix + "Dead.png";
    int deadTex = texMgr.loadTexture(prefix + "_dead", deadPath);
    if (deadTex >= 0) {
        Animation deadAnim(prefix + "_dead", 1.0f);
        deadAnim.addFrame(deadTex, 0.5f);
        animMgr.addAnimation(prefix + "_dead", std::move(deadAnim));
    }
    
    // Для летучих мышей и пчел - отдельная анимация полета
    if (m_type == EnemyType::Bat || m_type == EnemyType::Bee) {
        std::string flyPath1 = basePath + "flyFly1.png";
        std::string flyPath2 = basePath + "flyFly2.png";
        
        int fly1Tex = texMgr.loadTexture(prefix + "_fly1", flyPath1);
        int fly2Tex = texMgr.loadTexture(prefix + "_fly2", flyPath2);
        
        if (fly1Tex >= 0 && fly2Tex >= 0) {
            Animation flyAnim(prefix + "_fly", 10.0f);
            flyAnim.addFrame(fly1Tex, 0.1f);
            flyAnim.addFrame(fly2Tex, 0.1f);
            animMgr.addAnimation(prefix + "_fly", std::move(flyAnim));
        }
    }
    
    // Для рыб - плавание
    if (m_type == EnemyType::Fish) {
        std::string swim1Path = basePath + "fishSwim1.png";
        std::string swim2Path = basePath + "fishSwim2.png";
        
        int swim1Tex = texMgr.loadTexture("fish_swim1", swim1Path);
        int swim2Tex = texMgr.loadTexture("fish_swim2", swim2Path);
        
        if (swim1Tex >= 0 && swim2Tex >= 0) {
            Animation swimAnim("fish_swim", 8.0f);
            swimAnim.addFrame(swim1Tex, 0.125f);
            swimAnim.addFrame(swim2Tex, 0.125f);
            animMgr.addAnimation("fish_swim", std::move(swimAnim));
        }
    }
    
    m_currentAnim = animMgr.getAnimation(prefix + "_walk");
    if (!m_currentAnim && m_type == EnemyType::Fish) {
        m_currentAnim = animMgr.getAnimation("fish_swim");
    }
    if (!m_currentAnim && (m_type == EnemyType::Bat || m_type == EnemyType::Bee)) {
        m_currentAnim = animMgr.getAnimation(prefix + "_fly");
    }
}

void Enemy::update(float dt) {
    if (m_state == EnemyState::Dead) {
        m_deathTimer -= dt;
        return;
    }
    
    if (m_state == EnemyState::Hurt) {
        return;
    }
    
    applyAI(dt);
    applyPhysics(dt);
    updateAnimation();
    
    if (m_currentAnim) {
        m_currentAnim->update(dt);
    }
}

void Enemy::applyPhysics(float dt) {
    if (m_type == EnemyType::Bat || m_type == EnemyType::Bee) {
        // Летающие враги не подвержены гравитации
        m_bounds.x += m_velocity.x * dt;
        m_bounds.y += m_velocity.y * dt;
    } else if (m_type == EnemyType::Fish) {
        // Рыбы плавают
        m_bounds.x += m_velocity.x * dt;
        m_bounds.y += m_velocity.y * dt;
    } else {
        // Наземные враги
        m_velocity.y += GRAVITY * dt;
        m_bounds.x += m_velocity.x * dt;
        m_bounds.y += m_velocity.y * dt;
        
        // Простая проверка земли
        if (m_bounds.y + m_bounds.height >= 500) {
            m_bounds.y = 500 - m_bounds.height;
            m_velocity.y = 0;
        }
    }
}

void Enemy::applyAI(float dt) {
    switch (m_type) {
        case EnemyType::Slime:
        case EnemyType::Snail:
            // Патрулирование
            if (m_facingRight) {
                m_velocity.x = m_moveSpeed;
                if (m_bounds.x > m_patrolEnd) {
                    m_facingRight = false;
                }
            } else {
                m_velocity.x = -m_moveSpeed;
                if (m_bounds.x < m_patrolStart) {
                    m_facingRight = true;
                }
            }
            break;
            
        case EnemyType::Bat:
        case EnemyType::Bee:
            // Летающее патрулирование с синусоидальным движением
            if (m_facingRight) {
                m_velocity.x = m_moveSpeed;
                m_velocity.y = std::sin(glfwGetTime() * 3.0f) * 30.0f;
                if (m_bounds.x > m_patrolEnd) {
                    m_facingRight = false;
                }
            } else {
                m_velocity.x = -m_moveSpeed;
                m_velocity.y = std::sin(glfwGetTime() * 3.0f) * 30.0f;
                if (m_bounds.x < m_patrolStart) {
                    m_facingRight = true;
                }
            }
            break;
            
        case EnemyType::Fish:
            // Плавание туда-сюда
            if (m_facingRight) {
                m_velocity.x = m_moveSpeed;
                if (m_bounds.x > m_patrolEnd) {
                    m_facingRight = false;
                }
            } else {
                m_velocity.x = -m_moveSpeed;
                if (m_bounds.x < m_patrolStart) {
                    m_facingRight = true;
                }
            }
            break;
    }
}

void Enemy::updateAnimation() {
    auto& animMgr = AnimationManager::getInstance();
    std::string prefix;
    
    switch (m_type) {
        case EnemyType::Slime: prefix = "slime"; break;
        case EnemyType::Snail: prefix = "snail"; break;
        case EnemyType::Bat: prefix = "bat"; break;
        case EnemyType::Fish: prefix = "fish"; break;
        case EnemyType::Bee: prefix = "bee"; break;
    }
    
    if (m_state == EnemyState::Dead) {
        m_currentAnim = animMgr.getAnimation(prefix + "_dead");
    } else if (m_type == EnemyType::Bat || m_type == EnemyType::Bee) {
        m_currentAnim = animMgr.getAnimation(prefix + "_fly");
    } else if (m_type == EnemyType::Fish) {
        m_currentAnim = animMgr.getAnimation("fish_swim");
    } else {
        m_currentAnim = animMgr.getAnimation(prefix + "_walk");
    }
    
    if (m_currentAnim && !m_currentAnim->isPlaying()) {
        m_currentAnim->play();
    }
}

void Enemy::render() {
    if (m_state == EnemyState::Dead && m_deathTimer <= 0) return;
    
    float x = m_bounds.x;
    float y = m_bounds.y;
    float w = m_bounds.width;
    float h = m_bounds.height;
    
    glm::vec4 color = m_facingRight ? glm::vec4(1.0f) : glm::vec4(-1.0f, 1.0f, 1.0f, 1.0f);
    
    if (m_state == EnemyState::Dead) {
        color = glm::vec4(1.0f, 1.0f, 1.0f, 0.5f);
    }
    
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

void Enemy::takeDamage(int amount) {
    if (m_state == EnemyState::Dead) return;
    
    m_health -= amount;
    if (m_health <= 0) {
        kill();
    }
}

void Enemy::kill() {
    m_state = EnemyState::Dead;
    m_deathTimer = 1.0f;
    m_velocity = glm::vec2(0, 0);
}

void Enemy::setPosition(float x, float y) {
    m_bounds.x = x;
    m_bounds.y = y;
}

void Enemy::setPatrolRange(float start, float end) {
    m_patrolStart = start;
    m_patrolEnd = end;
}

void Enemy::resolveCollision(const AABB& other) {
    if (!m_bounds.intersects(other)) return;
    
    float overlapLeft = (m_bounds.x + m_bounds.width) - other.x;
    float overlapRight = (other.x + other.width) - m_bounds.x;
    float overlapTop = (m_bounds.y + m_bounds.height) - other.y;
    float overlapBottom = (other.y + other.height) - m_bounds.y;
    
    float minOverlap = std::min({overlapLeft, overlapRight, overlapTop, overlapBottom});
    
    if (minOverlap == overlapTop) {
        m_bounds.y = other.y - m_bounds.height;
        m_velocity.y = 0;
    } else if (minOverlap == overlapBottom) {
        m_bounds.y = other.y + other.height;
        m_velocity.y = 0;
    } else if (minOverlap == overlapLeft) {
        m_bounds.x = other.x - m_bounds.width;
        m_velocity.x = 0;
        m_facingRight = true;
    } else if (minOverlap == overlapRight) {
        m_bounds.x = other.x + other.width;
        m_velocity.x = 0;
        m_facingRight = false;
    }
}
