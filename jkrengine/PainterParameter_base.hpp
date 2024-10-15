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

using StorageBufferType         = VulkanBufferVMA;
using StorageBufferTypeCoherent = VulkanBufferVMA;
using UniformBufferType         = VulkanBufferVMA;
using StorageAndUniform         = VulkanBufferVMA;
using StorageImageType          = VulkanImageVMA;
using UniformImageType          = VulkanImageVMA;
using SkyboxImageType           = VulkanImageVMA;
using DepthImageType            = VulkanImageVMA;
using StorageBufferType         = VulkanBufferVMA;
using UniformSamplerType        = VulkanSampler;

class PainterParameterBase {
    public:
    GETTER GetBinding() const { return mBinding; }
    GETTER GetSet() const { return mSet; }
    PainterParameterBase(Instance &inInstance)
        : mInstance(inInstance), mVulkanDescriptorSetHandler(mInstance.GetDevice()) {}
    ~PainterParameterBase()                              = default;
    PainterParameterBase(PainterParameterBase &&inParam) = default;

    public:
    void SetupStorageBuffer(Up<StorageBufferType> &inStorageBuffer, vk::DeviceSize inDeviceSize);
    void SetupUniformBuffer(Up<UniformBufferType> &inUniformBuffer,
                            vk::DeviceSize inDeviceSize,
                            void **inMappedMemoryRegion);
    void SetupStorageBufferCoherent(Up<StorageBufferTypeCoherent> &inUniformBuffer,
                                    vk::DeviceSize inDeviceSize,
                                    void **inMappedMemoryRegion);
    void SetupStorageImage(Up<VulkanSampler> &inStorageImageSampler,
                           Up<StorageImageType> &inStorageImage,
                           uint32_t inWidth,
                           uint32_t inHeight);
    void SetupUniformImage(Up<VulkanSampler> &inUniformImageSampler,
                           Up<UniformImageType> &inUniformImage,
                           const std::string_view inFileName);
    void SetupDepthImage(Up<VulkanSampler> &inDepthImageSampler,
                         Up<DepthImageType> &inDepthImage,
                         uint32_t inWidth,
                         uint32_t inHeight);
    void SetupUniformImage(Up<VulkanSampler> &inUniformImageSampler,
                           Up<UniformImageType> &inUniformImage,
                           std::vector<s> inFileNames);
    void SetupUniformImage(Up<VulkanSampler> &inUniformImageSampler,
                           Up<UniformImageType> &inUniformImage,
                           void **inData,
                           uint32_t inWidth,
                           uint32_t inHeight,
                           uint32_t inChannelCount);
    void SetupSkyboxImage(Up<VulkanSampler> &inUnifromImageSampler,
                          Up<SkyboxImageType> &inSkyboxImage,
                          std::vector<s> inFileNames);

    protected:
    Instance &mInstance;
    VulkanDescriptorUpdateHandler mVulkanDescriptorSetHandler;
    static std::mutex mMutex;
    int mBinding = -1;
    int mSet     = -1;

    private:
    void SetupImage(Up<VulkanSampler> &inUniformImageSampler,
                    Up<VulkanImageVMA> &inUniformImage,
                    std::vector<s> inFileNames,
                    ImageContext inImageContext);
};

} // namespace Jkr