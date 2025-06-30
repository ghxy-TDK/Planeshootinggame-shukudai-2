#include "SoundManager.h"
#pragma comment(lib, "winmm.lib")

SoundManager::SoundManager() : sound_enabled_(true) {
    // 打开音频设备别名
    mciSendString(_T("open res\\backgroundmusic2.mp3 alias bgm"), NULL, 0, NULL);
    mciSendString(_T("open res\\shoot.wav alias shoot"), NULL, 0, NULL);
    mciSendString(_T("open res\\explosion.wav alias explosion"), NULL, 0, NULL);
    mciSendString(_T("open res\\powerup.wav alias powerup"), NULL, 0, NULL);
    mciSendString(_T("open res\\damage.wav alias damage"), NULL, 0, NULL);
    mciSendString(_T("open res\\gameover.wav alias gameover"), NULL, 0, NULL);
}

void SoundManager::playBGM() {
    if (sound_enabled_) {
        mciSendString(_T("play bgm repeat from 0"), NULL, 0, NULL);
    }
}

void SoundManager::stopBGM() {
    mciSendString(_T("stop bgm"), NULL, 0, NULL);
}

void SoundManager::playEffect(const char* effect) {
    if (sound_enabled_) {
        if (strcmp(effect, "shoot") == 0) {
            mciSendString(_T("seek shoot to start"), NULL, 0, NULL);
            mciSendString(_T("play shoot"), NULL, 0, NULL);
        }
        else if (strcmp(effect, "explosion") == 0) {
            mciSendString(_T("seek explosion to start"), NULL, 0, NULL);
            mciSendString(_T("play explosion"), NULL, 0, NULL);
        }
        else if (strcmp(effect, "powerup") == 0) {
            mciSendString(_T("seek powerup to start"), NULL, 0, NULL);
            mciSendString(_T("play powerup"), NULL, 0, NULL);
        }
        else if (strcmp(effect, "damage") == 0) {
            mciSendString(_T("seek damage to start"), NULL, 0, NULL);
            mciSendString(_T("play damage"), NULL, 0, NULL);
        }
        else if (strcmp(effect, "gameover") == 0) {
            mciSendString(_T("seek gameover to start"), NULL, 0, NULL);
            mciSendString(_T("play gameover"), NULL, 0, NULL);
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