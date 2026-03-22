#pragma once

#include "Common.h"
#include <raylib.h>
#include <vector>
#include <string>
#include <functional>

class AchievementManager {
public:
    static AchievementManager& Instance();
    
    void Initialize();
    void Update(float dt);
    void Draw();
    
    void UnlockAchievement(const std::string& id);
    void ProgressAchievement(const std::string& id, int amount = 1);
    
    bool IsUnlocked(const std::string& id) const;
    int GetProgress(const std::string& id) const;
    int GetUnlockedCount() const;
    int GetTotalCount() const;
    float GetCompletionPercentage() const;
    
    const std::vector<Achievement>& GetAchievements() const { return achievements; }
    
    // Achievement triggers
    void OnCoinCollected(int amount = 1);
    void OnEnemyDefeated(int amount = 1);
    void OnLevelCompleted();
    void OnPlayerDeath();
    void OnDamageTaken(int damage);
    void OnJump(int amount = 1);
    void OnPowerUpCollected();
    void OnSecretFound();
    void OnPerfectLevel();
    void OnSpeedrun(float time);
    void OnCombo(int count);
    
private:
    AchievementManager();
    ~AchievementManager();
    AchievementManager(const AchievementManager&) = delete;
    AchievementManager& operator=(const AchievementManager&) = delete;
    
    void RegisterAchievement(const std::string& id, const std::string& name, 
                            const std::string& description, int required, Color color);
    void ShowNotification(const Achievement& achievement);
    
    std::vector<Achievement> achievements;
    std::vector<Achievement> newlyUnlocked;
    
    // Notification display
    float notificationTimer;
    Achievement currentNotification;
    bool showingNotification;
    float notificationAlpha;
    Vector2 notificationPosition;
    
    // Stats tracking
    int totalCoins;
    int totalEnemiesDefeated;
    int totalLevelsCompleted;
    int totalDeaths;
    int totalDamageTaken;
    int totalJumps;
    int powerUpsCollected;
    int secretsFound;
    int perfectLevels;
    int maxCombo;
    int currentCombo;
    
    float bestSpeedrunTime;
};
