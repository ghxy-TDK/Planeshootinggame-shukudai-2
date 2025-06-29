#pragma once
#include "common.h"
#include "Player.h"
#include "Bullet.h"
#include "Enemy.h"
#include "PowerUp.h"
#include "Explosion.h"
#include "SoundManager.h"
#include <vector>

// 管理游戏整体逻辑
class GameManager {
private:
    Star stars_[MAX_STAR];
    Player player_;
    std::vector<Bullet> bullets_;
    std::vector<Enemy> enemies_;
    std::vector<PowerUp> powerups_;
    std::vector<Explosion> explosions_;
    SoundManager sound_manager_;

    GameState game_state_;
    int score_, level_, enemy_spawn_timer_, bullet_cooldown_, powerup_timer_, danmaku_global_timer_;
    bool key_state_[256];
    int boss_spawn_cooldown_;
    bool boss_active_;
    bool running_;
    bool firing_toggle_;

    // 技能系统相关变量
    bool laser_active_;
    int laser_duration_;
    int laser_cooldown_;

    bool shield_active_;
    int shield_duration_;
    int shield_cooldown_;

    // 技能常量
    static const int LASER_DURATION_TIME = 180;
    static const int LASER_COOLDOWN_TIME = 600;
    static const int SHIELD_DURATION_TIME = 300;
    static const int SHIELD_COOLDOWN_TIME = 900;

    void initStars();
    void updateStars();
    void spawnEnemies();
    void spawnPowerups();
    void enemyShoot();
    void globalDanmaku();
    void updateGameObjects();
    void checkCollisions();
    void activateLaserSkill();
    void updateLaserSkill();
    void deactivateLaserSkill();
    void activateShieldSkill();
    void deactivateShieldSkill();

public:
    struct SkillStatus {
        bool laser_ready;
        int laser_cooldown_remaining;
        bool shield_ready;
        int shield_cooldown_remaining;
        bool laser_active;
        bool shield_active;
        int laser_duration_remaining;
        int shield_duration_remaining;
    };

    GameManager();
    void handleInput();
    void drawMenu();
    void drawGame();
    void drawUI();
    void drawPauseScreen();
    void drawGameOver();
    void drawGradientText(int x, int y, LPCTSTR text, int fontSize, COLORREF startColor, COLORREF endColor, float spacingFactor = 1.0f);
    void drawBlinkingText(int x, int y, LPCTSTR text, int fontSize, COLORREF color1, COLORREF color2, int time);
    void resetGame();
    void update();
    void draw();
    void run();
    void quitGame();
    void cleanupResources();
    bool isPlayerShielded();
    SkillStatus getSkillStatus();
};

