#include "VulkanQueueContext.hpp"
#include "VulkanCommandBuffer.hpp"
#include "VulkanFence.hpp"
#include "VulkanSemaphore.hpp"
class VulkanSemaphore;
class VulkanFence;
class VulkanCommandBuffer;
using namespace ksai;

VulkanQueueContext::VulkanQueueContext(VulkanInstance &inInstance, VulkanPhysicalDevice &inDevice)
    : mPhysicalDevice(inDevice.GetPhysicalDeviceHandle()) {
    SDL_Window *Window =
         SDL_CreateWindow("Test", 0, 0, 100, 100, SDL_WINDOW_HIDDEN | SDL_WINDOW_VULKAN);
    VulkanSurface Surface(inInstance, Window);
    auto SurfaceHandle     = Surface.GetSurfaceHandle();
    mQueueFamilyProperties = mPhysicalDevice.getQueueFamilyProperties();
    auto PropertyIterator  = std::find_if(mQueueFamilyProperties.begin(),
                                         mQueueFamilyProperties.end(),
                                         [](vk::QueueFamilyProperties const &qfp) {
                                             return (qfp.queueFlags & vk::QueueFlagBits::eGraphics);
                                         });

    mGraphicsQueueIndex    = std::distance(mQueueFamilyProperties.begin(), PropertyIterator);
    assert(mGraphicsQueueIndex < mQueueFamilyProperties.size());

    size_t presentQueueFamilyIndex =
         mPhysicalDevice.getSurfaceSupportKHR(mGraphicsQueueIndex, SurfaceHandle)
              ? mGraphicsQueueIndex
              : mQueueFamilyProperties.size();
    mPresentQueueIndex = presentQueueFamilyIndex;

    if (presentQueueFamilyIndex == mQueueFamilyProperties.size()) {
        for (int i = 0; i < mQueueFamilyProperties.size(); i++) {
            bool isQueueSupportedForPresentAndGraphics =
                 (mQueueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics) &&
                 mPhysicalDevice.getSurfaceSupportKHR(mGraphicsQueueIndex, SurfaceHandle);
            if (isQueueSupportedForPresentAndGraphics) {
                mGraphicsQueueIndex = i;
                mPresentQueueIndex  = i;
                break;
            }
        }

        bool isPresentQueueDifferent = (mPresentQueueIndex == mQueueFamilyProperties.size());
        if (isPresentQueueDifferent) {
            for (int i = 0; i < mQueueFamilyProperties.size(); i++) {
                bool SupportsPresentation = mPhysicalDevice.getSurfaceSupportKHR(i, SurfaceHandle);
                if (SupportsPresentation) {
                    mPresentQueueIndex = i;
                    break;
                }
            }
        }
    }
    bool NoQueueForGraphicsOrPresent = (mGraphicsQueueIndex == mQueueFamilyProperties.size()) ||
                                       (mPresentQueueIndex == mQueueFamilyProperties.size());
    ksai_print("GraphicsQueue Index:");
    ksai_print(std::to_string(mGraphicsQueueIndex).c_str());
    ksai_print("PresentQueueIndex:");
    ksai_print(std::to_string(mPresentQueueIndex).c_str());

    if (NoQueueForGraphicsOrPresent) {
        ksai_print("No Queue For Graphics Or Present");
        throw std::runtime_error("Error : Count not find a suitable queue for graphics or present");
    }
    SDL_DestroyWindow(Window);
}

VulkanQueueContext::~VulkanQueueContext() {}
