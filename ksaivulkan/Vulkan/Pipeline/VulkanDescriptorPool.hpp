#pragma once
#include "VulkanDescriptorPoolContext.hpp"

namespace ksai {
	class VulkanDescriptorPoolBase
	{
	public:
		VulkanDescriptorPoolBase(const VulkanDevice& inDevice)
			: mDevice (inDevice.GetDeviceHandle())
		{

		}
		~VulkanDescriptorPoolBase()
		{
			mDevice.destroyDescriptorPool(mPool);
		}
	public:
		GETTER& GetDescriptorPoolHandle() const { return mPool; }
		GETTER GetMaxDescriptorSets() const { return mMaxDescriptorSets; }
	protected:
		const vk::Device& mDevice;
		vk::DescriptorPool mPool;
		int mMaxDescriptorSets = 0;
	};
}


namespace ksai {
	class VulkanDescriptorPool : public VulkanDescriptorPoolBase
	{
	public:
		VulkanDescriptorPool(const VulkanDevice& inDevice, int inMaxDescriptorSets, int PoolSize)
                : VulkanDescriptorPoolBase(inDevice)
                , mPoolContext(PoolSize)
		{
			mPool = inDevice.GetDeviceHandle().createDescriptorPool(
				vk::DescriptorPoolCreateInfo(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet, inMaxDescriptorSets, mPoolContext.GetDescriptorPoolSizesArray())
			);
			mMaxDescriptorSets = PoolSize;
		}
		~VulkanDescriptorPool() = default;
	private:
		const VulkanDescriptorPoolContext mPoolContext;
	};
}
