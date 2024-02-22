#include "Instance.hpp"
#include "TestUtils.hpp"

Jkr::Instance::Instance(uint32_t inNoOfDescriptors, uint32_t inPoolSize)
    : mInstance()
    , mMessenger(mInstance)
    , mPhysicalDevice(mInstance)
    , mQueueContext(mInstance, mPhysicalDevice)
    , mDevice(mPhysicalDevice, mQueueContext)
    , mGraphicsQueue(mQueueContext, mDevice)
    , mCommandPool(mDevice, mQueueContext)
    , mUtilCommandBuffer(mDevice, mCommandPool)
    , mDescriptorPool(mDevice, inNoOfDescriptors, inPoolSize)
    , mVmaAllocator(mInstance, mDevice)
{
    mThreadPool.SetThreadCount(std::thread::hardware_concurrency() - 1);
    SpirvHelper::Init();
}
