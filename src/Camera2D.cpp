#include "Camera2D.h"

#ifdef _WIN32
#undef CloseWindow
#undef ShowCursor
#endif

GameCamera2D::GameCamera2D()
    : target{0, 0}
    , smoothTarget{0, 0}
    , bounds{0, 0, 0, 0}
    , targetZoom(1.0f)
    , smoothZoom(1.0f)
    , smoothSpeed(5.0f)
    , useSmoothFollow(true)
{
    raylibCamera.target = {0, 0};
    raylibCamera.offset = {SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f};
    raylibCamera.rotation = 0.0f;
    raylibCamera.zoom = 1.0f;
}

void GameCamera2D::Update(float dt) {
    // Smooth follow
    if (useSmoothFollow) {
        smoothTarget = LerpVec2(smoothTarget, target, dt * smoothSpeed);
        smoothZoom = Lerp(smoothZoom, targetZoom, dt * smoothSpeed);
    } else {
        smoothTarget = target;
        smoothZoom = targetZoom;
    }
    
    // Apply bounds
    if (bounds.width > 0 && bounds.height > 0) {
        float halfWidth = (SCREEN_WIDTH / 2) / smoothZoom;
        float halfHeight = (SCREEN_HEIGHT / 2) / smoothZoom;
        
        smoothTarget.x = fmaxf(bounds.x + halfWidth, fminf(smoothTarget.x, bounds.x + bounds.width - halfWidth));
        smoothTarget.y = fmaxf(bounds.y + halfHeight, fminf(smoothTarget.y, bounds.y + bounds.height - halfHeight));
    }
    
    // Apply screen shake
    Vector2 shake = screenShake.Update(dt);
    shakeOffset = shake;
    
    // Update raylib camera
    raylibCamera.target = smoothTarget;
    raylibCamera.zoom = smoothZoom;
}

void GameCamera2D::BeginDraw() {
    BeginMode2D(raylibCamera);
}

void GameCamera2D::EndDraw() {
    EndMode2D();
}

void GameCamera2D::SetTarget(Vector2 newTarget) {
    target = newTarget;
}

void GameCamera2D::SetBounds(Rectangle newBounds) {
    bounds = newBounds;
}

void GameCamera2D::SetZoom(float zoom) {
    targetZoom = fmaxf(0.5f, fminf(zoom, 3.0f));
}

void GameCamera2D::SetShake(float intensity, float duration) {
    screenShake.Start(intensity, duration);
}

Vector2 GameCamera2D::ScreenToWorld(Vector2 screenPos) const {
    return GetScreenToWorld2D(screenPos, raylibCamera);
}

Vector2 GameCamera2D::WorldToScreen(Vector2 worldPos) const {
    return GetWorldToScreen2D(worldPos, raylibCamera);
}

bool GameCamera2D::IsOnScreen(Vector2 worldPos, float margin) const {
    Vector2 screenPos = WorldToScreen(worldPos);
    return screenPos.x >= -margin && screenPos.x <= SCREEN_WIDTH + margin &&
           screenPos.y >= -margin && screenPos.y <= SCREEN_HEIGHT + margin;
}

bool GameCamera2D::IsOnScreen(Rectangle rect, float margin) const {
    Vector2 topLeft = WorldToScreen({rect.x, rect.y});
    Vector2 bottomRight = WorldToScreen({rect.x + rect.width, rect.y + rect.height});

    return bottomRight.x >= -margin && topLeft.x <= SCREEN_WIDTH + margin &&
           bottomRight.y >= -margin && topLeft.y <= SCREEN_HEIGHT + margin;
}
