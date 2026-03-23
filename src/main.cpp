#include "Game.h"
#include <iostream>

int main(int argc, char* argv[]) {
    std::cout << "Starting Platformer..." << std::endl;
    
    Game game;
    
    if (!game.init()) {
        std::cerr << "Failed to initialize game!" << std::endl;
        return 1;
    }
    
    std::cout << "Game initialized successfully!" << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "  A/Left - Move Left" << std::endl;
    std::cout << "  D/Right - Move Right" << std::endl;
    std::cout << "  Space/W/Up - Jump" << std::endl;
    std::cout << "  S/Down - Crouch" << std::endl;
    std::cout << "  P - Pause" << std::endl;
    std::cout << "  R - Respawn" << std::endl;
    std::cout << "  ESC - Quit" << std::endl;
    
    game.run();
    
    return 0;
}
