#pragma once
#include "GameObject.h"
#include"common.h"
// 表示道具
class PowerUp : public GameObject {
public:
    int type_, life_timer_; // 0=生命, 1=分数, 2=攻击速度

    PowerUp(float _x, float _y, int _type = 0);
    void update() override;
    void draw() override;
    void updateHitbox() override;
};

