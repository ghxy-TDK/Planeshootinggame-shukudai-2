#include "GameManager.h"

// 星星背景
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
        putpixel((int)stars_[i].x, stars_[i].y, RGB(0, 0, 0));
        stars_[i].x += stars_[i].step;
        if (stars_[i].x > WINDOW_WIDTH) {
            stars_[i].x = 0;
            stars_[i].y = rand() % WINDOW_HEIGHT;
        }
        putpixel((int)stars_[i].x, stars_[i].y, stars_[i].color);
    }
}

// 游戏整体绘制
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

// 运行主循环
void GameManager::run() {
    running_ = true;
    while (running_) {
        handleInput();
        update();
        FlushBatchDraw();
        draw();
        Sleep(16);
        if (key_state_[VK_ESCAPE]) quitGame();
    }
    cleanupResources();
}

void GameManager::quitGame() { running_ = false; }
void GameManager::cleanupResources() { /*资源清理占位*/ }

// 重置游戏状态
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

// 补齐缺失的函数定义
void GameManager::drawMenu() {
    cleardevice();
    updateStars();

    // 绘制标题
    drawGradientText(WINDOW_WIDTH / 2 - 250, 80, _T("宇 宙 飞 机 大 战"), 60,
        RGB(0, 150, 255), RGB(150, 0, 255), 1.3f);

    setlinecolor(RGB(100, 150, 255));
    line(WINDOW_WIDTH / 2 - 220, 160, WINDOW_WIDTH / 2 + 220, 160);

    settextcolor(RGB(100, 200, 255));
    settextstyle(24, 0, _T("微软雅黑"));
    outtextxy(WINDOW_WIDTH / 2 - 140, 180, _T("穿越星际的终极战斗"));

    // 菜单选项区域背景
    setfillcolor(RGB(0, 0, 0, 150));
    solidrectangle(WINDOW_WIDTH / 2 - 150, 250, WINDOW_WIDTH / 2 + 150, 500);

    // 定义菜单项
    struct MenuItem {
        TCHAR text[50];
        int y;
        bool hovered;
        int action; // 0:闯关模式 1:无尽模式 2:退出游戏
    } menuItems[3] = {
        {_T("闯关模式"), 280, false, 0},
        {_T("无尽模式"), 350, false, 1},
        {_T("退出游戏"), 420, false, 2}
    };

    // 获取鼠标位置
    MOUSEMSG msg;
    while (MouseHit()) {
        msg = GetMouseMsg();

        // 检查鼠标悬停
        for (auto& item : menuItems) {
            item.hovered = (msg.x >= WINDOW_WIDTH / 2 - 120 && msg.x <= WINDOW_WIDTH / 2 + 120 &&
                msg.y >= item.y - 20 && msg.y <= item.y + 20);

            // 检查鼠标点击
            if (item.hovered && msg.uMsg == WM_LBUTTONDOWN) {
                switch (item.action) {
                case 0: // 闯关模式
                    gameMode_ = STAGE_MODE;
                    game_state_ = PLAYING;
                    resetGame();
                    return;
                case 1: // 无尽模式
                    gameMode_ = ENDLESS_MODE;
                    game_state_ = PLAYING;
                    resetGame();
                    return;
                case 2: // 退出游戏
                    exit(0);
                    return;
                }
            }
        }
    }

    // 绘制菜单项
    settextstyle(30, 0, _T("微软雅黑"));
    for (const auto& item : menuItems) {
        if (item.hovered) {
            settextcolor(RGB(255, 215, 0)); // 悬停时金色
            setfillcolor(RGB(50, 50, 100, 150));
            solidrectangle(WINDOW_WIDTH / 2 - 120, item.y - 25, WINDOW_WIDTH / 2 + 120, item.y + 25);
        }
        else {
            settextcolor(RGB(200, 200, 255)); // 普通状态
        }

        outtextxy(WINDOW_WIDTH / 2 - textwidth(item.text) / 2, item.y - 15, item.text);
    }

    // 绘制控制说明
    settextcolor(RGB(200, 200, 255));
    settextstyle(18, 0, _T("Arial"));
    outtextxy(WINDOW_WIDTH / 2 - 120, 520, _T("控制方式:"));
    outtextxy(WINDOW_WIDTH / 2 - 120, 550, _T("WASD/方向键: 移动飞船"));
    outtextxy(WINDOW_WIDTH / 2 - 120, 580, _T("空格/J键: 发射子弹"));
    outtextxy(WINDOW_WIDTH / 2 - 120, 610, _T("E键: 发射激光"));
    outtextxy(WINDOW_WIDTH / 2 - 120, 640, _T("Q键: 展开护盾"));

    // 声音设置
    settextcolor(sound_manager_.isSoundEnabled() ?
        RGB(150, 255, 150) :
        RGB(255, 100, 100));
    settextstyle(16, 0, _T("微软雅黑"));

    TCHAR soundText[50];
    _stprintf_s(soundText, _T("%s 声音: %s (M键切换)"),
        sound_manager_.isSoundEnabled() ? _T("🔊") : _T("🔇"),
        sound_manager_.isSoundEnabled() ? _T("开") : _T("关"));
    outtextxy(WINDOW_WIDTH / 2 - textwidth(soundText) / 2, 670, soundText);
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
    TCHAR text[100];

    // 绘制左侧技能条
    drawSkillBarsUI();

    // 绘制右侧数据面板
    drawDataPanelUI();

    // 绘制音效控制
    drawSoundControlUI();

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

void GameManager::drawSkillBarsUI() {
    SkillStatus skill_status = getSkillStatus();

    // 技能条基础参数
    const int BAR_WIDTH = 25;
    const int BAR_HEIGHT = 200;
    const int BAR_X = 20;
    const int LASER_BAR_Y = 120;
    const int SHIELD_BAR_Y = 340;

    // 绘制激光技能条
    drawVerticalSkillBar1(BAR_X, LASER_BAR_Y, BAR_WIDTH, BAR_HEIGHT,
        skill_status.laser_active, skill_status.laser_ready,
        skill_status.laser_duration_remaining, skill_status.laser_cooldown_remaining,
        RGB(255, 50, 50), RGB(255, 150, 150), _T("LASER"), _T("E"));

    // 绘制护盾技能条
    drawVerticalSkillBar2(BAR_X, SHIELD_BAR_Y, BAR_WIDTH, BAR_HEIGHT,
        skill_status.shield_active, skill_status.shield_ready,
        skill_status.shield_duration_remaining, skill_status.shield_cooldown_remaining,
        RGB(50, 150, 255), RGB(150, 200, 255), _T("SHIELD"), _T("Q"));
}

void GameManager::drawVerticalSkillBar1(int x, int y, int width, int height,
    bool active, bool ready, float duration, float cooldown,
    COLORREF primary_color, COLORREF glow_color,
    const TCHAR* skill_name, const TCHAR* key_bind) {
    // 绘制外边框（科幻感边框）
    setlinecolor(RGB(100, 200, 255));
    setlinestyle(PS_SOLID, 2);
    rectangle(x - 2, y - 2, x + width + 2, y + height + 2);

    // 绘制内部背景
    setfillcolor(RGB(10, 20, 40));
    solidrectangle(x, y, x + width, y + height);

    float fill_percentage = 0.0f;
    COLORREF bar_color = RGB(50, 50, 50);

    if (active) {
        // 技能激活状态 - 持续时间3秒
        fill_percentage = duration / (3.0f * 60.0f); // 3秒 * 60帧/秒
        if (fill_percentage > 1.0f) fill_percentage = 1.0f;
        bar_color = primary_color;

        // 添加脉动效果
        int pulse = (int)(sin(GetTickCount() * 0.01) * 20 + 20);
        bar_color = RGB(min(255, GetRValue(bar_color) + pulse),
            min(255, GetGValue(bar_color) + pulse / 2),
            min(255, GetBValue(bar_color) + pulse / 2));
    }
    else if (ready) {
        // 技能就绪状态
        fill_percentage = 1.0f;
        bar_color = RGB(50, 255, 50);

        // 添加闪烁效果
        int blink = (int)(sin(GetTickCount() * 0.02) * 50 + 200);
        bar_color = RGB(50, blink, 50);
    }
    else {
        // 冷却状态 - 冷却时间10秒
        fill_percentage = 1.0f - (cooldown / (10.0f * 60.0f)); // 10秒 * 60帧/秒
        if (fill_percentage < 0.0f) fill_percentage = 0.0f;
        bar_color = RGB(100, 100, 100);
    }

    // 绘制进度条
    int fill_height = (int)(height * fill_percentage);
    if (fill_height > 0) {
        setfillcolor(bar_color);
        solidrectangle(x + 2, y + height - fill_height, x + width - 2, y + height - 2);

        // 添加光晕效果
        if (active || ready) {
            for (int i = 1; i <= 3; i++) {
                setlinecolor(RGB(GetRValue(glow_color) / (i * 2),
                    GetGValue(glow_color) / (i * 2),
                    GetBValue(glow_color) / (i * 2)));
                rectangle(x - i, y + height - fill_height - i,
                    x + width + i, y + height + i);
            }
        }
    }

    // 绘制刻度线
    setlinecolor(RGB(100, 150, 200));
    for (int i = 1; i < 4; i++) {
        int mark_y = y + (height * i / 4);
        line(x + width - 5, mark_y, x + width, mark_y);
    }

    // 绘制技能名称和按键提示
    settextcolor(RGB(200, 200, 255));
    settextstyle(12, 0, _T("微软雅黑"));

    // 技能名显示在技能条上方
    outtextxy(x - 5, y - 20, skill_name);

    // 按键提示显示在技能条下方
    settextcolor(RGB(255, 255, 100));
    settextstyle(16, 0, _T("微软雅黑"));
    outtextxy(x + 5, y + height + 5, key_bind);
}

void GameManager::drawVerticalSkillBar2(int x, int y, int width, int height,
    bool active, bool ready, float duration, float cooldown,
    COLORREF primary_color, COLORREF glow_color,
    const TCHAR* skill_name, const TCHAR* key_bind) {
    // 绘制外边框（科幻感边框）
    setlinecolor(RGB(100, 200, 255));
    setlinestyle(PS_SOLID, 2);
    rectangle(x - 2, y - 2, x + width + 2, y + height + 2);

    // 绘制内部背景
    setfillcolor(RGB(10, 20, 40));
    solidrectangle(x, y, x + width, y + height);

    float fill_percentage = 0.0f;
    COLORREF bar_color = RGB(50, 50, 50);

    if (active) {
        // 技能激活状态 - 持续时间5秒
        fill_percentage = duration / (5.0f * 60.0f); // 5秒 * 60帧/秒
        if (fill_percentage > 1.0f) fill_percentage = 1.0f;
        bar_color = primary_color;

        // 添加脉动效果
        int pulse = (int)(sin(GetTickCount() * 0.01) * 20 + 20);
        bar_color = RGB(min(255, GetRValue(bar_color) + pulse),
            min(255, GetGValue(bar_color) + pulse / 2),
            min(255, GetBValue(bar_color) + pulse / 2));
    }
    else if (ready) {
        // 技能就绪状态
        fill_percentage = 1.0f;
        bar_color = RGB(50, 255, 50);

        // 添加闪烁效果
        int blink = (int)(sin(GetTickCount() * 0.02) * 50 + 200);
        bar_color = RGB(50, blink, 50);
    }
    else {
        // 冷却状态 - 冷却时间15秒
        fill_percentage = 1.0f - (cooldown / (15.0f * 60.0f)); // 15秒 * 60帧/秒
        if (fill_percentage < 0.0f) fill_percentage = 0.0f;
        bar_color = RGB(100, 100, 100);
    }

    // 绘制进度条
    int fill_height = (int)(height * fill_percentage);
    if (fill_height > 0) {
        setfillcolor(bar_color);
        solidrectangle(x + 2, y + height - fill_height, x + width - 2, y + height - 2);

        // 添加光晕效果
        if (active || ready) {
            for (int i = 1; i <= 3; i++) {
                setlinecolor(RGB(GetRValue(glow_color) / (i * 2),
                    GetGValue(glow_color) / (i * 2),
                    GetBValue(glow_color) / (i * 2)));
                rectangle(x - i, y + height - fill_height - i,
                    x + width + i, y + height + i);
            }
        }
    }

    // 绘制刻度线
    setlinecolor(RGB(100, 150, 200));
    for (int i = 1; i < 4; i++) {
        int mark_y = y + (height * i / 4);
        line(x + width - 5, mark_y, x + width, mark_y);
    }

    // 绘制技能名称和按键提示
    settextcolor(RGB(200, 200, 255));
    settextstyle(12, 0, _T("微软雅黑"));

    // 技能名显示在技能条上方
    outtextxy(x - 5, y - 20, skill_name);

    // 按键提示显示在技能条下方
    settextcolor(RGB(255, 255, 100));
    settextstyle(16, 0, _T("微软雅黑"));
    outtextxy(x + 5, y + height + 5, key_bind);
}

void GameManager::drawDataPanelUI() {
    // 数据面板参数
    const int PANEL_WIDTH = 240;
    const int PANEL_HEIGHT = 140;
    const int PANEL_X = WINDOW_WIDTH - PANEL_WIDTH - 10;
    const int PANEL_Y = 10;

    // 绘制面板背景
    drawSciFiPanelBackground(PANEL_X, PANEL_Y, PANEL_WIDTH, PANEL_HEIGHT);

    // 绘制数据条
    drawHorizontalDataBar(PANEL_X + 10, PANEL_Y + 10, _T("SCORE"), score_, 9999, RGB(255, 215, 0));
    drawHorizontalDataBar(PANEL_X + 10, PANEL_Y + 35, _T("LEVEL"), level_, 15, RGB(0, 255, 255));
    drawHorizontalDataBar(PANEL_X + 10, PANEL_Y + 60, _T("LIVES"), player_.lives_, 9, RGB(255, 100, 100));
    drawHorizontalDataBar(PANEL_X + 10, PANEL_Y + 85, _T("DAMAGE"), player_.damage_, 10, RGB(255, 150, 0));
    drawHorizontalDataBar(PANEL_X + 10, PANEL_Y + 110, _T("SPEED"), player_.speed_, 15, RGB(100, 255, 150));

    // 攻速提升效果提示
    if (player_.attack_speed_boost_timer_ > 0) {
        drawAttackBoostEffect(WINDOW_WIDTH - 1070, 50);
    }
}

void GameManager::drawSciFiPanelBackground(int x, int y, int width, int height) {
    // 外框光晕
    for (int i = 0; i < 5; i++) {
        setlinecolor(RGB(50 - i * 8, 150 - i * 20, 255 - i * 30));
        setlinestyle(PS_SOLID, 1);
        rectangle(x - i, y - i, x + width + i, y + height + i);
    }

    // 半透明背景
    setfillcolor(RGB(5, 15, 35));
    solidrectangle(x, y, x + width, y + height);

    // 内部装饰线条
    setlinecolor(RGB(100, 200, 255));
    line(x + 5, y + 5, x + width - 5, y + 5);
    line(x + 5, y + height - 5, x + width - 5, y + height - 5);
}

void GameManager::drawHorizontalDataBar(int x, int y, const TCHAR* label, int value, int max_value, COLORREF color) {
    const int LABEL_WIDTH = 70;
    const int BAR_WIDTH = 120;
    const int BAR_HEIGHT = 15;
    const int VALUE_WIDTH = 60;

    // 标签
    settextcolor(RGB(200, 200, 255));
    settextstyle(12, 0, _T("微软雅黑"));
    outtextxy(x, y + 2, label);

    // 数值条背景
    setfillcolor(RGB(20, 30, 50));
    solidrectangle(x + LABEL_WIDTH, y, x + LABEL_WIDTH + BAR_WIDTH, y + BAR_HEIGHT);

    // 数值条填充
    float percentage = (float)value / max_value;
    if (percentage > 1.0f) percentage = 1.0f;
    int fill_width = (int)(BAR_WIDTH * percentage);

    if (fill_width > 0) {
        setfillcolor(color);
        solidrectangle(x + LABEL_WIDTH + 1, y + 1, x + LABEL_WIDTH + fill_width - 1, y + BAR_HEIGHT - 1);

        // 添加闪光效果
        COLORREF highlight = RGB(min(255, GetRValue(color) + 50),
            min(255, GetGValue(color) + 50),
            min(255, GetBValue(color) + 50));
        setfillcolor(highlight);
        solidrectangle(x + LABEL_WIDTH + 1, y + 1, x + LABEL_WIDTH + fill_width - 1, y + 4);
    }

    // 边框
    setlinecolor(RGB(100, 150, 200));
    rectangle(x + LABEL_WIDTH, y, x + LABEL_WIDTH + BAR_WIDTH, y + BAR_HEIGHT);

    // 数值显示
    settextcolor(RGB(255, 255, 255));
    settextstyle(12, 0, _T("微软雅黑"));
    TCHAR value_text[20];
    _stprintf_s(value_text, _T("%d"), value);
    outtextxy(x + LABEL_WIDTH + BAR_WIDTH + 5, y + 2, value_text);
}

void GameManager::drawAttackBoostEffect(int x, int y) {
    // 攻速提升效果的动态显示
    TCHAR boost_text[50];
    _stprintf_s(boost_text, _T("⚡ BOOST: %.1fs"), player_.attack_speed_boost_timer_ / 60.0f);

    // 添加闪烁效果
    int alpha = (int)(sin(GetTickCount() * 0.02) * 100 + 155);
    settextcolor(RGB(255, 255, alpha));
    settextstyle(14, 0, _T("微软雅黑"));

    // 绘制背景框
    int text_width = textwidth(boost_text);
    setfillcolor(RGB(50, 50, 0));
    solidrectangle(x, y, x + text_width + 10, y + 25);
    setlinecolor(RGB(255, 255, 100));
    rectangle(x, y, x + text_width + 10, y + 25);

    outtextxy(x + 5, y + 5, boost_text);
}

void GameManager::drawSoundControlUI() {
    const int CONTROL_X = WINDOW_WIDTH - 1070;
    const int CONTROL_Y = 10;

    // 音效控制背景
    setfillcolor(RGB(10, 20, 40));
    solidrectangle(CONTROL_X, CONTROL_Y, CONTROL_X + 110, CONTROL_Y + 25);

    setlinecolor(RGB(100, 150, 200));
    rectangle(CONTROL_X, CONTROL_Y, CONTROL_X + 110, CONTROL_Y + 25);

    // 音效图标和状态
    settextcolor(sound_manager_.isSoundEnabled() ? RGB(100, 255, 100) : RGB(255, 100, 100));
    settextstyle(12, 0, _T("微软雅黑"));

    TCHAR sound_text[30];
    _stprintf_s(sound_text, _T("SOUND %s (M)"),
        sound_manager_.isSoundEnabled() ? _T("ON") : _T("OFF"));
    outtextxy(CONTROL_X + 5, CONTROL_Y + 6, sound_text);
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
