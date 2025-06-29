#pragma once
#include"common.h"
// 表示爆炸效果
class Explosion {
public:
    float x, y;
    int frame_, max_frames_;
    bool active_;

    Explosion(float _x, float _y);
    void update();
    void draw();
};

