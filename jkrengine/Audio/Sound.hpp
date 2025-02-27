#pragma once
#include <SDLWindow.hpp>

namespace Jkr::Audio {
class Sound {
    public:
    Sound();
    ~Sound();
    void Play();
    void Pause();
    void SetupAudioDevice();
    void FromMemory(v<char> &inAudio);
    void FromFile(sv inFileName);

    private:
    opt<bool> mGenerated = std::nullopt;
    SDL_AudioDeviceID mDeviceId;
    SDL_AudioSpec mAudioSpec;
    uint8_t *mAudioBuffer;
    uint32_t mAudioLength;
};
} // namespace Jkr::Audio
