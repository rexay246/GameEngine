#include "AudioSystem.h"
#include "AudioPlayer.h"
#include "WavLoader.h"
#include "Utils.h"
#include "External/JSON/Includes.h"
#include "Engine/Logging/Logging.h"
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <thread>
#include <memory>
#include <mutex>

using json = nlohmann::json;

namespace AudioSystem
{
    static std::unique_ptr<WaveData> s_BackgroundData;
    static std::unordered_map<std::string, std::unique_ptr<WaveData>> s_SFXMap;
    static std::string s_BackgroundPath;

    struct LoopData
    {
        std::thread thread;
        std::atomic<bool> stopFlag{ false };
    };

    static std::unordered_map<std::string, LoopData> s_LoopingSounds;
    std::mutex s_LoopingMutex;


    bool Initialize(const std::string& configPath)
    {
        std::ifstream ifs(configPath);
        if (!ifs)
        {
            eae6320::Logging::OutputError("[AudioSystem] Failed to open config: %s", configPath.c_str());
            return false;
        }

        json j;
        try { ifs >> j; }
        catch (const std::exception& e)
        {
            eae6320::Logging::OutputError("[AudioSystem] JSON parse error: %s", e.what());
            return false;
        }

        // --- Background ---
        if (j.contains("wav_path") && j["wav_path"].is_string())
        {
            s_BackgroundPath = j["wav_path"].get<std::string>();
            eae6320::Logging::OutputMessage("[AudioSystem] Loading Background: %s", s_BackgroundPath.c_str());

            std::vector<uint8_t> bgBytes;
            if (!readFileToVector(s_BackgroundPath, bgBytes))
            {
                eae6320::Logging::OutputError("[AudioSystem] Failed to read background file: %s", s_BackgroundPath.c_str());
                return false;
            }

            s_BackgroundData = std::make_unique<WaveData>();
            if (!parseWav(bgBytes, *s_BackgroundData))
            {
                eae6320::Logging::OutputError("[AudioSystem] Failed to parse background WAV: %s", s_BackgroundPath.c_str());
                return false;
            }

            eae6320::Logging::OutputMessage(
                "[AudioSystem] Loaded Background - Channels: %u | Rate: %u | Bits: %u",
                s_BackgroundData->format.nChannels,
                s_BackgroundData->format.nSamplesPerSec,
                s_BackgroundData->format.wBitsPerSample
            );
        }
        else
        {
            eae6320::Logging::OutputError("[AudioSystem] Missing 'wav_path' in config.");
        }

        // --- SFX ---
        if (j.contains("sfx") && j["sfx"].is_object())
        {
            for (auto& [key, val] : j["sfx"].items())
            {
                if (!val.is_string()) continue;

                std::string path = val.get<std::string>();
                eae6320::Logging::OutputMessage("[AudioSystem] Loading SFX [%s]: %s", key.c_str(), path.c_str());

                std::vector<uint8_t> bytes;
                if (!readFileToVector(path, bytes))
                {
                    eae6320::Logging::OutputError("[AudioSystem] Failed to read SFX file: %s", path.c_str());
                    continue;
                }

                auto data = std::make_unique<WaveData>();
                if (parseWav(bytes, *data))
                {
                    s_SFXMap[key] = std::move(data);
                    eae6320::Logging::OutputMessage("[AudioSystem] Loaded SFX: %s", key.c_str());
                }
                else
                {
                    eae6320::Logging::OutputError("[AudioSystem] Failed to parse SFX WAV: %s", path.c_str());
                }
            }
        }
        else
        {
            eae6320::Logging::OutputError("[AudioSystem] No 'sfx' section found in config.");
        }

        return true;
    }

    bool Play(float volume)
    {
        if (!s_BackgroundData)
        {
            eae6320::Logging::OutputError("[AudioSystem] No background WAV loaded!");
            return false;
        }
        return playWaveAsync(*s_BackgroundData, volume);
    }

    bool PlayFromFile(const std::string& wavPath, float volume)
    {
        std::vector<uint8_t> fileBytes;
        if (!readFileToVector(wavPath, fileBytes))
        {
            eae6320::Logging::OutputError("[AudioSystem] Failed to read WAV: %s", wavPath.c_str());
            return false;
        }

        WaveData wave;
        if (!parseWav(fileBytes, wave))
        {
            eae6320::Logging::OutputError("[AudioSystem] Invalid WAV format for: %s", wavPath.c_str());
            return false;
        }

        return playWaveAsync(wave, volume);
    }

    bool PlaySFX(const std::string& key, float volume)
    {
        auto it = s_SFXMap.find(key);
        if (it == s_SFXMap.end())
        {
            eae6320::Logging::OutputError("[AudioSystem] SFX key not found: %s", key.c_str());
            return false;
        }

        if (!it->second)
        {
            eae6320::Logging::OutputError("[AudioSystem] SFX data is null for key: %s", key.c_str());
            return false;
        }

        return playWaveAsync(*it->second, volume);
    }

    bool PlayLoopingSFX(const std::string& key, float volume)
    {
        auto itSfx = s_SFXMap.find(key);
        if (itSfx == s_SFXMap.end() || !itSfx->second)
        {
            eae6320::Logging::OutputError("[AudioSystem] Looping SFX not found: %s", key.c_str());
            return false;
        }

        {
            std::lock_guard<std::mutex> lock(s_LoopingMutex);

            if (s_LoopingSounds.find(key) != s_LoopingSounds.end())
                return true;

            auto [iter, inserted] = s_LoopingSounds.try_emplace(key);
            LoopData& entry = iter->second;
            entry.stopFlag.store(false);

            WaveData& soundData = *itSfx->second;
            entry.thread = std::thread([key, &soundData, volume, stopFlag = &entry.stopFlag]()
                {
                    eae6320::Logging::OutputMessage("[AudioSystem] Looping SFX started: %s", key.c_str());
                    while (!stopFlag->load())
                    {
                        playWaveAsync(soundData, volume);
                        std::this_thread::sleep_for(std::chrono::milliseconds(250));
                    }
                    eae6320::Logging::OutputMessage("[AudioSystem] Looping SFX stopping thread: %s", key.c_str());
                });
        }

        return true;
    }

    void StopLoopingSFX(const std::string& key)
    {
        std::lock_guard<std::mutex> lock(s_LoopingMutex);

        auto it = s_LoopingSounds.find(key);
        if (it == s_LoopingSounds.end())
        {
            eae6320::Logging::OutputError("[AudioSystem] No looping SFX found to stop: %s", key.c_str());
            return;
        }

        it->second.stopFlag.store(true);

        if (it->second.thread.joinable())
            it->second.thread.join();

        s_LoopingSounds.erase(it);
        eae6320::Logging::OutputMessage("[AudioSystem] Looping SFX stopped: %s", key.c_str());
    }

    void Shutdown()
    {
        for (auto& pair : s_LoopingSounds)
        {
            pair.second.stopFlag.store(true);
        }
        for (auto& pair : s_LoopingSounds)
        {
            if (pair.second.thread.joinable())
                pair.second.thread.join();
        }

        s_LoopingSounds.clear();
        s_BackgroundData.reset();
        s_SFXMap.clear();

        eae6320::Logging::OutputMessage("[AudioSystem] Shutdown complete.");
    }
}
