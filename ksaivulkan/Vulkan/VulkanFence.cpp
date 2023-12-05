#include "VulkanFence.hpp"
using namespace ksai;
#ifdef max
#undef max
#endif
#include <limits>

VulkanFence::VulkanFence(const VulkanDevice& inDevice) : mDevice(inDevice.GetDeviceHandle())
{
	auto FenceCreateInfo = vk::FenceCreateInfo(vk::FenceCreateFlags())
		.setFlags(vk::FenceCreateFlagBits::eSignaled);
	mFence = mDevice.createFence(FenceCreateInfo);
}

VulkanFence::~VulkanFence()
{
	mDevice.destroyFence(mFence);
}

void VulkanFence::Wait()
{
	vk::Result Result =  mDevice.waitForFences(mFence, true, std::numeric_limits<uint32_t>::max());
}

void VulkanFence::Reset()
{
	mDevice.resetFences(mFence);
}
