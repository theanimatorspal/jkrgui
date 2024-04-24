#pragma once
#include "Instance.hpp"
#include "VulkanBuffer.hpp"
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
using StorageBufferType  = VulkanBufferVMA<BufferContext::Storage, MemoryType::DeviceLocal>;
using UniformSamplerType = VulkanSampler;
} // namespace Jkr

namespace Jkr {
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
} // namespace Jkr

namespace Jkr {
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
    stbi_set_flip_vertically_on_load(true);
    for (int i = 0; auto& u : inFileNames) {
        Datas.push_back(stbi_load(u.data(), &Width, &Height, &Channels, STBI_rgb_alpha));
        Datas_ptr.push_back(&Datas[i]);
        i++;
    }

    inUniformImage =
         MakeUp<T>(mInstance.GetVMA(), mInstance.GetDevice(), Width, Height, 4, inFileNames.size());
    inUniformImageSampler = MakeUp<VulkanSampler>(mInstance.GetDevice());
    inUniformImage->SubmitImmediateCmdCopyFromData(mInstance.GetGraphicsQueue(),
                                                   mInstance.GetUtilCommandBuffer(),
                                                   mInstance.GetDevice(),
                                                   Channels * Height * Width,
                                                   Datas_ptr);

    for (void* u : Datas) {
        stbi_image_free(u);
    }
}
} // namespace Jkr