#pragma once

#include "GameObject.h"
#include"common.h"
// ��ʾ�ӵ�
class Bullet : public GameObject {
public:
    int damage_, type_; // 0=����ӵ�, 1=������ͨ�ӵ�, 2=��Ļ�ӵ�

    Bullet(float _x, float _y, int _type = 0, float _vx = 0, float _vy = -8, int _damage = 1);
    void update() override;
    void draw() override;
    void updateHitbox() override;
};

