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
                                      ImageLayout::eShaderReadOnlyOptimal,
                                      ImageLayout::eTransferSrcOptimal,
                                      PipelineStageFlagBits::eColorAttachmentOutput,
                                      PipelineStageFlagBits::eTransfer,
                                      AccessFlagBits::eMemoryWrite,
                                      AccessFlagBits::eMemoryRead);
    DstImage.CmdTransitionImageLayout(cmd,
                                      ImageLayout::eUndefined,
                                      ImageLayout::eTransferDstOptimal,
                                      PipelineStageFlagBits::eAllGraphics,
                                      PipelineStageFlagBits::eTransfer,
                                      AccessFlagBits::eMemoryRead,
                                      AccessFlagBits::eMemoryWrite);

    ImageCopy Copy;
    Copy.setSrcOffset(Offset3D{0, 0, 0})
         .setDstOffset(Offset3D{0, 0, 0})
         .setExtent(Extent3D{SrcImage.GetImageExtent(), 1})
         .setSrcSubresource(SrcSubLayers)
         .setDstSubresource(DstSubLayers);
    cmd.GetCommandBufferHandle().copyImage(SrcImage.GetImageHandle(),
                                           ImageLayout::eTransferSrcOptimal,
                                           DstImage.GetImageHandle(),
                                           ImageLayout::eTransferDstOptimal,
                                           Copy);
    DstImage.CmdTransitionImageLayout(cmd,
                                      ImageLayout::eTransferDstOptimal,
                                      ImageLayout::eGeneral,
                                      PipelineStageFlagBits::eTransfer,
                                      PipelineStageFlagBits::eFragmentShader,
                                      AccessFlagBits::eMemoryWrite,
                                      AccessFlagBits::eMemoryRead);
}

void CopyWindowDeferredImageToCustomPainterImage(
     Window &inWindow, Renderer::CustomPainterImage &inCustomPainterImage) {
    using namespace vk;
    auto &DstImage = inCustomPainterImage.GetPainterParam().GetStorageImage();
    auto &SrcImage = inWindow.GetDeferredPass().GetDeferredCompositionImage().GetUniformImage();
    ImageSubresourceLayers SrcSubLayers(ImageAspectFlagBits::eColor, 0, 0, 1);
    ImageSubresourceLayers DstSubLayers(ImageAspectFlagBits::eColor, 0, 0, 1);
    auto &cmd =
         inWindow.GetCommandBuffers(Window::ParameterContext::None)[inWindow.GetCurrentFrame()];
    SrcImage.CmdTransitionImageLayout(cmd,
                                      ImageLayout::eShaderReadOnlyOptimal,
                                      ImageLayout::eTransferSrcOptimal,
                                      PipelineStageFlagBits::eColorAttachmentOutput,
                                      PipelineStageFlagBits::eTransfer,
                                      AccessFlagBits::eMemoryWrite,
                                      AccessFlagBits::eMemoryRead);
    DstImage.CmdTransitionImageLayout(cmd,
                                      ImageLayout::eUndefined,
                                      ImageLayout::eTransferDstOptimal,
                                      PipelineStageFlagBits::eAllGraphics,
                                      PipelineStageFlagBits::eTransfer,
                                      AccessFlagBits::eMemoryRead,
                                      AccessFlagBits::eMemoryWrite);

    ImageCopy Copy;
    Copy.setSrcOffset(Offset3D{0, 0, 0})
         .setDstOffset(Offset3D{0, 0, 0})
         .setExtent(Extent3D{SrcImage.GetImageExtent(), 1})
         .setSrcSubresource(SrcSubLayers)
         .setDstSubresource(DstSubLayers);
    cmd.GetCommandBufferHandle().copyImage(SrcImage.GetImageHandle(),
                                           ImageLayout::eTransferSrcOptimal,
                                           DstImage.GetImageHandle(),
                                           ImageLayout::eTransferDstOptimal,
                                           Copy);
    DstImage.CmdTransitionImageLayout(cmd,
                                      ImageLayout::eTransferDstOptimal,
                                      ImageLayout::eGeneral,
                                      PipelineStageFlagBits::eTransfer,
                                      PipelineStageFlagBits::eFragmentShader,
                                      AccessFlagBits::eMemoryWrite,
                                      AccessFlagBits::eMemoryRead);
}

}; // namespace Jkr