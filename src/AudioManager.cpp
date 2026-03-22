#include "AudioManager.h"
#include <filesystem>

AudioManager::AudioManager()
    : musicVolume(0.5f)
    , masterVolume(1.0f)
    , initialized(false)
    , currentMusicName("")
{
}

AudioManager::~AudioManager() {
    Shutdown();
}

AudioManager& AudioManager::Instance() {
    static AudioManager instance;
    return instance;
}

void AudioManager::Initialize() {
    if (initialized) return;
    
    InitAudioDevice();
    LoadSounds();
    LoadMusic();
    
    initialized = true;
}

void AudioManager::Shutdown() {
    for (auto& pair : sounds) {
        UnloadSound(pair.second);
    }
    sounds.clear();
    
    for (auto& pair : musicTracks) {
        UnloadMusicStream(pair.second);
    }
    musicTracks.clear();
    
    if (IsMusicPlaying()) {
        StopMusicStream(currentMusic);
    }
    
    CloseAudioDevice();
    initialized = false;
}

void AudioManager::LoadSounds() {
    // Sound file paths
    const char* soundPaths[] = {
        "assets/audio/jump.wav",
        "assets/audio/land.wav",
        "assets/audio/coin.wav",
        "assets/audio/powerup.wav",
        "assets/audio/hurt.wav",
        "assets/audio/death.wav",
        "assets/audio/enemy_death.wav",
        "assets/audio/attack.wav",
        "assets/audio/level_complete.wav",
        "assets/audio/menu_select.wav",
        "assets/audio/menu_back.wav",
        "assets/audio/achievement.wav",
        "assets/audio/water_splash.wav"
    };
    
    const char* soundNames[] = {
        "jump", "land", "coin", "powerup", "hurt", "death",
        "enemy_death", "attack", "level_complete", "menu_select",
        "menu_back", "achievement", "water_splash"
    };
    
    // Load sounds that exist
    for (size_t i = 0; i < sizeof(soundPaths) / sizeof(soundPaths[0]); i++) {
        if (FileExists(soundPaths[i])) {
            sounds[soundNames[i]] = LoadSound(soundPaths[i]);
        }
    }
}

void AudioManager::LoadMusic() {
    const char* musicPaths[] = {
        "assets/audio/music_menu.ogg",
        "assets/audio/music_forest.ogg",
        "assets/audio/music_cave.ogg",
        "assets/audio/music_castle.ogg",
        "assets/audio/music_boss.ogg",
        "assets/audio/music_victory.ogg"
    };
    
    const char* musicNames[] = {
        "menu", "forest", "cave", "castle", "boss", "victory"
    };
    
    for (size_t i = 0; i < sizeof(musicPaths) / sizeof(musicPaths[0]); i++) {
        if (FileExists(musicPaths[i])) {
            musicTracks[musicNames[i]] = LoadMusicStream(musicPaths[i]);
        }
    }
}

void AudioManager::PlaySound(SoundType type) {
    const char* name = nullptr;
    switch (type) {
        case SoundType::JUMP: name = "jump"; break;
        case SoundType::LAND: name = "land"; break;
        case SoundType::COIN: name = "coin"; break;
        case SoundType::POWERUP: name = "powerup"; break;
        case SoundType::HURT: name = "hurt"; break;
        case SoundType::DEATH: name = "death"; break;
        case SoundType::ENEMY_DEATH: name = "enemy_death"; break;
        case SoundType::ATTACK: name = "attack"; break;
        case SoundType::LEVEL_COMPLETE: name = "level_complete"; break;
        case SoundType::MENU_SELECT: name = "menu_select"; break;
        case SoundType::MENU_BACK: name = "menu_back"; break;
        case SoundType::ACHIEVEMENT: name = "achievement"; break;
        case SoundType::WATER_SPLASH: name = "water_splash"; break;
    }
    if (name) PlaySoundFile(name);
}

void AudioManager::PlaySoundFile(const std::string& name) {
    auto it = sounds.find(name);
    if (it != sounds.end()) {
        ::PlaySound(it->second);
    }
}

void AudioManager::StopSound(const std::string& name) {
    auto it = sounds.find(name);
    if (it != sounds.end()) {
        ::StopSound(it->second);
    }
}

void AudioManager::StopSound(SoundType type) {
    std::string name;
    switch (type) {
        case SoundType::JUMP: name = "jump"; break;
        case SoundType::DEATH: name = "death"; break;
        default: break;
    }
    if (!name.empty()) StopSound(name);
}

void AudioManager::PlayMusic(MusicType type) {
    std::string name;
    
    switch (type) {
        case MusicType::MENU: name = "menu"; break;
        case MusicType::LEVEL_FOREST: name = "forest"; break;
        case MusicType::LEVEL_CAVE: name = "cave"; break;
        case MusicType::LEVEL_CASTLE: name = "castle"; break;
        case MusicType::BOSS: name = "boss"; break;
        case MusicType::VICTORY: name = "victory"; break;
    }
    
    PlayMusic(name);
}

void AudioManager::PlayMusic(const std::string& name) {
    if (IsMusicPlaying()) StopMusicStream(currentMusic);
    
    auto it = musicTracks.find(name);
    if (it != musicTracks.end()) {
        currentMusic = it->second;
        currentMusicName = name;
        ::SetMusicVolume(currentMusic, musicVolume);
        PlayMusicStream(currentMusic);
    }
}

void AudioManager::StopMusic() {
    if (IsMusicPlaying()) {
        StopMusicStream(currentMusic);
    }
}

void AudioManager::PauseMusic() {
    if (IsMusicPlaying()) {
        PauseMusicStream(currentMusic);
    }
}

void AudioManager::ResumeMusic() {
    if (IsMusicPlaying() && !IsMusicStreamPlaying(currentMusic)) {
        ResumeMusicStream(currentMusic);
    }
}

void AudioManager::SetMusicVolume(float volume) {
    musicVolume = volume;
    if (IsMusicPlaying()) ::SetMusicVolume(currentMusic, volume);
}

void AudioManager::SetMasterVolume(float volume) {
    masterVolume = volume;
    // raylib uses SetMasterVolume for global volume
    ::SetMasterVolume(volume);
}

bool AudioManager::IsMusicPlaying() const {
    return IsMusicStreamPlaying(currentMusic);
}

void AudioManager::Update() {
    if (initialized) {
        UpdateMusicStream(currentMusic);
    }
}
