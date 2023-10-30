#pragma once
#include "VulkanDevice.hpp"
#include "VulkanCommandPool.hpp"
#include "VulkanFrameBuffer.hpp"
#include <Pipeline/VulkanPipelineLayout.hpp>
namespace ksai {

	class VulkanCommandBuffer
	{
	public:
		VulkanCommandBuffer(const VulkanDevice& inDevice, const VulkanCommandPool& inPool);
		~VulkanCommandBuffer();
		GETTER& GetCommandBufferHandle() const { return mBuffer; }
		void Begin() const { mBuffer.begin(vk::CommandBufferBeginInfo()); }
		void Reset() const { mBuffer.reset(); }
		void End() const { mBuffer.end(); }
		template <RenderPassContext inContext, typename FrameBuffer>
		void BeginRenderPass(const VulkanRenderPass<inContext>& inRenderPass, const VulkanSurface& inSurface, const FrameBuffer& inFrameBuffer, std::array<float, 5> inClearValue) const;
		void EndRenderPass() const ;

		template<class T>
		void PushConstants(const VulkanPipelineLayoutBase& inPipelineLayout, const vk::ArrayProxy<const T> inValues) const
		{
				mBuffer.pushConstants<T>(inPipelineLayout.GetPipelineLayoutHandle(), vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment | vk::ShaderStageFlagBits::eCompute, 0, inValues);
		}
	private:
		const vk::Device& mDevice;
		vk::CommandBuffer mBuffer;
	};

	template <RenderPassContext inContext, typename FrameBuffer>
	inline void VulkanCommandBuffer::BeginRenderPass(const VulkanRenderPass<inContext>& inRenderPass, const VulkanSurface& inSurface, const FrameBuffer& inFrameBuffer, std::array<float, 5> inClearValue) const
	{
	}

	using FrameBufferType = VulkanFrameBuffer<RenderPassContext::Default, 2, VulkanImage<ImageContext::ExternalHandled>, VulkanImage<ImageContext::DepthImage>>;

	template<>
	inline void VulkanCommandBuffer::BeginRenderPass<RenderPassContext::Default, FrameBufferType>(const VulkanRenderPass<RenderPassContext::Default>& inRenderPass, const VulkanSurface& inSurface, const FrameBufferType& inFrameBuffer, std::array<float, 5> inClearValue) const
	{
		const auto& Extent = inSurface.GetExtent();
		std::array<vk::ClearValue, 2> ClearValues;
		ClearValues[0].color = vk::ClearColorValue(inClearValue[0], inClearValue[1], inClearValue[2], inClearValue[3]);
		ClearValues[1].depthStencil = vk::ClearDepthStencilValue(inClearValue[4]);
		auto RenderPassBeginInfo = vk::RenderPassBeginInfo()
			.setRenderPass(inRenderPass.GetRenderPassHandle())
			.setFramebuffer(inFrameBuffer.GetFrameBufferHandle())
			.setRenderArea(vk::Rect2D(vk::Offset2D(0, 0), Extent))
			.setClearValueCount(ClearValues.size())
			.setClearValues(ClearValues);
		mBuffer.beginRenderPass(RenderPassBeginInfo, vk::SubpassContents::eInline);
		mBuffer.setViewport(0, vk::Viewport(0.0f, 0.0f, static_cast<float>(Extent.width), static_cast<float>(Extent.height), 0.0f, 1.0f));
		mBuffer.setScissor(0, vk::Rect2D(vk::Offset2D(0, 0), Extent));
	}
}
