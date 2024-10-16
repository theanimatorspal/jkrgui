#pragma once

#ifndef ANDROID
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#else
#include <SDL.h>
#endif

#include "VulkanInstance.hpp"
#include "VulkanPhysicalDevice.hpp"

namespace ksai {
class VulkanSurface {
    public:
    struct CreateInfo {
        VulkanInstance *mInstance;
        SDL_Window *mWindow;
    };

    VulkanSurface() = default;
    ~VulkanSurface();
    VulkanSurface(VulkanSurface &other)             = delete;
    VulkanSurface &operator=(VulkanSurface &other)  = delete;
    VulkanSurface(VulkanSurface &&other)            = default;
    VulkanSurface &operator=(VulkanSurface &&other) = default;
    operator vk::SurfaceKHR() const { return mSurface; }

    void Init(CreateInfo inCreateInfo);
    void Destroy();

    VulkanSurface(VulkanInstance &inInstance, SDL_Window *inWindow);
    VulkanSurface &
    ProcessCurrentSurfaceConditions(VulkanPhysicalDevice &inPhysicalDevice,
                                    vk::PresentModeKHR inMode = vk::PresentModeKHR::eFifo);
    VulkanSurface &ProcessCurrentSurfaceExtents(VulkanPhysicalDevice &inPhysicalDevice);
    GETTER &GetSurfaceHandle() const { return mSurface; }
    GETTER &GetWindowHandle() const { return mWindow; }
    GETTER &GetSurfaceCapabilities() const { return mSurfaceCapabilities; }
    GETTER &GetSurfaceImageFormat() const { return mSurfaceImageFormat; }
    GETTER &GetExtent() const { return mExtent; }
    GETTER &GetPresentMode() const { return mPresentMode; }
    GETTER &GetPreTransform() const { return mPreTransform; }
    GETTER &GetCompositeAlpha() const { return mCompositeAlpha; }
    GETTER GetColorSpace() const { return mColorSpace; }

    private:
    vk::Instance *mInstance = nullptr;
    vk::SurfaceKHR mSurface = nullptr;
    SDL_Window *mWindow     = nullptr;
    vk::Format mSurfaceImageFormat;
    vk::ColorSpaceKHR mColorSpace;
    vk::SurfaceCapabilitiesKHR mSurfaceCapabilities;
    vk::Extent2D mExtent;
    vk::PresentModeKHR mPresentMode;
    vk::SurfaceTransformFlagBitsKHR mPreTransform;
    vk::CompositeAlphaFlagBitsKHR mCompositeAlpha;
    bool mInitialized = false;
};

} // namespace ksai
