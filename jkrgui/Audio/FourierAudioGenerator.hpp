#include "AudioDevice.hpp"

namespace Jkr::Audio {
	class FourierAudioGenerator {
	public:
		enum class Type {
			SINE,
			COSINE
		};
		FourierAudioGenerator(AudioDevice& inAudioDevice);

		template<Type inType>
		void QueueAudio();
	private:
		void QueueAudio(std::span<uint32_t> inAudio) { SDL_QueueAudio(mAudioDevice.GetId(), static_cast<void*>(inAudio.data()), inAudio.size()); }

		AudioDevice& mAudioDevice;
		float mVolume = 0.1;
		float mFrequency = 200.0f;
		float mFourierOutput = 0.0f; // TODO Legacy ununderstanable code
		std::array<float, 7> mSvaraFrequency = { 256, 280, 312, 346, 384, 426, 480 };
	};
}
