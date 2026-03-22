#pragma once

#include "Common.h"
#include "TileMap.h"
#include "Enemy.h"
#include <raylib.h>
#include <vector>
#include <string>

struct LevelData {
    std::string filename;
    std::string name;
    std::string background;
    int difficulty;
    float timeLimit;
    int requiredCoins;
    Vector2 spawnPoint;
    Vector2 endPoint;
};

class Level {
public:
    Level();
    ~Level();
    
    bool Load(const std::string& filename);
    void Update(float dt, class Player& player);
    void Draw(const class GameCamera2D& camera);
    void DrawBackground(const class GameCamera2D& camera);
    void DrawForeground(const class GameCamera2D& camera);
    
    void Reset();
    
    TileMap& GetTileMap() { return tileMap; }
    const TileMap& GetTileMap() const { return tileMap; }
    
    std::vector<Enemy>& GetEnemies() { return enemies; }
    const std::vector<Enemy>& GetEnemies() const { return enemies; }
    
    std::string GetName() const { return name; }
    std::string GetNextLevel() const { return nextLevel; }
    bool IsLastLevel() const { return nextLevel.empty(); }
    
    float GetTimeElapsed() const { return timeElapsed; }
    float GetTimeLimit() const { return timeLimit; }
    int GetCoinsCollected() const { return coinsCollected; }
    int GetTotalCoins() const;
    int GetEnemiesDefeated() const { return enemiesDefeated; }
    int GetTotalEnemies() const { return static_cast<int>(enemies.size()); }
    
    bool IsComplete() const;
    bool HasTimeLimit() const { return timeLimit > 0; }
    
private:
    void SpawnEnemies();
    void UpdateCollectibles(float dt, Player& player);
    void UpdateEnemies(float dt, Player& player);
    void CheckLevelComplete(Player& player);
    
    TileMap tileMap;
    std::vector<Enemy> enemies;
    std::vector<Enemy> initialEnemies;
    
    std::string name;
    std::string nextLevel;
    std::string background;
    
    float timeElapsed;
    float timeLimit;
    int coinsCollected;
    int enemiesDefeated;
    int requiredCoins;
    
    bool loaded;
};
