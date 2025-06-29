#include "Explosion.h"
Explosion::Explosion(float _x, float _y) : x(_x), y(_y), frame_(0), max_frames_(15), active_(true) {}

void Explosion::update() {
    if (++frame_ >= max_frames_) active_ = false;
}

void Explosion::draw() {
    int size = frame_ * 2;
    setfillcolor(RGB(255, 255 - frame_ * 15, 0));
    solidcircle(x, y, size);
    setfillcolor(RGB(255, 100, 0));
    solidcircle(x, y, size / 2);
}