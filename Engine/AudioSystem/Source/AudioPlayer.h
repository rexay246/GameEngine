#pragma once
#include <windows.h>
#include <mmsystem.h>
#include <vector>
#include <cstdint>
#include <iostream>
#include "WavLoader.h"

#pragma comment(lib, "winmm.lib")

// Simple blocking WAV player using the Windows waveOut API
bool playWaveAsync(const WaveData& wave, float volume);
