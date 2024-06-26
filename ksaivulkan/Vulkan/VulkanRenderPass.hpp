#pragma once
#include "VulkanDevice.hpp"
#include "VulkanImage.hpp"
#include "VulkanSurface.hpp"
namespace ksai {

enum class RenderPassContext { Default, MSAA, Shadow, SingleColorAttachment };

class VulkanRenderPassBase {
    public:
    operator vk::RenderPass() const { return mRenderPass; }
    GETTER& GetRenderPassHandle() const { return mRenderPass; }

    protected:
    vk::RenderPass mRenderPass;
};

template <RenderPassContext inRenderPassContext>
class VulkanRenderPass : public VulkanRenderPassBase {
    public:
    VulkanRenderPass(const VulkanDevice& inDevice) : mDevice(inDevice.GetDeviceHandle()) {}
    VulkanRenderPass(const VulkanDevice& inDevice,
                     const VulkanSurface& inSurface,
                     const VulkanImage& inDepthImage);
    VulkanRenderPass(const VulkanDevice& inDevice, VulkanImageBase& inDepthImage);
    VulkanRenderPass(const VulkanDevice& inDevice,
                     const VulkanSurface& inSurface,
                     const VulkanImage& inColorImageTarget,
                     const VulkanImage& inDepthImage,
                     vk::SampleCountFlagBits inMSAASamples);
    ~VulkanRenderPass() { mDevice.destroyRenderPass(mRenderPass); }

    private:
    const vk::Device& mDevice;
};

} // namespace ksai
