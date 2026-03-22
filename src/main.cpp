#include "Game.h"
#include <cstdlib>

int main(int argc, char* argv[]) {
    // Create and initialize game
    Game game;
    
    if (!game.Initialize()) {
        return EXIT_FAILURE;
    }
    
    // Run game loop
    game.Run();
    
    // Cleanup
    game.Shutdown();
    
    return EXIT_SUCCESS;
}
