#pragma once
#include "Global/Standards.hpp"
#include "TestUtils.hpp"
#include <chrono>
#include <VulkanInstance.hpp>
#include <VulkanMessenger.hpp>
#include <VulkanSurface.hpp>
#include <VulkanPhysicalDevice.hpp>
#include <VulkanQueueContext.hpp>
#include <VulkanDevice.hpp>
#include <VulkanQueue.hpp>
#include <VulkanSwapChain.hpp>
#include <VulkanImage.hpp>
#include <VulkanRenderPass.hpp>
#include <VulkanFrameBuffer.hpp>
#include <VulkanFence.hpp>
#include <VulkanCommandPool.hpp>
#include <VulkanCommandBuffer.hpp>
#include <Vulkan/Pipeline/VulkanPipeline.hpp>
#include <Vulkan/Pipeline/VulkanDescriptorPool.hpp>
#include <Vulkan/Pipeline/VulkanDescriptorSet.hpp>
#include <Vulkan/Pipeline/VulkanDescriptorUpdateHandler.hpp>
#include <Vulkan/VulkanBufferVMA.hpp>
#include <Vulkan/VulkanImageVMA.hpp>
#include <Vulkan/VulkanSampler.hpp>


namespace Jkr {
	using namespace ksai;
	static Timer time;
	class Instance
	{
	public:
		Instance();
		GETTER& GetVulkanInstance() const { return mInstance; }
		GETTER& GetPhysicalDevice() const { return mPhysicalDevice; }
		GETTER& GetQueueContext() const { return mQueueContext; }
		GETTER& GetDevice() const { return mDevice; }

		GETTER& GetGraphicsQueue() const { return mGraphicsQueue; }
        GETTER &GetCommandPool() const { return mCommandPool; }
        // GETTER& GetCommandBuffers() const { return mCommandBuffers; }
        GETTER &GetUtilCommandBuffer() const { return mUtilCommandBuffer; }
        GETTER& GetDescriptorPool() const { return mDescriptorPool; }

		GETTER& GetVMA() const { return mVmaAllocator; }
	private:
		static const int mMaxFramesInFlight = 2;
		static const int DescriptorPoolSize = 10000;
	private:
		const VulkanInstance mInstance;
		const VulkanMessenger mMessenger;
		const VulkanSurface mSurface;
		const VulkanPhysicalDevice mPhysicalDevice;
		const VulkanQueueContext mQueueContext;
		const VulkanDevice mDevice;
		const VulkanQueue<QueueContext::Graphics> mGraphicsQueue;
        const VulkanCommandPool mCommandPool;
        // const std::array<VulkanCommandBuffer, mMaxFramesInFlight> mCommandBuffers;
        const VulkanCommandBuffer mUtilCommandBuffer;
        const VulkanDescriptorPool<DescriptorPoolSize> mDescriptorPool;
		const VulkanVMA mVmaAllocator;
	};
}
