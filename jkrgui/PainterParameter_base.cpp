#include "PainterParameter_base.hpp"
#include "PainterParameter.hpp"

void Jkr::PainterParameterBase::Setup(Up<StorageBufferType>& inStorageBuffer,
                                      vk::DeviceSize inDeviceSize) {
    inStorageBuffer =
         MakeUp<StorageBufferType>(mInstance.GetVMA(), mInstance.GetDevice(), inDeviceSize);
}

void Jkr::PainterParameterBase::Setup(Up<UniformBufferType>& inUniformBuffer,
                                      vk::DeviceSize inDeviceSize,
                                      void** inMappedMemoryRegion) {
    inUniformBuffer =
         MakeUp<UniformBufferType>(mInstance.GetVMA(), mInstance.GetDevice(), inDeviceSize);
    inUniformBuffer->MapMemoryRegion(inMappedMemoryRegion);
}

void Jkr::PainterParameterBase::Setup(Up<StorageBufferTypeCoherent>& inStorageBuffer,
                                      vk::DeviceSize inDeviceSize,
                                      void** inMappedMemoryRegion) {
    inStorageBuffer =
         MakeUp<StorageBufferTypeCoherent>(mInstance.GetVMA(), mInstance.GetDevice(), inDeviceSize);
    inStorageBuffer->MapMemoryRegion(inMappedMemoryRegion);
}

// TODO Replace Verbosity with
void Jkr::PainterParameterBase::Setup(Up<VulkanSampler>& inStorageImageSampler,
                                      Up<StorageImageType>& inStorageImage,
                                      uint32_t inWidth,
                                      uint32_t inHeight) {
    inStorageImage = MakeUp<VulkanImageVMA<ImageContext::Storage>>(
         mInstance.GetVMA(), mInstance.GetDevice(), inWidth, inHeight);
    inStorageImageSampler = MakeUp<VulkanSampler>(mInstance.GetDevice());
}

void Jkr::PainterParameterBase::Setup(Up<VulkanSampler>& inDepthImageSampler,
                                      Up<DepthImageType>& inDepthImage,
                                      uint32_t inWidth,
                                      uint32_t inHeight) {
    inDepthImage =
         MakeUp<DepthImageType>(mInstance.GetVMA(), mInstance.GetDevice(), inWidth, inHeight);
    inDepthImageSampler = MakeUp<VulkanSampler>(mInstance.GetDevice(), ImageContext::DepthImage);
}

void Jkr::PainterParameterBase::Setup(Up<VulkanSampler>& inUniformImageSampler,
                                      Up<UniformImageType>& inUniformImage,
                                      const std::string_view inFileName) {
    int Width;
    int Height;
    int Channels;
    void* data     = stbi_load(inFileName.data(), &Width, &Height, &Channels, STBI_rgb_alpha);
    inUniformImage = MakeUp<VulkanImageVMA<ImageContext::Default>>(
         mInstance.GetVMA(), mInstance.GetDevice(), Width, Height);
    inUniformImageSampler = MakeUp<VulkanSampler>(mInstance.GetDevice());

    VulkanCommandPool Pool(mInstance.GetDevice(), mInstance.GetGraphicsQueue().GetQueueContext());
    VulkanCommandBuffer Cmd(mInstance.GetDevice(), Pool);
    VulkanFence Fence(mInstance.GetDevice()); // TODO Remove this

    inUniformImage->SubmitImmediateCmdCopyFromData(mInstance.GetGraphicsQueue(),
                                                   Cmd,
                                                   mInstance.GetDevice(),
                                                   Fence,
                                                   &data,
                                                   4 * Width * Height);
    mInstance.GetDevice().Wait();
    stbi_image_free(data);
}

void Jkr::PainterParameterBase::Setup(Up<VulkanSampler>& inUniformImageSampler,
                                      Up<UniformImageType>& inUniformImage,
                                      void** inData,
                                      uint32_t inWidth,
                                      uint32_t inHeight,
                                      uint32_t inChannelCount) {
    inUniformImage = MakeUp<VulkanImageVMA<ImageContext::Default>>(
         mInstance.GetVMA(), mInstance.GetDevice(), inWidth, inHeight, inChannelCount);
    inUniformImageSampler = MakeUp<VulkanSampler>(mInstance.GetDevice());

    VulkanCommandPool Pool(mInstance.GetDevice(), mInstance.GetGraphicsQueue().GetQueueContext());
    VulkanCommandBuffer Cmd(mInstance.GetDevice(), Pool);
    VulkanFence Fence(mInstance.GetDevice());

    inUniformImage->SubmitImmediateCmdCopyFromData(mInstance.GetGraphicsQueue(),
                                                   Cmd,
                                                   mInstance.GetDevice(),
                                                   Fence,
                                                   inData,
                                                   inWidth * inHeight * inChannelCount);
    mInstance.GetDevice().Wait();
}

void Jkr::PainterParameterBase::Setup(Up<VulkanSampler>& inUniformImageSampler,
                                      Up<UniformImageType>& inUniformImage,
                                      std::vector<s> inFileNames) {
    SetupImage<UniformImageType>(inUniformImageSampler, inUniformImage, inFileNames);
}

void Jkr::PainterParameterBase::Setup(Up<VulkanSampler>& inUniformImageSampler,
                                      Up<SkyboxImageType>& inUniformImage,
                                      std::vector<s> inFileNames) {
    SetupImage<SkyboxImageType>(inUniformImageSampler, inUniformImage, inFileNames);
}