#pragma once
#include "VulkanDevice.hpp"
#include "VulkanImage.hpp"
#include "VulkanSurface.hpp"
namespace ksai {

enum class RenderPassContext { Default, MSAA, Shadow, SingleColorAttachment, Deferred };

class VulkanRenderPassBase {
    public:
    operator vk::RenderPass() const { return mRenderPass; }
    GETTER &GetRenderPassHandle() const { return mRenderPass; }
    GETTER GetAttachmentCount() const { return mColorAttachmentCount; }

    protected:
    vk::RenderPass mRenderPass = nullptr;
    int mColorAttachmentCount  = 1;
};

template <RenderPassContext inRenderPassContext>
class VulkanRenderPass : public VulkanRenderPassBase {
    public:
    VulkanRenderPass()                                         = default;
    VulkanRenderPass(const VulkanRenderPass &other)            = delete;
    VulkanRenderPass &operator=(const VulkanRenderPass &other) = delete;
    VulkanRenderPass(VulkanRenderPass &&other)                 = default;
    VulkanRenderPass &operator=(VulkanRenderPass &&other) {
        mDevice           = std::move(other.mDevice);
        mRenderPass       = std::move(other.mRenderPass);
        other.mRenderPass = nullptr;
        other.mDevice     = nullptr;
        return *this;
    }

    VulkanRenderPass(const VulkanDevice &inDevice) : mDevice(&inDevice.GetDeviceHandle()) {}
    VulkanRenderPass(const VulkanDevice &inDevice,
                     const VulkanSurface &inSurface, /// @todo change this as below
                     const VulkanImage &inDepthImage);
    VulkanRenderPass(const VulkanDevice &inDevice, VulkanImageBase &inDepthImage);
    VulkanRenderPass(const VulkanDevice &inDevice,
                     const vk::Format inImageFormat,
                     const VulkanImage &inColorImageTarget,
                     const VulkanImage &inDepthImage,
                     vk::SampleCountFlagBits inMSAASamples);
    VulkanRenderPass(const VulkanDevice &inDevice,
                     const VulkanImageBase &inPositionImage,
                     const VulkanImageBase &inNormalImage,
                     const VulkanImageBase &inAlbedoImage,
                     const VulkanImageBase &inDepthImage);
    ~VulkanRenderPass() {
        if (mRenderPass) {
            mDevice->destroyRenderPass(mRenderPass);
        }
    }

    private:
    const vk::Device *mDevice = nullptr;
};

} // namespace ksai
