#pragma once
#include <vector>
#include <cstdint>
#include <windows.h>

// Represents parsed PCM WAV data
struct WaveData
{
    WAVEFORMATEX format{};
    std::vector<uint8_t> pcmData;
};

// Reads and parses a .wav file (PCM only)
bool parseWav(const std::vector<uint8_t>& fileBytes, WaveData& out);
