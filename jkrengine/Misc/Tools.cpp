#include "Tools.hpp"

namespace Jkr {
void CopyWindowDeferredImageToShapeImage(Window &inWindow, Renderer::Shape &inShape2d, int inId) {
    using namespace vk;
    auto &DstImage = inShape2d.GetImages()[inId]->GetUniformImage();
    auto &SrcImage = inWindow.GetDeferredPass().GetDeferredCompositionImage().GetUniformImage();
    ImageSubresourceLayers SrcSubLayers(ImageAspectFlagBits::eColor, 0, 0, 1);
    ImageSubresourceLayers DstSubLayers(ImageAspectFlagBits::eColor, 0, 0, 1);
    auto &cmd =
         inWindow.GetCommandBuffers(Window::ParameterContext::None)[inWindow.GetCurrentFrame()];
    SrcImage.CmdTransitionImageLayout(cmd,
                                      vk::ImageLayout::eShaderReadOnlyOptimal,
                                      vk::ImageLayout::eTransferSrcOptimal,
                                      vk::PipelineStageFlagBits::eColorAttachmentOutput,
                                      vk::PipelineStageFlagBits::eTransfer,
                                      vk::AccessFlagBits::eMemoryWrite,
                                      vk::AccessFlagBits::eMemoryRead);
    DstImage.CmdTransitionImageLayout(cmd,
                                      vk::ImageLayout::eUndefined,
                                      vk::ImageLayout::eTransferDstOptimal,
                                      vk::PipelineStageFlagBits::eAllGraphics,
                                      vk::PipelineStageFlagBits::eTransfer,
                                      vk::AccessFlagBits::eMemoryRead,
                                      vk::AccessFlagBits::eMemoryWrite);

    vk::ImageCopy Copy;
    Copy.setSrcOffset(vk::Offset3D{0, 0, 0})
         .setDstOffset(vk::Offset3D{0, 0, 0})
         .setExtent(vk::Extent3D{SrcImage.GetImageExtent(), 1})
         .setSrcSubresource(SrcSubLayers)
         .setDstSubresource(DstSubLayers);
    cmd.GetCommandBufferHandle().copyImage(SrcImage.GetImageHandle(),
                                           vk::ImageLayout::eTransferSrcOptimal,
                                           DstImage.GetImageHandle(),
                                           vk::ImageLayout::eTransferDstOptimal,
                                           Copy);
    DstImage.CmdTransitionImageLayout(cmd,
                                      vk::ImageLayout::eTransferDstOptimal,
                                      vk::ImageLayout::eGeneral,
                                      vk::PipelineStageFlagBits::eTransfer,
                                      vk::PipelineStageFlagBits::eFragmentShader,
                                      vk::AccessFlagBits::eMemoryWrite,
                                      vk::AccessFlagBits::eMemoryRead);
}
}; // namespace Jkr