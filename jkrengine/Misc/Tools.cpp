#include "Tools.hpp"
#include "Renderers/ThreeD/World3D.hpp"
#include <Renderers/ThreeD/PBR.hpp>
#include <Pipeline/VulkanDescriptorUpdateHandler.hpp>
#include <Renderers/ThreeD/Uniform3D.hpp>
#include "JkrFile.hpp"
#include <stb_image_write.h>

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

static auto EquirectangularToMultiConversion(Jkr::Instance &inInstance,
                                             Renderer::_3D::Shape &inShape3D,
                                             uint32_t inSkyboxModelIndex,
                                             sv inEquirectangularCubeMapHDR,
                                             sv inEquirectangularToCubeMap_vs,
                                             sv inEquirectangularToCubeMap_fs,
                                             uint32_t Dim) {
    /// Equirectangular to Multi conversion
    int width, height, nrComponents;
    ///@warning R16G16B16 layout is not supported (in my device), so default RGBA8Unorm is used
    /// here,
    /// may have to do some adjustment, lesse. @todo Query the Default supported image formats
    /// beforehand
    auto *data = stbi_load(inEquirectangularCubeMapHDR.data(), &width, &height, &nrComponents, 4);
    VulkanImageVMA EquirectangularCubeMapHDR;
    EquirectangularCubeMapHDR.Init({
         .inVMA                 = &inInstance.GetVMA(),
         .inDevice              = &inInstance.GetDevice(),
         .inWidth               = static_cast<ui>(width),
         .inHeight              = static_cast<ui>(height),
         .inImageContext        = ImageContext::Default,
         .inChannel             = static_cast<ui>(nrComponents),
         .inLayerCount          = 1,
         .inSamples             = 1,
         .inMips                = 1,
         .inCreateOnlyImageView = false,
         .inUsageBits           = vk::ImageUsageFlagBits::eTransferDst |
                        vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eSampled,
         .inLayout             = std::nullopt,
         .inFormat             = std::nullopt,
         .inImageViewType      = std::nullopt,
         .inImageViewBaseLayer = 0,
    });
    EquirectangularCubeMapHDR.SubmitImmediateCmdCopyFromDataWithStagingBuffer(
         inInstance.GetTransferQueue(),
         inInstance.GetUtilCommandBuffer(),
         inInstance.GetDevice(),
         inInstance.GetUtilCommandBufferFence(),
         reinterpret_cast<void **>(&data),
         width * height * 4,
         inInstance.GetStagingBuffer(width * height * 4));

    stbi_image_free(data);

    VulkanSampler EnvMapSampler;
    EnvMapSampler.Init(
         {.mDevice       = &inInstance.GetDevice(),
          .mImageContext = ImageContext::Default,
          .mMinLod       = 0.0f,
          .mMaxLod       = 1.0f,
          .mCreateInfo   = vk::SamplerCreateInfo()
                              .setAddressModeU(vk::SamplerAddressMode::eClampToEdge)
                              .setAddressModeV(vk::SamplerAddressMode::eClampToEdge)
                              .setAddressModeW(vk::SamplerAddressMode::eClampToEdge)});

    Up<VulkanImageVMA> CubeMultiMap = MakeUp<VulkanImageVMA>();
    CubeMultiMap->Init({
         .inVMA                 = &inInstance.GetVMA(),
         .inDevice              = &inInstance.GetDevice(),
         .inWidth               = static_cast<ui>(Dim),
         .inHeight              = static_cast<ui>(Dim),
         .inImageContext        = ImageContext::CubeCompatible,
         .inChannel             = static_cast<ui>(4),
         .inLayerCount          = 6,
         .inSamples             = 1,
         .inMips                = 1,
         .inCreateOnlyImageView = false,
         .inUsageBits           = vk::ImageUsageFlagBits::eTransferDst |
                        vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eSampled,
         .inLayout             = std::nullopt,
         .inFormat             = std::nullopt,
         .inImageViewType      = std::nullopt,
         .inImageViewBaseLayer = 0,
    });

    VulkanImageVMA OffscreenFBufferImage;
    OffscreenFBufferImage.Init({
         .inVMA                 = &inInstance.GetVMA(),
         .inDevice              = &inInstance.GetDevice(),
         .inWidth               = static_cast<ui>(Dim),
         .inHeight              = static_cast<ui>(Dim),
         .inImageContext        = ImageContext::Default,
         .inChannel             = static_cast<ui>(4),
         .inLayerCount          = 6,
         .inSamples             = 1,
         .inMips                = 1,
         .inCreateOnlyImageView = false,
         .inUsageBits =
              vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eColorAttachment,
         .inLayout             = vk::ImageLayout::eUndefined,
         .inFormat             = std::nullopt,
         .inImageViewType      = std::nullopt,
         .inImageViewBaseLayer = 0,
    });

    VulkanRenderPass<RenderPassContext::SingleColorAttachment> RenderPass(inInstance.GetDevice(),
                                                                          OffscreenFBufferImage);
    VulkanFrameBuffer FrameBuffer;
    FrameBuffer.Init(inInstance.GetDevice(), RenderPass, OffscreenFBufferImage);
    Renderer::_3D::Simple3D Shader;
    Shader.CompileWithCustomRenderPass(inInstance,
                                       RenderPass,
                                       "cache2/EquirectangularMapHDRToMutiple.glsl",
                                       inEquirectangularToCubeMap_vs,
                                       inEquirectangularToCubeMap_fs,
                                       "",
                                       false);

    VulkanDescriptorPool DescriptorPool(inInstance.GetDevice(), 10, 10);
    Renderer::_3D::Uniform3D Uniform3D(inInstance);
    Uniform3D.Build(Shader, DescriptorPool);
    VulkanDescriptorUpdateHandler Handler(inInstance.GetDevice());
    Handler.RW(ImageContext::Default,
               Uniform3D.GetVulkanDescriptorSet(),
               EquirectangularCubeMapHDR,
               EnvMapSampler,
               0,
               0,
               0);

    using namespace vk;
    using namespace glm;

    mat4 CaptureProjection = perspective(radians(90.0f), 1.0f, 0.1f, 512.0f);
    v<mat4> CaptureViews   = {
         //    lookAt(vec3(0.0f), vec3(1.f, 0.f, 0.f), vec3(0.f, -1.f, 0.f)),
         //    lookAt(vec3(0.0f), vec3(-1.f, 0.f, 0.f), vec3(0.f, -1.f, 0.f)),
         //    lookAt(vec3(0.0f), vec3(0.f, 1.f, 0.f), vec3(0.f, 0.f, 1.f)),
         //    lookAt(vec3(0.0f), vec3(0.f, -1.f, 0.f), vec3(0.f, 0.f, -1.f)),
         //    lookAt(vec3(0.0f), vec3(0.f, 0.f, 1.f), vec3(0.f, -1.f, 0.f)),
         //    lookAt(vec3(0.0f), vec3(0.f, 0.f, -1.f), vec3(0.f, -1.f, 0.f)),

         // POSITIVE_X
         glm::rotate(glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
                     glm::radians(180.0f),
                     glm::vec3(1.0f, 0.0f, 0.0f)),
         // NEGATIVE_X
         glm::rotate(
              glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
              glm::radians(180.0f),
              glm::vec3(1.0f, 0.0f, 0.0f)),

         // POSITIVE_Y
         glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
         // NEGATIVE_Y
         glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)),

         // POSITIVE_Z
         glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
         // NEGATIVE_Z
         glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f)),

    };
    struct PushBlock {
        ///@warning written as model in Shader.lua
        mat4 projection;
        ///@warning written as m2 in Shader.lua
        mat4 view = mat4(vec4(0), vec4(0), vec4(0), vec4(0));
    } PushBlock;
    PushBlock.projection = CaptureProjection;

    auto &Cmd            = inInstance.GetUtilCommandBuffer();
    auto &Cmdh           = Cmd.GetCommandBufferHandle();
    vk::Viewport Viewport(0, 0, (float)Dim, (float)Dim);
    vk::Rect2D Scissor(vk::Offset2D{0}, vk::Extent2D{Dim, Dim});
    inInstance.GetUtilCommandBufferFence().Wait();
    inInstance.GetUtilCommandBufferFence().Reset();
    Cmd.Begin();
    for (int i = 0; i < 6; ++i) {
        Cmdh.setViewport(0, Viewport);
        Cmdh.setScissor(0, Scissor);
        PushBlock.view = CaptureProjection * CaptureViews[i];
        Cmd.BeginRenderPass(
             RenderPass, vk::Extent2D{Dim, Dim}, FrameBuffer, {1.0f, 1.0f, 1.0f, 1.0f, 1.0f});

        inShape3D.GetPrimitive().GetVertexBufferPtr()->Bind<BufferContext::Vertex>(Cmd);
        inShape3D.GetPrimitive().GetIndexBufferPtr()->Bind<BufferContext::Index>(Cmd);
        Uniform3D.GetVulkanDescriptorSet().Bind(
             vk::PipelineBindPoint::eGraphics,
             Cmd,
             Shader.GetPainterCache().GetVertexFragmentPipelineLayout(),
             0);

        Shader.BindByCommandBuffer(Cmd);

        Shader.DrawByCommandBuffer(
             Cmd, inShape3D, PushBlock, 0, inShape3D.GetIndexCount(inSkyboxModelIndex), 1);

        Cmd.EndRenderPass();
        CubeMultiMap->CmdCopyImageFromImageAfterStage(Cmd,
                                                      inInstance.GetDevice(),
                                                      OffscreenFBufferImage,
                                                      vk::PipelineStageFlagBits::eAllGraphics,
                                                      vk::AccessFlagBits::eNone,
                                                      static_cast<int>(Viewport.width),
                                                      static_cast<int>(Viewport.height),
                                                      0,
                                                      0,
                                                      0,
                                                      i,
                                                      1,
                                                      vk::ImageLayout::eGeneral,
                                                      vk::ImageLayout::eShaderReadOnlyOptimal,
                                                      vk::ImageLayout::eShaderReadOnlyOptimal);
    }
    Cmd.End();
    inInstance.GetGraphicsQueue().Submit<SubmitContext::SingleTime>(Cmd);
    return std::make_tuple(std::move(CubeMultiMap), width, height, nrComponents);
}

static const std::string CubeMultiMapString    = "CUBE_MULTI_MAP";
static const std::string IrradianceCubeString  = "IRRADIANCE_CUBE";
static const std::string PrefilteredCubeString = "PREFILTERED_CUBE";
static const std::string BRDFLookupTableString = "BRDF_LOOKUP_TABLE";

void SetupPBR(Jkr::Instance &inInstance,
              Jkr::Window &inWindow,
              Renderer::_3D::Uniform3D &inUniform3D,
              Renderer::_3D::World3D &inWorld3D,
              Renderer::_3D::Shape &inShape3D,
              int inSkyboxModelIndex,
              Misc::FileJkr &inJkrFile,
              sv inCachePrefix,
              sv inBRDF_vs,
              sv inBRDF_fs,
              sv inIrradianceCube_vs,
              sv inIrradianceCube_fs,
              sv inPrefilteredCube_vs,
              sv inPrefilteredCube_fs,
              sv inEquirectangularToCubeMap_vs,
              sv inEquirectangularToCubeMap_fs,
              sv inEquirectangularCubeMapHDR) {
    uint32_t Dim = 512;
    inInstance.GetStagingBuffer(512 * 512 * 10 * 24);
    std::string prefix = s(inCachePrefix);
    using namespace std;

    auto [CubeMultiMap, width, height, nrComponents] =
         EquirectangularToMultiConversion(inInstance,
                                          inShape3D,
                                          inSkyboxModelIndex,
                                          inEquirectangularCubeMapHDR,
                                          inEquirectangularToCubeMap_vs,
                                          inEquirectangularToCubeMap_fs,
                                          Dim);
    inUniform3D.AddGenerateBRDFLookupTable(inInstance,
                                           inWindow,
                                           "cache2/" + s(inCachePrefix) + "BRDFLookupTable.glsl",
                                           inBRDF_vs,
                                           inBRDF_fs,
                                           "",
                                           false);
    inUniform3D.AddGenerateIrradianceCube(inInstance,
                                          inWindow,
                                          inShape3D,
                                          inSkyboxModelIndex,
                                          *CubeMultiMap,
                                          inWorld3D,
                                          "cache2/" + s(inCachePrefix) + "IrradianceCube.glsl",
                                          inIrradianceCube_vs,
                                          inIrradianceCube_fs,
                                          "",
                                          false);
    inUniform3D.AddGeneratePrefilteredCube(inInstance,
                                           inWindow,
                                           inShape3D,
                                           inSkyboxModelIndex,
                                           *CubeMultiMap,
                                           inWorld3D,
                                           "cache2/" + s(inCachePrefix) + "PrefilteredCube.glsl",
                                           inPrefilteredCube_vs,
                                           inPrefilteredCube_fs,
                                           "",
                                           false);

    Up<Jkr::PainterParameter<PainterParameterContext::SkyboxImage>> SkyboxImage =
         MakeUp<Jkr::PainterParameter<PainterParameterContext::SkyboxImage>>(inInstance);
    SkyboxImage->mUniformImagePtr = std::move(CubeMultiMap);
    SkyboxImage->mSampler         = MakeUp<VulkanSampler>(inInstance.GetDevice());

    inUniform3D.AddSkyboxImage(*SkyboxImage);

    auto &Images = inUniform3D.GetSkyboxImagesRef();
    Images.insert_or_assign(kstd::BindingIndex::CubeMapImage, mv(SkyboxImage));
}

static auto ImgToFile(auto &ini, auto &inJkrFile, auto image_prefix, auto &img) {
    inJkrFile.Write((image_prefix + "PROPS").c_str(), img.GetImageProperty());
    for (int mip = 0; mip < img.GetImageProperty().mMipLevels; ++mip) {
        ///@note Property is also being written here
        inJkrFile.Write(
             (image_prefix + "IMAGE" + std::to_string(mip)).data(),
             img.SubmitImmediateCmdGetImageToVector(
                  ini.GetTransferQueue(),
                  ini.GetStagingBuffer(img.GetImageExtent().width * img.GetImageExtent().height *
                                       4 * img.GetImageProperty().mArrayLayers),
                  ini.GetUtilCommandBuffer(),
                  ini.GetUtilCommandBufferFence(),
                  vk::ImageLayout::eShaderReadOnlyOptimal,
                  img.GetImageExtent().width * std::pow(0.5f, mip),
                  img.GetImageExtent().height * std::pow(0.5f, mip),
                  mip,
                  0,
                  img.GetImageProperty().mArrayLayers));
    }
}

static auto FileToImage(Instance &ini, FileJkr &inJkrFile, auto image_prefix) {
    Up<VulkanImageVMA> Image = mu<VulkanImageVMA>();
    ImageProperties Props    = inJkrFile.Read<ImageProperties>((image_prefix + "PROPS").c_str());
    VulkanImageVMA::CreateInfo Info;
    Info.inVMA             = &ini.GetVMA();
    Info.inDevice          = &ini.GetDevice();
    Info.inImageProperties = Props;
    Image->Init(Info);
    ///@warning HardCoded 4
    for (int mip = 0; mip < Props.mMipLevels; ++mip) {
        v<char> image_Vec =
             inJkrFile.Read<v<char>>((image_prefix + "IMAGE" + std::to_string(mip)).c_str());
        void *data = image_Vec.data();
        Image->SubmitImmediateCmdCopyFromDataWithStagingBuffer(
             ini.GetTransferQueue(),
             ini.GetUtilCommandBuffer(),
             ini.GetDevice(),
             ini.GetUtilCommandBufferFence(),
             &data,
             image_Vec.size(),
             ini.GetStagingBuffer(image_Vec.size()),
             vk::ImageLayout::eShaderReadOnlyOptimal,
             Image->GetImageExtent().width * std::pow(0.5f, mip),
             Image->GetImageExtent().height * std::pow(0.5f, mip),
             mip,
             0,
             Props.mArrayLayers);
    }

    return std::move(Image);
}

void SerializeDeserializeUniform3D(Instance &ini,
                                   sv inIdName,
                                   Misc::FileJkr &inJkrFile,
                                   Renderer::_3D::Uniform3D &inUniform3D) {
    auto prefix            = s(inIdName);
    auto image_prefix      = prefix + "IMG";
    auto ubuffer_prefix    = prefix + "BUF";
    auto storagebuf_prefix = prefix + "STG";
    auto skybox_prefix     = prefix + "SKY";
    ///@warning  Only 255 bindings are possible of each type
    using Header       = v<char>;
    bool file_has_data = inJkrFile.GetFileContents().contains(image_prefix) or
                         inJkrFile.GetFileContents().contains(ubuffer_prefix) or
                         inJkrFile.GetFileContents().contains(storagebuf_prefix) or
                         inJkrFile.GetFileContents().contains(skybox_prefix);
    if (not file_has_data) {
        {
            Header img_header;
            v<int> image_binding_set;
            auto &images = inUniform3D.GetImagesRef();
            for (int i = 0; auto &[key, value] : images) {
                img_header.push_back(0);
                img_header[i] = key;
                ImgToFile(
                     ini, inJkrFile, image_prefix + std::to_string(key), value->GetUniformImage());
                image_binding_set.push_back(value->GetSet());
                ++i;
            }
            inJkrFile.Write(image_prefix.c_str(), img_header);
            inJkrFile.Write((image_prefix + "BSET").c_str(), image_binding_set);
        }
        {
            auto &sboxes = inUniform3D.GetSkyboxImagesRef();
            v<int> skybox_binding_set;
            Header sbox_header;
            for (int i = 0; auto &[key, value] : sboxes) {
                sbox_header.push_back(0);
                sbox_header[i] = key;
                ImgToFile(
                     ini, inJkrFile, skybox_prefix + std::to_string(key), value->GetUniformImage());
                skybox_binding_set.push_back(value->GetSet());
                ++i;
            }
            inJkrFile.Write(skybox_prefix.c_str(), sbox_header);
            inJkrFile.Write((skybox_prefix + "BSET").c_str(), skybox_binding_set);
        }
        {
            auto &buffers = inUniform3D.GetUniformBuffersRef();
            Header ubuf_header;
            v<int> buffer_binding_set;
            for (int i = 0; auto &[key, value] : buffers) {
                ubuf_header.push_back(0);
                ubuf_header[i] = key;
                inJkrFile.Write(
                     (ubuffer_prefix + std::to_string(key)).c_str(),
                     value->GetUniformBuffer().SubmitImmediateGetBufferToVector(
                          ini.GetTransferQueue(),
                          ini.GetStagingBuffer(value->GetUniformBuffer().GetBufferSize()),
                          ini.GetUtilCommandBuffer(),
                          ini.GetUtilCommandBufferFence()));
                buffer_binding_set.push_back(value->GetSet());
                ++i;
            }
            inJkrFile.Write(ubuffer_prefix.c_str(), ubuf_header);
            inJkrFile.Write((ubuffer_prefix + "BSET").c_str(), buffer_binding_set);
        }
        {
            auto &sbuffers = inUniform3D.GetStorageBuffersRef();
            Header sbuf_header;
            v<int> sbuffer_binding_set;
            for (int i = 0; auto &[key, value] : sbuffers) {
                sbuf_header.push_back(0);
                sbuf_header[i] = key;
                inJkrFile.Write(
                     (storagebuf_prefix + std::to_string(key)).c_str(),
                     value->mStorageBufferCoherentPtr->SubmitImmediateGetBufferToVector(
                          ini.GetTransferQueue(),
                          ini.GetStagingBuffer(value->GetStorageBufferCoherent().GetBufferSize()),
                          ini.GetUtilCommandBuffer(),
                          ini.GetUtilCommandBufferFence()));
                sbuffer_binding_set.push_back(value->GetSet());
                ++i;
            }
            inJkrFile.Write(storagebuf_prefix.c_str(), sbuf_header);
            inJkrFile.Write((storagebuf_prefix + "BSET").c_str(), sbuffer_binding_set);
        }

    } else {
        using ImageType         = Jkr::Renderer::_3D::Uniform3D::ImageType;
        using SkyboxImageType   = Jkr::Renderer::_3D::Uniform3D::SkyboxImageType;
        using UniformBufferType = Jkr::Renderer::_3D::Uniform3D::UniformBufferType;
        using StorageBufferType = Jkr::Renderer::_3D::Uniform3D::StorageBufferType;
        {
            Header img_header        = inJkrFile.Read<Header>(image_prefix.c_str());
            v<int> image_binding_set = inJkrFile.Read<v<int>>((image_prefix + "BSET").c_str());
            for (int i = 0; char key : img_header) {
                auto Image = FileToImage(ini, inJkrFile, image_prefix + std::to_string(key));
                up<ImageType> Param     = mu<ImageType>(ini);
                Param->mSampler         = mu<VulkanSampler>(ini.GetDevice());
                Param->mUniformImagePtr = mv(Image);
                Param->Register(
                     0, (int)key, 0, inUniform3D.GetVulkanDescriptorSet(), image_binding_set[i]);
                inUniform3D.GetImagesRef()[(int)key] = mv(Param);
                ++i;
            }
        }
        {
            Header skbox_header       = inJkrFile.Read<Header>(skybox_prefix.c_str());
            v<int> skybox_binding_set = inJkrFile.Read<v<int>>((skybox_prefix + "BSET").c_str());
            for (int i = 0; char key : skbox_header) {
                auto Image = FileToImage(ini, inJkrFile, skybox_prefix + std::to_string(key));
                up<SkyboxImageType> Param = mu<SkyboxImageType>(ini);
                Param->mSampler           = mu<VulkanSampler>(ini.GetDevice());
                Param->mUniformImagePtr   = mv(Image);
                Param->Register(
                     0, (int)key, 0, inUniform3D.GetVulkanDescriptorSet(), skybox_binding_set[i]);
                inUniform3D.GetSkyboxImagesRef()[(int)key] = mv(Param);
                ++i;
            }
        }
        {
            Header ubuf_header         = inJkrFile.Read<Header>(ubuffer_prefix.c_str());
            v<int> ubuffer_binding_set = inJkrFile.Read<v<int>>((ubuffer_prefix + "BSET").c_str());
            for (int i = 0; char key : ubuf_header) {
                auto Buffer =
                     inJkrFile.Read<v<char>>((ubuffer_prefix + std::to_string(key)).c_str());
                auto data                     = Buffer.data();
                up<VulkanBufferVMA> VMABuffer = mu<VulkanBufferVMA>();
                VMABuffer->Init({&ini.GetVMA(),
                                 &ini.GetDevice(),
                                 Buffer.size(),
                                 BufferContext::Uniform,
                                 MemoryType::HostVisibleAndCoherenet});

                VMABuffer->SubmitImmediateCmdCopyFrom(ini.GetTransferQueue(),
                                                      ini.GetUtilCommandBuffer(),
                                                      ini.GetUtilCommandBufferFence(),
                                                      Buffer.data());
                up<UniformBufferType> Param = mu<UniformBufferType>(ini);
                Param->mUniformBufferPtr    = mv(VMABuffer);
                Param->Register(
                     0, (int)key, 0, inUniform3D.GetVulkanDescriptorSet(), ubuffer_binding_set[i]);
                inUniform3D.GetUniformBuffersRef()[(int)key] = mv(Param);
                ++i;
            }
        }

        {
            Header sbuf_header = inJkrFile.Read<Header>(storagebuf_prefix.c_str());
            v<int> sbuffer_binding_set =
                 inJkrFile.Read<v<int>>((storagebuf_prefix + "BSET").c_str());
            for (int i = 0; char key : sbuf_header) {
                auto Buffer =
                     inJkrFile.Read<v<char>>((storagebuf_prefix + std::to_string(key)).c_str());
                auto data                     = Buffer.data();
                up<VulkanBufferVMA> VMABuffer = mu<VulkanBufferVMA>();
                VMABuffer->Init({&ini.GetVMA(),
                                 &ini.GetDevice(),
                                 Buffer.size(),
                                 BufferContext::Storage,
                                 MemoryType::HostVisibleAndCoherenet});
                VMABuffer->SubmitImmediateCmdCopyFrom(ini.GetTransferQueue(),
                                                      ini.GetUtilCommandBuffer(),
                                                      ini.GetUtilCommandBufferFence(),
                                                      Buffer.data());
                up<StorageBufferType> Param      = mu<StorageBufferType>(ini);
                Param->mStorageBufferCoherentPtr = mv(VMABuffer);
                Param->RegisterCoherent(
                     0, (int)key, 0, inUniform3D.GetVulkanDescriptorSet(), sbuffer_binding_set[i]);
                inUniform3D.GetStorageBuffersRef()[(int)key] = mv(Param);
                ++i;
            }
        }
    }
    ini.GetTransferQueue().Wait();
}

void SerializeDeserializeWorld3D(Instance &ini,
                                 Window &inw,
                                 sv inIdName,
                                 Misc::FileJkr &inJkrFile,
                                 Renderer::_3D::World3D &inWorld3D) {
    auto &Cameras   = inWorld3D.GetCamras();
    auto &Models    = inWorld3D.GetModels();
    auto &Uniforms  = inWorld3D.GetUniforms();
    auto &Simple3Ds = inWorld3D.GetSimple3Ds();
    auto &Lights    = inWorld3D.GetLights();

    auto Prefix     = s(inIdName);

    if (inJkrFile.GetFileContents().contains((Prefix))) {
        Cameras.resize(0);
        Models.resize(0);
        Uniforms.resize(0);
        Simple3Ds.resize(0);
        Lights.resize(0);
        Cameras = inJkrFile.Read<v<Renderer::_3D::Camera3D>>((Prefix + "CAMERAS").c_str());
        Lights  = inJkrFile.Read<v<Renderer::_3D::World3D::Light3D>>((Prefix + "LIGHTS").c_str());

        v<Renderer::_3D::Simple3D::CompileContext> CompileContexts =
             inJkrFile.Read<decltype(CompileContexts)>((Prefix + "COMPILE_CONTEXTS").c_str());

        for (int i = 0; i < CompileContexts.size(); ++i) {
            inWorld3D.AddSimple3D(ini, inw);
            auto &s3d                 = inWorld3D.GetSimple3Ds()[i];
            s3d->GetPainterCachePtr() = mu<PainterCache>(ini);
            s3d->GetPainterCache().ReadFromCacheFile(inJkrFile,
                                                     Prefix + "SIMPLE3Ds" + std::to_string(i));
            s3d->Compile(ini, inw, " ", " ", " ", " ", true, CompileContexts[i]);
        }

        size_type uniformSize = inJkrFile.Read<size_type>((Prefix + "UNIFORM_SIZE").c_str());
        for (int i = 0; i < uniformSize; ++i) {
            inWorld3D.AddUniform3D(ini);
            auto Simple3DEntryName  = Prefix + "UNIFORMS" + std::to_string(i) + "SIMPLE3DINDEX";
            auto AssociatedSimple3D = inJkrFile.Read<int>(Simple3DEntryName.c_str());
            std::cout << "UNIFORM i = " << i << "; SIMPLE" << AssociatedSimple3D << "\n";
            inWorld3D.GetUniforms()[i]->Build(*inWorld3D.GetSimple3Ds()[AssociatedSimple3D]);
            SerializeDeserializeUniform3D(ini,
                                          s(inIdName) + "UNIFORMS" + std::to_string(i),
                                          inJkrFile,
                                          *inWorld3D.GetUniforms()[i]);
        }
    } else {
        inJkrFile.Write((Prefix + "CAMERAS").c_str(), Cameras);
        inJkrFile.Write((Prefix + "LIGHTS").c_str(), Lights);

        ///@note CompileContexts are required for compilation of Shaders during Read
        v<Renderer::_3D::Simple3D::CompileContext> CompileContexts;
        CompileContexts.resize(Simple3Ds.size());

        ///@note This is needed as the Uniforms don't store which INDEX of the shader is that
        /// associated with, since I am using reflections, I have to "Build" the uniform by a shader
        ///@note the Simple3D.GetPainterCache().GetCacheFileEntryName() is set while calling the
        /// WriteToCacheFile() function for the painter cache, as shown below *CacheFileEntryName*
        umap<std::string, int> CacheFileEntryNameToWorldSimple3DIndex;
        for (int i = 0; auto &Simple3D : Simple3Ds) {
            auto CacheFileEntryName = Prefix + "SIMPLE3Ds" + std::to_string(i);
            Simple3D->GetPainterCache().WriteToCacheFile(inJkrFile, CacheFileEntryName);
            CacheFileEntryNameToWorldSimple3DIndex[CacheFileEntryName] = i;
            CompileContexts[i] = Simple3D->GetCompileContext();
            ++i;
        }
        inJkrFile.Write((Prefix + "COMPILE_CONTEXTS").c_str(), CompileContexts);

        inJkrFile.Write((Prefix + "UNIFORM_SIZE").c_str(), Uniforms.size());
        for (int i = 0; auto &Uniform : Uniforms) {
            ///@note Here
            auto AssociatedSimple3D = CacheFileEntryNameToWorldSimple3DIndex
                 [Uniform->GetAssociatedSimple3D()->GetPainterCache().GetCacheFileEntryName()];
            std::cout << "UNIFORM i = " << i << "; SIMPLE" << AssociatedSimple3D << "\n";
            inJkrFile.Write((Prefix + "UNIFORMS" + std::to_string(i) + "SIMPLE3DINDEX").c_str(),
                            AssociatedSimple3D);
            SerializeDeserializeUniform3D(
                 ini, s(inIdName) + "UNIFORMS" + std::to_string(i), inJkrFile, *Uniform);
            ++i;
        }

        inJkrFile.Write(Prefix.c_str(), true);
    }
}

void SerializeDeserializeShape3D(Instance &ini,
                                 sv inIdName,
                                 Misc::FileJkr &inJkrFile,
                                 Renderer::_3D::Shape &inShape) {
    auto vid  = s(inIdName) + "vbuffer";
    auto iid  = s(inIdName) + "fbuffer";
    auto ids  = s(inIdName) + "idbuffer";
    auto vidv = s(inIdName) + "vbuffervector";
    auto iidv = s(inIdName) + "fbuffervector";
    if (not inJkrFile.GetFileContents().contains(vid.c_str())) {
        auto [Ids, Vertices, Indices, vptr, iptr] = inShape.GetSerializablesRefs();
        inJkrFile.Write(ids.c_str(), *Ids);

        auto &vBuffer = *inShape.GetPrimitive().GetVertexBufferPtr();
        auto vBufferVector =
             inShape.GetPrimitive().GetVertexBufferPtr()->SubmitImmediateGetBufferToVector(
                  ini.GetGraphicsQueue(),
                  ini.GetStagingBuffer(vBuffer.GetBufferSize()),
                  ini.GetUtilCommandBuffer(),
                  ini.GetUtilCommandBufferFence());
        auto &iBuffer = *inShape.GetPrimitive().GetIndexBufferPtr();
        auto iBufferVector =
             iBuffer.SubmitImmediateGetBufferToVector(ini.GetGraphicsQueue(),
                                                      ini.GetStagingBuffer(iBuffer.GetBufferSize()),
                                                      ini.GetUtilCommandBuffer(),
                                                      ini.GetUtilCommandBufferFence());
        inJkrFile.Write(vid.c_str(), vBufferVector);
        inJkrFile.Write(iid.c_str(), iBufferVector);
        inJkrFile.Write(vidv.c_str(), *Vertices);
        inJkrFile.Write(iidv.c_str(), *Indices);
    } else {
        using namespace ksai::kstd;
        up<Primitive> Pri = MakeUp<Primitive>(ini);
        auto vBuffer      = inJkrFile.Read<v<char>>(vid.c_str());
        auto iBuffer      = inJkrFile.Read<v<char>>(iid.c_str());
        auto idBuffer =
             inJkrFile.Read<v<Renderer::_3D::Shape3D::VertexIndexMemOffset>>(ids.c_str());
        ///@warning here, NON-Integrated GPUs are not handled, and will also not be handled for
        /// now
        /// it will be handled later, as there are a lot of @todo Refactors to be done
        Pri->GetIndexBufferPtr()  = MakeUp<VulkanBufferVMA>(ini.GetVMA(),
                                                           ini.GetDevice(),
                                                           iBuffer.size(),
                                                           BufferContext::Index,
                                                           MemoryType::HostVisibleAndCoherenet);
        Pri->GetVertexBufferPtr() = MakeUp<VulkanBufferVMA>(ini.GetVMA(),
                                                            ini.GetDevice(),
                                                            vBuffer.size(),
                                                            BufferContext::Vertex,
                                                            MemoryType::HostVisibleAndCoherenet);

        Pri->GetIndexBufferPtr()->SubmitImmediateCmdCopyFrom(ini.GetTransferQueue(),
                                                             ini.GetUtilCommandBuffer(),
                                                             ini.GetUtilCommandBufferFence(),
                                                             iBuffer.data());
        Pri->GetVertexBufferPtr()->SubmitImmediateCmdCopyFrom(ini.GetTransferQueue(),
                                                              ini.GetUtilCommandBuffer(),
                                                              ini.GetUtilCommandBufferFence(),
                                                              vBuffer.data());
        Pri->SetIndexCount(iBuffer.size() / sizeof(uint32_t));
        inShape.GetPrimitivePtr() = mv(Pri);

        auto [Ids, Vertices, Indices, CurrentIndexPointer, CurrentVertexPointer] =
             inShape.GetSerializablesRefs();

        *Ids                  = idBuffer;
        *Vertices             = inJkrFile.Read<v<Vertex3D>>(vidv.c_str());
        *Indices              = inJkrFile.Read<v<uint32_t>>(iidv.c_str());
        *CurrentIndexPointer  = Indices->size();
        *CurrentVertexPointer = Vertices->size();
    }
}

void SerializeDeserializeObjectVector(sv inName,
                                      v<Renderer::_3D::Object3D> &inObject3d,
                                      Misc::FileJkr &inJkrFile) {
    if (inJkrFile.GetFileContents().contains(s(inName))) {

        ///@todo change this to name.data() everywhere
        inJkrFile.Write(s(inName).c_str(), inObject3d);
    } else {
        inObject3d = inJkrFile.Read<v<Renderer::_3D::Object3D>>(s(inName).c_str());
    }
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

} // namespace Jkr::Misc

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