#pragma once
#include "VulkanDevice.hpp"
#include "VulkanSurface.hpp"
#include "VulkanImage.hpp"
namespace ksai {

	enum class RenderPassContext
	{
		Default,
		OffScreen,
		MousePicking
	};

	class VulkanRenderPassBase
	{
	public:
		inline constexpr const auto& GetRenderPassHandle() const { return mRenderPass; }
	protected:
		vk::RenderPass mRenderPass;
	};
}

namespace ksai {
	template <RenderPassContext inRenderPassContext>
	class VulkanRenderPass : public VulkanRenderPassBase
	{
	public:
		VulkanRenderPass(const VulkanDevice& inDevice);
		VulkanRenderPass(const VulkanDevice& inDevice, const VulkanSurface& inSurface, const VulkanImage<ImageContext::DepthImage>& inDepthImage)
			requires(inRenderPassContext == RenderPassContext::Default);
		~VulkanRenderPass();
	private:
		const vk::Device& mDevice;
	};

	template<RenderPassContext inRenderPassContext>
	inline VulkanRenderPass<inRenderPassContext>::VulkanRenderPass(const VulkanDevice& inDevice, const VulkanSurface& inSurface, const VulkanImage<ImageContext::DepthImage>& inDepthImage)
		requires(inRenderPassContext == RenderPassContext::Default) : mDevice(inDevice.GetDeviceHandle())
	{
		vk::Format SurfaceSwapChainFormat = inSurface.GetSurfaceImageFormat();
		vk::Format DepthImageFormat = inDepthImage.GetImageFormat();
		vk::AttachmentDescription DepthAttachment = vk::AttachmentDescription(vk::AttachmentDescriptionFlags(), DepthImageFormat)
			.setInitialLayout(vk::ImageLayout::eUndefined)
			.setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal)
			.setLoadOp(vk::AttachmentLoadOp::eClear);
		vk::AttachmentReference DepthAttachmentRef(1, vk::ImageLayout::eDepthStencilAttachmentOptimal);

		vk::AttachmentDescription ColorAttachment = vk::AttachmentDescription(vk::AttachmentDescriptionFlags(), SurfaceSwapChainFormat)
			.setInitialLayout(vk::ImageLayout::eUndefined)
			.setFinalLayout(vk::ImageLayout::ePresentSrcKHR)
			.setLoadOp(vk::AttachmentLoadOp::eClear)
			.setStoreOp(vk::AttachmentStoreOp::eStore);
		vk::AttachmentReference ColorAttachmentRef(0, vk::ImageLayout::eColorAttachmentOptimal);

		vk::SubpassDescription SubPassDescription = vk::SubpassDescription(vk::SubpassDescriptionFlags(), vk::PipelineBindPoint::eGraphics)
			.setColorAttachmentCount(1)
			.setPColorAttachments(&ColorAttachmentRef)
			.setPDepthStencilAttachment(&DepthAttachmentRef);

		std::array<vk::SubpassDescription, 1> SubpassDescriptions;
		SubpassDescriptions.fill(SubPassDescription);
		std::array<vk::AttachmentDescription, 2> AttachmentDescriptions;
		AttachmentDescriptions = { ColorAttachment, DepthAttachment };

		vk::SubpassDependency SubpassDependency = vk::SubpassDependency()
			.setSrcSubpass(VK_SUBPASS_EXTERNAL)
			.setDstSubpass(0)
			.setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
			.setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
			.setSrcAccessMask(vk::AccessFlagBits::eNone)
			.setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite);

		vk::SubpassDependency ImplicitDependency = vk::SubpassDependency()
			.setSrcSubpass(0)
			.setDstSubpass(0)
			.setSrcStageMask(vk::PipelineStageFlagBits::eVertexShader)
			.setDstStageMask(vk::PipelineStageFlagBits::eFragmentShader)
			.setSrcAccessMask(vk::AccessFlagBits::eNone)
			.setDstAccessMask(vk::AccessFlagBits::eNone)
			.setDependencyFlags(vk::DependencyFlagBits::eByRegion);

		vk::SubpassDependency FinalDependency = vk::SubpassDependency()
			.setSrcSubpass(0)
			.setDstSubpass(VK_SUBPASS_EXTERNAL)
			.setSrcStageMask(vk::PipelineStageFlagBits::eLateFragmentTests)
			.setDstStageMask(vk::PipelineStageFlagBits::eLateFragmentTests)
			.setSrcAccessMask(vk::AccessFlagBits::eNone)
			.setDstAccessMask(vk::AccessFlagBits::eNone)
			.setDependencyFlags(vk::DependencyFlagBits::eByRegion);

		std::array<vk::SubpassDependency, 3> SubpassDependencies;
		SubpassDependencies = { SubpassDependency, ImplicitDependency, FinalDependency};

		mRenderPass = mDevice.createRenderPass(
			vk::RenderPassCreateInfo()
			.setAttachmentCount(AttachmentDescriptions.size())
			.setAttachments(AttachmentDescriptions)
			.setSubpassCount(SubpassDescriptions.size())
			.setSubpasses(SubpassDescriptions)
			.setDependencyCount(SubpassDependencies.size())
			.setDependencies(SubpassDependencies)
		);
	}

	template<RenderPassContext inRenderPassContext>
	inline VulkanRenderPass<inRenderPassContext>::VulkanRenderPass(const VulkanDevice& inDevice) : mDevice(inDevice)
	{
	}

	template<RenderPassContext inRenderPassContext>
	inline VulkanRenderPass<inRenderPassContext>::~VulkanRenderPass()
	{
		mDevice.destroyRenderPass(mRenderPass);
	}
}
