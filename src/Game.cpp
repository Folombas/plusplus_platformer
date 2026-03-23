#include "Game.h"
#include "Texture.h"
#include "Animation.h"
#include "Renderer.h"
#include "Audio.h"
#include "Input.h"
#include <iostream>
#include <cstdlib>

Game::Game()
    : m_window(nullptr)
    , m_running(false)
    , m_paused(false)
    , m_deltaTime(0.0f)
    , m_lastFrameTime(0.0f)
    , m_heartFullTexture(-1)
    , m_heartEmptyTexture(-1)
{}

Game::~Game() {
    shutdown();
}

bool Game::initGLFW() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    
    m_window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Platformer", nullptr, nullptr);
    if (!m_window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    
    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1); // VSync
    
    return true;
}

bool Game::initGLEW() {
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW: " << glewGetErrorString(err) << std::endl;
        return false;
    }
    
    // GLEW иногда генерирует ошибку при инициализации, игнорируем её
    glGetError();
    
    return true;
}

void Game::initGame() {
    // Инициализация менеджеров
    Renderer::getInstance().init();
    
    // Инициализация звука (может не удалась без WAV файлов)
    if (AudioManager::getInstance().init()) {
        // Загружаем звуки (только WAV)
        auto& audioMgr = AudioManager::getInstance();
        // Примечание: текущие звуки в OGG формате, нужны WAV
        // audioMgr.loadSound("jump", "assets/audio/jump.wav");
        // audioMgr.loadSound("hit", "assets/audio/hit.wav");
        std::cout << "Audio initialized (WAV files required for sound effects)" << std::endl;
    } else {
        std::cout << "Audio not available (OpenAL not initialized)" << std::endl;
    }
    
    Input::getInstance().init(m_window);
    
    // Загружаем HUD текстуры
    auto& texMgr = TextureManager::getInstance();
    m_heartFullTexture = texMgr.loadTexture("heart_full", "assets/Base pack/HUD/hud_heartFull.png");
    m_heartEmptyTexture = texMgr.loadTexture("heart_empty", "assets/Base pack/HUD/hud_heartEmpty.png");
    
    // Загружаем уровень
    std::string levelPath = "assets/level.txt";
    std::ifstream levelFile(levelPath);
    if (levelFile.good()) {
        m_tileMap.loadFromFile(levelPath);
    } else {
        // Генерируем уровень процедурно
        m_tileMap.generateLevel(50, 12);
    }
    
    // Инициализируем игрока
    m_player.loadAnimations();
    glm::vec2 spawn = m_tileMap.getPlayerSpawn();
    if (spawn.x != 0 || spawn.y != 0) {
        m_player.setPosition(spawn.x, spawn.y);
    } else {
        m_player.setPosition(100, 400);
    }
}

bool Game::init() {
    if (!initGLFW()) return false;
    if (!initGLEW()) return false;
    
    initGame();
    
    m_lastFrameTime = static_cast<float>(glfwGetTime());
    m_running = true;
    
    return true;
}

void Game::handleInput() {
    Input& input = Input::getInstance();
    
    // Закрытие игры
    if (glfwWindowShouldClose(m_window) || input.isKeyPressed(GLFW_KEY_ESCAPE)) {
        m_running = false;
    }
    
    // Пауза
    if (input.isKeyPressed(GLFW_KEY_P)) {
        m_paused = !m_paused;
    }
    
    if (m_paused) return;
    
    // Движение
    if (input.isKeyDown(GLFW_KEY_A) || input.isKeyDown(GLFW_KEY_LEFT)) {
        m_player.moveLeft();
    } else if (input.isKeyDown(GLFW_KEY_D) || input.isKeyDown(GLFW_KEY_RIGHT)) {
        m_player.moveRight();
    } else {
        m_player.stopHorizontal();
    }
    
    // Прыжок
    if (input.isKeyDown(GLFW_KEY_SPACE) || input.isKeyDown(GLFW_KEY_W) || input.isKeyDown(GLFW_KEY_UP)) {
        m_player.jump();
    }
    
    // Приседание
    m_player.setCrouching(input.isKeyDown(GLFW_KEY_S) || input.isKeyDown(GLFW_KEY_DOWN));
    
    // Рестарт
    if (input.isKeyPressed(GLFW_KEY_R)) {
        respawnPlayer();
    }
}

void Game::update() {
    float currentTime = static_cast<float>(glfwGetTime());
    m_deltaTime = currentTime - m_lastFrameTime;
    m_lastFrameTime = currentTime;
    
    // Ограничиваем delta time
    if (m_deltaTime > 0.1f) m_deltaTime = 0.1f;
    
    Input::getInstance().update();
    handleInput();
    
    if (m_paused) return;
    
    m_player.update(m_deltaTime);
    m_tileMap.update(m_deltaTime);
    
    // Коллизии с тайлами
    m_tileMap.checkCollisions(m_player);
    
    // Сбор предметов
    m_tileMap.checkCollectibles(m_player);
    
    // Проверка смерти от падения
    if (m_player.getPosition().y > WINDOW_HEIGHT + 100) {
        m_player.kill();
    }
    
    // Смерть игрока
    if (m_player.getState() == PlayerState::Dead) {
        onPlayerDeath();
    }
    
    // Камера следует за игроком
    float cameraX = m_player.getPosition().x - WINDOW_WIDTH / 2.0f;
    float cameraY = m_player.getPosition().y - WINDOW_HEIGHT / 2.0f;
    
    cameraX = Utils::clamp(cameraX, 0.0f, m_tileMap.getWidth() * TILE_SIZE - WINDOW_WIDTH);
    cameraY = Utils::clamp(cameraY, 0.0f, m_tileMap.getHeight() * TILE_SIZE - WINDOW_HEIGHT);
    
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, 0.0f));
    Renderer::getInstance().setCamera(view);
}

void Game::render() {
    Renderer::getInstance().beginFrame();
    
    m_tileMap.render();
    m_player.render();
    
    if (!m_paused) {
        renderHUD();
    } else {
        // Рендерим текст паузы
        Renderer::getInstance().drawText("PAUSED", WINDOW_WIDTH / 2 - 50, WINDOW_HEIGHT / 2, 30, glm::vec3(1.0f));
    }
    
    Renderer::getInstance().endFrame();
    
    glfwSwapBuffers(m_window);
}

void Game::renderHUD() {
    auto& renderer = Renderer::getInstance();
    
    // Сердца (здоровье)
    int maxHealth = m_player.getMaxHealth();
    int health = m_player.getHealth();
    
    for (int i = 0; i < maxHealth; i++) {
        float x = 20 + i * 45;
        float y = 20;
        
        int texId = (i < health) ? m_heartFullTexture : m_heartEmptyTexture;
        if (texId >= 0) {
            renderer.drawSprite(texId, x, y, 40, 40);
        }
    }
    
    // Монеты
    std::string coinsText = "Coins: " + std::to_string(m_player.getCoins());
    renderer.drawText(coinsText, WINDOW_WIDTH - 150, 20, 24, glm::vec3(1.0f, 0.84f, 0.0f));
    
    // Очки
    std::string scoreText = "Score: " + std::to_string(m_player.getScore());
    renderer.drawText(scoreText, WINDOW_WIDTH - 150, 50, 24, glm::vec3(1.0f));
}

void Game::onPlayerDeath() {
    // Небольшая задержка перед рестартом
    static float deathTimer = 0.0f;
    deathTimer += m_deltaTime;
    
    if (deathTimer > 2.0f) {
        respawnPlayer();
        deathTimer = 0.0f;
    }
}

void Game::respawnPlayer() {
    glm::vec2 spawn = m_tileMap.getPlayerSpawn();
    if (spawn.x != 0 || spawn.y != 0) {
        m_player.respawn(spawn);
    } else {
        m_player.respawn(glm::vec2(100, 400));
    }
}

void Game::run() {
    while (m_running) {
        glfwPollEvents();
        update();
        render();
    }
}

void Game::shutdown() {
    Renderer::getInstance().shutdown();
    AudioManager::getInstance().shutdown();
    
    if (m_window) {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }
    
    glfwTerminate();
}
