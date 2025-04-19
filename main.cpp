#include "src/Core/Engine.h"
#include <SDL.h>

int main(int argc, char** argv)
{
    SDL_Log("Starting Game...");

    if (!Engine::GetInstance()->Init()) {
        SDL_Log("Engine initialization failed!");
        return -1;
    }

    SDL_Log("Entering Main Loop...");
    while(Engine::GetInstance()->IsRunning())
    {
        Engine::GetInstance()->Events();
        Engine::GetInstance()->Update();
        Engine::GetInstance()->Render();
    }
    SDL_Log("Exited Main Loop.");

    Engine::GetInstance()->Clean();
    SDL_Log("Game Shut Down Gracefully.");
    return 0;
}
