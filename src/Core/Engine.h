#ifndef ENGINE_H
#define ENGINE_H

#include <vector>
#include <string>
#include "SDL.h"
#include "SDL_image.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

class Player;

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

private:
    Engine() : m_IsRunning(false), m_Window(nullptr), m_Renderer(nullptr), m_Player(nullptr), m_lastTick(0), m_deltaTime(0.0f), m_BackgroundScrollX(0.0f) {}

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
};

#endif // ENGINE_H
