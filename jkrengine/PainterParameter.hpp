#pragma once
#include "PainterParameter_base.hpp"
#include "VulkanImageContext.hpp"

namespace Jkr {
template <PainterParameterContext inContext>
class PainterParameter : public PainterParameterBase {};

template <>
class PainterParameter<PainterParameterContext::StorageBuffer> : public PainterParameterBase {
    public:
    GETTER &GetStorageBuffer() const { return *mStorageBufferPtr; }
    GETTER &GetStorageBufferCoherent() const { return *mStorageBufferCoherentPtr; }
    GETTER &GetStorageMappedMemoryRegion() { return mStorageBufferCoherentMemoryMappedRegion; }
    void Setup(vk::DeviceSize inStorageBufferSize) {
        PainterParameterBase::SetupStorageBuffer(mStorageBufferPtr, inStorageBufferSize);
    }
    void SetupCoherent(vk::DeviceSize inStorageBufferSize) {
        PainterParameterBase::SetupStorageBufferCoherent(mStorageBufferCoherentPtr,
                                                         inStorageBufferSize,
                                                         &mStorageBufferCoherentMemoryMappedRegion);
    }
    void Register(vk::DeviceSize inOffset,
                  uint32_t inDstBinding,
                  uint32_t inDstArrayElement,
                  VulkanDescriptorSet &inDescriptorSet,
                  ui inSet = 0) {
        mSet     = inSet;
        mBinding = inDstBinding;
        mVulkanDescriptorSetHandler.RW(BufferContext::Storage,
                                       inDescriptorSet,
                                       *mStorageBufferPtr,
                                       inOffset,
                                       inDstBinding,
                                       inDstArrayElement,
                                       inSet);
    }
    void RegisterCoherent(vk::DeviceSize inOffset,
                          uint32_t inDstBinding,
                          uint32_t inDstArrayElement,
                          VulkanDescriptorSet &inDescriptorSet,
                          ui inSet = 0) {
        mSet     = inSet;
        mBinding = inDstBinding;
        mVulkanDescriptorSetHandler.RW(BufferContext::Storage,
                                       inDescriptorSet,
                                       *mStorageBufferCoherentPtr,
                                       inOffset,
                                       inDstBinding,
                                       inDstArrayElement,
                                       inSet);
    }

    PainterParameter(Instance &inInstance) : PainterParameterBase(inInstance) {}

    Up<StorageBufferType> mStorageBufferPtr;
    Up<StorageBufferTypeCoherent> mStorageBufferCoherentPtr;

    private:
    void *mStorageBufferCoherentMemoryMappedRegion;
};

///@warning Uniform buffer is always coherent
template <>
class PainterParameter<PainterParameterContext::UniformBuffer> : public PainterParameterBase {
    public:
    GETTER &GetUniformMappedMemoryRegion() {
        ///@note Legacy compatible fix
        mUniformBufferPtr->MapMemoryRegion(&mUniformMappedMemoryRegion);
        return mUniformMappedMemoryRegion;
    }
    GETTER &GetUniformBuffer() const { return *mUniformBufferPtr; }
    void Setup(vk::DeviceSize inUniformBufferSize) {
        PainterParameterBase::SetupUniformBuffer(
             mUniformBufferPtr, inUniformBufferSize, &mUniformMappedMemoryRegion);
    }
    void Register(vk::DeviceSize inOffset,
                  uint32_t inDstBinding,
                  uint32_t inDstArrayElement,
                  VulkanDescriptorSet &inDescriptorSet,
                  ui inSet = 0) {
        mSet     = inSet;
        mBinding = inDstBinding;
        mVulkanDescriptorSetHandler.RW(BufferContext::Uniform,
                                       inDescriptorSet,
                                       *mUniformBufferPtr,
                                       inOffset,
                                       inDstBinding,
                                       inDstArrayElement,
                                       inSet);
    }

    PainterParameter(Instance &inInstance) : PainterParameterBase(inInstance) {}

    Up<UniformBufferType> mUniformBufferPtr;

    private:
    void *mUniformMappedMemoryRegion;
};

template <>
class PainterParameter<PainterParameterContext::StorageImage> : public PainterParameterBase {
    public:
    GETTER GetStorageImagePtr() const { return mStorageImagePtr.get(); }
    GETTER &GetStorageImage() const { return *mStorageImagePtr; }
    GETTER &GetStorageImageSampler() const { return *mSampler; }
    void Setup(uint32_t inWidth, uint32_t inHeight) {
        PainterParameterBase::SetupStorageImage(mSampler, mStorageImagePtr, inWidth, inHeight);
    }
    void Register(vk::DeviceSize inOffset,
                  uint32_t inDstBinding,
                  uint32_t inDstArrayElement,
                  VulkanDescriptorSet &inDescriptorSet,
                  ui inSet = 0) {
        mSet     = inSet;
        mBinding = inDstBinding;
        mVulkanDescriptorSetHandler.RW(ImageContext::Storage,
                                       inDescriptorSet,
                                       *mStorageImagePtr,
                                       *mSampler,
                                       inDstBinding,
                                       inDstArrayElement,
                                       inSet);
    }

    PainterParameter(Instance &inInstance) : PainterParameterBase(inInstance) {}

    Up<StorageImageType> mStorageImagePtr;
    Up<VulkanSampler> mSampler;
};

template <>
class PainterParameter<PainterParameterContext::UniformImage> : public PainterParameterBase {
    public:
    GETTER &GetUniformImage() const { return *mUniformImagePtr; }
    GETTER &GetDepthImage() const { return *mDepthImagePtr; }
    GETTER &GetUniformImageSampler() const { return *mSampler; }
    void Setup(const std::string_view inFileName) {
        PainterParameterBase::SetupUniformImage(mSampler, mUniformImagePtr, inFileName);
    }
    void Setup(void **inData, uint32_t inWidth, uint32_t inHeight, uint32_t inChannelCount) {
        PainterParameterBase::SetupUniformImage(
             mSampler, mUniformImagePtr, inData, inWidth, inHeight, inChannelCount);
    }
    void Setup(std::vector<s> inFileNames) {
        PainterParameterBase::SetupUniformImage(mSampler, mUniformImagePtr, inFileNames);
    }
    void SetupDepth(ui inWidth, ui inHeight) {
        PainterParameterBase::SetupDepthImage(mSampler, mDepthImagePtr, inWidth, inHeight);
    }
    void Register(vk::DeviceSize inOffset,
                  uint32_t inDstBinding,
                  uint32_t inDstArrayElement,
                  VulkanDescriptorSet &inDescriptorSet,
                  ui inSet = 0) {
        mSet     = inSet;
        mBinding = inDstBinding;
        mVulkanDescriptorSetHandler.RW(ImageContext::Default,
                                       inDescriptorSet,
                                       *mUniformImagePtr,
                                       *mSampler,
                                       inDstBinding,
                                       inDstArrayElement,
                                       inSet);
    }
    void RegisterDepth(vk::DeviceSize inOffset,
                       uint32_t inDstBinding,
                       uint32_t inDstArrayElement,
                       VulkanDescriptorSet &inDescriptorSet,
                       ui inSet = 0) { // TODO Deprecate this
        mSet     = inSet;
        mBinding = inDstBinding;
        mVulkanDescriptorSetHandler.RW(ImageContext::Default,
                                       inDescriptorSet,
                                       *mDepthImagePtr,
                                       *mSampler,
                                       inDstBinding,
                                       inDstArrayElement,
                                       inSet);
    }

    PainterParameter(Instance &inInstance) : PainterParameterBase(inInstance) {}

    Up<UniformImageType> mUniformImagePtr;
    Up<DepthImageType> mDepthImagePtr; // TODO Remove this
    Up<VulkanSampler> mSampler;
};

template <>
class PainterParameter<PainterParameterContext::SkyboxImage> : public PainterParameterBase {
    public:
    GETTER &GetUniformImage() const { return *mUniformImagePtr; }
    GETTER &GetUniformImageSampler() const { return *mSampler; }
    void Setup(std::vector<s> inFileNames) {
        PainterParameterBase::SetupSkyboxImage(mSampler, mUniformImagePtr, inFileNames);
    }
    void Register(vk::DeviceSize inOffset,
                  uint32_t inDstBinding,
                  uint32_t inDstArrayElement,
                  VulkanDescriptorSet &inDescriptorSet,
                  ui inSet = 0) {
        mSet     = inSet;
        mBinding = inDstBinding;
        mVulkanDescriptorSetHandler.RW(ImageContext::CubeCompatible,
                                       inDescriptorSet,
                                       *mUniformImagePtr,
                                       *mSampler,
                                       inDstBinding,
                                       inDstArrayElement,
                                       inSet);
    }

    PainterParameter(Instance &inInstance) : PainterParameterBase(inInstance) {}

    Up<SkyboxImageType> mUniformImagePtr;
    Up<VulkanSampler> mSampler;
};

template <>
class PainterParameter<PainterParameterContext::UniformSampler> : public PainterParameterBase {
    public:
    void Setup() { mSampler = MakeUp<VulkanSampler>(mInstance.GetDevice()); }
    GETTER &GetSampler() const { return *mSampler; }

    PainterParameter(Instance &inInstance) : PainterParameterBase(inInstance) {}

    Up<VulkanSampler> mSampler;
};

} // namespace Jkr
