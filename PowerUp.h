#pragma once
#include "GameObject.h"
#include"common.h"
// ��ʾ����
class PowerUp : public GameObject {
public:
    int type_, life_timer_; // 0=����, 1=����, 2=�����ٶ�

    PowerUp(float _x, float _y, int _type = 0);
    void update() override;
    void draw() override;
    void updateHitbox() override;
};

