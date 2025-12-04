#pragma once
#include <string>

namespace AudioSystem
{
    bool Initialize(const std::string& configPath);
    bool Play(float volume);
    bool PlayFromFile(const std::string& wavPath, float volume);
    bool PlaySFX(const std::string& key, float volume);
    bool PlayLoopingSFX(const std::string& key, float volume);
    void StopLoopingSFX(const std::string& key);
    void Shutdown();

}
