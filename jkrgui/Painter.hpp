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
}


namespace Jkr {
	class Painter
	{
	public:
		Painter(const Instance& inInstance, const Window& inWindow, const PainterCache& inCache);
		Painter(const Instance& inInstance, const Window& inWindow, const PainterCache& inCache, uint32_t inNoOfVariableDescriptorCount);


		template<class PushType>
		inline void Draw(const Primitive& inPrimitive, const vk::ArrayProxy<PushType>  inPushConstants, const Window& inWindow)
		{
			auto& Cmd = mInstance.GetCommandBuffers()[inWindow.GetCurrentFrame()];
			inPrimitive.GetVertexBufferPtr()->Bind<BufferContext::Vertex>(Cmd);
			inPrimitive.GetIndexBufferPtr()->Bind<BufferContext::Index>(Cmd);
			Cmd.GetCommandBufferHandle().bindDescriptorSets(
				vk::PipelineBindPoint::eGraphics,
				mGUIPainterCache.GetVertexFragmentPipelineLayout().GetPipelineLayoutHandle(),
				0,
				mVertexFragmentDescriptorSet.GetDescriptorSetHandle(),
				{});
			Cmd.PushConstants<PushType>(mGUIPainterCache.GetVertexFragmentPipelineLayout(), inPushConstants);
			mVulkanPipeline.Bind<PipelineContext::Default>(Cmd);
			mVulkanPipeline.DrawIndexed(Cmd, inPrimitive.GetIndexCount(), 1, 0, 0, 0);
		}
		template<class PushType>
		inline void Draw(const Primitive& inPrimitive, const vk::ArrayProxy<PushType>  inPushConstants)
		{
			Draw(inPrimitive, inPushConstants, mWindow);
		}

		template<class PushType>
		inline void BindDrawParamters_EXT(const Primitive& inPrimitive, const Window& inWindow)
		{
			auto& Cmd = mInstance.GetCommandBuffers()[inWindow.GetCurrentFrame()];
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
		inline void BindDrawParamtersDescriptorsOnly_EXT(const Primitive& inPrimitive, const Window& inWindow)
		{
			auto& Cmd = mInstance.GetCommandBuffers()[inWindow.GetCurrentFrame()];
			Cmd.GetCommandBufferHandle().bindDescriptorSets(
				vk::PipelineBindPoint::eGraphics,
				mGUIPainterCache.GetVertexFragmentPipelineLayout().GetPipelineLayoutHandle(),
				0,
				mVertexFragmentDescriptorSet.GetDescriptorSetHandle(),
				{});
		}

		template<class PushType>
		static inline void BindDrawParamtersVertexAndIndexBuffersOnly_EXT(const Instance& inInstance, const Primitive& inPrimitive, const Window& inWindow)
		{
			auto& Cmd = inInstance.GetCommandBuffers()[inWindow.GetCurrentFrame()];
			inPrimitive.GetVertexBufferPtr()->Bind<BufferContext::Vertex>(Cmd);
			inPrimitive.GetIndexBufferPtr()->Bind<BufferContext::Index>(Cmd);
		}

		template<class PushType>
		inline void BindDrawParamtersPipelineOnly_EXT(const Primitive& inPrimitive, const Window& inWindow)
		{
			auto index = inWindow.GetCurrentFrame();
			auto& Cmd = mInstance.GetCommandBuffers()[index];
			mVulkanPipeline.Bind<PipelineContext::Default>(Cmd);
		}


		template<class PushType>
		inline void BindDrawParamters_EXT(const Primitive& inPrimitive)
		{
			BindDrawParamters_EXT<PushType>(inPrimitive, mWindow);
		}

		template<class PushType>
		inline void Draw_EXT(const Primitive& inPrimitive, const vk::ArrayProxy<PushType> inPushConstants, const Window& inWindow, uint32_t inIndexCount, uint32_t inInstanceCount, uint32_t inFirstIndex, uint32_t inFirstInstance)
		{
			auto& Cmd = mInstance.GetCommandBuffers()[inWindow.GetCurrentFrame()];
			Cmd.PushConstants<PushType>(mGUIPainterCache.GetVertexFragmentPipelineLayout(), inPushConstants);
			mVulkanPipeline.DrawIndexed(Cmd, inIndexCount, inInstanceCount, inFirstIndex, 0, inFirstInstance);
		}
		template<class PushType>
		inline void Draw_EXT(const Primitive& inPrimitive, const vk::ArrayProxy<PushType> inPushConstants, uint32_t inIndexCount, uint32_t inInstanceCount, uint32_t inFirstIndex, uint32_t inFirstInstance)
		{
			Draw_EXT(inPrimitive, inPushConstants, mWindow, inIndexCount, inInstanceCount, inFirstIndex, inFirstInstance);
		}

        template<class PushType>
        inline void Dispatch(const vk::ArrayProxy<PushType> inPushConstants,
                             uint32_t inCountX,
                             uint32_t inCountY,
                             uint32_t inCountZ)
        {
            auto &Cmd = mInstance.GetCommandBuffers()[mWindow.GetCurrentFrame()];
            Cmd.GetCommandBufferHandle().bindDescriptorSets(
				vk::PipelineBindPoint::eCompute,
				mGUIPainterCache.GetComputePipelineLayout().GetPipelineLayoutHandle(),
				0,
				mComputeDescriptorSet.GetDescriptorSetHandle(),
				{});
			mVulkanComputePipeline.Bind<PipelineContext::Compute>(Cmd);
			Cmd.PushConstants<PushType>(mGUIPainterCache.GetVertexFragmentPipelineLayout(), inPushConstants);
			Cmd.GetCommandBufferHandle().dispatch(inCountX, inCountY, inCountZ);
        }

        template<class PushType>
        inline void BindDispatchParameters_EXT(const Window &inWindow)
        {
            auto &Cmd = mInstance.GetCommandBuffers()[inWindow.GetCurrentFrame()];
            Cmd.GetCommandBufferHandle().bindDescriptorSets(
				vk::PipelineBindPoint::eCompute,
				mGUIPainterCache.GetComputePipelineLayout().GetPipelineLayoutHandle(),
				0,
				mComputeDescriptorSet.GetDescriptorSetHandle(),
				{});
			mVulkanComputePipeline.Bind<PipelineContext::Compute>(Cmd);
        }

        template<class PushType>
        inline void BindDispatchParameters_EXT()
        {
            BindDispatchParameters_EXT<PushType>(mWindow);
        }

        template<class PushType>
        inline void Dispatch_EXT(const vk::ArrayProxy<PushType> inPushConstants,
                                 uint32_t inCountX,
                                 uint32_t inCountY,
                                 uint32_t inCountZ)
        {
            auto &Cmd = mInstance.GetCommandBuffers()[mWindow.GetCurrentFrame()];
            Cmd.PushConstants<PushType>(mGUIPainterCache.GetVertexFragmentPipelineLayout(), inPushConstants);
			Cmd.GetCommandBufferHandle().dispatch(inCountX, inCountY, inCountZ);
        }

        template<PainterParameterContext inContext>
        void RegisterPainterParameter(const PainterParameter<inContext> &inPainterParameter,
                                      vk::DeviceSize inOffset,
                                      uint32_t inDstBinding,
                                      uint32_t inDstArrayElement);

        void  OptimizeParameter(const PainterParameter<PainterParameterContext::StorageImage>& inImage, const Window& inWindow)
		{
			auto& Cmd = mInstance.GetUtilCommandBuffer().GetCommandBufferHandle();
			Cmd.begin(vk::CommandBufferBeginInfo());
			OptimizeImageParameter(mInstance.GetUtilCommandBuffer(), inImage, inWindow);
			Cmd.end();
			mInstance.GetGraphicsQueue().Submit<SubmitContext::SingleTime>(mInstance.GetUtilCommandBuffer());
		}





	private:
		void OptimizeImageParameter(const VulkanCommandBuffer& inBuffer, const PainterParameter<PainterParameterContext::StorageImage>& inImage, const Window& inWindow);
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
}

namespace Jkr
{


}




//void RegisterPainterParameter(const Primitive& inPrimitive, vk::DeviceSize inOffset, uint32_t inDstBinding, uint32_t inDstArrayElement)
//{
//	if (inPrimitive.GetStorageBufferPtr()) {
//		mDescriptorUpdateHandler.Write<BufferContext::Storage>(mComputeDescriptorSet, *inPrimitive.GetStorageBufferPtr(), inOffset, inDstBinding, inDstArrayElement);
//		mDescriptorUpdateHandler.Write<BufferContext::Storage>(mVertexFragmentDescriptorSet, *inPrimitive.GetStorageBufferPtr(), inOffset, inDstBinding, inDstArrayElement);
//	}
//	if (inPrimitive.GetUniformBufferPtr())
//	{
//		mDescriptorUpdateHandler.Write<BufferContext::Uniform>(mComputeDescriptorSet, *inPrimitive.GetUniformBufferPtr(), inOffset, inDstBinding, inDstArrayElement);
//		mDescriptorUpdateHandler.Write<BufferContext::Uniform>(mVertexFragmentDescriptorSet, *inPrimitive.GetUniformBufferPtr(), inOffset, inDstBinding, inDstArrayElement);
//	}
//	if (inPrimitive.GetStorageImagePtr())
//	{
//		mDescriptorUpdateHandler.Write<ImageContext::Storage>(mVertexFragmentDescriptorSet, *inPrimitive.GetStorageImagePtr(), *inPrimitive.GetSamplerPtr(), inDstBinding, inDstArrayElement);
//		mDescriptorUpdateHandler.Write<ImageContext::Storage>(mComputeDescriptorSet, *inPrimitive.GetStorageImagePtr(), *inPrimitive.GetSamplerPtr(), inDstBinding, inDstArrayElement);
//	}
//}
