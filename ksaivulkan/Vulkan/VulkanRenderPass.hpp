#pragma once
#include "VulkanDevice.hpp"
#include "VulkanImage.hpp"
#include "VulkanSurface.hpp"
namespace ksai {

enum class RenderPassContext {
    Default,
    OffScreen
};

class VulkanRenderPassBase {
public:
    GETTER& GetRenderPassHandle() const { return mRenderPass; }

protected:
    vk::RenderPass mRenderPass;
};
}

namespace ksai {
template <RenderPassContext inRenderPassContext>
class VulkanRenderPass : public VulkanRenderPassBase {
public:
    VulkanRenderPass(const VulkanDevice& inDevice)
        : mDevice(inDevice.GetDeviceHandle())
    {
    }
    VulkanRenderPass(const VulkanDevice& inDevice, const VulkanSurface& inSurface, const VulkanImage<ImageContext::DepthImage>& inDepthImage);
    ~VulkanRenderPass()
    {
        mDevice.destroyRenderPass(mRenderPass);
    }

private:
    const vk::Device& mDevice;
};

}
