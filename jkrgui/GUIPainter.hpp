#pragma once
#include "GUIWindow.hpp"
#include "GUIPrimitive.hpp"
#include "GUIPainterCache.hpp"
#include "GUIPainterParameter.hpp"

namespace Jkr {
	class GUIPainterBase
	{
	public:
	private:
	};
}


namespace Jkr {
	class GUIPainter
	{
	public:
		GUIPainter(const Instance& inInstance, const GUIWindow& inWindow, const GUIPainterCache& inCache);
		GUIPainter(const Instance& inInstance, const GUIWindow& inWindow, const GUIPainterCache& inCache, uint32_t inNoOfVariableDescriptorCount);


		template<class PushType>
		constexpr void Draw(const GUIPrimitive& inPrimitive, const vk::ArrayProxy<PushType>  inPushConstants, const GUIWindow& inWindow)
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
		constexpr void Draw(const GUIPrimitive& inPrimitive, const vk::ArrayProxy<PushType>  inPushConstants)
		{
			Draw(inPrimitive, inPushConstants, mWindow);
		}

		template<class PushType>
		constexpr void BindDrawParamters_EXT(const GUIPrimitive& inPrimitive, const GUIWindow& inWindow)
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
		constexpr void BindDrawParamters_EXT(const GUIPrimitive& inPrimitive)
		{
			BindDrawParamters_EXT<PushType>(inPrimitive, mWindow);
		}

		template<class PushType>
		constexpr void Draw_EXT(const GUIPrimitive& inPrimitive, const vk::ArrayProxy<PushType> inPushConstants, const GUIWindow& inWindow, uint32_t inIndexCount, uint32_t inInstanceCount, uint32_t inFirstIndex, uint32_t inFirstInstance)
		{
			auto& Cmd = mInstance.GetCommandBuffers()[inWindow.GetCurrentFrame()];
			Cmd.PushConstants<PushType>(mGUIPainterCache.GetVertexFragmentPipelineLayout(), inPushConstants);
			mVulkanPipeline.DrawIndexed(Cmd, inIndexCount, inInstanceCount, inFirstIndex, 0, inFirstInstance);
		}
		template<class PushType>
		constexpr void Draw_EXT(const GUIPrimitive& inPrimitive, const vk::ArrayProxy<PushType> inPushConstants, uint32_t inIndexCount, uint32_t inInstanceCount, uint32_t inFirstIndex, uint32_t inFirstInstance)
		{
			Draw_EXT(inPrimitive, inPushConstants, mWindow, inIndexCount, inInstanceCount, inFirstIndex, inFirstInstance);
		}















		template <class PushType>
		constexpr void Dispatch_GUIRenderer(const GUIPrimitive& inPrimitive, const vk::ArrayProxy<PushType> inPushConstants, const GUIWindow& inWindow, uint32_t inCountX, uint32_t inCountY, uint32_t inCountZ)
		{
			auto& Cmd = mInstance.GetCommandBuffers()[mWindow.GetCurrentFrame()];
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
		template <class PushType>
		constexpr void Dispatch_GUIRenderer(const GUIPrimitive& inPrimitive, const vk::ArrayProxy<PushType> inPushConstants, uint32_t inCountX, uint32_t inCountY, uint32_t inCountZ)
		{
			Dispatch_GUIRenderer(inPrimitive, inPushConstants, mWindow, inCountX, inCountY, inCountZ);
		}


		template <class PushType>
		constexpr void BindDispatchParameters_EXT(const GUIPrimitive& inPrimitive, const GUIWindow& inWindow)
		{
			auto& Cmd = mInstance.GetCommandBuffers()[inWindow.GetCurrentFrame()];
			Cmd.GetCommandBufferHandle().bindDescriptorSets(
				vk::PipelineBindPoint::eCompute,
				mGUIPainterCache.GetComputePipelineLayout().GetPipelineLayoutHandle(),
				0,
				mComputeDescriptorSet.GetDescriptorSetHandle(),
				{});
			mVulkanComputePipeline.Bind<PipelineContext::Compute>(Cmd);
		}

		template <class PushType>
		constexpr void BindDispatchParameters_EXT(const GUIPrimitive& inPrimitive)
		{
			BindDispatchParameters_EXT<PushType>(inPrimitive, mWindow);
		}


		template <class PushType>
		constexpr void Dispatch_EXT(const GUIPrimitive& inPrimitive, const vk::ArrayProxy<PushType> inPushConstants, uint32_t inCountX, uint32_t inCountY, uint32_t inCountZ)
		{
			auto& Cmd = mInstance.GetCommandBuffers()[mWindow.GetCurrentFrame()];
			Cmd.PushConstants<PushType>(mGUIPainterCache.GetVertexFragmentPipelineLayout(), inPushConstants);
			Cmd.GetCommandBufferHandle().dispatch(inCountX, inCountY, inCountZ);
		}











		template<GUIPainterParameterContext inContext>
		void RegisterPainterParameter(
			const GUIPainterParameter<inContext>& inPainterParameter,
			vk::DeviceSize inOffset,
			uint32_t inDstBinding,
			uint32_t inDstArrayElement
		);
		void  OptimizeParameter(const GUIPainterParameter<GUIPainterParameterContext::StorageImage>& inImage, const GUIWindow& inWindow)
		{
			auto& Cmd = mInstance.GetUtilCommandBuffer().GetCommandBufferHandle();
			Cmd.begin(vk::CommandBufferBeginInfo());
			OptimizeImageParameter(mInstance.GetUtilCommandBuffer(), inImage, inWindow);
			Cmd.end();
			mInstance.GetGraphicsQueue().Submit<SubmitContext::SingleTime>(mInstance.GetUtilCommandBuffer());
		}





	private:
		void OptimizeImageParameter(const VulkanCommandBuffer& inBuffer, const GUIPainterParameter<GUIPainterParameterContext::StorageImage>& inImage, const GUIWindow& inWindow);
		const Instance& mInstance;
		const GUIWindow& mWindow;
		const GUIPainterCache& mGUIPainterCache;
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




//void RegisterPainterParameter(const GUIPrimitive& inPrimitive, vk::DeviceSize inOffset, uint32_t inDstBinding, uint32_t inDstArrayElement)
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
