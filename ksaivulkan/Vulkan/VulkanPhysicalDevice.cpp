#include "VulkanPhysicalDevice.hpp"
using namespace ksai;

VulkanPhysicalDevice::VulkanPhysicalDevice(const VulkanInstance& inInstance) : mInstance(inInstance.GetInstanceHandle())
{
	mPhysicalDevice = mInstance.enumeratePhysicalDevices().front();
}

ksai::VulkanPhysicalDevice::~VulkanPhysicalDevice()
{
}

