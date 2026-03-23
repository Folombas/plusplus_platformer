#pragma once

#include "Common.h"
#include "Player.h"
#include "TileMap.h"
#include "Renderer.h"
#include "Audio.h"
#include "Input.h"
#include <GLFW/glfw3.h>

class Game {
public:
    Game();
    ~Game();
    
    bool init();
    void run();
    void shutdown();
    
private:
    GLFWwindow* m_window;
    bool m_running;
    bool m_paused;
    
    Player m_player;
    TileMap m_tileMap;
    
    float m_deltaTime;
    float m_lastFrameTime;
    
    // HUD текстуры
    int m_heartFullTexture;
    int m_heartEmptyTexture;
    
    bool initGLFW();
    bool initGLEW();
    void initGame();
    void handleInput();
    void update();
    void render();
    void renderHUD();
    
    void onPlayerDeath();
    void respawnPlayer();
};
