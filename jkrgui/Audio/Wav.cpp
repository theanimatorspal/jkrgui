#include "Wav.hpp"

using namespace Jkr::Audio;

void Wav::Build(AudioDevice& inDevice, sv inFileName) {
    if (SDL_LoadWAV(inFileName.data(), &mAudioSpec, &mAudioBuffer, &mWaveLength) == nullptr) {
        std::cout << "Error Loading Audio" << inFileName << '\n';
    }
}

Wav::Wav() {}
Wav::~Wav() {
    if (mAudioBuffer != nullptr) {
        SDL_FreeWAV(mAudioBuffer);
    }
}

void Wav::Play(AudioDevice& inDevice) {
    SDL_QueueAudio(inDevice.GetId(), mAudioBuffer, mWaveLength);
    inDevice.Play();
}

void Wav::Pause(AudioDevice& inDevice) { inDevice.Pause(); }

Wav::Wav(AudioDevice& inDevice, sv inFileName) { Build(inDevice, inFileName); }

up<Wav> Wav::Create(AudioDevice& inDevice, sv inFileName) {
    auto Obj = mu<Wav>();
    Obj->Build(inDevice, inFileName);
    return mv(Obj);
}