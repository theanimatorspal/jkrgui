#pragma once
#include <vma/vk_mem_alloc.h>
#include "VulkanDevice.hpp"

namespace ksai {
	class VulkanVMA
	{
	public:
		VulkanVMA(const VulkanInstance& inInstance, const VulkanDevice& inDevice)
		{
			auto info = VmaAllocatorCreateInfo();
			info.device = inDevice.GetDeviceHandle();
			info.instance = inInstance.GetInstanceHandle();
			info.vulkanApiVersion = VK_API_VERSION_1_3;
			info.physicalDevice = inDevice.GetPhysicalDeviceHandle();
			vmaCreateAllocator(&info, &mAllocator);
		}
		~VulkanVMA()
		{
			vmaDestroyAllocator(mAllocator);
		}
		GETTER& GetVMAHandle() const { return mAllocator; }
	private:
		VmaAllocator mAllocator;
	};
}
