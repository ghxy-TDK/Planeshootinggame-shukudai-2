#pragma once
#include <string>
#include"common.h"
// 管理游戏音效
class SoundManager {
private:
    bool sound_enabled_;

public:
    SoundManager();
    void playBGM();
    void stopBGM();
    void playEffect(const char* effect);
    void toggleSound();
    bool isSoundEnabled() const;
};

