#include "GameManager.h"

// Åö×²¼ì²âÂß¼­
void GameManager::checkCollisions() {
    for (auto& bullet : bullets_) {
        if (!bullet.active_ || (bullet.type_ != 0 && bullet.type_ != 3)) continue;
        for (auto& enemy : enemies_) {
            if (enemy.active_ && bullet.hitbox_.intersects(enemy.hitbox_)) {
                bullet.active_ = false;
                enemy.health_ -= bullet.damage_;
                if (bullet.type_ == 2) sound_manager_.playEffect("shoot");
                if (enemy.health_ <= 0) {
                    enemy.active_ = false;
                    explosions_.emplace_back(enemy.x, enemy.y);
                    sound_manager_.playEffect("explosion");
                    score_ += (enemy.enemy_type_ == 0) ? 10 : (enemy.enemy_type_ == 1) ? 30 : 100;
                    if (enemy.enemy_type_ == 2) {
                        boss_active_ = false;
                        boss_spawn_cooldown_ = 600;
                    }
                    int new_level = score_ / 200 + 1;
                    if (new_level > level_) {
                        level_ = new_level;
                        if (level_ % 3 == 0) player_.damage_++;
                        if (level_ % 4 == 0) player_.speed_ = min(10, player_.speed_ + 1);
                    }
                }
            }
        }
    }

    for (auto& bullet : bullets_) {
        if (!bullet.active_ || (bullet.type_ != 1 && bullet.type_ != 2)) continue;
        if (bullet.hitbox_.intersects(player_.hitbox_)) {
            bullet.active_ = false;
            if (!shield_active_) {
                player_.takeDamage();
                sound_manager_.playEffect("damage");
                if (player_.lives_ <= 0) {
                    game_state_ = GAME_OVER;
                    sound_manager_.stopBGM();
                    sound_manager_.playEffect("gameover");
                }
            }
            else {
                sound_manager_.playEffect("powerup");
            }
        }
    }

    for (auto& enemy : enemies_) {
        if (enemy.active_ && player_.hitbox_.intersects(enemy.hitbox_)) {
            enemy.active_ = false;
            explosions_.emplace_back(enemy.x, enemy.y);
            sound_manager_.playEffect("explosion");
            if (!shield_active_) {
                player_.takeDamage();
                sound_manager_.playEffect("damage");
                if (player_.lives_ <= 0) {
                    game_state_ = GAME_OVER;
                    sound_manager_.stopBGM();
                    sound_manager_.playEffect("gameover");
                }
            }
            else {
                sound_manager_.playEffect("powerup");
            }
        }
    }

    for (auto& powerup : powerups_) {
        if (powerup.active_ && powerup.hitbox_.intersects(player_.hitbox_)) {
            powerup.active_ = false;
            sound_manager_.playEffect("powerup");
            if (powerup.type_ == 0) player_.heal();
            else if (powerup.type_ == 1) score_ += 50;
            else if (powerup.type_ == 2) player_.attack_speed_boost_timer_ = 300;
        }
    }
}
