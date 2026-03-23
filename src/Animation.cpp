#include "Animation.h"

Animation::Animation(const std::string& name, float fps)
    : m_name(name)
    , m_frameDuration(1.0f / fps)
    , m_elapsedTime(0.0f)
    , m_currentFrame(0)
    , m_playing(false)
    , m_loop(true)
    , m_finished(false)
{}

void Animation::addFrame(int textureId, float duration) {
    m_frames.push_back({textureId, duration, 0, 0, 0, 0});
}

void Animation::addFrameFromSpriteSheet(int textureId, int x, int y, int w, int h, float duration) {
    m_frames.push_back({textureId, duration, x, y, w, h});
}

void Animation::update(float dt) {
    if (!m_playing || m_frames.empty()) return;
    
    m_elapsedTime += dt;
    
    while (m_elapsedTime >= m_frames[m_currentFrame].duration) {
        m_elapsedTime -= m_frames[m_currentFrame].duration;
        m_currentFrame++;
        
        if (m_currentFrame >= m_frames.size()) {
            if (m_loop) {
                m_currentFrame = 0;
            } else {
                m_currentFrame = m_frames.size() - 1;
                m_playing = false;
                m_finished = true;
                return;
            }
        }
    }
}

void Animation::reset() {
    m_currentFrame = 0;
    m_elapsedTime = 0.0f;
    m_finished = false;
}

void Animation::play() {
    m_playing = true;
    m_finished = false;
}

void Animation::pause() {
    m_playing = false;
}

void Animation::stop() {
    m_playing = false;
    reset();
}

int Animation::getCurrentFrameTexture() const {
    if (m_frames.empty()) return -1;
    return m_frames[m_currentFrame].textureId;
}

void Animation::getCurrentFrameRect(int& x, int& y, int& w, int& h) const {
    if (m_frames.empty()) {
        x = y = w = h = 0;
        return;
    }
    x = m_frames[m_currentFrame].x;
    y = m_frames[m_currentFrame].y;
    w = m_frames[m_currentFrame].width;
    h = m_frames[m_currentFrame].height;
}

AnimationManager& AnimationManager::getInstance() {
    static AnimationManager instance;
    return instance;
}

void AnimationManager::addAnimation(const std::string& name, Animation&& anim) {
    m_animations.emplace(name, std::move(anim));
}

Animation* AnimationManager::getAnimation(const std::string& name) {
    auto it = m_animations.find(name);
    return (it != m_animations.end()) ? &it->second : nullptr;
}
