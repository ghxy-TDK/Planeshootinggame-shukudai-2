#include "SoundManager.h"
#pragma comment(lib, "winmm.lib")
SoundManager::SoundManager() : sound_enabled_(true) {}

void SoundManager::playBGM() {
    if (sound_enabled_) {
        PlaySound(_T("bgm.wav"), NULL, SND_ASYNC | SND_LOOP | SND_FILENAME);
    }
}

void SoundManager::stopBGM() {
    PlaySound(NULL, NULL, SND_ASYNC);
}

void SoundManager::playEffect(const char* effect) {
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

void SoundManager::toggleSound() {
    sound_enabled_ = !sound_enabled_;
    if (!sound_enabled_) {
        stopBGM();
    }
}

bool SoundManager::isSoundEnabled() const {
    return sound_enabled_;
}