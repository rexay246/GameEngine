#include "WavLoader.h"
#include <cstring>
#include <iostream>

static bool readLE32(const std::vector<uint8_t>& bytes, size_t& pos, uint32_t& v)
{
    if (pos + 4 > bytes.size()) return false;
    v = bytes[pos] | (bytes[pos + 1] << 8) | (bytes[pos + 2] << 16) | (bytes[pos + 3] << 24);
    pos += 4;
    return true;
}

static bool readLE16(const std::vector<uint8_t>& bytes, size_t& pos, uint16_t& v)
{
    if (pos + 2 > bytes.size()) return false;
    v = bytes[pos] | (bytes[pos + 1] << 8);
    pos += 2;
    return true;
}

bool parseWav(const std::vector<uint8_t>& fileBytes, WaveData& out)
{
    size_t pos = 0;
    if (fileBytes.size() < 12) return false;
    if (memcmp(fileBytes.data(), "RIFF", 4) != 0) return false;
    if (memcmp(fileBytes.data() + 8, "WAVE", 4) != 0) return false;
    pos = 12;

    bool fmtFound = false, dataFound = false;
    uint16_t audioFormat = 0, numChannels = 0, bitsPerSample = 0;
    uint32_t sampleRate = 0;
    std::vector<uint8_t> dataChunk;

    while (pos + 8 <= fileBytes.size())
    {
        char chunkId[5] = { 0 };
        memcpy(chunkId, &fileBytes[pos], 4);
        pos += 4;

        uint32_t chunkSize = 0;
        if (!readLE32(fileBytes, pos, chunkSize)) return false;

        if (memcmp(chunkId, "fmt ", 4) == 0)
        {
            if (!readLE16(fileBytes, pos, audioFormat)) return false;
            if (!readLE16(fileBytes, pos, numChannels)) return false;
            if (!readLE32(fileBytes, pos, sampleRate)) return false;

            uint32_t byteRate; uint16_t blockAlign;
            if (!readLE32(fileBytes, pos, byteRate)) return false;
            if (!readLE16(fileBytes, pos, blockAlign)) return false;
            if (!readLE16(fileBytes, pos, bitsPerSample)) return false;

            pos += (chunkSize - 16); // skip remaining if any
            fmtFound = true;
        }
        else if (memcmp(chunkId, "data", 4) == 0)
        {
            if (pos + chunkSize > fileBytes.size()) return false;
            dataChunk.resize(chunkSize);
            memcpy(dataChunk.data(), fileBytes.data() + pos, chunkSize);
            pos += chunkSize;
            dataFound = true;
        }
        else
        {
            pos += chunkSize;
        }

        if (chunkSize & 1) pos++; // align
    }

    if (!fmtFound || !dataFound || audioFormat != 1)
    {
        std::cerr << "[WavLoader] Unsupported or invalid WAV format.\n";
        return false;
    }

    out.format.wFormatTag = WAVE_FORMAT_PCM;
    out.format.nChannels = numChannels;
    out.format.nSamplesPerSec = sampleRate;
    out.format.wBitsPerSample = bitsPerSample;
    out.format.nBlockAlign = (numChannels * bitsPerSample) / 8;
    out.format.nAvgBytesPerSec = out.format.nSamplesPerSec * out.format.nBlockAlign;
    out.format.cbSize = 0;
    out.pcmData.swap(dataChunk);
    return true;
}
