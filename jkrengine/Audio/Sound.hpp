#pragma once
#include <SDLWindow.hpp>

namespace Jkr::Audio {
class Sound {
    public:
    Sound(sv inFileName);
    ~Sound();
    void Play();
    void Pause();
    void SetupAudioDevice();

    private:
    SDL_AudioDeviceID mDeviceId;
    SDL_AudioSpec mAudioSpec;
    uint8_t* mAudioBuffer;
    uint32_t mAudioLength;
};
} // namespace Jkr::Audio
