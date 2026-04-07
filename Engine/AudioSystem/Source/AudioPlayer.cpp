#include "AudioPlayer.h"
#include <thread>
#include <algorithm>
#include <Engine/Logging/Logging.h>

bool playWaveAsync(const WaveData& wave, float volume)
{
    const DWORD bufferSize = static_cast<DWORD>(wave.pcmData.size());
    if (bufferSize == 0)
    {
        eae6320::Logging::OutputError ("[AudioPlayer] playWaveAsync: empty PCM buffer");
        return false;
    }

    volume = std::clamp(volume, 0.0f, 1.0f);

    try
    {
        std::thread playbackThread([wave, bufferSize, volume]() {
            HWAVEOUT hWaveOut = nullptr;
            MMRESULT mmr = waveOutOpen(&hWaveOut, WAVE_MAPPER, &wave.format,
                0, 0, CALLBACK_NULL);
            if (mmr != MMSYSERR_NOERROR)
            {
                eae6320::Logging::OutputError("[AudioPlayer] waveOutOpen failed:", mmr);
                return;
            }

            // Allocate buffer and copy PCM data
            uint8_t* pcmCopy = new uint8_t[bufferSize];
            std::memcpy(pcmCopy, wave.pcmData.data(), bufferSize);

            // Apply volume
            if (wave.format.wBitsPerSample == 8)
            {
                // 8-bit PCM is unsigned [0, 255], 128 = 0
                for (DWORD i = 0; i < bufferSize; ++i)
                {
                    int sample = static_cast<int>(pcmCopy[i]) - 128;
                    sample = static_cast<int>(sample * volume);
                    pcmCopy[i] = static_cast<uint8_t>(std::clamp(sample + 128, 0, 255));
                }
            }
            else if (wave.format.wBitsPerSample == 16)
            {
                // 16-bit PCM is signed [-32768, 32767]
                int16_t* samples = reinterpret_cast<int16_t*>(pcmCopy);
                DWORD sampleCount = bufferSize / sizeof(int16_t);
                for (DWORD i = 0; i < sampleCount; ++i)
                {
                    int sample = static_cast<int>(samples[i]);
                    sample = static_cast<int>(sample * volume);
                    samples[i] = static_cast<int16_t>(std::clamp(sample, -32768, 32767));
                }
            }

            WAVEHDR* hdr = new WAVEHDR{};
            hdr->lpData = reinterpret_cast<LPSTR>(pcmCopy);
            hdr->dwBufferLength = bufferSize;
            hdr->dwFlags = 0;
            hdr->dwLoops = 0;

            mmr = waveOutPrepareHeader(hWaveOut, hdr, sizeof(WAVEHDR));
            if (mmr != MMSYSERR_NOERROR)
            {
                std::cerr << "[AudioPlayer] waveOutPrepareHeader failed: " << mmr << "\n";
                delete[] pcmCopy;
                delete hdr;
                waveOutClose(hWaveOut);
                return;
            }

            mmr = waveOutWrite(hWaveOut, hdr, sizeof(WAVEHDR));
            if (mmr != MMSYSERR_NOERROR)
            {
                std::cerr << "[AudioPlayer] waveOutWrite failed: " << mmr << "\n";
                waveOutUnprepareHeader(hWaveOut, hdr, sizeof(WAVEHDR));
                delete[] pcmCopy;
                delete hdr;
                waveOutClose(hWaveOut);
                return;
            }

            // Wait until done
            while (!(hdr->dwFlags & WHDR_DONE))
                std::this_thread::sleep_for(std::chrono::milliseconds(10));

            waveOutUnprepareHeader(hWaveOut, hdr, sizeof(WAVEHDR));
            waveOutClose(hWaveOut);
            delete[] pcmCopy;
            delete hdr;
            });

        playbackThread.detach();
    }
    catch (const std::system_error& e)
    {
        eae6320::Logging::OutputError("[AudioPlayer] Failed to create playback thread: ", e.what());
        return false;
    }

    return true;
}