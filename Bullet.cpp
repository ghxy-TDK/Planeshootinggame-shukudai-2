#include "Bullet.h"
Bullet::Bullet(float _x, float _y, int _type, float _vx, float _vy, int _damage)
    : GameObject(_x, _y), damage_(_damage), type_(_type) {
    vx = _vx; vy = _vy;
}

void Bullet::update() {
    GameObject::update();
    if (y < -20 || y > WINDOW_HEIGHT + 20 || x < -20 || x > WINDOW_WIDTH + 20) active_ = false;
}

void Bullet::draw() {
    if (type_ == 0) {
        setfillcolor(RGB(255, 255, 0));
        solidrectangle(x - 3, y - 12, x + 3, y + 12);
    }
    else if (type_ == 1) {
        setfillcolor(RGB(255, 100, 100));
        solidrectangle(x - 2, y - 6, x + 2, y + 6);
    }
    else if (type_ == 2) {
        setfillcolor(RGB(255, 50, 150));
        solidcircle(x, y, 3);
    }
    else if (type_ == 3) {
        setfillcolor(RGB(0, 255, 0));
        solidrectangle(x - 1, y - 30, x + 1, y);
    }
}

void Bullet::updateHitbox() {
    if (type_ == 2) hitbox_ = { (int)x - 3, (int)y - 3, 6, 6 };
    else hitbox_ = { (int)x - 2, (int)y - 6, 4, 12 };
}