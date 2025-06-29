#include "Player.h"
Player::Player(float _x, float _y) : GameObject(_x, _y), lives_(3), max_lives_(5), invulnerable_timer_(0),
damage_(1), speed_(5), invulnerable_(false), attack_speed_boost_timer_(0), shield_active_(false), shield_timer_(0) {}

void Player::loadImage() {
    loadimage(&player_img_, _T("res\\FEIJI.png"));
    img_width_ = player_img_.getwidth();
    img_height_ = player_img_.getheight();
}

void Player::update() {
    GameObject::update();
    if (invulnerable_ && --invulnerable_timer_ <= 0) invulnerable_ = false;
    if (attack_speed_boost_timer_ > 0) attack_speed_boost_timer_--;
}

void Player::draw() {
    if (!invulnerable_ || (invulnerable_timer_ / 3) % 2) {
        putimage(x - img_width_ / 2, y - img_height_ / 2, &player_img_, SRCAND);
        putimage(x - img_width_ / 2, y - img_height_ / 2, &player_img_, SRCPAINT);
    }
}

void Player::updateHitbox() {
    hitbox_ = { (int)x - img_width_ / 2, (int)y - img_height_ / 2, img_width_, img_height_ };
}

void Player::takeDamage() {
    if (!invulnerable_) {
        lives_--;
        invulnerable_ = true;
        invulnerable_timer_ = 90;
    }
}

void Player::heal() {
    if (lives_ < max_lives_) lives_++;
}

void Player::drawShield() {
    if (shield_active_) {

        // 绘制多个同心圆，颜色逐渐变浅，营造半透明效果
        setlinecolor(RGB(0, 150, 255));
        setlinestyle(PS_SOLID, 1);

        // 外圈 - 最浅
        circle(x, y, img_width_ - 40);
        circle(x, y, img_width_ - 43);

        // 中圈 - 中等
        setlinecolor(RGB(0, 180, 255));
        circle(x, y, img_width_ - 46);
        circle(x, y, img_width_ - 49);

        // 内圈 - 较亮
        setlinecolor(RGB(100, 220, 255));
        setlinestyle(PS_SOLID, 2);
        circle(x, y, img_width_ - 52);

        // 核心光环效果
        setlinecolor(RGB(150, 240, 255));
        setlinestyle(PS_SOLID, 1);
        circle(x, y, img_width_ -55);

        // 恢复默认设置
        setlinestyle(PS_SOLID, 1);
    }
}