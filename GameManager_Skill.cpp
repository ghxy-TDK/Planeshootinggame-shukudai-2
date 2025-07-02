#include "GameManager.h"

// 激光技能逻辑
void GameManager::activateLaserSkill() {
    laser_active_ = true;
    laser_duration_ = LASER_DURATION_TIME;
}

void GameManager::updateLaserSkill() {
    if (!laser_active_) return;

    if (laser_duration_ % 2 == 0) {
        MOUSEMSG m; bool mouse_found = false;
        while (MouseHit()) { m = GetMouseMsg(); mouse_found = true; }

        float tx = mouse_found ? m.x : player_.x;
        float ty = mouse_found ? m.y : player_.y - 100;

        float dx = tx - player_.x, dy = ty - player_.y;
        float dist = sqrt(dx * dx + dy * dy);
        if (dist < 1.0f) dist = 1.0f;

        bullets_.emplace_back(player_.x, player_.y - 20, 3, dx / dist * 15.0f, dy / dist * 15.0f, player_.damage_ * 1.5f);

        if (laser_duration_ % 10 == 0) sound_manager_.playEffect("shoot");
    }

    if (--laser_duration_ <= 0) {
        deactivateLaserSkill();
        laser_cooldown_ = LASER_COOLDOWN_TIME;
    }
}

void GameManager::deactivateLaserSkill() {
    laser_active_ = false;
    laser_duration_ = 0;
}

// 护盾技能逻辑
void GameManager::activateShieldSkill() {
    shield_active_ = true;
    shield_duration_ = SHIELD_DURATION_TIME;
    player_.shield_active_ = true;
    player_.shield_timer_ = SHIELD_DURATION_TIME;
    sound_manager_.playEffect("powerup");
}

void GameManager::deactivateShieldSkill() {
    shield_active_ = false;
    shield_duration_ = 0;
    player_.shield_active_ = false;
}

// 技能状态接口
bool GameManager::isPlayerShielded() { return shield_active_; }

GameManager::SkillStatus GameManager::getSkillStatus() {
    return {
        laser_cooldown_ <= 0, laser_cooldown_,
        shield_cooldown_ <= 0, shield_cooldown_,
        laser_active_, shield_active_,
        laser_duration_, shield_duration_
    };
}
