#pragma once
#include "VulkanDevice.hpp"
#include "VulkanQueueContext.hpp"
#include "VulkanSemaphore.hpp"
#include "VulkanFence.hpp"

namespace ksai {
	class VulkanQueueBase
	{
	public:
		VulkanQueueBase(const VulkanQueueContext& inQueueContext, const VulkanDevice& inDevice);
		void Wait() const { mQueue.waitIdle(); }
	protected:
		const vk::Device& mDevice;
		const VulkanQueueContext& mQueueContext;
		vk::Queue mQueue;
	};
}

namespace ksai {
	class VulkanCommandBuffer;
	template<size_t inMaxFramesFlight> class VulkanSwapChain;

	template <QueueContext inContext>
	class VulkanQueue : public VulkanQueueBase
	{
	public:
		VulkanQueue(const VulkanQueueContext& inQueueContext, const VulkanDevice& inDevice);
		~VulkanQueue();

		template<SubmitContext inSubmitContext>
		void Submit(const VulkanSemaphore& inImageAvailableSemaphore, const VulkanSemaphore& inRenderFinishedSemaphore, const VulkanFence& inFlightFence, const VulkanCommandBuffer& inCommandBuffer)
			const requires (inSubmitContext == SubmitContext::ColorAttachment);

		template<SubmitContext inSubmitContext>
		void Submit(const VulkanCommandBuffer& inCommandBuffer) const requires (inSubmitContext == SubmitContext::SingleTime);

		template<size_t inMaxFramesInFlight, SubmitContext inSubmitContext>
		uint32_t Present(const VulkanSwapChain<inMaxFramesInFlight>& inSwapChain, const VulkanSemaphore& inRenderFinishedSemaphore, uint32_t inImageIndex) const;

	};
}

namespace ksai {

	template<QueueContext inContext>
	inline VulkanQueue<inContext>::VulkanQueue(const VulkanQueueContext& inQueueContext, const VulkanDevice& inDevice)
		: VulkanQueueBase(inQueueContext, inDevice)
	{
	}

	template<QueueContext inContext>
	inline VulkanQueue<inContext>::~VulkanQueue()
	{
	}


	template<>
	inline VulkanQueue<QueueContext::Graphics>::VulkanQueue(const VulkanQueueContext& inQueueContext, const VulkanDevice& inDevice) :
		VulkanQueueBase(inQueueContext, inDevice)
	{
		mQueue = mDevice.getQueue(inQueueContext.GetGraphicsQueueFamilyIndex(), 0);
	}

	template<QueueContext inContext>
	template<SubmitContext inSubmitContext>
	inline void VulkanQueue<inContext>::Submit(const VulkanSemaphore& inImageAvailableSemaphore, const VulkanSemaphore& inRenderFinishedSemaphore, const VulkanFence& inFlightFence, const VulkanCommandBuffer& inCommandBuffer)
		const requires (inSubmitContext == SubmitContext::ColorAttachment)
	{
		vk::PipelineStageFlags WaitFlags(vk::PipelineStageFlagBits::eColorAttachmentOutput);
		vk::SubmitInfo info;
		info = vk::SubmitInfo(inImageAvailableSemaphore.GetSemaphoreHandle(), WaitFlags, inCommandBuffer.GetCommandBufferHandle(), inRenderFinishedSemaphore.GetSemaphoreHandle());
		mQueue.submit(info);
		mQueue.waitIdle();
	}

	template<QueueContext inContext>
	template<SubmitContext inSubmitContext>
	inline void VulkanQueue<inContext>::Submit(const VulkanCommandBuffer& inCommandBuffer)
		const requires (inSubmitContext == SubmitContext::SingleTime)
	{
		vk::SubmitInfo SubmitInfo({}, {}, inCommandBuffer.GetCommandBufferHandle(), {});
		mQueue.submit(SubmitInfo);
		mQueue.waitIdle();
	}

	template<QueueContext inContext>
	template<size_t inMaxFramesInFlight, SubmitContext inSubmitContext>
	inline uint32_t VulkanQueue<inContext>::Present(const VulkanSwapChain<inMaxFramesInFlight>& inSwapChain, const VulkanSemaphore& inRenderFinishedSemaphore, uint32_t inImageIndex) const
	{
		return 0;
	}

	template<>
	template<size_t inMaxFramesInFlight, SubmitContext inSubmitContext>
	inline uint32_t VulkanQueue<QueueContext::Graphics>::Present(const VulkanSwapChain<inMaxFramesInFlight>& inSwapChain, const VulkanSemaphore& inRenderFinishedSemaphore, uint32_t inImageIndex) const
	{
		auto PresentInfoKHR = vk::PresentInfoKHR({}, inSwapChain.GetSwapChainHandle(), inImageIndex)
			.setWaitSemaphoreCount(1)
			.setWaitSemaphores(inRenderFinishedSemaphore.GetSemaphoreHandle());
		vk::Result Result = mQueue.presentKHR(PresentInfoKHR);
		return static_cast<uint32_t>(Result);
	}
}
