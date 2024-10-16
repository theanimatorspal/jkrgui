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
    VulkanRenderPass()                                   = default;
    VulkanRenderPass(VulkanRenderPass &other)            = delete;
    VulkanRenderPass &operator=(VulkanRenderPass &other) = delete;
    VulkanRenderPass(VulkanRenderPass &&other)           = default;
    VulkanRenderPass &operator=(VulkanRenderPass &&other) {
        mDevice           = std::move(other.mDevice);
        mRenderPass       = std::move(other.mRenderPass);
        other.mRenderPass = nullptr;
        other.mDevice     = nullptr;
        return *this;
    }

    VulkanRenderPass(VulkanDevice &inDevice) : mDevice(&inDevice.GetDeviceHandle()) {}
    VulkanRenderPass(VulkanDevice &inDevice,
                     VulkanSurface &inSurface, /// @todo change this as below
                     VulkanImage &inDepthImage);
    VulkanRenderPass(VulkanDevice &inDevice, VulkanImageBase &inDepthImage);
    VulkanRenderPass(VulkanDevice &inDevice,
                     vk::Format inImageFormat,
                     VulkanImage &inColorImageTarget,
                     VulkanImage &inDepthImage,
                     vk::SampleCountFlagBits inMSAASamples);
    VulkanRenderPass(VulkanDevice &inDevice,
                     VulkanImageBase &inPositionImage,
                     VulkanImageBase &inNormalImage,
                     VulkanImageBase &inAlbedoImage,
                     VulkanImageBase &inDepthImage);
    ~VulkanRenderPass() {
        if (mRenderPass) {
            mDevice->destroyRenderPass(mRenderPass);
        }
    }

    private:
    vk::Device *mDevice = nullptr;
};

} // namespace ksai
