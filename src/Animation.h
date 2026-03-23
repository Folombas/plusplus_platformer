#pragma once

#include <vector>
#include <string>

struct Frame {
    int textureId;
    float duration;
    int x, y, width, height; // координаты в спрайтшите
};

class Animation {
public:
    Animation(const std::string& name = "", float fps = 10.0f);
    
    void addFrame(int textureId, float duration = 0.1f);
    void addFrameFromSpriteSheet(int textureId, int x, int y, int w, int h, float duration = 0.1f);
    
    void update(float dt);
    void reset();
    void play();
    void pause();
    void stop();
    
    int getCurrentFrameTexture() const;
    void getCurrentFrameRect(int& x, int& y, int& w, int& h) const;
    
    bool isFinished() const { return m_finished; }
    bool isPlaying() const { return m_playing; }
    const std::string& getName() const { return m_name; }
    
private:
    std::string m_name;
    std::vector<Frame> m_frames;
    float m_frameDuration;
    float m_elapsedTime;
    size_t m_currentFrame;
    bool m_playing;
    bool m_loop;
    bool m_finished;
};

class AnimationManager {
public:
    static AnimationManager& getInstance();
    
    void addAnimation(const std::string& name, Animation&& anim);
    Animation* getAnimation(const std::string& name);
    
private:
    AnimationManager() = default;
    std::unordered_map<std::string, Animation> m_animations;
};
