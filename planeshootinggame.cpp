#include <graphics.h>
#include <conio.h>
#include <math.h>
#include <time.h>
#include <vector>
#include <algorithm>
#include <mmsystem.h>    // 添加声音支持
#pragma comment(lib, "winmm.lib") // 链接多媒体库

#define WINDOW_WIDTH 1080
#define WINDOW_HEIGHT 1080
#define MAX_STAR 150
#define MAX_BULLETS 100
#define MAX_ENEMIES 15
#define MAX_EXPLOSIONS 30

/// @brief 游戏状态枚举
enum GameState { MENU, PLAYING, GAME_OVER, PAUSED };

/// @brief 表示背景中的星星
struct Star {
    double x;
    int y, color;
    double step;
};

/// @brief 表示碰撞检测的矩形
struct Rect {
    int x, y, width, height;
    /// @brief 检查两个矩形是否相交
    /// @param other 要检查的另一个矩形
    /// @return 如果相交返回true，否则false
    bool intersects(const Rect& other) const {
        return x < other.x + other.width && x + width > other.x &&
            y < other.y + other.height && y + height > other.y;
    }
};

/// @brief 所有游戏对象的基类
class GameObject {
public:
    float x, y, vx, vy;
    bool active_;
    Rect hitbox_;

    /// @brief 构造函数
    /// @param _x 初始X坐标
    /// @param _y 初始Y坐标
    GameObject(float _x = 0, float _y = 0) : x(_x), y(_y), vx(0), vy(0), active_(true) {}
    /// @brief 更新对象位置和碰撞盒
    virtual void update() { x += vx; y += vy; updateHitbox(); }
    /// @brief 绘制对象
    virtual void draw() = 0;
    /// @brief 更新碰撞盒
    virtual void updateHitbox() = 0;
};

/// @brief 表示玩家的飞船
class Player : public GameObject {
public:
    bool laser_active_;
    int laser_timer_;
    bool shield_active_;
    int shield_timer_;
    int lives_, max_lives_, invulnerable_timer_, damage_, speed_;
    bool invulnerable_;
    int attack_speed_boost_timer_; // 攻击速度提升持续时间
    IMAGE player_img_;  // 添加图片成员
    int img_width_, img_height_;  // 图片尺寸
    /// @brief 构造函数
    /// @param _x 初始X坐标
    /// @param _y 初始Y坐标
    Player(float _x, float _y) : GameObject(_x, _y), lives_(3), max_lives_(5), invulnerable_timer_(0),
        damage_(1), speed_(5), invulnerable_(false), attack_speed_boost_timer_(0) {
    }
    void loadImage() {
        loadimage(&player_img_, _T("res\\FEIJI.png"));  // 加载图片
        img_width_ = player_img_.getwidth();
        img_height_ = player_img_.getheight();
    }
    /// @brief 更新玩家状态
    void update() override {
        GameObject::update();
        if (invulnerable_ && --invulnerable_timer_ <= 0) invulnerable_ = false;
        if (attack_speed_boost_timer_ > 0) attack_speed_boost_timer_--;
    }

    /// @brief 绘制玩家飞船
    void draw() override {
        if (!invulnerable_ || (invulnerable_timer_ / 3) % 2) {
            // 使用透明方式绘制图片
            putimage(x - img_width_ / 2, y - img_height_ / 2, &player_img_, SRCAND);  // 掩码图
            putimage(x - img_width_ / 2, y - img_height_ / 2, &player_img_, SRCPAINT); // 原图
        }
    }

    /// @brief 更新碰撞盒
    void updateHitbox() override {
        hitbox_ = { (int)x - img_width_ / 2, (int)y - img_height_ / 2, img_width_, img_height_ };
    }
    /// @brief 玩家受到伤害
    void takeDamage() {
        if (!invulnerable_) {
            lives_--;
            invulnerable_ = true;
            invulnerable_timer_ = 90;
        }
    }

    /// @brief 玩家恢复生命
    void heal() { if (lives_ < max_lives_) lives_++; }
};

/// @brief 表示子弹
class Bullet : public GameObject {
public:
    int damage_, type_; // 0=玩家子弹, 1=敌人普通子弹, 2=弹幕子弹

    /// @brief 构造函数
    /// @param _x 初始X坐标
    /// @param _y 初始Y坐标
    /// @param _type 子弹类型
    /// @param _vx X轴速度
    /// @param _vy Y轴速度
    /// @param _damage 伤害值
    Bullet(float _x, float _y, int _type = 0, float _vx = 0, float _vy = -8, int _damage = 1)
        : GameObject(_x, _y), damage_(_damage), type_(_type) {
        vx = _vx; vy = _vy;
    }

    /// @brief 更新子弹状态
    void update() override {
        GameObject::update();
        if (y < -20 || y > WINDOW_HEIGHT + 20 || x < -20 || x > WINDOW_WIDTH + 20) active_ = false;
    }

    /// @brief 绘制子弹
    void draw() override {
        if (type_ == 0) { // 玩家子弹
            setfillcolor(RGB(255, 255, 0));
            solidrectangle(x - 3, y - 12, x + 3, y + 12);
        }
        else if (type_ == 1) { // 敌人普通子弹
            setfillcolor(RGB(255, 100, 100));
            solidrectangle(x - 2, y - 6, x + 2, y + 6);
        }
        else if (type_ == 2) { // 弹幕子弹
            setfillcolor(RGB(255, 50, 150));
            solidcircle(x, y, 3);
        }
        else if (type_ == 3) { // 激光子弹
            setfillcolor(RGB(0, 255, 0));
            solidrectangle(x - 1, y - 30, x + 1, y);
        }
    }

    /// @brief 更新碰撞盒
    void updateHitbox() override {
        if (type_ == 2) hitbox_ = { (int)x - 3, (int)y - 3, 6, 6 };
        else hitbox_ = { (int)x - 2, (int)y - 6, 4, 12 };
    }
};

/// @brief 表示敌机
class Enemy : public GameObject {
public:
    int health_, max_health_, enemy_type_, shoot_timer_, danmaku_timer_;
    int bullet_pattern_; // 敌人的弹幕模式

    /// @brief 构造函数
    /// @param _x 初始X坐标
    /// @param _y 初始Y坐标
    /// @param type 敌人类型 (0=普通, 1=精英, 2=首领)
    Enemy(float _x, float _y, int type = 0) : GameObject(_x, _y), enemy_type_(type), shoot_timer_(0), danmaku_timer_(0) {
        vy = 2 + rand() % 3;
        if (rand() % 2) vx = (rand() % 3) - 1;

        if (type == 0) { max_health_ = health_ = 1; bullet_pattern_ = 0; }       // 普通敌人
        else if (type == 1) { max_health_ = health_ = 5; bullet_pattern_ = rand() % 2; } // 精英敌人，随机模式
        else { max_health_ = health_ = 15; vy = 1; bullet_pattern_ = rand() % 3; }     // 首领敌人，随机模式
    }

    /// @brief 更新敌人状态
    void update() override {
        GameObject::update();
        if (y > WINDOW_HEIGHT + 50) active_ = false;
        if (x <= 20 || x >= WINDOW_WIDTH - 20) vx = -vx;
        shoot_timer_++;
        danmaku_timer_++;
    }

    /// @brief 绘制敌机
    void draw() override {
        int r = 255, g = 0, b = 0;
        if (enemy_type_ == 1) { r = 255; g = 100; b = 0; }
        else if (enemy_type_ == 2) { r = 150; g = 0; b = 255; }

        setfillcolor(RGB(r, g, b));
        int w = (enemy_type_ == 2) ? 25 : (enemy_type_ == 1) ? 18 : 12;
        int h = (enemy_type_ == 2) ? 20 : (enemy_type_ == 1) ? 16 : 12;
        solidrectangle(x - w, y - h, x + w, y + h);

        // 普通和精英敌人的血条（首领血条单独绘制）
        if (health_ < max_health_ && enemy_type_ != 2) {
            setfillcolor(RGB(255, 0, 0));
            solidrectangle(x - w, y - h - 8, x + w, y - h - 5);
            setfillcolor(RGB(0, 255, 0));
            int health_width = (2 * w * health_) / max_health_;
            solidrectangle(x - w, y - h - 8, x - w + health_width, y - h - 5);
        }
    }

    /// @brief 更新碰撞盒
    void updateHitbox() override {
        int w = (enemy_type_ == 2) ? 25 : (enemy_type_ == 1) ? 18 : 12;
        int h = (enemy_type_ == 2) ? 20 : (enemy_type_ == 1) ? 16 : 12;
        hitbox_ = { (int)x - w, (int)y - h, 2 * w, 2 * h };
    }

    /// @brief 检查是否可以射击
    /// @return 可以射击返回true
    bool canShoot() { return shoot_timer_ >= (enemy_type_ == 2 ? 30 : 60); }
    /// @brief 检查是否可以释放弹幕
    /// @return 可以释放弹幕返回true
    bool canDanmaku() { return danmaku_timer_ >= (enemy_type_ == 2 ? 120 : 240); }
    /// @brief 重置射击计时器
    void resetShoot() { shoot_timer_ = 0; }
    /// @brief 重置弹幕计时器
    void resetDanmaku() { danmaku_timer_ = 0; }
};

/// @brief 表示道具
class PowerUp : public GameObject {
public:
    int type_, life_timer_; // 0=生命, 1=分数, 2=攻击速度

    /// @brief 构造函数
    /// @param _x 初始X坐标
    /// @param _y 初始Y坐标
    /// @param _type 道具类型
    PowerUp(float _x, float _y, int _type = 0) : GameObject(_x, _y), type_(_type), life_timer_(300) {
        vy = 2;
    }

    /// @brief 更新道具状态
    void update() override {
        GameObject::update();
        if (y > WINDOW_HEIGHT + 20 || --life_timer_ <= 0) active_ = false;
    }

    /// @brief 绘制道具
    void draw() override {
        if (type_ == 0) { // 生命道具
            setfillcolor(RGB(0, 255, 0));
            solidcircle(x, y, 8);
            setfillcolor(RGB(255, 255, 255));
            outtextxy(x - 4, y - 6, _T("+"));
        }
        else if (type_ == 1) { // 分数道具
            setfillcolor(RGB(255, 255, 0));
            solidcircle(x, y, 8);
            setfillcolor(RGB(255, 255, 255));
            outtextxy(x - 4, y - 6, _T("$"));
        }
        else { // 攻击速度道具
            setfillcolor(RGB(0, 200, 255));
            solidcircle(x, y, 8);
            setfillcolor(RGB(255, 255, 255));
            outtextxy(x - 4, y - 6, _T("S"));
        }
    }

    /// @brief 更新碰撞盒
    void updateHitbox() override { hitbox_ = { (int)x - 8, (int)y - 8, 16, 16 }; }
};

/// @brief 表示爆炸效果
class Explosion {
public:
    float x, y;
    int frame_, max_frames_;
    bool active_;

    /// @brief 构造函数
    /// @param _x 初始X坐标
    /// @param _y 初始Y坐标
    Explosion(float _x, float _y) : x(_x), y(_y), frame_(0), max_frames_(15), active_(true) {}

    /// @brief 更新爆炸状态
    void update() { if (++frame_ >= max_frames_) active_ = false; }

    /// @brief 绘制爆炸
    void draw() {
        int size = frame_ * 2;
        setfillcolor(RGB(255, 255 - frame_ * 15, 0));
        solidcircle(x, y, size);
        setfillcolor(RGB(255, 100, 0));
        solidcircle(x, y, size / 2);
    }
};

/// @brief 管理游戏音效
class SoundManager {
private:
    bool sound_enabled_;

public:
    /// @brief 构造函数
    SoundManager() : sound_enabled_(true) {}

    /// @brief 播放背景音乐
    void playBGM() {
        if (sound_enabled_) {
            PlaySound(_T("bgm.wav"), NULL, SND_ASYNC | SND_LOOP | SND_FILENAME);
        }
    }

    /// @brief 停止背景音乐
    void stopBGM() {
        PlaySound(NULL, NULL, SND_ASYNC);
    }

    /// @brief 播放音效
    /// @param effect 要播放的音效名称
    void playEffect(const char* effect) {
        if (sound_enabled_) {
            if (strcmp(effect, "shoot") == 0) {
                PlaySound(_T("shoot.wav"), NULL, SND_ASYNC | SND_FILENAME);
            }
            else if (strcmp(effect, "explosion") == 0) {
                PlaySound(_T("explosion.wav"), NULL, SND_ASYNC | SND_FILENAME);
            }
            else if (strcmp(effect, "powerup") == 0) {
                PlaySound(_T("powerup.wav"), NULL, SND_ASYNC | SND_FILENAME);
            }
            else if (strcmp(effect, "damage") == 0) {
                PlaySound(_T("damage.wav"), NULL, SND_ASYNC | SND_FILENAME);
            }
            else if (strcmp(effect, "gameover") == 0) {
                PlaySound(_T("gameover.wav"), NULL, SND_ASYNC | SND_FILENAME);
            }
        }
    }

    /// @brief 切换声音开关
    void toggleSound() {
        sound_enabled_ = !sound_enabled_;
        if (!sound_enabled_) {
            stopBGM();
        }
    }

    /// @brief 检查声音是否启用
    /// @return 启用返回true
    bool isSoundEnabled() const {
        return sound_enabled_;
    }
};

/// @brief 管理游戏整体逻辑
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
    int boss_spawn_cooldown_; // 首领敌机生成冷却
    bool boss_active_; // 首领敌机是否激活
    bool running_; // 游戏运行状态标志
    bool firing_toggle_;  // 持续发射模式

    // 技能系统相关变量
    bool laser_active_ = false;          // 激光是否激活
    int laser_duration_ = 0;             // 激光剩余持续时间
    int laser_cooldown_ = 0;             // 激光冷却时间

    bool shield_active_ = false;         // 护盾是否激活
    int shield_duration_ = 0;            // 护盾剩余持续时间
    int shield_cooldown_ = 0;            // 护盾冷却时间

    // 技能常量
    static const int LASER_DURATION_TIME = 180;    // 激光持续3秒 (60fps * 3)
    static const int LASER_COOLDOWN_TIME = 600;    // 激光冷却10秒
    static const int SHIELD_DURATION_TIME = 300;   // 护盾持续5秒
    static const int SHIELD_COOLDOWN_TIME = 900;   // 护盾冷却15秒

public:
    /// @brief 构造函数
    GameManager() :
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
        running_(true), // 初始化运行状态为 true
        firing_toggle_(false)
    {
        initStars();
        memset(key_state_, 0, sizeof(key_state_));
    }

    /// @brief 初始化背景星星
    void initStars() {
        for (int i = 0; i < MAX_STAR; i++) {
            stars_[i].x = rand() % WINDOW_WIDTH;
            stars_[i].y = rand() % WINDOW_HEIGHT;
            stars_[i].step = (rand() % 2000) / 1000.0 + 0.3;
            int c = (int)(stars_[i].step * 150 + 50);
            stars_[i].color = RGB(c, c, c);
        }
    }

    /// @brief 更新星星位置
    void updateStars() {
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

    /// @brief 处理用户输入
    void handleInput() {
        for (int i = 0; i < 256; i++) key_state_[i] = (GetAsyncKeyState(i) & 0x8000) != 0;

        if (game_state_ == MENU && (key_state_[VK_RETURN] || key_state_[VK_SPACE])) {
            game_state_ = PLAYING;
            resetGame();
            sound_manager_.playBGM(); // 开始播放背景音乐
        }
        else if (game_state_ == PLAYING) {
            int speed = player_.speed_;
            // 移动控制
            if (key_state_['A'] || key_state_[VK_LEFT]) player_.x = max(20.0f, player_.x - speed);
            if (key_state_['D'] || key_state_[VK_RIGHT]) player_.x = min((float)WINDOW_WIDTH - 20, player_.x + speed);
            if (key_state_['W'] || key_state_[VK_UP]) player_.y = max(20.0f, player_.y - speed);
            if (key_state_['S'] || key_state_[VK_DOWN]) player_.y = min((float)WINDOW_HEIGHT - 20, player_.y + speed);

            // 检测 J 键或空格键切换持续发射
            static bool prev_j_state = false;
            static bool prev_space_state = false;
            bool current_j_state = (GetAsyncKeyState('J') & 0x8000) != 0;
            bool current_space_state = (GetAsyncKeyState(VK_SPACE) & 0x8000) != 0;

            if ((current_j_state && !prev_j_state) || (current_space_state && !prev_space_state)) {
                firing_toggle_ = !firing_toggle_; // 切换状态
                Sleep(150); // 防抖
            }
            prev_j_state = current_j_state;
            prev_space_state = current_space_state;

            // 技能系统 - 激光技能 (E键)
            static bool prev_e_state = false;
            bool current_e_state = (GetAsyncKeyState('E') & 0x8000) != 0;
            if (current_e_state && !prev_e_state && laser_cooldown_ <= 0) {
                activateLaserSkill();
                laser_cooldown_ = LASER_COOLDOWN_TIME; // 设置冷却时间
                sound_manager_.playEffect("laser_activate");
                Sleep(100); // 防抖
            }
            prev_e_state = current_e_state;

            // 技能系统 - 护盾技能 (Q键)
            static bool prev_q_state = false;
            bool current_q_state = (GetAsyncKeyState('Q') & 0x8000) != 0;
            if (current_q_state && !prev_q_state && shield_cooldown_ <= 0) {
                activateShieldSkill();
                shield_cooldown_ = SHIELD_COOLDOWN_TIME; // 设置冷却时间
                sound_manager_.playEffect("shield_activate");
                Sleep(100); // 防抖
            }
            prev_q_state = current_q_state;

            // 激光技能持续输出逻辑
            if (laser_active_ && laser_duration_ > 0) {
                updateLaserSkill();
                laser_duration_--;
            }
            else if (laser_active_) {
                deactivateLaserSkill();
            }

            // 护盾技能持续时间逻辑
            if (shield_active_ && shield_duration_ > 0) {
                shield_duration_--;
            }
            else if (shield_active_) {
                deactivateShieldSkill();
            }

            // 持续发射逻辑（只有在激光未激活时才能普通射击）
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
                Sleep(200); // 防抖
            }

            // 声音开关
            if (key_state_['M']) {
                sound_manager_.toggleSound();
                Sleep(200); // 防抖
            }
        }
        else if (game_state_ == PAUSED && key_state_['P']) {
            game_state_ = PLAYING;
            Sleep(200); // 防抖
        }
        else if (game_state_ == GAME_OVER && key_state_['R']) {
            game_state_ = MENU;
        }

        // 更新冷却时间
        if (bullet_cooldown_ > 0) bullet_cooldown_--;
        if (laser_cooldown_ > 0) laser_cooldown_--;
        if (shield_cooldown_ > 0) shield_cooldown_--;
    }

    /// @brief 激活激光技能
    void activateLaserSkill() {
        laser_active_ = true;
        laser_duration_ = LASER_DURATION_TIME;
        // 可以在这里添加激光开始的视觉效果
    }

    /// @brief 更新激光技能逻辑
    void updateLaserSkill() {
        if (!laser_active_) return;

        // 激光每帧都会发射高频率的子弹
        if (laser_duration_ % 2 == 0) { // 每2帧发射一次，提高射速
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

            float bullet_speed = 15.0f; // 激光子弹速度更快
            float laser_damage = player_.damage_ * 1.5f; // 激光伤害是普通攻击的1.5倍

            // 发射激光子弹（type = 3 表示激光子弹）
            bullets_.push_back(Bullet(player_.x, player_.y - 20, 3,
                (dx / distance) * bullet_speed,
                (dy / distance) * bullet_speed,
                laser_damage));

            if (laser_duration_ % 10 == 0) { // 每10帧播放一次音效，避免过于频繁
                sound_manager_.playEffect("laser_fire");
            }
        }

        laser_duration_--;
        if (laser_duration_ <= 0) {
            deactivateLaserSkill();
            laser_cooldown_ = LASER_COOLDOWN_TIME;
        }
    }

    /// @brief 关闭激光技能
    void deactivateLaserSkill() {
        laser_active_ = false;
        laser_duration_ = 0;
        // 可以在这里添加激光结束的视觉效果
    }

    /// @brief 激活护盾技能
    void activateShieldSkill() {
        shield_active_ = true;
        shield_duration_ = SHIELD_DURATION_TIME;
        // 可以在这里添加护盾开始的视觉效果
    }

    /// @brief 关闭护盾技能
    void deactivateShieldSkill() {
        shield_active_ = false;
        shield_duration_ = 0;
        // 可以在这里添加护盾结束的视觉效果
    }

    /// @brief 检查玩家是否受到护盾保护（在受伤判定中使用）
    bool isPlayerShielded() {
        return shield_active_;
    }

    /// @brief 获取技能状态信息（用于UI显示）
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

    SkillStatus getSkillStatus() {
        return {
            laser_cooldown_ <= 0,           // 激光是否就绪
            laser_cooldown_,                // 激光剩余冷却时间
            shield_cooldown_ <= 0,          // 护盾是否就绪
            shield_cooldown_,               // 护盾剩余冷却时间
            laser_active_,                  // 激光是否正在使用
            shield_active_,                 // 护盾是否正在使用
            laser_duration_,                // 激光剩余持续时间
            shield_duration_                // 护盾剩余持续时间
        };
    }

    /// @brief 绘制主菜单
    // 绘制渐变文字（增加字符间距参数）
    void drawGradientText(int x, int y, LPCTSTR text, int fontSize,
        COLORREF startColor, COLORREF endColor,
        float spacingFactor = 1.0f) {  // 新增间距系数参数
        settextstyle(fontSize, 0, _T("微软雅黑"));  // 改用更美观的字体

        // 计算渐变步长
        int length = _tcslen(text);
        int step = 255 / (length > 1 ? length - 1 : 1);

        for (int i = 0; i < length; ++i) {
            // 计算当前字符的颜色
            int r = GetRValue(startColor) + (GetRValue(endColor) - GetRValue(startColor)) * i / (length - 1);
            int g = GetGValue(startColor) + (GetGValue(endColor) - GetGValue(startColor)) * i / (length - 1);
            int b = GetBValue(endColor) + (GetBValue(startColor) - GetBValue(endColor)) * i / (length - 1);
            settextcolor(RGB(r, g, b));

            // 绘制单个字符（增加间距系数）
            TCHAR ch[2] = { text[i], 0 };
            outtextxy(x + i * fontSize * 0.7 * spacingFactor, y, ch);  // 0.7改为可调节参数
        }
    }

    // 绘制闪烁文字
    void drawBlinkingText(int x, int y, LPCTSTR text, int fontSize, COLORREF color1, COLORREF color2, int time) {
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


    void drawMenu() {
        cleardevice();
        updateStars();

        drawGradientText(WINDOW_WIDTH / 2 - 250, 100, _T("宇 宙 飞 机 大 战"), 60,
            RGB(0, 150, 255), RGB(150, 0, 255), 1.3f);  // 添加空格和间距系数


        // 添加星空分割线
        setlinecolor(RGB(100, 150, 255));
        line(WINDOW_WIDTH / 2 - 220, 180, WINDOW_WIDTH / 2 + 220, 180);


        // 绘制副标题（位置下移）
        settextcolor(RGB(100, 200, 255));
        settextstyle(24, 0, _T("微软雅黑"));  // 增大字号
        outtextxy(WINDOW_WIDTH / 2 - 140, 200, _T("穿越星际的终极战斗"));  // 位置调整
        // 绘制开始提示 - 闪烁效果
        drawBlinkingText(WINDOW_WIDTH / 2 - 120, 280, _T("按Enter键开始游戏"), 30, RGB(255, 255, 0), RGB(255, 100, 0), 30);

        // 绘制控制说明
        settextcolor(RGB(200, 200, 255));
        settextstyle(18, 0, _T("Arial"));
        outtextxy(WINDOW_WIDTH / 2 - 120, 350, _T("控制方式:"));
        outtextxy(WINDOW_WIDTH / 2 - 120, 380, _T("WASD/方向键: 移动飞船"));
        outtextxy(WINDOW_WIDTH / 2 - 120, 410, _T("空格/J键: 发射激光"));
        outtextxy(WINDOW_WIDTH / 2 - 120, 440, _T("P键: 暂停游戏"));

        // 声音状态显示（融入风格）
        settextcolor(sound_manager_.isSoundEnabled() ?
            RGB(150, 255, 150) :   // 开启时绿色
            RGB(255, 100, 100));   // 关闭时红色
        settextstyle(16, 0, _T("微软雅黑"));

        // 居中显示声音状态，带图标效果
        TCHAR soundText[50];
        _stprintf_s(soundText, _T("%s 声音: %s (M键切换)"),
            sound_manager_.isSoundEnabled() ? _T("🔊") : _T("🔇"),
            sound_manager_.isSoundEnabled() ? _T("开") : _T("关"));
        outtextxy(WINDOW_WIDTH / 2 - textwidth(soundText) / 2, 470, soundText);

        // 绘制游戏提示（位置下移）
        settextcolor(RGB(150, 255, 150));
        settextstyle(18, 0, _T("微软雅黑"));
        outtextxy(WINDOW_WIDTH / 2 - 180, 510, _T("收集能量道具增强你的飞船!"));
    }

    /// @brief 绘制游戏画面
    void drawGame() {
        player_.draw();
        for (auto& bullet : bullets_) bullet.draw();
        for (auto& enemy : enemies_) enemy.draw();
        for (auto& powerup : powerups_) powerup.draw();
        for (auto& explosion : explosions_) explosion.draw();
        drawUI();
    }

    /// @brief 绘制用户界面
    void drawUI() {
        settextcolor(RGB(255, 255, 255));
        settextstyle(18, 0, _T("Arial"));

        TCHAR text[100];
        _stprintf_s(text, _T("Score: %d  Level: %d  Lives: %d"), score_, level_, player_.lives_);
        outtextxy(WINDOW_WIDTH - 300, 50, text);


        _stprintf_s(text, _T("Damage: %d  Speed: %d"), player_.damage_, player_.speed_);
        outtextxy(WINDOW_WIDTH - 300, 75, text);


        // 显示攻击速度提升状态
        if (player_.attack_speed_boost_timer_ > 0) {
            _stprintf_s(text, _T("Attack Speed Boost: %.1f s"), player_.attack_speed_boost_timer_ / 60.0f);
            outtextxy(10, 60, text);
        }

        // 获取技能状态信息（修正：调用GameManager的方法而不是Player的方法）
        SkillStatus skill_status = getSkillStatus();

        // 显示激光技能状态
        if (skill_status.laser_active) {
            // 激光技能激活中
            settextcolor(RGB(255, 100, 100)); // 激活时用红色显示
            _stprintf_s(text, _T("Laser Active: %.1f s"), skill_status.laser_duration_remaining / 60.0f);
            outtextxy(10, 80, text);
        }
        else if (skill_status.laser_ready) {
            // 激光技能就绪
            settextcolor(RGB(100, 255, 100)); // 就绪时用绿色显示
            _stprintf_s(text, _T("Laser Ready (E)"));
            outtextxy(10, 80, text);
        }
        else {
            // 激光技能冷却中
            settextcolor(RGB(255, 255, 100)); // 冷却时用黄色显示
            _stprintf_s(text, _T("Laser Cooldown: %.1f s"), skill_status.laser_cooldown_remaining / 60.0f);
            outtextxy(10, 80, text);
        }

        // 显示护盾技能状态
        if (skill_status.shield_active) {
            // 护盾技能激活中
            settextcolor(RGB(100, 100, 255)); // 激活时用蓝色显示
            _stprintf_s(text, _T("Shield Active: %.1f s"), skill_status.shield_duration_remaining / 60.0f); 
            outtextxy(10, 100, text);
        }
        else if (skill_status.shield_ready) {
            // 护盾技能就绪
            settextcolor(RGB(100, 255, 100)); // 就绪时用绿色显示
            _stprintf_s(text, _T("Shield Ready (Q)"));
            outtextxy(10, 100, text);
        }
        else {
            // 护盾技能冷却中
            settextcolor(RGB(255, 255, 100)); // 冷却时用黄色显示
            _stprintf_s(text, _T("Shield Cooldown: %.1f s"), skill_status.shield_cooldown_remaining / 60.0f);
            outtextxy(10, 100, text);
        }

        // 显示声音状态
        settextcolor(RGB(200, 200, 100));
        _stprintf_s(text, _T("Sound: %s (M to toggle)"), sound_manager_.isSoundEnabled() ? _T("ON") : _T("OFF"));
        outtextxy(WINDOW_WIDTH - textwidth(text) - 10, 10, text);

        // 绘制首领血条
        for (auto& enemy : enemies_) {
            if (enemy.active_ && enemy.enemy_type_ == 2) {
                settextcolor(RGB(255, 255, 255));
                settextstyle(20, 0, _T("Arial"));
                _stprintf_s(text, _T("BOSS: %d/%d"), enemy.health_, enemy.max_health_);
                outtextxy(WINDOW_WIDTH / 2 - textwidth(text) / 2, 60, text); // 居中文本

                // 更大的血条显示
                int boss_bar_width = 400;
                int boss_bar_height = 15;
                int boss_bar_x = WINDOW_WIDTH / 2 - boss_bar_width / 2;
                int boss_bar_y = 90;

                setfillcolor(RGB(255, 0, 0)); // 血条红色背景
                solidrectangle(boss_bar_x, boss_bar_y, boss_bar_x + boss_bar_width, boss_bar_y + boss_bar_height);

                setfillcolor(RGB(0, 255, 0)); // 当前血量绿色
                int current_boss_health_width = (boss_bar_width * enemy.health_) / enemy.max_health_;
                solidrectangle(boss_bar_x, boss_bar_y, boss_bar_x + current_boss_health_width, boss_bar_y + boss_bar_height);
                break; // 假设同一时间只有一个首领
            }
        }
    }

    /// @brief 绘制暂停画面
    void drawPauseScreen() {
        // 绘制半透明黑色背景，覆盖整个窗口
        setfillcolor(RGB(0, 0, 0, 180)); // 半透明黑色，突出暂停效果
        solidrectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

        // 设置字体样式以获取PAUSED文字的准确宽度
        settextstyle(45, 0, _T("Arial")); // 临时设置字体样式

        // 计算“PAUSED”文字的X轴居中位置
        int paused_text_width = textwidth(_T("PAUSED"));
        int paused_x = WINDOW_WIDTH / 2 - paused_text_width / 2;
        // 设定“PAUSED”文字的Y坐标，不进行垂直居中，而是根据屏幕中心略微上移
        int paused_y = WINDOW_HEIGHT / 2 - 50; // 根据原代码位置并结合视觉调整

        // 绘制“PAUSED”文字，采用闪烁效果和渐变色
        drawBlinkingText(paused_x, paused_y, _T("PAUSED"), 45, RGB(255, 255, 0), RGB(255, 100, 0), 30); // 增大字号，添加闪烁

        // 绘制“按P键继续”提示
        settextcolor(RGB(150, 200, 255)); // 柔和的蓝色调
        settextstyle(24, 0, _T("微软雅黑")); // 改用微软雅黑，增大字号

        // 计算“按P键继续游戏”的X轴居中位置
        int continue_text_width = textwidth(_T("按P键继续游戏"));
        int continue_x = WINDOW_WIDTH / 2 - continue_text_width / 2;
        // 放置在PAUSED文字下方，并增加适当间距
        int continue_y = paused_y + textheight(_T("PAUSED")) + 20; // 使用PAUSED的字体高度计算间距
        outtextxy(continue_x, continue_y, _T("按P键继续游戏")); // 文字内容更符合中文语境

        // 显示声音状态，与主菜单保持一致的风格
        settextcolor(sound_manager_.isSoundEnabled() ?
            RGB(150, 255, 150) :    // 开启时绿色
            RGB(255, 100, 100));    // 关闭时红色
        settextstyle(16, 0, _T("微软雅黑")); // 与主菜单统一字体和字号

        // 居中显示声音状态，带图标效果
        TCHAR sound_status_text[50]; // 声音状态文本缓冲区
        _stprintf_s(sound_status_text, _T("%s 声音: %s (M键切换)"),
            sound_manager_.isSoundEnabled() ? _T("🔊") : _T("🔇"), // 根据状态显示图标
            sound_manager_.isSoundEnabled() ? _T("开") : _T("关")); // 显示开启或关闭
        int sound_text_width = textwidth(sound_status_text);
        int sound_x = WINDOW_WIDTH / 2 - sound_text_width / 2;
        // 放置在“按P键继续”文字下方，并增加间距
        int sound_y = continue_y + textheight(_T("按P键继续游戏")) + 30; // 使用“按P键继续游戏”的字体高度计算间距
        outtextxy(sound_x, sound_y, sound_status_text);
    }

    /// @brief 绘制游戏结束画面
    void drawGameOver() {
        setfillcolor(RGB(0, 0, 0, 180)); // 半透明黑色
        solidrectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

        // 绘制“GAME OVER”文字，采用更具冲击力的渐变效果
        settextstyle(60, 0, _T("微软雅黑"));
        int game_over_x = WINDOW_WIDTH / 2 - textwidth(_T("GAME OVER")) / 2;
        drawGradientText(game_over_x, 100, _T("GAME OVER"), 60,
            RGB(255, 0, 0), RGB(150, 0, 0), 1.3f); // 更深的红色渐变，更大间距

        // 添加强调的星空分割线，与菜单界面风格统一
        setlinecolor(RGB(255, 100, 100)); // 更鲜明的红色调分割线
        line(WINDOW_WIDTH / 2 - 250, 190, WINDOW_WIDTH / 2 + 250, 190); // 延长分割线长度，下移

        // 绘制最终得分 - 突出显示
        settextcolor(RGB(255, 255, 50)); // 更亮的黄色，增加视觉冲击力
        settextstyle(36, 0, _T("微软雅黑")); // 增大字号，强调重要性
        TCHAR final_score_text[100]; // 最终得分文本缓冲区
        _stprintf_s(final_score_text, _T("最终得分: %d"), score_);
        int score_x = WINDOW_WIDTH / 2 - textwidth(final_score_text) / 2;
        outtextxy(score_x, 240, final_score_text); // 调整位置，与分割线保持适当距离

        // 绘制达到等级 - 次要信息，但仍保持清晰
        settextcolor(RGB(100, 255, 100)); // 柔和的绿色
        settextstyle(28, 0, _T("微软雅黑")); // 字号略小，区分层级
        TCHAR level_reached_text[100]; // 达到等级文本缓冲区
        _stprintf_s(level_reached_text, _T("达到等级: %d"), level_);
        int level_x = WINDOW_WIDTH / 2 - textwidth(level_reached_text) / 2;
        outtextxy(level_x, 300, level_reached_text); // 放置在分数下方

        // 绘制返回菜单提示 - 引导操作，采用闪烁效果和更明显的颜色
        // 计算X轴居中位置
        settextstyle(32, 0, _T("微软雅黑")); // 临时设置字体以便计算宽度
        int return_menu_x = WINDOW_WIDTH / 2 - textwidth(_T("按R键返回主菜单")) / 2;
        drawBlinkingText(return_menu_x, 380, _T("按R键返回主菜单"), 32,
            RGB(0, 255, 255), RGB(0, 150, 200), 30); // 青色系闪烁，吸引注意力

        // 显示声音状态 - 保持与菜单和暂停界面的统一风格
        settextcolor(sound_manager_.isSoundEnabled() ?
            RGB(150, 255, 150) :    // 开启时绿色
            RGB(255, 100, 100));    // 关闭时红色
        settextstyle(16, 0, _T("微软雅黑")); // 统一字体和字号
        TCHAR sound_status_text[50]; // 声音状态文本缓冲区
        _stprintf_s(sound_status_text, _T("%s 声音: %s (M键切换)"),
            sound_manager_.isSoundEnabled() ? _T("🔊") : _T("🔇"),
            sound_manager_.isSoundEnabled() ? _T("开") : _T("关"));
        int sound_status_x = WINDOW_WIDTH / 2 - textwidth(sound_status_text) / 2;
        outtextxy(sound_status_x, 450, sound_status_text); // 位置调整

        // 绘制游戏提示 - 提供额外信息或鼓励
        settextcolor(RGB(180, 255, 180)); // 柔和的绿色
        settextstyle(20, 0, _T("微软雅黑")); // 略大字号，提高可读性
        int game_tip_x = WINDOW_WIDTH / 2 - textwidth(_T("收集能量道具增强你的飞船！")) / 2;
        outtextxy(game_tip_x, 520, _T("收集能量道具增强你的飞船！")); // 居中显示，位置下移
    }

    /// @brief 重置游戏
    void resetGame() {
        player_ = Player(WINDOW_WIDTH / 2, WINDOW_HEIGHT - 80);
        player_.loadImage();  // 加载玩家图片
        bullets_.clear();
        enemies_.clear();
        powerups_.clear();
        explosions_.clear();
        score_ = 0; // 初始分数
        level_ = 1; // 初始等级
        enemy_spawn_timer_ = bullet_cooldown_ = powerup_timer_ = danmaku_global_timer_ = 0;
        boss_spawn_cooldown_ = 0;
        boss_active_ = false;
    }

    /// @brief 生成敌人
    void spawnEnemies() {
        enemy_spawn_timer_++;
        int spawn_rate = max(20, 50 - level_ * 3);

        if (enemy_spawn_timer_ >= spawn_rate && enemies_.size() < MAX_ENEMIES) {
            float x = 50 + rand() % (WINDOW_WIDTH - 100);
            int type = 0; // 默认普通敌人

            if (level_ >= 3 && rand() % 100 < 15) type = 1; // 精英敌人
            if (level_ >= 5 && rand() % 100 < 8) { // 首领敌机生成几率
                // 只在首领未激活且冷却结束时生成
                if (!boss_active_ && boss_spawn_cooldown_ <= 0) {
                    type = 2; // 首领敌机
                    boss_active_ = true;
                }
                else {
                    type = 0; // 如果首领已激活或在冷却中，生成普通敌人
                }
            }

            // 如果首领已激活且不是首领类型才添加敌人
            if (type == 2) {
                Enemy enemy(x, -30, type);
                enemy.health_ = enemy.max_health_ = enemy.health_ + (level_ - 1) * 10; // 随等级提升血量
                enemies_.push_back(enemy);
                enemy_spawn_timer_ = 0;
            }
            else if (type == 1) { // 精英敌人
                Enemy enemy(x, -30, type);
                enemy.health_ = enemy.max_health_ = enemy.health_ + (level_ - 1) * 2; // 随等级提升血量
                enemies_.push_back(enemy);
                enemy_spawn_timer_ = 0;
            }
            else if (type == 0) { // 普通敌人
                Enemy enemy(x, -30, type);
                enemy.health_ = enemy.max_health_ = enemy.health_ + (level_ - 1) / 2; // 随等级提升血量
                enemies_.push_back(enemy);
                enemy_spawn_timer_ = 0;
            }
        }

        // 更新首领生成冷却
        if (boss_spawn_cooldown_ > 0) {
            boss_spawn_cooldown_--;
        }
    }

    /// @brief 生成道具
    void spawnPowerups() {
        powerup_timer_++;
        if (powerup_timer_ >= 240) { // 每4秒生成一个道具
            float x = 100 + rand() % (WINDOW_WIDTH - 200);
            int type_choice = rand() % 10;
            int type;
            if (type_choice < 3) type = 0; // 30% 生命道具
            else if (type_choice < 8) type = 1; // 50% 分数道具
            else type = 2; // 20% 攻击速度道具
            powerups_.push_back(PowerUp(x, -20, type));
            powerup_timer_ = 0;
        }
    }

    /// @brief 敌人射击
    void enemyShoot() {
        for (auto& enemy : enemies_) {
            if (enemy.active_ && enemy.canShoot()) {
                bullets_.push_back(Bullet(enemy.x, enemy.y + 20, 1, 0, 4));
                enemy.resetShoot();
                sound_manager_.playEffect("shoot"); // 播放敌人射击音效
            }
        }
    }

    /// @brief 全局弹幕
    void globalDanmaku() {
        danmaku_global_timer_++;
        int interval = max(120, 300 - level_ * 20); // 弹幕频率随等级提高
        int bullet_level = level_;

        if (danmaku_global_timer_ >= interval) {
            for (auto& enemy : enemies_) {
                if (enemy.active_) {
                    if (enemy.enemy_type_ == 2) { // 首领敌机弹幕模式
                        if (enemy.bullet_pattern_ == 0) { // 360度散射
                            int bullet_count = 16 + bullet_level * 2;//子弹数量
                            for (int i = 0; i < bullet_count; i++) {
                                float angle = (2 * 3.14159f * i) / bullet_count;//360度均匀分布
                                float speed = 2 + bullet_level * 0.5;//速度
                                float vx = cos(angle) * speed;
                                float vy = sin(angle) * speed;
                                bullets_.push_back(Bullet(enemy.x, enemy.y, 2, vx, vy));
                            }
                        }
                        else if (enemy.bullet_pattern_ == 1) { // 扇形弹幕
                            int num_bullets_in_fan = 5 + bullet_level;
                            float fan_angle = 60.0f * (3.14159f / 180.0f); // 60度扇形
                            float start_angle = atan2(player_.y - enemy.y, player_.x - enemy.x) - fan_angle / 2;//指向玩家

                            for (int i = 0; i < num_bullets_in_fan; i++) {
                                float angle = start_angle + (fan_angle / (num_bullets_in_fan - 1)) * i;
                                float speed = 2 + bullet_level * 0.75;
                                float vx = cos(angle) * speed;
                                float vy = sin(angle) * speed;
                                bullets_.push_back(Bullet(enemy.x, enemy.y, 2, vx, vy));
                            }
                        }
                        else if (enemy.bullet_pattern_ == 2) { // 追踪子弹
                            // 发射少量追踪子弹
                            for (int i = 0; i < 3; i++) {
                                // 计算从敌人到玩家的向量
                                float dx = player_.x - enemy.x;
                                float dy = player_.y - enemy.y;
                                float distance = sqrt(dx * dx + dy * dy);
                                if (distance < 1.0f) distance = 1.0f; // 避免除零

                                float speed = 1.5f + bullet_level * 0.5f; // 追踪子弹速度
                                float vx = (dx / distance) * speed;
                                float vy = (dy / distance) * speed;
                                bullets_.push_back(Bullet(enemy.x, enemy.y, 2, vx, vy));
                            }
                        }
                    }
                    else if (enemy.enemy_type_ == 1) {
                        int bullet_count = 12 + bullet_level;
                        float speed = 1.5f + bullet_level * 0.5f;
                        for (int i = 0; i < bullet_count; i++) {
                            float angle = (2 * 3.14159f * i) / bullet_count;
                            float vx = cos(angle) * speed;
                            float vy = sin(angle) * speed;
                            bullets_.push_back(Bullet(enemy.x, enemy.y, 2, vx, vy));
                        }
                    }
                    else { // 普通敌人弹幕模式（简单散射）
                        int bullet_count = 8 + bullet_level;
                        for (int i = 0; i < bullet_count; i++) {
                            float angle = (2 * 3.14159f * i) / bullet_count;
                            float speed = 1 + bullet_level * 0.25;
                            float vx = cos(angle) * speed;
                            float vy = sin(angle) * speed;
                            bullets_.push_back(Bullet(enemy.x, enemy.y, 2, vx, vy));
                        }
                    }
                    sound_manager_.playEffect("shoot"); // 播放弹幕音效
                }
            }
            danmaku_global_timer_ = 0;
        }
    }

    /// @brief 更新游戏对象
    void updateGameObjects() {
        player_.update();

        // 更新并移除无效子弹
        for (auto it = bullets_.begin(); it != bullets_.end();) {
            it->update();
            if (!it->active_) it = bullets_.erase(it);
            else ++it;
        }

        // 更新并移除无效敌人
        for (auto it = enemies_.begin(); it != enemies_.end();) {
            it->update();
            if (!it->active_) {
                if (it->y > WINDOW_HEIGHT) score_ = max(0, score_ - 5); // 敌人逃脱惩罚
                it = enemies_.erase(it);
            }
            else ++it;
        }

        // 更新并移除无效道具
        for (auto it = powerups_.begin(); it != powerups_.end();) {
            it->update();
            if (!it->active_) it = powerups_.erase(it);
            else ++it;
        }

        // 更新并移除无效爆炸效果
        for (auto it = explosions_.begin(); it != explosions_.end();) {
            it->update();
            if (!it->active_) it = explosions_.erase(it);
            else ++it;
        }
    }

    /// @brief 检测碰撞
    void checkCollisions() {
        // 玩家子弹击中敌人（包括普通子弹和激光子弹）
        for (auto& bullet : bullets_) {
            if (bullet.active_ && (bullet.type_ == 0 || bullet.type_ == 3)) {
                for (auto& enemy : enemies_) {
                    if (enemy.active_ && bullet.hitbox_.intersects(enemy.hitbox_)) {
                        bullet.active_ = false;
                        enemy.health_ -= bullet.damage_;

                        // 激光子弹特效（可选）
                        if (bullet.type_ == 2) {
                            // 可以添加激光击中特效
                            sound_manager_.playEffect("laser_hit");
                        }

                        if (enemy.health_ <= 0) {
                            enemy.active_ = false;
                            explosions_.push_back(Explosion(enemy.x, enemy.y));
                            sound_manager_.playEffect("explosion");
                            score_ += (enemy.enemy_type_ == 0) ? 10 : (enemy.enemy_type_ == 1) ? 30 : 100;

                            // 如果首领死亡，重置激活标志并开始冷却
                            if (enemy.enemy_type_ == 2) {
                                boss_active_ = false;
                                boss_spawn_cooldown_ = 60 * 10; // 10秒冷却
                            }

                            // 升级检查和玩家属性提升
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

        // 敌人子弹击中玩家
        for (auto& bullet : bullets_) {
            if (bullet.active_ && (bullet.type_ == 1 || bullet.type_ == 2) && bullet.hitbox_.intersects(player_.hitbox_)) {
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
                }
            }
        }

        // 敌人撞击玩家
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
                    // 此处留空，可选：护盾阻挡音效
                }
            }
        }

        // 玩家拾取道具
        for (auto& powerup : powerups_) {
            if (powerup.active_ && powerup.hitbox_.intersects(player_.hitbox_)) {
                powerup.active_ = false;
                sound_manager_.playEffect("powerup");
                if (powerup.type_ == 0) { // 生命道具
                    player_.heal();
                }
                else if (powerup.type_ == 1) { // 分数道具
                    score_ += 50;
                }
                else if (powerup.type_ == 2) { // 攻击速度道具
                    player_.attack_speed_boost_timer_ = 60 * 5; // 5秒持续时间
                }
            }
        }
    }

    /// @brief 绘制游戏元素
    void draw() {
        cleardevice();
        updateStars(); // 先更新星星，作为背景

        if (game_state_ == MENU) drawMenu();
        else if (game_state_ == PLAYING || game_state_ == PAUSED) {
            drawGame();
            if (game_state_ == PAUSED) drawPauseScreen();
        }
        else if (game_state_ == GAME_OVER) drawGameOver();
    }

    /// @brief 更新游戏逻辑
    void update() {
        if (game_state_ == PLAYING) {
            spawnEnemies();
            spawnPowerups();
            enemyShoot();
            globalDanmaku();
            updateGameObjects();
            checkCollisions();
        }
    }

    /// @brief 游戏主循环
    void run() {
        running_ = true; // 确保运行状态为true

        while (running_) {
            // 1. 处理输入（包括窗口关闭事件）
            handleInput();

            // 2. 更新游戏状态
            update();

            // 3. 渲染游戏画面
            draw();

            // 4. 控制帧率
            Sleep(16); // 约60FPS

            // 5. 检查退出条件（保留ESC退出）
            if (key_state_[VK_ESCAPE]) {
                quitGame();
            }
        }

        // 游戏循环结束后执行清理
        cleanupResources();
    }

    // 退出游戏
    void quitGame() {
        running_ = false; // 设置退出标志
    }

    // 清理资源
    void cleanupResources() {
    }
};

/// @brief 主函数
int main() {
    srand((unsigned)time(NULL)); // 初始化随机数生成器
    initgraph(WINDOW_WIDTH, WINDOW_HEIGHT); // 初始化图形窗口
    setbkcolor(RGB(0, 0, 0)); // 设置背景色为黑色

    GameManager game; // 创建游戏管理器实例
    game.run();       // 运行游戏循环

    closegraph(); // 关闭图形窗口
    return 0;
}