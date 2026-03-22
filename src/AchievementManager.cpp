#include "AchievementManager.h"
#include "AudioManager.h"
#include <raylib.h>

#ifdef _WIN32
#undef CloseWindow
#undef ShowCursor
#endif

static Font GetDefaultFont() {
    return GetFontDefault();
}

AchievementManager::AchievementManager()
    : notificationTimer(0)
    , showingNotification(false)
    , notificationAlpha(0)
    , notificationPosition{50, 50}
    , totalCoins(0)
    , totalEnemiesDefeated(0)
    , totalLevelsCompleted(0)
    , totalDeaths(0)
    , totalDamageTaken(0)
    , totalJumps(0)
    , powerUpsCollected(0)
    , secretsFound(0)
    , perfectLevels(0)
    , maxCombo(0)
    , currentCombo(0)
    , bestSpeedrunTime(99999)
{
}

AchievementManager::~AchievementManager() {
}

AchievementManager& AchievementManager::Instance() {
    static AchievementManager instance;
    return instance;
}

void AchievementManager::Initialize() {
    // Register all achievements
    RegisterAchievement("first_coin", "First Steps", "Collect your first coin", 1, GOLD);
    RegisterAchievement("coin_collector", "Coin Collector", "Collect 50 coins", 50, GOLD);
    RegisterAchievement("coin_master", "Coin Master", "Collect 200 coins", 200, YELLOW);
    
    RegisterAchievement("first_enemy", "Monster Slayer", "Defeat your first enemy", 1, RED);
    RegisterAchievement("enemy_hunter", "Enemy Hunter", "Defeat 25 enemies", 25, ORANGE);
    RegisterAchievement("enemy_master", "Enemy Master", "Defeat 100 enemies", 100, MAROON);
    
    RegisterAchievement("first_level", "Adventure Begins", "Complete your first level", 1, GREEN);
    RegisterAchievement("level_runner", "Level Runner", "Complete 10 levels", 10, LIME);
    RegisterAchievement("speedster", "Speedster", "Complete a level in under 30 seconds", 1, SKYBLUE);
    
    RegisterAchievement("jumper", "Jump Master", "Jump 100 times", 100, PURPLE);
    RegisterAchievement("powerup_user", "Power Up!", "Collect a power-up", 1, PINK);
    RegisterAchievement("survivor", "Survivor", "Take 50 damage and survive", 50, DARKGRAY);
    
    RegisterAchievement("combo_5", "Combo x5", "Get a 5-hit combo", 5, ORANGE);
    RegisterAchievement("combo_10", "Combo x10", "Get a 10-hit combo", 10, GOLD);
    
    RegisterAchievement("secret_finder", "Secret Finder", "Find a secret area", 1, DARKPURPLE);
    RegisterAchievement("perfectionist", "Perfectionist", "Complete a level perfectly", 1, RAYWHITE);
}

void AchievementManager::RegisterAchievement(const std::string& id, const std::string& name,
                                              const std::string& description, int required, Color color) {
    Achievement achievement;
    achievement.id = id;
    achievement.name = name;
    achievement.description = description;
    achievement.required = required;
    achievement.progress = 0;
    achievement.unlocked = false;
    achievement.color = color;
    
    achievements.push_back(achievement);
}

void AchievementManager::Update(float dt) {
    // Update notification
    if (showingNotification) {
        notificationTimer -= dt;
        
        if (notificationTimer < 0.5f) {
            notificationAlpha = notificationTimer / 0.5f;
        } else if (notificationAlpha < 1.0f) {
            notificationAlpha += dt * 5;
            if (notificationAlpha > 1.0f) notificationAlpha = 1.0f;
        }
        
        if (notificationTimer <= 0) {
            showingNotification = false;
            newlyUnlocked.erase(newlyUnlocked.begin());
        }
    } else if (!newlyUnlocked.empty()) {
        showingNotification = true;
        notificationTimer = 3.0f;
        notificationAlpha = 0;
        currentNotification = newlyUnlocked[0];
        AudioManager::Instance().PlaySound(SoundType::ACHIEVEMENT);
    }
}

void AchievementManager::Draw() {
    if (!showingNotification || !achievements.empty()) {
        // Draw notification
        int boxWidth = 350;
        int boxHeight = 80;
        float x = 50;
        float y = 50;
        
        // Background
        DrawRectangle(x - 10, y - 10, boxWidth + 20, boxHeight + 20, 
                      TintAlpha(BLACK, 0.7f * notificationAlpha));
        DrawRectangle(x, y, boxWidth, boxHeight, 
                      TintAlpha(DARKGRAY, 0.9f * notificationAlpha));
        
        // Border
        DrawRectangleLines(x, y, boxWidth, boxHeight, 
                           TintAlpha(currentNotification.color, notificationAlpha));
        
        // Icon
        DrawCircle(x + 40, y + 40, 25, TintAlpha(currentNotification.color, notificationAlpha));
        DrawTextEx(GetDefaultFont(), "!", {x + 35, y + 25}, 30, 2, WHITE);

        // Title
        DrawTextEx(GetDefaultFont(), "ACHIEVEMENT UNLOCKED!", {x + 90, y + 15}, 16, 2, TintAlpha(GOLD, notificationAlpha));

        // Name
        DrawTextEx(GetDefaultFont(), currentNotification.name.c_str(), {x + 90, y + 38}, 20, 2, WHITE);

        // Description
        DrawTextEx(GetDefaultFont(), currentNotification.description.c_str(), {x + 90, y + 60}, 14, 2, TintAlpha(LIGHTGRAY, notificationAlpha));
    }
    
    // Draw achievement list (optional, for debug/menu)
    // DrawAchievementList();
}

void AchievementManager::UnlockAchievement(const std::string& id) {
    for (auto& achievement : achievements) {
        if (achievement.id == id && !achievement.unlocked) {
            achievement.unlocked = true;
            achievement.progress = achievement.required;
            newlyUnlocked.push_back(achievement);
        }
    }
}

void AchievementManager::ProgressAchievement(const std::string& id, int amount) {
    for (auto& achievement : achievements) {
        if (achievement.id == id && !achievement.unlocked) {
            achievement.progress += amount;
            if (achievement.progress >= achievement.required) {
                UnlockAchievement(id);
            }
        }
    }
}

bool AchievementManager::IsUnlocked(const std::string& id) const {
    for (const auto& achievement : achievements) {
        if (achievement.id == id) {
            return achievement.unlocked;
        }
    }
    return false;
}

int AchievementManager::GetProgress(const std::string& id) const {
    for (const auto& achievement : achievements) {
        if (achievement.id == id) {
            return achievement.progress;
        }
    }
    return 0;
}

int AchievementManager::GetUnlockedCount() const {
    int count = 0;
    for (const auto& achievement : achievements) {
        if (achievement.unlocked) count++;
    }
    return count;
}

int AchievementManager::GetTotalCount() const {
    return static_cast<int>(achievements.size());
}

float AchievementManager::GetCompletionPercentage() const {
    if (achievements.empty()) return 0;
    return (static_cast<float>(GetUnlockedCount()) / GetTotalCount()) * 100;
}

// Achievement triggers
void AchievementManager::OnCoinCollected(int amount) {
    totalCoins += amount;
    ProgressAchievement("first_coin", amount);
    ProgressAchievement("coin_collector", amount);
    ProgressAchievement("coin_master", amount);
}

void AchievementManager::OnEnemyDefeated(int amount) {
    totalEnemiesDefeated += amount;
    currentCombo++;
    if (currentCombo > maxCombo) maxCombo = currentCombo;
    
    ProgressAchievement("first_enemy", amount);
    ProgressAchievement("enemy_hunter", amount);
    ProgressAchievement("enemy_master", amount);
    
    if (currentCombo >= 5) {
        ProgressAchievement("combo_5", 1);
    }
    if (currentCombo >= 10) {
        ProgressAchievement("combo_10", 1);
    }
}

void AchievementManager::OnLevelCompleted() {
    totalLevelsCompleted++;
    ProgressAchievement("first_level", 1);
    ProgressAchievement("level_runner", 1);
}

void AchievementManager::OnPlayerDeath() {
    totalDeaths++;
    currentCombo = 0;
}

void AchievementManager::OnDamageTaken(int damage) {
    totalDamageTaken += damage;
    ProgressAchievement("survivor", damage);
}

void AchievementManager::OnJump(int amount) {
    totalJumps += amount;
    ProgressAchievement("jumper", amount);
}

void AchievementManager::OnPowerUpCollected() {
    powerUpsCollected++;
    ProgressAchievement("powerup_user", 1);
}

void AchievementManager::OnSecretFound() {
    secretsFound++;
    ProgressAchievement("secret_finder", 1);
}

void AchievementManager::OnPerfectLevel() {
    perfectLevels++;
    ProgressAchievement("perfectionist", 1);
}

void AchievementManager::OnSpeedrun(float time) {
    if (time < bestSpeedrunTime) {
        bestSpeedrunTime = time;
        if (time < 30) {
            ProgressAchievement("speedster", 1);
        }
    }
}

void AchievementManager::OnCombo(int count) {
    currentCombo = count;
    if (count >= 5) ProgressAchievement("combo_5", 1);
    if (count >= 10) ProgressAchievement("combo_10", 1);
}
