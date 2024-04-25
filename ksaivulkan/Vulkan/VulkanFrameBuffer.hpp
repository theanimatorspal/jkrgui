#pragma once
#include "VulkanRenderPass.hpp"

namespace ksai {
class VulkanFrameBufferBase {
    public:
    GETTER& GetFrameBufferHandle() const { return mBuffer; }

    protected:
    vk::Framebuffer mBuffer;
};

template <size_t NoOfAttachements = 2, typename... T>
class VulkanFrameBuffer : public VulkanFrameBufferBase {
    public:
    VulkanFrameBuffer(const VulkanDevice& inDevice,
                      const VulkanRenderPassBase& inRenderPass,
                      const T&... inT);
    ~VulkanFrameBuffer();
    void ExplicitlyDestroy();
    VulkanFrameBuffer& operator=(VulkanFrameBuffer&& Other) {
        ExplicitlyDestroy();
        mBuffer       = std::move(Other.mBuffer);
        mExtent       = std::move(Other.mExtent);
        Other.mBuffer = nullptr;
        return *this;
    }

    private:
    const vk::Device& mDevice;
    std::vector<vk::ImageView> Attachments;
    vk::Extent2D mExtent;
};

template <size_t NoOfAttachements, typename... T>
inline VulkanFrameBuffer<NoOfAttachements, T...>::VulkanFrameBuffer(
     const VulkanDevice& inDevice, const VulkanRenderPassBase& inRenderPass, const T&... inT)
    : mDevice(inDevice.GetDeviceHandle()) {
    Attachments.reserve(NoOfAttachements);

    (
         [&] {
             Attachments.push_back(inT.GetImageViewHandle());
             mExtent = inT.GetImageExtent();
         }(),
         ...);

    auto FrameBufferCreateInfo = vk::FramebufferCreateInfo(vk::FramebufferCreateFlags(),
                                                           inRenderPass.GetRenderPassHandle(),
                                                           Attachments,
                                                           mExtent.width,
                                                           mExtent.height,
                                                           1);
    mBuffer = inDevice.GetDeviceHandle().createFramebuffer(FrameBufferCreateInfo);
}

template <size_t NoOfAttachements, typename... T>
inline VulkanFrameBuffer<NoOfAttachements, T...>::~VulkanFrameBuffer() {
    ExplicitlyDestroy();
}

template <size_t NoOfAttachements, typename... T>
inline void VulkanFrameBuffer<NoOfAttachements, T...>::ExplicitlyDestroy() {
    if (mBuffer) {
        mDevice.waitIdle();
        mDevice.destroyFramebuffer(mBuffer);
    }
    mBuffer = nullptr;
}
} // namespace ksai
