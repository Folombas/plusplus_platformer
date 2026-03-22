#include "Level.h"
#include "Player.h"
#include "AudioManager.h"
#include "ParticleSystem.h"
#include "AchievementManager.h"

#ifdef _WIN32
#undef CloseWindow
#undef ShowCursor
#endif

Level::Level()
    : name("Untitled")
    , nextLevel("")
    , background("forest")
    , timeElapsed(0)
    , timeLimit(0)
    , coinsCollected(0)
    , enemiesDefeated(0)
    , requiredCoins(0)
    , loaded(false)
{
}

Level::~Level() {
}

bool Level::Load(const std::string& filename) {
    if (!tileMap.LoadFromFile(filename)) {
        return false;
    }
    
    name = tileMap.GetLevelName();
    
    // Spawn enemies at predefined positions or generate default
    SpawnEnemies();
    
    // Store initial enemy positions for reset
    initialEnemies = enemies;
    
    timeElapsed = 0;
    coinsCollected = 0;
    enemiesDefeated = 0;
    
    loaded = true;
    return true;
}

void Level::SpawnEnemies() {
    // Default enemy spawns based on level layout
    // In a full implementation, this would be loaded from the level file
    
    int mapWidth = tileMap.GetWidth();
    int mapHeight = tileMap.GetHeight();
    int tileSize = tileMap.GetTileSize();
    
    // Add some enemies at strategic positions
    if (mapWidth > 20) {
        enemies.emplace_back(Vector2{400, float((mapHeight - 6) * tileSize)}, EnemyType::GROUND);
        enemies.back().SetPatrolPoints({300, float((mapHeight - 6) * tileSize)}, {500, float((mapHeight - 6) * tileSize)});
    }
    
    if (mapWidth > 35) {
        enemies.emplace_back(Vector2{800, float((mapHeight - 4) * tileSize)}, EnemyType::JUMPING);
        enemies.back().SetPatrolPoints({700, float((mapHeight - 4) * tileSize)}, {900, float((mapHeight - 4) * tileSize)});
    }
    
    if (mapWidth > 45) {
        enemies.emplace_back(Vector2{1200, float((mapHeight - 6) * tileSize)}, EnemyType::FLYING);
        enemies.back().SetPatrolPoints({1100, float((mapHeight - 8) * tileSize)}, {1300, float((mapHeight - 6) * tileSize)});
    }
}

void Level::Reset() {
    tileMap.LoadFromFile("");  // Regenerate
    enemies = initialEnemies;
    timeElapsed = 0;
    coinsCollected = 0;
    enemiesDefeated = 0;
}

void Level::Update(float dt, Player& player) {
    if (!loaded) return;
    
    // Update time
    timeElapsed += dt;
    
    // Update tilemap (animations, collectibles)
    tileMap.Update(dt);
    
    // Update collectibles collision
    UpdateCollectibles(dt, player);
    
    // Update enemies
    UpdateEnemies(dt, player);
    
    // Check level completion
    CheckLevelComplete(player);
}

void Level::UpdateCollectibles(float dt, Player& player) {
    auto& collectibles = tileMap.GetCollectibles();
    Rectangle playerBounds = player.GetBounds();
    
    for (auto& coin : collectibles) {
        if (coin.collected) continue;
        
        // Simple circle collision
        float dist = Vector2Distance(coin.position, player.GetPosition());
        if (dist < 30) {
            coin.collected = true;
            coinsCollected++;
            player.AddCoins(coin.value);
            player.AddScore(coin.value * 10);
            
            // Play sound and effects
            AudioManager::Instance().PlaySound(SoundType::COIN);
            
            // Achievement
            AchievementManager::Instance().OnCoinCollected();
        }
    }
}

void Level::UpdateEnemies(float dt, Player& player) {
    Rectangle playerBounds = player.GetBounds();
    
    for (auto& enemy : enemies) {
        if (!enemy.IsActive()) continue;
        
        enemy.Update(dt, tileMap, &player);
        
        // Check collision with player
        if (enemy.IsActive() && !player.IsDead()) {
            Rectangle enemyBounds = enemy.GetBounds();
            
            if (CheckCollisionRecs(playerBounds, enemyBounds)) {
                // Check if player is stomping the enemy
                if (player.GetVelocity().y > 0 &&
                    player.GetPosition().y < enemy.GetPosition().y - 10) {
                    // Stomp!
                    enemy.TakeDamage(1, {player.IsFacingRight() ? 1.0f : -1.0f, 1.0f});
                    player.SetVelocity({player.GetVelocity().x, PLAYER_JUMP * 0.7f});  // Bounce
                    
                    player.AddScore(enemy.GetHealth() > 0 ? 0 : 50);
                    
                    if (!enemy.IsActive() || enemy.GetHealth() <= 0) {
                        enemiesDefeated++;
                        AchievementManager::Instance().OnEnemyDefeated();
                    }
                } else {
                    // Player takes damage
                    if (!player.IsInvincible()) {
                        Vector2 hitDir = {
                            player.GetPosition().x < enemy.GetPosition().x ? -1 : 1,
                            -0.5f
                        };
                        player.TakeDamage(enemy.GetHealth() > 0 ? 1 : 1, hitDir);
                        AchievementManager::Instance().OnDamageTaken(1);
                    }
                }
            }
        }
    }
    
    // Remove dead enemies
    enemies.erase(
        std::remove_if(enemies.begin(), enemies.end(),
            [](const Enemy& e) { return !e.IsActive(); }),
        enemies.end()
    );
}

void Level::CheckLevelComplete(Player& player) {
    Vector2 levelEnd = tileMap.GetLevelEnd();
    if (levelEnd.x <= 0) return;
    
    float dist = Vector2Distance(player.GetPosition(), levelEnd);
    if (dist < 50) {
        // Check if all requirements are met
        if (coinsCollected >= requiredCoins) {
            // Level complete!
            AchievementManager::Instance().OnLevelCompleted();
        }
    }
}

void Level::Draw(const GameCamera2D& camera) {
    if (!loaded) return;
    
    tileMap.Draw(camera);
    
    // Draw enemies
    for (auto& enemy : enemies) {
        enemy.Draw(camera);
    }
}

void Level::DrawBackground(const GameCamera2D& camera) {
    // Draw parallax background layers
    std::vector<Texture2D> layers;
    
    // Load parallax layers
    const char* layerPaths[] = {
        "assets/parallax/forest/parallax_forest_pack/layers/parallax-forest-back-trees.png",
        "assets/parallax/forest/parallax_forest_pack/layers/parallax-forest-middle-trees.png",
        "assets/parallax/forest/parallax_forest_pack/layers/parallax-forest-lights.png",
        "assets/parallax/forest/parallax_forest_pack/layers/parallax-forest-front-trees.png"
    };
    
    for (const char* path : layerPaths) {
        if (FileExists(path)) {
            Texture2D tex = LoadTexture(path);
            if (tex.id != 0) {
                layers.push_back(tex);
            }
        }
    }
    
    if (!layers.empty()) {
        tileMap.DrawBackground(camera, layers);
        
        // Clean up
        for (auto& tex : layers) {
            UnloadTexture(tex);
        }
    } else {
        // Fallback gradient background
        DrawGradientV(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 
                      SKYBLUE, DARKBLUE);
    }
}

void Level::DrawForeground(const GameCamera2D& /*camera*/) {
    // Draw foreground elements (UI overlays, etc.)
}

int Level::GetTotalCoins() const {
    return static_cast<int>(tileMap.GetCollectibles().size());
}

bool Level::IsComplete() const {
    return coinsCollected >= requiredCoins;
}
