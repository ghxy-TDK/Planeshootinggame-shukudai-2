#include "GameManager.h"


GameManager::GameManager() :
    player_(WINDOW_WIDTH / 2, WINDOW_HEIGHT - 80),
    game_state_(MENU),
    score_(0),
    level_(1),
    enemy_spawn_timer_(0),
    bullet_cooldown_(0),
    powerup_timer_(0),
    danmaku_global_timer_(0),
    boss_spawn_cooldown_(0),
    boss_active_(false),
    running_(true),
    firing_toggle_(false),
    laser_active_(false),
    laser_duration_(0),
    laser_cooldown_(0),
    shield_active_(false),
    shield_duration_(0),
    shield_cooldown_(0)
{
    initStars();  // 初始化星星背景
    memset(key_state_, 0, sizeof(key_state_));  // 初始化按键状态数组
    player_.loadImage();// 加载玩家图像
    
}

void GameManager::handleInput() {
    for (int i = 0; i < 256; i++) key_state_[i] = (GetAsyncKeyState(i) & 0x8000) != 0;

    if (game_state_ == MENU && (key_state_[VK_RETURN] || key_state_[VK_SPACE])) {
        game_state_ = PLAYING;
        resetGame();
        sound_manager_.playBGM();
    }
    else if (game_state_ == PLAYING) {
        int speed = player_.speed_;
        if (key_state_['A'] || key_state_[VK_LEFT]) player_.x = max(20.0f, player_.x - speed);
        if (key_state_['D'] || key_state_[VK_RIGHT]) player_.x = min((float)WINDOW_WIDTH - 20, player_.x + speed);
        if (key_state_['W'] || key_state_[VK_UP]) player_.y = max(20.0f, player_.y - speed);
        if (key_state_['S'] || key_state_[VK_DOWN]) player_.y = min((float)WINDOW_HEIGHT - 20, player_.y + speed);

        static bool prev_j_state = false;
        static bool prev_space_state = false;
        bool current_j_state = (GetAsyncKeyState('J') & 0x8000) != 0;
        bool current_space_state = (GetAsyncKeyState(VK_SPACE) & 0x8000) != 0;

        if ((current_j_state && !prev_j_state) || (current_space_state && !prev_space_state)) {
            firing_toggle_ = !firing_toggle_;
            Sleep(150);
        }
        prev_j_state = current_j_state;
        prev_space_state = current_space_state;

        static bool prev_e_state = false;
        bool current_e_state = (GetAsyncKeyState('E') & 0x8000) != 0;
        if (current_e_state && !prev_e_state && laser_cooldown_ <= 0) {
            activateLaserSkill();
            laser_cooldown_ = LASER_COOLDOWN_TIME;
            sound_manager_.playEffect("shoot");
            Sleep(100);
        }
        prev_e_state = current_e_state;

        static bool prev_q_state = false;
        bool current_q_state = (GetAsyncKeyState('Q') & 0x8000) != 0;
        if (current_q_state && !prev_q_state && shield_cooldown_ <= 0) {
            activateShieldSkill();
            shield_cooldown_ = SHIELD_COOLDOWN_TIME;
            sound_manager_.playEffect("shoot");
            Sleep(100);
        }
        prev_q_state = current_q_state;

        if (laser_active_ && laser_duration_ > 0) {
            updateLaserSkill();
            laser_duration_--;
        }
        else if (laser_active_) {
            deactivateLaserSkill();
        }

        if (shield_active_ && shield_duration_ > 0) {
            shield_duration_--;
        }
        else if (shield_active_) {
            deactivateShieldSkill();
        }

        if (firing_toggle_ && bullet_cooldown_ <= 0 && !laser_active_) {
            MOUSEMSG m;
            bool mouse_found = false;
            while (MouseHit()) {
                m = GetMouseMsg();
                mouse_found = true;
            }
            float target_x = player_.x;
            float target_y = player_.y - 100;
            if (mouse_found) {
                target_x = m.x;
                target_y = m.y;
            }
            float dx = target_x - player_.x;
            float dy = target_y - player_.y;
            float distance = sqrt(dx * dx + dy * dy);
            if (distance < 1.0f) distance = 1.0f;
            float bullet_speed = 10.0f;
            bullets_.push_back(Bullet(player_.x, player_.y - 20, 0, (dx / distance) * bullet_speed, (dy / distance) * bullet_speed, player_.damage_));
            bullet_cooldown_ = (player_.attack_speed_boost_timer_ > 0) ? 3 : 6;
            sound_manager_.playEffect("shoot");
        }

        if (key_state_['P']) {
            game_state_ = PAUSED;
            Sleep(200);
        }

        if (key_state_['M']) {
            sound_manager_.toggleSound();
            Sleep(200);
        }
    }
    else if (game_state_ == PAUSED && key_state_['P']) {
        game_state_ = PLAYING;
        Sleep(200);
    }
    else if (game_state_ == GAME_OVER && key_state_['R']) {
        game_state_ = MENU;
    }

    if (bullet_cooldown_ > 0) bullet_cooldown_--;
    if (laser_cooldown_ > 0) laser_cooldown_--;
    if (shield_cooldown_ > 0) shield_cooldown_--;
}

void GameManager::activateLaserSkill() {
    laser_active_ = true;
    laser_duration_ = LASER_DURATION_TIME;
}

void GameManager::updateLaserSkill() {
    if (!laser_active_) return;

    if (laser_duration_ % 2 == 0) {
        MOUSEMSG m;
        bool mouse_found = false;
        while (MouseHit()) {
            m = GetMouseMsg();
            mouse_found = true;
        }

        float target_x = player_.x;
        float target_y = player_.y - 100;
        if (mouse_found) {
            target_x = m.x;
            target_y = m.y;
        }

        float dx = target_x - player_.x;
        float dy = target_y - player_.y;
        float distance = sqrt(dx * dx + dy * dy);
        if (distance < 1.0f) distance = 1.0f;

        float bullet_speed = 15.0f;
        float laser_damage = player_.damage_ * 1.5f;

        bullets_.push_back(Bullet(player_.x, player_.y - 20, 3,
            (dx / distance) * bullet_speed,
            (dy / distance) * bullet_speed,
            laser_damage));

        if (laser_duration_ % 10 == 0) {
            sound_manager_.playEffect("shoot");
        }
    }

    laser_duration_--;
    if (laser_duration_ <= 0) {
        deactivateLaserSkill();
        laser_cooldown_ = LASER_COOLDOWN_TIME;
    }
}

void GameManager::deactivateLaserSkill() {
    laser_active_ = false;
    laser_duration_ = 0;
}

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

bool GameManager::isPlayerShielded() {
    return shield_active_;
}

GameManager::SkillStatus GameManager::getSkillStatus() {
    return {
        laser_cooldown_ <= 0,
        laser_cooldown_,
        shield_cooldown_ <= 0,
        shield_cooldown_,
        laser_active_,
        shield_active_,
        laser_duration_,
        shield_duration_
    };
}

void GameManager::drawMenu() {
    cleardevice();
    updateStars();

    drawGradientText(WINDOW_WIDTH / 2 - 250, 100, _T("宇 宙 飞 机 大 战"), 60,
        RGB(0, 150, 255), RGB(150, 0, 255), 1.3f);

    setlinecolor(RGB(100, 150, 255));
    line(WINDOW_WIDTH / 2 - 220, 180, WINDOW_WIDTH / 2 + 220, 180);

    settextcolor(RGB(100, 200, 255));
    settextstyle(24, 0, _T("微软雅黑"));
    outtextxy(WINDOW_WIDTH / 2 - 140, 200, _T("穿越星际的终极战斗"));

    drawBlinkingText(WINDOW_WIDTH / 2 - 120, 280, _T("按Enter键开始游戏"), 30, RGB(255, 255, 0), RGB(255, 100, 0), 30);

    settextcolor(RGB(200, 200, 255));
    settextstyle(18, 0, _T("Arial"));
    outtextxy(WINDOW_WIDTH / 2 - 120, 350, _T("控制方式:"));
    outtextxy(WINDOW_WIDTH / 2 - 120, 380, _T("WASD/方向键: 移动飞船"));
    outtextxy(WINDOW_WIDTH / 2 - 120, 410, _T("空格/J键: 发射子弹"));
    outtextxy(WINDOW_WIDTH / 2 - 120, 440, _T("E键: 发射激光"));
    outtextxy(WINDOW_WIDTH / 2 - 120, 470, _T("Q键: 展开护盾"));
    outtextxy(WINDOW_WIDTH / 2 - 120, 500, _T("P键: 暂停游戏"));

    settextcolor(sound_manager_.isSoundEnabled() ?
        RGB(150, 255, 150) :
        RGB(255, 100, 100));
    settextstyle(16, 0, _T("微软雅黑"));

    TCHAR soundText[50];
    _stprintf_s(soundText, _T("%s 声音: %s (M键切换)"),
        sound_manager_.isSoundEnabled() ? _T("🔊") : _T("🔇"),
        sound_manager_.isSoundEnabled() ? _T("开") : _T("关"));
    outtextxy(WINDOW_WIDTH / 2 - textwidth(soundText) / 2 - 70, 530, soundText);

    settextcolor(RGB(150, 255, 150));
    settextstyle(18, 0, _T("微软雅黑"));
    outtextxy(WINDOW_WIDTH / 2 - 120, 560, _T("收集能量道具增强你的飞船!"));
}

void GameManager::drawGame() {
    player_.draw();
    player_.drawShield();
    for (auto& bullet : bullets_) bullet.draw();
    for (auto& enemy : enemies_) enemy.draw();
    for (auto& powerup : powerups_) powerup.draw();
    for (auto& explosion : explosions_) explosion.draw();
    drawUI();
}

void GameManager::drawUI() {
    settextcolor(RGB(255, 255, 255));
    settextstyle(18, 0, _T("微软雅黑"));

    TCHAR text[100];
    _stprintf_s(text, _T("分数: %d  等级: %d  剩余生命: %d"), score_, level_, player_.lives_);
    outtextxy(WINDOW_WIDTH - 300, 50, text);

    _stprintf_s(text, _T("伤害: %d  移速: %d"), player_.damage_, player_.speed_);
    outtextxy(WINDOW_WIDTH - 300, 75, text);

    if (player_.attack_speed_boost_timer_ > 0) {
        _stprintf_s(text, _T("攻速提升效果: %.1f 秒"), player_.attack_speed_boost_timer_ / 60.0f);
        outtextxy(10, 60, text);
    }

    SkillStatus skill_status = getSkillStatus();

    if (skill_status.laser_active) {
        settextcolor(RGB(255, 100, 100));
        _stprintf_s(text, _T("激光子弹效果: %.1f 秒"), skill_status.laser_duration_remaining / 60.0f);
        outtextxy(10, 80, text);
    }
    else if (skill_status.laser_ready) {
        settextcolor(RGB(100, 255, 100));
        _stprintf_s(text, _T("激光子弹就绪 (E)"));
        outtextxy(10, 80, text);
    }
    else {
        settextcolor(RGB(255, 255, 100));
        _stprintf_s(text, _T("激光子弹冷却: %.1f 秒"), skill_status.laser_cooldown_remaining / 60.0f);
        outtextxy(10, 80, text);
    }

    if (skill_status.shield_active) {
        settextcolor(RGB(100, 100, 255));
        _stprintf_s(text, _T("护盾效果: %.1f 秒"), skill_status.shield_duration_remaining / 60.0f);
        outtextxy(10, 100, text);
    }
    else if (skill_status.shield_ready) {
        settextcolor(RGB(100, 255, 100));
        _stprintf_s(text, _T("护盾就绪 (Q)"));
        outtextxy(10, 100, text);
    }
    else {
        settextcolor(RGB(255, 255, 100));
        _stprintf_s(text, _T("护盾效果冷却: %.1f 秒"), skill_status.shield_cooldown_remaining / 60.0f);
        outtextxy(10, 100, text);
    }

    settextcolor(RGB(200, 200, 100));
    _stprintf_s(text, _T("声音: %s (M)"), sound_manager_.isSoundEnabled() ? _T("开") : _T("关"));
    outtextxy(WINDOW_WIDTH - textwidth(text) - 10, 10, text);

    for (auto& enemy : enemies_) {
        if (enemy.active_ && enemy.enemy_type_ == 2) {
            settextcolor(RGB(255, 255, 255));
            settextstyle(20, 0, _T("Arial"));
            _stprintf_s(text, _T("BOSS: %d/%d"), enemy.health_, enemy.max_health_);
            outtextxy(WINDOW_WIDTH / 2 - textwidth(text) / 2, 60, text);

            int boss_bar_width = 400;
            int boss_bar_height = 15;
            int boss_bar_x = WINDOW_WIDTH / 2 - boss_bar_width / 2;
            int boss_bar_y = 90;

            setfillcolor(RGB(255, 0, 0));
            solidrectangle(boss_bar_x, boss_bar_y, boss_bar_x + boss_bar_width, boss_bar_y + boss_bar_height);

            setfillcolor(RGB(0, 255, 0));
            int current_boss_health_width = (boss_bar_width * enemy.health_) / enemy.max_health_;
            solidrectangle(boss_bar_x, boss_bar_y, boss_bar_x + current_boss_health_width, boss_bar_y + boss_bar_height);
            break;
        }
    }
}

void GameManager::drawPauseScreen() {
    setfillcolor(RGB(0, 0, 0, 180));
    solidrectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    settextstyle(45, 0, _T("Arial"));
    int paused_text_width = textwidth(_T("PAUSED"));
    int paused_x = WINDOW_WIDTH / 2 - paused_text_width / 2;
    int paused_y = WINDOW_HEIGHT / 2 - 50;

    drawBlinkingText(paused_x, paused_y, _T("PAUSED"), 45, RGB(255, 255, 0), RGB(255, 100, 0), 30);

    settextcolor(RGB(150, 200, 255));
    settextstyle(24, 0, _T("微软雅黑"));

    int continue_text_width = textwidth(_T("按P键继续游戏"));
    int continue_x = WINDOW_WIDTH / 2 - continue_text_width / 2;
    int continue_y = paused_y + textheight(_T("PAUSED")) + 20;
    outtextxy(continue_x, continue_y, _T("按P键继续游戏"));

    settextcolor(sound_manager_.isSoundEnabled() ?
        RGB(150, 255, 150) :
        RGB(255, 100, 100));
    settextstyle(16, 0, _T("微软雅黑"));

    TCHAR sound_status_text[50];
    _stprintf_s(sound_status_text, _T("%s 声音: %s (M键切换)"),
        sound_manager_.isSoundEnabled() ? _T("🔊") : _T("🔇"),
        sound_manager_.isSoundEnabled() ? _T("开") : _T("关"));
    int sound_text_width = textwidth(sound_status_text);
    int sound_x = WINDOW_WIDTH / 2 - sound_text_width / 2;
    int sound_y = continue_y + textheight(_T("按P键继续游戏")) + 30;
    outtextxy(sound_x, sound_y, sound_status_text);
}

void GameManager::drawGameOver() {
    setfillcolor(RGB(0, 0, 0, 180));
    solidrectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    settextstyle(60, 0, _T("微软雅黑"));
    int game_over_x = WINDOW_WIDTH / 2 - textwidth(_T("GAME OVER")) / 2;
    drawGradientText(game_over_x, 100, _T("GAME OVER"), 60,
        RGB(255, 0, 0), RGB(150, 0, 0), 1.3f);

    setlinecolor(RGB(255, 100, 100));
    line(WINDOW_WIDTH / 2 - 250, 190, WINDOW_WIDTH / 2 + 250, 190);

    settextcolor(RGB(255, 255, 50));
    settextstyle(36, 0, _T("微软雅黑"));
    TCHAR final_score_text[100];
    _stprintf_s(final_score_text, _T("最终得分: %d"), score_);
    int score_x = WINDOW_WIDTH / 2 - textwidth(final_score_text) / 2;
    outtextxy(score_x, 240, final_score_text);

    settextcolor(RGB(100, 255, 100));
    settextstyle(28, 0, _T("微软雅黑"));
    TCHAR level_reached_text[100];
    _stprintf_s(level_reached_text, _T("达到等级: %d"), level_);
    int level_x = WINDOW_WIDTH / 2 - textwidth(level_reached_text) / 2;
    outtextxy(level_x, 300, level_reached_text);

    settextstyle(32, 0, _T("微软雅黑"));
    int return_menu_x = WINDOW_WIDTH / 2 - textwidth(_T("按R键返回主菜单")) / 2;
    drawBlinkingText(return_menu_x, 380, _T("按R键返回主菜单"), 32,
        RGB(0, 255, 255), RGB(0, 150, 200), 30);

    settextcolor(sound_manager_.isSoundEnabled() ?
        RGB(150, 255, 150) :
        RGB(255, 100, 100));
    settextstyle(16, 0, _T("微软雅黑"));
    TCHAR sound_status_text[50];
    _stprintf_s(sound_status_text, _T("%s 声音: %s (M键切换)"),
        sound_manager_.isSoundEnabled() ? _T("🔊") : _T("🔇"),
        sound_manager_.isSoundEnabled() ? _T("开") : _T("关"));
    int sound_status_x = WINDOW_WIDTH / 2 - textwidth(sound_status_text) / 2;
    outtextxy(sound_status_x, 450, sound_status_text);

    settextcolor(RGB(180, 255, 180));
    settextstyle(20, 0, _T("微软雅黑"));
    int game_tip_x = WINDOW_WIDTH / 2 - textwidth(_T("收集能量道具增强你的飞船！")) / 2;
    outtextxy(game_tip_x, 520, _T("收集能量道具增强你的飞船！"));
}

void GameManager::drawGradientText(int x, int y, LPCTSTR text, int fontSize,
    COLORREF startColor, COLORREF endColor, float spacingFactor) {
    settextstyle(fontSize, 0, _T("微软雅黑"));

    int length = _tcslen(text);
    int step = 255 / (length > 1 ? length - 1 : 1);

    for (int i = 0; i < length; ++i) {
        int r = GetRValue(startColor) + (GetRValue(endColor) - GetRValue(startColor)) * i / (length - 1);
        int g = GetGValue(startColor) + (GetGValue(endColor) - GetGValue(startColor)) * i / (length - 1);
        int b = GetBValue(endColor) + (GetBValue(startColor) - GetBValue(endColor)) * i / (length - 1);
        settextcolor(RGB(r, g, b));

        TCHAR ch[2] = { text[i], 0 };
        outtextxy(x + i * fontSize * 0.7 * spacingFactor, y, ch);
    }
}

void GameManager::drawBlinkingText(int x, int y, LPCTSTR text, int fontSize, COLORREF color1, COLORREF color2, int time) {
    static int counter = 0;
    counter++;

    if ((counter / time) % 2 == 0) {
        settextcolor(color1);
    }
    else {
        settextcolor(color2);
    }

    settextstyle(fontSize, 0, _T("Arial"));
    outtextxy(x, y, text);
}

void GameManager::resetGame() {
    player_ = Player(WINDOW_WIDTH / 2, WINDOW_HEIGHT - 80);
    player_.loadImage();
    bullets_.clear();
    enemies_.clear();
    powerups_.clear();
    explosions_.clear();
    score_ = 0;
    level_ = 1;
    enemy_spawn_timer_ = bullet_cooldown_ = powerup_timer_ = danmaku_global_timer_ = 0;
    boss_spawn_cooldown_ = 0;
    boss_active_ = false;
    laser_active_ = false;
    laser_duration_ = 0;
    laser_cooldown_ = 0;
    shield_active_ = false;
    shield_duration_ = 0;
    shield_cooldown_ = 0;
}

void GameManager::spawnEnemies() {
    enemy_spawn_timer_++;
    int spawn_rate = max(20, 50 - level_ * 3);

    if (enemy_spawn_timer_ >= spawn_rate && enemies_.size() < MAX_ENEMIES) {
        float x = 50 + rand() % (WINDOW_WIDTH - 100);
        int type = 0;

        if (level_ >= 3 && rand() % 100 < 15) type = 1;
        if (level_ >= 5 && rand() % 100 < 8) {
            if (!boss_active_ && boss_spawn_cooldown_ <= 0) {
                type = 2;
                boss_active_ = true;
            }
        }

        Enemy enemy(x, -30, type);
        enemy.loadImage(); // 确保新敌人生成时加载图片

        // 根据类型设置属性
        if (type == 2) {
            enemy.health_ = enemy.max_health_ = enemy.health_ + (level_ - 1) * 10;
        }
        else if (type == 1) {
            enemy.health_ = enemy.max_health_ = enemy.health_ + (level_ - 1) * 2;
        }

        enemies_.push_back(enemy);
        enemy_spawn_timer_ = 0;
    }
}

void GameManager::spawnPowerups() {
    powerup_timer_++;
    if (powerup_timer_ >= 240) {
        float x = 100 + rand() % (WINDOW_WIDTH - 200);
        int type_choice = rand() % 10;
        int type;
        if (type_choice < 3) type = 0;
        else if (type_choice < 8) type = 1;
        else type = 2;
        powerups_.push_back(PowerUp(x, -20, type));
        powerup_timer_ = 0;
    }
}

void GameManager::enemyShoot() {
    for (auto& enemy : enemies_) {
        if (enemy.active_ && enemy.canShoot()) {
            bullets_.push_back(Bullet(enemy.x, enemy.y + 20, 1, 0, 4));
            enemy.resetShoot();
            sound_manager_.playEffect("shoot");
        }
    }
}

void GameManager::globalDanmaku() {
    danmaku_global_timer_++;
    int interval = max(120, 300 - level_ * 20);
    int bullet_level = level_;

    if (danmaku_global_timer_ >= interval) {
        for (auto& enemy : enemies_) {
            if (enemy.active_) {
                if (enemy.enemy_type_ == 2) {
                    if (enemy.bullet_pattern_ == 0) {
                        int bullet_count = 16 + bullet_level * 2;
                        for (int i = 0; i < bullet_count; i++) {
                            float angle = (2 * 3.14159f * i) / bullet_count;
                            float speed = 2 + bullet_level * 0.5;
                            float vx = cos(angle) * speed;
                            float vy = sin(angle) * speed;
                            bullets_.push_back(Bullet(enemy.x, enemy.y, 2, vx, vy));
                        }
                    }
                    else if (enemy.bullet_pattern_ == 1) {
                        int num_bullets_in_fan = 5 + bullet_level;
                        float fan_angle = 60.0f * (3.14159f / 180.0f);
                        float start_angle = atan2(player_.y - enemy.y, player_.x - enemy.x) - fan_angle / 2;

                        for (int i = 0; i < num_bullets_in_fan; i++) {
                            float angle = start_angle + (fan_angle / (num_bullets_in_fan - 1)) * i;
                            float speed = 2 + bullet_level * 0.75;
                            float vx = cos(angle) * speed;
                            float vy = sin(angle) * speed;
                            bullets_.push_back(Bullet(enemy.x, enemy.y, 2, vx, vy));
                        }
                    }
                    else if (enemy.bullet_pattern_ == 2) {
                        for (int i = 0; i < 3; i++) {
                            float dx = player_.x - enemy.x;
                            float dy = player_.y - enemy.y;
                            float distance = sqrt(dx * dx + dy * dy);
                            if (distance < 1.0f) distance = 1.0f;

                            float speed = 1.5f + bullet_level * 0.5f;
                            float vx = (dx / distance) * speed;
                            float vy = (dy / distance) * speed;
                            bullets_.push_back(Bullet(enemy.x, enemy.y, 2, vx, vy));
                        }
                    }
                }
                else if (enemy.enemy_type_ == 1) {
                    int bullet_count = 5 + bullet_level;
                    float speed = 1.5f + bullet_level * 0.5f;
                    for (int i = 0; i < bullet_count; i++) {
                        float angle = (2 * 3.14159f * i) / bullet_count;
                        float vx = cos(angle) * speed;
                        float vy = sin(angle) * speed;
                        bullets_.push_back(Bullet(enemy.x, enemy.y, 2, vx, vy));
                    }
                }
                else {
                    int bullet_count = 3 + bullet_level;
                    for (int i = 0; i < bullet_count; i++) {
                        float angle = (2 * 3.14159f * i) / bullet_count;
                        float speed = 1 + bullet_level * 0.25;
                        float vx = cos(angle) * speed;
                        float vy = sin(angle) * speed;
                        bullets_.push_back(Bullet(enemy.x, enemy.y, 2, vx, vy));
                    }
                }
                sound_manager_.playEffect("shoot");
            }
        }
        danmaku_global_timer_ = 0;
    }
}

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

void GameManager::checkCollisions() {
    for (auto& bullet : bullets_) {
        if (bullet.active_ && (bullet.type_ == 0 || bullet.type_ == 3)) {
            for (auto& enemy : enemies_) {
                if (enemy.active_ && bullet.hitbox_.intersects(enemy.hitbox_)) {
                    bullet.active_ = false;
                    enemy.health_ -= bullet.damage_;

                    if (bullet.type_ == 2) {
                        sound_manager_.playEffect("shoot");
                    }

                    if (enemy.health_ <= 0) {
                        enemy.active_ = false;
                        explosions_.push_back(Explosion(enemy.x, enemy.y));
                        sound_manager_.playEffect("explosion");
                        score_ += (enemy.enemy_type_ == 0) ? 10 : (enemy.enemy_type_ == 1) ? 30 : 100;

                        if (enemy.enemy_type_ == 2) {
                            boss_active_ = false;
                            boss_spawn_cooldown_ = 60 * 10;
                        }

                        int new_level = score_ / 300 + 1;
                        if (new_level > level_) {
                            level_ = new_level;
                            if (level_ % 3 == 0) player_.damage_++;
                            if (level_ % 4 == 0) player_.speed_ = min(10, player_.speed_ + 1);
                        }
                    }
                }
            }
        }
    }

    for (auto& bullet : bullets_) {
        if (bullet.active_ && (bullet.type_ == 1 || bullet.type_ == 2) &&
            bullet.hitbox_.intersects(player_.hitbox_)) {
            if (!shield_active_) {
                bullet.active_ = false;
                player_.takeDamage();
                sound_manager_.playEffect("damage");
                if (player_.lives_ <= 0) {
                    game_state_ = GAME_OVER;
                    sound_manager_.stopBGM();
                    sound_manager_.playEffect("gameover");
                }
            }
            else {
                bullet.active_ = false;
                sound_manager_.playEffect("powerup");
            }
        }
    }

    for (auto& enemy : enemies_) {
        if (enemy.active_ && player_.hitbox_.intersects(enemy.hitbox_)) {
            enemy.active_ = false;
            explosions_.push_back(Explosion(enemy.x, enemy.y));
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
            if (powerup.type_ == 0) {
                player_.heal();
            }
            else if (powerup.type_ == 1) {
                score_ += 50;
            }
            else if (powerup.type_ == 2) {
                player_.attack_speed_boost_timer_ = 60 * 5;
            }
        }
    }
}
void GameManager::initStars() {
    for (int i = 0; i < MAX_STAR; i++) {
        stars_[i].x = rand() % WINDOW_WIDTH;
        stars_[i].y = rand() % WINDOW_HEIGHT;
        stars_[i].step = (rand() % 2000) / 1000.0 + 0.3;
        int c = (int)(stars_[i].step * 150 + 50);
        stars_[i].color = RGB(c, c, c);
    }
}

void GameManager::updateStars() {
    for (int i = 0; i < MAX_STAR; i++) {
        putpixel((int)stars_[i].x, stars_[i].y, RGB(0, 0, 0)); // 清除旧像素
        stars_[i].x += stars_[i].step;
        if (stars_[i].x > WINDOW_WIDTH) {
            stars_[i].x = 0;
            stars_[i].y = rand() % WINDOW_HEIGHT;
        }
        putpixel((int)stars_[i].x, stars_[i].y, stars_[i].color); // 绘制新像素
    }
}

void GameManager::draw() {
    cleardevice();
    updateStars();

    if (game_state_ == MENU) drawMenu();
    else if (game_state_ == PLAYING || game_state_ == PAUSED) {
        drawGame();
        if (game_state_ == PAUSED) drawPauseScreen();
    }
    else if (game_state_ == GAME_OVER) drawGameOver();
}

void GameManager::update() {
    if (game_state_ == PLAYING) {
        if (shield_active_ && shield_duration_ > 0) {
            shield_duration_--;
            player_.shield_timer_--;

            if (shield_duration_ < 60) {
                player_.shield_active_ = (shield_duration_ / 5) % 2 == 0;
            }
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
}

void GameManager::run() {
    running_ = true;

    while (running_) {
        handleInput();
        update();
        draw();
        Sleep(16);

        if (key_state_[VK_ESCAPE]) {
            quitGame();
        }
    }

    cleanupResources();
}

void GameManager::quitGame() {
    running_ = false;
}

void GameManager::cleanupResources() {
    // 清理资源
}
