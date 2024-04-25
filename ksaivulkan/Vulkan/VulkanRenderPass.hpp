#pragma once
#include "VulkanDevice.hpp"
#include "VulkanImage.hpp"
#include "VulkanSurface.hpp"
namespace ksai {

enum class RenderPassContext { Default, MSAA, Shadow };

class VulkanRenderPassBase {
    public:
    GETTER& GetRenderPassHandle() const { return mRenderPass; }

    protected:
    vk::RenderPass mRenderPass;
};
} // namespace ksai

namespace ksai {
template <RenderPassContext inRenderPassContext>
class VulkanRenderPass : public VulkanRenderPassBase {
    public:
    VulkanRenderPass(const VulkanDevice& inDevice) : mDevice(inDevice.GetDeviceHandle()) {}
    VulkanRenderPass(const VulkanDevice& inDevice,
                     const VulkanSurface& inSurface,
                     const VulkanImage<ImageContext::DepthImage>& inDepthImage);
    VulkanRenderPass(const VulkanDevice& inDevice, const VulkanImageBase& inDepthImage);
    VulkanRenderPass(const VulkanDevice& inDevice,
                     const VulkanSurface& inSurface,
                     const VulkanImage<ImageContext::ColorAttach>& inColorImageTarget,
                     const VulkanImage<ImageContext::DepthImage>& inDepthImage,
                     vk::SampleCountFlagBits inMSAASamples);
    ~VulkanRenderPass() { mDevice.destroyRenderPass(mRenderPass); }

    private:
    const vk::Device& mDevice;
};

} // namespace ksai
