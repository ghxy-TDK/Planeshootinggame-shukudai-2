#include "Enemy.h"
Enemy::Enemy(float _x, float _y, int type) : GameObject(_x, _y), enemy_type_(type), shoot_timer_(0), danmaku_timer_(0) {
    vy = 2 + rand() % 3;
    if (rand() % 2) vx = (rand() % 3) - 1;

    if (type == 0) { max_health_ = health_ = 1; bullet_pattern_ = 0; }
    else if (type == 1) { max_health_ = health_ = 5; bullet_pattern_ = rand() % 2; }
    else { max_health_ = health_ = 15; vy = 1; bullet_pattern_ = rand() % 3; }
}

void Enemy::loadImage() {
    // 根据敌人类型加载不同图片
    switch (enemy_type_) {
    case 0:
        loadimage(&enemy_img_, _T("res\\enemy002.png"));
        break;
    case 1:
        loadimage(&enemy_img_, _T("res\\enemy001.png"));
        break;
    case 2:
        loadimage(&enemy_img_, _T("res\\BOSS.png"));
        break;
    }

    img_width_ = enemy_img_.getwidth();
    img_height_ = enemy_img_.getheight();
}
void Enemy::update() {
    GameObject::update();
    if (y > WINDOW_HEIGHT + 50) active_ = false;
    if (x <= 20 || x >= WINDOW_WIDTH - 20) vx = -vx;
    shoot_timer_++;
    danmaku_timer_++;
}

void Enemy::draw() {
    // 使用图片绘制敌人
    putimage(x - img_width_ / 2, y - img_height_ / 2, &enemy_img_, SRCAND);
    putimage(x - img_width_ / 2, y - img_height_ / 2, &enemy_img_, SRCPAINT);

    // 绘制血条(精英和Boss显示)
    if (health_ < max_health_ && enemy_type_ > 0) {
        int barWidth = img_width_ / 2;
        int barHeight = 5;
        int barX = x - barWidth / 2;
        int barY = y - img_height_ / 2 - 10;

        // 背景(剩余血量)
        setfillcolor(RGB(255, 0, 0));
        solidrectangle(barX, barY, barX + barWidth, barY + barHeight);

        // 当前血量
        setfillcolor(RGB(0, 255, 0));
        int currentWidth = (barWidth * health_) / max_health_;
        solidrectangle(barX, barY, barX + currentWidth, barY + barHeight);
    }
}

void Enemy::updateHitbox() {
    hitbox_ = {
        (int)x - img_width_ / 2,
        (int)y - img_height_ / 2,
        img_width_,
        img_height_
    };
}
bool Enemy::canShoot() { return shoot_timer_ >= (enemy_type_ == 2 ? 30 : 60); }
bool Enemy::canDanmaku() { return danmaku_timer_ >= (enemy_type_ == 2 ? 120 : 240); }
void Enemy::resetShoot() { shoot_timer_ = 0; }
void Enemy::resetDanmaku() { danmaku_timer_ = 0; }