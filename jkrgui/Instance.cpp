#include "Instance.hpp"
#include "TestUtils.hpp"

Jkr::Instance::Instance(uint32_t inNoOfDescriptors, uint32_t inPoolSize)
    : mInstance()
    , mMessenger(mInstance)
    , mSurface(mInstance, TestWindow().GetHandle())
    , mPhysicalDevice(mInstance)
    , mQueueContext(mPhysicalDevice, mSurface)
    , mDevice(mPhysicalDevice, mQueueContext)
    , mGraphicsQueue(mQueueContext, mDevice)
    , mCommandPool(mDevice, mQueueContext)
    , mUtilCommandBuffer(mDevice, mCommandPool)
    , mDescriptorPool(mDevice, inNoOfDescriptors, inPoolSize)
    , mVmaAllocator(mInstance, mDevice)
{
    mThreadPool.SetThreadCount(std::thread::hardware_concurrency());
}
