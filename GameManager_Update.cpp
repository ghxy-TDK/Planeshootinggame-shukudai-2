#include "GameManager.h"

// 游戏数据更新
void GameManager::updateGameObjects() {
    player_.update();

    for (auto it = bullets_.begin(); it != bullets_.end();) {
        it->update();
        if (!it->active_) it = bullets_.erase(it);
        else ++it;
    }

    for (auto it = enemies_.begin(); it != enemies_.end();) {
        it->update();
        if (!it->active_) {
            if (it->y > WINDOW_HEIGHT) score_ = max(0, score_ - 5);
            it = enemies_.erase(it);
        }
        else ++it;
    }

    for (auto it = powerups_.begin(); it != powerups_.end();) {
        it->update();
        if (!it->active_) it = powerups_.erase(it);
        else ++it;
    }

    for (auto it = explosions_.begin(); it != explosions_.end();) {
        it->update();
        if (!it->active_) it = explosions_.erase(it);
        else ++it;
    }
}

// 游戏主循环
void GameManager::update() {
    if (game_state_ != PLAYING) return;

    if (shield_active_ && shield_duration_ > 0) {
        shield_duration_--;
        player_.shield_timer_--;
        if (shield_duration_ < 60) player_.shield_active_ = (shield_duration_ / 5) % 2 == 0;
    }
    else if (shield_active_) {
        deactivateShieldSkill();
    }

    spawnEnemies();
    spawnPowerups();
    enemyShoot();
    globalDanmaku();
    updateGameObjects();
    checkCollisions();
}
