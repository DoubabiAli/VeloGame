#include "Engine.h"
#include "../Graphics/TextureManager.h"
#include "../Objects/Player.h"
#include <cmath>

Engine* Engine::s_Instance = nullptr;

bool Engine::Init()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
    {
         SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
         return false;
    }
    if (IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG) != (IMG_INIT_JPG | IMG_INIT_PNG))
    {
        SDL_Log("Failed to initialize SDL_image: %s", IMG_GetError());
        SDL_Quit();
        return false;
    }

    m_Window = SDL_CreateWindow("Velo Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if (m_Window == nullptr)
    {
        SDL_Log("Failed to create Window: %s", SDL_GetError());
        IMG_Quit();
        SDL_Quit();
        return false;
    }

    m_Renderer = SDL_CreateRenderer(m_Window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (m_Renderer == nullptr)
    {
        SDL_Log("Failed to create Renderer: %s", SDL_GetError());
        SDL_DestroyWindow(m_Window);
        IMG_Quit();
        SDL_Quit();
        return false;
    }

    if (!TextureManager::GetInstance()->Load("background", "assets/Background1.png")) {
        SDL_Log("Error loading assets/Background1.png!");
        return false;
    }
    if (!TextureManager::GetInstance()->Load("track", "assets/Track.png")) {
        SDL_Log("Error loading assets/Track.png!");
        return false;
    }

    if (!TextureManager::GetInstance()->Load("player", "assets/player_bike.png")) {
        SDL_Log("Error loading assets/player_bike.png!");
        return false;
    }

    float laneHeight = TRACK_HEIGHT / 3.0f;
    m_laneYPositions.push_back(TRACK_Y_POSITION + laneHeight * 0.5f);
    m_laneYPositions.push_back(TRACK_Y_POSITION + laneHeight * 1.5f);
    m_laneYPositions.push_back(TRACK_Y_POSITION + laneHeight * 2.5f);

    m_Player = new Player();
    float playerStartX = 150.0f;
    if (!m_Player->load("player", playerStartX, m_laneYPositions)) {
         SDL_Log("Failed to load player data!");
         return false;
    }

    m_lastTick = SDL_GetTicks();
    m_deltaTime = 0.0f;
    m_BackgroundScrollX = 0.0f;

    m_IsRunning = true;
    SDL_Log("Engine initialization successful!");
    return true;
}

void Engine::Update()
{

    Uint32 currentTick = SDL_GetTicks();
    m_deltaTime = (currentTick - m_lastTick) / 1000.0f;
    m_lastTick = currentTick;

    if (m_deltaTime > 0.05f) {
       m_deltaTime = 0.05f;
    }

    if (m_Player) {
        m_Player->update(m_deltaTime);
    }

    if (m_Player) {
        float playerSpeed = m_Player->getSpeed();
        float scrollAmount = playerSpeed * m_deltaTime;
        m_BackgroundScrollX -= scrollAmount;
        m_BackgroundScrollX = fmod(m_BackgroundScrollX, static_cast<float>(SCREEN_WIDTH));
    }
}

void Engine::Render()
{
    SDL_SetRenderDrawColor(m_Renderer, 100, 150, 200, 255);
    SDL_RenderClear(m_Renderer);

    int bgScrollInt = static_cast<int>(m_BackgroundScrollX);
    TextureManager::GetInstance()->Draw("background", bgScrollInt, 0, SCREEN_WIDTH, 400);
    TextureManager::GetInstance()->Draw("background", bgScrollInt + SCREEN_WIDTH, 0, SCREEN_WIDTH, 400);

    TextureManager::GetInstance()->Draw("track", 0, static_cast<int>(TRACK_Y_POSITION), SCREEN_WIDTH, static_cast<int>(TRACK_HEIGHT));

    if (m_Player) {
        m_Player->draw();
    }

    SDL_RenderPresent(m_Renderer);
}

void Engine::Events()
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type)
        {
            case SDL_QUIT:
                Quit();
                break;

            case SDL_KEYDOWN:
            case SDL_KEYUP:
                if (m_Player) {
                    m_Player->handleEvent(event);
                }
                break;

            default:
                break;
        }
    }
}

bool Engine::Clean()
{
    SDL_Log("Cleaning Engine...");
    TextureManager::GetInstance()->Clean();

    if (m_Player) {
        delete m_Player;
        m_Player = nullptr;
    }

    SDL_DestroyRenderer(m_Renderer);
    SDL_DestroyWindow(m_Window);
    m_Renderer = nullptr;
    m_Window = nullptr;

    IMG_Quit();
    SDL_Quit();
    return true;
}

void Engine::Quit()
{
    m_IsRunning = false;
    SDL_Log("Engine Quitting...");
}
