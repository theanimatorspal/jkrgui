#pragma once
#include <variant>
#include "VulkanSwapChain.hpp"
#include "VulkanImage.hpp"
#include "VulkanRenderPass.hpp"

namespace ksai {

	template <RenderPassContext inRenderPassContext = RenderPassContext::Default, size_t NoOfAttachements = 2, typename ... T>
	class VulkanFrameBuffer
	{
	public:
		VulkanFrameBuffer(const VulkanDevice& inDevice, const VulkanRenderPass<inRenderPassContext>& inRenderPass, const T & ... inT);
		~VulkanFrameBuffer();
		void ExplicitlyDestroy();
		VulkanFrameBuffer& operator=(VulkanFrameBuffer&& Other)
		{
			ExplicitlyDestroy();
			mBuffer = std::move(Other.mBuffer);
			mExtent = std::move(Other.mExtent);
			Other.mBuffer = nullptr;
			return *this;
		}
		inline constexpr const auto& GetFrameBufferHandle() const { return mBuffer; }
	private:
		const vk::Device& mDevice;
		std::vector<vk::ImageView> Attachments;
		vk::Framebuffer mBuffer;
		vk::Extent2D mExtent;
	};

	template<RenderPassContext inRenderPassContext, size_t NoOfAttachements, typename ...T>
	inline VulkanFrameBuffer<inRenderPassContext, NoOfAttachements, T...>::VulkanFrameBuffer(const VulkanDevice& inDevice, const VulkanRenderPass<inRenderPassContext>& inRenderPass, const T & ...inT) : mDevice(inDevice.GetDeviceHandle())
	{
		Attachments.reserve(NoOfAttachements);

		([&] {
			Attachments.push_back(inT.GetImageViewHandle());
			mExtent = inT.GetImageExtent();
			} (), ...);

		auto FrameBufferCreateInfo = vk::FramebufferCreateInfo(
			vk::FramebufferCreateFlags(),
			inRenderPass.GetRenderPassHandle(),
			Attachments,
			mExtent.width,
			mExtent.height,
			1
		);
		mBuffer = inDevice.GetDeviceHandle().createFramebuffer(FrameBufferCreateInfo);
	}

	template<RenderPassContext inRenderPassContext, size_t NoOfAttachements, typename ...T>
	inline VulkanFrameBuffer<inRenderPassContext, NoOfAttachements, T...>::~VulkanFrameBuffer()
	{
		ExplicitlyDestroy();
	}

	template<RenderPassContext inRenderPassContext, size_t NoOfAttachements, typename ...T>
	inline void VulkanFrameBuffer<inRenderPassContext, NoOfAttachements, T...>::ExplicitlyDestroy()
	{
		if (mBuffer)
		{
			mDevice.waitIdle();
			mDevice.destroyFramebuffer(mBuffer);
		}
		mBuffer = nullptr;
	}
}
