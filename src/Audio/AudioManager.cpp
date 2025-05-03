#include "AudioManager.h"
#include <SDL.h>

AudioManager* AudioManager::s_Instance = nullptr;

bool AudioManager::Init() {
    if (Mix_Init(MIX_INIT_OGG | MIX_INIT_MP3) == 0) {
         SDL_Log("Failed to init SDL_mixer: %s", Mix_GetError());
         return false;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        SDL_Log("Failed to open audio device: %s", Mix_GetError());
        Mix_Quit();
        return false;
    }
    SDL_Log("SDL_mixer initialized successfully!");
    return true;
}

void AudioManager::Clean() {
    SDL_Log("Cleaning AudioManager...");
    for (auto const& [id, chunk] : m_SoundMap) {
        if (chunk != nullptr) {
            Mix_FreeChunk(chunk);
        }
    }
    m_SoundMap.clear();
    SDL_Log("Sounds freed.");

    for (auto const& [id, music] : m_MusicMap) {
        if (music != nullptr) {
            Mix_FreeMusic(music);
        }
    }
    m_MusicMap.clear();
    SDL_Log("Music freed.");

    Mix_CloseAudio();
    Mix_Quit();
    SDL_Log("SDL_mixer closed.");
}

bool AudioManager::LoadMusic(const std::string& id, const std::string& source) {
    if (IsMusicLoaded(id)) {
        SDL_Log("Music '%s' already loaded.", id.c_str());
        return true;
    }
    Mix_Music* music = Mix_LoadMUS(source.c_str());
    if (music == nullptr) {
        SDL_Log("Failed to load music '%s': %s", source.c_str(), Mix_GetError());
        return false;
    }
    m_MusicMap[id] = music;
    SDL_Log("Loaded Music: %s as ID: %s", source.c_str(), id.c_str());
    return true;
}

bool AudioManager::LoadSound(const std::string& id, const std::string& source) {
    if (IsSoundLoaded(id)) {
        SDL_Log("Sound '%s' already loaded.", id.c_str());
        return true;
    }
    Mix_Chunk* sound = Mix_LoadWAV(source.c_str());
    if (sound == nullptr) {
        SDL_Log("Failed to load sound '%s': %s", source.c_str(), Mix_GetError());
        return false;
    }
    m_SoundMap[id] = sound;
    SDL_Log("Loaded Sound: %s as ID: %s", source.c_str(), id.c_str());
    return true;
}

void AudioManager::PlayMusic(const std::string& id, int loops) {
    if (!IsMusicLoaded(id)) {
        SDL_Log("Cannot play music '%s': Not loaded.", id.c_str());
        return;
    }
    if (Mix_PlayingMusic()) {
        Mix_HaltMusic();
    }
    if (Mix_PlayMusic(m_MusicMap[id], loops) == -1) {
        SDL_Log("Failed to play music '%s': %s", id.c_str(), Mix_GetError());
    }
}

void AudioManager::PlaySound(const std::string& id, int loops) {
    if (!IsSoundLoaded(id)) {
        SDL_Log("Cannot play sound '%s': Not loaded.", id.c_str());
        return;
    }
    if (Mix_PlayChannel(-1, m_SoundMap[id], loops) == -1) {
        SDL_Log("Failed to play sound '%s' on a channel: %s", id.c_str(), Mix_GetError());
    }
}

void AudioManager::StopMusic() {
    if (Mix_PlayingMusic()) {
        Mix_HaltMusic();
    }
}

void AudioManager::PauseMusic() {
    if (Mix_PlayingMusic()) {
        Mix_PauseMusic();
    }
}

void AudioManager::ResumeMusic() {
    if (Mix_PausedMusic()) {
        Mix_ResumeMusic();
    }
}/////////
bool AudioManager::IsMusicPaused() {
    return Mix_PausedMusic();
}
///

void AudioManager::SetMusicVolume(int volume) {
    volume = (volume < 0) ? 0 : (volume > MIX_MAX_VOLUME) ? MIX_MAX_VOLUME : volume;
    Mix_VolumeMusic(volume);
}

void AudioManager::SetSoundVolume(const std::string& soundId, int volume) {
     if (!IsSoundLoaded(soundId)) {
        SDL_Log("Cannot set volume for sound '%s': Not loaded.", soundId.c_str());
        return;
    }
    volume = (volume < 0) ? 0 : (volume > MIX_MAX_VOLUME) ? MIX_MAX_VOLUME : volume;
    Mix_VolumeChunk(m_SoundMap[soundId], volume);
}

void AudioManager::SetAllSoundsVolume(int volume) {
     volume = (volume < 0) ? 0 : (volume > MIX_MAX_VOLUME) ? MIX_MAX_VOLUME : volume;
     Mix_Volume(-1, volume);
}


bool AudioManager::IsMusicLoaded(const std::string& id) {
    return m_MusicMap.find(id) != m_MusicMap.end() && m_MusicMap[id] != nullptr;
}

bool AudioManager::IsSoundLoaded(const std::string& id) {
     return m_SoundMap.find(id) != m_SoundMap.end() && m_SoundMap[id] != nullptr;
}
