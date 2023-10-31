#pragma once
#include "Instance.hpp"
#include "Window.hpp"
namespace Jkr
{
	enum class PainterParameterContext
	{
		StorageBuffer,
		UniformBuffer,
		StorageImage,
		UniformImage,
		UniformSampler
	};
	using StorageBufferType = VulkanBufferVMA<BufferContext::Storage, MemoryType::DeviceLocal>;
	using UniformBufferType = VulkanBufferVMA<BufferContext::Uniform, MemoryType::HostVisibleAndCoherenet>;
	using StorageImageType = VulkanImageVMA<ImageContext::Storage>;
	using UniformImageType = VulkanImageVMA<ImageContext::Default>;
	using StorageBufferType = VulkanBufferVMA<BufferContext::Storage, MemoryType::DeviceLocal>;
	using UniformSamplerType = VulkanSampler;
}

namespace Jkr
{
	class PainterParameterBase
	{
	public:
		PainterParameterBase(const Instance& inInstance) : mInstance(inInstance) {}
		~PainterParameterBase() = default;
		PainterParameterBase(PainterParameterBase&& inParam) = default;
	protected:
		void Setup(Up<StorageBufferType>& inStorageBuffer, vk::DeviceSize inDeviceSize);
		void Setup(Up<UniformBufferType>& inUniformBuffer, vk::DeviceSize inDeviceSize, void** inMappedMemoryRegion);
		void Setup(Up<VulkanSampler>& inStorageImageSampler, Up<StorageImageType>& inStorageImage, uint32_t inWidth, uint32_t inHeight);
		void Setup(Up<VulkanSampler>& inUniformImageSampler, Up<UniformImageType>& inUniformImage, std::string inFileName);
		void Setup(Up<VulkanSampler>& inUniformImageSampler, Up<UniformImageType>& inUniformImage, void** inData, uint32_t inWidth, uint32_t inHeight, uint32_t inChannelCount);
		const Instance& mInstance;
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
		GETTER& GetStorageImage() const { return *mStorageImagePtr; }
		GETTER& GetStorageImageSampler() const { return *mSampler; }
	private:
		Up<StorageImageType> mStorageImagePtr;
		Up<VulkanSampler> mSampler;
	};

	template<>
	class PainterParameter<PainterParameterContext::UniformImage> : public PainterParameterBase {
	public:
		PainterParameter(const Instance& inInstance) : PainterParameterBase(inInstance) {}
		inline void Setup(std::string inFileName) { PainterParameterBase::Setup(mSampler, mUniformImagePtr, inFileName); }
		inline void Setup(void** inData, uint32_t inWidth, uint32_t inHeight, uint32_t inChannelCount) { PainterParameterBase::Setup(mSampler, mUniformImagePtr, inData, inWidth, inHeight, inChannelCount); }
		GETTER& GetStorageImage() const { return *mUniformImagePtr; }
		GETTER& GetStorageImageSampler() const { return *mSampler; }
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
