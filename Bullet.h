#pragma once

#include "GameObject.h"
#include"common.h"
// 表示子弹
class Bullet : public GameObject {
public:
    int damage_, type_; // 0=玩家子弹, 1=敌人普通子弹, 2=弹幕子弹

    Bullet(float _x, float _y, int _type = 0, float _vx = 0, float _vy = -8, int _damage = 1);
    void update() override;
    void draw() override;
    void updateHitbox() override;
};

