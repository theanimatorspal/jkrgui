#pragma once
#include "PainterCache.hpp"
#include "PainterParameter.hpp"
#include "Primitive.hpp"
#include "Window.hpp"

namespace Jkr {

enum class RegisterMode { AllShaders, VertexFragmentOnly, ComputeOnly };

class Painter {
          using CmdParam = Window::ParameterContext;

      public:
          Painter(const Instance& inInstance, const Window& inWindow, const PainterCache& inCache);
          Painter(const Instance& inInstance, const Window& inWindow, const PainterCache& inCache, uint32_t inNoOfVariableDescriptorCount);

          template <class PushType>
          void Draw(const Primitive& inPrimitive,
                    const vk::ArrayProxy<PushType> inPushConstants,
                    const Window& inWindow,
                    CmdParam inCmdParam = CmdParam::UI);
          template <class PushType> void Draw(const Primitive& inPrimitive, const vk::ArrayProxy<PushType> inPushConstants);

          void BindDrawParamters_EXT(const Primitive& inPrimitive, const Window& inWindow, CmdParam inCmdContext = CmdParam::UI);
          void BindDrawParamtersDescriptorsOnly_EXT(const Primitive& inPrimitive, const Window& inWindow, CmdParam inCmdContext = CmdParam::UI);
          static void BindDrawParamtersVertexAndIndexBuffersOnly_EXT(const Instance& inInstance,
                                                                     const Primitive& inPrimitive,
                                                                     const Window& inWindow,
                                                                     CmdParam inCmdContext = CmdParam::UI);

          void BindDrawParamtersPipelineOnly_EXT(const Primitive& inPrimitive, const Window& inWindow, CmdParam inCmdContext = CmdParam::UI);
          void BindDrawParamtersPipelineOnly_EXT(const Window& inWindow, CmdParam inCmdContext = CmdParam::UI);

          template <class PushType>
          void Draw_EXT(const Primitive& inPrimitive,
                        const vk::ArrayProxy<PushType> inPushConstants,
                        const Window& inWindow,
                        uint32_t inIndexCount,
                        uint32_t inInstanceCount,
                        uint32_t inFirstIndex,
                        uint32_t inFirstInstance,
                        CmdParam inCmdContext = CmdParam::UI);
          template <class PushType>
          void Draw_EXT(const Primitive& inPrimitive,
                        const vk::ArrayProxy<PushType> inPushConstants,
                        uint32_t inIndexCount,
                        uint32_t inInstanceCount,
                        uint32_t inFirstIndex,
                        uint32_t inFirstInstance,
                        CmdParam inCmdContext = CmdParam::UI);

          template <class PushType>
          void Dispatch(const vk::ArrayProxy<PushType> inPushConstants,
                        uint32_t inCountX,
                        uint32_t inCountY,
                        uint32_t inCountZ,
                        CmdParam inCmdContext = CmdParam::UI);

          void BindDispatchParameters_EXT(const Window& inWindow, CmdParam inCmdContext = CmdParam::UI);
          void BindDispatchParametersDescriptorsOnly_EXT(const Window& inWindow, CmdParam inCmdContext = CmdParam::UI);
          void BindDispatchParametersPipelineOnly_EXT(const Window& inWindow, CmdParam inCmdContext = CmdParam::UI);

          template <class PushType>
          void Dispatch_EXT(const vk::ArrayProxy<PushType> inPushConstants,
                            uint32_t inCountX,
                            uint32_t inCountY,
                            uint32_t inCountZ,
                            CmdParam inCmdContext = CmdParam::UI);

          template <PainterParameterContext inContext>
          void RegisterPainterParameter(const PainterParameter<inContext>& inPainterParameter,
                                        vk::DeviceSize inOffset,
                                        uint32_t inDstBinding,
                                        uint32_t inDstArrayElement,
                                        RegisterMode inRegisterMode = RegisterMode::AllShaders);

          static void OptimizeParameter(const Instance& inInstance,
                                        const PainterParameter<PainterParameterContext::StorageImage>& inImage,
                                        const Window& inWindow);

      private:
          static void OptimizeImageParameter(const Instance& inInstance,
                                             const VulkanCommandBuffer& inBuffer,
                                             const PainterParameter<PainterParameterContext::StorageImage>& inImage,
                                             const Window& inWindow);
          const Instance& mInstance;
          const Window& mWindow;
          const PainterCache& mGUIPainterCache;
          VulkanPipeline<2, PipelineContext::Default> mVulkanPipeline;
          VulkanPipeline<1, PipelineContext::Compute> mVulkanComputePipeline;

      private:
          VulkanDescriptorSet mComputeDescriptorSet;
          VulkanDescriptorSet mVertexFragmentDescriptorSet;
          VulkanDescriptorUpdateHandler mDescriptorUpdateHandler;
};

inline void Painter::BindDrawParamtersDescriptorsOnly_EXT(const Primitive& inPrimitive, const Window& inWindow, CmdParam inCmdParam) {
          auto& Cmd = mWindow.GetCommandBuffers(inCmdParam)[inWindow.GetCurrentFrame()];
          Cmd.GetCommandBufferHandle().bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                                          mGUIPainterCache.GetVertexFragmentPipelineLayout().GetPipelineLayoutHandle(),
                                                          0,
                                                          mVertexFragmentDescriptorSet.GetDescriptorSetHandle(),
                                                          {});
}

inline void Painter::BindDrawParamtersVertexAndIndexBuffersOnly_EXT(const Instance& inInstance,
                                                                    const Primitive& inPrimitive,
                                                                    const Window& inWindow,
                                                                    CmdParam inCmdParam) {
          auto& Cmd = inWindow.GetCommandBuffers(inCmdParam)[inWindow.GetCurrentFrame()];
          inPrimitive.GetVertexBufferPtr()->Bind<BufferContext::Vertex>(Cmd);
          inPrimitive.GetIndexBufferPtr()->Bind<BufferContext::Index>(Cmd);
}

// TODO to be removed
inline void Painter::BindDrawParamtersPipelineOnly_EXT(const Primitive& inPrimitive, const Window& inWindow, CmdParam inCmdParam) {
          auto index = inWindow.GetCurrentFrame();
          auto& Cmd = inWindow.GetCommandBuffers(inCmdParam)[index];
          mVulkanPipeline.Bind<PipelineContext::Default>(Cmd);
}

inline void Painter::BindDrawParamtersPipelineOnly_EXT(const Window& inWindow, CmdParam inCmdParam) {
          auto index = inWindow.GetCurrentFrame();
          auto& Cmd = inWindow.GetCommandBuffers(inCmdParam)[index];
          mVulkanPipeline.Bind<PipelineContext::Default>(Cmd);
}

template <class PushType>
void Painter::Dispatch_EXT(
 const vk::ArrayProxy<PushType> inPushConstants, uint32_t inCountX, uint32_t inCountY, uint32_t inCountZ, CmdParam inCmdParam) {
          auto& Cmd = mWindow.GetCommandBuffers(inCmdParam)[mWindow.GetCurrentFrame()];
          Cmd.PushConstants<PushType>(mGUIPainterCache.GetComputePipelineLayout(), inPushConstants);
          Cmd.GetCommandBufferHandle().dispatch(inCountX, inCountY, inCountZ);
}

template <class PushType>
void Painter::Draw_EXT(const Primitive& inPrimitive,
                       const vk::ArrayProxy<PushType> inPushConstants,
                       const Window& inWindow,
                       uint32_t inIndexCount,
                       uint32_t inInstanceCount,
                       uint32_t inFirstIndex,
                       uint32_t inFirstInstance,
                       CmdParam inCmdParam) {
          auto& Cmd = inWindow.GetCommandBuffers(inCmdParam)[inWindow.GetCurrentFrame()];
          Cmd.PushConstants<PushType>(mGUIPainterCache.GetVertexFragmentPipelineLayout(), inPushConstants);
          mVulkanPipeline.DrawIndexed(Cmd, inIndexCount, inInstanceCount, inFirstIndex, 0, inFirstInstance);
}

template <class PushType>
void Painter::Dispatch(const vk::ArrayProxy<PushType> inPushConstants, uint32_t inCountX, uint32_t inCountY, uint32_t inCountZ, CmdParam inCmdParam) {
          auto& Cmd = mWindow.GetCommandBuffers(inCmdParam)[mWindow.GetCurrentFrame()];
          Cmd.GetCommandBufferHandle().bindDescriptorSets(vk::PipelineBindPoint::eCompute,
                                                          mGUIPainterCache.GetComputePipelineLayout().GetPipelineLayoutHandle(),
                                                          0,
                                                          mComputeDescriptorSet.GetDescriptorSetHandle(),
                                                          {});
          mVulkanComputePipeline.Bind<PipelineContext::Compute>(Cmd);
          Cmd.PushConstants<PushType>(mGUIPainterCache.GetVertexFragmentPipelineLayout(), inPushConstants);
          Cmd.GetCommandBufferHandle().dispatch(inCountX, inCountY, inCountZ);
}

template <class PushType> void Painter::Draw(const Primitive& inPrimitive, const vk::ArrayProxy<PushType> inPushConstants) {
          Draw(inPrimitive, inPushConstants, mWindow);
}

template <class PushType>
void Painter::Draw(const Primitive& inPrimitive, const vk::ArrayProxy<PushType> inPushConstants, const Window& inWindow, CmdParam inCmdParam) {
          auto& Cmd = mWindow.GetCommandBuffers(inCmdParam)[inWindow.GetCurrentFrame()];
          inPrimitive.GetVertexBufferPtr()->Bind<BufferContext::Vertex>(Cmd);
          inPrimitive.GetIndexBufferPtr()->Bind<BufferContext::Index>(Cmd);
          Cmd.GetCommandBufferHandle().bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                                          mGUIPainterCache.GetVertexFragmentPipelineLayout().GetPipelineLayoutHandle(),
                                                          0,
                                                          mVertexFragmentDescriptorSet.GetDescriptorSetHandle(),
                                                          {});
          Cmd.PushConstants<PushType>(mGUIPainterCache.GetVertexFragmentPipelineLayout(), inPushConstants);
          mVulkanPipeline.Bind<PipelineContext::Default>(Cmd);
          mVulkanPipeline.DrawIndexed(Cmd, inPrimitive.GetIndexCount(), 1, 0, 0, 0);
}

} // namespace Jkr
