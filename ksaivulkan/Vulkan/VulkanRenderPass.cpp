#include "VulkanRenderPass.hpp"
using namespace ksai;
template <RenderPassContext T>
using vr = VulkanRenderPass<T>;

template <>
VulkanRenderPass<RenderPassContext::Default>::VulkanRenderPass(const VulkanDevice& inDevice, const VulkanSurface& inSurface, const VulkanImage<ImageContext::DepthImage>& inDepthImage)
	: mDevice(inDevice.GetDeviceHandle())
{
	vk::Format SurfaceSwapChainFormat = inSurface.GetSurfaceImageFormat();
	vk::Format DepthImageFormat = inDepthImage.GetImageFormat();
	vk::AttachmentDescription DepthAttachment = vk::AttachmentDescription(vk::AttachmentDescriptionFlags(), DepthImageFormat)
		.setInitialLayout(vk::ImageLayout::eUndefined)
		.setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal)
		.setLoadOp(vk::AttachmentLoadOp::eClear)
		.setStoreOp(vk::AttachmentStoreOp::eDontCare)
		.setSamples(vk::SampleCountFlagBits::e1);

	vk::AttachmentReference DepthAttachmentRef(1, vk::ImageLayout::eDepthStencilAttachmentOptimal);

	vk::AttachmentDescription ColorAttachment = vk::AttachmentDescription(vk::AttachmentDescriptionFlags(), SurfaceSwapChainFormat)
		.setInitialLayout(vk::ImageLayout::eUndefined)
		.setFinalLayout(vk::ImageLayout::ePresentSrcKHR)
		.setLoadOp(vk::AttachmentLoadOp::eClear)
		.setStoreOp(vk::AttachmentStoreOp::eStore)
		.setSamples(vk::SampleCountFlagBits::e1);
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
		.setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests)
		.setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests)
		.setSrcAccessMask(vk::AccessFlagBits::eNone)
		.setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite);

	vk::SubpassDependency SelfDependency = vk::SubpassDependency()
		.setSrcSubpass(0)
		.setDstSubpass(0)
		.setSrcStageMask(vk::PipelineStageFlagBits::eLateFragmentTests)
		.setDstStageMask(vk::PipelineStageFlagBits::eLateFragmentTests)
		.setSrcAccessMask(vk::AccessFlagBits::eNone)
		.setDstAccessMask(vk::AccessFlagBits::eNone)
		.setDependencyFlags(vk::DependencyFlagBits::eByRegion);

	std::array<vk::SubpassDependency, 2> SubpassDependencies;
	SubpassDependencies = { SubpassDependency,
		SelfDependency };

	mRenderPass = mDevice.createRenderPass(
		vk::RenderPassCreateInfo()
		.setAttachmentCount(AttachmentDescriptions.size())
		.setAttachments(AttachmentDescriptions)
		.setSubpassCount(SubpassDescriptions.size())
		.setSubpasses(SubpassDescriptions)
		.setDependencyCount(SubpassDependencies.size())
		.setDependencies(SubpassDependencies));
}

template <>
VulkanRenderPass<RenderPassContext::OffScreen>::VulkanRenderPass(const VulkanDevice& inDevice, const VulkanSurface& inSurface, const VulkanImage<ImageContext::DepthImage>& inDepthImage)
	: mDevice(inDevice.GetDeviceHandle())
{
	vk::Format SurfaceSwapChainFormat = inSurface.GetSurfaceImageFormat();
	vk::Format DepthImageFormat = inDepthImage.GetImageFormat();
	vk::AttachmentDescription DepthAttachment = vk::AttachmentDescription(vk::AttachmentDescriptionFlags(), DepthImageFormat)
		.setInitialLayout(vk::ImageLayout::eUndefined)
		.setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal)
		.setLoadOp(vk::AttachmentLoadOp::eClear)
		.setStoreOp(vk::AttachmentStoreOp::eDontCare);
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
		.setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests)
		.setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests)
		.setSrcAccessMask(vk::AccessFlagBits::eNone)
		.setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite);

	vk::SubpassDependency SelfDependency = vk::SubpassDependency()
		.setSrcSubpass(0)
		.setDstSubpass(0)
		.setSrcStageMask(vk::PipelineStageFlagBits::eLateFragmentTests)
		.setDstStageMask(vk::PipelineStageFlagBits::eLateFragmentTests)
		.setSrcAccessMask(vk::AccessFlagBits::eNone)
		.setDstAccessMask(vk::AccessFlagBits::eNone)
		.setDependencyFlags(vk::DependencyFlagBits::eByRegion);

	std::array<vk::SubpassDependency, 2> SubpassDependencies;
	SubpassDependencies = { SubpassDependency,
		SelfDependency };

	mRenderPass = mDevice.createRenderPass(
		vk::RenderPassCreateInfo()
		.setAttachmentCount(AttachmentDescriptions.size())
		.setAttachments(AttachmentDescriptions)
		.setSubpassCount(SubpassDescriptions.size())
		.setSubpasses(SubpassDescriptions)
		.setDependencyCount(SubpassDependencies.size())
		.setDependencies(SubpassDependencies));
}
