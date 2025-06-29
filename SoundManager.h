#pragma once
#include <string>
#include"common.h"
// ������Ϸ��Ч
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

