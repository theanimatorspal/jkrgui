#pragma once
#include "PainterCache.hpp"
#include "PainterParameter.hpp"
#include "Pipeline/VulkanPipeline.hpp"
#include "Pipeline/VulkanPipelineLayout.hpp"
#include "Primitive.hpp"
#include "VulkanRenderPass.hpp"
#include "Window_base.hpp"

namespace Jkr {

class Painter {
    using CmdParam = Window_base::ParameterContext;

    public:
    Painter(Instance &inInstance,
            Window_base &inWindow,
            PainterCache &inCache,
            PipelineContext inPipelineContext = PipelineContext::Default);
    Painter(Instance &inInstance,
            Window_base &inWindow,
            PainterCache &inCache,
            uint32_t inNoOfVariableDescriptorCount);
    Painter(Instance &inInstance,
            VulkanRenderPassBase &inRenderPass,
            PainterCache &inCache,
            PipelineContext inPipelineContext = PipelineContext::Default);

    void BindDrawPipeline(Primitive &inPrimitive,
                          Window_base &inWindow,
                          CmdParam inCmdContext = CmdParam::UI);
    void BindDrawPipeline(Window_base &inWindow, CmdParam inCmdContext = CmdParam::UI);
    void BindDrawPipeline(VulkanCommandBuffer &inBuffer);

    template <class PushType>
    void Draw_EXT(vk::ArrayProxy<PushType> inPushConstants,
                  Window_base &inWindow,
                  uint32_t inIndexCount,
                  uint32_t inInstanceCount,
                  uint32_t inFirstIndex,
                  uint32_t inFirstInstance,
                  CmdParam inCmdContext = CmdParam::UI);

    template <class PushType>
    void Draw_EXT(vk::ArrayProxy<PushType> inPushConstants,
                  VulkanCommandBuffer &inCmdBuffer,
                  uint32_t inIndexCount,
                  uint32_t inInstanceCount,
                  uint32_t inFirstIndex,
                  uint32_t inFirstInstance);

    void BindComputePipeline(Window_base &inWindow, CmdParam inCmdContext = CmdParam::UI);
    void BindComputePipeline(VulkanCommandBuffer &inBuffer);

    template <class PushType>
    void Dispatch_EXT(Window_base &inWindow,
                      vk::ArrayProxy<PushType> inPushConstants,
                      uint32_t inCountX,
                      uint32_t inCountY,
                      uint32_t inCountZ,
                      CmdParam inCmdContext = CmdParam::UI);

    template <class PushType>
    void Dispatch_EXT(VulkanCommandBuffer &inCmdBuffer,
                      vk::ArrayProxy<PushType> inPushConstants,
                      uint32_t inCountX,
                      uint32_t inCountY,
                      uint32_t inCountZ);

    static void OptimizeParameter(Instance &inInstance,
                                  PainterParameter<PainterParameterContext::StorageImage> &inImage,
                                  Window_base &inWindow);

    private:
    static void
    OptimizeImageParameter(Instance &inInstance,
                           VulkanCommandBuffer &inBuffer,
                           PainterParameter<PainterParameterContext::StorageImage> &inImage,
                           Window_base &inWindow);
    Instance &mInstance;
    PainterCache &mGUIPainterCache;
    VulkanPipelineBase mVulkanPipeline;
    VulkanPipelineBase mVulkanComputePipeline;
};

template <class PushType>
void Painter::Dispatch_EXT(Window_base &inWindow,
                           vk::ArrayProxy<PushType> inPushConstants,
                           uint32_t inCountX,
                           uint32_t inCountY,
                           uint32_t inCountZ,
                           CmdParam inCmdParam) {
    auto &Cmd = inWindow.GetCommandBuffers(inCmdParam)[inWindow.GetCurrentFrame()];
    Cmd.PushConstants<PushType>(mGUIPainterCache.GetComputePipelineLayout(), inPushConstants);
    Cmd.GetCommandBufferHandle().dispatch(inCountX, inCountY, inCountZ);
}

template <class PushType>
void Painter::Draw_EXT(vk::ArrayProxy<PushType> inPushConstants,
                       Window_base &inWindow,
                       uint32_t inIndexCount,
                       uint32_t inInstanceCount,
                       uint32_t inFirstIndex,
                       uint32_t inFirstInstance,
                       CmdParam inCmdParam) {
    auto &Cmd = inWindow.GetCommandBuffers(inCmdParam)[inWindow.GetCurrentFrame()];
    Cmd.PushConstants<PushType>(mGUIPainterCache.GetVertexFragmentPipelineLayout(),
                                inPushConstants);
    mVulkanPipeline.DrawIndexed(
         Cmd, inIndexCount, inInstanceCount, inFirstIndex, 0, inFirstInstance);
}

template <class PushType>
void Painter::Draw_EXT(vk::ArrayProxy<PushType> inPushConstants,
                       VulkanCommandBuffer &inCmdBuffer,
                       uint32_t inIndexCount,
                       uint32_t inInstanceCount,
                       uint32_t inFirstIndex,
                       uint32_t inFirstInstance) {

    inCmdBuffer.PushConstants<PushType>(mGUIPainterCache.GetVertexFragmentPipelineLayout(),
                                        inPushConstants);
    mVulkanPipeline.DrawIndexed(
         inCmdBuffer, inIndexCount, inInstanceCount, inFirstIndex, 0, inFirstInstance);
}

template <class PushType>
void Painter::Dispatch_EXT(VulkanCommandBuffer &inCmdBuffer,
                           vk::ArrayProxy<PushType> inPushConstants,
                           uint32_t inCountX,
                           uint32_t inCountY,
                           uint32_t inCountZ) {
    inCmdBuffer.PushConstants<PushType>(mGUIPainterCache.GetComputePipelineLayout(),
                                        inPushConstants);
    inCmdBuffer.GetCommandBufferHandle().dispatch(inCountX, inCountY, inCountZ);
}

} // namespace Jkr
