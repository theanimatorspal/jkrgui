#include "VulkanSwapChain.hpp"
#include "VulkanCommandBuffer.hpp"
#include "VulkanBuffer.hpp"
#include <limits>
using namespace ksai;
#ifdef max
#undef max
#endif

ksai::VulkanSwapChainBase::VulkanSwapChainBase(const VulkanDevice& inDevice)
    : mDevice(inDevice.GetDeviceHandle())
{
}

ksai::VulkanSwapChainBase::~VulkanSwapChainBase()
{
    ExplicitlyDestroyOldSwapChain();
    ExplicitlyDestroy();
}

void ksai::VulkanSwapChainBase::ExplicitlyDestroy()
{
    if (mSwapChain) {
        mDevice.waitIdle();
        for (auto& u : mSwapChainImageViews) {
            mDevice.destroyImageView(u);
        }
        mDevice.destroy(mSwapChain);
        mSwapChainImageViews.clear();
        mSwapChainImages.clear();
        mSwapChain = nullptr;
    }
}

void ksai::VulkanSwapChainBase::ExplicitlyDestroyOldSwapChain()
{
    if (mOldSwapChain) {
        mDevice.waitIdle();
        for (auto& u : mOldSwapChainImageViews) {
            mDevice.destroyImageView(u);
        }
        mDevice.destroy(mOldSwapChain);
        mOldSwapChainImages.clear();
        mOldSwapChainImageViews.clear();
        mOldSwapChain = nullptr;
    }
}

std::pair<uint32_t, uint32_t> ksai::VulkanSwapChainBase::AcquireNextImage(const VulkanSemaphore& inSemapore)
{
    vk::ResultValue<uint32_t> Result = mDevice.acquireNextImageKHR(mSwapChain, std::numeric_limits<uint32_t>::max(), inSemapore.GetSemaphoreHandle());
    return std::pair<uint32_t, uint32_t>(static_cast<uint32_t>(Result.result), static_cast<uint32_t>(Result.value));
}

