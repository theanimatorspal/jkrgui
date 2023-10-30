#include "VulkanQueue.hpp"
using namespace ksai;

ksai::VulkanQueueBase::VulkanQueueBase(const VulkanQueueContext& inQueueContext, const VulkanDevice& inDevice) 
	: mDevice(inDevice.GetDeviceHandle()),
	mQueueContext(inQueueContext)
{
}
