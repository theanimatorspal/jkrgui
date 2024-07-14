#include "Painter.hpp"
#include "Pipeline/VulkanPipelineLayout.hpp"

using namespace Jkr;

Jkr::Painter::Painter(const Instance& inInstance,
                      const Window_base& inWindow,
                      const PainterCache& inCache,
                      PipelineContext inPipelineContext)
    : mInstance(inInstance),
      mGUIPainterCache(inCache),
      mVulkanPipeline(inInstance.GetDevice()),
      mVulkanComputePipeline(inInstance.GetDevice()) {
    mVulkanPipeline.BuildPipeline(inCache.GetPipelineCache(),
                                  inCache.GetPipelineContext(),
                                  inWindow.GetRenderPass(),
                                  inCache.GetVertexFragmentPipelineLayout(),
                                  inCache.GetVertexFragmentShaderModulesArray(),
                                  inPipelineContext);
    mVulkanComputePipeline.BuildPipeline(inCache.GetPipelineCache(),
                                         inCache.GetPipelineContext(),
                                         inWindow.GetRenderPass(),
                                         inCache.GetComputePipelineLayout(),
                                         inCache.GetComputePipelineModule(),
                                         PipelineContext::Compute);
}

Jkr::Painter::Painter(const Instance& inInstance,
                      const Window_base& inWindow,
                      const PainterCache& inCache,
                      uint32_t inNoOfVariableDescriptorCount)
    : mInstance(inInstance),
      mGUIPainterCache(inCache),
      mVulkanPipeline(inInstance.GetDevice()),
      mVulkanComputePipeline(inInstance.GetDevice()) {

    mVulkanPipeline.BuildPipeline(inCache.GetPipelineCache(),
                                  inCache.GetPipelineContext(),
                                  inWindow.GetRenderPass(),
                                  inCache.GetVertexFragmentPipelineLayout(),
                                  inCache.GetVertexFragmentShaderModulesArray(),
                                  PipelineContext::Default);
    mVulkanComputePipeline.BuildPipeline(inCache.GetPipelineCache(),
                                         inCache.GetPipelineContext(),
                                         inWindow.GetRenderPass(),
                                         inCache.GetComputePipelineLayout(),
                                         inCache.GetComputePipelineModule(),
                                         PipelineContext::Compute);
}

Jkr::Painter::Painter(const Instance& inInstance,
                      const VulkanRenderPassBase& inRenderPass,
                      const PainterCache& inCache,
                      PipelineContext inPipelineContext)
    : mInstance(inInstance),
      mGUIPainterCache(inCache),
      mVulkanPipeline(inInstance.GetDevice()),
      mVulkanComputePipeline(inInstance.GetDevice()) {
    mVulkanPipeline.BuildPipeline(inCache.GetPipelineCache(),
                                  inCache.GetPipelineContext(),
                                  inRenderPass,
                                  inCache.GetVertexFragmentPipelineLayout(),
                                  inCache.GetVertexFragmentShaderModulesArray(),
                                  inPipelineContext);
    mVulkanComputePipeline.BuildPipeline(inCache.GetPipelineCache(),
                                         inCache.GetPipelineContext(),
                                         inRenderPass,
                                         inCache.GetComputePipelineLayout(),
                                         inCache.GetComputePipelineModule(),
                                         PipelineContext::Compute);
}

void Painter::OptimizeParameter(
     const Instance& inInstance,
     const PainterParameter<PainterParameterContext::StorageImage>& inImage,
     const Window_base& inWindow) {
    inInstance.GetUtilCommandBufferFence().Wait();
    inInstance.GetUtilCommandBufferFence().Reset();
    inInstance.GetUtilCommandBuffer().Begin();
    OptimizeImageParameter(inInstance, inInstance.GetUtilCommandBuffer(), inImage, inWindow);
    inInstance.GetUtilCommandBuffer().End();
    inInstance.GetGraphicsQueue().Submit<SubmitContext::SingleTime>(
         inInstance.GetUtilCommandBuffer(), inInstance.GetUtilCommandBufferFence());
}

void Jkr::Painter::OptimizeImageParameter(
     const Instance& inInstance,
     const VulkanCommandBuffer& inBuffer,
     const PainterParameter<PainterParameterContext::StorageImage>& inImage,
     const Window_base& inWindow) {
    inImage.GetStorageImage().CmdTransitionImageLayout(inBuffer,
                                                       vk::ImageLayout::eUndefined,
                                                       vk::ImageLayout::eGeneral,
                                                       vk::PipelineStageFlagBits::eVertexShader,
                                                       vk::PipelineStageFlagBits::eFragmentShader,
                                                       vk::AccessFlagBits::eNone,
                                                       vk::AccessFlagBits::eNone);
}

using namespace Jkr;

void Jkr::Painter::BindComputePipeline(const Window_base& inWindow, CmdParam inParam) {
    auto index = inWindow.GetCurrentFrame();
    auto& Cmd  = inWindow.GetCommandBuffers(inParam)[index];
    mVulkanComputePipeline.Bind<PipelineContext::Compute>(Cmd);
}

void Jkr::Painter::BindComputePipeline(VulkanCommandBuffer& inBuffer) {
    mVulkanComputePipeline.Bind<PipelineContext::Compute>(inBuffer);
}

void Painter::BindDrawPipeline(const Primitive& inPrimitive,
                               const Window_base& inWindow,
                               CmdParam inCmdParam) {
    auto index = inWindow.GetCurrentFrame();
    auto& Cmd  = inWindow.GetCommandBuffers(inCmdParam)[index];
    mVulkanPipeline.Bind<PipelineContext::Default>(Cmd);
}

void Painter::BindDrawPipeline(const Window_base& inWindow, CmdParam inCmdParam) {
    auto index = inWindow.GetCurrentFrame();
    auto& Cmd  = inWindow.GetCommandBuffers(inCmdParam)[index];
    mVulkanPipeline.Bind<PipelineContext::Default>(Cmd);
}

void Painter::BindDrawPipeline(VulkanCommandBuffer& inCommandBuffer) {
    mVulkanPipeline.Bind<PipelineContext::Default>(inCommandBuffer);
}