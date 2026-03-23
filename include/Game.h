#pragma once

#include "Common.h"
#include "Player.h"
#include "Level.h"
#include "Camera2D.h"
#include "ParticleSystem.h"
#include "AchievementManager.h"
#include <raylib.h>
#include <vector>
#include <string>

struct GameConfig {
    bool fullscreen;
    bool vsync;
    float masterVolume;
    float musicVolume;
    bool showFPS;
    bool screenShake;
    bool particles;
};

class Game {
public:
    Game();
    ~Game();
    
    bool Initialize();
    void Run();
    void Shutdown();
    
private:
    // Main loop
    void Update();
    void Draw();
    void FixedUpdate(float fixedDt);
    
    // State management
    void ChangeState(GameState newState);
    void UpdateGameState();
    
    // Menu
    void UpdateMenu();
    void DrawMenu();
    void DrawMainMenu();
    void DrawPauseMenu();
    void DrawLevelSelect();
    
    // Gameplay
    void UpdateGameplay();
    void DrawGameplay();
    void DrawHUD();
    void DrawLevelComplete();
    void DrawVictory();
    void DrawGameOver();
    
    // Transitions
    void StartTransition(const std::string& levelName);
    void UpdateTransition();
    void DrawTransition();
    
    // Level management
    bool LoadLevel(const std::string& filename);
    void UnloadCurrentLevel();
    void NextLevel();
    void RestartLevel();
    
    // Assets
    void LoadAssets();
    void UnloadAssets();
    void LoadFonts();
    void LoadTextures();
    
    // Input
    void HandleInput();
    void HandleDebugInput();
    
    // Visual effects
    void DrawBackground();
    void DrawForeground();
    void DrawParticles();
    void DrawDamageNumbers();
    void DrawScreenEffects();
    void DrawDebugInfo();
    
    // Save/Load
    void SaveGame();
    void LoadGame();
    void SaveSettings();
    void LoadSettings();
    
    // Game state
    GameState state;
    GameState previousState;
    
    // Player
    Player player;
    
    // Level
    Level currentLevel;
    std::vector<std::string> levelOrder;
    int currentLevelIndex;
    
    // Camera
    GameCamera2D camera;
    
    // Particles
    ParticleSystem particleSystem;
    
    // Screen effects
    ScreenShake screenShake;
    float transitionAlpha;
    bool transitioning;
    std::string transitionTarget;
    
    // Damage numbers
    std::vector<DamageNumber> damageNumbers;
    
    // Assets
    Font mainFont;
    Font titleFont;
    Font uiFont;
    std::vector<Texture2D> parallaxLayers;
    Texture2D playerTexture;
    Texture2D enemyTextures[4];
    Texture2D tilesetTexture;
    Texture2D uiTexture;
    Texture2D panelTexture;
    
    // Config
    GameConfig config;
    
    // Timing
    float accumulator;
    float fixedTimeStep;
    float deltaTime;
    
    // UI
    int selectedMenuItem;
    int selectedLevel;
    float menuTimer;
    bool showDebugInfo;
    
    // Stats
    int totalPlayTime;
    int deaths;
    int coinsCollected;
};
