#pragma once
#include "Simple3D.hpp"
#include <Renderers/ThreeD/Shape3D.hpp>
#include <Instance.hpp>
#include <Painter.hpp>
#include <VulkanFrameBuffer.hpp>
#include <VulkanImageVMA.hpp>
#include <VulkanRenderPass.hpp>

namespace Jkr::Renderer {
using namespace ksai;
using namespace Jkr;
class PBR {
    static Up<VulkanImageVMA> GenerateBRDFLookupTable(Instance& inInstance,
                                                      WindowMulT& inWindow,
                                                      std::string_view inFileName,
                                                      std::string_view inVertexShader,
                                                      std::string_view inFragmentShader,
                                                      std::string_view inComputeShader,
                                                      bool inShouldLoad);
    static void GenerateIrradianceCube(Instance& inInstance,
                                       WindowMulT& inWindow,
                                       _3D::Shape& inShape,
                                       int inSkyboxIndex,
                                       int inSkyboxModelIndex,
                                       std::string_view inFileName,
                                       std::string_view inVertexShader,
                                       std::string_view inFragmentShader,
                                       std::string_view inComputeShader,
                                       bool inShouldLoad) {
        const uint32_t Dim     = 64;
        const uint32_t NumMips = static_cast<uint32_t>(floor(log2(Dim))) + 1;
        VulkanImageVMA IrradianceCubeMap(inInstance.GetVMA(),
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
                                             ImageContext::ColorAttach,
                                             4,
                                             1,
                                             1,
                                             1,
                                             vk::ImageUsageFlagBits::eTransferSrc);
        VulkanRenderPass<RenderPassContext::SingleColorAttachment> RenderPass(
             inInstance.GetDevice(), OffscreenFBufferImage);
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
                   IrradianceCubeMap,
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
             glm::rotate(
                  glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
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
        IrradianceCubeMap.CmdTransitionImageLayout(
             Cmd,
             vk::ImageLayout::eUndefined,
             vk::ImageLayout::eTransferDstOptimal,
             vk::PipelineStageFlagBits::eAllCommands,
             vk::PipelineStageFlagBits::eAllCommands, // TODO Improve this
             vk::AccessFlagBits::eNone,
             vk::AccessFlagBits::eNone);

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

                PushBlock.mvp =
                     glm::perspective((float)(M_PI / 2.0), 1.0f, 0.1f, 512.0f) * matrices[f];
                Simple3D.BindByCommandBuffer(Cmd);
                DescriptorSet.Bind(vk::PipelineBindPoint::eGraphics,
                                   Cmd,
                                   Simple3D.GetPainterCache().GetVertexFragmentPipelineLayout());
                Simple3D.DrawByCommandBuffer(
                     Cmd, inShape, PushBlock, 0, inShape.GetIndexCount(inSkyboxModelIndex), 1);
                Cmd.EndRenderPass();

                IrradianceCubeMap.CmdCopyImageFromImageAfterStage(
                     inInstance.GetGraphicsQueue(),
                     Cmd,
                     inInstance.GetDevice(),
                     OffscreenFBufferImage,
                     vk::PipelineStageFlagBits::eFragmentShader,
                     vk::AccessFlagBits::eNone,
                     0,
                     0,
                     m,
                     f);
            }
        }

        Cmd.End();

        // TODO Submit
        inInstance.GetGraphicsQueue().Wait();
    }
};
} // namespace Jkr::Renderer