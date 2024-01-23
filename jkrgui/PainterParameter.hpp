#pragma once
#include "Instance.hpp"

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
using UniformBufferType
    = VulkanBufferVMA<BufferContext::Uniform, MemoryType::HostVisibleAndCoherenet>;
using StorageImageType = VulkanImageVMA<ImageContext::Storage>;
using UniformImageType = VulkanImageVMA<ImageContext::Default>;
using SkyboxImageType = VulkanImageVMA<ImageContext::CubeCompatible>;
using StorageBufferType = VulkanBufferVMA<BufferContext::Storage, MemoryType::DeviceLocal>;
using UniformSamplerType = VulkanSampler;
} // namespace Jkr

namespace Jkr {
class PainterParameterBase {
public:
    PainterParameterBase(const Instance& inInstance)
        : mInstance(inInstance)
        , mVulkanDescriptorSetHandler(mInstance.GetDevice())
    {
    }
    ~PainterParameterBase() = default;
    PainterParameterBase(PainterParameterBase&& inParam) = default;

protected:
    void Setup(Up<StorageBufferType>& inStorageBuffer, vk::DeviceSize inDeviceSize);
    void Setup(Up<UniformBufferType>& inUniformBuffer, vk::DeviceSize inDeviceSize, void** inMappedMemoryRegion);
    void Setup(Up<VulkanSampler>& inStorageImageSampler, Up<StorageImageType>& inStorageImage, uint32_t inWidth, uint32_t inHeight);
    void Setup(Up<VulkanSampler>& inUniformImageSampler, Up<UniformImageType>& inUniformImage, const std::string_view inFileName);
    void Setup(Up<VulkanSampler>& inUniformImageSampler, Up<UniformImageType>& inUniformImage, std::span<const sv> inFileNames);
    void Setup(Up<VulkanSampler>& inUniformImageSampler, Up<UniformImageType>& inUniformImage, void** inData, uint32_t inWidth, uint32_t inHeight, uint32_t inChannelCount);
    void Setup(Up<VulkanSampler>& inUnifromImageSampler, Up<SkyboxImageType>& inSkyboxImage, std::span<const sv> inFileNames);

protected:
    const Instance& mInstance;
    VulkanDescriptorUpdateHandler mVulkanDescriptorSetHandler;

private:
    template <typename T>
    void SetupImage(Up<VulkanSampler>& inUniformImageSampler, Up<T>& inUniformImage, std::span<const sv> inFileNames);
};

template <PainterParameterContext inContext>
class PainterParameter : public PainterParameterBase { };

template <typename T>
inline void PainterParameterBase::SetupImage(Up<VulkanSampler>& inUniformImageSampler, Up<T>& inUniformImage, std::span<const sv> inFileNames)
{
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

    inUniformImage = MakeUp<T>(mInstance.GetVMA(), mInstance.GetDevice(), Width, Height, inFileNames.size());
    inUniformImageSampler = MakeUp<VulkanSampler>(mInstance.GetDevice());
    inUniformImage->SubmitImmediateCmdCopyFromData(
        mInstance.GetGraphicsQueue(),
        mInstance.GetUtilCommandBuffer(),
        mInstance.GetDevice(),
        Channels * Height * Width, Datas_ptr);

    for (void* u : Datas) {
        stbi_image_free(u);
    }
}
}

namespace Jkr {
template <>
class PainterParameter<PainterParameterContext::StorageBuffer> : public PainterParameterBase {
public:
    PainterParameter(const Instance& inInstance)
        : PainterParameterBase(inInstance)
    {
    }
    inline void Setup(vk::DeviceSize inStorageBufferSize) { PainterParameterBase::Setup(mStorageBufferPtr, inStorageBufferSize); }
    GETTER& GetStorageBuffer() const { return *mStorageBufferPtr; }

private:
    Up<StorageBufferType> mStorageBufferPtr;
};

template <>
class PainterParameter<PainterParameterContext::UniformBuffer> : public PainterParameterBase {
public:
    PainterParameter(const Instance& inInstance)
        : PainterParameterBase(inInstance)
    {
    }
    inline void Setup(vk::DeviceSize inUniformBufferSize)
    {
        PainterParameterBase::Setup(mUniformBufferPtr, inUniformBufferSize, &mUniformMappedMemoryRegion);
        mSize = inUniformBufferSize;
    }
    GETTER& GetUniformMappedMemoryRegion() { return mUniformMappedMemoryRegion; }
    GETTER& GetUniformBuffer() const { return *mUniformBufferPtr; }
    GETTER GetUniformBufferSize() const { return mSize; }

private:
    sz mSize;
    Up<UniformBufferType> mUniformBufferPtr;
    void* mUniformMappedMemoryRegion;
};

template <>
class PainterParameter<PainterParameterContext::StorageImage> : public PainterParameterBase {
public:
    PainterParameter(const Instance& inInstance)
        : PainterParameterBase(inInstance)
    {
    }
    inline void Setup(uint32_t inWidth, uint32_t inHeight)
    {
        PainterParameterBase::Setup(mSampler, mStorageImagePtr, inWidth, inHeight);
    }
    GETTER GetStorageImagePtr() const { return mStorageImagePtr.get(); }
    GETTER& GetStorageImage() const { return *mStorageImagePtr; }
    GETTER& GetStorageImageSampler() const { return *mSampler; }

private:
    Up<StorageImageType> mStorageImagePtr;
    Up<VulkanSampler> mSampler;
};

template <>
class PainterParameter<PainterParameterContext::UniformImage> : public PainterParameterBase {
public:
    PainterParameter(const Instance& inInstance)
        : PainterParameterBase(inInstance)
    {
    }
    inline void Setup(const std::string_view inFileName) { PainterParameterBase::Setup(mSampler, mUniformImagePtr, inFileName); }
    inline void Setup(void** inData, uint32_t inWidth, uint32_t inHeight, uint32_t inChannelCount) { PainterParameterBase::Setup(mSampler, mUniformImagePtr, inData, inWidth, inHeight, inChannelCount); }
    inline void Setup(std::span<const sv> inFileNames) { PainterParameterBase::Setup(mSampler, mUniformImagePtr, inFileNames); }
    GETTER& GetUniformImage() const { return *mUniformImagePtr; }
    GETTER& GetUniformImageSampler() const { return *mSampler; }
    void Register(
        vk::DeviceSize inOffset,
        uint32_t inDstBinding,
        uint32_t inDstArrayElement,
        VulkanDescriptorSet& inDescriptorSet)
    {
        mVulkanDescriptorSetHandler.Write<ImageContext::Default>(
            inDescriptorSet,
            *mUniformImagePtr,
            *mSampler,
            inDstBinding,
            inDstArrayElement);
    }

private:
    Up<UniformImageType> mUniformImagePtr;
    Up<VulkanSampler> mSampler;
};

template <>
class PainterParameter<PainterParameterContext::SkyboxImage> : public PainterParameterBase {
public:
    PainterParameter(const Instance& inInstance)
        : PainterParameterBase(inInstance)
    {
    }
    inline void Setup(std::span<const sv> inFileNames) { PainterParameterBase::Setup(mSampler, mUniformImagePtr, inFileNames); }
    GETTER& GetUniformImage() const { return *mUniformImagePtr; }
    GETTER& GetUniformImageSampler() const { return *mSampler; }
    void Register(
        vk::DeviceSize inOffset,
        uint32_t inDstBinding,
        uint32_t inDstArrayElement,
        VulkanDescriptorSet& inDescriptorSet)
    {
        mVulkanDescriptorSetHandler.Write<ImageContext::Default>(
            inDescriptorSet,
            *mUniformImagePtr,
            *mSampler,
            inDstBinding,
            inDstArrayElement);
    }

private:
    Up<SkyboxImageType> mUniformImagePtr;
    Up<VulkanSampler> mSampler;
};

template <>
class PainterParameter<PainterParameterContext::UniformSampler> : public PainterParameterBase {
public:
    PainterParameter(const Instance& inInstance)
        : PainterParameterBase(inInstance)
    {
    }
    inline void Setup() { mSampler = MakeUp<VulkanSampler>(mInstance.GetDevice()); }
    GETTER& GetSampler() const { return *mSampler; }

private:
    Up<VulkanSampler> mSampler;
};

}
