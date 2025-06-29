#include "PowerUp.h"
PowerUp::PowerUp(float _x, float _y, int _type) : GameObject(_x, _y), type_(_type), life_timer_(300) {
    vy = 2;
}

void PowerUp::update() {
    GameObject::update();
    if (y > WINDOW_HEIGHT + 20 || --life_timer_ <= 0) active_ = false;
}

void PowerUp::draw() {
    if (type_ == 0) {
        setfillcolor(RGB(0, 255, 0));
        solidcircle(x, y, 8);
        setfillcolor(RGB(255, 255, 255));
        outtextxy(x - 4, y - 6, _T("+"));
    }
    else if (type_ == 1) {
        setfillcolor(RGB(255, 255, 0));
        solidcircle(x, y, 8);
        setfillcolor(RGB(255, 255, 255));
        outtextxy(x - 4, y - 6, _T("$"));
    }
    else {
        setfillcolor(RGB(0, 200, 255));
        solidcircle(x, y, 8);
        setfillcolor(RGB(255, 255, 255));
        outtextxy(x - 4, y - 6, _T("S"));
    }
}

void PowerUp::updateHitbox() {
    hitbox_ = { (int)x - 8, (int)y - 8, 16, 16 };
}