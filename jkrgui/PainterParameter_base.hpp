#pragma once
#include "Instance.hpp"
#include "VulkanBuffer.hpp"
#include <Vulkan/Pipeline/VulkanDescriptorUpdateHandler.hpp>
#include <VulkanBufferVMA.hpp>
#include <VulkanImageVMA.hpp>
#include <VulkanSampler.hpp>

extern "C" {
#include <Vendor/stbi/stb_image.h>
}

namespace Jkr {
enum class PainterParameterContext {
    StorageBuffer,
    UniformBuffer,
    StorageImage,
    UniformImage,
    SkyboxImage,
    UniformSampler
};

using StorageBufferType = VulkanBufferVMA<BufferContext::Storage, MemoryType::DeviceLocal>;
using StorageBufferTypeCoherent =
     VulkanBufferVMA<BufferContext::Storage, MemoryType::HostVisibleAndCoherenet>;
using UniformBufferType =
     VulkanBufferVMA<BufferContext::Uniform, MemoryType::HostVisibleAndCoherenet>;
using StorageAndUniform  = VulkanBufferVMA<BufferContext::Storage | BufferContext::Uniform,
                                           MemoryType::HostVisibleAndCoherenet>;
using StorageImageType   = VulkanImageVMA<ImageContext::Storage>;
using UniformImageType   = VulkanImageVMA<ImageContext::Default>;
using SkyboxImageType    = VulkanImageVMA<ImageContext::CubeCompatible>;
using DepthImageType     = VulkanImageVMA<ImageContext::DepthImage>;
using StorageBufferType  = VulkanBufferVMA<BufferContext::Storage, MemoryType::DeviceLocal>;
using UniformSamplerType = VulkanSampler;

class PainterParameterBase {
    public:
    PainterParameterBase(const Instance& inInstance)
        : mInstance(inInstance), mVulkanDescriptorSetHandler(mInstance.GetDevice()) {}
    ~PainterParameterBase()                              = default;
    PainterParameterBase(PainterParameterBase&& inParam) = default;

    public:
    void Setup(Up<StorageBufferType>& inStorageBuffer, vk::DeviceSize inDeviceSize);
    void Setup(Up<UniformBufferType>& inUniformBuffer,
               vk::DeviceSize inDeviceSize,
               void** inMappedMemoryRegion);
    void Setup(Up<StorageBufferTypeCoherent>& inUniformBuffer,
               vk::DeviceSize inDeviceSize,
               void** inMappedMemoryRegion);
    void Setup(Up<VulkanSampler>& inStorageImageSampler,
               Up<StorageImageType>& inStorageImage,
               uint32_t inWidth,
               uint32_t inHeight);
    void Setup(Up<VulkanSampler>& inUniformImageSampler,
               Up<UniformImageType>& inUniformImage,
               const std::string_view inFileName);
    void Setup(Up<VulkanSampler>& inDepthImageSampler,
               Up<DepthImageType>& inDepthImage,
               uint32_t inWidth,
               uint32_t inHeight);
    void Setup(Up<VulkanSampler>& inUniformImageSampler,
               Up<UniformImageType>& inUniformImage,
               std::vector<s> inFileNames);
    void Setup(Up<VulkanSampler>& inUniformImageSampler,
               Up<UniformImageType>& inUniformImage,
               void** inData,
               uint32_t inWidth,
               uint32_t inHeight,
               uint32_t inChannelCount);
    void Setup(Up<VulkanSampler>& inUnifromImageSampler,
               Up<SkyboxImageType>& inSkyboxImage,
               std::vector<s> inFileNames);

    protected:
    const Instance& mInstance;
    VulkanDescriptorUpdateHandler mVulkanDescriptorSetHandler;

    private:
    template <typename T>
    void SetupImage(Up<VulkanSampler>& inUniformImageSampler,
                    Up<T>& inUniformImage,
                    std::vector<s> inFileNames);
};

template <typename T>
inline void PainterParameterBase::SetupImage(Up<VulkanSampler>& inUniformImageSampler,
                                             Up<T>& inUniformImage,
                                             std::vector<s> inFileNames) {
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

    VulkanCommandPool Pool(mInstance.GetDevice(), mInstance.GetGraphicsQueue().GetQueueContext());
    VulkanCommandBuffer Cmd(mInstance.GetDevice(), Pool);
    VulkanFence Fence(mInstance.GetDevice()); // TODO Remove this

    inUniformImage =
         MakeUp<T>(mInstance.GetVMA(), mInstance.GetDevice(), Width, Height, 4, inFileNames.size());
    inUniformImageSampler = MakeUp<VulkanSampler>(mInstance.GetDevice());
    inUniformImage->SubmitImmediateCmdCopyFromData(mInstance.GetGraphicsQueue(),
                                                   Cmd,
                                                   mInstance.GetDevice(),
                                                   Fence,
                                                   Channels * Height * Width,
                                                   Datas_ptr);

    for (void* u : Datas) {
        stbi_image_free(u);
    }
}
} // namespace Jkr