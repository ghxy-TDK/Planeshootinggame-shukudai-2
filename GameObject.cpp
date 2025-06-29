#include "GameObject.h"
GameObject::GameObject(float _x, float _y) : x(_x), y(_y), vx(0), vy(0), active_(true) {}

void GameObject::update() {
    x += vx;
    y += vy;
    updateHitbox();
}