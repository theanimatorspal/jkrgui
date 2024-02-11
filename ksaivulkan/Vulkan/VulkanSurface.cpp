#include "VulkanSurface.hpp"
#ifdef ANDROID
#include <SDL_syswm.h>
#include <SDL_vulkan.h>
#else
#include <SDL2/SDL_vulkan.h>
#endif
#include <cglm/cglm.h>
#include <iostream>
using namespace ksai;
#undef max

VulkanSurface::VulkanSurface(const VulkanInstance& inInstance, SDL_Window* inWindow)
    : mInstance(inInstance.GetInstanceHandle())
    , mWindow(inWindow)
{
    SDL_bool Error = SDL_Vulkan_CreateSurface(inWindow, inInstance.GetInstanceHandle(), (VkSurfaceKHR*)&mSurface);
    if (Error == SDL_FALSE) {
        ksai_print("Surface Creation Error");
        std::cout << "Error Surface Create\n";
    }
}

VulkanSurface::~VulkanSurface()
{
    mInstance.destroySurfaceKHR(mSurface);
}

VulkanSurface& VulkanSurface::ProcessCurrentSurfaceConditions(const VulkanPhysicalDevice& inPhysicalDevice)
{
    ProcessCurrentSurfaceExtents(inPhysicalDevice);
    mPresentMode = vk::PresentModeKHR::eFifo; // Guaranteed by the spec to be supported
    //mPresentMode = vk::PresentModeKHR::eImmediate;
    mPreTransform = (mSurfaceCapabilities.supportedTransforms & vk::SurfaceTransformFlagBitsKHR::eIdentity)
        ? vk::SurfaceTransformFlagBitsKHR::eIdentity
        : mSurfaceCapabilities.currentTransform;

    mCompositeAlpha = (mSurfaceCapabilities.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::ePreMultiplied) ? vk::CompositeAlphaFlagBitsKHR::ePreMultiplied
        : (mSurfaceCapabilities.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::ePostMultiplied)            ? vk::CompositeAlphaFlagBitsKHR::ePostMultiplied
        : (mSurfaceCapabilities.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::eInherit)                   ? vk::CompositeAlphaFlagBitsKHR::eInherit
                                                                                                                     : vk::CompositeAlphaFlagBitsKHR::eOpaque;
    return *this;
}

VulkanSurface& VulkanSurface::ProcessCurrentSurfaceExtents(const VulkanPhysicalDevice& inPhysicalDevice)
{
    const auto& mPhysicalDevice = inPhysicalDevice.GetPhysicalDeviceHandle();
    std::vector<vk::SurfaceFormatKHR> formats = mPhysicalDevice.getSurfaceFormatsKHR(mSurface);
    assert(!formats.empty());

    mSurfaceImageFormat = (formats[0].format == vk::Format::eUndefined) ? vk::Format::eR8G8B8A8Unorm : formats[0].format;
    // TODO Make this better
    mSurfaceImageFormat = vk::Format::eR8G8B8A8Unorm;

    mSurfaceCapabilities = mPhysicalDevice.getSurfaceCapabilitiesKHR(mSurface);
    bool surfaceSizeUndefined = mSurfaceCapabilities.currentExtent.width == std::numeric_limits<uint32_t>::max();

    if (surfaceSizeUndefined) {
        int width, height;
        SDL_GetWindowSize(const_cast<SDL_Window*>(mWindow), &width, &height);
        mExtent.width = std::clamp<decltype(width)>(width, mSurfaceCapabilities.minImageExtent.width, mSurfaceCapabilities.maxImageExtent.width);
        mExtent.height = std::clamp<decltype(height)>(height, mSurfaceCapabilities.minImageExtent.height, mSurfaceCapabilities.maxImageExtent.height);
    } else {
        mExtent = mSurfaceCapabilities.currentExtent;
    }
    return *this;
}
