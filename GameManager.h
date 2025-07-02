#pragma once
#include "common.h"
#include "Player.h"
#include "Bullet.h"
#include "Enemy.h"
#include "PowerUp.h"
#include "Explosion.h"
#include "SoundManager.h"
#include <vector>

enum GameMode { STAGE_MODE, ENDLESS_MODE };

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
    bool boss_active_, running_, firing_toggle_;
    bool prev_pause_state_, prev_mute_state_, prev_j_state_, prev_space_state_, prev_e_state_, prev_q_state_;
    bool laser_active_, shield_active_;
    int laser_duration_, laser_cooldown_, shield_duration_, shield_cooldown_;
    static const int LASER_DURATION_TIME = 180, LASER_COOLDOWN_TIME = 600, SHIELD_DURATION_TIME = 300, SHIELD_COOLDOWN_TIME = 900;

    void initStars(); void updateStars(); void spawnEnemies(); void spawnPowerups(); void enemyShoot(); void globalDanmaku();
    void updateGameObjects(); void checkCollisions(); void activateLaserSkill(); void updateLaserSkill();
    void deactivateLaserSkill(); void activateShieldSkill(); void deactivateShieldSkill();

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
    void handleInput(); void draw(); void drawMenu(); void drawGame(); void drawUI();
    void drawPauseScreen(); void drawGameOver(); void drawGradientText(int x, int y, LPCTSTR text, int fontSize, COLORREF startColor, COLORREF endColor, float spacingFactor = 1.0f);
    void drawBlinkingText(int x, int y, LPCTSTR text, int fontSize, COLORREF color1, COLORREF color2, int time);
    void drawSkillBarsUI();
    void drawVerticalSkillBar1(int x, int y, int width, int height, bool active, bool ready, float duration, float cooldown, COLORREF primary_color, COLORREF glow_color, const TCHAR* skill_name, const TCHAR* key_bind);
    void drawVerticalSkillBar2(int x, int y, int width, int height, bool active, bool ready, float duration, float cooldown, COLORREF primary_color, COLORREF glow_color, const TCHAR* skill_name, const TCHAR* key_bind);
    void drawDataPanelUI(); void drawSciFiPanelBackground(int x, int y, int width, int height);
    void drawHorizontalDataBar(int x, int y, const TCHAR* label, int value, int max_value, COLORREF color);
    void drawAttackBoostEffect(int x, int y); void drawSoundControlUI();
    void resetGame(); void update(); void run(); void quitGame(); void cleanupResources();
    bool isPlayerShielded(); SkillStatus getSkillStatus(); GameMode gameMode_;
};
