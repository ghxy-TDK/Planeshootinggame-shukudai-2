#include "Enemy.h"
Enemy::Enemy(float _x, float _y, int type) : GameObject(_x, _y), enemy_type_(type), shoot_timer_(0), danmaku_timer_(0) {
    vy = 2 + rand() % 3;
    if (rand() % 2) vx = (rand() % 3) - 1;

    if (type == 0) { max_health_ = health_ = 1; bullet_pattern_ = 0; }
    else if (type == 1) { max_health_ = health_ = 5; bullet_pattern_ = rand() % 2; }
    else { max_health_ = health_ = 15; vy = 1; bullet_pattern_ = rand() % 3; }
}

void Enemy::update() {
    GameObject::update();
    if (y > WINDOW_HEIGHT + 50) active_ = false;
    if (x <= 20 || x >= WINDOW_WIDTH - 20) vx = -vx;
    shoot_timer_++;
    danmaku_timer_++;
}

void Enemy::draw() {
    int r = 255, g = 0, b = 0;
    if (enemy_type_ == 1) { r = 255; g = 100; b = 0; }
    else if (enemy_type_ == 2) { r = 150; g = 0; b = 255; }

    setfillcolor(RGB(r, g, b));
    int w = (enemy_type_ == 2) ? 25 : (enemy_type_ == 1) ? 18 : 12;
    int h = (enemy_type_ == 2) ? 20 : (enemy_type_ == 1) ? 16 : 12;
    solidrectangle(x - w, y - h, x + w, y + h);

    if (health_ < max_health_ && enemy_type_ != 2) {
        setfillcolor(RGB(255, 0, 0));
        solidrectangle(x - w, y - h - 8, x + w, y - h - 5);
        setfillcolor(RGB(0, 255, 0));
        int health_width = (2 * w * health_) / max_health_;
        solidrectangle(x - w, y - h - 8, x - w + health_width, y - h - 5);
    }
}

void Enemy::updateHitbox() {
    int w = (enemy_type_ == 2) ? 25 : (enemy_type_ == 1) ? 18 : 12;
    int h = (enemy_type_ == 2) ? 20 : (enemy_type_ == 1) ? 16 : 12;
    hitbox_ = { (int)x - w, (int)y - h, 2 * w, 2 * h };
}

bool Enemy::canShoot() { return shoot_timer_ >= (enemy_type_ == 2 ? 30 : 60); }
bool Enemy::canDanmaku() { return danmaku_timer_ >= (enemy_type_ == 2 ? 120 : 240); }
void Enemy::resetShoot() { shoot_timer_ = 0; }
void Enemy::resetDanmaku() { danmaku_timer_ = 0; }