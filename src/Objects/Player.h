#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <vector>
#include <SDL.h>

class Player {
public:
    Player();

    bool load(std::string textureId, float startX, const std::vector<float>& laneYPositions);
    void handleEvent(const SDL_Event& event);
    void update(float deltaTime);
    void draw();
    float getSpeed() const;
    void reset(float startX, const std::vector<float>& laneYPositions);
    void ApplySpeedPenalty();
    SDL_Rect GetCollider() const;

    void IncreaseMaxSpeed(float amount, float absoluteMax);


private:
    float m_x;
    float m_currentY;
    float m_targetY;
    int m_currentLane;
    int m_numLanes;
    std::vector<float> m_laneYPositions;
    float m_laneChangeAnimSpeed;
    float m_speed;
    float m_acceleration;
    float m_braking;
    float m_drag;
    float m_maxSpeed;
    float m_minSpeed;
    std::string m_textureId;
    int m_width;
    int m_height;

    bool m_isSlowed;
    Uint32 m_slowedStartTime;
    const float m_penaltySpeed = 50.0f;
    const Uint32 m_penaltyDuration = 2500;

    float m_initialMaxSpeed;


    void setLane(int laneIndex);
};

#endif // PLAYER_H
