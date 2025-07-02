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
    initStars();
    memset(key_state_, 0, sizeof(key_state_));
    player_.loadImage();
}
