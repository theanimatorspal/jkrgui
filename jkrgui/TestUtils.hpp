#pragma once
#define SDL_MAIN_HANDLED
#include "ksaiSandBox/glslHelper.hpp"
#include <SDL2/SDL.h>
#include <chrono>
#include <iostream>
#include <memory>
#include <sstream>
#define GETTER inline auto

namespace Jkr {
class TestWindow {
public:
    TestWindow()
    {
        mWindow = SDL_CreateWindow("Hello", 0, 0, 1920, 1080, SDL_WINDOW_VULKAN | SDL_WINDOW_HIDDEN);
    }
    ~TestWindow()
    {
        SDL_DestroyWindow(mWindow);
    }

public:
    GETTER GetHandle() const { return mWindow; }

private:
    SDL_Window* mWindow;
};

}

namespace Jkr {
class ShaderCompiler {
public:
    ShaderCompiler(const std::string& inVertexShaderString,
        const std::string& inFragmentShaderString,
        std::vector<uint32_t>& outVertexShaderModule,
        std::vector<uint32_t>& outFragmentShaderModule)
    {
        bool success = SpirvHelper::GLSLtoSPV(vk::ShaderStageFlagBits::eVertex, inVertexShaderString.c_str(), outVertexShaderModule);
        if (!success)
            assert("VertexShader Failed" && false);
        success = SpirvHelper::GLSLtoSPV(vk::ShaderStageFlagBits::eFragment, inFragmentShaderString.c_str(), outFragmentShaderModule);
        if (!success)
            assert("FragmentShader failed" && false);
    }
    ShaderCompiler(
        const std::string& inComputeShaderString,
        std::vector<uint32_t>& outComputeShaderModule)
    {
        bool success = SpirvHelper::GLSLtoSPV(vk::ShaderStageFlagBits::eCompute, inComputeShaderString.c_str(), outComputeShaderModule);
        if (!success)
            assert("ComputeShader failed" && false);
    }
};
}

namespace Jkr {
class Timer {
private:
    // Type aliases to make accessing nested type easier
    using Clock = std::chrono::steady_clock;
    using Second = std::chrono::duration<double, std::ratio<1>>;

    std::chrono::time_point<Clock> m_beg { Clock::now() };

public:
    inline void reset()
    {
        m_beg = Clock::now();
    }

    inline void elapsed(const std::string inString) const
    {
        std::cout << "In Func: " << inString << " : " << std::chrono::duration_cast<Second>(Clock::now() - m_beg).count() << "s\n";
    }

    inline auto elapsed() const
    {
        return std::chrono::duration_cast<Second>(Clock::now() - m_beg).count();
    }
};
}

namespace Jkr::Test {
static Timer FrameTimer;
template <size_t NoOfSamples>
class FrameRateMeasure {
public:
    inline void StartFrame()
    {
        FrameTimer.reset();
    }
    inline void EndFrame()
    {
        mFrameRates[mCurrentIndex] = 1 / FrameTimer.elapsed();
        mCurrentIndex = (mCurrentIndex + 1) % mFrameRates.size();
    }
    inline float GetAvg()
    {
        if (mCurrentIndex == mFrameRates.size() - 1) {
            auto avg = 0.0f;
            for (auto i : mFrameRates) {
                avg += i;
            }
            mCurrentAvgFrameRate = avg / mFrameRates.size();
        }
        return mCurrentAvgFrameRate;
    }

private:
    int mCurrentIndex = 0;
    int mCurrentAvgFrameRate = 0;
    std::array<double, NoOfSamples> mFrameRates;
};
}

#define MakeUp std::make_unique
#define MakeSH std::make_shared

template <class T>
using Up = std::unique_ptr<T>;
template <class T>
using Sp = std::shared_ptr<T>;

template <typename T>
constexpr bool IsWithinRange(T inValue, T inLeft, T inRight)
{
    return (inLeft < inValue) and (inRight > inValue);
}
