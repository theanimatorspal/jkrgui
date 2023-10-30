#pragma once
#include "Instance.hpp"
#include "GUIWindow.hpp"
namespace Jkr
{
	enum class GUIPainterParameterContext
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
	class GUIPainterParameterBase
	{
	public:
		GUIPainterParameterBase(const Instance& inInstance) : mInstance(inInstance) {}
		~GUIPainterParameterBase() = default;
		GUIPainterParameterBase(GUIPainterParameterBase&& inParam) = default;
	protected:
		void Setup(Up<StorageBufferType>& inStorageBuffer, vk::DeviceSize inDeviceSize);
		void Setup(Up<UniformBufferType>& inUniformBuffer, vk::DeviceSize inDeviceSize, void** inMappedMemoryRegion);
		void Setup(Up<VulkanSampler>& inStorageImageSampler, Up<StorageImageType>& inStorageImage, uint32_t inWidth, uint32_t inHeight);
		void Setup(Up<VulkanSampler>& inUniformImageSampler, Up<UniformImageType>& inUniformImage, std::string inFileName);
		void Setup(Up<VulkanSampler>& inUniformImageSampler, Up<UniformImageType>& inUniformImage, void** inData, uint32_t inWidth, uint32_t inHeight, uint32_t inChannelCount);
		const Instance& mInstance;
	};
	template <GUIPainterParameterContext inContext>
	class GUIPainterParameter : public GUIPainterParameterBase { };
}

namespace Jkr
{
	template<>
	class GUIPainterParameter<GUIPainterParameterContext::StorageBuffer> : public GUIPainterParameterBase {
	public:
		GUIPainterParameter(const Instance& inInstance) : GUIPainterParameterBase(inInstance) {}
		inline void Setup(vk::DeviceSize inStorageBufferSize) { GUIPainterParameterBase::Setup(mStorageBufferPtr, inStorageBufferSize); }
		GETTER& GetStorageBuffer() const { return *mStorageBufferPtr; }
	private:
		Up<StorageBufferType> mStorageBufferPtr;
	};

	template<>
	class GUIPainterParameter<GUIPainterParameterContext::UniformBuffer> : public GUIPainterParameterBase {
	public:
		GUIPainterParameter(const Instance& inInstance) : GUIPainterParameterBase(inInstance) {}
		inline void Setup(vk::DeviceSize inUniformBufferSize) { GUIPainterParameterBase::Setup(mUniformBufferPtr, inUniformBufferSize, &mUniformMappedMemoryRegion); }
		GETTER& GetUniformMappedMemoryRegion() { return mUniformMappedMemoryRegion; }
		GETTER& GetUniformBuffer() const { return *mUniformBufferPtr; }
	private:
		Up<UniformBufferType> mUniformBufferPtr;
		void* mUniformMappedMemoryRegion;
	};

	template<>
	class GUIPainterParameter<GUIPainterParameterContext::StorageImage> : public GUIPainterParameterBase {
	public:
		GUIPainterParameter(const Instance& inInstance) : GUIPainterParameterBase(inInstance) {}
		inline void Setup(uint32_t inWidth, uint32_t inHeight)
		{
			GUIPainterParameterBase::Setup(mSampler, mStorageImagePtr, inWidth, inHeight);
		}
		GETTER& GetStorageImage() const { return *mStorageImagePtr; }
		GETTER& GetStorageImageSampler() const { return *mSampler; }
	private:
		Up<StorageImageType> mStorageImagePtr;
		Up<VulkanSampler> mSampler;
	};

	template<>
	class GUIPainterParameter<GUIPainterParameterContext::UniformImage> : public GUIPainterParameterBase {
	public:
		GUIPainterParameter(const Instance& inInstance) : GUIPainterParameterBase(inInstance) {}
		inline void Setup(std::string inFileName) { GUIPainterParameterBase::Setup(mSampler, mUniformImagePtr, inFileName); }
		inline void Setup(void** inData, uint32_t inWidth, uint32_t inHeight, uint32_t inChannelCount) { GUIPainterParameterBase::Setup(mSampler, mUniformImagePtr, inData, inWidth, inHeight, inChannelCount); }
		GETTER& GetStorageImage() const { return *mUniformImagePtr; }
		GETTER& GetStorageImageSampler() const { return *mSampler; }
	private:
		Up<UniformImageType> mUniformImagePtr;
		Up<VulkanSampler> mSampler;
	};

	template<>
	class GUIPainterParameter<GUIPainterParameterContext::UniformSampler> : public GUIPainterParameterBase {
	public:
		GUIPainterParameter(const Instance& inInstance) : GUIPainterParameterBase(inInstance) {}
		inline void Setup() { mSampler = MakeUp<VulkanSampler>(mInstance.GetDevice()); }
		GETTER& GetSampler() const { return *mSampler; }
	private:
		Up<VulkanSampler> mSampler;
	};

}
