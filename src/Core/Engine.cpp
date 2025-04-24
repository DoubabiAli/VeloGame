#include "Engine.h"
#include "../Graphics/TextureManager.h"
#include "../Objects/Player.h"
#include <cmath>
#include <iomanip>
#include <sstream>
#include <string>

Engine* Engine::s_Instance = nullptr;

bool Engine::Init()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
         SDL_Log("Failed to initialize SDL: %s", SDL_GetError()); return false; }
    if (IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG) != (IMG_INIT_JPG | IMG_INIT_PNG)) {
        SDL_Log("Failed to initialize SDL_image: %s", IMG_GetError()); SDL_Quit(); return false; }

    m_Window = SDL_CreateWindow("Velo Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if (m_Window == nullptr) {
        SDL_Log("Failed to create Window: %s", SDL_GetError()); IMG_Quit(); SDL_Quit(); return false; }

    m_Renderer = SDL_CreateRenderer(m_Window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (m_Renderer == nullptr) {
        SDL_Log("Failed to create Renderer: %s", SDL_GetError()); SDL_DestroyWindow(m_Window); IMG_Quit(); SDL_Quit(); return false; }

    if (!TextureManager::GetInstance()->Load("background", "assets/Background1.png")) { SDL_Log("Error loading assets/Background1.png!"); return false; }
    if (!TextureManager::GetInstance()->Load("track", "assets/Track.png")) { SDL_Log("Error loading assets/Track.png!"); return false; }
    if (!TextureManager::GetInstance()->Load("player", "assets/player_bike.png")) { SDL_Log("Error loading assets/player_bike.png!"); return false; }
    if (!TextureManager::GetInstance()->Load("start", "assets/timer/start.png")) { SDL_Log("Erreur : Impossible de charger start.png"); return false; }
    m_timerTextures.push_back("start");
    for(int i = 0; i <= 60; i++) {
        std::ostringstream ss; ss << std::setw(2) << std::setfill('0') << i; std::string timerName = ss.str(); std::string path = "assets/timer/" + timerName + ".png";
        if(!TextureManager::GetInstance()->Load(timerName, path)) { SDL_Log("Erreur : Impossible de charger %s", path.c_str()); return false; }
        m_timerTextures.push_back(timerName);
    }
    if (!TextureManager::GetInstance()->Load("end", "assets/timer/end.png")) { SDL_Log("Erreur : Impossible de charger end.png"); return false; }
    m_timerTextures.push_back("end");
    if (!TextureManager::GetInstance()->Load("gameover", "assets/game_over.png")) {
        SDL_Log("Erreur : Impossible de charger assets/game_over.png");
        TextureManager::GetInstance()->Clean(); SDL_DestroyRenderer(m_Renderer); SDL_DestroyWindow(m_Window); IMG_Quit(); SDL_Quit(); return false;
    }

    float laneHeight = TRACK_HEIGHT / 3.0f;
    m_laneYPositions.push_back(TRACK_Y_POSITION + laneHeight * 0.5f); m_laneYPositions.push_back(TRACK_Y_POSITION + laneHeight * 1.5f); m_laneYPositions.push_back(TRACK_Y_POSITION + laneHeight * 2.5f);
    m_Player = new Player(); float playerStartX = 150.0f;
    if (!m_Player || !m_Player->load("player", playerStartX, m_laneYPositions)) {
        SDL_Log("Failed to create or load player data!"); if(m_Player) delete m_Player; m_Player = nullptr;
        TextureManager::GetInstance()->Clean(); SDL_DestroyRenderer(m_Renderer); SDL_DestroyWindow(m_Window); IMG_Quit(); SDL_Quit(); return false;
    }
    m_lastTick = SDL_GetTicks(); m_deltaTime = 0.0f; m_BackgroundScrollX = 0.0f;
    m_IsRunning = true; m_gameState = STATE_START_SCREEN; m_remainingSeconds = 60; m_lastSecondUpdate = SDL_GetTicks();
    m_gameOverStartTime = 0;
    m_showGameOverScreen = false;

    SDL_Log("Engine initialization successful! Press RIGHT ARROW to start...");
    return true;
}

void Engine::Update()
{
    Uint32 currentTick = SDL_GetTicks();
    m_deltaTime = (currentTick - m_lastTick) / 1000.0f;
    m_lastTick = currentTick;
    if (m_deltaTime > 0.05f) { m_deltaTime = 0.05f; }

    if (m_gameState == STATE_PLAYING)
    {
        if (m_Player) { m_Player->update(m_deltaTime); }
        if (m_Player) { float playerSpeed = m_Player->getSpeed(); float scrollAmount = playerSpeed * m_deltaTime; m_BackgroundScrollX -= scrollAmount; if (m_BackgroundScrollX <= -SCREEN_WIDTH) { m_BackgroundScrollX += SCREEN_WIDTH; } }

        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - m_lastSecondUpdate >= 1000 && m_remainingSeconds > 0) {
            m_remainingSeconds--;
            m_lastSecondUpdate = currentTime;
            if (m_remainingSeconds == 0) {
                SDL_Log("TIME'S UP! Entering Game Over sequence...");
                m_gameState = STATE_GAME_OVER;
                m_gameOverStartTime = SDL_GetTicks();
                m_showGameOverScreen = false;
            }
        }
    }
    else if (m_gameState == STATE_GAME_OVER)
    {
        if (!m_showGameOverScreen) {
             Uint32 currentTime = SDL_GetTicks();
             if (currentTime - m_gameOverStartTime >= 2000) {
                 SDL_Log("Game Over delay finished. Displaying Game Over screen.");
                 m_showGameOverScreen = true;
             }
        }
    }
}

void Engine::Render()
{
    if (m_gameState == STATE_GAME_OVER)
    {
        if (m_showGameOverScreen)
        {
            SDL_SetRenderDrawColor(m_Renderer, 0, 0, 0, 255);
            SDL_RenderClear(m_Renderer);
            int imgW = 0, imgH = 0;
            if (TextureManager::GetInstance()->QueryTexture("gameover", &imgW, &imgH)) {
                int imgX = (SCREEN_WIDTH - imgW) / 2;
                int imgY = (SCREEN_HEIGHT - imgH) / 2;
                TextureManager::GetInstance()->Draw("gameover", imgX, imgY, imgW, imgH);
            } else {
                SDL_Log("Warning: Could not find 'gameover' texture to render.");
            }
        }
        else
        {
            SDL_SetRenderDrawColor(m_Renderer, 100, 150, 200, 255);
            SDL_RenderClear(m_Renderer);

            int bgScrollInt = static_cast<int>(m_BackgroundScrollX);
            TextureManager::GetInstance()->Draw("background", bgScrollInt, 0, SCREEN_WIDTH, 400);
            TextureManager::GetInstance()->Draw("background", bgScrollInt + SCREEN_WIDTH, 0, SCREEN_WIDTH, 400);
            TextureManager::GetInstance()->Draw("track", 0, static_cast<int>(TRACK_Y_POSITION), SCREEN_WIDTH, static_cast<int>(TRACK_HEIGHT));
            if (m_Player) { m_Player->draw(); }

             if (TextureManager::GetInstance()->QueryTexture("end", nullptr, nullptr)) {
                 TextureManager::GetInstance()->Draw("end", m_timerRect.x, m_timerRect.y, m_timerRect.w, m_timerRect.h);
             } else if (TextureManager::GetInstance()->QueryTexture("00", nullptr, nullptr)) {
                 TextureManager::GetInstance()->Draw("00", m_timerRect.x, m_timerRect.y, m_timerRect.w, m_timerRect.h);
             }
        }
    }
    else
    {
        SDL_SetRenderDrawColor(m_Renderer, 100, 150, 200, 255);
        SDL_RenderClear(m_Renderer);

        int bgScrollInt = static_cast<int>(m_BackgroundScrollX);
        TextureManager::GetInstance()->Draw("background", bgScrollInt, 0, SCREEN_WIDTH, 400);
        TextureManager::GetInstance()->Draw("background", bgScrollInt + SCREEN_WIDTH, 0, SCREEN_WIDTH, 400);
        TextureManager::GetInstance()->Draw("track", 0, static_cast<int>(TRACK_Y_POSITION), SCREEN_WIDTH, static_cast<int>(TRACK_HEIGHT));

        if (m_gameState == STATE_START_SCREEN) {
            if (m_Player) { m_Player->draw(); }
            if (TextureManager::GetInstance()->QueryTexture("start", nullptr, nullptr)) {
                TextureManager::GetInstance()->Draw("start", m_timerRect.x, m_timerRect.y, m_timerRect.w, m_timerRect.h);
            }
        } else if (m_gameState == STATE_PLAYING) {
            if (m_Player) { m_Player->draw(); }
            std::string currentTimerTextureId = "end";
            if (m_remainingSeconds > 0 && m_remainingSeconds <= 60) { std::ostringstream ss; ss << std::setw(2) << std::setfill('0') << m_remainingSeconds; currentTimerTextureId = ss.str(); }
            if (TextureManager::GetInstance()->QueryTexture(currentTimerTextureId, nullptr, nullptr)) {
                 TextureManager::GetInstance()->Draw(currentTimerTextureId, m_timerRect.x, m_timerRect.y, m_timerRect.w, m_timerRect.h);
            }
        }
    }

    SDL_RenderPresent(m_Renderer);
}

void Engine::Events()
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) { Quit(); return; }

        switch (m_gameState) {
            case STATE_START_SCREEN:
                if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RIGHT) {
                    SDL_Log("RIGHT ARROW pressed! Changing state to PLAYING.");
                    m_gameState = STATE_PLAYING;
                    m_lastSecondUpdate = SDL_GetTicks();
                    m_remainingSeconds = 60;
                    m_gameOverStartTime = 0;
                    m_showGameOverScreen = false;
                } break;
            case STATE_PLAYING:
                if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) { if (m_Player && event.key.repeat == 0) { m_Player->handleEvent(event); } } break;
            case STATE_GAME_OVER:
                if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_r) {
                     SDL_Log("Restarting game...");
                     m_gameState = STATE_START_SCREEN;
                     m_BackgroundScrollX = 0.0f;
                     m_remainingSeconds = 60;
                     m_gameOverStartTime = 0;
                     m_showGameOverScreen = false;
                     if(m_Player) { m_Player->reset(150.0f, m_laneYPositions); }
                } break;
        }
    }
}

bool Engine::Clean()
{
    SDL_Log("Cleaning Engine...");
    TextureManager::GetInstance()->Clean();
    if (m_Player) { delete m_Player; m_Player = nullptr; }
    SDL_DestroyRenderer(m_Renderer);
    SDL_DestroyWindow(m_Window);
    m_Renderer = nullptr; m_Window = nullptr;
    m_timerTextures.clear();
    IMG_Quit();
    SDL_Quit();
    return true;
}

void Engine::Quit()
{
    m_IsRunning = false;
    SDL_Log("Engine Quitting...");
}
