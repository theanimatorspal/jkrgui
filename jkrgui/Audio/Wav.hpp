#pragma once
#include "AudioDevice.hpp"

namespace Jkr::Audio {
using namespace ksai;
class Wav {
    public:
    up<Wav> Create(AudioDevice& inDevice, sv inFileName);
    void Build(AudioDevice& inDevice, sv inFileName);

    void Play(AudioDevice& inDevice);
    void Pause(AudioDevice& inDevice);

    Wav();
    Wav(AudioDevice& inDevice, sv inFileName);
    ~Wav();

    private:
    SDL_AudioSpec mAudioSpec;
    uint8_t* mAudioBuffer = nullptr;
    uint32_t mWaveLength;
};
}; // namespace Jkr::Audio