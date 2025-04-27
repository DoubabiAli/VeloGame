#ifndef ENGINE_H
#define ENGINE_H

#include <vector>
#include <string>
#include <random>
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "Obstacle.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

class Player;
enum GameState {
    STATE_MAIN_MENU,
    STATE_START_SCREEN,
    STATE_PLAYING,
    STATE_GAME_OVER,
    STATE_ABOUT,
    STATE_WIN
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
    inline void SetGameState(GameState state) { m_gameState = state; }

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
        m_showGameOverScreen(false),
        m_obstacleSpawnInterval(2.3f),
        m_timeSinceLastSpawn(0.0f),
        m_minSpawnInterval(0.55f),
        m_lastDifficultyIncreaseTime(0),
        m_difficultyIncreaseInterval(6000),
        m_spawnIntervalReduction(0.18f),
        m_obstacleTextureWidth(0),
        m_obstacleTextureHeight(0),
        m_rng(std::random_device{}()),
        m_lastMaxSpeedIncreaseTime(0),
        m_maxSpeedIncreaseInterval(1000),
        m_maxSpeedIncreaseAmount(20.0f),
        m_absoluteMaxPlayerSpeed(2000.0f),
        m_doubleSpawnChance(40),
        m_totalDistanceTraveled(0.0f),
        m_uiFont(nullptr),
        m_distanceTexture(nullptr),
        m_lastDisplayedDistance(-1)
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

    std::vector<Obstacle> m_obstacles;
    float m_obstacleSpawnInterval;
    float m_timeSinceLastSpawn;
    float m_minSpawnInterval;
    Uint32 m_lastDifficultyIncreaseTime;
    const Uint32 m_difficultyIncreaseInterval;
    float m_spawnIntervalReduction;
    int m_obstacleTextureWidth;
    int m_obstacleTextureHeight;
    std::vector<std::string> m_obstacleTextureIds;
    std::mt19937 m_rng;

    Uint32 m_lastMaxSpeedIncreaseTime;
    const Uint32 m_maxSpeedIncreaseInterval;
    const float m_maxSpeedIncreaseAmount;
    const float m_absoluteMaxPlayerSpeed;

    const int m_doubleSpawnChance;

    float m_totalDistanceTraveled;
    const float WIN_DISTANCE = 35000.0f;

    TTF_Font* m_uiFont;
    SDL_Rect m_distanceRect = { 15, 15, 0, 0 };
    SDL_Texture* m_distanceTexture;
    int m_lastDisplayedDistance;


    void SpawnObstacle();
};

#endif // ENGINE_H
