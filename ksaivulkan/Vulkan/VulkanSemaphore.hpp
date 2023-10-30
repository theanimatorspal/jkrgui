#pragma once
#include "VulkanDevice.hpp"
namespace ksai {

	class VulkanSemaphore
	{
	public:
		VulkanSemaphore(const VulkanDevice& inDevice);
		~VulkanSemaphore();
		GETTER& GetSemaphoreHandle() const { return mSemaphore; }
	private:
		const vk::Device& mDevice;
		vk::Semaphore mSemaphore;
	};

}
