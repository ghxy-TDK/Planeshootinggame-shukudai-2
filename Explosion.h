#pragma once
#include"common.h"
// ��ʾ��ըЧ��
class Explosion {
public:
    float x, y;
    int frame_, max_frames_;
    bool active_;

    Explosion(float _x, float _y);
    void update();
    void draw();
};

