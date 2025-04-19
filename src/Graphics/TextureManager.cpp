#include "TextureManager.h"
#include "../Core/Engine.h"
#include <SDL_image.h>

TextureManager* TextureManager::s_Instance = nullptr;

bool TextureManager::Load(std::string id, std::string filename)
{
    if (m_TextureMap.count(id)) {
        SDL_Log("Texture '%s' (ID: %s) already loaded.", filename.c_str(), id.c_str());
        return true;
    }

    SDL_Surface* surface = IMG_Load(filename.c_str());
    if(surface == nullptr)
    {
        SDL_Log("Failed to load texture file: %s, Error: %s", filename.c_str(), IMG_GetError());
        return false;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(Engine::GetInstance()->GetRenderer(), surface);
    SDL_FreeSurface(surface);
    if(texture == nullptr)
    {
        SDL_Log("Failed to create texture from surface for file %s: Error: %s", filename.c_str(), SDL_GetError());
        return false;
    }

    m_TextureMap[id] = texture;
    SDL_Log("Loaded Texture: %s as ID: %s", filename.c_str(), id.c_str());
    return true;
}

void TextureManager::Draw(std::string id, int x, int y, int width, int height, SDL_RendererFlip flip)
{
    SDL_Rect dstRect = {x, y, width, height};

    auto it = m_TextureMap.find(id);
    if (it != m_TextureMap.end()) {
       SDL_RenderCopyEx(Engine::GetInstance()->GetRenderer(), it->second, NULL, &dstRect, 0, nullptr, flip);
    } else {
        SDL_Log("Warning: Attempted to draw non-existent texture ID: %s", id.c_str());
    }
}

bool TextureManager::QueryTexture(const std::string& id, int* width, int* height) {
    auto it = m_TextureMap.find(id);
    if (it == m_TextureMap.end()) {
        SDL_Log("Texture ID '%s' not found in QueryTexture.", id.c_str());
        if (width) *width = 0;
        if (height) *height = 0;
        return false;
    }

    if (SDL_QueryTexture(it->second, nullptr, nullptr, width, height) != 0) {
         SDL_Log("Failed to query texture '%s': %s", id.c_str(), SDL_GetError());
         if (width) *width = 0;
         if (height) *height = 0;
         return false;
    }
    return true;
}

void TextureManager::Drop(std::string id)
{
    auto it = m_TextureMap.find(id);
    if (it != m_TextureMap.end()) {
        if (it->second != nullptr) {
             SDL_DestroyTexture(it->second);
        }
        m_TextureMap.erase(it);
        SDL_Log("Dropped texture ID: %s", id.c_str());
    } else {
         SDL_Log("Warning: Tried to drop non-existent texture ID: %s", id.c_str());
    }
}

void TextureManager::Clean()
{
    SDL_Log("Cleaning TextureManager...");
    std::map<std::string, SDL_Texture*>::iterator it;
    for(it = m_TextureMap.begin(); it != m_TextureMap.end(); ++it) {
        if (it->second != nullptr) {
             SDL_DestroyTexture(it->second);
        }
    }
    m_TextureMap.clear();
    SDL_Log("Texture map cleaned!");
}
