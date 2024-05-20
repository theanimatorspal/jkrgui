#include "PBR.hpp"
using namespace Jkr::Renderer;
using namespace ksai;
using namespace Jkr;

Up<VulkanImageVMA> PBR::GenerateBRDFLookupTable(Instance& inInstance,
                                                WindowMulT& inWindow,
                                                std::string_view inFileName,
                                                std::string_view inVertexShader,
                                                std::string_view inFragmentShader,
                                                std::string_view inComputeShader,
                                                bool inShouldLoad) {
    const int Dim                              = 512;
    Up<VulkanImageVMA> ColorAttachmentImagePtr = MakeUp<VulkanImageVMA>(
         inInstance.GetVMA(), inInstance.GetDevice(), Dim, Dim, ImageContext::ColorAttach);
    VulkanRenderPass<RenderPassContext::SingleColorAttachment> RenderPass(inInstance.GetDevice(),
                                                                          *ColorAttachmentImagePtr);
    VulkanFrameBuffer<1, VulkanImageVMA> FrameBuffer(
         inInstance.GetDevice(), RenderPass, *ColorAttachmentImagePtr);

    _3D::Simple3D Simple3D(inInstance, inWindow);
    Simple3D.CompileWithCustomRenderPass(inInstance,
                                         inWindow,
                                         RenderPass,
                                         inFileName,
                                         inVertexShader,
                                         inFragmentShader,
                                         inComputeShader,
                                         inShouldLoad);

    VulkanDescriptorSet DescriptorSet(
         inInstance.GetDevice(),
         inInstance.GetDescriptorPool(),
         Simple3D.GetPainterCache().GetVertexFragmentDescriptorSetLayout());

    VulkanCommandBuffer CommandBuffer(inInstance.GetDevice(), inInstance.GetCommandPool());

    CommandBuffer.Begin();
    CommandBuffer.BeginRenderPass(
         RenderPass, vk::Extent2D{Dim, Dim}, FrameBuffer, std::array<float, 5>{1, 1, 1, 1, 1});

    CommandBuffer.EndRenderPass();
    CommandBuffer.End();
    inInstance.GetGraphicsQueue().Wait();
    return std::move(ColorAttachmentImagePtr);
}