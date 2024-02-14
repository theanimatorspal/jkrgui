#include "AudioDevice.hpp"

Jkr::Audio::AudioDevice::AudioDevice(uint32_t inFrequency, AudioFormat inFormat, uint32_t inChannels, uint32_t inSamples)
{
	mAudioSpecDesired.freq = inFrequency;
	mAudioSpecDesired.format = inFormat;
	mAudioSpecDesired.channels = 2;
	mAudioSpecDesired.samples = 512;
	mAudioSpecDesired.callback = NULL;
	mAudioSpecDesired.userdata = (void*)&mUserData;
	mAudioDeviceId = SDL_OpenAudioDevice(NULL, 0, &mAudioSpecDesired, &mAudioSpecProvided, SDL_AUDIO_ALLOW_FORMAT_CHANGE);
}

Jkr::Audio::AudioDevice::~AudioDevice()
{
}

