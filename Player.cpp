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
        setfillcolor(BLUE);
        solidcircle(x, y, img_width_ + 15);
        setlinecolor(RGB(0, 200, 255));
        setlinestyle(PS_SOLID, 3);
        circle(x, y, img_width_ + 15);
        setlinestyle(PS_SOLID, 1);
    }
}