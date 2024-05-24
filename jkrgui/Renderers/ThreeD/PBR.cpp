#include "PBR.hpp"
#include "Uniform3D.hpp"
#include "World3D.hpp"

namespace Jkr::Renderer {

Up<VulkanImageVMA> PBR::GenerateBRDFLookupTable(Instance& inInstance,
                                                WindowMulT& inWindow,
                                                std::string_view inFileName,
                                                std::string_view inVertexShader,
                                                std::string_view inFragmentShader,
                                                std::string_view inComputeShader,
                                                bool inShouldLoad) {
    const int Dim                              = 512;
    Up<VulkanImageVMA> ColorAttachmentImagePtr = MakeUp<VulkanImageVMA>(
         inInstance.GetVMA(),
         inInstance.GetDevice(),
         Dim,
         Dim,
         ImageContext::Default,
         4,
         1,
         1,
         1,
         vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eColorAttachment);
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

    Simple3D.BindByCommandBuffer(CommandBuffer);
    CommandBuffer.GetCommandBufferHandle().draw(3, 1, 0, 0);

    CommandBuffer.EndRenderPass();
    CommandBuffer.End();
    inInstance.GetGraphicsQueue().Submit<SubmitContext::SingleTime>(CommandBuffer);
    inInstance.GetGraphicsQueue().Wait();
    return mv(ColorAttachmentImagePtr);
}

Up<VulkanImageVMA> PBR::GenerateIrradianceCube(Instance& inInstance,
                                               WindowMulT& inWindow,
                                               _3D::Shape& inShape,
                                               int inSkyboxModelIndex,
                                               VulkanImageBase& inEnvironmentCubeMap,
                                               std::string_view inFileName,
                                               std::string_view inVertexShader,
                                               std::string_view inFragmentShader,
                                               std::string_view inComputeShader,
                                               bool inShouldLoad,
                                               _3D::World3D& inWorld3D) {
    const uint32_t Dim     = 64;
    const uint32_t NumMips = static_cast<uint32_t>(floor(log2(Dim))) + 1;
    auto IrradianceCubeMap = MakeUp<VulkanImageVMA>(inInstance.GetVMA(),
                                                    inInstance.GetDevice(),
                                                    Dim,
                                                    Dim,
                                                    ImageContext::CubeCompatible,
                                                    4,
                                                    6,
                                                    1,
                                                    NumMips,
                                                    vk::ImageUsageFlagBits::eTransferDst);

    VulkanImageVMA OffscreenFBufferImage(inInstance.GetVMA(),
                                         inInstance.GetDevice(),
                                         Dim,
                                         Dim,
                                         ImageContext::Default,
                                         4,
                                         1,
                                         1,
                                         1,
                                         vk::ImageUsageFlagBits::eTransferSrc |
                                              vk::ImageUsageFlagBits::eColorAttachment);
    VulkanRenderPass<RenderPassContext::SingleColorAttachment> RenderPass(inInstance.GetDevice(),
                                                                          OffscreenFBufferImage);
    VulkanFrameBuffer<1, VulkanImageVMA> FrameBuffer(
         inInstance.GetDevice(), RenderPass, OffscreenFBufferImage);

    _3D::Simple3D Simple3D(inInstance, inWindow);
    Simple3D.CompileWithCustomRenderPass(inInstance,
                                         inWindow,
                                         RenderPass,
                                         inFileName,
                                         inVertexShader,
                                         inFragmentShader,
                                         inComputeShader,
                                         inShouldLoad);

    VulkanDescriptorPool DescriptorPool(inInstance.GetDevice(), 10, 10);
    _3D::Uniform3D Uniform(inInstance);
    Uniform.Build(Simple3D, DescriptorPool);
    inWorld3D.AddWorldInfoToUniform3DEXT(Uniform);

    VulkanDescriptorUpdateHandler Handler(inInstance.GetDevice());
    VulkanSampler Sampler(
         inInstance.GetDevice(), ImageContext::Default, 0.0f, static_cast<float>(NumMips));

    Handler.RW(ImageContext::Default,
               Uniform.GetVulkanDescriptorSet(),
               inEnvironmentCubeMap,
               Sampler,
               kstd::BindingIndex::Uniform::CubeMapImage,
               0,
               1);

    using namespace glm;
    float DeltaPhi   = (2.0f * float(M_PI)) / 180.0f;
    float DeltaTheta = (0.5f * float(M_PI)) / 64.0f;

    struct PushBlock {
        mat4 mvp;
        mat4 m2 = mat4(vec4(), vec4(0), vec4(0), vec4(0));
    } PushBlock;

    std::vector<glm::mat4> matrices = {
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

    VulkanCommandBuffer Cmd(inInstance.GetDevice(), inInstance.GetCommandPool());
    Cmd.Begin();
    //     IrradianceCubeMap->CmdTransitionImageLayout(
    //          Cmd,
    //          vk::ImageLayout::eUndefined,
    //          vk::ImageLayout::eTransferDstOptimal,
    //          vk::PipelineStageFlagBits::eAllCommands,
    //          vk::PipelineStageFlagBits::eAllCommands, // TODO Improve this
    //          vk::AccessFlagBits::eNone,
    //          vk::AccessFlagBits::eNone);

    vk::Viewport Viewport((float)Dim, (float)Dim, 0.0f, 1.0f);
    vk::Rect2D Scissor(vk::Offset2D{0}, vk::Extent2D{Dim, Dim});

    auto Cmdh = Cmd.GetCommandBufferHandle();
    for (uint32_t m = 0; m < NumMips; m++) {
        for (uint32_t f = 0; f < 6; f++) {
            Viewport.width  = static_cast<float>(Dim * std::pow(0.5f, m));
            Viewport.height = static_cast<float>(Dim * std::pow(0.5f, m));
            Cmdh.setViewport(0, Viewport);
            Cmdh.setScissor(0, Scissor);
            Cmd.BeginRenderPass(RenderPass,
                                vk::Extent2D(Dim, Dim),
                                FrameBuffer,
                                std::array<float, 5>{0.0f, 0.0f, 0.2f, 0.0f});

            PushBlock.mvp = glm::perspective((float)(M_PI / 2.0), 1.0f, 0.1f, 512.0f) * matrices[f];
            inShape.GetPrimitive().GetVertexBufferPtr()->Bind<BufferContext::Vertex>(Cmd);
            inShape.GetPrimitive().GetIndexBufferPtr()->Bind<BufferContext::Index>(Cmd);
            Simple3D.BindByCommandBuffer(Cmd);

            Uniform.GetVulkanDescriptorSet().Bind(
                 vk::PipelineBindPoint::eGraphics,
                 Cmd,
                 Simple3D.GetPainterCache().GetVertexFragmentPipelineLayout(),
                 0);
            Uniform.GetVulkanDescriptorSet().Bind(
                 vk::PipelineBindPoint::eGraphics,
                 Cmd,
                 Simple3D.GetPainterCache().GetVertexFragmentPipelineLayout(),
                 1);
            Simple3D.DrawByCommandBuffer(
                 Cmd, inShape, PushBlock, 0, inShape.GetIndexCount(inSkyboxModelIndex), 1);
            Cmd.EndRenderPass();

            IrradianceCubeMap->GetImagePropertyRef().mInitialImageLayout =
                 vk::ImageLayout::eUndefined;
            OffscreenFBufferImage.GetImagePropertyRef().mInitialImageLayout =
                 vk::ImageLayout::eUndefined;

            IrradianceCubeMap->CmdCopyImageFromImageAfterStage(
                 Cmd,
                 inInstance.GetDevice(),
                 OffscreenFBufferImage,
                 vk::PipelineStageFlagBits::eFragmentShader,
                 vk::AccessFlagBits::eNone,
                 static_cast<int>(Viewport.width),
                 static_cast<int>(Viewport.height),
                 0,
                 0,
                 m,
                 f,
                 1);
        }
    }

    Cmd.End();
    inInstance.GetGraphicsQueue().Submit<SubmitContext::SingleTime>(Cmd);
    inInstance.GetGraphicsQueue().Wait();
    return mv(IrradianceCubeMap);
}

Up<VulkanImageVMA> PBR::GeneratePrefilteredCube(Instance& inInstance,
                                                WindowMulT& inWindow,
                                                _3D::Shape& inShape,
                                                int inSkyboxModelIndex,
                                                VulkanImageBase& inEnvironmentCubeMap,
                                                std::string_view inFileName,
                                                std::string_view inVertexShader,
                                                std::string_view inFragmentShader,
                                                std::string_view inComputeShader,
                                                bool inShouldLoad,
                                                _3D::World3D& inUniform) {
    const int32_t Dim            = 512;
    const uint32_t NumMips       = static_cast<uint32_t>(floor(log2(Dim))) + 1;
    auto PrefilteredCubeMapImage = MakeUp<VulkanImageVMA>(inInstance.GetVMA(),
                                                          inInstance.GetDevice(),
                                                          Dim,
                                                          Dim,
                                                          ImageContext::CubeCompatible,
                                                          4,
                                                          1,
                                                          1,
                                                          1,
                                                          vk::ImageUsageFlagBits::eTransferDst);

    VulkanImageVMA OffscreenImage(
         inInstance.GetVMA(), inInstance.GetDevice(), Dim, Dim, ImageContext::ColorAttach);
    VulkanRenderPass<RenderPassContext::SingleColorAttachment> RenderPass(inInstance.GetDevice(),
                                                                          OffscreenImage);
    VulkanFrameBuffer<1, VulkanImageVMA> FrameBuffer(
         inInstance.GetDevice(), RenderPass, OffscreenImage);
    VulkanCommandBuffer Cmd(inInstance.GetDevice(), inInstance.GetCommandPool());
    Cmd.Begin();
    OffscreenImage.CmdTransitionImageLayout(
         Cmd,
         vk::ImageLayout::eUndefined,
         vk::ImageLayout::eColorAttachmentOptimal,
         vk::PipelineStageFlagBits::eAllCommands,
         vk::PipelineStageFlagBits::eAllCommands, // TODO Improve this
         vk::AccessFlagBits::eNone,
         vk::AccessFlagBits::eNone);
    Cmd.End();
    inInstance.GetGraphicsQueue().Submit<SubmitContext::SingleTime>(Cmd);
    inInstance.GetGraphicsQueue().Wait();

    _3D::Simple3D Simple3D(inInstance, inWindow);
    Simple3D.CompileWithCustomRenderPass(inInstance,
                                         inWindow,
                                         RenderPass,
                                         inFileName,
                                         inVertexShader,
                                         inFragmentShader,
                                         inComputeShader,
                                         inShouldLoad);

    VulkanDescriptorPool DescriptorPool(inInstance.GetDevice(), 1, 1);
    VulkanDescriptorSet DescriptorSet(
         inInstance.GetDevice(),
         DescriptorPool,
         Simple3D.GetPainterCache().GetVertexFragmentDescriptorSetLayout());
    VulkanDescriptorUpdateHandler Handler(inInstance.GetDevice());
    VulkanSampler Sampler(
         inInstance.GetDevice(), ImageContext::Default, 0.0f, static_cast<float>(NumMips));
    Handler.RW(ImageContext::Default,
               DescriptorSet,
               inEnvironmentCubeMap,
               Sampler,
               kstd::BindingIndex::Uniform::CubeMapImage,
               0,
               1);
    struct PushBlock {
        glm::mat4 mvp;
        glm::mat4 m2;
    } PushBlock;
    using namespace glm;

    std::vector<glm::mat4> Matrices = {
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

    Cmd.Begin();
    PrefilteredCubeMapImage->CmdTransitionImageLayout(
         Cmd,
         vk::ImageLayout::eUndefined,
         vk::ImageLayout::eTransferDstOptimal,
         vk::PipelineStageFlagBits::eAllCommands,
         vk::PipelineStageFlagBits::eAllCommands, // TODO Improve this
         vk::AccessFlagBits::eNone,
         vk::AccessFlagBits::eNone);

    Cmd.BeginRenderPass(RenderPass,
                        vk::Extent2D{Dim, Dim},
                        FrameBuffer,
                        std::array<float, 5>{0.0f, 0.0f, 0.2f, 0.0f, 1.0f});

    vk::Viewport Viewport((float)Dim, (float)Dim, 0.0f, 1.0f);
    vk::Rect2D Scissor(vk::Offset2D{0}, vk::Extent2D{Dim, Dim});

    auto Cmdh = Cmd.GetCommandBufferHandle();
    for (uint32_t m = 0; m < NumMips; m++) {
        float roughness = (float)m / (float)(NumMips - 1);
        PushBlock.m2[0] = glm::vec4(roughness, 0, 0, 0);
        for (uint32_t f = 0; f < 6; f++) {
            Viewport.width  = static_cast<float>(Dim * std::pow(0.5f, m));
            Viewport.height = static_cast<float>(Dim * std::pow(0.5f, m));
            Cmdh.setViewport(0, Viewport);
            Cmdh.setScissor(0, Scissor);
            PushBlock.mvp = glm::perspective((float)(M_PI / 2.0), 1.0f, 0.1f, 512.0f) * Matrices[f];

            Simple3D.BindByCommandBuffer(Cmd);
            DescriptorSet.Bind(vk::PipelineBindPoint::eGraphics,
                               Cmd,
                               Simple3D.GetPainterCache().GetVertexFragmentPipelineLayout());
            Simple3D.DrawByCommandBuffer(
                 Cmd, inShape, PushBlock, 0, inShape.GetIndexCount(inSkyboxModelIndex), 1);
            PrefilteredCubeMapImage->CmdCopyImageFromImageAfterStage(
                 Cmd,
                 inInstance.GetDevice(),
                 OffscreenImage,
                 vk::PipelineStageFlagBits::eFragmentShader,
                 vk::AccessFlagBits::eNone,
                 0,
                 0,
                 m,
                 f);
        }
    }
    Cmd.EndRenderPass();
    Cmd.End();

    return mv(PrefilteredCubeMapImage);
}
//
} // namespace Jkr::Renderer