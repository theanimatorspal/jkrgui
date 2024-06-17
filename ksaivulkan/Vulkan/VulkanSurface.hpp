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
    operator vk::SurfaceKHR() const { return mSurface; }
    VulkanSurface(const VulkanInstance& inInstance, SDL_Window* inWindow);
    ~VulkanSurface();
    VulkanSurface&
    ProcessCurrentSurfaceConditions(const VulkanPhysicalDevice& inPhysicalDevice,
                                    vk::PresentModeKHR inMode = vk::PresentModeKHR::eFifo);
    VulkanSurface& ProcessCurrentSurfaceExtents(const VulkanPhysicalDevice& inPhysicalDevice);
    GETTER& GetSurfaceHandle() const { return mSurface; }
    GETTER& GetWindowHandle() const { return mWindow; }
    GETTER& GetSurfaceCapabilities() const { return mSurfaceCapabilities; }
    GETTER& GetSurfaceImageFormat() const { return mSurfaceImageFormat; }
    GETTER& GetExtent() const { return mExtent; }
    GETTER& GetPresentMode() const { return mPresentMode; }
    GETTER& GetPreTransform() const { return mPreTransform; }
    GETTER& GetCompositeAlpha() const { return mCompositeAlpha; }

    private:
    const vk::Instance& mInstance;
    vk::SurfaceKHR mSurface;
    SDL_Window* mWindow;
    vk::Format mSurfaceImageFormat;
    vk::SurfaceCapabilitiesKHR mSurfaceCapabilities;
    vk::Extent2D mExtent;
    vk::PresentModeKHR mPresentMode;
    vk::SurfaceTransformFlagBitsKHR mPreTransform;
    vk::CompositeAlphaFlagBitsKHR mCompositeAlpha;
};

} // namespace ksai
