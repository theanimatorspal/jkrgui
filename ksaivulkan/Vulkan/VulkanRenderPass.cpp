#include "VulkanRenderPass.hpp"
using namespace ksai;
template <RenderPassContext T>
using vr = VulkanRenderPass<T>;

// TODO Uniformize Parameters in these functions
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
		.setPColorAttachments(&ColorAttachmentRef) // TODO Remove this Addressing
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
VulkanRenderPass<RenderPassContext::MSAA>::VulkanRenderPass(const VulkanDevice& inDevice, const VulkanSurface& inSurface, const VulkanImage<ImageContext::ColorAttach>& inColorImageTarget, const VulkanImage<ImageContext::DepthImage>& inDepthImage, vk::SampleCountFlagBits inMSAASamples)
	: mDevice(inDevice.GetDeviceHandle())
{
	vk::Format SurfaceSwapChainFormat = inSurface.GetSurfaceImageFormat();
	vk::Format DepthImageFormat = inDepthImage.GetImageFormat();
	vk::AttachmentDescription DepthAttachment = vk::AttachmentDescription(vk::AttachmentDescriptionFlags(), DepthImageFormat)
		.setInitialLayout(vk::ImageLayout::eUndefined)
		.setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal)
		.setLoadOp(vk::AttachmentLoadOp::eClear)
		.setStoreOp(vk::AttachmentStoreOp::eDontCare)
		.setSamples(inMSAASamples);
	vk::AttachmentReference DepthAttachmentRef(1, vk::ImageLayout::eDepthStencilAttachmentOptimal);

	vk::AttachmentDescription ColorAttachment = vk::AttachmentDescription(vk::AttachmentDescriptionFlags(), inColorImageTarget.GetImageFormat())
		.setInitialLayout(vk::ImageLayout::eUndefined)
		// .setFinalLayout(vk::ImageLayout::ePresentSrcKHR)
		.setFinalLayout(vk::ImageLayout::eColorAttachmentOptimal) // For multi sampled images cannot be presented directly
		.setLoadOp(vk::AttachmentLoadOp::eClear)
		.setStoreOp(vk::AttachmentStoreOp::eStore)
		.setSamples(inMSAASamples);
	vk::AttachmentReference ColorAttachmentRef(0, vk::ImageLayout::eColorAttachmentOptimal);

	vk::AttachmentDescription ColorAttachmentResolve = vk::AttachmentDescription(vk::AttachmentDescriptionFlags(), inColorImageTarget.GetImageFormat())
		.setLoadOp(vk::AttachmentLoadOp::eDontCare)
		.setStoreOp(vk::AttachmentStoreOp::eStore)
		.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
		.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
		.setInitialLayout(vk::ImageLayout::eUndefined)
		.setFinalLayout(vk::ImageLayout::ePresentSrcKHR);
	vk::AttachmentReference ColorAttachmentResolveRef(2, vk::ImageLayout::eColorAttachmentOptimal); // Subpass ma hune layout expected

	vk::SubpassDescription SubPassDescription = vk::SubpassDescription(vk::SubpassDescriptionFlags(), vk::PipelineBindPoint::eGraphics)
		.setColorAttachmentCount(1)
		.setPColorAttachments(&ColorAttachmentRef) // TODO Remove this addressing
		.setPDepthStencilAttachment(&DepthAttachmentRef)
		.setResolveAttachments(ColorAttachmentResolveRef);

	std::array<vk::SubpassDescription, 1> SubpassDescriptions;
	SubpassDescriptions.fill(SubPassDescription);
	std::array<vk::AttachmentDescription, 3> AttachmentDescriptions;
	AttachmentDescriptions = { ColorAttachment, DepthAttachment, ColorAttachmentResolve };

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
