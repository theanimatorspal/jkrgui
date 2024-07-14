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

template <size_t NoOfAttachements = 2, typename... T> class VulkanFrameBuffer : public VulkanFrameBufferBase {
    public:
    VulkanFrameBuffer() = default;
    ~VulkanFrameBuffer() {
        if (mInitialized) {
            Destroy();
        }
    }
    VulkanFrameBuffer(const VulkanFrameBuffer &other)            = delete;
    VulkanFrameBuffer &operator=(const VulkanFrameBuffer &other) = delete;
    VulkanFrameBuffer(VulkanFrameBuffer &&other)                 = default;
    VulkanFrameBuffer &operator=(VulkanFrameBuffer &&Other) {
        ExplicitlyDestroy();
        mBuffer       = std::move(Other.mBuffer);
        mExtent       = std::move(Other.mExtent);
        Other.mBuffer = nullptr;
        return *this;
    }

    /**
     * @brief Can accept VulkanImage, VulkanImageVMA or vk::ImageView
     *
     *
     * Just pass the specified type starting from the third parameter,
     * creating frame buffers for multiple layers is supported with
     * vk::ImageView being supported
     */
    void Init(const VulkanDevice &inDevice, const VulkanRenderPassBase &inRenderPass, const T &...inT) {
        mDevice = &inDevice.GetDeviceHandle();
        Attachments.reserve(NoOfAttachements);
        (
             [&] {
                 if constexpr (Has_GetImageViewHandle<T>) {
                     Attachments.push_back(inT.GetImageViewHandle());
                     mExtent = inT.GetImageExtent();
                 } else {
                     Attachments.push_back(inT);
                 }
             }(),
             ...);
        auto FrameBufferCreateInfo = vk::FramebufferCreateInfo(
             vk::FramebufferCreateFlags(), inRenderPass.GetRenderPassHandle(), Attachments, mExtent.width, mExtent.height, 1);
        mBuffer      = inDevice.GetDeviceHandle().createFramebuffer(FrameBufferCreateInfo);
        mInitialized = true;
    }
    void Destroy() {
        ExplicitlyDestroy();
        mInitialized = false;
    }
    VulkanFrameBuffer(const VulkanDevice &inDevice, const VulkanRenderPassBase &inRenderPass, const T &...inT) {
        Init(inDevice, inRenderPass, inT...);
    }
    void ExplicitlyDestroy() {
        if (mBuffer) {
            mDevice->waitIdle();
            mDevice->destroyFramebuffer(mBuffer);
        }
        mBuffer = nullptr;
    }

    private:
    const vk::Device *mDevice;
    std::vector<vk::ImageView> Attachments;
    vk::Extent2D mExtent;
    bool mInitialized = false;
};

} // namespace ksai
