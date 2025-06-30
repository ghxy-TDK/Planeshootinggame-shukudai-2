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

    // ������Ƶ�ļ�������·��
    std::string FindAudioFile(const std::string& filename);

    // ����ļ��Ƿ����
    bool FileExists(const std::string& filepath);

    // ���ŵ����ļ����ڲ�ʹ�ã�
    bool PlayAudioFile(const std::string& filepath, bool loop = false, bool async = true);

public:
    SoundManager();
    ~SoundManager();

    static SoundManager* GetInstance();
    static void DestroyInstance();

    // ��ʼ��
    bool Initialize();
    void Shutdown();

    // �������ֿ���
    bool PlayBGM(const std::string& filename);
    void StopBGM();
    void PauseBGM();
    void ResumeBGM();
    bool IsBGMPlaying() const;

    // ��Ч����
    bool LoadSoundEffect(const std::string& name, const std::string& filename);
    bool PlaySoundEffect(const std::string& name);
    void StopAllSounds();

    // �������ƣ��򻯰棩
    void SetMasterVolume(float volume);
    void SetBGMVolume(float volume);
    void SetSFXVolume(float volume);
    float GetMasterVolume() const { return master_volume_; }
    float GetBGMVolume() const { return bgm_volume_; }
    float GetSFXVolume() const { return sfx_volume_; }

    // �ܿ���
    void SetSoundEnabled(bool enabled);
    bool IsSoundEnabled() const { return sound_enabled_; }
    void ToggleSound();

    // ��ݷ��������ݾɽӿڣ�
    void playBGM();
    void stopBGM() { StopBGM(); }
    void playEffect(const char* effect);
    void toggleSound() { ToggleSound(); }
    bool isSoundEnabled() const { return IsSoundEnabled(); }

    // ���Է���
    void ListAvailableFiles();
};