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
    // 预加载音效文件路径
    sound_effects_["shoot"] = "shoot.wav";
    sound_effects_["explosion"] = "explosion.wav";
    sound_effects_["powerup"] = "powerup.wav";
    sound_effects_["damage"] = "damage.wav";
    sound_effects_["gameover"] = "gameover.wav";

    // 输出调试信息
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
    // 可能的搜索路径
    std::vector<std::string> search_paths = {
        "",                    // 当前目录
        "res\\",              // res目录
        "assets\\",           // assets目录
        "audio\\",            // audio目录
        "sounds\\",           // sounds目录
        ".\\",                // 明确的当前目录
        ".\\res\\",           // 明确的res目录
        ".\\assets\\",        // 明确的assets目录
        ".\\audio\\",         // 明确的audio目录
        ".\\sounds\\"         // 明确的sounds目录
    };

    // 可能的文件扩展名
    std::vector<std::string> extensions = {
        "",       // 原始文件名（可能已包含扩展名）
        ".wav",   // WAV格式
        ".mp3"    // MP3格式
    };

    std::string base_filename = filename;

    // 如果文件名已经包含扩展名，移除它用于搜索
    size_t dot_pos = filename.find_last_of('.');
    if (dot_pos != std::string::npos) {
        std::string ext = filename.substr(dot_pos);
        if (ext == ".wav" || ext == ".mp3") {
            base_filename = filename.substr(0, dot_pos);
        }
    }

    // 搜索所有可能的组合
    for (const auto& path : search_paths) {
        for (const auto& ext : extensions) {
            std::string full_path = path + base_filename + ext;
            if (FileExists(full_path)) {
                return full_path;
            }

            // 也尝试原始文件名
            full_path = path + filename;
            if (FileExists(full_path)) {
                return full_path;
            }
        }
    }

    return ""; // 未找到文件
}

bool SoundManager::PlayAudioFile(const std::string& filepath, bool loop, bool async) {
    if (!sound_enabled_ || filepath.empty()) {
        return false;
    }

    DWORD flags = SND_FILENAME;
    if (async) flags |= SND_ASYNC;
    if (loop) flags |= SND_LOOP;

    // 转换为宽字符（如果需要）
    std::wstring wfilepath(filepath.begin(), filepath.end());

    BOOL result = PlaySoundW(wfilepath.c_str(), NULL, flags);

    if (!result) {
        // 尝试使用ANSI版本
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

    StopBGM(); // 停止当前BGM

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

        // 如果存储的是相对路径，重新查找
        if (!FileExists(filepath)) {
            filepath = FindAudioFile(filepath);
            if (!filepath.empty()) {
                sound_effects_[name] = filepath; // 更新为找到的路径
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

// 兼容旧接口的方法
void SoundManager::playBGM() {
    // 尝试多个可能的背景音乐文件名
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
            return; // 成功播放就退出
        }
    }

    OutputDebugStringA("Failed to play any background music file\n");
}

void SoundManager::playEffect(const char* effect) {
    if (!sound_enabled_) return;

    std::string effectName(effect);
    if (!PlaySoundEffect(effectName)) {
        // 如果播放失败，尝试直接按文件名查找
        std::string filename = effectName + ".wav";
        std::string filepath = FindAudioFile(filename);
        if (!filepath.empty()) {
            PlayAudioFile(filepath, false, true);
        }
    }
}