#include "ParticleSystem.h"
#include "Camera2D.h"

#ifdef _WIN32
#undef CloseWindow
#undef ShowCursor
#endif

ParticleSystem::ParticleSystem()
    : maxParticles(1000)
{
    // Configure particle types
    
    // Blood
    bloodConfig.type = ParticleType::BLOOD;
    bloodConfig.baseColor = MAROON;
    bloodConfig.endColor = Color{139, 0, 0, 255};
    bloodConfig.startSize = 6;
    bloodConfig.endSize = 2;
    bloodConfig.startSpeed = 100;
    bloodConfig.endSpeed = 20;
    bloodConfig.spread = 3.14f;
    bloodConfig.lifetime = 1.0f;
    bloodConfig.gravity = 500;
    bloodConfig.fadeOut = true;
    bloodConfig.shrink = true;
    
    // Spark
    sparkConfig.type = ParticleType::SPARK;
    sparkConfig.baseColor = YELLOW;
    sparkConfig.endColor = ORANGE;
    sparkConfig.startSize = 4;
    sparkConfig.endSize = 1;
    sparkConfig.startSpeed = 200;
    sparkConfig.endSpeed = 50;
    sparkConfig.spread = 6.28f;
    sparkConfig.lifetime = 0.5f;
    sparkConfig.gravity = 200;
    sparkConfig.fadeOut = true;
    sparkConfig.shrink = true;
    
    // Smoke
    smokeConfig.type = ParticleType::SMOKE;
    smokeConfig.baseColor = {200, 200, 200, 150};
    smokeConfig.endColor = {100, 100, 100, 0};
    smokeConfig.startSize = 20;
    smokeConfig.endSize = 50;
    smokeConfig.startSpeed = 50;
    smokeConfig.endSpeed = 20;
    smokeConfig.spread = 3.14f;
    smokeConfig.lifetime = 1.5f;
    smokeConfig.gravity = -30;
    smokeConfig.fadeOut = true;
    smokeConfig.shrink = false;
    
    // Dust
    dustConfig.type = ParticleType::DUST;
    dustConfig.baseColor = {200, 180, 150, 180};
    dustConfig.endColor = {150, 130, 100, 0};
    dustConfig.startSize = 8;
    dustConfig.endSize = 15;
    dustConfig.startSpeed = 80;
    dustConfig.endSpeed = 20;
    dustConfig.spread = 3.14f;
    dustConfig.lifetime = 0.5f;
    dustConfig.gravity = 100;
    dustConfig.fadeOut = true;
    dustConfig.shrink = false;
    
    // Coin sparkle
    coinSparkleConfig.type = ParticleType::COIN_SPARKLE;
    coinSparkleConfig.baseColor = GOLD;
    coinSparkleConfig.endColor = YELLOW;
    coinSparkleConfig.startSize = 5;
    coinSparkleConfig.endSize = 2;
    coinSparkleConfig.startSpeed = 100;
    coinSparkleConfig.endSpeed = 30;
    coinSparkleConfig.spread = 6.28f;
    coinSparkleConfig.lifetime = 0.6f;
    coinSparkleConfig.gravity = -50;
    coinSparkleConfig.fadeOut = true;
    coinSparkleConfig.shrink = true;
    
    // Explosion
    explosionConfig.type = ParticleType::EXPLOSION;
    explosionConfig.baseColor = ORANGE;
    explosionConfig.endColor = Color{139, 0, 0, 255};
    explosionConfig.startSize = 15;
    explosionConfig.endSize = 5;
    explosionConfig.startSpeed = 300;
    explosionConfig.endSpeed = 100;
    explosionConfig.spread = 6.28f;
    explosionConfig.lifetime = 0.8f;
    explosionConfig.gravity = 100;
    explosionConfig.fadeOut = true;
    explosionConfig.shrink = true;
    
    // Magic
    magicConfig.type = ParticleType::MAGIC;
    magicConfig.baseColor = PURPLE;
    magicConfig.endColor = PINK;
    magicConfig.startSize = 6;
    magicConfig.endSize = 2;
    magicConfig.startSpeed = 80;
    magicConfig.endSpeed = 30;
    magicConfig.spread = 6.28f;
    magicConfig.lifetime = 0.8f;
    magicConfig.gravity = -20;
    magicConfig.fadeOut = true;
    magicConfig.shrink = true;
    
    // Water splash
    waterSplashConfig.type = ParticleType::WATER_SPLASH;
    waterSplashConfig.baseColor = SKYBLUE;
    waterSplashConfig.endColor = BLUE;
    waterSplashConfig.startSize = 5;
    waterSplashConfig.endSize = 3;
    waterSplashConfig.startSpeed = 150;
    waterSplashConfig.endSpeed = 50;
    waterSplashConfig.spread = 3.14f;
    waterSplashConfig.lifetime = 0.6f;
    waterSplashConfig.gravity = 400;
    waterSplashConfig.fadeOut = true;
    waterSplashConfig.shrink = true;
}

ParticleSystem::~ParticleSystem() {
}

void ParticleSystem::Update(float dt) {
    for (auto& particle : particles) {
        if (particle.active) {
            UpdateParticle(particle, dt);
        }
    }
}

void ParticleSystem::UpdateParticle(Particle& particle, float dt) {
    particle.life -= dt;
    
    if (particle.life <= 0) {
        particle.active = false;
        return;
    }
    
    float t = 1.0f - (particle.life / particle.maxLife);
    
    // Apply velocity
    particle.position.x += particle.velocity.x * dt;
    particle.position.y += particle.velocity.y * dt;
    
    // Apply gravity
    particle.velocity.y += 300 * dt;
    
    // Apply drag
    particle.velocity.x *= 0.98f;
    particle.velocity.y *= 0.98f;
}

void ParticleSystem::Draw(const GameCamera2D& camera) {
    for (const auto& particle : particles) {
        if (particle.active) {
            DrawParticle(particle, camera);
        }
    }
}

void ParticleSystem::DrawParticle(const Particle& particle, const GameCamera2D& /*camera*/) {
    Color color = particle.color;
    float size = particle.size;
    
    if (particle.fadeOut) color.a = (unsigned char)(color.a * (particle.life / particle.maxLife));
    if (particle.shrink) size *= (particle.life / particle.maxLife);
    
    DrawCircle(particle.position.x, particle.position.y, size, color);
}

void ParticleSystem::Emit(Vector2 position, ParticleConfig config, int count) {
    for (int i = 0; i < count; i++) {
        // Find inactive particle or create new one
        Particle* particle = nullptr;
        for (auto& p : particles) {
            if (!p.active) {
                particle = &p;
                break;
            }
        }
        
        if (!particle) {
            if (static_cast<int>(particles.size()) >= maxParticles) {
                // Remove oldest particle
                particles.erase(particles.begin());
            }
            particles.emplace_back();
            particle = &particles.back();
        }
        
        // Initialize particle
        float angle = GetRandomValue(0, 360) * DEG2RAD;
        if (config.spread < 6.28f) {
            angle = (GetRandomValue(0, 100) / 100.0f - 0.5f) * config.spread;
        }
        
        float speed = config.startSpeed + (config.endSpeed - config.startSpeed) * 
                      (GetRandomValue(0, 100) / 100.0f);
        
        particle->position = position;
        particle->velocity = {
            cosf(angle) * speed,
            sinf(angle) * speed
        };
        particle->life = config.lifetime;
        particle->maxLife = config.lifetime;
        particle->color = config.baseColor;
        particle->size = config.startSize;
        particle->active = true;
        particle->fadeOut = config.fadeOut;
        particle->shrink = config.shrink;
    }
}

void ParticleSystem::EmitBlood(Vector2 position, int amount) {
    Emit(position, bloodConfig, amount);
}

void ParticleSystem::EmitSpark(Vector2 position, Color color, int amount) {
    ParticleConfig spark = sparkConfig;
    spark.baseColor = color;
    spark.endColor = {
        static_cast<unsigned char>(color.r * 0.5f),
        static_cast<unsigned char>(color.g * 0.5f),
        static_cast<unsigned char>(color.b * 0.5f),
        color.a
    };
    Emit(position, spark, amount);
}

void ParticleSystem::EmitSmoke(Vector2 position, int amount) {
    Emit(position, smokeConfig, amount);
}

void ParticleSystem::EmitDust(Vector2 position, int amount) {
    Emit(position, dustConfig, amount);
}

void ParticleSystem::EmitCoinSparkle(Vector2 position) {
    Emit(position, coinSparkleConfig, 10);
}

void ParticleSystem::EmitExplosion(Vector2 position, int amount) {
    Emit(position, explosionConfig, amount);
}

void ParticleSystem::EmitMagic(Vector2 position, Color color, int amount) {
    ParticleConfig magic = magicConfig;
    magic.baseColor = color;
    magic.endColor = color;
    Emit(position, magic, amount);
}

void ParticleSystem::EmitWaterSplash(Vector2 position, int amount) {
    Emit(position, waterSplashConfig, amount);
}

void ParticleSystem::EmitJumpDust(Vector2 position) {
    EmitDust(position, 5);
}

void ParticleSystem::EmitLandDust(Vector2 position) {
    EmitDust(position, 8);
}

void ParticleSystem::EmitDeathExplosion(Vector2 position) {
    EmitExplosion(position, 30);
    EmitSmoke(position, 10);
}

void ParticleSystem::Clear() {
    particles.clear();
}

int ParticleSystem::GetActiveParticleCount() const {
    int count = 0;
    for (const auto& p : particles) {
        if (p.active) count++;
    }
    return count;
}

Color ParticleSystem::InterpolateColor(Color from, Color to, float t) {
    return {
        static_cast<unsigned char>(from.r + (to.r - from.r) * t),
        static_cast<unsigned char>(from.g + (to.g - from.g) * t),
        static_cast<unsigned char>(from.b + (to.b - from.b) * t),
        static_cast<unsigned char>(from.a + (to.a - from.a) * t)
    };
}
