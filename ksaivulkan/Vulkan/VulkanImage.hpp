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
    GETTER& GetImagePropertyRef() { return mImageProperties; }

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
    void CmdCopyImageFromImageAfterStage(const VulkanCommandBuffer& inCmdBuffer,
                                         const VulkanDevice& inDevice,
                                         VulkanImageBase& inImage,
                                         vk::PipelineStageFlags inAfterStage,
                                         vk::AccessFlags inAfterStageAccessFlags,
                                         int inWidth                 = -1,
                                         int inHeight                = -1,
                                         int inFromMipLevel          = 0,
                                         int inFromBaseArrayLayer    = 0,
                                         int inToMipLevel            = 0,
                                         int inToBaseArrayLayer      = 0,
                                         int inArrayLayersToBeCopied = 1);
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

class VulkanImage : public VulkanImageBase {
    public:
    VulkanImage& operator=(VulkanImage&& Other) noexcept {
        ExplicitDestroy();
        MoveMembers(Other);
        return *this;
    }
    void MoveMembers(ksai::VulkanImage& Other);
    ~VulkanImage() { ExplicitDestroy(); }
    VulkanImage(const VulkanDevice& inDevice, ImageContext inImageContext);
    VulkanImage(const VulkanDevice& inDevice, ui inWidth, ui inHeight, ImageContext inImageContext);
    VulkanImage(const VulkanDevice& inDevice,
                const VulkanSurface& inSurface,
                ui inMSAASamples            = 1,
                ImageContext inImageContext = ImageContext::Default);
    VulkanImage(const VulkanDevice& inDevice,
                const VulkanSurface& inSurface,
                const vk::Image& inImage,
                vk::ImageView& inImageView,
                ImageContext inImageContext);

    private:
    void ExplicitDestroy();

    private:
    vk::DeviceMemory mDeviceMemory;
};

class VulkanImageExternalHandled : public VulkanImageBase {
    public:
    VulkanImageExternalHandled(const VulkanDevice& inDevice,
                               const VulkanSurface& inSurface,
                               vk::Image inImage,
                               vk::ImageView inImageView);
    ~VulkanImageExternalHandled() = default;
};

} // namespace ksai
