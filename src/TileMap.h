#pragma once

#include "Common.h"
#include <vector>

struct Collectible {
    enum Type { Coin, Heart, Gem };
    
    Type type;
    float x, y;
    float width, height;
    bool collected;
    int textureId;
    float bobTimer;
    
    AABB getBounds() const {
        return {x, y, width, height};
    }
};

class TileMap {
public:
    TileMap();
    
    bool loadFromFile(const std::string& path);
    void generateLevel(int width, int height);
    
    void update(float dt);
    void render();
    
    // Доступ к тайлам
    Tile getTile(int col, int row) const;
    void setTile(int col, int row, TileType type);
    
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    float getTileSize() const { return TILE_SIZE; }
    
    // Коллизии
    bool isSolid(int col, int row) const;
    bool isPlatform(int col, int row) const;
    AABB getTileBounds(int col, int row) const;
    
    // Проверка коллизий с игроком
    void checkCollisions(class Player& player);
    void checkCollectibles(class Player& player);
    
    // Спавн игрока
    glm::vec2 getPlayerSpawn() const { return m_playerSpawn; }
    
    // Враги
    void addEnemy(class Enemy&& enemy);
    std::vector<class Enemy>& getEnemies() { return m_enemies; }
    const std::vector<class Enemy>& getEnemies() const { return m_enemies; }
    
    // Предметы
    void addCollectible(const Collectible& collectible);
    std::vector<Collectible>& getCollectibles() { return m_collectibles; }
    
private:
    int m_width;
    int m_height;
    std::vector<Tile> m_tiles;
    std::vector<Collectible> m_collectibles;
    std::vector<class Enemy> m_enemies;
    glm::vec2 m_playerSpawn;
    
    int m_groundTexture;
    int m_platformTexture;
    int m_brickTexture;
    int m_blockTexture;
    int m_coinTexture;
    int m_heartTexture;
    
    void loadTextures();
    char tileTypeToChar(TileType type) const;
    TileType charToTileType(char c) const;
};
