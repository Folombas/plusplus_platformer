#pragma once

#include "Common.h"
#include <raylib.h>
#include <vector>

enum class ParticleType {
    SPARK,
    SMOKE,
    BLOOD,
    DUST,
    COIN_SPARKLE,
    EXPLOSION,
    MAGIC,
    WATER_SPLASH
};

struct ParticleConfig {
    ParticleType type;
    Color baseColor;
    Color endColor;
    float startSize;
    float endSize;
    float startSpeed;
    float endSpeed;
    float spread;
    float lifetime;
    float gravity;
    bool fadeOut;
    bool shrink;
};

class ParticleSystem {
public:
    ParticleSystem();
    ~ParticleSystem();
    
    void Update(float dt);
    void Draw(const class GameCamera2D& camera);
    
    void Emit(Vector2 position, ParticleConfig config, int count);
    void EmitBlood(Vector2 position, int amount);
    void EmitSpark(Vector2 position, Color color, int amount);
    void EmitSmoke(Vector2 position, int amount);
    void EmitDust(Vector2 position, int amount);
    void EmitCoinSparkle(Vector2 position);
    void EmitExplosion(Vector2 position, int amount);
    void EmitMagic(Vector2 position, Color color, int amount);
    void EmitWaterSplash(Vector2 position, int amount);
    void EmitJumpDust(Vector2 position);
    void EmitLandDust(Vector2 position);
    void EmitDeathExplosion(Vector2 position);
    
    void Clear();
    int GetActiveParticleCount() const;
    
private:
    void UpdateParticle(Particle& particle, float dt);
    void DrawParticle(const Particle& particle, const class GameCamera2D& camera);
    Color InterpolateColor(Color from, Color to, float t);
    
    std::vector<Particle> particles;
    int maxParticles;
    
    // Pre-configured particle configs
    ParticleConfig bloodConfig;
    ParticleConfig sparkConfig;
    ParticleConfig smokeConfig;
    ParticleConfig dustConfig;
    ParticleConfig coinSparkleConfig;
    ParticleConfig explosionConfig;
    ParticleConfig magicConfig;
    ParticleConfig waterSplashConfig;
};
