#pragma once
#include "Window.hpp"
#include "Primitive.hpp"
#include "PainterCache.hpp"
#include "PainterParameter.hpp"

namespace Jkr {

    class PainterBase
	{
	public:
	private:
	};


	class Painter
	{
	public:
		Painter(const Instance& inInstance, const Window& inWindow, const PainterCache& inCache);
		Painter(const Instance& inInstance, const Window& inWindow, const PainterCache& inCache, uint32_t inNoOfVariableDescriptorCount);

        template<class PushType>
        void Draw(const Primitive &inPrimitive,
                  const vk::ArrayProxy<PushType> inPushConstants,
                  const Window &inWindow);
        template<class PushType>
        void Draw(const Primitive &inPrimitive, const vk::ArrayProxy<PushType> inPushConstants);

        template<class PushType>
        void BindDrawParamters_EXT(const Primitive &inPrimitive, const Window &inWindow);

        template<class PushType>
        void BindDrawParamtersDescriptorsOnly_EXT(const Primitive &inPrimitive,
                                                  const Window &inWindow);

        template<class PushType>
        static void BindDrawParamtersVertexAndIndexBuffersOnly_EXT(const Instance &inInstance,
                                                                   const Primitive &inPrimitive,
                                                                   const Window &inWindow);

        template<class PushType>
        void BindDrawParamtersPipelineOnly_EXT(const Primitive &inPrimitive, const Window &inWindow);

        template<class PushType>
        void BindDrawParamters_EXT(const Primitive &inPrimitive);

        template<class PushType>
        void Draw_EXT(const Primitive &inPrimitive,
                      const vk::ArrayProxy<PushType> inPushConstants,
                      const Window &inWindow,
                      uint32_t inIndexCount,
                      uint32_t inInstanceCount,
                      uint32_t inFirstIndex,
                      uint32_t inFirstInstance);
        template<class PushType>
        void Draw_EXT(const Primitive &inPrimitive,
                      const vk::ArrayProxy<PushType> inPushConstants,
                      uint32_t inIndexCount,
                      uint32_t inInstanceCount,
                      uint32_t inFirstIndex,
                      uint32_t inFirstInstance);

        template<class PushType>
        void Dispatch(const vk::ArrayProxy<PushType> inPushConstants,
                      uint32_t inCountX,
                      uint32_t inCountY,
                      uint32_t inCountZ);

        template<class PushType>
        void BindDispatchParameters_EXT(const Window &inWindow);

        template<class PushType>
        void BindDispatchParameters_EXT();

        template<class PushType>
        void Dispatch_EXT(const vk::ArrayProxy<PushType> inPushConstants,
                          uint32_t inCountX,
                          uint32_t inCountY,
                          uint32_t inCountZ);

        template<PainterParameterContext inContext>
        void RegisterPainterParameter(const PainterParameter<inContext> &inPainterParameter,
                                      vk::DeviceSize inOffset,
                                      uint32_t inDstBinding,
                                      uint32_t inDstArrayElement);

        void OptimizeParameter(const PainterParameter<PainterParameterContext::StorageImage> &inImage,
                               const Window &inWindow);

    private:
        void OptimizeImageParameter(
            const VulkanCommandBuffer &inBuffer,
            const PainterParameter<PainterParameterContext::StorageImage> &inImage,
            const Window &inWindow);
        const Instance &mInstance;
        const Window &mWindow;
        const PainterCache &mGUIPainterCache;
        VulkanPipeline<2, PipelineContext::Default> mVulkanPipeline;
        VulkanPipeline<1, PipelineContext::Compute> mVulkanComputePipeline;

    private:
        VulkanDescriptorSet mComputeDescriptorSet;
        VulkanDescriptorSet mVertexFragmentDescriptorSet;
        VulkanDescriptorUpdateHandler mDescriptorUpdateHandler;
    };

    template<class PushType>
    void Painter::Dispatch_EXT(const vk::ArrayProxy<PushType> inPushConstants,
                               uint32_t inCountX,
                               uint32_t inCountY,
                               uint32_t inCountZ)
    {
        auto &Cmd = mWindow.GetCommandBuffers()[mWindow.GetCurrentFrame()];
        Cmd.PushConstants<PushType>(mGUIPainterCache.GetVertexFragmentPipelineLayout(),
                                    inPushConstants);
        Cmd.GetCommandBufferHandle().dispatch(inCountX, inCountY, inCountZ);
    }

    template<class PushType>
    void Painter::BindDispatchParameters_EXT()
    {
        BindDispatchParameters_EXT<PushType>(mWindow);
    }

    template<class PushType>
    void Painter::BindDispatchParameters_EXT(const Window &inWindow)
    {
        auto &Cmd = inWindow.GetCommandBuffers()[inWindow.GetCurrentFrame()];
        Cmd.GetCommandBufferHandle()
            .bindDescriptorSets(vk::PipelineBindPoint::eCompute,
                                mGUIPainterCache.GetComputePipelineLayout().GetPipelineLayoutHandle(),
                                0,
                                mComputeDescriptorSet.GetDescriptorSetHandle(),
                                {});
        mVulkanComputePipeline.Bind<PipelineContext::Compute>(Cmd);
    }

    template<class PushType>
    void Painter::Draw_EXT(const Primitive &inPrimitive,
                           const vk::ArrayProxy<PushType> inPushConstants,
                           uint32_t inIndexCount,
                           uint32_t inInstanceCount,
                           uint32_t inFirstIndex,
                           uint32_t inFirstInstance)
    {
        Draw_EXT(inPrimitive,
                 inPushConstants,
                 mWindow,
                 inIndexCount,
                 inInstanceCount,
                 inFirstIndex,
                 inFirstInstance);
    }

    template<class PushType>
    void Painter::Draw_EXT(const Primitive &inPrimitive,
                           const vk::ArrayProxy<PushType> inPushConstants,
                           const Window &inWindow,
                           uint32_t inIndexCount,
                           uint32_t inInstanceCount,
                           uint32_t inFirstIndex,
                           uint32_t inFirstInstance)
    {
        auto &Cmd = inWindow.GetCommandBuffers()[inWindow.GetCurrentFrame()];
        Cmd.PushConstants<PushType>(mGUIPainterCache.GetVertexFragmentPipelineLayout(),
                                    inPushConstants);
        mVulkanPipeline
            .DrawIndexed(Cmd, inIndexCount, inInstanceCount, inFirstIndex, 0, inFirstInstance);
    }

    template<class PushType>
    void Painter::Dispatch(const vk::ArrayProxy<PushType> inPushConstants,
                           uint32_t inCountX,
                           uint32_t inCountY,
                           uint32_t inCountZ)
    {
        auto &Cmd = mWindow.GetCommandBuffers()[mWindow.GetCurrentFrame()];
        Cmd.GetCommandBufferHandle()
            .bindDescriptorSets(vk::PipelineBindPoint::eCompute,
                                mGUIPainterCache.GetComputePipelineLayout().GetPipelineLayoutHandle(),
                                0,
                                mComputeDescriptorSet.GetDescriptorSetHandle(),
                                {});
        mVulkanComputePipeline.Bind<PipelineContext::Compute>(Cmd);
        Cmd.PushConstants<PushType>(mGUIPainterCache.GetVertexFragmentPipelineLayout(),
                                    inPushConstants);
        Cmd.GetCommandBufferHandle().dispatch(inCountX, inCountY, inCountZ);
    }

    template<class PushType>
    void Painter::BindDrawParamtersDescriptorsOnly_EXT(const Primitive &inPrimitive,
                                                       const Window &inWindow)
    {
        auto &Cmd = mWindow.GetCommandBuffers()[inWindow.GetCurrentFrame()];
        Cmd.GetCommandBufferHandle().bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics,
            mGUIPainterCache.GetVertexFragmentPipelineLayout().GetPipelineLayoutHandle(),
            0,
            mVertexFragmentDescriptorSet.GetDescriptorSetHandle(),
            {});
    }

    template<class PushType>
    void Painter::BindDrawParamters_EXT(const Primitive &inPrimitive)
    {
        BindDrawParamters_EXT<PushType>(inPrimitive, mWindow);
    }

    template<class PushType>
    void Painter::BindDrawParamtersPipelineOnly_EXT(const Primitive &inPrimitive,
                                                    const Window &inWindow)
    {
        auto index = inWindow.GetCurrentFrame();
        auto &Cmd = inWindow.GetCommandBuffers()[index];
        mVulkanPipeline.Bind<PipelineContext::Default>(Cmd);
    }

    template<class PushType>
    void Painter::BindDrawParamtersVertexAndIndexBuffersOnly_EXT(const Instance &inInstance,
                                                                 const Primitive &inPrimitive,
                                                                 const Window &inWindow)
    {
        auto &Cmd = inWindow.GetCommandBuffers()[inWindow.GetCurrentFrame()];
        inPrimitive.GetVertexBufferPtr()->Bind<BufferContext::Vertex>(Cmd);
        inPrimitive.GetIndexBufferPtr()->Bind<BufferContext::Index>(Cmd);
    }

    template<class PushType>
    void Painter::BindDrawParamters_EXT(const Primitive &inPrimitive, const Window &inWindow)
    {
        auto &Cmd = mWindow.GetCommandBuffers()[inWindow.GetCurrentFrame()];
        inPrimitive.GetVertexBufferPtr()->Bind<BufferContext::Vertex>(Cmd);
        inPrimitive.GetIndexBufferPtr()->Bind<BufferContext::Index>(Cmd);
        Cmd.GetCommandBufferHandle().bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics,
            mGUIPainterCache.GetVertexFragmentPipelineLayout().GetPipelineLayoutHandle(),
            0,
            mVertexFragmentDescriptorSet.GetDescriptorSetHandle(),
            {});
        mVulkanPipeline.Bind<PipelineContext::Default>(Cmd);
    }

    template<class PushType>
    void Painter::Draw(const Primitive &inPrimitive, const vk::ArrayProxy<PushType> inPushConstants)
    {
        Draw(inPrimitive, inPushConstants, mWindow);
    }

    template<class PushType>
    void Painter::Draw(const Primitive &inPrimitive,
                       const vk::ArrayProxy<PushType> inPushConstants,
                       const Window &inWindow)
    {
        auto &Cmd = mWindow.GetCommandBuffers()[inWindow.GetCurrentFrame()];
        inPrimitive.GetVertexBufferPtr()->Bind<BufferContext::Vertex>(Cmd);
        inPrimitive.GetIndexBufferPtr()->Bind<BufferContext::Index>(Cmd);
        Cmd.GetCommandBufferHandle().bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics,
            mGUIPainterCache.GetVertexFragmentPipelineLayout().GetPipelineLayoutHandle(),
            0,
            mVertexFragmentDescriptorSet.GetDescriptorSetHandle(),
            {});
        Cmd.PushConstants<PushType>(mGUIPainterCache.GetVertexFragmentPipelineLayout(),
                                    inPushConstants);
        mVulkanPipeline.Bind<PipelineContext::Default>(Cmd);
        mVulkanPipeline.DrawIndexed(Cmd, inPrimitive.GetIndexCount(), 1, 0, 0, 0);
    }

    } // namespace Jkr
