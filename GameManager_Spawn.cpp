#include "GameManager.h"

void GameManager::spawnEnemies() {
    enemy_spawn_timer_++;
    int spawn_rate = max(20, static_cast<int>(50.0f - level_ * 3.0f));

    if (enemy_spawn_timer_ >= spawn_rate && enemies_.size() < MAX_ENEMIES) {
        float x = 50 + rand() % (WINDOW_WIDTH - 300);
        int type = 0;
        if (level_ >= 3 && rand() % 100 < 15) type = 1;
        if (level_ >= 5 && rand() % 100 < 8 && !boss_active_ && boss_spawn_cooldown_ <= 0) {
            type = 2;
            boss_active_ = true;
        }

        Enemy enemy(x, -30, type);
        enemy.loadImage();

        if (type == 2) enemy.health_ = enemy.max_health_ += (level_ - 1) * 10;
        else if (type == 1) enemy.health_ = enemy.max_health_ += (level_ - 1) * 2;

        enemies_.push_back(enemy);
        enemy_spawn_timer_ = 0;
    }
}

void GameManager::spawnPowerups() {
    powerup_timer_++;
    if (powerup_timer_ >= 240) {
        float x = 100.0f + static_cast<float>(rand() % (WINDOW_WIDTH - 400));
        int type = (rand() % 10 < 3) ? 0 : (rand() % 10 < 8) ? 1 : 2;
        powerups_.emplace_back(x, -20.0f, static_cast<float>(type));
        powerup_timer_ = 0;
    }
}

void GameManager::enemyShoot() {
    for (auto& enemy : enemies_) {
        if (enemy.active_ && enemy.canShoot()) {
            bullets_.emplace_back(enemy.x, enemy.y + 20, 1, 0, 4);
            enemy.resetShoot();
            sound_manager_.playEffect("shoot");
        }
    }
}

void GameManager::globalDanmaku() {
    danmaku_global_timer_++;
    int interval = max(120, static_cast<int>(300.0f - level_ * 20.0f)); 

    if (danmaku_global_timer_ >= interval) {
        for (auto& enemy : enemies_) {
            if (!enemy.active_) continue;

            if (enemy.enemy_type_ == 2) {
                int pattern = enemy.bullet_pattern_;
                if (pattern == 0) {
                    int n = 16 + level_ * 2;
                    for (int i = 0; i < n; i++) {
                        float a = 2 * 3.14159f * i / n;
                        bullets_.emplace_back(enemy.x, enemy.y, 2, cos(a) * (2 + level_ * 0.5f), sin(a) * (2 + level_ * 0.5f));
                    }
                }
                else if (pattern == 1) {
                    int n = 5 + level_;
                    float fan = 60.0f * 3.14159f / 180.0f;
                    float start = atan2(player_.y - enemy.y, player_.x - enemy.x) - fan / 2;
                    for (int i = 0; i < n; i++) {
                        float a = start + fan * i / (n - 1);
                        bullets_.emplace_back(enemy.x, enemy.y, 2, cos(a) * (2 + level_ * 0.75f), sin(a) * (2 + level_ * 0.75f));
                    }
                }
                else {
                    for (int i = 0; i < 3; i++) {
                        float dx = player_.x - enemy.x;
                        float dy = player_.y - enemy.y;
                        float dist = sqrt(dx * dx + dy * dy);
                        if (dist < 1.0f) dist = 1.0f;
                        bullets_.emplace_back(enemy.x, enemy.y, 2, dx / dist * (1.5f + level_ * 0.5f), dy / dist * (1.5f + level_ * 0.5f));
                    }
                }
            }
            else if (enemy.enemy_type_ == 1) {
                int n = 4 + level_;
                for (int i = 0; i < n; i++) {
                    float a = 2 * 3.14159f * i / n;
                    bullets_.emplace_back(enemy.x, enemy.y, 2, cos(a) * (1.5f + level_ * 0.5f), sin(a) * (1.5f + level_ * 0.5f));
                }
            }
            else {
                int n = 2 + level_;
                for (int i = 0; i < n; i++) {
                    float a = 2 * 3.14159f * i / n;
                    bullets_.emplace_back(enemy.x, enemy.y, 2, cos(a) * (1 + level_ * 0.25f), sin(a) * (1 + level_ * 0.25f));
                }
            }
            sound_manager_.playEffect("shoot");
        }
        danmaku_global_timer_ = 0;
    }
}