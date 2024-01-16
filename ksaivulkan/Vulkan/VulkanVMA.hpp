#pragma once
#include "VulkanDevice.hpp"
#define VMA_VULKAN_VERSION 1001000 // Vulkan 1.1
#include <vma/vk_mem_alloc.h>

namespace ksai {
	class VulkanVMA
	{
	public:
		VulkanVMA(const VulkanInstance& inInstance, const VulkanDevice& inDevice)
		{
			auto info = VmaAllocatorCreateInfo();
			info.device = inDevice.GetDeviceHandle();
			info.instance = inInstance.GetInstanceHandle();
			info.vulkanApiVersion = VK_API_VERSION_1_1;
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
