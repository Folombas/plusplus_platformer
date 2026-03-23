#include "Audio.h"
#include <iostream>
#include <fstream>
#include <vector>

// Простая заглушка для загрузки WAV
// В полной версии нужно использовать libsndfile или SDL_mixer для OGG
bool loadWAV(const std::string& path, std::vector<ALbyte>& data, ALsizei& freq, ALenum& format) {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Cannot open file: " << path << std::endl;
        return false;
    }
    
    // Проверка WAV заголовка
    char header[12];
    file.read(header, 12);
    
    if (header[0] != 'R' || header[1] != 'I' || header[2] != 'F' || header[3] != 'F' ||
        header[8] != 'W' || header[9] != 'A' || header[10] != 'V' || header[11] != 'E') {
        std::cerr << "Not a valid WAV file: " << path << " (may be OGG)" << std::endl;
        return false;
    }
    
    file.seekg(0);
    char fullHeader[44];
    file.read(fullHeader, 44);
    
    // Частота дискретизации (байты 24-27)
    freq = 0;
    for (int i = 0; i < 4; i++) {
        freq |= (static_cast<unsigned char>(fullHeader[24 + i]) << (i * 8));
    }
    
    int channels = static_cast<unsigned char>(fullHeader[22]);
    int bitsPerSample = static_cast<unsigned char>(fullHeader[34]);
    
    if (channels == 1 && bitsPerSample == 8) {
        format = AL_FORMAT_MONO8;
    } else if (channels == 2 && bitsPerSample == 8) {
        format = AL_FORMAT_STEREO8;
    } else if (channels == 1 && bitsPerSample == 16) {
        format = AL_FORMAT_MONO16;
    } else if (channels == 2 && bitsPerSample == 16) {
        format = AL_FORMAT_STEREO16;
    } else {
        std::cerr << "Unsupported WAV format: " << channels << " channels, " 
                  << bitsPerSample << " bits" << std::endl;
        return false;
    }
    
    // Находим data chunk
    char chunkId[4];
    ALuint chunkSize;
    while (file.read(chunkId, 4)) {
        file.read(reinterpret_cast<char*>(&chunkSize), 4);
        if (chunkId[0] == 'd' && chunkId[1] == 'a' && chunkId[2] == 't' && chunkId[3] == 'a') {
            break;
        }
        file.seekg(chunkSize, std::ios::cur);
    }
    
    data.resize(chunkSize);
    file.read(reinterpret_cast<char*>(data.data()), chunkSize);
    
    return true;
}

Sound::Sound() : m_buffer(0), m_source(0), m_loaded(false) {
    alGenSources(1, &m_source);
}

Sound::~Sound() {
    if (m_source != 0) {
        alDeleteSources(1, &m_source);
    }
    if (m_buffer != 0) {
        alDeleteBuffers(1, &m_buffer);
    }
}

bool Sound::loadFromFile(const std::string& path) {
    std::vector<ALbyte> data;
    ALsizei freq;
    ALenum format;
    
    if (!loadWAV(path, data, freq, format)) {
        return false;
    }
    
    alGenBuffers(1, &m_buffer);
    alBufferData(m_buffer, format, data.data(), static_cast<ALsizei>(data.size()), freq);
    
    alSourcei(m_source, AL_BUFFER, m_buffer);
    m_loaded = true;
    return true;
}

void Sound::play() {
    if (!m_loaded) return;
    
    ALint state;
    alGetSourcei(m_source, AL_SOURCE_STATE, &state);
    if (state == AL_PLAYING) {
        alSourceStop(m_source);
    }
    alSourcePlay(m_source);
}

void Sound::stop() {
    if (m_loaded) {
        alSourceStop(m_source);
    }
}

void Sound::setLoop(bool loop) {
    if (m_loaded) {
        alSourcei(m_source, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
    }
}

void Sound::setVolume(float volume) {
    if (m_loaded) {
        alSourcef(m_source, AL_GAIN, volume);
    }
}

bool Sound::isPlaying() const {
    if (!m_loaded) return false;
    ALint state;
    alGetSourcei(m_source, AL_SOURCE_STATE, &state);
    return state == AL_PLAYING;
}

AudioManager& AudioManager::getInstance() {
    static AudioManager instance;
    return instance;
}

bool AudioManager::init() {
    m_device = alcOpenDevice(nullptr);
    if (!m_device) {
        std::cerr << "Failed to open OpenAL device" << std::endl;
        return false;
    }
    
    m_context = alcCreateContext(m_device, nullptr);
    if (!m_context) {
        std::cerr << "Failed to create OpenAL context" << std::endl;
        alcCloseDevice(m_device);
        return false;
    }
    
    alcMakeContextCurrent(m_context);
    return true;
}

void AudioManager::shutdown() {
    if (m_context) {
        alcMakeContextCurrent(nullptr);
        alcDestroyContext(m_context);
    }
    if (m_device) {
        alcCloseDevice(m_device);
    }
}

int AudioManager::loadSound(const std::string& name, const std::string& path) {
    auto it = m_nameToId.find(name);
    if (it != m_nameToId.end()) {
        return it->second;
    }
    
    auto sound = std::make_unique<Sound>();
    if (!sound->loadFromFile(path)) {
        return -1;
    }
    
    int id = m_nextId++;
    m_nameToId[name] = id;
    m_sounds[id] = std::move(sound);
    return id;
}

void AudioManager::playSound(const std::string& name) {
    auto it = m_nameToId.find(name);
    if (it != m_nameToId.end()) {
        playSound(it->second);
    }
}

void AudioManager::playSound(int id) {
    auto it = m_sounds.find(id);
    if (it != m_sounds.end()) {
        it->second->setVolume(m_masterVolume);
        it->second->play();
    }
}

void AudioManager::stopAll() {
    for (auto& pair : m_sounds) {
        pair.second->stop();
    }
}

void AudioManager::setMasterVolume(float volume) {
    m_masterVolume = Utils::clamp(volume, 0.0f, 1.0f);
}
