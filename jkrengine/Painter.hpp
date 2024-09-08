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
    Painter(const Instance &inInstance,
            const Window_base &inWindow,
            const PainterCache &inCache,
            PipelineContext inPipelineContext = PipelineContext::Default);
    Painter(const Instance &inInstance,
            const Window_base &inWindow,
            const PainterCache &inCache,
            uint32_t inNoOfVariableDescriptorCount);
    Painter(const Instance &inInstance,
            const VulkanRenderPassBase &inRenderPass,
            const PainterCache &inCache,
            PipelineContext inPipelineContext = PipelineContext::Default);

    void BindDrawPipeline(const Primitive &inPrimitive, const Window_base &inWindow, CmdParam inCmdContext = CmdParam::UI);
    void BindDrawPipeline(const Window_base &inWindow, CmdParam inCmdContext = CmdParam::UI);
    void BindDrawPipeline(const VulkanCommandBuffer &inBuffer);

    template <class PushType>
    void Draw_EXT(const vk::ArrayProxy<PushType> inPushConstants,
                  const Window_base &inWindow,
                  uint32_t inIndexCount,
                  uint32_t inInstanceCount,
                  uint32_t inFirstIndex,
                  uint32_t inFirstInstance,
                  CmdParam inCmdContext = CmdParam::UI);

    template <class PushType>
    void Draw_EXT(const vk::ArrayProxy<PushType> inPushConstants,
                  VulkanCommandBuffer &inCmdBuffer,
                  uint32_t inIndexCount,
                  uint32_t inInstanceCount,
                  uint32_t inFirstIndex,
                  uint32_t inFirstInstance);

    void BindComputePipeline(const Window_base &inWindow, CmdParam inCmdContext = CmdParam::UI);
    void BindComputePipeline(VulkanCommandBuffer &inBuffer);

    template <class PushType>
    void Dispatch_EXT(Window_base &inWindow,
                      const vk::ArrayProxy<PushType> inPushConstants,
                      uint32_t inCountX,
                      uint32_t inCountY,
                      uint32_t inCountZ,
                      CmdParam inCmdContext = CmdParam::UI);

    template <class PushType>
    void Dispatch_EXT(VulkanCommandBuffer &inCmdBuffer,
                      const vk::ArrayProxy<PushType> inPushConstants,
                      uint32_t inCountX,
                      uint32_t inCountY,
                      uint32_t inCountZ);

    static void OptimizeParameter(const Instance &inInstance,
                                  const PainterParameter<PainterParameterContext::StorageImage> &inImage,
                                  const Window_base &inWindow);

    private:
    static void OptimizeImageParameter(const Instance &inInstance,
                                       const VulkanCommandBuffer &inBuffer,
                                       const PainterParameter<PainterParameterContext::StorageImage> &inImage,
                                       const Window_base &inWindow);
    const Instance &mInstance;
    const PainterCache &mGUIPainterCache;
    VulkanPipelineBase mVulkanPipeline;
    VulkanPipelineBase mVulkanComputePipeline;
};

template <class PushType>
void Painter::Dispatch_EXT(Window_base &inWindow,
                           const vk::ArrayProxy<PushType> inPushConstants,
                           uint32_t inCountX,
                           uint32_t inCountY,
                           uint32_t inCountZ,
                           CmdParam inCmdParam) {
    auto &Cmd = inWindow.GetCommandBuffers(inCmdParam)[inWindow.GetCurrentFrame()];
    Cmd.PushConstants<PushType>(mGUIPainterCache.GetComputePipelineLayout(), inPushConstants);
    Cmd.GetCommandBufferHandle().dispatch(inCountX, inCountY, inCountZ);
}

template <class PushType>
void Painter::Draw_EXT(const vk::ArrayProxy<PushType> inPushConstants,
                       const Window_base &inWindow,
                       uint32_t inIndexCount,
                       uint32_t inInstanceCount,
                       uint32_t inFirstIndex,
                       uint32_t inFirstInstance,
                       CmdParam inCmdParam) {
    auto &Cmd = inWindow.GetCommandBuffers(inCmdParam)[inWindow.GetCurrentFrame()];
    Cmd.PushConstants<PushType>(mGUIPainterCache.GetVertexFragmentPipelineLayout(), inPushConstants);
    mVulkanPipeline.DrawIndexed(Cmd, inIndexCount, inInstanceCount, inFirstIndex, 0, inFirstInstance);
}

template <class PushType>
void Painter::Draw_EXT(const vk::ArrayProxy<PushType> inPushConstants,
                       VulkanCommandBuffer &inCmdBuffer,
                       uint32_t inIndexCount,
                       uint32_t inInstanceCount,
                       uint32_t inFirstIndex,
                       uint32_t inFirstInstance) {

    inCmdBuffer.PushConstants<PushType>(mGUIPainterCache.GetVertexFragmentPipelineLayout(), inPushConstants);
    mVulkanPipeline.DrawIndexed(inCmdBuffer, inIndexCount, inInstanceCount, inFirstIndex, 0, inFirstInstance);
}

template <class PushType>
void Painter::Dispatch_EXT(VulkanCommandBuffer &inCmdBuffer,
                           const vk::ArrayProxy<PushType> inPushConstants,
                           uint32_t inCountX,
                           uint32_t inCountY,
                           uint32_t inCountZ) {
    inCmdBuffer.PushConstants<PushType>(mGUIPainterCache.GetComputePipelineLayout(), inPushConstants);
    inCmdBuffer.GetCommandBufferHandle().dispatch(inCountX, inCountY, inCountZ);
}

} // namespace Jkr
