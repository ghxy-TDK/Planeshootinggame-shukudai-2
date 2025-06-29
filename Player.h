#pragma once

#include "GameObject.h"

// 表示玩家的飞船
class Player : public GameObject {
public:
    int shield_timer_;
    int lives_, max_lives_, invulnerable_timer_, damage_, speed_;
    bool invulnerable_;
    bool shield_active_;
    int attack_speed_boost_timer_;
    IMAGE player_img_;
    int img_width_, img_height_;

    Player(float _x, float _y);
    void loadImage();
    void update() override;
    void draw() override;
    void updateHitbox() override;
    void takeDamage();
    void heal();
    void drawShield();
};


