#pragma once
#include "GameObject.h"
#include"common.h"
// 表示敌机
class Enemy : public GameObject {
public:
    int health_, max_health_, enemy_type_, shoot_timer_, danmaku_timer_;
    int bullet_pattern_; // 敌人的弹幕模式

    Enemy(float _x, float _y, int type = 0);
    void update() override;
    void draw() override;
    void updateHitbox() override;
    bool canShoot();
    bool canDanmaku();
    void resetShoot();
    void resetDanmaku();
};
