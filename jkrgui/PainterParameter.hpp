#pragma once
#include "Instance.hpp"

namespace Jkr {
enum class PainterParameterContext {
    StorageBuffer,
    UniformBuffer,
    StorageImage,
    UniformImage,
    UniformSampler
};
using StorageBufferType = VulkanBufferVMA<BufferContext::Storage, MemoryType::DeviceLocal>;
using UniformBufferType
    = VulkanBufferVMA<BufferContext::Uniform, MemoryType::HostVisibleAndCoherenet>;
using StorageImageType = VulkanImageVMA<ImageContext::Storage>;
using UniformImageType = VulkanImageVMA<ImageContext::Default>;
using StorageBufferType = VulkanBufferVMA<BufferContext::Storage, MemoryType::DeviceLocal>;
using UniformSamplerType = VulkanSampler;
} // namespace Jkr

namespace Jkr
{
	class PainterParameterBase
	{
	public:
		PainterParameterBase(const Instance& inInstance) : mInstance(inInstance), mVulkanDescriptorSetHandler(mInstance.GetDevice()) {}
		~PainterParameterBase() = default;
		PainterParameterBase(PainterParameterBase&& inParam) = default;
	protected:
		void Setup(Up<StorageBufferType>& inStorageBuffer, vk::DeviceSize inDeviceSize);
		void Setup(Up<UniformBufferType>& inUniformBuffer, vk::DeviceSize inDeviceSize, void** inMappedMemoryRegion);
		void Setup(Up<VulkanSampler>& inStorageImageSampler, Up<StorageImageType>& inStorageImage, uint32_t inWidth, uint32_t inHeight);
		void Setup(Up<VulkanSampler>& inUniformImageSampler, Up<UniformImageType>& inUniformImage, const std::string_view inFileName);
		void Setup(Up<VulkanSampler>& inUniformImageSampler, Up<UniformImageType>& inUniformImage, void** inData, uint32_t inWidth, uint32_t inHeight, uint32_t inChannelCount);
	protected:
		const Instance& mInstance;
		VulkanDescriptorUpdateHandler mVulkanDescriptorSetHandler;
	};
	template <PainterParameterContext inContext>
	class PainterParameter : public PainterParameterBase { };
}

namespace Jkr
{
	template<>
	class PainterParameter<PainterParameterContext::StorageBuffer> : public PainterParameterBase {
	public:
		PainterParameter(const Instance& inInstance) : PainterParameterBase(inInstance) {}
		inline void Setup(vk::DeviceSize inStorageBufferSize) { PainterParameterBase::Setup(mStorageBufferPtr, inStorageBufferSize); }
		GETTER& GetStorageBuffer() const { return *mStorageBufferPtr; }
	private:
		Up<StorageBufferType> mStorageBufferPtr;
	};

	template<>
	class PainterParameter<PainterParameterContext::UniformBuffer> : public PainterParameterBase {
	public:
		PainterParameter(const Instance& inInstance) : PainterParameterBase(inInstance) {}
		inline void Setup(vk::DeviceSize inUniformBufferSize) { PainterParameterBase::Setup(mUniformBufferPtr, inUniformBufferSize, &mUniformMappedMemoryRegion); }
		GETTER& GetUniformMappedMemoryRegion() { return mUniformMappedMemoryRegion; }
		GETTER& GetUniformBuffer() const { return *mUniformBufferPtr; }
	private:
		Up<UniformBufferType> mUniformBufferPtr;
		void* mUniformMappedMemoryRegion;
	};

	template<>
	class PainterParameter<PainterParameterContext::StorageImage> : public PainterParameterBase {
	public:
		PainterParameter(const Instance& inInstance) : PainterParameterBase(inInstance) {}
		inline void Setup(uint32_t inWidth, uint32_t inHeight)
		{
			PainterParameterBase::Setup(mSampler, mStorageImagePtr, inWidth, inHeight);
		}
        GETTER GetStorageImagePtr() const { return mStorageImagePtr.get(); }
        GETTER &GetStorageImage() const { return *mStorageImagePtr; }
        GETTER& GetStorageImageSampler() const { return *mSampler; }
	private:
		Up<StorageImageType> mStorageImagePtr;
		Up<VulkanSampler> mSampler;
	};

	template<>
	class PainterParameter<PainterParameterContext::UniformImage> : public PainterParameterBase {
	public:
		PainterParameter(const Instance& inInstance) : PainterParameterBase(inInstance) {}
		inline void Setup(const std::string_view inFileName) { PainterParameterBase::Setup(mSampler, mUniformImagePtr, inFileName); }
		inline void Setup(void** inData, uint32_t inWidth, uint32_t inHeight, uint32_t inChannelCount) { PainterParameterBase::Setup(mSampler, mUniformImagePtr, inData, inWidth, inHeight, inChannelCount); }
		GETTER& GetUniformImage() const { return *mUniformImagePtr; }
		GETTER& GetUniformImageSampler() const { return *mSampler; }
		void Register(
			vk::DeviceSize inOffset,
			uint32_t inDstBinding,
			uint32_t inDstArrayElement,
			VulkanDescriptorSet& inDescriptorSet
		)
		{
			mVulkanDescriptorSetHandler.Write<ImageContext::Default>(
				inDescriptorSet,
				*mUniformImagePtr,
				*mSampler,
				inDstBinding,
				inDstArrayElement
			);

		}
	private:
		Up<UniformImageType> mUniformImagePtr;
		Up<VulkanSampler> mSampler;
	};

	template<>
	class PainterParameter<PainterParameterContext::UniformSampler> : public PainterParameterBase {
	public:
		PainterParameter(const Instance& inInstance) : PainterParameterBase(inInstance) {}
		inline void Setup() { mSampler = MakeUp<VulkanSampler>(mInstance.GetDevice()); }
		GETTER& GetSampler() const { return *mSampler; }
	private:
		Up<VulkanSampler> mSampler;
	};

}
