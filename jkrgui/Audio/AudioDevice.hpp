#ifdef ANDROID
#include <AndroidInclude/SDL2/SDL.h>
#else
#include <SDL2/SDL.h>
#endif

#include <Config.hpp>
#include <array>
#include <functional>

namespace Jkr::Audio {
using AudioCallbackType = SDL_AudioCallback;
using AudioFormat = SDL_AudioFormat;

class AudioDevice {
      public:
          GETTER GetId() { return mAudioDeviceId; }
          void Pause() { SDL_PauseAudioDevice(mAudioDeviceId, 1); }
          void Play() { SDL_PauseAudioDevice(mAudioDeviceId, 0); }
          AudioDevice(uint32_t inFrequency = 44100, AudioFormat inFormat = AUDIO_F32, uint32_t inChannels = 2, uint32_t inSamples = 512);
          ~AudioDevice();

      private:
          SDL_AudioSpec mAudioSpecDesired;
          SDL_AudioSpec mAudioSpecProvided;
          SDL_AudioDeviceID mAudioDeviceId;
          uint64_t mUserData;
          uint64_t mAudioSpecWant;
};
} // namespace Jkr::Audio
