#ifndef MAINMENU_H
#define MAINMENU_H

#include "SDL.h"
#include "../Core/Engine.h"

class MainMenu {
public:
    static MainMenu* GetInstance() {
        return s_Instance = (s_Instance != nullptr) ? s_Instance : new MainMenu();
    }

    bool Init();
    void HandleEvent(SDL_Event& event);
    void Update(float deltaTime);

    void Render();
    void Clean();

private:
    MainMenu() {}
    static MainMenu* s_Instance;

    SDL_Rect m_playButtonRect = {250, 250, 300, 80};
    SDL_Rect m_aboutButtonRect = {260, 350, 280, 80};
    SDL_Rect m_quitButtonRect = {250, 450, 290, 80};
    bool m_playHovered, m_aboutHovered, m_quitHovered;
    bool m_playClicked, m_aboutClicked, m_quitClicked;
    float m_playScale = 1.0f;
    float m_aboutScale = 1.0f;
    float m_quitScale = 1.0f;
    float m_hoverScale = 1.2f;



};

#endif
