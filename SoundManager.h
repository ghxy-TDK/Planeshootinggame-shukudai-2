#pragma once
#include <windows.h>
#include <mmsystem.h>
#include <string>
#include <map>
#include <vector>
#include "common.h"

#pragma comment(lib, "winmm.lib")

class SoundManager {
private:
    static SoundManager* instance_;

    bool sound_enabled_;
    bool bgm_playing_;
    float master_volume_;
    float bgm_volume_;
    float sfx_volume_;

    std::string current_bgm_file_;
    std::map<std::string, std::string> sound_effects_;

    // 查找音频文件的完整路径
    std::string FindAudioFile(const std::string& filename);

    // 检查文件是否存在
    bool FileExists(const std::string& filepath);

    // 播放单个文件（内部使用）
    bool PlayAudioFile(const std::string& filepath, bool loop = false, bool async = true);

public:
    SoundManager();
    ~SoundManager();

    static SoundManager* GetInstance();
    static void DestroyInstance();

    // 初始化
    bool Initialize();
    void Shutdown();

    // 背景音乐控制
    bool PlayBGM(const std::string& filename);
    void StopBGM();
    void PauseBGM();
    void ResumeBGM();
    bool IsBGMPlaying() const;

    // 音效控制
    bool LoadSoundEffect(const std::string& name, const std::string& filename);
    bool PlaySoundEffect(const std::string& name);
    void StopAllSounds();

    // 音量控制（简化版）
    void SetMasterVolume(float volume);
    void SetBGMVolume(float volume);
    void SetSFXVolume(float volume);
    float GetMasterVolume() const { return master_volume_; }
    float GetBGMVolume() const { return bgm_volume_; }
    float GetSFXVolume() const { return sfx_volume_; }

    // 总开关
    void SetSoundEnabled(bool enabled);
    bool IsSoundEnabled() const { return sound_enabled_; }
    void ToggleSound();

    // 便捷方法（兼容旧接口）
    void playBGM();
    void stopBGM() { StopBGM(); }
    void playEffect(const char* effect);
    void toggleSound() { ToggleSound(); }
    bool isSoundEnabled() const { return IsSoundEnabled(); }

    // 调试方法
    void ListAvailableFiles();
};