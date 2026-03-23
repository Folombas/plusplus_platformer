#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <AL/al.h>
#include <AL/alc.h>

class Sound {
public:
    Sound();
    ~Sound();
    
    bool loadFromFile(const std::string& path);
    void play();
    void stop();
    void setLoop(bool loop);
    void setVolume(float volume);
    
    bool isPlaying() const;
    
private:
    ALuint m_buffer;
    ALuint m_source;
    bool m_loaded;
};

class AudioManager {
public:
    static AudioManager& getInstance();
    
    bool init();
    void shutdown();
    
    int loadSound(const std::string& name, const std::string& path);
    void playSound(const std::string& name);
    void playSound(int id);
    void stopAll();
    
    void setMasterVolume(float volume);
    
private:
    AudioManager() = default;
    
    ALCdevice* m_device = nullptr;
    ALCcontext* m_context = nullptr;
    std::unordered_map<std::string, int> m_nameToId;
    std::unordered_map<int, std::unique_ptr<Sound>> m_sounds;
    int m_nextId = 0;
    float m_masterVolume = 1.0f;
};
