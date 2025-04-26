#ifndef OBSTACLE_H
#define OBSTACLE_H

#include <SDL_rect.h>
#include <string>

struct Obstacle {
    SDL_Rect collider;
    bool isActive;
    std::string textureId;

    Obstacle() : isActive(true), textureId("") {}
};

#endif // OBSTACLE_H
