#pragma once
#include <string>
#include <windows.h>
#include <mmsystem.h>
#include "common.h"

// …˘“Ùπ‹¿Ì¿‡
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