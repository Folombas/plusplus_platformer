#include "Enemy.h"
#include "TileMap.h"
#include "Player.h"
#include "AudioManager.h"

#ifdef _WIN32
#undef CloseWindow
#undef ShowCursor
#endif

Enemy::Enemy()
    : position{0, 0}
    , velocity{0, 0}
    , patrolLeft{0, 0}
    , patrolRight{0, 0}
    , type(EnemyType::GROUND)
    , state(EnemyState::PATROL)
    , active(false)
    , health(1)
    , maxHealth(1)
    , damage(1)
    , scoreValue(100)
    , detectionRange(200)
    , attackRange(40)
    , attackCooldown(1.0f)
    , attackTimer(0)
    , facingRight(true)
    , moveSpeed(ENEMY_SPEED)
    , jumpForce(-400)
    , grounded(false)
    , currentFrame(0)
    , frameTimer(0)
    , frameWidth(32)
    , frameHeight(32)
    , currentState("idle")
    , hurtFlashTimer(0)
    , deathTimer(0)
    , hurtColor(WHITE)
{
}

Enemy::Enemy(Vector2 pos, EnemyType enemyType)
    : Enemy()
{
    position = pos;
    type = enemyType;
    active = true;
    
    // Configure based on type
    switch (type) {
        case EnemyType::GROUND:
            health = 1;
            maxHealth = 1;
            moveSpeed = 80;
            scoreValue = 100;
            break;
        case EnemyType::FLYING:
            health = 1;
            maxHealth = 1;
            moveSpeed = 100;
            jumpForce = 0;
            scoreValue = 150;
            break;
        case EnemyType::JUMPING:
            health = 1;
            maxHealth = 1;
            moveSpeed = 60;
            jumpForce = -500;
            scoreValue = 200;
            break;
        case EnemyType::BOSS:
            health = 10;
            maxHealth = 10;
            moveSpeed = 50;
            detectionRange = 400;
            scoreValue = 1000;
            break;
    }
    
    LoadSprites();
}

Enemy::~Enemy() {
    if (spritesheet.width != 0) {
        UnloadTexture(spritesheet);
    }
}

void Enemy::LoadSprites() {
    std::string path;
    switch (type) {
        case EnemyType::GROUND:
            path = "assets/sprites/platformer/Base pack/Enemies/slimeWalk.png";
            break;
        case EnemyType::FLYING:
            path = "assets/sprites/platformer/Base pack/Enemies/flyWalk.png";
            break;
        case EnemyType::JUMPING:
            path = "assets/sprites/platformer/Base pack/Enemies/batFly.png";
            break;
        case EnemyType::BOSS:
            path = "assets/sprites/platformer/Extra animations and enemies/big_slime.png";
            break;
    }
    
    Image img = LoadImage(path.c_str());
    if (img.data != nullptr) {
        spritesheet = LoadTextureFromImage(img);
        UnloadImage(img);
        
        frameWidth = spritesheet.width / 8;
        frameHeight = spritesheet.height;
    } else {
        // Placeholder
        Color enemyColor = MAGENTA;
        if (type == EnemyType::FLYING) enemyColor = PURPLE;
        if (type == EnemyType::BOSS) enemyColor = DARKGREEN;
        
        Image placeholder = GenImageColor(32, 32, enemyColor);
        spritesheet = LoadTextureFromImage(placeholder);
        UnloadImage(placeholder);
        
        frameWidth = 32;
        frameHeight = 32;
    }
}

void Enemy::SetPatrolPoints(Vector2 left, Vector2 right) {
    patrolLeft = left;
    patrolRight = right;
}

Rectangle Enemy::GetBounds() const {
    float size = frameWidth * 0.8f;
    if (type == EnemyType::BOSS) size *= 2;
    
    return {
        position.x - size / 2,
        position.y - size / 2,
        size,
        type == EnemyType::FLYING ? size * 0.6f : size
    };
}

void Enemy::Update(float dt, const TileMap& tileMap, const Player* player) {
    if (!active || state == EnemyState::DEAD) {
        if (state == EnemyState::DEAD) {
            deathTimer -= dt;
            if (deathTimer <= 0) {
                active = false;
            }
        }
        return;
    }
    
    UpdateAI(dt, player);
    UpdatePhysics(dt, tileMap);
    UpdateAnimation(dt);
    
    // Update hurt flash
    if (hurtFlashTimer > 0) {
        hurtFlashTimer -= dt;
    }
    
    // Update attack cooldown
    if (attackTimer > 0) {
        attackTimer -= dt;
    }
}

void Enemy::UpdateAI(float dt, const Player* player) {
    if (!player || player->IsDead()) {
        state = EnemyState::PATROL;
        return;
    }
    
    float distToPlayer = Vector2Distance(position, player->GetPosition());
    
    // State machine
    switch (state) {
        case EnemyState::PATROL:
            if (distToPlayer < detectionRange) {
                state = EnemyState::CHASE;
            } else {
                // Patrol behavior
                if (type == EnemyType::GROUND || type == EnemyType::JUMPING) {
                    if (facingRight && position.x > patrolRight.x) {
                        facingRight = false;
                    } else if (!facingRight && position.x < patrolLeft.x) {
                        facingRight = true;
                    }
                    
                    velocity.x = facingRight ? moveSpeed : -moveSpeed;
                } else if (type == EnemyType::FLYING) {
                    // Flying enemies hover in place or move slowly
                    velocity.x = facingRight ? moveSpeed * 0.5f : -moveSpeed * 0.5f;
                    
                    // Bob up and down
                    velocity.y = sin(GetTime() * 3) * 30;
                }
            }
            break;
            
        case EnemyState::CHASE:
            if (distToPlayer > detectionRange * 1.5f) {
                state = EnemyState::PATROL;
            } else {
                // Chase player
                if (player->GetPosition().x > position.x) {
                    facingRight = true;
                    velocity.x = moveSpeed;
                } else {
                    facingRight = false;
                    velocity.x = -moveSpeed;
                }
                
                // Jumping enemies try to jump at player
                if (type == EnemyType::JUMPING && grounded && attackTimer <= 0) {
                    velocity.y = jumpForce;
                    grounded = false;
                    attackTimer = 2.0f;
                }
            }
            break;
            
        case EnemyState::HURT:
            velocity.x = 0;
            if (hurtFlashTimer <= 0) {
                state = EnemyState::PATROL;
            }
            break;
            
        default:
            break;
    }
}

void Enemy::UpdatePhysics(float dt, const TileMap& tileMap) {
    if (type == EnemyType::FLYING) {
        // Flying enemies ignore gravity
        position.x += velocity.x * dt;
        position.y += velocity.y * dt;
        return;
    }
    
    // Apply gravity
    velocity.y += GRAVITY * dt;
    if (velocity.y > 800) velocity.y = 800;
    
    // Apply horizontal movement
    position.x += velocity.x * dt;
    
    // Horizontal collision
    Rectangle bounds = GetBounds();
    int startX = static_cast<int>((bounds.x - 5) / tileMap.GetTileSize());
    int endX = static_cast<int>((bounds.x + bounds.width + 5) / tileMap.GetTileSize());
    int startY = static_cast<int>((bounds.y - 5) / tileMap.GetTileSize());
    int endY = static_cast<int>((bounds.y + bounds.height + 5) / tileMap.GetTileSize());
    
    for (int y = startY; y <= endY; y++) {
        for (int x = startX; x <= endX; x++) {
            if (tileMap.IsTileSolid(x, y)) {
                Rectangle tileRect = tileMap.GetTileCollision(x, y);
                if (CheckCollisionRecs(bounds, tileRect)) {
                    if (velocity.x > 0) {
                        position.x = tileRect.x - bounds.width / 2 - 0.1f;
                    } else if (velocity.x < 0) {
                        position.x = tileRect.x + tileRect.width + bounds.width / 2 + 0.1f;
                    }
                    velocity.x = -velocity.x;  // Reverse direction
                    facingRight = !facingRight;
                }
            }
        }
    }
    
    // Apply vertical movement
    position.y += velocity.y * dt;
    bounds = GetBounds();
    
    // Vertical collision
    grounded = false;
    for (int y = startY; y <= endY; y++) {
        for (int x = startX; x <= endX; x++) {
            if (tileMap.IsTileSolid(x, y)) {
                Rectangle tileRect = tileMap.GetTileCollision(x, y);
                if (CheckCollisionRecs(bounds, tileRect)) {
                    if (velocity.y > 0) {
                        position.y = tileRect.y - bounds.height / 2 - 0.1f;
                        grounded = true;
                        velocity.y = 0;
                    } else if (velocity.y < 0) {
                        position.y = tileRect.y + tileRect.height + bounds.height / 2 + 0.1f;
                        velocity.y = 0;
                    }
                }
            }
        }
    }
}

void Enemy::UpdateAnimation(float dt) {
    float frameSpeed = 0.15f;
    
    frameTimer += dt;
    if (frameTimer >= frameSpeed) {
        frameTimer -= frameSpeed;
        currentFrame = (currentFrame + 1) % 4;
    }
}

void Enemy::Draw(const GameCamera2D& camera) {
    if (!active) return;
    
    Color tint = WHITE;
    
    if (state == EnemyState::DEAD) {
        tint = TintAlpha(WHITE, deathTimer / 0.5f);
    } else if (hurtFlashTimer > 0) {
        tint = hurtColor;
    }
    
    float scale = 1.0f;
    if (type == EnemyType::BOSS) scale = 2.0f;
    
    if (state == EnemyState::DEAD) {
        // Squashed death animation
        DrawTexturePro(
            spritesheet,
            GetSpriteSource(spritesheet, frameWidth, frameHeight, currentFrame),
            {
                position.x - frameWidth * 0.4f * scale,
                position.y + frameHeight * 0.3f * scale,
                frameWidth * 0.8f * scale,
                frameHeight * 0.2f * scale
            },
            {frameWidth * 0.4f, frameHeight * 0.1f},
            facingRight ? 0 : 180,
            tint
        );
    } else {
        DrawTexturePro(
            spritesheet,
            GetSpriteSource(spritesheet, frameWidth, frameHeight, currentFrame),
            {
                position.x - frameWidth * 0.4f * scale,
                position.y - frameHeight * 0.4f * scale,
                frameWidth * 0.8f * scale,
                frameHeight * 0.8f * scale
            },
            {frameWidth * 0.4f, frameHeight * 0.4f},
            facingRight ? 0 : 180,
            tint
        );
    }
    
    // Draw health bar for boss
    if (type == EnemyType::BOSS && health < maxHealth) {
        float barWidth = 100 * scale;
        float barHeight = 8;
        float healthPercent = static_cast<float>(health) / maxHealth;
        
        DrawRectangle(
            position.x - barWidth / 2,
            position.y - frameHeight * scale - 20,
            barWidth,
            barHeight,
            BLACK
        );
        DrawRectangle(
            position.x - barWidth / 2,
            position.y - frameHeight * scale - 20,
            barWidth * healthPercent,
            barHeight,
            RED
        );
    }
}

void Enemy::TakeDamage(int damage, Vector2 hitDirection) {
    if (state == EnemyState::DEAD || state == EnemyState::HURT) return;
    
    health -= damage;
    hurtFlashTimer = 0.2f;
    hurtColor = RED;
    
    // Knockback
    velocity.x = hitDirection.x * 300;
    velocity.y = hitDirection.y * 300 - 100;
    
    if (health <= 0) {
        Kill();
    }
}

void Enemy::Kill() {
    if (state == EnemyState::DEAD) return;
    
    state = EnemyState::DEAD;
    deathTimer = 0.5f;
    velocity = {0, 0};
    
    AudioManager::Instance().PlaySound(SoundType::ENEMY_DEATH);
}
