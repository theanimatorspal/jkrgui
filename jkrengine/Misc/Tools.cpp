#include "Tools.hpp"
#include "Renderers/ThreeD/World3D.hpp"
#include <Renderers/ThreeD/PBR.hpp>
#include <Pipeline/VulkanDescriptorUpdateHandler.hpp>
#include <Renderers/ThreeD/Uniform3D.hpp>
#include "JkrFile.hpp"

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
    if (inJkrFile.GetFileContents().contains(prefix + CubeMultiMapString)) {

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
        {
            auto Vector = inJkrFile.Read<v<char>>((prefix + CubeMultiMapString).c_str());
            auto data   = Vector.data();
            CubeMultiMap->SubmitImmediateCmdCopyFromDataWithStagingBuffer(
                 inInstance.GetTransferQueue(),
                 inInstance.GetUtilCommandBuffer(),
                 inInstance.GetDevice(),
                 inInstance.GetUtilCommandBufferFence(),
                 reinterpret_cast<void **>(&data),
                 Vector.size(),
                 inInstance.GetStagingBuffer(Vector.size()),
                 vk::ImageLayout::eShaderReadOnlyOptimal,
                 -1,
                 -1,
                 0,
                 0,
                 6);
        }

        Up<VulkanImageVMA> BRDFLUTImage =
             MakeUp<VulkanImageVMA>(inInstance.GetVMA(),
                                    inInstance.GetDevice(),
                                    Jkr::Renderer::PBR::BRDFLUTDimension,
                                    Jkr::Renderer::PBR::BRDFLUTDimension,
                                    ImageContext::Default,
                                    4,
                                    1,
                                    1,
                                    1,
                                    vk::ImageUsageFlagBits::eSampled);
        {
            auto Vector = inJkrFile.Read<v<char>>((prefix + BRDFLookupTableString).c_str());
            auto data   = Vector.data();
            BRDFLUTImage->SubmitImmediateCmdCopyFromDataWithStagingBuffer(
                 inInstance.GetTransferQueue(),
                 inInstance.GetUtilCommandBuffer(),
                 inInstance.GetDevice(),
                 inInstance.GetUtilCommandBufferFence(),
                 reinterpret_cast<void **>(&data),
                 Vector.size(),
                 inInstance.GetStagingBuffer(Vector.size()),
                 vk::ImageLayout::eShaderReadOnlyOptimal,
                 -1,
                 -1,
                 0,
                 0,
                 1);
        }

        auto IrradianceCubeMap = MakeUp<VulkanImageVMA>(
             inInstance.GetVMA(),
             inInstance.GetDevice(),
             Jkr::Renderer::PBR::IrradianceCubeDimension,
             Jkr::Renderer::PBR::IrradianceCubeDimension,
             ImageContext::CubeCompatible,
             4,
             6,
             1,
             static_cast<uint32_t>(floor(log2(Jkr::Renderer::PBR::IrradianceCubeDimension)) + 1),
             vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eTransferSrc,
             vk::ImageLayout::eUndefined);

        for (int mip = 0; mip < IrradianceCubeMap->GetImageProperty().mMipLevels; ++mip) {
            auto Vector = inJkrFile.Read<v<char>>(
                 (prefix + IrradianceCubeString + std::to_string(mip)).c_str());
            auto data = Vector.data();
            IrradianceCubeMap->SubmitImmediateCmdCopyFromDataWithStagingBuffer(
                 inInstance.GetTransferQueue(),
                 inInstance.GetUtilCommandBuffer(),
                 inInstance.GetDevice(),
                 inInstance.GetUtilCommandBufferFence(),
                 reinterpret_cast<void **>(&data),
                 Vector.size(),
                 inInstance.GetStagingBuffer(Vector.size()),
                 vk::ImageLayout::eShaderReadOnlyOptimal,
                 static_cast<int>(IrradianceCubeMap->GetImageExtent().width * std::pow(0.5f, mip)),
                 static_cast<int>(IrradianceCubeMap->GetImageExtent().height * std::pow(0.5f, mip)),
                 mip,
                 0,
                 6);
        }

        auto PrefilteredCubeMapImage = MakeUp<VulkanImageVMA>(
             inInstance.GetVMA(),
             inInstance.GetDevice(),
             Jkr::Renderer::PBR::PrefilteredCubeDimension,
             Jkr::Renderer::PBR::PrefilteredCubeDimension,
             ImageContext::CubeCompatible,
             4,
             6,
             1,
             static_cast<uint32_t>(floor(log2(Jkr::Renderer::PBR::PrefilteredCubeDimension)) + 1),
             vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eTransferSrc,
             vk::ImageLayout::eUndefined);

        for (int mip = 0; mip < PrefilteredCubeMapImage->GetImageProperty().mMipLevels; ++mip) {
            auto Vector = inJkrFile.Read<v<char>>(
                 (prefix + PrefilteredCubeString + std::to_string(mip)).c_str());
            auto data = Vector.data();
            PrefilteredCubeMapImage->SubmitImmediateCmdCopyFromDataWithStagingBuffer(
                 inInstance.GetTransferQueue(),
                 inInstance.GetUtilCommandBuffer(),
                 inInstance.GetDevice(),
                 inInstance.GetUtilCommandBufferFence(),
                 reinterpret_cast<void **>(&data),
                 Vector.size(),
                 inInstance.GetStagingBuffer(Vector.size()),
                 vk::ImageLayout::eShaderReadOnlyOptimal,
                 static_cast<int>(PrefilteredCubeMapImage->GetImageExtent().width *
                                  std::pow(0.5f, mip)),
                 static_cast<int>(PrefilteredCubeMapImage->GetImageExtent().height *
                                  std::pow(0.5f, mip)),
                 mip,
                 0,
                 6);
        }

        /// Load the PBR to Uniform
        {
            auto &SkyboxImages = inUniform3D.GetSkyboxImagesRef();
            Up<Jkr::PainterParameter<PainterParameterContext::SkyboxImage>> SkyboxImage =
                 MakeUp<Jkr::PainterParameter<PainterParameterContext::SkyboxImage>>(inInstance);
            SkyboxImage->mUniformImagePtr = std::move(CubeMultiMap);
            SkyboxImage->mSampler         = MakeUp<VulkanSampler>(inInstance.GetDevice());
            inUniform3D.AddSkyboxImage(*SkyboxImage); // This Registers it
            SkyboxImages.insert_or_assign(kstd::BindingIndex::CubeMapImage, mv(SkyboxImage));

            auto &UniformImages = inUniform3D.GetImagesRef();
            Up<Jkr::PainterParameter<PainterParameterContext::UniformImage>> BRDFlut =
                 MakeUp<Jkr::PainterParameter<PainterParameterContext::UniformImage>>(inInstance);
            BRDFlut->mUniformImagePtr = std::move(BRDFLUTImage);
            BRDFlut->mSampler =
                 MakeUp<VulkanSampler>(inInstance.GetDevice(), ImageContext::CubeCompatible);
            BRDFlut->Register(
                 0, inUniform3D.BrdfLUTBindingIndex, 0, inUniform3D.GetVulkanDescriptorSet());
            UniformImages[inUniform3D.BrdfLUTBindingIndex] = std::move(BRDFlut);

            Up<Jkr::PainterParameter<PainterParameterContext::UniformImage>> IrradianceCube =
                 MakeUp<Jkr::PainterParameter<PainterParameterContext::UniformImage>>(inInstance);
            IrradianceCube->mUniformImagePtr = std::move(IrradianceCubeMap);
            IrradianceCube->mSampler         = MakeUp<VulkanSampler>(
                 inInstance.GetDevice(),
                 ImageContext::CubeCompatible,
                 0.0,
                 static_cast<float>(static_cast<uint32_t>(floor(
                                         log2((float)Renderer::PBR::IrradianceCubeDimension))) +
                                    1));
            IrradianceCube->Register(0,
                                     inUniform3D.IrradianceCubeBindingIndex,
                                     0,
                                     inUniform3D.GetVulkanDescriptorSet());
            UniformImages[inUniform3D.IrradianceCubeBindingIndex] = std::move(IrradianceCube);

            Up<Jkr::PainterParameter<PainterParameterContext::UniformImage>> PrefilteredCube =
                 MakeUp<Jkr::PainterParameter<PainterParameterContext::UniformImage>>(inInstance);
            PrefilteredCube->mUniformImagePtr = std::move(PrefilteredCubeMapImage);
            PrefilteredCube->mSampler         = MakeUp<VulkanSampler>(
                 inInstance.GetDevice(),
                 ImageContext::CubeCompatible,
                 0.0,
                 static_cast<float>(static_cast<uint32_t>(floor(
                                         log2((float)Renderer::PBR::PrefilteredCubeDimension))) +
                                    1));

            PrefilteredCube->Register(0,
                                      inUniform3D.PrefilteredCubeBindingIndex,
                                      0,
                                      inUniform3D.GetVulkanDescriptorSet());
            UniformImages[inUniform3D.PrefilteredCubeBindingIndex] = std::move(PrefilteredCube);
        }

    } else {
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
                                               "cache2/" + s(inCachePrefix) +
                                                    "BRDFLookupTable.glsl",
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
                                               "cache2/" + s(inCachePrefix) +
                                                    "PrefilteredCube.glsl",
                                               inPrefilteredCube_vs,
                                               inPrefilteredCube_fs,
                                               "",
                                               false);

        {
            ///@warning @warning @warning @todo 4 is hardcoded here, Fix This
            /// actually 4 is hardcoded everywhere, sigh.
            ///@warning All 6 layers of images are copied
            inJkrFile.Write(
                 (prefix + CubeMultiMapString).c_str(),
                 CubeMultiMap->SubmitImmediateCmdGetImageToVector(
                      inInstance.GetTransferQueue(),
                      inInstance.GetStagingBuffer(CubeMultiMap->GetImageExtent().width *
                                                  CubeMultiMap->GetImageExtent().height * 4 * 6),
                      inInstance.GetUtilCommandBuffer(),
                      inInstance.GetUtilCommandBufferFence(),
                      vk::ImageLayout::eShaderReadOnlyOptimal,
                      -1,
                      -1,
                      0,
                      0,
                      6));

            auto &BRDFLookupTable =
                 inUniform3D.GetImagesRef()[inUniform3D.BrdfLUTBindingIndex]->GetUniformImage();
            inJkrFile.Write(
                 (prefix + BRDFLookupTableString).c_str(),
                 BRDFLookupTable.SubmitImmediateCmdGetImageToVector(
                      inInstance.GetTransferQueue(),
                      inInstance.GetStagingBuffer(BRDFLookupTable.GetImageExtent().width *
                                                  BRDFLookupTable.GetImageExtent().height * 4 * 6),
                      inInstance.GetUtilCommandBuffer(),
                      inInstance.GetUtilCommandBufferFence(),
                      vk::ImageLayout::eShaderReadOnlyOptimal,
                      -1,
                      -1,
                      0,
                      0,
                      1));

            auto &PrefilteredCube =
                 inUniform3D.GetImagesRef()[inUniform3D.PrefilteredCubeBindingIndex]
                      ->GetUniformImage();
            for (int mip = 0; mip < PrefilteredCube.GetImageProperty().mMipLevels; ++mip) {
                inJkrFile.Write((prefix + PrefilteredCubeString + to_string(mip)).c_str(),
                                PrefilteredCube.SubmitImmediateCmdGetImageToVector(
                                     inInstance.GetTransferQueue(),
                                     inInstance.GetStagingBuffer(
                                          PrefilteredCube.GetImageExtent().width *
                                          PrefilteredCube.GetImageExtent().height * 4 * 6),
                                     inInstance.GetUtilCommandBuffer(),
                                     inInstance.GetUtilCommandBufferFence(),
                                     vk::ImageLayout::eShaderReadOnlyOptimal,
                                     static_cast<int>(PrefilteredCube.GetImageExtent().width *
                                                      std::pow(0.5f, mip)),
                                     static_cast<int>(PrefilteredCube.GetImageExtent().height *
                                                      std::pow(0.5f, mip)),
                                     mip,
                                     0,
                                     6));
            }

            auto &IrradianceCube =
                 inUniform3D.GetImagesRef()[inUniform3D.IrradianceCubeBindingIndex]
                      ->GetUniformImage();

            for (int mip = 0; mip < IrradianceCube.GetImageProperty().mMipLevels; ++mip) {
                inJkrFile.Write((prefix + IrradianceCubeString + to_string(mip)).c_str(),
                                IrradianceCube.SubmitImmediateCmdGetImageToVector(
                                     inInstance.GetTransferQueue(),
                                     inInstance.GetStagingBuffer(
                                          IrradianceCube.GetImageExtent().width *
                                          IrradianceCube.GetImageExtent().height * 4 * 6),
                                     inInstance.GetUtilCommandBuffer(),
                                     inInstance.GetUtilCommandBufferFence(),
                                     vk::ImageLayout::eShaderReadOnlyOptimal,
                                     static_cast<int>(IrradianceCube.GetImageExtent().width *
                                                      std::pow(0.5f, mip)),
                                     static_cast<int>(IrradianceCube.GetImageExtent().height *
                                                      std::pow(0.5f, mip)),
                                     mip,
                                     0,
                                     6));
            }
        }

        Up<Jkr::PainterParameter<PainterParameterContext::SkyboxImage>> SkyboxImage =
             MakeUp<Jkr::PainterParameter<PainterParameterContext::SkyboxImage>>(inInstance);
        SkyboxImage->mUniformImagePtr = std::move(CubeMultiMap);
        SkyboxImage->mSampler         = MakeUp<VulkanSampler>(inInstance.GetDevice());

        inUniform3D.AddSkyboxImage(*SkyboxImage);

        auto &Images = inUniform3D.GetSkyboxImagesRef();
        Images.insert_or_assign(kstd::BindingIndex::CubeMapImage, mv(SkyboxImage));
    }
}

static auto ImgToFile(auto &ini, auto &inJkrFile, auto image_prefix, auto &img) {
    for (int mip = 0; mip < img.GetImageProperty().mMipLevels; ++mip) {
        ///@note Property is also being written here
        inJkrFile.Write((image_prefix + "PROPS").c_str(), img.GetImageProperty());
        inJkrFile.Write(
             (image_prefix + "IMAGE").data(),
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
    v<char> image_Vec = inJkrFile.Read<v<char>>((image_prefix + "IMAGE").c_str());
    void *data        = image_Vec.data();
    size_t size       = Props.mExtent.width * Props.mExtent.height * Props.mArrayLayers;
    for (int mip = 0; mip < Props.mMipLevels; ++mip) {
        Image->SubmitImmediateCmdCopyFromDataWithStagingBuffer(
             ini.GetTransferQueue(),
             ini.GetUtilCommandBuffer(),
             ini.GetDevice(),
             ini.GetUtilCommandBufferFence(),
             &data,
             size,
             ini.GetStagingBuffer(size),
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
            auto &images = inUniform3D.GetImagesRef();
            for (int i = 0; auto &[key, value] : images) {
                img_header.push_back(0);
                img_header[i] = key;
                ImgToFile(
                     ini, inJkrFile, image_prefix + std::to_string(key), value->GetUniformImage());
                ++i;
            }
            inJkrFile.Write(image_prefix.c_str(), img_header);
        }
        {
            auto &sboxes = inUniform3D.GetSkyboxImagesRef();
            Header sbox_header;
            for (int i = 0; auto &[key, value] : sboxes) {
                sbox_header.push_back(0);
                sbox_header[i] = key;
                ImgToFile(
                     ini, inJkrFile, skybox_prefix + std::to_string(key), value->GetUniformImage());
                ++i;
            }
            inJkrFile.Write(skybox_prefix.c_str(), sbox_header);
        }
        {
            auto &buffers = inUniform3D.GetUniformBuffersRef();
            Header ubuf_header;
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
                ++i;
            }
            inJkrFile.Write(image_prefix.c_str(), ubuf_header);
        }
        {
            auto &sbuffers = inUniform3D.GetStorageBuffersRef();
            Header sbuf_header;
            for (int i = 0; auto &[key, value] : sbuffers) {
                sbuf_header.push_back(0);
                sbuf_header[i] = key;
                inJkrFile.Write(
                     (storagebuf_prefix + std::to_string(key)).c_str(),
                     value->GetStorageBuffer().SubmitImmediateGetBufferToVector(
                          ini.GetTransferQueue(),
                          ini.GetStagingBuffer(value->GetStorageBuffer().GetBufferSize()),
                          ini.GetUtilCommandBuffer(),
                          ini.GetUtilCommandBufferFence()));
                ++i;
            }
            inJkrFile.Write(image_prefix.c_str(), sbuf_header);
        }

    } else {
        using ImageType         = Jkr::Renderer::_3D::Uniform3D::ImageType;
        using SkyboxImageType   = Jkr::Renderer::_3D::Uniform3D::SkyboxImageType;
        using UniformBufferType = Jkr::Renderer::_3D::Uniform3D::UniformBufferType;
        using StorageBufferType = Jkr::Renderer::_3D::Uniform3D::StorageBufferType;
        {
            Header img_header = inJkrFile.Read<Header>(image_prefix.c_str());
            for (char key : img_header) {
                auto Image = FileToImage(ini, inJkrFile, image_prefix + std::to_string(key));
                up<ImageType> Param     = mu<ImageType>(ini);
                Param->mSampler         = mu<VulkanSampler>(ini.GetDevice());
                Param->mUniformImagePtr = mv(Image);
                Param->Register(0, key, 0, inUniform3D.GetVulkanDescriptorSet());
                inUniform3D.GetImagesRef()[key] = mv(Param);
            }
        }
        {
            Header skbox_header = inJkrFile.Read<Header>(skybox_prefix.c_str());
            for (char key : skbox_header) {
                auto Image = FileToImage(ini, inJkrFile, skybox_prefix + std::to_string(key));
                up<SkyboxImageType> Param = mu<SkyboxImageType>(ini);
                Param->mSampler           = mu<VulkanSampler>(ini.GetDevice());
                Param->mUniformImagePtr   = mv(Image);
                Param->Register(0, key, 0, inUniform3D.GetVulkanDescriptorSet());
                inUniform3D.GetSkyboxImagesRef()[(int)key] = mv(Param);
            }
        }
        {
            Header ubuf_header = inJkrFile.Read<Header>(ubuffer_prefix.c_str());
            for (char key : ubuf_header) {
                auto Buffer =
                     inJkrFile.Read<v<char>>((ubuffer_prefix + std::to_string(key)).c_str());
                auto data                     = Buffer.data();
                up<VulkanBufferVMA> VMABuffer = mu<VulkanBufferVMA>();
                VMABuffer->Init({&ini.GetVMA(),
                                 &ini.GetDevice(),
                                 Buffer.size(),
                                 BufferContext::Uniform,
                                 MemoryType::HostVisibleAndCoherenet});
                up<UniformBufferType> Param = mu<UniformBufferType>(ini);
                Param->mUniformBufferPtr    = mv(VMABuffer);
                Param->Register(0, key, 0, inUniform3D.GetVulkanDescriptorSet());
                inUniform3D.GetUniformBuffersRef()[(int)key] = mv(Param);
            }
        }
        {
            Header sbuf_header = inJkrFile.Read<Header>(storagebuf_prefix.c_str());
            for (char key : sbuf_header) {
                auto Buffer =
                     inJkrFile.Read<v<char>>((storagebuf_prefix + std::to_string(key)).c_str());
                auto data                     = Buffer.data();
                up<VulkanBufferVMA> VMABuffer = mu<VulkanBufferVMA>();
                VMABuffer->Init({&ini.GetVMA(),
                                 &ini.GetDevice(),
                                 Buffer.size(),
                                 BufferContext::Storage,
                                 MemoryType::DeviceLocal});
                up<StorageBufferType> Param = mu<StorageBufferType>(ini);
                Param->mStorageBufferPtr    = mv(VMABuffer);
                Param->Register(0, key, 0, inUniform3D.GetVulkanDescriptorSet());
                inUniform3D.GetStorageBuffersRef()[(int)key] = mv(Param);
            }
        }
    }
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
    ///@note This should have been already added in the inJkrFile
    //     auto &Simple3Ds = inWorld3D.GetSimple3Ds();
    auto &Lights = inWorld3D.GetLights();
    auto Prefix  = s(inIdName);

    if (inJkrFile.GetFileContents().contains((Prefix))) {
        v<Renderer::_3D::Camera3D> Cams =
             inJkrFile.Read<decltype(Cams)>((Prefix + "CAMERAS").c_str());
        v<Renderer::_3D::World3D::Light3D> Lights =
             inJkrFile.Read<decltype(Lights)>((Prefix + "LIGHT3D").c_str());

        v<Renderer::_3D::Simple3D::CompileContext> CompileContexts =
             inJkrFile.Read<decltype(CompileContexts)>((Prefix + "COMPILE_CONTEXTS").c_str());

        for (int i = 0; i < CompileContexts.size(); ++i) {
            inWorld3D.AddSimple3D(ini, inw);
            auto &s3d = inWorld3D.GetSimple3Ds()[i];
            s3d->GetPainterCache().ReadFromCacheFile(inJkrFile,
                                                     Prefix + "SIMPLE3Ds" + std::to_string(i));
            s3d->Compile(ini, inw, " ", " ", " ", " ", true, CompileContexts[i]);
        }

        size_type uniformSize = inJkrFile.Read<size_type>((Prefix + "UNIFORM_SIZE").c_str());
        for (int i = 0; i < uniformSize; ++i) {
            inWorld3D.AddUniform3D(ini);
            auto Simple3DEntryName = inJkrFile.Read<std::string>(
                 (Prefix + "UNIFORMS" + std::to_string(i) + "SIMPLE3DINDEX").c_str());
            auto AssociatedSimple3D = inJkrFile.Read<int>(Simple3DEntryName.c_str());
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
            inJkrFile.Write((Prefix + "UNIFORMS" + std::to_string(i) + "SIMPLE3DINDEX").c_str(),
                            CacheFileEntryNameToWorldSimple3DIndex[Uniform->GetAssociatedSimple3D()
                                                                        ->GetPainterCache()
                                                                        .GetCacheFileEntryName()]);
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
    auto vid = s(inIdName) + "vbuffer";
    auto iid = s(inIdName) + "fbuffer";
    if (not inJkrFile.GetFileContents().contains(vid.c_str())) {
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
                                                      ini.GetStagingBuffer(vBuffer.GetBufferSize()),
                                                      ini.GetUtilCommandBuffer(),
                                                      ini.GetUtilCommandBufferFence());
        inJkrFile.Write(vid.c_str(), vBufferVector);
        inJkrFile.Write(iid.c_str(), iBufferVector);
    } else {
        up<Primitive> Pri = MakeUp<Primitive>(ini);
        auto vBuffer      = inJkrFile.Read<v<char>>(vid.c_str());
        auto iBuffer      = inJkrFile.Read<v<char>>(iid.c_str());
        ///@warning here, NON-Integrated GPUs are not handled, and will also not be handled for now
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

        Pri->GetIndexBufferPtr()->SubmitImmediateCmdCopyFrom(
             ini.GetTransferQueue(), ini.GetUtilCommandBuffer(), iBuffer.data());
        Pri->GetVertexBufferPtr()->SubmitImmediateCmdCopyFrom(
             ini.GetTransferQueue(), ini.GetUtilCommandBuffer(), vBuffer.data());
        Pri->SetIndexCount(iBuffer.size() / 4);

        inShape.GetPrimitivePtr() = mv(Pri);
    }
}

} // namespace Jkr::Misc