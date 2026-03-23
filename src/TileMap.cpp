#include "TileMap.h"
#include "Camera2D.h"
#include <fstream>
#include <sstream>
#include <algorithm>

#ifdef _WIN32
#undef CloseWindow
#undef ShowCursor
#endif

TileMap::TileMap()
    : width(0), height(0), tileSize(TILE_SIZE)
    , spawnPoint{100, 100}, levelEnd{0, 0}
    , levelName("Untitled"), levelMusic("")
{
    for (int i = 0; i < 4; i++) parallaxOffset[i] = 0;
    
    tileProperties.resize(256);
    for (int i = 0; i < 256; i++) {
        tileProperties[i].id = i;
        tileProperties[i].solid = false;
        tileProperties[i].dangerous = false;
        tileProperties[i].oneWay = false;
    }
    for (int i = 1; i <= 80; i++) tileProperties[i].solid = true;
    tileProperties[90].dangerous = true;
    tileProperties[91].dangerous = true;
    
    const char* tilePaths[] = {
        "assets/sprites/platformer/Base pack/Tiles/dirt.png",
        "assets/sprites/platformer/Base pack/Tiles/grass.png",
        "assets/sprites/platformer/Base pack/Tiles/brickWall.png"
    };
    for (const char* path : tilePaths) {
        if (FileExists(path)) {
            tileset.texture = LoadTexture(path);
            tileset.tileWidth = tileSize;
            tileset.tileHeight = tileSize;
            tileset.tilesPerRow = tileset.texture.width / tileSize;
            break;
        }
    }
    if (tileset.texture.width == 0) {
        Image img = GenImageColor(tileSize, tileSize, BROWN);
        tileset.texture = LoadTextureFromImage(img);
        UnloadImage(img);
        tileset.tileWidth = tileSize;
        tileset.tileHeight = tileSize;
        tileset.tilesPerRow = 1;
    }
}

TileMap::~TileMap() {
    if (tileset.texture.width != 0) UnloadTexture(tileset.texture);
}

void TileMap::GenerateDefaultLevel() {
    width = 50; height = 15;
    tiles.assign(width * height, 0);
    levelName = "Default Level";
    
    for (int x = 0; x < width; x++) {
        tiles[(height-1)*width + x] = 2;
        tiles[(height-2)*width + x] = 2;
    }
    for (int x = 5; x < 15; x++) tiles[(height-5)*width + x] = 1;
    for (int x = 20; x < 35; x++) tiles[(height-8)*width + x] = 3;
    
    for (int i = 0; i < 10; i++) {
        Collectible coin;
        coin.position = {100.0f + i*80.0f, float((height-10)*tileSize)};
        coin.type = "coin"; coin.collected = false;
        coin.bobOffset = i * 0.5f; coin.value = 10;
        collectibles.push_back(coin);
    }
    spawnPoint = {100, float((height-4)*tileSize)};
    levelEnd = {float((width-2)*tileSize), float((height-4)*tileSize)};
}

bool TileMap::LoadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) { GenerateDefaultLevel(); return true; }
    
    std::string line; int row = 0;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        if (line.find("SPAWN:") == 0) { sscanf_s(line.c_str(), "SPAWN:%f,%f", &spawnPoint.x, &spawnPoint.y); continue; }
        if (line.find("END:") == 0) { sscanf_s(line.c_str(), "END:%f,%f", &levelEnd.x, &levelEnd.y); continue; }
        if (line.find("NAME:") == 0) { levelName = line.substr(5); continue; }
        if (line.find("COIN:") == 0) {
            float x, y; char type[32]; int value;
            sscanf_s(line.c_str(), "COIN:%f,%f,%31s,%d", &x, &y, type, (unsigned)sizeof(type), &value);
            Collectible coin; coin.position = {x, y}; coin.type = type;
            coin.collected = false; coin.bobOffset = float(GetRandomValue(0,100))/100.0f*6.28f;
            coin.value = value > 0 ? value : 10;
            collectibles.push_back(coin); continue;
        }
        std::istringstream iss(line); int tileId, col = 0;
        while (iss >> tileId) {
            if (row >= height) height = row + 1;
            if (col >= width) width = col + 1;
            if (row*100 + col >= (int)tiles.size()) tiles.resize((row+1)*100);
            tiles[row*100 + col] = tileId; col++;
        }
        row++;
    }
    file.close();
    if (spawnPoint.x == 100 && spawnPoint.y == 100) spawnPoint = {100, 100};
    return true;
}

void TileMap::Update(float dt) {
    for (auto& coin : collectibles) if (!coin.collected) coin.bobOffset += dt * 3;
}

void TileMap::Draw(const GameCamera2D& camera, int /*renderLayer*/) {
    Vector2 camPos = camera.GetPosition();
    float zoom = camera.GetZoom();
    int startCol = std::max(0, (int)((camPos.x - SCREEN_WIDTH/(2*zoom)) / tileSize) - 1);
    int endCol = std::min(width, (int)((camPos.x + SCREEN_WIDTH/(2*zoom)) / tileSize) + 2);
    int startRow = std::max(0, (int)((camPos.y - SCREEN_HEIGHT/(2*zoom)) / tileSize) - 1);
    int endRow = std::min(height, (int)((camPos.y + SCREEN_HEIGHT/(2*zoom)) / tileSize) + 2);
    
    for (int y = startRow; y < endRow; y++) {
        for (int x = startCol; x < endCol; x++) {
            int tileId = GetTile(x, y);
            if (tileId == 0) continue;
            float px = float(x * tileSize), py = float(y * tileSize);
            int tRow = tileId / tileset.tilesPerRow, tCol = tileId % tileset.tilesPerRow;
            Rectangle src = {float(tCol*tileset.tileWidth), float(tRow*tileset.tileHeight),
                            float(tileset.tileWidth), float(tileset.tileHeight)};
            Rectangle dst = {px, py, float(tileSize), float(tileSize)};
            DrawTexturePro(tileset.texture, src, dst, {0,0}, 0, WHITE);
        }
    }
    for (auto& coin : collectibles) {
        if (coin.collected) continue;
        float bobY = sinf(coin.bobOffset) * 5;
        Color c = GOLD;
        if (coin.type == "gem") c = MAGENTA;
        else if (coin.type == "star") c = YELLOW;
        DrawCircle(coin.position.x, coin.position.y + bobY, 12, c);
        DrawCircle(coin.position.x-3, coin.position.y+bobY-3, 4, Fade(WHITE, 0.6f));
    }
    if (levelEnd.x > 0) {
        DrawRectangle((int)levelEnd.x-20, (int)levelEnd.y-60, 40, 80, Fade(GREEN, 0.3f));
        DrawRectangleLines((int)levelEnd.x-20, (int)levelEnd.y-60, 40, 80, GREEN);
        float fw = sinf(GetTime()*5)*5;
        Vector2 tri[3] = {{levelEnd.x, levelEnd.y-60}, {levelEnd.x+30+fw, levelEnd.y-50}, {levelEnd.x, levelEnd.y-40}};
        DrawTriangle(tri[0], tri[1], tri[2], RED);
    }
}

void TileMap::DrawBackground(const GameCamera2D& camera, const std::vector<Texture2D>& layers) {
    Vector2 camPos = camera.GetPosition();
    for (size_t i = 0; i < layers.size() && i < 4; i++) {
        float pf = 0.2f + i*0.2f, ox = camPos.x * pf;
        float tw = float(layers[i].width); ox = fmodf(ox, tw);
        DrawTexture(layers[i], (int)(-ox), 0, WHITE);
        DrawTexture(layers[i], (int)(tw - ox), 0, WHITE);
    }
}

int TileMap::GetTile(int x, int y) const {
    if (x < 0 || x >= width || y < 0 || y >= height) return 0;
    return tiles[y * 100 + x];
}
void TileMap::SetTile(int x, int y, int id) {
    if (x < 0 || x >= width || y < 0 || y >= height) return;
    tiles[y * 100 + x] = id;
}
Rectangle TileMap::GetTileBounds(int x, int y) const {
    return {float(x*tileSize), float(y*tileSize), float(tileSize), float(tileSize)};
}
bool TileMap::IsTileSolid(int x, int y) const {
    if (x < 0 || x >= width || y < 0 || y >= height) return false;
    int t = GetTile(x, y); return t != 0 && tileProperties[t].solid;
}
bool TileMap::IsTileDangerous(int x, int y) const {
    if (x < 0 || x >= width || y < 0 || y >= height) return false;
    return tileProperties[GetTile(x, y)].dangerous;
}
bool TileMap::IsTileOneWay(int x, int y) const {
    if (x < 0 || x >= width || y < 0 || y >= height) return false;
    return tileProperties[GetTile(x, y)].oneWay;
}
Rectangle TileMap::GetTileCollision(int x, int y) const { return GetTileBounds(x, y); }

std::vector<Rectangle> TileMap::GetTileCollisionsAround(Vector2 worldPos, int range) const {
    std::vector<Rectangle> r;
    int sx = (int)((worldPos.x - range*tileSize)/tileSize);
    int ex = (int)((worldPos.x + range*tileSize)/tileSize);
    int sy = (int)((worldPos.y - range*tileSize)/tileSize);
    int ey = (int)((worldPos.y + range*tileSize)/tileSize);
    for (int y = sy; y <= ey; y++) for (int x = sx; x <= ex; x++)
        if (IsTileSolid(x, y)) r.push_back(GetTileCollision(x, y));
    return r;
}
