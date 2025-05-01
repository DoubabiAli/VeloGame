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
    MainMenu() :
        m_playHovered(false), m_aboutHovered(false), m_quitHovered(false),
        m_playClicked(false), m_aboutClicked(false), m_quitClicked(false),
        m_volDownHovered(false), m_volUpHovered(false), m_muteToggleHovered(false),
        m_playScale(1.0f), m_aboutScale(1.0f), m_quitScale(1.0f),
        m_volDownScale(1.0f), m_volUpScale(1.0f), m_muteToggleScale(1.0f),
        m_hoverScale(1.1f)
    {}
    static MainMenu* s_Instance;

    SDL_Rect m_playButtonRect = {250, 250, 300, 80};
    SDL_Rect m_aboutButtonRect = {260, 350, 280, 80};
    SDL_Rect m_quitButtonRect = {250, 450, 290, 80};
    bool m_playHovered, m_aboutHovered, m_quitHovered;
    bool m_playClicked, m_aboutClicked, m_quitClicked;
    float m_playScale;
    float m_aboutScale;
    float m_quitScale;

    SDL_Rect m_volDownRect = { SCREEN_WIDTH - 160, SCREEN_HEIGHT - 60, 40, 40 };
    SDL_Rect m_muteToggleRect = { SCREEN_WIDTH - 110, SCREEN_HEIGHT - 60, 40, 40 };
    SDL_Rect m_volUpRect = { SCREEN_WIDTH - 60, SCREEN_HEIGHT - 60, 40, 40 };
    bool m_volDownHovered, m_volUpHovered, m_muteToggleHovered;
    float m_volDownScale;
    float m_volUpScale;
    float m_muteToggleScale;

    float m_hoverScale;

};

#endif
