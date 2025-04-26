#include <algorithm>
#include <cmath>
#include <iomanip>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

#include "../Graphics/TextureManager.h"
#include "../Objects/Player.h"
#include "Engine.h"

Engine* Engine::s_Instance = nullptr;

bool Engine::Init() {
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
    SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
    return false;
  }
  if (IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG) != (IMG_INIT_JPG | IMG_INIT_PNG)) {
    SDL_Log("Failed to initialize SDL_image: %s", IMG_GetError());
    SDL_Quit();
    return false;
  }
  if (TTF_Init() == -1) {
    SDL_Log("Failed to initialize SDL_ttf: %s", TTF_GetError());
    IMG_Quit();
    SDL_Quit();
    return false;
  }

  m_Window = SDL_CreateWindow("Velo Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
  if (m_Window == nullptr) {
    SDL_Log("Failed to create Window: %s", SDL_GetError());
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    return false;
  }

  m_Renderer = SDL_CreateRenderer(m_Window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (m_Renderer == nullptr) {
    SDL_Log("Failed to create Renderer: %s", SDL_GetError());
    SDL_DestroyWindow(m_Window);
    TTF_Quit();
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
  if (!TextureManager::GetInstance()->Load("start", "assets/timer/start.png")) {
    SDL_Log("Erreur : Impossible de charger start.png");
    return false;
  }
  m_timerTextures.push_back("start");
  for (int i = 0; i <= 60; i++) {
    std::ostringstream ss;
    ss << std::setw(2) << std::setfill('0') << i;
    std::string timerName = ss.str();
    std::string path = "assets/timer/" + timerName + ".png";
    if (!TextureManager::GetInstance()->Load(timerName, path)) {
      SDL_Log("Erreur : Impossible de charger %s", path.c_str());
      return false;
    }
    m_timerTextures.push_back(timerName);
  }
  if (!TextureManager::GetInstance()->Load("end", "assets/timer/end.png")) {
    SDL_Log("Erreur : Impossible de charger end.png");
    return false;
  }
  m_timerTextures.push_back("end");
  if (!TextureManager::GetInstance()->Load("gameover", "assets/game_over.png")) {
    SDL_Log("Erreur : Impossible de charger assets/game_over.png");
    TextureManager::GetInstance()->Clean();
    SDL_DestroyRenderer(m_Renderer);
    SDL_DestroyWindow(m_Window);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    return false;
  }
  if (!TextureManager::GetInstance()->Load("win", "assets/win.png")) {
    SDL_Log("Erreur : Impossible de charger assets/win.png");
    TextureManager::GetInstance()->Clean();
    SDL_DestroyRenderer(m_Renderer);
    SDL_DestroyWindow(m_Window);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    return false;
  }

  m_obstacleTextureIds.clear();
  std::vector<std::pair<std::string, std::string>> obstaclesToLoad = {{"obstacle1", "assets/obstacle1.png"}, {"obstacle2", "assets/obstacle2.png"}, {"obstacle3", "assets/obstacle3.png"}, {"obstacle4", "assets/obstacle4.png"}};
  bool firstObstacleLoaded = false;
  for (const auto& obsData : obstaclesToLoad) {
    if (!TextureManager::GetInstance()->Load(obsData.first, obsData.second)) {
      continue;
    }
    m_obstacleTextureIds.push_back(obsData.first);
    if (!firstObstacleLoaded) {
      TextureManager::GetInstance()->QueryTexture(obsData.first, &m_obstacleTextureWidth, &m_obstacleTextureHeight);
      if (m_obstacleTextureWidth > 0) {
        firstObstacleLoaded = true;
      }
    }
  }
  if (m_obstacleTextureIds.empty() || !firstObstacleLoaded) {
    SDL_Log("Erreur critique: Aucun obstacle n'a pu être chargé ou leurs dimensions n'ont pu être déterminées.");
    TextureManager::GetInstance()->Clean();
    SDL_DestroyRenderer(m_Renderer);
    SDL_DestroyWindow(m_Window);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    return false;
  }

  m_uiFont = TTF_OpenFont("assets/PixelifySans-Regular.ttf", 24);
  if (m_uiFont == nullptr) {
    SDL_Log("Failed to load UI font: %s", TTF_GetError());
    TextureManager::GetInstance()->Clean();
    SDL_DestroyRenderer(m_Renderer);
    SDL_DestroyWindow(m_Window);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    return false;
  }

  float laneHeight = TRACK_HEIGHT / 3.0f;
  m_laneYPositions.push_back(TRACK_Y_POSITION + laneHeight * 0.5f);
  m_laneYPositions.push_back(TRACK_Y_POSITION + laneHeight * 1.5f);
  m_laneYPositions.push_back(TRACK_Y_POSITION + laneHeight * 2.5f);
  m_Player = new Player();
  float playerStartX = 150.0f;
  if (!m_Player || !m_Player->load("player", playerStartX, m_laneYPositions)) {
    SDL_Log("Failed to create or load player data!");
    if (m_Player) delete m_Player;
    m_Player = nullptr;
    TextureManager::GetInstance()->Clean();
    TTF_CloseFont(m_uiFont);
    SDL_DestroyRenderer(m_Renderer);
    SDL_DestroyWindow(m_Window);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    return false;
  }

  m_lastTick = SDL_GetTicks();
  m_deltaTime = 0.0f;
  m_BackgroundScrollX = 0.0f;
  m_IsRunning = true;
  m_gameState = STATE_START_SCREEN;
  m_remainingSeconds = 60;
  m_lastSecondUpdate = SDL_GetTicks();
  m_gameOverStartTime = 0;
  m_showGameOverScreen = false;
  m_totalDistanceTraveled = 0.0f;
  m_timeSinceLastSpawn = 1.0f;
  m_obstacleSpawnInterval = 2.3f;
  m_lastDifficultyIncreaseTime = SDL_GetTicks();
  m_lastMaxSpeedIncreaseTime = SDL_GetTicks();
  m_distanceTexture = nullptr;
  m_lastDisplayedDistance = -1;

  SDL_Log("Engine initialization successful! Press RIGHT ARROW to start...");
  return true;
}

void Engine::SpawnObstacle() {
  if (m_obstacleTextureIds.empty() || m_laneYPositions.empty() || m_obstacleTextureWidth <= 0 || m_laneYPositions.size() < 1) return;
  std::vector<int> availableLaneIndices(m_laneYPositions.size());
  std::iota(availableLaneIndices.begin(), availableLaneIndices.end(), 0);
  int nearThresholdX = SCREEN_WIDTH / 2;
  for (const auto& existingObstacle : m_obstacles) {
    if (existingObstacle.isActive && existingObstacle.collider.x > nearThresholdX) {
      float obsCenterY = existingObstacle.collider.y + existingObstacle.collider.h / 2.0f;
      for (size_t i = 0; i < m_laneYPositions.size(); ++i) {
        if (std::abs(obsCenterY - m_laneYPositions[i]) < (m_laneYPositions[1] - m_laneYPositions[0]) * 0.4f) {
          availableLaneIndices.erase(std::remove(availableLaneIndices.begin(), availableLaneIndices.end(), i), availableLaneIndices.end());
          break;
        }
      }
    }
  }
  if (availableLaneIndices.empty()) {
    return;
  }
  std::uniform_int_distribution<int> chanceDist(1, 100);
  bool spawnTwo = (availableLaneIndices.size() >= 2) && (chanceDist(m_rng) <= m_doubleSpawnChance);
  int obstaclesToSpawn = spawnTwo ? 2 : 1;
  std::uniform_int_distribution<int> textureDist(0, m_obstacleTextureIds.size() - 1);
  for (int i = 0; i < obstaclesToSpawn; ++i) {
    if (availableLaneIndices.empty()) break;
    std::uniform_int_distribution<int> availableLaneDist(0, availableLaneIndices.size() - 1);
    int chosenAvailableIndex = availableLaneDist(m_rng);
    int finalLaneIndex = availableLaneIndices[chosenAvailableIndex];
    Obstacle newObstacle;
    newObstacle.textureId = m_obstacleTextureIds[textureDist(m_rng)];
    newObstacle.collider.w = m_obstacleTextureWidth;
    newObstacle.collider.h = m_obstacleTextureHeight;
    newObstacle.collider.x = SCREEN_WIDTH + 50;
    newObstacle.collider.y = static_cast<int>(m_laneYPositions[finalLaneIndex] - m_obstacleTextureHeight / 2.0f);
    newObstacle.isActive = true;
    m_obstacles.push_back(newObstacle);
    availableLaneIndices.erase(availableLaneIndices.begin() + chosenAvailableIndex);
  }
}

void Engine::Update() {
  Uint32 currentTick = SDL_GetTicks();
  m_deltaTime = (currentTick - m_lastTick) / 1000.0f;
  m_lastTick = currentTick;
  if (m_deltaTime > 0.05f) {
    m_deltaTime = 0.05f;
  }

  if (m_gameState == STATE_PLAYING) {
    if (m_Player) {
      m_Player->update(m_deltaTime);
    }
    float playerSpeed = 0.0f;
    if (m_Player) {
      playerSpeed = m_Player->getSpeed();
    }
    float scrollAmount = playerSpeed * m_deltaTime;
    m_BackgroundScrollX -= scrollAmount;
    if (m_BackgroundScrollX <= -SCREEN_WIDTH) {
      m_BackgroundScrollX += SCREEN_WIDTH;
    }
    m_totalDistanceTraveled += playerSpeed * m_deltaTime;

    int currentDisplayedDistance = static_cast<int>(m_totalDistanceTraveled / 10.0f);  // Diviser par 10 pour la mise à jour
    if (currentDisplayedDistance != m_lastDisplayedDistance) {
      if (m_distanceTexture != nullptr) {
        SDL_DestroyTexture(m_distanceTexture);
        m_distanceTexture = nullptr;
      }
      std::stringstream ssDistance;
      ssDistance << "Distance: " << currentDisplayedDistance << " m / 3500 m";
      SDL_Color textColor = {255, 255, 255, 255};
      if (m_uiFont) {
        SDL_Surface* surface = TTF_RenderText_Solid(m_uiFont, ssDistance.str().c_str(), textColor);
        if (surface) {
          m_distanceTexture = SDL_CreateTextureFromSurface(m_Renderer, surface);
          if (m_distanceTexture) {
            SDL_QueryTexture(m_distanceTexture, NULL, NULL, &m_distanceRect.w, &m_distanceRect.h);
          } else {
            SDL_Log("Failed to create distance texture: %s", SDL_GetError());
          }
          SDL_FreeSurface(surface);
        } else {
          SDL_Log("Failed to create distance surface: %s", TTF_GetError());
        }
      }
      m_lastDisplayedDistance = currentDisplayedDistance;
    }

    if (currentTick - m_lastDifficultyIncreaseTime >= m_difficultyIncreaseInterval) {
      m_obstacleSpawnInterval -= m_spawnIntervalReduction;
      if (m_obstacleSpawnInterval < m_minSpawnInterval) {
        m_obstacleSpawnInterval = m_minSpawnInterval;
      }
      m_lastDifficultyIncreaseTime = currentTick;
      SDL_Log("Spawn Rate Increased! New interval: %.2f", m_obstacleSpawnInterval);
    }
    if (currentTick - m_lastMaxSpeedIncreaseTime >= m_maxSpeedIncreaseInterval) {
      if (m_Player) {
        m_Player->IncreaseMaxSpeed(m_maxSpeedIncreaseAmount, m_absoluteMaxPlayerSpeed);
      }
      m_lastMaxSpeedIncreaseTime = currentTick;
    }
    m_timeSinceLastSpawn += m_deltaTime;
    if (m_timeSinceLastSpawn >= m_obstacleSpawnInterval) {
      SpawnObstacle();
      m_timeSinceLastSpawn = 0.0f;
    }

    SDL_Rect playerFullCollider = m_Player->GetCollider();
    for (auto it = m_obstacles.begin(); it != m_obstacles.end();) {
      if (!it->isActive) {
        it++;
        continue;
      }
      it->collider.x -= static_cast<int>(scrollAmount);
      SDL_Rect obstacleFullCollider = it->collider;
      float reductionFactor = 0.6f;
      SDL_Rect playerCollisionBox;
      playerCollisionBox.w = static_cast<int>(playerFullCollider.w * reductionFactor);
      playerCollisionBox.h = static_cast<int>(playerFullCollider.h * reductionFactor);
      playerCollisionBox.x = playerFullCollider.x + (playerFullCollider.w - playerCollisionBox.w) / 2;
      playerCollisionBox.y = playerFullCollider.y + (playerFullCollider.h - playerCollisionBox.h) / 2;
      SDL_Rect obstacleCollisionBox;
      obstacleCollisionBox.w = static_cast<int>(obstacleFullCollider.w * reductionFactor);
      obstacleCollisionBox.h = static_cast<int>(obstacleFullCollider.h * reductionFactor);
      obstacleCollisionBox.x = obstacleFullCollider.x + (obstacleFullCollider.w - obstacleCollisionBox.w) / 2;
      obstacleCollisionBox.y = obstacleFullCollider.y + (obstacleFullCollider.h - obstacleCollisionBox.h) / 2;
      if (SDL_HasIntersection(&playerCollisionBox, &obstacleCollisionBox)) {
        SDL_Log("Collision detected!");
        m_Player->ApplySpeedPenalty();
        it->isActive = false;
      }
      if (it->collider.x + it->collider.w < 0) {
        it = m_obstacles.erase(it);
      } else {
        ++it;
      }
    }
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
    if (m_totalDistanceTraveled >= WIN_DISTANCE) {
      SDL_Log("WIN CONDITION MET! Distance: %.2f", m_totalDistanceTraveled);
      m_gameState = STATE_WIN;
    }
  } else if (m_gameState == STATE_GAME_OVER) {
    if (!m_showGameOverScreen) {
      Uint32 currentTime = SDL_GetTicks();
      if (currentTime - m_gameOverStartTime >= 2000) {
        SDL_Log("Game Over delay finished. Displaying Game Over screen.");
        m_showGameOverScreen = true;
      }
    }
  }
}

void Engine::Render() {
  if (m_gameState == STATE_GAME_OVER) {
    if (m_showGameOverScreen) {
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
    } else {
      SDL_SetRenderDrawColor(m_Renderer, 100, 150, 200, 255);
      SDL_RenderClear(m_Renderer);
      int bgScrollInt = static_cast<int>(m_BackgroundScrollX);
      TextureManager::GetInstance()->Draw("background", bgScrollInt, 0, SCREEN_WIDTH, 400);
      TextureManager::GetInstance()->Draw("background", bgScrollInt + SCREEN_WIDTH, 0, SCREEN_WIDTH, 400);
      TextureManager::GetInstance()->Draw("track", 0, static_cast<int>(TRACK_Y_POSITION), SCREEN_WIDTH, static_cast<int>(TRACK_HEIGHT));
      if (m_Player) {
        m_Player->draw();
      }
      for (const auto& obs : m_obstacles) {
        if (obs.isActive) {
          TextureManager::GetInstance()->Draw(obs.textureId, obs.collider.x, obs.collider.y, obs.collider.w, obs.collider.h);
        }
      }
      if (TextureManager::GetInstance()->QueryTexture("end", nullptr, nullptr)) {
        TextureManager::GetInstance()->Draw("end", m_timerRect.x, m_timerRect.y, m_timerRect.w, m_timerRect.h);
      } else if (TextureManager::GetInstance()->QueryTexture("00", nullptr, nullptr)) {
        TextureManager::GetInstance()->Draw("00", m_timerRect.x, m_timerRect.y, m_timerRect.w, m_timerRect.h);
      }
    }
  } else if (m_gameState == STATE_WIN) {
    SDL_SetRenderDrawColor(m_Renderer, 20, 20, 80, 255);
    SDL_RenderClear(m_Renderer);
    int imgW = 0, imgH = 0;
    if (TextureManager::GetInstance()->QueryTexture("win", &imgW, &imgH)) {
      int imgX = (SCREEN_WIDTH - imgW) / 2;
      int imgY = (SCREEN_HEIGHT - imgH) / 2;
      TextureManager::GetInstance()->Draw("win", imgX, imgY, imgW, imgH);
    } else {
      SDL_Log("Warning: Could not find 'win' texture to render.");
    }
  } else {
    SDL_SetRenderDrawColor(m_Renderer, 100, 150, 200, 255);
    SDL_RenderClear(m_Renderer);
    int bgScrollInt = static_cast<int>(m_BackgroundScrollX);
    TextureManager::GetInstance()->Draw("background", bgScrollInt, 0, SCREEN_WIDTH, 400);
    TextureManager::GetInstance()->Draw("background", bgScrollInt + SCREEN_WIDTH, 0, SCREEN_WIDTH, 400);
    TextureManager::GetInstance()->Draw("track", 0, static_cast<int>(TRACK_Y_POSITION), SCREEN_WIDTH, static_cast<int>(TRACK_HEIGHT));
    if (m_gameState == STATE_PLAYING) {
      for (const auto& obs : m_obstacles) {
        if (obs.isActive) {
          TextureManager::GetInstance()->Draw(obs.textureId, obs.collider.x, obs.collider.y, obs.collider.w, obs.collider.h);
        }
      }
    }
    if (m_Player) {
      m_Player->draw();
    }

    if (m_gameState == STATE_START_SCREEN) {
      if (TextureManager::GetInstance()->QueryTexture("start", nullptr, nullptr)) {
        TextureManager::GetInstance()->Draw("start", m_timerRect.x, m_timerRect.y, m_timerRect.w, m_timerRect.h);
      }
    } else if (m_gameState == STATE_PLAYING) {
      std::string currentTimerTextureId = "end";
      if (m_remainingSeconds > 0 && m_remainingSeconds <= 60) {
        std::ostringstream ss;
        ss << std::setw(2) << std::setfill('0') << m_remainingSeconds;
        currentTimerTextureId = ss.str();
      }
      if (TextureManager::GetInstance()->QueryTexture(currentTimerTextureId, nullptr, nullptr)) {
        TextureManager::GetInstance()->Draw(currentTimerTextureId, m_timerRect.x, m_timerRect.y, m_timerRect.w, m_timerRect.h);
      }
    }

    if (m_distanceTexture != nullptr && (m_gameState == STATE_PLAYING || m_gameState == STATE_START_SCREEN)) {
      SDL_RenderCopy(m_Renderer, m_distanceTexture, NULL, &m_distanceRect);
    }
  }
  SDL_RenderPresent(m_Renderer);
}

void Engine::Events() {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT) {
      Quit();
      return;
    }
    switch (m_gameState) {
      case STATE_START_SCREEN:
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RIGHT) {
          SDL_Log("RIGHT ARROW pressed! Changing state to PLAYING.");
          m_gameState = STATE_PLAYING;
          m_lastSecondUpdate = SDL_GetTicks();
          m_remainingSeconds = 60;
          m_gameOverStartTime = 0;
          m_showGameOverScreen = false;
          m_obstacles.clear();
          m_timeSinceLastSpawn = 1.0f;
          m_obstacleSpawnInterval = 2.3f;
          m_lastDifficultyIncreaseTime = SDL_GetTicks();
          m_lastMaxSpeedIncreaseTime = SDL_GetTicks();
          m_totalDistanceTraveled = 0.0f;
          m_lastDisplayedDistance = -1;
          if (m_distanceTexture) {
            SDL_DestroyTexture(m_distanceTexture);
            m_distanceTexture = nullptr;
          }
          if (m_Player) {
            m_Player->reset(150.0f, m_laneYPositions);
          }
        }
        break;
      case STATE_PLAYING:
        if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
          if (m_Player && event.key.repeat == 0) {
            m_Player->handleEvent(event);
          }
        }
        break;
      case STATE_GAME_OVER:
      case STATE_WIN:
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_r) {
          SDL_Log("Restarting game...");
          m_gameState = STATE_START_SCREEN;
          m_BackgroundScrollX = 0.0f;
          m_remainingSeconds = 60;
          m_gameOverStartTime = 0;
          m_showGameOverScreen = false;
          m_obstacles.clear();
          m_timeSinceLastSpawn = 1.0f;
          m_obstacleSpawnInterval = 2.3f;
          m_lastDifficultyIncreaseTime = SDL_GetTicks();
          m_lastMaxSpeedIncreaseTime = SDL_GetTicks();
          m_totalDistanceTraveled = 0.0f;
          m_lastDisplayedDistance = -1;
          if (m_distanceTexture) {
            SDL_DestroyTexture(m_distanceTexture);
            m_distanceTexture = nullptr;
          }
          if (m_Player) {
            m_Player->reset(150.0f, m_laneYPositions);
          }
        }
        break;
    }
  }
}

bool Engine::Clean() {
  SDL_Log("Cleaning Engine...");
  TextureManager::GetInstance()->Clean();
  m_obstacles.clear();
  m_obstacleTextureIds.clear();
  if (m_distanceTexture != nullptr) {
    SDL_DestroyTexture(m_distanceTexture);
    m_distanceTexture = nullptr;
  }
  if (m_uiFont != nullptr) {
    TTF_CloseFont(m_uiFont);
    m_uiFont = nullptr;
  }
  if (m_Player) {
    delete m_Player;
    m_Player = nullptr;
  }
  SDL_DestroyRenderer(m_Renderer);
  SDL_DestroyWindow(m_Window);
  m_Renderer = nullptr;
  m_Window = nullptr;
  m_timerTextures.clear();
  TTF_Quit();
  IMG_Quit();
  SDL_Quit();
  return true;
}

void Engine::Quit() {
  m_IsRunning = false;
  SDL_Log("Engine Quitting...");
}
