#include "Game.h"
#include "AudioManager.h"
#include "AchievementManager.h"
#include "ParticleSystem.h"
#include <cstdlib>
#include <ctime>

#ifdef _WIN32
#undef CloseWindow
#undef ShowCursor
#endif

Game::Game()
    : state(GameState::MENU)
    , previousState(GameState::MENU)
    , currentLevelIndex(0)
    , transitionAlpha(0)
    , transitioning(false)
    , accumulator(0)
    , fixedTimeStep(1.0f / 60.0f)
    , deltaTime(0)
    , selectedMenuItem(0)
    , selectedLevel(0)
    , menuTimer(0)
    , showDebugInfo(false)
    , totalPlayTime(0)
    , deaths(0)
    , coinsCollected(0)
{
    // Initialize random seed
    srand(static_cast<unsigned int>(time(nullptr)));
    
    // Set up level order
    levelOrder = {
        "assets/levels/level1.txt",
        "assets/levels/level2.txt",
        "assets/levels/level3.txt"
    };
    
    // Config defaults
    config.fullscreen = false;
    config.vsync = true;
    config.masterVolume = 1.0f;
    config.musicVolume = 0.5f;
    config.showFPS = true;
    config.screenShake = true;
    config.particles = true;
}

Game::~Game() {
    Shutdown();
}

bool Game::Initialize() {
    // Initialize window
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "++ Platformer - Adventure Quest");
    SetTargetFPS(60);
    
    // Initialize audio
    AudioManager::Instance().Initialize();
    
    // Initialize achievement system
    AchievementManager::Instance().Initialize();
    
    // Load assets
    LoadAssets();
    
    // Load settings
    LoadSettings();
    
    // Start with menu music
    AudioManager::Instance().PlayMusic(MusicType::MENU);
    
    return true;
}

void Game::LoadAssets() {
    // Load fonts - try multiple fonts for different purposes
    const char* mainFontPaths[] = {
        "assets/fonts/arena-font/Arena-rvwaK.ttf",
        "assets/fonts/super-kindly-font/SuperKindly-drE8E.ttf",
        "assets/fonts/super-shiny-font/SuperShiny-0v0rG.ttf",
        "assets/fonts/super-adorable-font/SuperAdorable-MAvyp.ttf",
        "assets/fonts/SuperFeel-JpZqa.ttf",
        "assets/fonts/plaza-font/Plaza.ttf",
        "resources/fonts/arial.ttf"
    };

    for (const char* path : mainFontPaths) {
        if (FileExists(path)) {
            mainFont = LoadFontEx(path, 48, 0, 0);
            if (mainFont.glyphs != nullptr) {
                TraceLog(LOG_INFO, "Loaded main font: %s", path);
                break;
            }
        }
    }

    if (mainFont.glyphs == nullptr) {
        mainFont = GetFontDefault();
    }

    // Load title font (larger, more decorative)
    const char* titleFontPaths[] = {
        "assets/fonts/70s-disco-font/70s Disco.ttf",
        "assets/fonts/arena-font/Arena-rvwaK.ttf",
        "assets/fonts/super-shiny-font/SuperShiny-0v0rG.ttf"
    };

    for (const char* path : titleFontPaths) {
        if (FileExists(path)) {
            titleFont = LoadFontEx(path, 72, 0, 0);
            if (titleFont.glyphs != nullptr) {
                TraceLog(LOG_INFO, "Loaded title font: %s", path);
                break;
            }
        }
    }

    if (titleFont.glyphs == nullptr) {
        titleFont = mainFont;
    }

    uiFont = mainFont;

    // Load UI textures from new UI packs
    const char* uiTexturePaths[] = {
        "assets/ui/kenney_ui-pack-pixel-adventure/PNG/Default/Border/panel-border-001.png",
        "assets/ui/kenney_ui-pack-adventure/PNG/Default/Border/panel-border-001.png",
        "assets/ui/kenney_ui-pack/PNG/Default/Border/panel-border-001.png",
        "assets/sprites/platformer/Base pack/HUD/hud_heartFull.png",
        "assets/sprites/platformer/Base pack/HUD/hudHeart_full.png"
    };

    for (const char* path : uiTexturePaths) {
        if (FileExists(path)) {
            uiTexture = LoadTexture(path);
            if (uiTexture.id != 0) {
                TraceLog(LOG_INFO, "Loaded UI texture: %s", path);
                break;
            }
        }
    }

    // Load additional UI elements (buttons, panels)
    const char* panelPaths[] = {
        "assets/ui/kenney_ui-pack-pixel-adventure/PNG/Default/Panel/panel-001.png",
        "assets/ui/kenney_fantasy-ui-borders/PNG/Default/Panel/panel-001.png"
    };

    for (const char* path : panelPaths) {
        if (FileExists(path)) {
            panelTexture = LoadTexture(path);
            if (panelTexture.id != 0) {
                TraceLog(LOG_INFO, "Loaded panel texture: %s", path);
                break;
            }
        }
    }

    // Load parallax backgrounds - try multiple packs
    const char* bgPaths[][5] = {
        // Forest pack
        {
            "assets/parallax/forest/parallax_forest_pack/layers/parallax-forest-back-trees.png",
            "assets/parallax/forest/parallax_forest_pack/layers/parallax-forest-middle-trees.png",
            "assets/parallax/forest/parallax_forest_pack/layers/parallax-forest-lights.png",
            "assets/parallax/forest/parallax_forest_pack/layers/parallax-forest-front-trees.png",
            nullptr
        },
        // Mountain pack
        {
            "assets/sprites/parallax_mountain_pack/layers/parallax-mountain-back.png",
            "assets/sprites/parallax_mountain_pack/layers/parallax-mountain-middle.png",
            "assets/sprites/parallax_mountain_pack/layers/parallax-mountain-front.png",
            nullptr,
            nullptr
        }
    };

    for (const auto& pack : bgPaths) {
        for (const char* path : pack) {
            if (path == nullptr) break;
            if (FileExists(path)) {
                Texture2D tex = LoadTexture(path);
                if (tex.id != 0) {
                    parallaxLayers.push_back(tex);
                    TraceLog(LOG_INFO, "Loaded parallax layer: %s", path);
                }
            }
        }
        if (!parallaxLayers.empty()) break;
    }
}

void Game::UnloadAssets() {
    if (mainFont.glyphs != nullptr) {
        UnloadFont(mainFont);
    }
    if (titleFont.glyphs != nullptr && titleFont.id != mainFont.id) {
        UnloadFont(titleFont);
    }
    if (uiTexture.id != 0) {
        UnloadTexture(uiTexture);
    }
    if (panelTexture.id != 0) {
        UnloadTexture(panelTexture);
    }
    for (auto& tex : parallaxLayers) {
        if (tex.id != 0) {
            UnloadTexture(tex);
        }
    }
    parallaxLayers.clear();
}

void Game::Run() {
    // Main game loop
    while (!WindowShouldClose()) {
        deltaTime = GetFrameTime();
        
        Update();
        
        // Fixed timestep for physics
        accumulator += deltaTime;
        while (accumulator >= fixedTimeStep) {
            FixedUpdate(fixedTimeStep);
            accumulator -= fixedTimeStep;
        }
        
        Draw();
    }
}

void Game::Update() {
    HandleInput();
    
    // Update audio
    AudioManager::Instance().Update();
    
    // Update achievements
    AchievementManager::Instance().Update(deltaTime);
    
    // Update transition
    if (transitioning) {
        UpdateTransition();
    }
    
    // Update state
    UpdateGameState();
}

void Game::FixedUpdate(float fixedDt) {
    if (state == GameState::PLAYING) {
        // Update level
        currentLevel.Update(fixedDt, player);
        
        // Update camera
        camera.SetTarget(player.GetPosition());
        camera.Update(fixedDt);
        
        // Update particles
        particleSystem.Update(fixedDt);
    }
}

void Game::Draw() {
    BeginDrawing();

    ClearBackground({20, 20, 35});

    switch (state) {
        case GameState::MENU:
            DrawMenu();
            break;
        case GameState::PLAYING:
            DrawGameplay();
            break;
        case GameState::PAUSED:
            DrawGameplay();
            DrawPauseMenu();
            break;
        case GameState::LEVEL_COMPLETE:
            DrawLevelComplete();
            break;
        case GameState::GAME_OVER:
            DrawGameOver();
            break;
        case GameState::VICTORY:
            DrawVictory();
            break;
    }

    // Draw FPS if enabled
    if (config.showFPS) {
        DrawFPS(10, SCREEN_HEIGHT - 30);
    }

    EndDrawing();
}

void Game::UpdateGameState() {
    switch (state) {
        case GameState::MENU:
            UpdateMenu();
            break;
        case GameState::PLAYING:
            UpdateGameplay();
            break;
        case GameState::PAUSED:
            // Nothing to update
            break;
        case GameState::LEVEL_COMPLETE:
            // Wait for input
            break;
        case GameState::GAME_OVER:
            // Wait for input
            break;
        case GameState::VICTORY:
            // Victory!
            break;
    }
}

void Game::HandleInput() {
    // Global shortcuts
    if (IsKeyPressed(KEY_ESCAPE)) {
        if (state == GameState::PLAYING) {
            ChangeState(GameState::PAUSED);
        } else if (state == GameState::PAUSED) {
            ChangeState(GameState::PLAYING);
        } else if (state == GameState::MENU) {
            // Quit confirmation could go here
        }
    }
    
    // Debug input
    HandleDebugInput();
}

void Game::HandleDebugInput() {
    if (IsKeyPressed(KEY_F1)) {
        showDebugInfo = !showDebugInfo;
    }
    
    if (IsKeyPressed(KEY_R) && (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL))) {
        RestartLevel();
    }
    
    // God mode
    if (IsKeyPressed(KEY_G) && state == GameState::PLAYING) {
        // Toggle god mode (could be implemented)
    }
}

void Game::ChangeState(GameState newState) {
    previousState = state;
    state = newState;
    
    switch (newState) {
        case GameState::MENU:
            AudioManager::Instance().PlayMusic(MusicType::MENU);
            break;
        case GameState::PLAYING:
            // Music handled by level
            break;
        case GameState::PAUSED:
            AudioManager::Instance().PauseMusic();
            break;
        case GameState::LEVEL_COMPLETE:
            AudioManager::Instance().PlaySound(SoundType::LEVEL_COMPLETE);
            break;
        case GameState::GAME_OVER:
            // Game over sound
            break;
        case GameState::VICTORY:
            AudioManager::Instance().PlayMusic(MusicType::VICTORY);
            break;
    }
}

void Game::UpdateMenu() {
    menuTimer += deltaTime;
    
    // Menu input handled in DrawMenu
}

void Game::DrawMenu() {
    // Animated background
    float time = GetTime();
    DrawGradientV(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
                  Color{30, 30, 60, 255},
                  Color{10, 10, 30, 255});

    // Draw some particles
    for (int i = 0; i < 50; i++) {
        float x = (sin(time * 0.5f + i) * 0.5f + 0.5f) * SCREEN_WIDTH;
        float y = (cos(time * 0.3f + i * 0.5f) * 0.5f + 0.5f) * SCREEN_HEIGHT;
        float size = sin(time + i) * 3 + 5;
        DrawCircle(x, y, size, TintAlpha(WHITE, 0.3f));
    }

    // Draw decorative UI panels in background
    if (panelTexture.id != 0) {
        DrawTexturePro(panelTexture, 
                       {0, 0, static_cast<float>(panelTexture.width), static_cast<float>(panelTexture.height)},
                       {100, 100, 200, 150},
                       {0, 0}, 0, TintAlpha(WHITE, 0.1f));
        DrawTexturePro(panelTexture,
                       {0, 0, static_cast<float>(panelTexture.width), static_cast<float>(panelTexture.height)},
                       {SCREEN_WIDTH - 300, SCREEN_HEIGHT - 200, 250, 180},
                       {0, 0}, 0, TintAlpha(WHITE, 0.1f));
    }

    // Title with decorative font
    const char* title = "++ PLATFORMER";
    int titleSize = MeasureTextEx(titleFont, title, 72, 2).x;
    DrawTextEx(titleFont, title,
               {static_cast<float>((SCREEN_WIDTH - titleSize) / 2), 100.0f},
               72.0f, 2, GOLD);

    const char* subtitle = "Adventure Quest";
    int subSize = MeasureTextEx(mainFont, subtitle, 36, 2).x;
    DrawTextEx(mainFont, subtitle,
               {static_cast<float>((SCREEN_WIDTH - subSize) / 2), 180.0f},
               36.0f, 2, SKYBLUE);

    // Menu options with panel backgrounds
    const char* options[] = {"START GAME", "LEVEL SELECT", "OPTIONS", "QUIT"};
    int numOptions = 4;

    int startY = 280;
    int spacing = 65;

    for (int i = 0; i < numOptions; i++) {
        Color color = (i == selectedMenuItem) ? GOLD : WHITE;
        float scale = (i == selectedMenuItem) ? 1.15f : 1.0f;
        int fontSize = static_cast<int>(36 * scale);

        int textWidth = MeasureTextEx(mainFont, options[i], static_cast<float>(fontSize), 2).x;
        float textX = static_cast<float>((SCREEN_WIDTH - textWidth) / 2);
        float textY = static_cast<float>(startY + i * spacing);

        // Draw panel background for selected item
        if (i == selectedMenuItem && panelTexture.id != 0) {
            DrawTexturePro(panelTexture,
                           {0, 0, static_cast<float>(panelTexture.width), static_cast<float>(panelTexture.height)},
                           {textX - 30, textY - 8, static_cast<float>(textWidth + 60), 50},
                           {0, 0}, 0, TintAlpha(WHITE, 0.3f));
        }

        DrawTextEx(mainFont, options[i],
                   {textX, textY},
                   static_cast<float>(fontSize), 2, color);
    }

    // Menu navigation
    if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) {
        selectedMenuItem = (selectedMenuItem - 1 + numOptions) % numOptions;
        AudioManager::Instance().PlaySound(SoundType::MENU_SELECT);
    }
    if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) {
        selectedMenuItem = (selectedMenuItem + 1) % numOptions;
        AudioManager::Instance().PlaySound(SoundType::MENU_SELECT);
    }

    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
        AudioManager::Instance().PlaySound(SoundType::MENU_BACK);

        switch (selectedMenuItem) {
            case 0: // Start Game
                StartTransition(levelOrder[0]);
                break;
            case 1: // Level Select
                // Could implement level select screen
                break;
            case 2: // Options
                // Could implement options menu
                break;
            case 3: // Quit
                WindowShouldClose();
                break;
        }
    }

    // Credits
    DrawText("Use Arrow Keys/WASD to move, Space to jump",
             SCREEN_WIDTH / 2 - 200, SCREEN_HEIGHT - 80, 16, LIGHTGRAY);
    DrawText("Press ESC to pause", 
             SCREEN_WIDTH / 2 - 80, SCREEN_HEIGHT - 50, 16, LIGHTGRAY);
}

void Game::UpdateGameplay() {
    // Check for player death
    if (player.IsDead()) {
        deaths++;
        AchievementManager::Instance().OnPlayerDeath();
        
        // Show death particles
        particleSystem.EmitDeathExplosion(player.GetPosition());
        
        // Restart after delay
        RestartLevel();
    }
    
    // Check for level complete
    if (currentLevel.IsComplete()) {
        Vector2 levelEnd = currentLevel.GetTileMap().GetLevelEnd();
        float dist = Vector2Distance(player.GetPosition(), levelEnd);
        
        if (dist < 50) {
            // Level complete!
            ChangeState(GameState::LEVEL_COMPLETE);
            
            // Next level after delay
            // For now, just restart
        }
    }
}

void Game::DrawGameplay() {
    // Draw background
    currentLevel.DrawBackground(camera);
    
    // Start camera mode
    camera.BeginDraw();
    
    // Apply screen shake
    if (config.screenShake && screenShake.active) {
        Vector2 shake = screenShake.Update(deltaTime);
        // Already applied in camera
    }
    
    // Draw level
    currentLevel.Draw(camera);
    
    // Draw player
    player.Draw(camera);
    
    // Draw particles
    particleSystem.Draw(camera);
    
    // End camera mode
    camera.EndDraw();
    
    // Draw HUD
    DrawHUD();
    
    // Draw debug info
    if (showDebugInfo) {
        this->DrawDebugInfo();
    }
}

void Game::DrawHUD() {
    // Health bar
    int healthX = 30;
    int healthY = 30;
    
    DrawText("HEALTH", healthX, healthY - 20, 20, WHITE);
    
    for (int i = 0; i < player.GetHealth(); i++) {
        DrawRectangle(healthX + i * 45, healthY, 40, 40, RED);
        DrawRectangleLines(healthX + i * 45, healthY, 40, 40, WHITE);
        
        // Heart icon placeholder
        DrawCircle(healthX + i * 45 + 20, healthY + 20, 15, RED);
    }
    
    // Coins
    DrawText(TextFormat("COINS: %d", player.GetCoins()), 
             healthX, healthY + 80, 24, GOLD);
    
    // Score
    DrawText(TextFormat("SCORE: %d", player.GetScore()),
             SCREEN_WIDTH - 200, 30, 24, WHITE);
    
    // Level name
    DrawText(currentLevel.GetName().c_str(),
             SCREEN_WIDTH / 2 - 100, 30, 24, WHITE);
    
    // Time
    if (currentLevel.HasTimeLimit()) {
        float timeLeft = currentLevel.GetTimeLimit() - currentLevel.GetTimeElapsed();
        Color timeColor = timeLeft < 10 ? RED : WHITE;
        DrawText(TextFormat("TIME: %.1f", timeLeft),
                 SCREEN_WIDTH / 2 - 50, 60, 24, timeColor);
    }
    
    // Power-up indicator
    if (player.GetActivePowerUp() != PowerUpType::NONE) {
        const char* powerUpNames[] = {"NONE", "SPEED", "INVINCIBLE", "DOUBLE JUMP", "SHIELD"};
        int idx = static_cast<int>(player.GetActivePowerUp());
        
        Color powerColor = WHITE;
        switch (player.GetActivePowerUp()) {
            case PowerUpType::SPEED_BOOST: powerColor = YELLOW; break;
            case PowerUpType::INVINCIBILITY: powerColor = GOLD; break;
            case PowerUpType::DOUBLE_JUMP: powerColor = PURPLE; break;
            case PowerUpType::SHIELD: powerColor = SKYBLUE; break;
            default: break;
        }
        
        DrawText(TextFormat("%s: %.1f", powerUpNames[idx], player.GetPowerUpTimer()),
                 SCREEN_WIDTH / 2 - 80, 90, 20, powerColor);
    }
}

void Game::DrawPauseMenu() {
    // Semi-transparent overlay
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, TintAlpha(BLACK, 0.7f));
    
    const char* title = "PAUSED";
    int titleSize = MeasureTextEx(mainFont, title, 72, 2).x;
    DrawTextEx(mainFont, title,
               {static_cast<float>((SCREEN_WIDTH - titleSize) / 2), 200.0f},
               72.0f, 2, WHITE);
    
    const char* options[] = {"RESUME", "RESTART LEVEL", "QUIT TO MENU"};
    int numOptions = 3;
    
    // Simple pause menu (no navigation for now)
    for (int i = 0; i < numOptions; i++) {
        float fontSize = 32.0f;
        int textWidth = MeasureTextEx(mainFont, options[i], fontSize, 2).x;
        Color color = WHITE;

        // Check hover
        float y = 320.0f + i * 60.0f;
        Rectangle btnRect = {static_cast<float>((SCREEN_WIDTH - textWidth) / 2 - 20), y - 10, 
                            static_cast<float>(textWidth + 40), 50.0f};
        
        Vector2 mousePos = GetMousePosition();
        if (CheckCollisionPointRec(mousePos, btnRect)) {
            color = GOLD;
            
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                AudioManager::Instance().PlaySound(SoundType::MENU_BACK);
                
                switch (i) {
                    case 0:
                        ChangeState(GameState::PLAYING);
                        break;
                    case 1:
                        RestartLevel();
                        ChangeState(GameState::PLAYING);
                        break;
                    case 2:
                        ChangeState(GameState::MENU);
                        break;
                }
            }
        }
        
        DrawTextEx(mainFont, options[i],
                   {static_cast<float>((SCREEN_WIDTH - textWidth) / 2), y},
                   fontSize, 2, color);
    }
}

void Game::DrawLevelComplete() {
    // Semi-transparent overlay
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, TintAlpha(BLACK, 0.8f));
    
    const char* title = "LEVEL COMPLETE!";
    int titleSize = MeasureTextEx(mainFont, title, 64, 2).x;
    DrawTextEx(mainFont, title,
               {static_cast<float>((SCREEN_WIDTH - titleSize) / 2), 200.0f},
               64.0f, 2, GOLD);

    // Stats
    DrawText(TextFormat("Time: %.2f seconds", currentLevel.GetTimeElapsed()),
             SCREEN_WIDTH / 2 - 100, 300, 28, WHITE);
    DrawText(TextFormat("Coins: %d/%d", currentLevel.GetCoinsCollected(), 
                        currentLevel.GetTotalCoins()),
             SCREEN_WIDTH / 2 - 100, 340, 28, WHITE);
    DrawText(TextFormat("Enemies Defeated: %d", currentLevel.GetEnemiesDefeated()),
             SCREEN_WIDTH / 2 - 100, 380, 28, WHITE);
    
    // Continue prompt
    const char* prompt = "Press SPACE to continue";
    int promptSize = MeasureTextEx(mainFont, prompt, 28, 2).x;
    DrawTextEx(mainFont, prompt,
               {static_cast<float>((SCREEN_WIDTH - promptSize) / 2), 450.0f},
               28.0f, 2, WHITE);
    
    if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
        NextLevel();
    }
}

void Game::DrawGameOver() {
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, TintAlpha(BLACK, 0.9f));
    
    const char* title = "GAME OVER";
    int titleSize = MeasureTextEx(mainFont, title, 72, 2).x;
    DrawTextEx(mainFont, title,
               {static_cast<float>((SCREEN_WIDTH - titleSize) / 2), 250.0f},
               72.0f, 2, RED);

    const char* prompt = "Press R to restart";
    int promptSize = MeasureTextEx(mainFont, prompt, 32, 2).x;
    DrawTextEx(mainFont, prompt,
               {static_cast<float>((SCREEN_WIDTH - promptSize) / 2), 400.0f},
               32.0f, 2, WHITE);
    
    if (IsKeyPressed(KEY_R)) {
        RestartLevel();
    }
}

void Game::DrawVictory() {
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, TintAlpha(BLACK, 0.9f));
    
    const char* title = "VICTORY!";
    int titleSize = MeasureTextEx(mainFont, title, 72, 2).x;
    DrawTextEx(mainFont, title,
               {static_cast<float>((SCREEN_WIDTH - titleSize) / 2), 200.0f},
               72.0f, 2, GOLD);
    
    const char* subtitle = "You have completed all levels!";
    int subSize = MeasureTextEx(mainFont, subtitle, 32, 2).x;
    DrawTextEx(mainFont, subtitle,
               {static_cast<float>((SCREEN_WIDTH - subSize) / 2), 300.0f},
               32.0f, 2, WHITE);
    
    // Stats
    DrawText(TextFormat("Total Deaths: %d", deaths),
             SCREEN_WIDTH / 2 - 80, 380, 28, WHITE);
    DrawText(TextFormat("Total Coins: %d", coinsCollected),
             SCREEN_WIDTH / 2 - 80, 420, 28, WHITE);
    
    const char* prompt = "Press SPACE to return to menu";
    int promptSize = MeasureTextEx(mainFont, prompt, 28, 2).x;
    DrawTextEx(mainFont, prompt,
               {static_cast<float>((SCREEN_WIDTH - promptSize) / 2), 500.0f},
               28.0f, 2, WHITE);
    
    if (IsKeyPressed(KEY_SPACE)) {
        ChangeState(GameState::MENU);
    }
}

void Game::StartTransition(const std::string& levelName) {
    transitionTarget = levelName;
    transitioning = true;
    transitionAlpha = 0;
}

void Game::UpdateTransition() {
    transitionAlpha += deltaTime * 2;
    
    if (transitionAlpha >= 1) {
        transitionAlpha = 1;
        
        // Load the level
        if (!transitionTarget.empty()) {
            LoadLevel(transitionTarget);
            transitionTarget = "";
        }
        
        // Fade out
        transitionAlpha = 1 - transitionAlpha;
        
        if (transitionAlpha <= 0) {
            transitioning = false;
            transitionAlpha = 0;
            ChangeState(GameState::PLAYING);
        }
    }
}

void Game::DrawTransition() {
    if (transitioning || transitionAlpha > 0) {
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 
                      TintAlpha(BLACK, transitionAlpha));
    }
}

bool Game::LoadLevel(const std::string& filename) {
    // Create level directory if it doesn't exist
    if (!DirectoryExists("assets/levels")) {
        MakeDirectory("assets/levels");
    }
    
    if (!currentLevel.Load(filename)) {
        // Load failed, create default
        currentLevel.Load("");
    }
    
    // Reset player
    Vector2 spawn = currentLevel.GetTileMap().GetSpawnPoint();
    player.Reset(spawn);
    player.SetSpawnPoint(spawn);
    
    // Setup camera
    camera.SetTarget(player.GetPosition());
    camera.SetBounds({0, 0, 
                      static_cast<float>(currentLevel.GetTileMap().GetWidth() * TILE_SIZE),
                      static_cast<float>(currentLevel.GetTileMap().GetHeight() * TILE_SIZE)});
    
    return true;
}

void Game::UnloadCurrentLevel() {
    // Cleanup current level resources
}

void Game::NextLevel() {
    currentLevelIndex++;
    
    if (currentLevelIndex >= static_cast<int>(levelOrder.size())) {
        // All levels complete!
        ChangeState(GameState::VICTORY);
        return;
    }
    
    StartTransition(levelOrder[currentLevelIndex]);
}

void Game::RestartLevel() {
    player.Reset(currentLevel.GetTileMap().GetSpawnPoint());
    currentLevel.Reset();
    
    camera.SetTarget(player.GetPosition());
    particleSystem.Clear();
    
    ChangeState(GameState::PLAYING);
}

void Game::DrawDebugInfo() {
    int y = 100;
    DrawText(TextFormat("Player Pos: %.1f, %.1f", player.GetPosition().x, player.GetPosition().y), 10, y, 16, LIME); y += 20;
    DrawText(TextFormat("Grounded: %s", player.IsGrounded() ? "Yes" : "No"), 10, y, 16, LIME); y += 20;
    DrawText(TextFormat("Particles: %d", particleSystem.GetActiveParticleCount()), 10, y, 16, LIME); y += 20;
    DrawText(TextFormat("FPS: %d", GetFPS()), 10, y, 16, LIME);
}

void Game::SaveGame() {
    // Save game state to file
}

void Game::LoadGame() {
    // Load game state from file
}

void Game::SaveSettings() {
    // Save settings to config file
}

void Game::LoadSettings() {
    // Load settings from config file
}

void Game::Shutdown() {
    UnloadAssets();
    AudioManager::Instance().Shutdown();
    CloseWindow();
}
