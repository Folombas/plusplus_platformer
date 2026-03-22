#include "Game.h"
#include <cstdlib>
#include <iostream>

int main(int argc, char* argv[]) {
    std::cout << "Starting PlusPlus Platformer..." << std::endl;
    
    // Create and initialize game
    Game game;

    std::cout << "Initializing game..." << std::endl;
    if (!game.Initialize()) {
        std::cout << "Initialization failed!" << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Running game loop..." << std::endl;
    // Run game loop
    game.Run();

    std::cout << "Shutting down..." << std::endl;
    // Cleanup
    game.Shutdown();

    return EXIT_SUCCESS;
}
