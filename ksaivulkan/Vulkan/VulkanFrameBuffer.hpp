#pragma once
#include "VulkanRenderPass.hpp"

namespace ksai {
class VulkanFrameBufferBase {
    public:
    GETTER &GetFrameBufferHandle() const { return mBuffer; }

    protected:
    vk::Framebuffer mBuffer;
};

template <typename T>
concept Has_GetImageViewHandle = requires(T &t) { t.GetImageViewHandle(); };

class VulkanFrameBuffer : public VulkanFrameBufferBase {
    public:
    VulkanFrameBuffer() = default;
    ~VulkanFrameBuffer() {
        if (mInitialized) {
            Destroy();
        }
    }
    VulkanFrameBuffer(VulkanFrameBuffer &other)            = delete;
    VulkanFrameBuffer &operator=(VulkanFrameBuffer &other) = delete;
    VulkanFrameBuffer(VulkanFrameBuffer &&other)           = default;
    VulkanFrameBuffer &operator=(VulkanFrameBuffer &&Other);

    ///
    /// @brief Can accept VulkanImage, VulkanImageVMA or vk::ImageView
    ///
    /// for vk::ImageView, a vk::Extent object has to be passed with the ImageViews
    /// like Init(device, renderPass, extent2d, imageview1, imageview2);
    ///
    ///
    void Init(VulkanDevice &inDevice, VulkanRenderPassBase &inRenderPass, const auto &...inT) {
        mDevice = &inDevice.GetDeviceHandle();
        (
             [&] {
                 if constexpr (Has_GetImageViewHandle<decltype(inT)>) {
                     Attachments.push_back(inT.GetImageViewHandle());
                     mExtent = inT.GetImageExtent();
                 } else if constexpr (std::is_same_v<decltype(inT), vk::Extent2D>) {
                     mExtent = inT;
                 } else {
                     Attachments.push_back(inT);
                 }
             }(),
             ...);
        auto FrameBufferCreateInfo = vk::FramebufferCreateInfo(vk::FramebufferCreateFlags(),
                                                               inRenderPass.GetRenderPassHandle(),
                                                               Attachments,
                                                               mExtent.width,
                                                               mExtent.height,
                                                               1);
        mBuffer      = inDevice.GetDeviceHandle().createFramebuffer(FrameBufferCreateInfo);
        mInitialized = true;
    }

    void Destroy();
    VulkanFrameBuffer(VulkanDevice &inDevice,
                      VulkanRenderPassBase &inRenderPass,
                      const auto &...inT) {
        Init(inDevice, inRenderPass, inT...);
    }
    void ExplicitlyDestroy();

    private:
    vk::Device *mDevice;
    std::vector<vk::ImageView> Attachments;
    vk::Extent2D mExtent;
    bool mInitialized = false;
};

} // namespace ksai
