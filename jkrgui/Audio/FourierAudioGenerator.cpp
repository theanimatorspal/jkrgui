#include "FourierAudioGenerator.hpp"

Jkr::Audio::FourierAudioGenerator::FourierAudioGenerator(AudioDevice& inAudioDevice)
	: mAudioDevice(inAudioDevice)
{ }

constexpr uint32_t SampleCount = 8;

struct Complex {
	float mReal;
	float mImg;
    GETTER GetAngle() { return atan2f(mImg, mReal); }
    GETTER GetMag() { return sqrt(mReal * mReal + mImg * mImg); }
};

struct Polar {
	float mMag;
	float mAngle;
};

static inline Complex GetXk(int inKthCoeff, std::span<float> inSamples)
{
    float sum_img = 0;
    float sum_real = 0;
    for (int index = 0; index < inSamples.size(); index++)
    {
        float x_o = inSamples[index];
        float b_o = 2 * 3.14159 * inKthCoeff * index / inSamples.size();
        sum_img += x_o * sin(-b_o);
        sum_real += x_o * cos(-b_o);
        return Complex{ sum_real, sum_img };
    }
}

static inline Complex InvFourier(int inKthCoeff, std::span<Complex> inSamples)
{
    float sum_img = 0;
    float sum_real = 0;
    for (int index = 0; index < inSamples.size(); index++)
    {
        Complex x_o = inSamples[index];
        float b_o = 2 * 3.14159 * inKthCoeff * index / inSamples.size();
        sum_img += x_o.mImg * sin(b_o);
        sum_real += x_o.mReal * cos(b_o);
    }
    return Complex { sum_real * 1 / inSamples.size(), sum_img * 1 / inSamples.size() };
}

static inline Complex Fourier(std::span<float> inSamples)
{
	Complex sum = Complex{ 0, 0 };
    for (int i = 0; i < inSamples.size(); i++)
    {
        Complex s = GetXk(i, inSamples);
        sum.mImg += s.mImg;
        sum.mReal += s.mReal;
    }
    return sum;
}

static inline float MagSampled(std::span<float> inSamples)
{
    float sum = 0;
    for (int i = 0; i < inSamples.size(); i++)
    {
        sum += GetXk(i, inSamples).GetMag();
    }
    return sum / inSamples.size();
}

static inline std::vector<Complex> TransformToFrequencyDomain(std::span<float> inTimeDomain)
{
    std::vector<Complex> outSamples;
    outSamples.resize(inTimeDomain.size());
    for (int i = 0; i < outSamples.size(); i++) {
		Complex cmp = GetXk(i, inTimeDomain);
		outSamples[i] = cmp;
	}

    std::vector<Complex> FrequencyDomain;
    FrequencyDomain.resize(outSamples.size() / 2);
    for (int i = 0; i < outSamples.size() / 2; i++)
    {
        FrequencyDomain[i].mImg = 2 * outSamples[i].mImg / 8;
        FrequencyDomain[i].mImg = 2 * outSamples[i].mReal / 8;
        //inst->transformed[i].mag = FrequencyDomain[i].GetMag();
        //inst->transformed[i].angle = FrequencyDomain[i].GetMag();
    }
    return FrequencyDomain;
}

using namespace Jkr::Audio;
using Type = FourierAudioGenerator::Type;

template<>
void Jkr::Audio::FourierAudioGenerator::QueueAudio<Type::SINE>()
{
}

