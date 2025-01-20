#include "VulkanRenderPass.hpp"

#include "VulkanImage.hpp"
#include "vulkan/vulkan_enums.hpp"

using namespace ksai;

template <>
VulkanRenderPass<RenderPassContext::Default>::VulkanRenderPass(VulkanDevice &inDevice,
                                                               VulkanSurface &inSurface,
                                                               VulkanImage &inDepthImage)
    : mDevice(&inDevice.GetDeviceHandle()) {
    mColorAttachmentCount             = 1;
    vk::Format SurfaceSwapChainFormat = inSurface.GetSurfaceImageFormat();
    vk::Format DepthImageFormat       = inDepthImage.GetImageFormat();
    vk::AttachmentDescription DepthAttachment =
         vk::AttachmentDescription(vk::AttachmentDescriptionFlags(), DepthImageFormat)
              .setInitialLayout(vk::ImageLayout::eUndefined)
              .setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal)
              .setLoadOp(vk::AttachmentLoadOp::eClear)
              .setStoreOp(vk::AttachmentStoreOp::eDontCare)
              .setSamples(vk::SampleCountFlagBits::e1);

    vk::AttachmentReference DepthAttachmentRef(1, vk::ImageLayout::eDepthStencilAttachmentOptimal);

    vk::AttachmentDescription ColorAttachment =
         vk::AttachmentDescription(vk::AttachmentDescriptionFlags(), SurfaceSwapChainFormat)
              .setInitialLayout(vk::ImageLayout::eUndefined)
              .setFinalLayout(vk::ImageLayout::ePresentSrcKHR)
              .setLoadOp(vk::AttachmentLoadOp::eClear)
              .setStoreOp(vk::AttachmentStoreOp::eStore)
              .setSamples(vk::SampleCountFlagBits::e1);
    vk::AttachmentReference ColorAttachmentRef(0, vk::ImageLayout::eColorAttachmentOptimal);

    vk::SubpassDescription SubPassDescription =
         vk::SubpassDescription(vk::SubpassDescriptionFlags(), vk::PipelineBindPoint::eGraphics)
              .setColorAttachmentCount(1)
              .setPColorAttachments(&ColorAttachmentRef) // TODO Remove this Addressing
              .setPDepthStencilAttachment(&DepthAttachmentRef);

    std::array<vk::SubpassDescription, 1> SubpassDescriptions;
    SubpassDescriptions.fill(SubPassDescription);
    std::array<vk::AttachmentDescription, 2> AttachmentDescriptions;
    AttachmentDescriptions = {ColorAttachment, DepthAttachment};

    vk::SubpassDependency SubpassDependency =
         vk::SubpassDependency()
              .setSrcSubpass(VK_SUBPASS_EXTERNAL)
              .setDstSubpass(0)
              .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput |
                               vk::PipelineStageFlagBits::eEarlyFragmentTests)
              .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput |
                               vk::PipelineStageFlagBits::eEarlyFragmentTests)
              .setSrcAccessMask(vk::AccessFlagBits::eNone)
              .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite |
                                vk::AccessFlagBits::eDepthStencilAttachmentWrite);

    vk::SubpassDependency SelfDependency =
         vk::SubpassDependency()
              .setSrcSubpass(0)
              .setDstSubpass(0)
              .setSrcStageMask(vk::PipelineStageFlagBits::eLateFragmentTests)
              .setDstStageMask(vk::PipelineStageFlagBits::eLateFragmentTests)
              .setSrcAccessMask(vk::AccessFlagBits::eNone)
              .setDstAccessMask(vk::AccessFlagBits::eNone)
              .setDependencyFlags(vk::DependencyFlagBits::eByRegion);

    std::array<vk::SubpassDependency, 2> SubpassDependencies;
    SubpassDependencies = {SubpassDependency, SelfDependency};

    mRenderPass         = mDevice->createRenderPass(vk::RenderPassCreateInfo()
                                                 .setAttachmentCount(AttachmentDescriptions.size())
                                                 .setAttachments(AttachmentDescriptions)
                                                 .setSubpassCount(SubpassDescriptions.size())
                                                 .setSubpasses(SubpassDescriptions)
                                                 .setDependencyCount(SubpassDependencies.size())
                                                 .setDependencies(SubpassDependencies));
}

template <>
VulkanRenderPass<RenderPassContext::Shadow>::VulkanRenderPass(VulkanDevice &inDevice,
                                                              VulkanImageBase &inDepthImage)
    : mDevice(&inDevice.GetDeviceHandle()) {
    mColorAttachmentCount       = 0;
    vk::Format DepthImageFormat = inDepthImage.GetImageFormat();
    vk::AttachmentDescription DepthAttachment =
         vk::AttachmentDescription(vk::AttachmentDescriptionFlags(), DepthImageFormat)
              .setInitialLayout(vk::ImageLayout::eUndefined)
              .setFinalLayout(vk::ImageLayout::eGeneral)
              .setLoadOp(vk::AttachmentLoadOp::eClear)
              .setStoreOp(vk::AttachmentStoreOp::eStore);
    vk::AttachmentReference DepthAttachmentRef(0, vk::ImageLayout::eDepthStencilAttachmentOptimal);
    vk::SubpassDescription SubPassDescription =
         vk::SubpassDescription(vk::SubpassDescriptionFlags(), vk::PipelineBindPoint::eGraphics)
              .setColorAttachmentCount(0)
              .setPDepthStencilAttachment(&DepthAttachmentRef);

    std::vector<vk::SubpassDescription> SubpassDescriptions       = {SubPassDescription};
    std::vector<vk::AttachmentDescription> AttachmentDescriptions = {DepthAttachment};

    vk::SubpassDependency SubpassDependency1 =
         vk::SubpassDependency()
              .setSrcSubpass(VK_SUBPASS_EXTERNAL)
              .setDstSubpass(0)
              .setSrcStageMask(vk::PipelineStageFlagBits::eFragmentShader)
              .setDstStageMask(vk::PipelineStageFlagBits::eEarlyFragmentTests)
              .setSrcAccessMask(vk::AccessFlagBits::eShaderRead)
              .setDstAccessMask(vk::AccessFlagBits::eDepthStencilAttachmentWrite)
              .setDependencyFlags(vk::DependencyFlagBits::eByRegion);

    vk::SubpassDependency SubpassDependency2 =
         vk::SubpassDependency()
              .setSrcSubpass(0)
              .setDstSubpass(VK_SUBPASS_EXTERNAL)
              .setSrcStageMask(vk::PipelineStageFlagBits::eLateFragmentTests)
              .setDstStageMask(vk::PipelineStageFlagBits::eFragmentShader)
              .setSrcAccessMask(vk::AccessFlagBits::eDepthStencilAttachmentWrite)
              .setDstAccessMask(vk::AccessFlagBits::eShaderRead)
              .setDependencyFlags(vk::DependencyFlagBits::eByRegion);
    std::vector<vk::SubpassDependency> SubpassDependencies = {SubpassDependency1,
                                                              SubpassDependency2};
    mRenderPass = mDevice->createRenderPass(vk::RenderPassCreateInfo()
                                                 .setAttachmentCount(AttachmentDescriptions.size())
                                                 .setAttachments(AttachmentDescriptions)
                                                 .setSubpasses(SubpassDescriptions)
                                                 .setDependencies(SubpassDependencies));
}

template <>
VulkanRenderPass<RenderPassContext::MSAA>::VulkanRenderPass(VulkanDevice &inDevice,
                                                            vk::Format inImageFormat,
                                                            VulkanImage &inColorImageTarget,
                                                            VulkanImage &inDepthImage,
                                                            vk::SampleCountFlagBits inMSAASamples)
    : mDevice(&inDevice.GetDeviceHandle()) {
    mColorAttachmentCount             = 1;
    vk::Format SurfaceSwapChainFormat = inImageFormat;
    vk::Format DepthImageFormat       = inDepthImage.GetImageFormat();

    vk::AttachmentDescription ColorAttachment =
         vk::AttachmentDescription(vk::AttachmentDescriptionFlags(),
                                   inColorImageTarget.GetImageFormat())
              .setInitialLayout(vk::ImageLayout::eUndefined)
              .setFinalLayout(
                   vk::ImageLayout::eColorAttachmentOptimal) // For multi sampled images cannot be
                                                             // presented directly
              .setLoadOp(vk::AttachmentLoadOp::eClear)
              .setStoreOp(vk::AttachmentStoreOp::eStore)
              .setSamples(inMSAASamples);
    vk::AttachmentReference ColorAttachmentRef(0, vk::ImageLayout::eColorAttachmentOptimal);

    vk::AttachmentDescription DepthAttachment =
         vk::AttachmentDescription(vk::AttachmentDescriptionFlags(), DepthImageFormat)
              .setInitialLayout(vk::ImageLayout::eUndefined)
              .setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal)
              .setLoadOp(vk::AttachmentLoadOp::eClear)
              .setStoreOp(vk::AttachmentStoreOp::eDontCare)
              .setSamples(inMSAASamples);
    vk::AttachmentReference DepthAttachmentRef(1, vk::ImageLayout::eDepthStencilAttachmentOptimal);

    vk::AttachmentDescription ColorAttachmentResolve =
         vk::AttachmentDescription(vk::AttachmentDescriptionFlags(),
                                   inColorImageTarget.GetImageFormat())
              .setInitialLayout(vk::ImageLayout::eUndefined)
              .setFinalLayout(vk::ImageLayout::eTransferSrcOptimal) // Offscreen MSAA
              .setLoadOp(vk::AttachmentLoadOp::eDontCare)
              .setStoreOp(vk::AttachmentStoreOp::eStore)
              .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
              .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare);
    vk::AttachmentReference ColorAttachmentResolveRef(
         2, vk::ImageLayout::eColorAttachmentOptimal); // Subpass ma hune layout expected

    vk::SubpassDescription SubPassDescription =
         vk::SubpassDescription(vk::SubpassDescriptionFlags(), vk::PipelineBindPoint::eGraphics)
              .setColorAttachmentCount(1)
              .setColorAttachments(ColorAttachmentRef)
              .setPDepthStencilAttachment(&DepthAttachmentRef)
              .setResolveAttachments(ColorAttachmentResolveRef);

    std::vector<vk::SubpassDescription> SubpassDescriptions       = {SubPassDescription};
    std::vector<vk::AttachmentDescription> AttachmentDescriptions = {
         ColorAttachment, DepthAttachment, ColorAttachmentResolve};

    std::vector<vk::SubpassDependency> SubpassDependencies = {
         // TODO Improve this
         vk::SubpassDependency()
              .setSrcSubpass(VK_SUBPASS_EXTERNAL)
              .setDstSubpass(0)
              .setSrcStageMask(vk::PipelineStageFlagBits::eFragmentShader |
                               vk::PipelineStageFlagBits::eTransfer |
                               vk::PipelineStageFlagBits::eAllCommands)
              .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput |
                               vk::PipelineStageFlagBits::eEarlyFragmentTests)
              .setSrcAccessMask(vk::AccessFlagBits::eMemoryRead)
              .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentRead |
                                vk::AccessFlagBits::eColorAttachmentWrite |
                                vk::AccessFlagBits::eDepthStencilAttachmentWrite)
              .setDependencyFlags(vk::DependencyFlagBits::eByRegion),
         vk::SubpassDependency()
              .setSrcSubpass(0)
              .setDstSubpass(0)
              .setSrcStageMask(vk::PipelineStageFlagBits::eLateFragmentTests)
              .setDstStageMask(vk::PipelineStageFlagBits::eLateFragmentTests)
              .setSrcAccessMask(vk::AccessFlagBits::eNone)
              .setDstAccessMask(vk::AccessFlagBits::eNone)
              .setDependencyFlags(vk::DependencyFlagBits::eByRegion),
         vk::SubpassDependency()
              .setSrcSubpass(0)
              .setDstSubpass(VK_SUBPASS_EXTERNAL)
              .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput |
                               vk::PipelineStageFlagBits::eLateFragmentTests)
              .setDstStageMask(vk::PipelineStageFlagBits::eFragmentShader |
                               vk::PipelineStageFlagBits::eTransfer)
              .setSrcAccessMask(vk::AccessFlagBits::eColorAttachmentRead |
                                vk::AccessFlagBits::eColorAttachmentWrite |
                                vk::AccessFlagBits::eDepthStencilAttachmentWrite)
              .setDstAccessMask(vk::AccessFlagBits::eMemoryRead)
              .setDependencyFlags(vk::DependencyFlagBits::eByRegion),
    };

    mRenderPass = mDevice->createRenderPass(vk::RenderPassCreateInfo()
                                                 .setAttachmentCount(AttachmentDescriptions.size())
                                                 .setAttachments(AttachmentDescriptions)
                                                 .setSubpassCount(SubpassDescriptions.size())
                                                 .setSubpasses(SubpassDescriptions)
                                                 .setDependencyCount(SubpassDependencies.size())
                                                 .setDependencies(SubpassDependencies));
}

// TODO change currentImageLayouts of aboves as well

template <>
VulkanRenderPass<RenderPassContext::SingleColorAttachment>::VulkanRenderPass(
     VulkanDevice &inDevice, VulkanImageBase &inColorImage)
    : mDevice(&inDevice.GetDeviceHandle()) {
    mColorAttachmentCount = 1;
    vk::AttachmentDescription ColorAttachment =
         vk::AttachmentDescription(vk::AttachmentDescriptionFlags(), inColorImage.GetImageFormat())
              .setInitialLayout(vk::ImageLayout::eUndefined)
              .setFinalLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
              .setLoadOp(vk::AttachmentLoadOp::eClear)
              .setStoreOp(vk::AttachmentStoreOp::eStore)
              .setSamples(vk::SampleCountFlagBits::e1);
    vk::AttachmentReference ColorAttachmentRef(0, vk::ImageLayout::eColorAttachmentOptimal);

    vk::SubpassDescription SubPassDescription =
         vk::SubpassDescription(vk::SubpassDescriptionFlags(), vk::PipelineBindPoint::eGraphics)
              .setColorAttachmentCount(1)
              .setPColorAttachments(&ColorAttachmentRef);
    std::vector<vk::SubpassDependency> SubpassDependencies = {
         vk::SubpassDependency()
              .setSrcSubpass(VK_SUBPASS_EXTERNAL)
              .setDstSubpass(0)
              .setSrcStageMask(vk::PipelineStageFlagBits::eBottomOfPipe)
              .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
              .setSrcAccessMask(vk::AccessFlagBits::eMemoryRead)
              .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentRead |
                                vk::AccessFlagBits::eColorAttachmentWrite)
              .setDependencyFlags(vk::DependencyFlagBits::eByRegion),

         vk::SubpassDependency()
              .setSrcSubpass(0)
              .setDstSubpass(VK_SUBPASS_EXTERNAL)
              .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
              .setDstStageMask(vk::PipelineStageFlagBits::eBottomOfPipe)
              .setSrcAccessMask(vk::AccessFlagBits::eColorAttachmentRead |
                                vk::AccessFlagBits::eColorAttachmentWrite)
              .setDstAccessMask(vk::AccessFlagBits::eMemoryRead)
              .setDependencyFlags(vk::DependencyFlagBits::eByRegion),
    };
    std::vector<vk::SubpassDescription> SubpassDescriptions       = {SubPassDescription};
    std::vector<vk::AttachmentDescription> AttachmentDescriptions = {ColorAttachment};

    mRenderPass = mDevice->createRenderPass(vk::RenderPassCreateInfo()
                                                 .setAttachments(AttachmentDescriptions)
                                                 .setSubpasses(SubpassDescriptions)
                                                 .setDependencies(SubpassDependencies));
}

template <>
VulkanRenderPass<RenderPassContext::Deferred>::VulkanRenderPass(VulkanDevice &inDevice,
                                                                VulkanImageBase &inPositionImage,
                                                                VulkanImageBase &inNormalImage,
                                                                VulkanImageBase &inAlbedoImage,
                                                                VulkanImageBase &inDepthImage)
    : mDevice(&inDevice.GetDeviceHandle()) {
    mColorAttachmentCount = 3;

    std::array<vk::AttachmentDescription, 4> AttachmentDescriptions;
    for (uint32_t i = 0; i < 4; ++i) {
        AttachmentDescriptions[i].samples        = vk::SampleCountFlagBits::e1;
        AttachmentDescriptions[i].loadOp         = vk::AttachmentLoadOp::eClear;
        AttachmentDescriptions[i].storeOp        = vk::AttachmentStoreOp::eStore;
        AttachmentDescriptions[i].stencilLoadOp  = vk::AttachmentLoadOp::eDontCare;
        AttachmentDescriptions[i].stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        if (i == 3) {
            AttachmentDescriptions[i].initialLayout = vk::ImageLayout::eUndefined;
            AttachmentDescriptions[i].finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
        } else {
            AttachmentDescriptions[i].initialLayout = vk::ImageLayout::eUndefined;
            AttachmentDescriptions[i].finalLayout   = vk::ImageLayout::eShaderReadOnlyOptimal;
        }
    }
    AttachmentDescriptions[0].format = inPositionImage.GetImageFormat();
    AttachmentDescriptions[1].format = inNormalImage.GetImageFormat();
    AttachmentDescriptions[2].format = inAlbedoImage.GetImageFormat();
    AttachmentDescriptions[3].format = inDepthImage.GetImageFormat();

    std::array<vk::AttachmentReference, 3> ColorReferences;
    ColorReferences[0] = {0, vk::ImageLayout::eColorAttachmentOptimal};
    ColorReferences[1] = {1, vk::ImageLayout::eColorAttachmentOptimal};
    ColorReferences[2] = {2, vk::ImageLayout::eColorAttachmentOptimal};
    vk::AttachmentReference DepthReference;
    DepthReference.setAttachment(3).setLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

    auto SubpassDescriptions = vk::SubpassDescription()
                                    .setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
                                    .setColorAttachments(ColorReferences)
                                    .setPDepthStencilAttachment(&DepthReference);

    std::array<vk::SubpassDependency, 2> SubpassDependencies = {
         vk::SubpassDependency()
              .setSrcSubpass(VK_SUBPASS_EXTERNAL)
              .setDstSubpass(0)
              .setSrcStageMask(vk::PipelineStageFlagBits::eFragmentShader)
              .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput |
                               vk::PipelineStageFlagBits::eEarlyFragmentTests)
              .setSrcAccessMask(vk::AccessFlagBits::eMemoryRead)
              .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentRead |
                                vk::AccessFlagBits::eColorAttachmentWrite |
                                vk::AccessFlagBits::eDepthStencilAttachmentWrite)
              .setDependencyFlags(vk::DependencyFlagBits::eByRegion),
         vk::SubpassDependency()
              .setSrcSubpass(0)
              .setDstSubpass(VK_SUBPASS_EXTERNAL)
              .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput |
                               vk::PipelineStageFlagBits::eLateFragmentTests)
              .setDstStageMask(vk::PipelineStageFlagBits::eFragmentShader)
              .setSrcAccessMask(vk::AccessFlagBits::eColorAttachmentRead |
                                vk::AccessFlagBits::eColorAttachmentWrite |
                                vk::AccessFlagBits::eDepthStencilAttachmentWrite)
              .setDstAccessMask(vk::AccessFlagBits::eMemoryRead)
              .setDependencyFlags(vk::DependencyFlagBits::eByRegion),
    };

    mRenderPass = mDevice->createRenderPass(vk::RenderPassCreateInfo()
                                                 .setAttachments(AttachmentDescriptions)
                                                 .setSubpasses(SubpassDescriptions)
                                                 .setDependencies(SubpassDependencies));
}