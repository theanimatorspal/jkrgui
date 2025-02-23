#include "Tools.hpp"
#include "Renderers/ThreeD/World3D.hpp"
#include <Renderers/ThreeD/PBR.hpp>
#include <Pipeline/VulkanDescriptorUpdateHandler.hpp>
#include <Renderers/ThreeD/Uniform3D.hpp>
#include "JkrFile.hpp"
#include <stb_image_write.h>

namespace Jkr {
///@note
///@param inSw inSubmitWindow the window that has to be submitted before present (this should be a
/// NoWindow Window i.e. Create the Window using Jkr.CreateWindowNoWindow in lua and appropriate
/// constructor in C++)
///@param inPw inPresentWindow the window to present to
void Window::SyncSubmitPresent(Window &inSw, Window &inPw) {
    auto Scf = inSw.mCurrentFrame;
    auto Pcf = inPw.mCurrentFrame;

    vk::PipelineStageFlags SWaitFlags(vk::PipelineStageFlagBits::eTopOfPipe);
    vk::SubmitInfo sinfo;
    sinfo = vk::SubmitInfo({},
                           {},
                           inSw.mCommandBuffers[Scf].GetCommandBufferHandle(),
                           inSw.mRenderFinishedSemaphores[Scf].GetSemaphoreHandle());

    auto PWaitFlags =
         std::to_array({vk::PipelineStageFlags(vk::PipelineStageFlagBits::eColorAttachmentOutput),
                        vk::PipelineStageFlags(vk::PipelineStageFlagBits::eTopOfPipe)});
    vk::SubmitInfo pinfo;
    auto Arr = std::to_array({inPw.mImageAvailableSemaphores[Pcf].GetSemaphoreHandle(),
                              inSw.mRenderFinishedSemaphores[Scf].GetSemaphoreHandle()});
    pinfo    = vk::SubmitInfo(Arr,
                           PWaitFlags,
                           inPw.mCommandBuffers[Pcf].GetCommandBufferHandle(),
                           inPw.mRenderFinishedSemaphores[Pcf].GetSemaphoreHandle());

    inSw.mInstance->GetGraphicsQueue().GetQueueHandle().submit(sinfo,
                                                               inSw.mFences[Scf].GetFenceHandle());
    inPw.mInstance->GetGraphicsQueue().GetQueueHandle().submit(pinfo,
                                                               inPw.mFences[Pcf].GetFenceHandle());
    {
        uint32_t Result =
             inPw.mInstance->GetGraphicsQueue().Present<SubmitContext::ColorAttachment>(
                  inPw.mSwapChain,
                  inPw.mRenderFinishedSemaphores[inPw.mCurrentFrame],
                  inPw.mAcquiredImageIndex);

        if (inPw.mSwapChain.ImageIsNotOptimal(Result)) {
            inPw.Refresh();
        } else {
            inPw.mCurrentFrame = (inPw.mCurrentFrame + 1) % mMaxFramesInFlight;
        }
    }
    inSw.mCurrentFrame = (inSw.mCurrentFrame + 1) % mMaxFramesInFlight;
}

} // namespace Jkr

namespace Jkr::Misc {
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
void CopyWindowRenderTargetImageToShapeImage(Window &inWindow,
                                             Renderer::Shape &inShape2d,
                                             int inId) {

    using namespace vk;
    auto &DstImage = inShape2d.GetImages()[inId]->GetUniformImage();
    auto &SrcImage = inWindow.GetColorImageRenderTarget();
    ImageSubresourceLayers SrcSubLayers(ImageAspectFlagBits::eColor, 0, 0, 1);
    ImageSubresourceLayers DstSubLayers(ImageAspectFlagBits::eColor, 0, 0, 1);
    auto &cmd =
         inWindow.GetCommandBuffers(Window::ParameterContext::None)[inWindow.GetCurrentFrame()];
    ///@note SrcImage is already in TransferSrcOptimal as it is done for offscreen rendering
    //     SrcImage.CmdTransitionImageLayout(cmd,
    //                                       ImageLayout::eShaderReadOnlyOptimal,
    //                                       ImageLayout::eTransferSrcOptimal,
    //                                       PipelineStageFlagBits::eColorAttachmentOutput,
    //                                       PipelineStageFlagBits::eTransfer,
    //                                       AccessFlagBits::eMemoryWrite,
    //                                       AccessFlagBits::eMemoryRead);
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
     Window &inWindow,
     Window &inWindowWithDeferredImage,
     Renderer::CustomPainterImage &inCustomPainterImage) {
    using namespace vk;
    auto &DstImage = inCustomPainterImage.GetPainterParam().GetStorageImage();
    auto &SrcImage = inWindowWithDeferredImage.GetDeferredPass()
                          .GetDeferredCompositionImage()
                          .GetUniformImage();
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
void CopyWindowRenderTargetImageToCustomPainterImage(
     Window &inWindow,
     Window &inWindowWithRenderTargetImage,
     Renderer::CustomPainterImage &inCustomPainterImage) {
    using namespace vk;
    auto &DstImage = inCustomPainterImage.GetPainterParam().GetStorageImage();
    auto &SrcImage = inWindowWithRenderTargetImage.GetColorImageRenderTarget();
    ImageSubresourceLayers SrcSubLayers(ImageAspectFlagBits::eColor, 0, 0, 1);
    ImageSubresourceLayers DstSubLayers(ImageAspectFlagBits::eColor, 0, 0, 1);
    auto &cmd =
         inWindow.GetCommandBuffers(Window::ParameterContext::None)[inWindow.GetCurrentFrame()];
    ///@note SrcImage is already in TransferSrcOptimal layout
    //     SrcImage.CmdTransitionImageLayout(cmd,
    //                                       ImageLayout::eShaderReadOnlyOptimal,
    //                                       ImageLayout::eTransferSrcOptimal,
    //                                       PipelineStageFlagBits::eColorAttachmentOutput,
    //                                       PipelineStageFlagBits::eTransfer,
    //                                       AccessFlagBits::eMemoryWrite,
    //                                       AccessFlagBits::eMemoryRead);
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

void CopyWindowDepthImageToCustomPainterImage(Window &inWindow,
                                              Window &inWindowWithDepthImage,
                                              Renderer::CustomPainterImage &inCustomPainterImage) {
    using namespace vk;
    auto &DstImage = inCustomPainterImage.GetPainterParam().GetStorageImage();
    auto &SrcImage = inWindowWithDepthImage.GetDepthImage();
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

void RegisterShapeRenderer3DToCustomPainterImage(Jkr::Instance &inInstance,
                                                 Renderer::_3D::Shape &inShape3d,
                                                 Renderer::CustomPainterImage &inCustomPainterImage,
                                                 int inVertexStorageBufferIndex,
                                                 int inIndexStorageBufferIndex) {
    auto &VBuffer  = *inShape3d.GetPrimitive().GetVertexBufferPtr();
    auto &IBuffer  = *inShape3d.GetPrimitive().GetIndexBufferPtr();
    auto &ImageDes = inCustomPainterImage.GetDescriptorSet();
    auto &Dev      = inInstance.GetDevice();
    ksai::VulkanDescriptorUpdateHandler Handler(Dev);

    /// Since This is for image painter, the only set that is actually used there is set 0,
    /// therefore only set 0
    Handler.RW(BufferContext::Storage, ImageDes, VBuffer, 0, inVertexStorageBufferIndex, 0, 0);
    Handler.RW(BufferContext::Storage, ImageDes, IBuffer, 0, inIndexStorageBufferIndex, 0, 0);
}
void RegisterCustomPainterImageToCustomPainterImage(
     Jkr::Instance &inInstance,
     Renderer::CustomPainterImage &inCustomPainterImage,
     Renderer::CustomPainterImage &inCustomPainterImageTobeRegistered,
     int inIndex) {
    VulkanDescriptorUpdateHandler Handler(inInstance.GetDevice());
    VulkanSampler Sampler;
    Handler.RW(ImageContext::Storage,
               *inCustomPainterImage.mVulkanDescriptorSet,
               inCustomPainterImageTobeRegistered.mPainterParam->GetStorageImage(),
               Sampler,
               inIndex,
               0,
               0);
}

v<char> GetArbritaryPassImageToVector(Instance &inInstance, Window &inWindow, int inIndex) {
    auto &Arbs  = inWindow.GetArbritaryPassses();
    auto &Arb   = *Arbs[inIndex];
    auto Extent = Arb.mImage->GetUniformImage().GetImageExtent();
    auto Vect   = Arb.mImage->GetUniformImage().SubmitImmediateCmdGetImageToVector(
         inInstance.GetTransferQueue(),
         inInstance.GetStagingBuffer(Extent.height * Extent.height * 4),
         inWindow.GetCommandBuffers(
              Window_base::ParameterContext::None)[inWindow.GetCurrentFrame()],
         inInstance.GetUtilCommandBufferFence());
    //
    return Vect;
}

void FillComputeImageWithVectorChar(Instance &inInstance,
                                    v<char> invector,
                                    Renderer::CustomPainterImage &inCustomPainterImage) {
    auto &Image = *inCustomPainterImage.GetPainterParam().GetStorageImagePtr();
    void *data  = invector.data();
    Image.SubmitImmediateCmdCopyFromDataWithStagingBuffer(
         inInstance.GetTransferQueue(),
         inInstance.GetUtilCommandBuffer(),
         inInstance.GetDevice(),
         inInstance.GetUtilCommandBufferFence(),
         &data,
         invector.size() * sizeof(char),
         inInstance.GetStagingBuffer(invector.size() * sizeof(char)),
         vk::ImageLayout::eGeneral);
}

void SavePNGFileFromVChar(
     v<char> &inCharVector, sv inFileName, int inWidth, int inHeight, int inComp) {
    const auto *CharData = (const stbi_uc *)inCharVector.data();
    int x, y, channels_in_file;
    int desired_channels = 4;
    auto Image_data      = stbi_load_from_memory(
         CharData, inCharVector.size(), &x, &y, &channels_in_file, desired_channels);
    stbi_write_bmp(inFileName.data(), inWidth, inHeight, inComp, Image_data);
    stbi_image_free(Image_data);
}

v<char> GetVCharRawFromVCharImage(v<char> &inCharVector, int inWidth, int inHeight) {
    v<char> Out;
    int x, y, channels_in_file;
    int desired_channels = 4;
    auto Image_data      = stbi_load_from_memory((const stbi_uc *)inCharVector.data(),
                                            inCharVector.size(),
                                            &x,
                                            &y,
                                            &channels_in_file,
                                            desired_channels);
    Out.resize(x * y * desired_channels);
    std::memcpy(Out.data(), Image_data, x * y * desired_channels);
    stbi_image_free(Image_data);
    return Out;
}

void RegisterShadowPassImageToUniform3D(Window &inWindow,
                                        Renderer::_3D::Uniform3D &inUniform3D,
                                        int inIndex) {
    auto &ShadowPass = inWindow.GetShadowPass();
    auto &Image      = ShadowPass.GetDepthImagePainterParameter();
    Image.Register(0, inIndex, 0, inUniform3D.GetVulkanDescriptorSet(), 0);
}

void RegisterShape2DImageToUniform3D(Renderer::Shape &inShape,
                                     Renderer::_3D::Uniform3D &inUniform3D,
                                     int inImageIndex,
                                     int inBindingIndex) {
    inShape.GetImages()[inImageIndex]->Register(
         0, inBindingIndex, 0, inUniform3D.GetVulkanDescriptorSet(), 1);
}

} // namespace Jkr::Misc
