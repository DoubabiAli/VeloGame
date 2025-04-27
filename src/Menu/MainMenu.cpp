#include "MainMenu.h"
#include "../Core/Engine.h"
#include "../Graphics/TextureManager.h"
#include <SDL.h>

MainMenu* MainMenu::s_Instance = nullptr;

bool MainMenu::Init() {

    if (!TextureManager::GetInstance()->Load("menu_bg", "assets/Menu/menu_background.png")) {
        SDL_Log("ERREUR: menu_background.png non chargé!");
        return false;
    }

    if (!TextureManager::GetInstance()->Load("play_btn", "assets/Menu/btn_jouer.png") ||
        !TextureManager::GetInstance()->Load("play_btn_hover", "assets/Menu/btn_jouer_hover.png")) {
        SDL_Log("ERREUR: Textures bouton Jouer non chargées!");
        return false;
    }

    if (!TextureManager::GetInstance()->Load("about_btn", "assets/Menu/btn_a_propos.png") ||
        !TextureManager::GetInstance()->Load("about_btn_hover", "assets/Menu/btn_a_propos_hover.png")) {
        SDL_Log("ERREUR: Textures bouton À Propos non chargées!");
        return false;
    }
    if (!TextureManager::GetInstance()->Load("about_screen", "assets/Menu/about_background.png")) {
        SDL_Log("Erreur : Impossible de charger about_background.png");
        return false;
    }
    if (!TextureManager::GetInstance()->Load("quit_btn", "assets/Menu/btn_quitter.png") ||
        !TextureManager::GetInstance()->Load("quit_btn_hover", "assets/Menu/btn_quitter_hover.png")) {
        SDL_Log("ERREUR: Textures bouton Quitter non chargées!");
        return false;
    }
    SDL_Log("SUCCÈS: Toutes les textures du menu sont chargées!");
    return true;
}

void MainMenu::HandleEvent(SDL_Event& event) {
    int x, y;
    SDL_GetMouseState(&x, &y);
    SDL_Point mousePoint = {x, y};

    m_playHovered = SDL_PointInRect(&mousePoint, &m_playButtonRect);
    m_aboutHovered = SDL_PointInRect(&mousePoint, &m_aboutButtonRect);
    m_quitHovered = SDL_PointInRect(&mousePoint, &m_quitButtonRect);

    if (event.type == SDL_MOUSEBUTTONDOWN) {
        if (m_playHovered) {
            Engine::GetInstance()->SetGameState(STATE_START_SCREEN);
            SDL_Log("Clic sur Jouer!");
        }
        else if (m_aboutHovered) {
            Engine::GetInstance()->SetGameState(STATE_ABOUT);
            }
        else if (m_quitHovered) {
            Engine::GetInstance()->Quit();
        }
    }
}
void MainMenu::Update(float deltaTime) {

    if (m_playHovered) {
        m_playScale = std::min(m_playScale + deltaTime * 8.0f, m_hoverScale);
    } else {
        m_playScale = std::max(m_playScale - deltaTime * 8.0f, 1.0f);
    }

    if (m_aboutHovered) {
        m_aboutScale = std::min(m_aboutScale + deltaTime * 8.0f, m_hoverScale);
    } else {
        m_aboutScale = std::max(m_aboutScale - deltaTime * 8.0f, 1.0f);
    }

    if (m_quitHovered) {
        m_quitScale = std::min(m_quitScale + deltaTime * 8.0f, m_hoverScale);
    } else {
        m_quitScale = std::max(m_quitScale - deltaTime * 8.0f, 1.0f);
    }
}

void MainMenu::Render() {
    TextureManager::GetInstance()->Draw("menu_bg", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    auto drawButton = [](const std::string& baseID, const SDL_Rect& rect, bool isHovered, float scale) {
        std::string textureID = baseID + (isHovered ? "_hover" : "");
        int scaledW = static_cast<int>(rect.w * scale);
        int scaledH = static_cast<int>(rect.h * scale);
        int posX = rect.x + (rect.w - scaledW) / 2;
        int posY = rect.y + (rect.h - scaledH) / 2;
        TextureManager::GetInstance()->Draw(textureID, posX, posY, scaledW, scaledH);
    };

    drawButton("play_btn", m_playButtonRect, m_playHovered, m_playScale);
    drawButton("about_btn", m_aboutButtonRect, m_aboutHovered, m_aboutScale);
    drawButton("quit_btn", m_quitButtonRect, m_quitHovered, m_quitScale);
}
void MainMenu::Clean() {

    TextureManager::GetInstance()->Drop("menu_bg");
    TextureManager::GetInstance()->Drop("play_btn");
    TextureManager::GetInstance()->Drop("about_btn");
    TextureManager::GetInstance()->Drop("quit_btn");


}
