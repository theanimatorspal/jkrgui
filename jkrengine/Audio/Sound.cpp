#include "Sound.hpp"

using namespace Jkr::Audio;
Sound::Sound() { Sound::SetupAudioDevice(); }

void Sound::FromMemory(v<char> &inAudio) {
    mAudioBuffer = (uint8_t*)inAudio.data();
    mAudioLength = inAudio.size();
    mGenerated   = true;
}

void Sound::FromFile(sv inFileName) {
    if (SDL_LoadWAV(inFileName.data(), &mAudioSpec, &mAudioBuffer, &mAudioLength) == nullptr) {
        mGenerated = false;
        Log("Couldn't load audio", "ERROR");
    }
}

Sound::~Sound() {
    if(mGenerated.has_value())
    {
        if (not mGenerated) {
            SDL_FreeWAV(mAudioBuffer);
        }
    }
    if (mDeviceId) {
        SDL_CloseAudioDevice(mDeviceId);
    }
}

void Sound::Play() {
    SDL_QueueAudio(mDeviceId, mAudioBuffer, mAudioLength);
    SDL_PauseAudioDevice(mDeviceId, 0);
}

void Sound::Pause() {
    SDL_ClearQueuedAudio(mDeviceId);
    SDL_PauseAudioDevice(mDeviceId, 1);
}

void Sound::SetupAudioDevice() {
    mAudioSpec.freq     = 44100;
    mAudioSpec.format   = AUDIO_U8;
    mAudioSpec.channels = 2;
    mAudioSpec.samples  = 1024 * 4;
    mAudioSpec.callback = nullptr;
    mDeviceId           = SDL_OpenAudioDevice(nullptr, 0, &mAudioSpec, nullptr, 0);
    if (mDeviceId == 0) {
        Log("Error, Failed to Initialize Audio Device", "ERROR");
    }
}