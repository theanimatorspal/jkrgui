#pragma once
#include "VulkanImageContext.hpp"
#include "VulkanQueue.hpp"

namespace ksai {
class VulkanCommandBuffer;
class VulkanImageBase {
    public:
    GETTER& GetImageFormat() const { return mImageProperties.mImageFormat; }
    GETTER& GetImageViewHandle() const { return mImageView; }
    GETTER& GetImageHandle() const { return mImage; }
    GETTER& GetImageExtent() const { return mImageProperties.mExtent; }
    GETTER GetInitialImageLayout() const { return mImageProperties.mInitialImageLayout; }
    GETTER GetAspect() const { return mImageProperties.mImageAspect; }
    GETTER GetImageProperty() const { return mImageProperties; }

    VulkanImageBase(const VulkanDevice& inDevice, bool inDestroyImageView = true);
    ~VulkanImageBase();
    void SubmitImmediateCmdCopyFromData(const VulkanQueue<QueueContext::Graphics>& inQueue,
                                        const VulkanCommandBuffer& inCmdBuffer,
                                        const VulkanDevice& inDevice,
                                        const VulkanFence& inFence,
                                        void** inData,
                                        vk::DeviceSize inSize);
    void SubmitImmediateCmdCopyFromData(const VulkanQueue<QueueContext::Graphics>& inQueue,
                                        const VulkanCommandBuffer& inCmdBuffer,
                                        const VulkanDevice& inDevice,
                                        const VulkanFence& inFence,
                                        vk::DeviceSize inSize,
                                        std::span<void**> inLayerImageDatas);
    void CmdCopyImageFromImageAfterStage(const VulkanQueue<QueueContext::Graphics>& inQueue,
                                         const VulkanCommandBuffer& inCmdBuffer,
                                         const VulkanDevice& inDevice,
                                         VulkanImageBase& inImage,
                                         vk::PipelineStageFlags inAfterStage,
                                         vk::AccessFlags inAfterStageAccessFlags);
    void CmdTransitionImageLayout(const VulkanCommandBuffer& inBuffer,
                                  vk::ImageLayout inOldImageLayout,
                                  vk::ImageLayout inNewImageLayout,
                                  vk::PipelineStageFlags inBeforeStage,
                                  vk::PipelineStageFlags inAfterStage,
                                  vk::AccessFlags inBeforeAccess,
                                  vk::AccessFlags inAfterAccess) const;

    public:
    void FillImageProperties(ImageContext inImageContext, uint32_t inNumSamples = 1);
    void GetMemoryTypeIndex(vk::MemoryPropertyFlagBits inFlag,
                            vk::Image inImage,
                            vk::DeviceSize& outSize,
                            ui& outIndex);
    void GetImageTiling(vk::Format inFormat,
                        vk::FormatFeatureFlagBits inFormatFeature,
                        vk::ImageTiling& outTiling);

    protected:
    void CreateImageAndBindMemory(vk::Image& inImage, vk::DeviceMemory& inDeviceMemory);
    void CreateImageView(vk::Image& inImage);

    protected:
    const vk::PhysicalDevice& mPhysicalDevice;
    const vk::Device& mDevice;
    vk::Image mImage         = nullptr;
    vk::ImageView mImageView = nullptr;
    ImageProperties mImageProperties;
    bool mDestroyImageView = true;
};
} // namespace ksai

namespace ksai {
template <ImageContext inImageContext> class VulkanImage : public VulkanImageBase {
    public:
    VulkanImage& operator=(VulkanImage&& Other) noexcept {
        ExplicitDestroy();
        MoveMembers(Other);
        return *this;
    }
    void MoveMembers(ksai::VulkanImage<inImageContext>& Other);
    VulkanImage();
    ~VulkanImage() { ExplicitDestroy(); }
    VulkanImage(const VulkanDevice& inDevice);
    VulkanImage(const VulkanDevice& inDevice, ui inWidth, ui inHeight);
    VulkanImage(const VulkanDevice& inDevice, const VulkanSurface& inSurface, ui inMSAASamples = 1);
    VulkanImage(const VulkanDevice& inDevice,
                const VulkanSurface& inSurface,
                const vk::Image& inImage,
                vk::ImageView& inImageView);

    private:
    void ExplicitDestroy();

    private:
    vk::DeviceMemory mDeviceMemory;
};
} // namespace ksai

namespace ksai {
// TODO Rethink This
template <> class VulkanImage<ImageContext::ExternalHandled> : public VulkanImageBase {
    public:
    VulkanImage(const VulkanDevice& inDevice,
                const VulkanSurface& inSurface,
                vk::Image inImage,
                vk::ImageView inImageView);
    ~VulkanImage() = default;
};
} // namespace ksai

namespace ksai {
template <ImageContext inImageContext>
inline void VulkanImage<inImageContext>::MoveMembers(ksai::VulkanImage<inImageContext>& Other) {
    mImage                   = std::move(Other.mImage);
    mImageView               = std::move(Other.mImageView);
    mDeviceMemory            = std::move(Other.mDeviceMemory);
    mImageProperties.mExtent = std::move(Other.mImageProperties.mExtent);
    Other.mImage             = nullptr;
    Other.mImageView         = nullptr;
    Other.mDeviceMemory      = nullptr;
}
template <ImageContext inImageContext> inline VulkanImage<inImageContext>::VulkanImage() {}

template <ImageContext inImageContext>
inline VulkanImage<inImageContext>::VulkanImage(const VulkanDevice& inDevice)
    : VulkanImageBase(inDevice) {
    FillImageProperties(inImageContext);
    CreateImageAndBindMemory(mImage, mDeviceMemory);
    CreateImageView(mImage);
}

template <ImageContext inImageContext>
inline VulkanImage<inImageContext>::VulkanImage(const VulkanDevice& inDevice,
                                                ui inWidth,
                                                ui inHeight)
    : VulkanImageBase(inDevice) {
    FillImageProperties(inImageContext);
    mImageProperties.mExtent.width  = inWidth;
    mImageProperties.mExtent.height = inHeight;
    CreateImageAndBindMemory(mImage, mDeviceMemory);
    CreateImageView(mImage);
}

template <ImageContext inImageContext>
inline VulkanImage<inImageContext>::VulkanImage(const VulkanDevice& inDevice,
                                                const VulkanSurface& inSurface,
                                                ui inMSAASamples)
    : VulkanImageBase(inDevice) {
    FillImageProperties(inImageContext, inMSAASamples);
    mImageProperties.mExtent = inSurface.GetExtent();
    CreateImageAndBindMemory(mImage, mDeviceMemory);
    CreateImageView(mImage);
}

template <ImageContext inImageContext>
inline VulkanImage<inImageContext>::VulkanImage(const VulkanDevice& inDevice,
                                                const VulkanSurface& inSurface,
                                                const vk::Image& inImage,
                                                vk::ImageView& inImageView)
    : VulkanImageBase(inDevice) {
    mImageProperties.mExtent = inSurface.GetExtent();
    mImage                   = inImage;
    mImageView               = inImageView;
}

template <ImageContext inImageContext> inline void VulkanImage<inImageContext>::ExplicitDestroy() {
    if (mImage) {
        mDevice.waitIdle();
        mDevice.destroyImage(mImage);
        mDevice.destroyImageView(mImageView);
        mDevice.freeMemory(mDeviceMemory);
        mImage        = nullptr;
        mImageView    = nullptr;
        mDeviceMemory = nullptr;
    }
}

} // namespace ksai

namespace ksai {
inline VulkanImage<ImageContext::ExternalHandled>::VulkanImage(const VulkanDevice& inDevice,
                                                               const VulkanSurface& inSurface,
                                                               vk::Image inImage,
                                                               vk::ImageView inImageView)
    : VulkanImageBase(inDevice, false) {
    mImageProperties.mExtent = inSurface.GetExtent();
    mImage                   = inImage;
    mImageView               = inImageView;
}
} // namespace ksai
