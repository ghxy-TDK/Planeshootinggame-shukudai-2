#pragma once
#include "common.h"

// 所有游戏对象的基类
class GameObject {
public:
    float x, y, vx, vy;
    bool active_;
    Rect hitbox_;

    GameObject(float _x = 0, float _y = 0);
    virtual void update();
    virtual void draw() = 0;
    virtual void updateHitbox() = 0;
};