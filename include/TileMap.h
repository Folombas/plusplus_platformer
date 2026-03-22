#pragma once

#include "Common.h"
#include <raylib.h>
#include <string>
#include <vector>

struct Tile {
    int id;
    bool solid;
    bool dangerous;
    bool oneWay;
    int animationFrame;
    float animationTimer;
    std::vector<int> animationFrames;
};

struct Tileset {
    Texture2D texture;
    int tileWidth;
    int tileHeight;
    int tilesPerRow;
};

class TileMap {
public:
    TileMap();
    ~TileMap();
    
    bool LoadFromFile(const std::string& filename);
    void Update(float dt);
    void Draw(const class GameCamera2D& camera, int renderLayer = -1);
    void DrawBackground(const class GameCamera2D& camera, const std::vector<Texture2D>& parallaxLayers);
    
    int GetTile(int x, int y) const;
    void SetTile(int x, int y, int id);
    
    int GetWidth() const { return width; }
    int GetHeight() const { return height; }
    int GetTileSize() const { return tileSize; }
    
    Vector2 GetSpawnPoint() const { return spawnPoint; }
    Vector2 GetLevelEnd() const { return levelEnd; }
    
    Rectangle GetTileBounds(int x, int y) const;
    bool IsTileSolid(int x, int y) const;
    bool IsTileDangerous(int x, int y) const;
    bool IsTileOneWay(int x, int y) const;
    
    // Collision
    Rectangle GetTileCollision(int x, int y) const;
    std::vector<Rectangle> GetTileCollisionsAround(Vector2 worldPos, int range) const;
    
    // Collectibles
    std::vector<Collectible>& GetCollectibles() { return collectibles; }
    const std::vector<Collectible>& GetCollectibles() const { return collectibles; }
    
    // Level info
    std::string GetLevelName() const { return levelName; }
    std::string GetLevelMusic() const { return levelMusic; }
    
private:
    void ParseTileProperties(const std::string& line);
    void ParseCollectibles(const std::string& line);
    void ParseEnemies(const std::string& line);
    void GenerateDefaultLevel();

    int width;
    int height;
    int tileSize;
    
    std::vector<int> tiles;
    std::vector<Tile> tileProperties;
    Tileset tileset;
    
    Vector2 spawnPoint;
    Vector2 levelEnd;
    
    std::vector<Collectible> collectibles;
    
    std::string levelName;
    std::string levelMusic;
    
    // Background layers
    std::vector<Texture2D> parallaxLayers;
    float parallaxOffset[4];
};
