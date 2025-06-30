#include "SoundManager.h"
#include <iostream>
#include <fstream>
#include <sstream>

SoundManager* SoundManager::instance_ = nullptr;

SoundManager::SoundManager()
    : sound_enabled_(true)
    , bgm_playing_(false)
    , master_volume_(1.0f)
    , bgm_volume_(0.8f)
    , sfx_volume_(1.0f) {
}

SoundManager::~SoundManager() {
    Shutdown();
}

SoundManager* SoundManager::GetInstance() {
    if (instance_ == nullptr) {
        instance_ = new SoundManager();
    }
    return instance_;
}

void SoundManager::DestroyInstance() {
    if (instance_) {
        delete instance_;
        instance_ = nullptr;
    }
}

bool SoundManager::Initialize() {
    // Ԥ������Ч�ļ�·��
    sound_effects_["shoot"] = "shoot.wav";
    sound_effects_["explosion"] = "explosion.wav";
    sound_effects_["powerup"] = "powerup.wav";
    sound_effects_["damage"] = "damage.wav";
    sound_effects_["gameover"] = "gameover.wav";

    // ���������Ϣ
    ListAvailableFiles();

    return true;
}

void SoundManager::Shutdown() {
    StopBGM();
    StopAllSounds();
}

bool SoundManager::FileExists(const std::string& filepath) {
    std::ifstream file(filepath);
    return file.good();
}

std::string SoundManager::FindAudioFile(const std::string& filename) {
    // ���ܵ�����·��
    std::vector<std::string> search_paths = {
        "",                    // ��ǰĿ¼
        "res\\",              // resĿ¼
        "assets\\",           // assetsĿ¼
        "audio\\",            // audioĿ¼
        "sounds\\",           // soundsĿ¼
        ".\\",                // ��ȷ�ĵ�ǰĿ¼
        ".\\res\\",           // ��ȷ��resĿ¼
        ".\\assets\\",        // ��ȷ��assetsĿ¼
        ".\\audio\\",         // ��ȷ��audioĿ¼
        ".\\sounds\\"         // ��ȷ��soundsĿ¼
    };

    // ���ܵ��ļ���չ��
    std::vector<std::string> extensions = {
        "",       // ԭʼ�ļ����������Ѱ�����չ����
        ".wav",   // WAV��ʽ
        ".mp3"    // MP3��ʽ
    };

    std::string base_filename = filename;

    // ����ļ����Ѿ�������չ�����Ƴ�����������
    size_t dot_pos = filename.find_last_of('.');
    if (dot_pos != std::string::npos) {
        std::string ext = filename.substr(dot_pos);
        if (ext == ".wav" || ext == ".mp3") {
            base_filename = filename.substr(0, dot_pos);
        }
    }

    // �������п��ܵ����
    for (const auto& path : search_paths) {
        for (const auto& ext : extensions) {
            std::string full_path = path + base_filename + ext;
            if (FileExists(full_path)) {
                return full_path;
            }

            // Ҳ����ԭʼ�ļ���
            full_path = path + filename;
            if (FileExists(full_path)) {
                return full_path;
            }
        }
    }

    return ""; // δ�ҵ��ļ�
}

bool SoundManager::PlayAudioFile(const std::string& filepath, bool loop, bool async) {
    if (!sound_enabled_ || filepath.empty()) {
        return false;
    }

    DWORD flags = SND_FILENAME;
    if (async) flags |= SND_ASYNC;
    if (loop) flags |= SND_LOOP;

    // ת��Ϊ���ַ��������Ҫ��
    std::wstring wfilepath(filepath.begin(), filepath.end());

    BOOL result = PlaySoundW(wfilepath.c_str(), NULL, flags);

    if (!result) {
        // ����ʹ��ANSI�汾
        result = PlaySoundA(filepath.c_str(), NULL, flags);
    }

    return result != FALSE;
}

bool SoundManager::PlayBGM(const std::string& filename) {
    if (!sound_enabled_) {
        return false;
    }

    std::string filepath = FindAudioFile(filename);
    if (filepath.empty()) {
        return false;
    }

    StopBGM(); // ֹͣ��ǰBGM

    if (PlayAudioFile(filepath, true, true)) {
        current_bgm_file_ = filepath;
        bgm_playing_ = true;
        return true;
    }

    return false;
}

void SoundManager::StopBGM() {
    PlaySound(NULL, NULL, SND_ASYNC);
    bgm_playing_ = false;
    current_bgm_file_.clear();
}

void SoundManager::PauseBGM() {
    if (bgm_playing_) {
        PlaySound(NULL, NULL, SND_ASYNC);
    }
}

void SoundManager::ResumeBGM() {
    if (bgm_playing_ && !current_bgm_file_.empty()) {
        PlayAudioFile(current_bgm_file_, true, true);
    }
}

bool SoundManager::IsBGMPlaying() const {
    return bgm_playing_;
}

bool SoundManager::LoadSoundEffect(const std::string& name, const std::string& filename) {
    std::string filepath = FindAudioFile(filename);
    if (!filepath.empty()) {
        sound_effects_[name] = filepath;
        return true;
    }
    return false;
}

bool SoundManager::PlaySoundEffect(const std::string& name) {
    if (!sound_enabled_) {
        return false;
    }

    auto it = sound_effects_.find(name);
    if (it != sound_effects_.end()) {
        std::string filepath = it->second;

        // ����洢�������·�������²���
        if (!FileExists(filepath)) {
            filepath = FindAudioFile(filepath);
            if (!filepath.empty()) {
                sound_effects_[name] = filepath; // ����Ϊ�ҵ���·��
            }
        }

        if (!filepath.empty()) {
            return PlayAudioFile(filepath, false, true);
        }
    }

    return false;
}

void SoundManager::StopAllSounds() {
    PlaySound(NULL, NULL, SND_ASYNC);
    bgm_playing_ = false;
}

void SoundManager::SetMasterVolume(float volume) {
    //master_volume_ = std::max(0.0f, std::min(1.0f, volume));
}

void SoundManager::SetBGMVolume(float volume) {
    //bgm_volume_ = std::max(0.0f, std::min(1.0f, volume));
}

void SoundManager::SetSFXVolume(float volume) {
    //sfx_volume_ = std::max(0.0f, std::min(1.0f, volume));
}

void SoundManager::SetSoundEnabled(bool enabled) {
    sound_enabled_ = enabled;
    if (!enabled) {
        StopBGM();
        StopAllSounds();
    }
}

void SoundManager::ToggleSound() {
    SetSoundEnabled(!sound_enabled_);
}

void SoundManager::ListAvailableFiles() {
    std::vector<std::string> test_files = {
        "backgroundmusic2.wav",
        "backgroundmusic2.mp3",
        "shoot.wav",
        "explosion.wav",
        "powerup.wav",
        "damage.wav",
        "gameover.wav"
    };

    OutputDebugStringA("=== SoundManager: Searching for audio files ===\n");

    for (const auto& filename : test_files) {
        std::string found_path = FindAudioFile(filename);
        if (!found_path.empty()) {
            std::string msg = "Found: " + filename + " at " + found_path + "\n";
            OutputDebugStringA(msg.c_str());
        }
        else {
            std::string msg = "NOT FOUND: " + filename + "\n";
            OutputDebugStringA(msg.c_str());
        }
    }

    OutputDebugStringA("=== End of audio file search ===\n");
}

// ���ݾɽӿڵķ���
void SoundManager::playBGM() {
    // ���Զ�����ܵı��������ļ���
    std::vector<std::string> bgm_files = {
        "backgroundmusic2.wav",
        "backgroundmusic2.mp3",
        "bgm.wav",
        "bgm.mp3",
        "background.wav",
        "background.mp3"
    };

    for (const auto& filename : bgm_files) {
        if (PlayBGM(filename)) {
            return; // �ɹ����ž��˳�
        }
    }

    OutputDebugStringA("Failed to play any background music file\n");
}

void SoundManager::playEffect(const char* effect) {
    if (!sound_enabled_) return;

    std::string effectName(effect);
    if (!PlaySoundEffect(effectName)) {
        // �������ʧ�ܣ�����ֱ�Ӱ��ļ�������
        std::string filename = effectName + ".wav";
        std::string filepath = FindAudioFile(filename);
        if (!filepath.empty()) {
            PlayAudioFile(filepath, false, true);
        }
    }
}