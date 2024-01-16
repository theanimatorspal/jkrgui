#include "VulkanQueueContext.hpp"
#include "VulkanSemaphore.hpp"
#include "VulkanCommandBuffer.hpp"
#include "VulkanFence.hpp"
#include <cglm/cglm.h>
class VulkanSemaphore;
class VulkanFence;
class VulkanCommandBuffer;
using namespace ksai;

VulkanQueueContext::VulkanQueueContext(const VulkanPhysicalDevice & inDevice, const VulkanSurface & inSurface) 
	: mPhysicalDevice(inDevice.GetPhysicalDeviceHandle()), mSurface(inSurface.GetSurfaceHandle()), mWindow(inSurface.GetWindowHandle())
{
	mQueueFamilyProperties = mPhysicalDevice.getQueueFamilyProperties();
	auto PropertyIterator = std::find_if(mQueueFamilyProperties.begin(), mQueueFamilyProperties.end(),
		[](vk::QueueFamilyProperties const& qfp)
		{
			return (qfp.queueFlags & vk::QueueFlagBits::eGraphics);
		}
	);

	mGraphicsQueueIndex = std::distance(mQueueFamilyProperties.begin(), PropertyIterator);
	assert(mGraphicsQueueIndex < mQueueFamilyProperties.size());

#define ForEachFamilyProperty(i) for(int i = 0; i < mQueueFamilyProperties.size(); i++)

	size_t presentQueueFamilyIndex = mPhysicalDevice
		.getSurfaceSupportKHR(mGraphicsQueueIndex, mSurface)
		? mGraphicsQueueIndex : mQueueFamilyProperties.size();
	mPresentQueueIndex = presentQueueFamilyIndex;

	if (presentQueueFamilyIndex == mQueueFamilyProperties.size())
	{
		ForEachFamilyProperty(i)
		{
			bool isQueueSupportedForPresentAndGraphics = (mQueueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics) && mPhysicalDevice.getSurfaceSupportKHR(mGraphicsQueueIndex, mSurface);
			if (isQueueSupportedForPresentAndGraphics)
			{
				mGraphicsQueueIndex = i;
				mPresentQueueIndex = i;
				break;
			}
		}

		bool isPresentQueueDifferent = (mPresentQueueIndex == mQueueFamilyProperties.size());
		if (isPresentQueueDifferent)
		{
			ForEachFamilyProperty(i)
			{
				bool SupportsPresentation = mPhysicalDevice.getSurfaceSupportKHR(i, mSurface);
				if (SupportsPresentation)
				{
					mPresentQueueIndex = i;
					break;
				}
			}
		}
	}
	bool NoQueueForGraphicsOrPresent = (mGraphicsQueueIndex == mQueueFamilyProperties.size()) || (mPresentQueueIndex == mQueueFamilyProperties.size());
	if (NoQueueForGraphicsOrPresent)
	{
		throw std::runtime_error("Error : Count not find a suitable queue for graphics or present");
	}
}


VulkanQueueContext::~VulkanQueueContext()
{
}

