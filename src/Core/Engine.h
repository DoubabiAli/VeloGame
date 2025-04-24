#ifndef ENGINE_H
#define ENGINE_H

#include <vector>
#include <string>
#include "SDL.h"
#include "SDL_image.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

class Player;

enum GameState {
    STATE_START_SCREEN,
    STATE_PLAYING,
    STATE_GAME_OVER
};

class Engine
{
public:
    static Engine* GetInstance()
    {
        return s_Instance = (s_Instance != nullptr)? s_Instance : new Engine();
    }

    bool Init();
    bool Clean();
    void Quit();

    void Update();
    void Render();
    void Events();

    inline bool IsRunning() const { return m_IsRunning; }
    inline SDL_Renderer* GetRenderer() { return m_Renderer; }
    inline GameState GetGameState() const { return m_gameState; }

private:
    Engine() :
        m_IsRunning(false),
        m_Window(nullptr),
        m_Renderer(nullptr),
        m_Player(nullptr),
        m_lastTick(0),
        m_deltaTime(0.0f),
        m_BackgroundScrollX(0.0f),
        m_remainingSeconds(60),
        m_lastSecondUpdate(0),
        m_gameState(STATE_START_SCREEN),
        m_gameOverStartTime(0),
        m_showGameOverScreen(false)
    {}

    bool m_IsRunning;
    SDL_Window* m_Window;
    SDL_Renderer* m_Renderer;
    static Engine* s_Instance;
    Player* m_Player;
    Uint32 m_lastTick;
    float m_deltaTime;
    float m_BackgroundScrollX;
    std::vector<float> m_laneYPositions;
    const float TRACK_Y_POSITION = 400.0f;
    const float TRACK_HEIGHT = 200.0f;
    int m_remainingSeconds;
    Uint32 m_lastSecondUpdate;
    std::vector<std::string> m_timerTextures;
    SDL_Rect m_timerRect = {SCREEN_WIDTH - 120, 20, 100, 100};
    GameState m_gameState;

    Uint32 m_gameOverStartTime;
    bool m_showGameOverScreen;
};

#endif // ENGINE_H
