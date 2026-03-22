#pragma once

#include <raylib.h>
#include <string>
#include <unordered_map>

enum class SoundType {
    JUMP,
    LAND,
    COIN,
    POWERUP,
    HURT,
    DEATH,
    ENEMY_DEATH,
    ATTACK,
    LEVEL_COMPLETE,
    MENU_SELECT,
    MENU_BACK,
    ACHIEVEMENT,
    WATER_SPLASH
};

enum class MusicType {
    MENU,
    LEVEL_FOREST,
    LEVEL_CAVE,
    LEVEL_CASTLE,
    BOSS,
    VICTORY
};

class AudioManager {
public:
    static AudioManager& Instance();
    
    void Initialize();
    void Shutdown();
    
    void PlaySound(SoundType type);
    void PlaySoundFile(const std::string& name);
    void StopSound(const std::string& name);
    void StopSound(SoundType type);
    
    void PlayMusic(MusicType type);
    void PlayMusic(const std::string& name);
    void StopMusic();
    void PauseMusic();
    void ResumeMusic();
    
    void SetMusicVolume(float volume);
    void SetMasterVolume(float volume);
    
    float GetMusicVolume() const { return musicVolume; }
    bool IsMusicPlaying() const;
    
    void Update();
    
private:
    AudioManager();
    ~AudioManager();
    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;
    
    void LoadSounds();
    void LoadMusic();
    
    std::unordered_map<std::string, Sound> sounds;
    std::unordered_map<std::string, Music> musicTracks;
    
    Music currentMusic;
    std::string currentMusicName;
    
    float musicVolume;
    float masterVolume;
    bool initialized;
};
