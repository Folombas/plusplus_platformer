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
    // Sound file paths - primary (new Kenney assets) and fallback
    const char* soundPaths[] = {
        // Jump sounds - try Kenney RPG audio first
        "assets/audio/kenney_rpg-audio/Audio/footstep_grass_000.ogg",
        "assets/audio/kenney_impact-sounds/Audio/footstep_grass_000.ogg",
        "assets/audio/jump.wav",
        
        // Land sounds
        "assets/audio/kenney_impact-sounds/Audio/impactGeneric_light_000.ogg",
        "assets/audio/land.wav",
        
        // Coin sounds
        "assets/audio/kenney_ui-audio/Audio/coin_000.ogg",
        "assets/audio/kenney_ui-audio/Audio/click_000.ogg",
        "assets/audio/coin.wav",
        
        // Powerup sounds
        "assets/audio/kenney_rpg-audio/Audio/powerup_000.ogg",
        "assets/audio/powerup.wav",
        
        // Hurt sounds
        "assets/audio/kenney_impact-sounds/Audio/impactPunch_heavy_000.ogg",
        "assets/audio/kenney_rpg-audio/Audio/hurt_000.ogg",
        "assets/audio/hurt.wav",
        
        // Death sounds
        "assets/audio/kenney_rpg-audio/Audio/death_000.ogg",
        "assets/audio/death.wav",
        
        // Enemy death sounds
        "assets/audio/kenney_impact-sounds/Audio/impactBell_heavy_000.ogg",
        "assets/audio/enemy_death.wav",
        
        // Attack sounds
        "assets/audio/kenney_rpg-audio/Audio/attack_000.ogg",
        "assets/audio/kenney_impact-sounds/Audio/impactGeneric_light_000.ogg",
        "assets/audio/attack.wav",
        
        // Level complete sounds
        "assets/audio/kenney_ui-audio/Audio/achievement_000.ogg",
        "assets/audio/level_complete.wav",
        
        // Menu select sounds
        "assets/audio/kenney_ui-audio/Audio/click_000.ogg",
        "assets/audio/kenney_ui-audio/Audio/roll_000.ogg",
        "assets/audio/menu_select.wav",
        
        // Menu back sounds
        "assets/audio/kenney_ui-audio/Audio/click_001.ogg",
        "assets/audio/menu_back.wav",
        
        // Achievement sounds
        "assets/audio/kenney_ui-audio/Audio/achievement_000.ogg",
        "assets/audio/achievement.wav",
        
        // Water splash sounds
        "assets/audio/kenney_impact-sounds/Audio/impactWater_heavy_000.ogg",
        "assets/audio/water_splash.wav"
    };

    const char* soundNames[] = {
        "jump", "jump_alt",
        "land", "land_alt",
        "coin", "coin_alt",
        "powerup", "powerup_alt",
        "hurt", "hurt_alt",
        "death", "death_alt",
        "enemy_death", "enemy_death_alt",
        "attack", "attack_alt",
        "level_complete", "level_complete_alt",
        "menu_select", "menu_select_alt",
        "menu_back", "menu_back_alt",
        "achievement", "achievement_alt",
        "water_splash", "water_splash_alt"
    };

    // Load sounds that exist - prioritize first available
    std::string loadedSounds[15]; // Track which sound we loaded for each category
    
    for (size_t i = 0; i < sizeof(soundPaths) / sizeof(soundPaths[0]); i++) {
        if (FileExists(soundPaths[i])) {
            // Extract base name for lookup
            std::string name = soundNames[i];
            // Remove "_alt" suffix for primary name
            size_t altPos = name.find("_alt");
            if (altPos != std::string::npos) {
                std::string baseName = name.substr(0, altPos);
                // Only load if we haven't loaded this sound yet
                if (loadedSounds[i / 2].empty()) {
                    sounds[baseName] = LoadSound(soundPaths[i]);
                    loadedSounds[i / 2] = baseName;
                    TraceLog(LOG_INFO, "Loaded sound: %s -> %s", soundPaths[i], baseName.c_str());
                }
            } else {
                if (sounds.find(name) == sounds.end()) {
                    sounds[name] = LoadSound(soundPaths[i]);
                    TraceLog(LOG_INFO, "Loaded sound: %s -> %s", soundPaths[i], name.c_str());
                }
            }
        }
    }
}

void AudioManager::LoadMusic() {
    // Music file paths - try Kenney audio packs first, then fallback to existing
    const char* musicPaths[][4] = {
        // Menu music
        {
            "assets/audio/kenney_rpg-audio/Audio/music_theme_000.ogg",
            "assets/audio/kenney_ui-audio/Audio/longUI_000.ogg",
            "assets/audio/music_menu.ogg",
            nullptr
        },
        // Forest level music
        {
            "assets/audio/kenney_rpg-audio/Audio/music_forest_000.ogg",
            "assets/audio/kenney_rpg-audio/Audio/music_day_000.ogg",
            "assets/audio/music_forest.ogg",
            nullptr
        },
        // Cave level music
        {
            "assets/audio/kenney_rpg-audio/Audio/music_cave_000.ogg",
            "assets/audio/kenney_rpg-audio/Audio/music_dungeon_000.ogg",
            "assets/audio/music_cave.ogg",
            nullptr
        },
        // Castle level music
        {
            "assets/audio/kenney_rpg-audio/Audio/music_castle_000.ogg",
            "assets/audio/kenney_rpg-audio/Audio/music_boss_000.ogg",
            "assets/audio/music_castle.ogg",
            nullptr
        },
        // Boss battle music
        {
            "assets/audio/kenney_rpg-audio/Audio/music_battle_000.ogg",
            "assets/audio/kenney_rpg-audio/Audio/music_boss_000.ogg",
            "assets/audio/music_boss.ogg",
            nullptr
        },
        // Victory music
        {
            "assets/audio/kenney_ui-audio/Audio/victory_000.ogg",
            "assets/audio/kenney_rpg-audio/Audio/music_victory_000.ogg",
            "assets/audio/music_victory.ogg",
            nullptr
        }
    };

    const char* musicNames[] = {
        "menu", "forest", "cave", "castle", "boss", "victory"
    };

    // Load first available music track for each category
    for (size_t i = 0; i < sizeof(musicPaths) / sizeof(musicPaths[0]); i++) {
        for (const char* path : musicPaths[i]) {
            if (path == nullptr) break;
            if (FileExists(path)) {
                musicTracks[musicNames[i]] = LoadMusicStream(path);
                TraceLog(LOG_INFO, "Loaded music: %s -> %s", path, musicNames[i]);
                break;
            }
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
