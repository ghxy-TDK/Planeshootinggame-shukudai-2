#pragma once
#include "common.h"

// ������Ϸ����Ļ���
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