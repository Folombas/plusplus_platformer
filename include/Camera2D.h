#pragma once

#include "Common.h"
#include <raylib.h>

class GameCamera2D {
public:
    GameCamera2D();
    
    void Update(float dt);
    void BeginDraw();
    void EndDraw();
    
    void SetTarget(Vector2 target);
    void SetBounds(Rectangle bounds);
    void SetZoom(float zoom);
    void SetShake(float intensity, float duration);
    
    Vector2 GetTarget() const { return target; }
    Vector2 GetPosition() const { return raylibCamera.target; }
    float GetZoom() const { return raylibCamera.zoom; }
    float GetRotation() const { return raylibCamera.rotation; }
    
    void AddShakeOffset(Vector2 offset) { shakeOffset = offset; }
    
    // Screen to world conversion
    Vector2 ScreenToWorld(Vector2 screenPos) const;
    Vector2 WorldToScreen(Vector2 worldPos) const;
    
    bool IsOnScreen(Vector2 worldPos, float margin = 0) const;
    bool IsOnScreen(Rectangle rect, float margin = 0) const;
    
private:
    ::Camera2D raylibCamera;
    Vector2 target;
    Vector2 smoothTarget;
    Rectangle bounds;
    
    float targetZoom;
    float smoothZoom;
    
    ScreenShake screenShake;
    Vector2 shakeOffset;
    
    float smoothSpeed;
    bool useSmoothFollow;
};
