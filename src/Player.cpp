#include "Player.h"
#include "TileMap.h"
#include "Camera2D.h"
#include "AudioManager.h"
#include <cmath>

#ifdef _WIN32
#undef CloseWindow
#undef ShowCursor
#endif

Player::Player() 
    : position{0, 0}
    , velocity{0, 0}
    , spawnPoint{0, 0}
    , health(3)
    , maxHealth(3)
    , coins(0)
    , score(0)
    , grounded(false)
    , facingRight(true)
    , dead(false)
    , currentFrame(0)
    , frameTimer(0)
    , framesPerRow(8)
    , frameWidth(32)
    , frameHeight(32)
    , currentState("idle")
    , activePowerUp(PowerUpType::NONE)
    , powerUpTimer(0)
    , jumpsLeft(1)
    , canDoubleJumpAbility(false)
    , invincibilityTimer(0)
    , invincibilityFrames(0)
    , squashStretch(1.0f)
    , trailIndex(0)
{
    for (int i = 0; i < 10; i++) {
        trailPositions[i] = {0, 0};
    }
    
    // Load player texture
    std::string path = "assets/sprites/platformer/Base pack/Player/p1_spritesheet.png";
    Image img = {0};
    
    if (FileExists(path.c_str())) {
        img = LoadImage(path.c_str());
    }
    
    // Try alternative paths
    if (img.data == nullptr) {
        path = "assets/sprites/platformer/Base pack/Player/p1_stand.png";
        if (FileExists(path.c_str())) {
            img = LoadImage(path.c_str());
        }
    }
    if (img.data != nullptr) {
        spritesheet = LoadTextureFromImage(img);
        UnloadImage(img);
        
        // Set animation frame size based on texture
        frameWidth = spritesheet.width / 9;  // 8 frames + stand
        frameHeight = spritesheet.height;
    } else {
        // Create placeholder texture
        Image placeholder = GenImageColor(32, 32, RED);
        spritesheet = LoadTextureFromImage(placeholder);
        UnloadImage(placeholder);
    }
}

Player::~Player() {
    if (spritesheet.width != 0) {
        UnloadTexture(spritesheet);
    }
}

void Player::Reset(Vector2 startPosition) {
    position = startPosition;
    velocity = {0, 0};
    spawnPoint = startPosition;
    health = maxHealth;
    coins = 0;
    score = 0;
    dead = false;
    grounded = false;
    facingRight = true;
    activePowerUp = PowerUpType::NONE;
    powerUpTimer = 0;
    jumpsLeft = 1;
    invincibilityTimer = 0;
    invincibilityFrames = 0;
    currentState = "idle";
    currentFrame = 0;
}

Rectangle Player::GetBounds() const {
    float width = frameWidth * 0.7f;
    float height = frameHeight * 0.9f;
    return {
        position.x - width / 2,
        position.y - height / 2,
        width,
        height
    };
}

void Player::Update(float dt, const TileMap& tileMap) {
    if (dead) return;
    
    // Update power-up timer
    if (activePowerUp != PowerUpType::NONE && powerUpTimer > 0) {
        powerUpTimer -= dt;
        if (powerUpTimer <= 0) {
            activePowerUp = PowerUpType::NONE;
        }
    }
    
    // Update invincibility
    if (invincibilityTimer > 0) {
        invincibilityTimer -= dt;
    }
    if (invincibilityFrames > 0) {
        invincibilityFrames--;
    }
    
    HandleInput();
    UpdatePhysics(dt);
    CheckTileCollisions(tileMap);
    UpdateAnimation(dt);
    
    // Update trail
    trailPositions[trailIndex] = position;
    trailIndex = (trailIndex + 1) % 10;
    
    // Squash and stretch recovery
    squashStretch = Lerp(squashStretch, 1.0f, dt * 10.0f);
}

void Player::HandleInput() {
    // Movement
    float moveInput = 0;
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
        moveInput = -1;
        facingRight = false;
    }
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
        moveInput = 1;
        facingRight = true;
    }
    
    // Apply movement
    float speed = PLAYER_SPEED;
    if (activePowerUp == PowerUpType::SPEED_BOOST && powerUpTimer > 0) {
        speed *= 1.5f;
    }
    
    velocity.x = moveInput * speed;
    
    // Jump
    if ((IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_SPACE)) && CanJump()) {
        Jump();
    }
}

bool Player::CanJump() const {
    if (grounded) return true;
    if (activePowerUp == PowerUpType::DOUBLE_JUMP && powerUpTimer > 0 && jumpsLeft > 0) {
        return true;
    }
    return false;
}

void Player::Jump() {
    velocity.y = PLAYER_JUMP;
    grounded = false;
    
    if (!IsGrounded()) {
        jumpsLeft--;  // Use double jump
    }
    
    // Squash effect
    squashStretch = 1.3f;
    
    // Play sound
    AudioManager::Instance().PlaySound(SoundType::JUMP);
    
    // Emit dust particles
    // (particle system will be called from Game)
}

void Player::UpdatePhysics(float dt) {
    // Apply gravity
    velocity.y += GRAVITY * dt;
    
    // Terminal velocity
    if (velocity.y > 1000) velocity.y = 1000;
    
    // Apply velocity
    position.x += velocity.x * dt;
    position.y += velocity.y * dt;
}

void Player::CheckTileCollisions(const TileMap& tileMap) {
    Rectangle bounds = GetBounds();
    
    // Calculate tile range to check
    int startX = static_cast<int>((bounds.x - 10) / tileMap.GetTileSize());
    int endX = static_cast<int>((bounds.x + bounds.width + 10) / tileMap.GetTileSize());
    int startY = static_cast<int>((bounds.y - 10) / tileMap.GetTileSize());
    int endY = static_cast<int>((bounds.y + bounds.height + 10) / tileMap.GetTileSize());
    
    bool wasGrounded = grounded;
    grounded = false;
    
    // Check horizontal collisions
    for (int y = startY; y <= endY; y++) {
        for (int x = startX; x <= endX; x++) {
            if (tileMap.IsTileSolid(x, y)) {
                Rectangle tileRect = tileMap.GetTileCollision(x, y);
                if (CheckCollisionRecs(bounds, tileRect)) {
                    // Horizontal collision
                    if (velocity.x > 0) {
                        position.x = tileRect.x - bounds.width / 2 - 0.1f;
                    } else if (velocity.x < 0) {
                        position.x = tileRect.x + tileRect.width + bounds.width / 2 + 0.1f;
                    }
                    velocity.x = 0;
                }
            }
        }
    }
    
    // Update bounds after horizontal collision
    bounds = GetBounds();
    startX = static_cast<int>((bounds.x - 10) / tileMap.GetTileSize());
    endX = static_cast<int>((bounds.x + bounds.width + 10) / tileMap.GetTileSize());
    startY = static_cast<int>((bounds.y - 10) / tileMap.GetTileSize());
    endY = static_cast<int>((bounds.y + bounds.height + 10) / tileMap.GetTileSize());
    
    // Check vertical collisions
    for (int y = startY; y <= endY; y++) {
        for (int x = startX; x <= endX; x++) {
            if (tileMap.IsTileSolid(x, y)) {
                Rectangle tileRect = tileMap.GetTileCollision(x, y);
                if (CheckCollisionRecs(bounds, tileRect)) {
                    // Vertical collision
                    if (velocity.y > 0) {
                        // Landing
                        position.y = tileRect.y - bounds.height / 2 - 0.1f;
                        grounded = true;
                        jumpsLeft = 1;
                        
                        // Landing effect
                        if (!wasGrounded && velocity.y > 300) {
                            squashStretch = 0.7f;
                            // Emit land dust
                        }
                        
                        velocity.y = 0;
                    } else if (velocity.y < 0) {
                        // Hitting ceiling
                        position.y = tileRect.y + tileRect.height + bounds.height / 2 + 0.1f;
                        velocity.y = 0;
                    }
                }
            }
        }
    }
    
    // Check for dangerous tiles
    for (int y = startY; y <= endY; y++) {
        for (int x = startX; x <= endX; x++) {
            if (tileMap.IsTileDangerous(x, y)) {
                Rectangle tileRect = tileMap.GetTileCollision(x, y);
                if (CheckCollisionRecs(bounds, tileRect)) {
                    TakeDamage(1, {0, -1});
                }
            }
        }
    }
    
    // Check if fell off the map
    if (position.y > tileMap.GetHeight() * tileMap.GetTileSize() + 200) {
        Kill();
    }
}

void Player::UpdateAnimation(float dt) {
    // Determine animation state
    std::string newState;
    
    if (!grounded) {
        newState = "jump";
    } else if (std::abs(velocity.x) > 10) {
        newState = "run";
    } else {
        newState = "idle";
    }
    
    if (invincibilityFrames > 0 && invincibilityFrames % 4 < 2) {
        // Flash when hurt
        return;
    }
    
    if (newState != currentState) {
        currentState = newState;
        currentFrame = 0;
        frameTimer = 0;
    }
    
    // Animation speeds
    float frameSpeed = 0.1f;
    if (currentState == "run") frameSpeed = 0.08f;
    if (currentState == "jump") frameSpeed = 0.15f;
    
    frameTimer += dt;
    if (frameTimer >= frameSpeed) {
        frameTimer -= frameSpeed;
        
        if (currentState == "idle") {
            currentFrame = 0;  // Stand frame
        } else if (currentState == "run") {
            currentFrame = 1 + (currentFrame + 1) % 6;  // Walk cycle
        } else if (currentState == "jump") {
            currentFrame = 7;  // Jump frame
        }
    }
}

void Player::Draw(const GameCamera2D& camera) {
    if (dead) return;
    
    // Blink when invincible
    if (invincibilityFrames > 0 && invincibilityFrames % 3 < 2) {
        return;
    }
    
    // Draw trail effect when speed boosted
    if (activePowerUp == PowerUpType::SPEED_BOOST && powerUpTimer > 0) {
        for (int i = 0; i < 5; i++) {
            int idx = (trailIndex - i - 1 + 10) % 10;
            Color trailColor = TintAlpha(WHITE, 0.3f - i * 0.05f);
            DrawTexturePro(
                spritesheet,
                GetSpriteSource(spritesheet, frameWidth, frameHeight, currentFrame),
                {
                    trailPositions[idx].x - frameWidth * 0.35f * squashStretch,
                    trailPositions[idx].y - frameHeight * 0.45f * squashStretch,
                    frameWidth * 0.7f * squashStretch,
                    frameHeight * 0.9f * squashStretch
                },
                {frameWidth * 0.35f, frameHeight * 0.45f},
                facingRight ? 0 : 180,
                trailColor
            );
        }
    }
    
    // Draw player
    Color tint = WHITE;
    if (activePowerUp == PowerUpType::INVINCIBILITY && powerUpTimer > 0) {
        tint = GOLD;
    } else if (activePowerUp == PowerUpType::SHIELD && powerUpTimer > 0) {
        tint = SKYBLUE;
    }
    
    DrawTexturePro(
        spritesheet,
        GetSpriteSource(spritesheet, frameWidth, frameHeight, currentFrame),
        {
            position.x - frameWidth * 0.35f * squashStretch,
            position.y - frameHeight * 0.45f * squashStretch,
            frameWidth * 0.7f * squashStretch,
            frameHeight * 0.9f * squashStretch
        },
        {frameWidth * 0.35f, frameHeight * 0.45f},
        facingRight ? 0 : 180,
        tint
    );
    
    // Draw shield effect
    if (activePowerUp == PowerUpType::SHIELD && powerUpTimer > 0) {
        DrawCircleLines(position.x, position.y, 25, TintAlpha(SKYBLUE, 0.5f + sin(GetTime()) * 0.2f));
        DrawCircleLines(position.x, position.y, 27, TintAlpha(SKYBLUE, 0.3f));
    }
}

void Player::TakeDamage(int damage, Vector2 hitDirection) {
    if (invincibilityTimer > 0 || dead) return;
    if (activePowerUp == PowerUpType::SHIELD && powerUpTimer > 0) {
        activePowerUp = PowerUpType::NONE;
        powerUpTimer = 0;
        invincibilityTimer = 1.0f;
        invincibilityFrames = 60;
        AudioManager::Instance().PlaySound(SoundType::HURT);
        return;
    }
    
    health -= damage;
    invincibilityTimer = 1.5f;
    invincibilityFrames = 90;
    
    // Knockback
    velocity.x = hitDirection.x * 400;
    velocity.y = hitDirection.y * 400 - 200;
    
    AudioManager::Instance().PlaySound(SoundType::HURT);
    
    if (health <= 0) {
        Kill();
    }
}

void Player::Kill() {
    if (dead) return;
    dead = true;
    AudioManager::Instance().PlaySound(SoundType::DEATH);
    // Death explosion particles will be emitted from Game
}

void Player::CollectPowerUp(PowerUpType type) {
    activePowerUp = type;
    powerUpTimer = 10.0f;  // 10 seconds
    AudioManager::Instance().PlaySound(SoundType::POWERUP);
}
