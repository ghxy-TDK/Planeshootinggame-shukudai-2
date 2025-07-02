#include "GameManager.h"

// ÊäÈë¿ØÖÆÂß¼­
void GameManager::handleInput() {
    for (int i = 0; i < 256; i++) key_state_[i] = (GetAsyncKeyState(i) & 0x8000) != 0;
    if (game_state_ == MENU) return;

    if (game_state_ == PLAYING) {
        int speed = player_.speed_;
        if (key_state_['A'] || key_state_[VK_LEFT]) player_.x = max(20.0f, player_.x - speed);
        if (key_state_['D'] || key_state_[VK_RIGHT]) player_.x = min((float)WINDOW_WIDTH - 20, player_.x + speed);
        if (key_state_['W'] || key_state_[VK_UP]) player_.y = max(20.0f, player_.y - speed);
        if (key_state_['S'] || key_state_[VK_DOWN]) player_.y = min((float)WINDOW_HEIGHT - 20, player_.y + speed);

        bool j_now = key_state_['J'];
        bool space_now = key_state_[VK_SPACE];
        if ((j_now && !prev_j_state_) || (space_now && !prev_space_state_)) {
            firing_toggle_ = !firing_toggle_;
            Sleep(150);
        }
        prev_j_state_ = j_now;
        prev_space_state_ = space_now;

        bool e_now = key_state_['E'];
        if (e_now && !prev_e_state_ && laser_cooldown_ <= 0) {
            activateLaserSkill();
            laser_cooldown_ = LASER_COOLDOWN_TIME;
            sound_manager_.playEffect("shoot");
            Sleep(100);
        }
        prev_e_state_ = e_now;

        bool q_now = key_state_['Q'];
        if (q_now && !prev_q_state_ && shield_cooldown_ <= 0) {
            activateShieldSkill();
            shield_cooldown_ = SHIELD_COOLDOWN_TIME;
            sound_manager_.playEffect("shoot");
            Sleep(100);
        }
        prev_q_state_ = q_now;

        if (laser_active_ && laser_duration_ > 0) updateLaserSkill();
        else if (laser_active_) deactivateLaserSkill();

        if (shield_active_ && shield_duration_ > 0) shield_duration_--;
        else if (shield_active_) deactivateShieldSkill();

        if (firing_toggle_ && bullet_cooldown_ <= 0 && !laser_active_) {
            MOUSEMSG m; bool mouse_found = false;
            while (MouseHit()) { m = GetMouseMsg(); mouse_found = true; }
            float tx = mouse_found ? m.x : player_.x;
            float ty = mouse_found ? m.y : player_.y - 100;
            float dx = tx - player_.x, dy = ty - player_.y;
            float dist = sqrt(dx * dx + dy * dy);
            if (dist < 1.0f) dist = 1.0f;
            bullets_.emplace_back(player_.x, player_.y - 20, 0, dx / dist * 10.0f, dy / dist * 10.0f, player_.damage_);
            bullet_cooldown_ = (player_.attack_speed_boost_timer_ > 0) ? 3 : 6;
            sound_manager_.playEffect("shoot");
        }

        if (key_state_['P'] && !prev_pause_state_) { game_state_ = PAUSED; Sleep(200); }
        prev_pause_state_ = key_state_['P'];

        if (key_state_['M'] && !prev_mute_state_) { sound_manager_.toggleSound(); Sleep(200); }
        prev_mute_state_ = key_state_['M'];
    }
    else if (game_state_ == PAUSED) {
        if (key_state_['P'] && !prev_pause_state_) { game_state_ = PLAYING; Sleep(200); }
        prev_pause_state_ = key_state_['P'];

        MOUSEMSG msg;
        while (MouseHit()) {
            msg = GetMouseMsg();
            if (msg.uMsg == WM_LBUTTONDOWN) { game_state_ = PLAYING; Sleep(200); }
        }

        if (key_state_[VK_ESCAPE]) { game_state_ = MENU; Sleep(200); }
    }
    else if (game_state_ == GAME_OVER) {
            if (key_state_['R']) { game_state_ = MENU; Sleep(200); }
    }

    if (bullet_cooldown_ > 0) bullet_cooldown_--;
    if (laser_cooldown_ > 0) laser_cooldown_--;
    if (shield_cooldown_ > 0) shield_cooldown_--;
    if (player_.attack_speed_boost_timer_ > 0) player_.attack_speed_boost_timer_--;
}
