#include "Sound.hpp"

using namespace Jkr::Audio;
Sound::Sound(sv inFileName) {
    if (SDL_LoadWAV(inFileName.data(), &mAudioSpec, &mAudioBuffer, &mAudioLength) == nullptr) {
        std::cout << "Couldn't load audio " << inFileName << "\n";
    }
}

Sound::~Sound() {
    SDL_FreeWAV(mAudioBuffer);
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
    mDeviceId = SDL_OpenAudioDevice(nullptr, 0, &mAudioSpec, nullptr, 0);
    if (mDeviceId == 0) {
        std::cout << "Error, Failed to Initialize Audio Deviec \n";
    }
}