#include "PainterParameter_base.hpp"
#include "PainterParameter.hpp"

using namespace ksai;
using namespace Jkr;

std::mutex Jkr::PainterParameterBase::mMutex;

void Jkr::PainterParameterBase::SetupStorageBuffer(Up<StorageBufferType>& inStorageBuffer,
                                                   vk::DeviceSize inDeviceSize) {
    inStorageBuffer = MakeUp<StorageBufferType>(mInstance.GetVMA(),
                                                mInstance.GetDevice(),
                                                inDeviceSize,
                                                ksai::BufferContext::Storage,
                                                ksai::MemoryType::DeviceLocal);
}

void Jkr::PainterParameterBase::SetupUniformBuffer(Up<UniformBufferType>& inUniformBuffer,
                                                   vk::DeviceSize inDeviceSize,
                                                   void** inMappedMemoryRegion) {
    inUniformBuffer = MakeUp<UniformBufferType>(mInstance.GetVMA(),
                                                mInstance.GetDevice(),
                                                inDeviceSize,
                                                ksai::BufferContext::Uniform,
                                                ksai::MemoryType::HostVisibleAndCoherenet);
    inUniformBuffer->MapMemoryRegion(inMappedMemoryRegion);
}

void Jkr::PainterParameterBase::SetupStorageBufferCoherent(
     Up<StorageBufferTypeCoherent>& inStorageBuffer,
     vk::DeviceSize inDeviceSize,
     void** inMappedMemoryRegion) {
    inStorageBuffer = MakeUp<StorageBufferTypeCoherent>(mInstance.GetVMA(),
                                                        mInstance.GetDevice(),
                                                        inDeviceSize,
                                                        ksai::BufferContext::Storage,
                                                        ksai::MemoryType::HostVisibleAndCoherenet);
    inStorageBuffer->MapMemoryRegion(inMappedMemoryRegion);
}

// using StorageImageType          = VulkanImageVMA<ImageContext::Storage>;
// using UniformImageType          = VulkanImageVMA<ImageContext::Default>;
// using SkyboxImageType           = VulkanImageVMA<ImageContext::CubeCompatible>;
// using DepthImageType            = VulkanImageVMA<ImageContext::DepthImage>;

// TODO Replace Verbosity with
void Jkr::PainterParameterBase::SetupStorageImage(Up<VulkanSampler>& inStorageImageSampler,
                                                  Up<StorageImageType>& inStorageImage,
                                                  uint32_t inWidth,
                                                  uint32_t inHeight) {
    inStorageImage = MakeUp<StorageImageType>(
         mInstance.GetVMA(), mInstance.GetDevice(), inWidth, inHeight, ImageContext::Storage);
    inStorageImageSampler = MakeUp<VulkanSampler>(mInstance.GetDevice());
}

void Jkr::PainterParameterBase::SetupDepthImage(Up<VulkanSampler>& inDepthImageSampler,
                                                Up<DepthImageType>& inDepthImage,
                                                uint32_t inWidth,
                                                uint32_t inHeight) {
    inDepthImage = MakeUp<DepthImageType>(
         mInstance.GetVMA(), mInstance.GetDevice(), inWidth, inHeight, ImageContext::DepthImage);
    inDepthImageSampler = MakeUp<VulkanSampler>(mInstance.GetDevice(), ImageContext::DepthImage);
}

void Jkr::PainterParameterBase::SetupUniformImage(Up<VulkanSampler>& inUniformImageSampler,
                                                  Up<UniformImageType>& inUniformImage,
                                                  const std::string_view inFileName) {
    int Width;
    int Height;
    int Channels;
    void* data     = stbi_load(inFileName.data(), &Width, &Height, &Channels, STBI_rgb_alpha);
    inUniformImage = MakeUp<DepthImageType>(
         mInstance.GetVMA(), mInstance.GetDevice(), Width, Height, ImageContext::Default);
    inUniformImageSampler = MakeUp<VulkanSampler>(mInstance.GetDevice());

    std::scoped_lock<std::mutex> Lock(mMutex);

    //     VulkanCommandPool Pool(mInstance.GetDevice(),
    //     mInstance.GetGraphicsQueue().GetQueueContext()); VulkanCommandBuffer
    //     Cmd(mInstance.GetDevice(), Pool);
    //     Remove this

    inUniformImage->SubmitImmediateCmdCopyFromData(mInstance.GetGraphicsQueue(),
                                                   mInstance.GetUtilCommandBuffer(),
                                                   mInstance.GetDevice(),
                                                   mInstance.GetUtilCommandBufferFence(),
                                                   &data,
                                                   4 * Width * Height);
    mInstance.GetDevice().Wait();
    stbi_image_free(data);
}

void Jkr::PainterParameterBase::SetupUniformImage(Up<VulkanSampler>& inUniformImageSampler,
                                                  Up<UniformImageType>& inUniformImage,
                                                  void** inData,
                                                  uint32_t inWidth,
                                                  uint32_t inHeight,
                                                  uint32_t inChannelCount) {
    inUniformImage        = MakeUp<VulkanImageVMA>(mInstance.GetVMA(),
                                            mInstance.GetDevice(),
                                            inWidth,
                                            inHeight,
                                            ksai::ImageContext::Default,
                                            inChannelCount);
    inUniformImageSampler = MakeUp<VulkanSampler>(mInstance.GetDevice());

    std::scoped_lock<std::mutex> Lock(mMutex);
    //     VulkanCommandPool Pool(mInstance.GetDevice(),
    //     mInstance.GetGraphicsQueue().GetQueueContext()); VulkanCommandBuffer
    //     Cmd(mInstance.GetDevice(), Pool); VulkanFence Fence(mInstance.GetDevice());

    inUniformImage->SubmitImmediateCmdCopyFromData(mInstance.GetGraphicsQueue(),
                                                   mInstance.GetUtilCommandBuffer(),
                                                   mInstance.GetDevice(),
                                                   mInstance.GetUtilCommandBufferFence(),
                                                   inData,
                                                   inWidth * inHeight * inChannelCount);
    mInstance.GetDevice().Wait();
}

void Jkr::PainterParameterBase::SetupUniformImage(Up<VulkanSampler>& inUniformImageSampler,
                                                  Up<UniformImageType>& inUniformImage,
                                                  std::vector<s> inFileNames) {
    SetupImage(inUniformImageSampler, inUniformImage, inFileNames, ImageContext::Default);
}

void Jkr::PainterParameterBase::SetupSkyboxImage(Up<VulkanSampler>& inUniformImageSampler,
                                                 Up<SkyboxImageType>& inUniformImage,
                                                 std::vector<s> inFileNames) {
    SetupImage(inUniformImageSampler, inUniformImage, inFileNames, ImageContext::CubeCompatible);
}

void PainterParameterBase::SetupImage(Up<VulkanSampler>& inUniformImageSampler,
                                      Up<VulkanImageVMA>& inUniformImage,
                                      std::vector<s> inFileNames,
                                      ImageContext inImageContext) {
    int Width;
    int Height;
    int Channels;

    std::vector<void*> Datas;
    Datas.reserve(inFileNames.size());
    std::vector<void**> Datas_ptr;
    Datas_ptr.reserve(inFileNames.size());
    for (int i = 0; auto& u : inFileNames) {
        Datas.push_back(stbi_load(u.data(), &Width, &Height, &Channels, STBI_rgb_alpha));
        Datas_ptr.push_back(&Datas[i]);
        i++;
    }

    // VulkanCommandPool Pool(mInstance.GetDevice(),
    // mInstance.GetGraphicsQueue().GetQueueContext()); VulkanCommandBuffer
    // Cmd(mInstance.GetDevice(), Pool); VulkanFence Fence(mInstance.GetDevice()); // TODO Remove
    // this

    std::scoped_lock<std::mutex> Lock(mMutex);

    inUniformImage        = MakeUp<VulkanImageVMA>(mInstance.GetVMA(),
                                            mInstance.GetDevice(),
                                            Width,
                                            Height,
                                            inImageContext,
                                            4,
                                            inFileNames.size());
    inUniformImageSampler = MakeUp<VulkanSampler>(mInstance.GetDevice());
    inUniformImage->SubmitImmediateCmdCopyFromData(mInstance.GetGraphicsQueue(),
                                                   mInstance.GetUtilCommandBuffer(),
                                                   mInstance.GetDevice(),
                                                   mInstance.GetUtilCommandBufferFence(),
                                                   Channels * Height * Width,
                                                   Datas_ptr);

    for (void* u : Datas) {
        stbi_image_free(u);
    }
}