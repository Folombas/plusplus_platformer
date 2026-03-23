#include "TileMap.h"
#include "Player.h"
#include "Enemy.h"
#include "Texture.h"
#include "Renderer.h"
#include <fstream>
#include <sstream>
#include <iostream>

TileMap::TileMap()
    : m_width(0)
    , m_height(0)
    , m_groundTexture(-1)
    , m_platformTexture(-1)
    , m_brickTexture(-1)
    , m_blockTexture(-1)
    , m_coinTexture(-1)
    , m_heartTexture(-1)
{}

void TileMap::loadTextures() {
    auto& texMgr = TextureManager::getInstance();
    
    // Загружаем текстуры тайлов из Base pack
    m_groundTexture = texMgr.loadTexture("ground", "assets/Base pack/Tiles/brickWall.png");
    m_platformTexture = texMgr.loadTexture("platform", "assets/Base pack/Tiles/bridge.png");
    m_brickTexture = texMgr.loadTexture("brick", "assets/Base pack/Tiles/box.png");
    m_blockTexture = texMgr.loadTexture("block", "assets/Base pack/Tiles/boxEmpty.png");
    
    // Предметы
    m_coinTexture = texMgr.loadTexture("coin", "assets/Base pack/Items/coinBronze.png");
    m_heartTexture = texMgr.loadTexture("heart", "assets/Base pack/Items/gemRed.png");
}

bool TileMap::loadFromFile(const std::string& path) {
    loadTextures();
    
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Cannot open level file: " << path << std::endl;
        return false;
    }
    
    std::string line;
    std::vector<std::string> lines;
    
    while (std::getline(file, line)) {
        if (!line.empty() && line[0] != '#') {
            lines.push_back(line);
        }
    }
    
    if (lines.empty()) {
        std::cerr << "Empty level file" << std::endl;
        return false;
    }
    
    m_height = static_cast<int>(lines.size());
    m_width = 0;
    
    for (const auto& l : lines) {
        if (static_cast<int>(l.length()) > m_width) {
            m_width = static_cast<int>(l.length());
        }
    }
    
    m_tiles.resize(m_width * m_height);
    
    for (int row = 0; row < m_height; row++) {
        for (int col = 0; col < static_cast<int>(lines[row].length()); col++) {
            char c = lines[row][col];
            TileType type = charToTileType(c);
            
            int idx = row * m_width + col;
            m_tiles[idx] = {type, 0, isSolid(col, row), false};
            
            // Спавн игрока
            if (c == 'P') {
                m_playerSpawn = glm::vec2(col * TILE_SIZE, row * TILE_SIZE);
            }
            
            // Предметы
            if (c == 'C') {
                Collectible coin;
                coin.type = Collectible::Coin;
                coin.x = col * TILE_SIZE + TILE_SIZE / 4;
                coin.y = row * TILE_SIZE + TILE_SIZE / 4;
                coin.width = TILE_SIZE / 2;
                coin.height = TILE_SIZE / 2;
                coin.collected = false;
                coin.textureId = m_coinTexture;
                coin.bobTimer = 0.0f;
                m_collectibles.push_back(coin);
            } else if (c == 'H') {
                Collectible heart;
                heart.type = Collectible::Heart;
                heart.x = col * TILE_SIZE + TILE_SIZE / 4;
                heart.y = row * TILE_SIZE + TILE_SIZE / 4;
                heart.width = TILE_SIZE / 2;
                heart.height = TILE_SIZE / 2;
                heart.collected = false;
                heart.textureId = m_heartTexture;
                heart.bobTimer = 0.0f;
                m_collectibles.push_back(heart);
            }
            
            // Враги
            if (c == 'S') {
                Enemy slime(EnemyType::Slime, col * TILE_SIZE, row * TILE_SIZE);
                slime.setPatrolRange((col - 2) * TILE_SIZE, (col + 2) * TILE_SIZE);
                m_enemies.push_back(std::move(slime));
            } else if (c == 'B') {
                Enemy bat(EnemyType::Bat, col * TILE_SIZE, row * TILE_SIZE);
                bat.setPatrolRange((col - 3) * TILE_SIZE, (col + 3) * TILE_SIZE);
                m_enemies.push_back(std::move(bat));
            }
        }
    }
    
    // Загружаем анимации для врагов
    for (auto& enemy : m_enemies) {
        enemy.loadAnimations();
    }
    
    return true;
}

void TileMap::generateLevel(int width, int height) {
    loadTextures();
    
    m_width = width;
    m_height = height;
    m_tiles.resize(m_width * m_height);
    
    // Заполняем воздухом
    for (int i = 0; i < m_width * m_height; i++) {
        m_tiles[i] = {TileType::Air, 0, false, false};
    }
    
    // Пол
    for (int col = 0; col < m_width; col++) {
        int idx = (m_height - 1) * m_width + col;
        m_tiles[idx] = {TileType::Ground, m_groundTexture, true, false};
        
        // Платформы
        if (col > 2 && col < m_width - 2 && col % 4 == 0) {
            int platformRow = m_height - 3 - (col % 8) / 4;
            if (platformRow > 1) {
                int pIdx = platformRow * m_width + col;
                m_tiles[pIdx] = {TileType::Platform, m_platformTexture, true, true};
                
                // Монета над платформой
                Collectible coin;
                coin.type = Collectible::Coin;
                coin.x = col * TILE_SIZE + TILE_SIZE / 4;
                coin.y = platformRow * TILE_SIZE - TILE_SIZE / 2;
                coin.width = TILE_SIZE / 2;
                coin.height = TILE_SIZE / 2;
                coin.collected = false;
                coin.textureId = m_coinTexture;
                coin.bobTimer = static_cast<float>(col);
                m_collectibles.push_back(coin);
            }
        }
    }
    
    // Стены по краям
    for (int row = 0; row < m_height; row++) {
        m_tiles[row * m_width] = {TileType::Block, m_blockTexture, true, false};
        m_tiles[row * m_width + m_width - 1] = {TileType::Block, m_blockTexture, true, false};
    }
    
    // Спавн игрока
    m_playerSpawn = glm::vec2(100.0f, 400.0f);
    
    // Добавляем врагов
    Enemy slime1(EnemyType::Slime, 400, 500);
    slime1.setPatrolRange(350, 550);
    m_enemies.push_back(std::move(slime1));
    
    Enemy slime2(EnemyType::Slime, 700, 500);
    slime2.setPatrolRange(600, 800);
    m_enemies.push_back(std::move(slime2));
    
    Enemy bat(EnemyType::Bat, 500, 300);
    bat.setPatrolRange(400, 700);
    m_enemies.push_back(std::move(bat));
    
    // Загружаем анимации
    for (auto& enemy : m_enemies) {
        enemy.loadAnimations();
    }
}

Tile TileMap::getTile(int col, int row) const {
    if (col < 0 || col >= m_width || row < 0 || row >= m_height) {
        return {TileType::Air, 0, false, false};
    }
    return m_tiles[row * m_width + col];
}

void TileMap::setTile(int col, int row, TileType type) {
    if (col < 0 || col >= m_width || row < 0 || row >= m_height) return;
    
    int idx = row * m_width + col;
    m_tiles[idx].type = type;
    m_tiles[idx].solid = isSolid(col, row);
}

bool TileMap::isSolid(int col, int row) const {
    TileType type = getTile(col, row).type;
    return type == TileType::Ground || type == TileType::Block || 
           type == TileType::Brick || type == TileType::Platform;
}

bool TileMap::isPlatform(int col, int row) const {
    return getTile(col, row).type == TileType::Platform;
}

AABB TileMap::getTileBounds(int col, int row) const {
    return {
        col * TILE_SIZE,
        row * TILE_SIZE,
        TILE_SIZE,
        TILE_SIZE
    };
}

void TileMap::update(float dt) {
    // Обновляем врагов
    for (auto& enemy : m_enemies) {
        if (enemy.isAlive()) {
            enemy.update(dt);
        }
    }
    
    // Обновляем предметы (анимация парения)
    for (auto& collectible : m_collectibles) {
        if (!collectible.collected) {
            collectible.bobTimer += dt;
        }
    }
}

void TileMap::render() {
    auto& renderer = Renderer::getInstance();
    
    // Рендерим тайлы
    for (int row = 0; row < m_height; row++) {
        for (int col = 0; col < m_width; col++) {
            Tile tile = getTile(col, row);
            if (tile.type != TileType::Air && tile.textureId >= 0) {
                float x = col * TILE_SIZE;
                float y = row * TILE_SIZE;
                renderer.drawSprite(tile.textureId, x, y, TILE_SIZE, TILE_SIZE);
            }
        }
    }
    
    // Рендерим предметы
    for (auto& collectible : m_collectibles) {
        if (!collectible.collected && collectible.textureId >= 0) {
            float bobOffset = std::sin(collectible.bobTimer * 5.0f) * 5.0f;
            renderer.drawSprite(
                collectible.textureId,
                collectible.x,
                collectible.y + bobOffset,
                collectible.width,
                collectible.height
            );
        }
    }
    
    // Рендерим врагов
    for (auto& enemy : m_enemies) {
        if (enemy.isAlive()) {
            enemy.render();
        }
    }
}

void TileMap::checkCollisions(Player& player) {
    AABB playerBounds = player.getBounds();
    
    // Проверяем тайлы вокруг игрока
    int startCol = static_cast<int>(playerBounds.x / TILE_SIZE) - 1;
    int endCol = static_cast<int>((playerBounds.x + playerBounds.width) / TILE_SIZE) + 1;
    int startRow = static_cast<int>(playerBounds.y / TILE_SIZE) - 1;
    int endRow = static_cast<int>((playerBounds.y + playerBounds.height) / TILE_SIZE) + 1;
    
    for (int row = startRow; row <= endRow; row++) {
        for (int col = startCol; col <= endCol; col++) {
            Tile tile = getTile(col, row);
            if (tile.solid) {
                AABB tileBounds = getTileBounds(col, row);
                bool isPlatform = isPlatform(col, row);
                player.resolveCollision(tileBounds, isPlatform);
            }
        }
    }
    
    // Проверяем врагов
    for (auto& enemy : m_enemies) {
        if (enemy.isAlive() && playerBounds.intersects(enemy.getBounds())) {
            // Игрок прыгает на врага сверху
            if (player.getVelocity().y > 0 &&
                playerBounds.y + playerBounds.height < enemy.getBounds().y + enemy.getBounds().height / 2) {
                enemy.takeDamage(1);
                player.setVelocity(player.getVelocity().x, -PLAYER_JUMP_FORCE / 2);
                player.addScore(enemy.getScoreValue());
            } else {
                player.takeDamage(1);
            }
        }
    }
    
    // Удаляем мертвых врагов
    m_enemies.erase(
        std::remove_if(m_enemies.begin(), m_enemies.end(),
            [](const Enemy& e) { return !e.isAlive() && /* deathTimer */ false; }),
        m_enemies.end()
    );
}

void TileMap::checkCollectibles(Player& player) {
    AABB playerBounds = player.getBounds();
    
    for (auto& collectible : m_collectibles) {
        if (!collectible.collected) {
            if (playerBounds.intersects(collectible.getBounds())) {
                collectible.collected = true;
                
                if (collectible.type == Collectible::Coin) {
                    player.addCoins(1);
                } else if (collectible.type == Collectible::Heart) {
                    player.heal(1);
                }
            }
        }
    }
}

void TileMap::addEnemy(Enemy&& enemy) {
    enemy.loadAnimations();
    m_enemies.push_back(std::move(enemy));
}

void TileMap::addCollectible(const Collectible& collectible) {
    m_collectibles.push_back(collectible);
}

char TileMap::tileTypeToChar(TileType type) const {
    switch (type) {
        case TileType::Air: return ' ';
        case TileType::Ground: return '#';
        case TileType::Platform: return '=';
        case TileType::Brick: return 'B';
        case TileType::Block: return 'X';
        default: return ' ';
    }
}

TileType TileMap::charToTileType(char c) const {
    switch (c) {
        case ' ': return TileType::Air;
        case '#': return TileType::Ground;
        case '=': return TileType::Platform;
        case 'B': return TileType::Brick;
        case 'X': return TileType::Block;
        case 'C': return TileType::Air;
        case 'H': return TileType::Air;
        case 'P': return TileType::Air;
        case 'S': return TileType::Air;
        default: return TileType::Air;
    }
}
