#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include <string>
#include <map>
#include <SDL_mixer.h>

class AudioManager {
public:
    static AudioManager* GetInstance() {
        return s_Instance = (s_Instance != nullptr) ? s_Instance : new AudioManager();
    }

    bool Init();
    void Clean();

    bool LoadMusic(const std::string& id, const std::string& source);
    bool LoadSound(const std::string& id, const std::string& source);

    void PlayMusic(const std::string& id, int loops = -1);
    void PlaySound(const std::string& id, int loops = 0);

    void StopMusic();
    void PauseMusic();
    void ResumeMusic();

    void SetMusicVolume(int volume);
    void SetSoundVolume(const std::string& soundId, int volume);
    void SetAllSoundsVolume(int volume);

private:
    AudioManager() {}
    static AudioManager* s_Instance;

    std::map<std::string, Mix_Music*> m_MusicMap;
    std::map<std::string, Mix_Chunk*> m_SoundMap;

    bool IsMusicLoaded(const std::string& id);
    bool IsSoundLoaded(const std::string& id);
};

#endif // AUDIOMANAGER_H
