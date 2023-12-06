#include "Instance.hpp"
#include "TestUtils.hpp"

Jkr::Instance::Instance()
	: mInstance(), mMessenger(mInstance), mSurface(mInstance, TestWindow().GetHandle()),
	mPhysicalDevice(mInstance), mQueueContext(mPhysicalDevice, mSurface), mDevice(mPhysicalDevice, mQueueContext),
	mGraphicsQueue(mQueueContext, mDevice),
	mCommandPool(mDevice, mQueueContext),
	mUtilCommandBuffer(mDevice, mCommandPool),
	mDescriptorPool(mDevice, 20),
	mVmaAllocator(mInstance, mDevice)
{
}
