#pragma once
#include "VulkanImageContext.hpp"
#include "VulkanQueue.hpp"
#include <vulkan/vulkan_raii.hpp>

namespace ksai {

class VulkanCommandBuffer;
class VulkanBufferVMA;
class VulkanImageBase {
    public:
    struct CreateInfo {
        VulkanDevice *mDevice;
        bool mDestroyImageView = true;
    };

    VulkanImageBase() = default;
    ~VulkanImageBase();
    VulkanImageBase(VulkanImageBase &other)             = delete;
    VulkanImageBase &operator=(VulkanImageBase &other)  = delete;
    VulkanImageBase(VulkanImageBase &&other)            = default;
    VulkanImageBase &operator=(VulkanImageBase &&other) = default;
    operator vk::Image() const { return mImage; }

    void Init(CreateInfo inCreateInfo);
    void Destroy();

    GETTER &GetImageFormat() const { return mImageProperties.mImageFormat; }
    GETTER &GetImageViewHandle() const { return mImageView; }
    GETTER &GetImageHandle() const { return mImage; }
    GETTER &GetImageExtent() const { return mImageProperties.mExtent; }
    GETTER GetInitialImageLayout() const { return mImageProperties.mInitialImageLayout; }
    GETTER GetAspect() const { return mImageProperties.mImageAspect; }
    GETTER GetImageProperty() const { return mImageProperties; }
    GETTER GetCurrentImageLayout() const { return mImageProperties.mCurrentImageLayout; }
    GETTER &GetCurrentImageLayoutRef() { return mImageProperties.mCurrentImageLayout; }
    GETTER &GetImagePropertyRef() { return mImageProperties; }

    void SetDebugUtilsName(s inS);

    VulkanImageBase(VulkanDevice &inDevice, bool inDestroyImageView = true);
    void SubmitImmediateCmdCopyFromDataWithStagingBuffer(
         VulkanQueue<QueueContext::Graphics> &inQueue,
         VulkanCommandBuffer &inCmdBuffer,
         VulkanDevice &inDevice,
         VulkanFence &inFence,
         void **inData,
         vk::DeviceSize inSize,
         VulkanBufferVMA &inStagingBuffer,
         vk::ImageLayout inImageLayout = vk::ImageLayout::eShaderReadOnlyOptimal,
         int inImageWidth              = -1,
         int inImageHeight             = -1,
         int inMipLevel                = 0,
         int inLayer                   = 0,
         int inLayersToBeCopied        = 1);
    void
    SubmitImmediateCmdCopyFromDataWithStagingBuffer(VulkanQueue<QueueContext::Graphics> &inQueue,
                                                    VulkanCommandBuffer &inCmdBuffer,
                                                    VulkanDevice &inDevice,
                                                    VulkanFence &inFence,
                                                    vk::DeviceSize inSize,
                                                    std::span<void **> inLayerImageDatas,
                                                    VulkanBufferVMA &inStagingBuffer);
    void CmdCopyImageFromImageAfterStage(
         VulkanCommandBuffer &inCmdBuffer,
         VulkanDevice &inDevice,
         VulkanImageBase &inImage,
         vk::PipelineStageFlags inAfterStage,
         vk::AccessFlags inAfterStageAccessFlags,
         int inWidth                               = -1,
         int inHeight                              = -1,
         int inFromMipLevel                        = 0,
         int inFromBaseArrayLayer                  = 0,
         int inToMipLevel                          = 0,
         int inToBaseArrayLayer                    = 0,
         int inArrayLayersToBeCopied               = 1,
         vk::ImageLayout inSrcImageLayoutTobeSetTo = vk::ImageLayout::eShaderReadOnlyOptimal,
         vk::ImageLayout inDstImageLayoutTobeSetTo = vk::ImageLayout::eShaderReadOnlyOptimal,
         opt<vk::ImageLayout> inSrcImageLayoutFrom = std::nullopt,
         opt<vk::ImageLayout> inDstImageLayoutFrom = std::nullopt);
    void CmdTransitionImageLayout(VulkanCommandBuffer &inBuffer,
                                  vk::ImageLayout inOldImageLayout,
                                  vk::ImageLayout inNewImageLayout,
                                  vk::PipelineStageFlags inBeforeStage,
                                  vk::PipelineStageFlags inAfterStage,
                                  vk::AccessFlags inBeforeAccess,
                                  vk::AccessFlags inAfterAccess);

    ///@warning Assumes the precision of each channel to be 1 byte
    std::vector<char>
    SubmitImmediateCmdGetImageToVector(VulkanQueue<QueueContext::Graphics> &inQueue,
                                       VulkanBufferVMA &inStagingBuffer,
                                       VulkanCommandBuffer &inBuffer,
                                       VulkanFence &inCommandBufferFence,
                                       vk::ImageLayout inImageLayout = vk::ImageLayout::eGeneral,
                                       int inImageWidth              = -1,
                                       int inImageHeight             = -1,
                                       int inMipLevel                = 0,
                                       int inLayer                   = 0,
                                       int inLayersToBeCopied        = 1);

    public:
    void FillImageProperties(ImageContext inImageContext, uint32_t inNumSamples = 1);
    void GetMemoryTypeIndex(vk::MemoryPropertyFlagBits inFlag,
                            vk::Image inImage,
                            vk::DeviceSize &outSize,
                            ui &outIndex);
    void GetImageTiling(vk::Format inFormat,
                        vk::FormatFeatureFlagBits inFormatFeature,
                        vk::ImageTiling &outTiling);

    void BuildImageViewsByLayers();
    GETTER &GetImageViews() { return mImageViews; }

    protected:
    void CreateImageAndBindMemory(vk::Image &inImage, vk::DeviceMemory &inDeviceMemory);
    void CreateImageView(vk::Image &inImage);

    protected:
    vk::PhysicalDevice *mPhysicalDevice;
    vk::Device *mDevice;
    VulkanDevice *mVulkanDevice;
    vk::Image mImage         = nullptr;
    vk::ImageView mImageView = nullptr;
    ImageProperties mImageProperties;
    bool mDestroyImageView                 = true;
    bool mInitialized                      = false;
    std::vector<vk::ImageView> mImageViews = {};
};

class VulkanImage : public VulkanImageBase {
    public:
    struct CreateInfo {
        VulkanDevice *mDevice      = nullptr;
        ImageContext mImageContext = ImageContext::Default;
        opt<ui> mWidth             = std::nullopt;
        opt<ui> mHeight            = std::nullopt;
        VulkanSurface *mSurface    = nullptr;
        ui mMSAASamples            = 1;
        vk::Image *inImage         = nullptr;
        vk::ImageView *inImageView = nullptr;
    };

    VulkanImage() = default;
    ~VulkanImage();
    VulkanImage(VulkanImage &other)            = delete;
    VulkanImage &operator=(VulkanImage &other) = delete;
    VulkanImage(VulkanImage &&other)           = default;
    VulkanImage &operator=(VulkanImage &&Other) noexcept {
        ExplicitDestroy();
        MoveMembers(Other);
        return *this;
    }
    void Init(CreateInfo inCreateInfo);
    void Destroy();

    void MoveMembers(ksai::VulkanImage &Other);
    VulkanImage(VulkanDevice &inDevice, ImageContext inImageContext);
    VulkanImage(VulkanDevice &inDevice, ui inWidth, ui inHeight, ImageContext inImageContext);
    VulkanImage(VulkanDevice &inDevice,
                VulkanSurface &inSurface,
                ui inMSAASamples            = 1,
                ImageContext inImageContext = ImageContext::Default);
    VulkanImage(VulkanDevice &inDevice,
                VulkanSurface &inSurface,
                vk::Image &inImage,
                vk::ImageView &inImageView,
                ImageContext inImageContext);

    private:
    void ExplicitDestroy();

    private:
    vk::DeviceMemory mDeviceMemory;
    bool mInitialized = false;
};

class VulkanImageExternalHandled : public VulkanImageBase {
    public:
    VulkanImageExternalHandled()                                              = default;
    ~VulkanImageExternalHandled()                                             = default;
    VulkanImageExternalHandled(VulkanImageExternalHandled &other)             = delete;
    VulkanImageExternalHandled &operator=(VulkanImageExternalHandled &other)  = delete;
    VulkanImageExternalHandled(VulkanImageExternalHandled &&other)            = default;
    VulkanImageExternalHandled &operator=(VulkanImageExternalHandled &&other) = default;

    VulkanImageExternalHandled(VulkanDevice &inDevice,
                               VulkanSurface &inSurface,
                               vk::Image inImage,
                               vk::ImageView inImageView);
};

} // namespace ksai
