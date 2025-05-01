#include "MainMenu.h"
#include "../Core/Engine.h"
#include "../Graphics/TextureManager.h"
#include "../Audio/AudioManager.h"
#include <SDL.h>
#include <algorithm>

MainMenu* MainMenu::s_Instance = nullptr;

bool MainMenu::Init() {
    if (!TextureManager::GetInstance()->Load("menu_bg", "assets/Menu/menu_background.png")) return false;
    if (!TextureManager::GetInstance()->Load("play_btn", "assets/Menu/btn_jouer.png") || !TextureManager::GetInstance()->Load("play_btn_hover", "assets/Menu/btn_jouer_hover.png")) return false;
    if (!TextureManager::GetInstance()->Load("about_btn", "assets/Menu/btn_a_propos.png") || !TextureManager::GetInstance()->Load("about_btn_hover", "assets/Menu/btn_a_propos_hover.png")) return false;
    if (!TextureManager::GetInstance()->Load("about_screen", "assets/Menu/about_background.png")) return false;
    if (!TextureManager::GetInstance()->Load("quit_btn", "assets/Menu/btn_quitter.png") || !TextureManager::GetInstance()->Load("quit_btn_hover", "assets/Menu/btn_quitter_hover.png")) return false;

    if (!TextureManager::GetInstance()->Load("vol_down_btn", "assets/Menu/vol_down.png") || !TextureManager::GetInstance()->Load("vol_down_btn_hover", "assets/Menu/vol_down_hover.png")) { SDL_Log("Error loading vol down textures"); return false; }
    if (!TextureManager::GetInstance()->Load("vol_up_btn", "assets/Menu/vol_up.png") || !TextureManager::GetInstance()->Load("vol_up_btn_hover", "assets/Menu/vol_up_hover.png")) { SDL_Log("Error loading vol up textures"); return false; }
    if (!TextureManager::GetInstance()->Load("mute_btn", "assets/Menu/mute.png") || !TextureManager::GetInstance()->Load("mute_btn_hover", "assets/Menu/mute_hover.png")) { SDL_Log("Error loading mute textures"); return false; }
    if (!TextureManager::GetInstance()->Load("unmute_btn", "assets/Menu/unmute.png") || !TextureManager::GetInstance()->Load("unmute_btn_hover", "assets/Menu/unmute_hover.png")) { SDL_Log("Error loading unmute textures"); return false; }


    SDL_Log("SUCCÈS: Toutes les textures du menu sont chargées!");
    return true;
}

void MainMenu::HandleEvent(SDL_Event& event) {
    int x, y; SDL_GetMouseState(&x, &y); SDL_Point mousePoint = {x, y};

    m_playHovered = SDL_PointInRect(&mousePoint, &m_playButtonRect);
    m_aboutHovered = SDL_PointInRect(&mousePoint, &m_aboutButtonRect);
    m_quitHovered = SDL_PointInRect(&mousePoint, &m_quitButtonRect);
    m_volDownHovered = SDL_PointInRect(&mousePoint, &m_volDownRect);
    m_volUpHovered = SDL_PointInRect(&mousePoint, &m_volUpRect);
    m_muteToggleHovered = SDL_PointInRect(&mousePoint, &m_muteToggleRect);

    if (event.type == SDL_MOUSEBUTTONDOWN) {
        if (m_playHovered) { AudioManager::GetInstance()->PlaySound("click", 0); Engine::GetInstance()->SetGameState(STATE_START_SCREEN); SDL_Log("Clic sur Jouer!"); }
        else if (m_aboutHovered) { AudioManager::GetInstance()->PlaySound("click", 0); Engine::GetInstance()->SetGameState(STATE_ABOUT); SDL_Log("Clic sur A Propos!"); }
        else if (m_quitHovered) { AudioManager::GetInstance()->PlaySound("click", 0); Engine::GetInstance()->Quit(); SDL_Log("Clic sur Quitter!"); }
        else if (m_volDownHovered) { AudioManager::GetInstance()->PlaySound("click", 0); Engine::GetInstance()->DecreaseVolume(); }
        else if (m_volUpHovered) { AudioManager::GetInstance()->PlaySound("click", 0); Engine::GetInstance()->IncreaseVolume(); }
        else if (m_muteToggleHovered) { AudioManager::GetInstance()->PlaySound("click", 0); Engine::GetInstance()->ToggleMute(); }
    }
}

void MainMenu::Update(float deltaTime) {
    m_playScale = std::min(m_hoverScale, std::max(1.0f, m_playScale + (m_playHovered ? 1.0f : -1.0f) * deltaTime * 8.0f));
    m_aboutScale = std::min(m_hoverScale, std::max(1.0f, m_aboutScale + (m_aboutHovered ? 1.0f : -1.0f) * deltaTime * 8.0f));
    m_quitScale = std::min(m_hoverScale, std::max(1.0f, m_quitScale + (m_quitHovered ? 1.0f : -1.0f) * deltaTime * 8.0f));
    m_volDownScale = std::min(m_hoverScale, std::max(1.0f, m_volDownScale + (m_volDownHovered ? 1.0f : -1.0f) * deltaTime * 8.0f));
    m_volUpScale = std::min(m_hoverScale, std::max(1.0f, m_volUpScale + (m_volUpHovered ? 1.0f : -1.0f) * deltaTime * 8.0f));
    m_muteToggleScale = std::min(m_hoverScale, std::max(1.0f, m_muteToggleScale + (m_muteToggleHovered ? 1.0f : -1.0f) * deltaTime * 8.0f));
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
    drawButton("vol_down_btn", m_volDownRect, m_volDownHovered, m_volDownScale);
    drawButton("vol_up_btn", m_volUpRect, m_volUpHovered, m_volUpScale);

    bool isMuted = Engine::GetInstance()->IsMuted();
    std::string muteBaseID = isMuted ? "unmute_btn" : "mute_btn";
    drawButton(muteBaseID, m_muteToggleRect, m_muteToggleHovered, m_muteToggleScale);
}

void MainMenu::Clean() {
    TextureManager::GetInstance()->Drop("menu_bg");
    TextureManager::GetInstance()->Drop("play_btn"); TextureManager::GetInstance()->Drop("play_btn_hover");
    TextureManager::GetInstance()->Drop("about_btn"); TextureManager::GetInstance()->Drop("about_btn_hover");
    TextureManager::GetInstance()->Drop("quit_btn"); TextureManager::GetInstance()->Drop("quit_btn_hover");
    TextureManager::GetInstance()->Drop("about_screen");
    TextureManager::GetInstance()->Drop("vol_down_btn"); TextureManager::GetInstance()->Drop("vol_down_btn_hover");
    TextureManager::GetInstance()->Drop("vol_up_btn"); TextureManager::GetInstance()->Drop("vol_up_btn_hover");
    TextureManager::GetInstance()->Drop("mute_btn"); TextureManager::GetInstance()->Drop("mute_btn_hover");
    TextureManager::GetInstance()->Drop("unmute_btn"); TextureManager::GetInstance()->Drop("unmute_btn_hover");
}
