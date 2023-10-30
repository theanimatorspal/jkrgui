#include "GUIPainter.hpp"

Jkr::GUIPainter::GUIPainter(const Instance& inInstance, const GUIWindow& inWindow, const GUIPainterCache& inCache) :
    mInstance(inInstance),
    mWindow(inWindow),
    mGUIPainterCache(inCache),
    mVulkanPipeline(
        inInstance.GetDevice(),
        inCache.GetPipelineCache(),
        inCache.GetPipelineContext(),
        inWindow.GetRenderPass(),
        inCache.GetVertexFragmentPipelineLayout(),
        inCache.GetVertexFragmentShaderModulesArray()
    ),
    mVulkanComputePipeline(
        inInstance.GetDevice(),
        inCache.GetPipelineCache(),
        inCache.GetPipelineContext(),
        inWindow.GetRenderPass(),
        inCache.GetComputePipelineLayout(),
        inCache.GetComputePipelineModule()
    ),
    mComputeDescriptorSet(inInstance.GetDevice(), inInstance.GetDescriptorPool(), inCache.GetComputePipelineDescriptorSetLayout()),
    mVertexFragmentDescriptorSet(inInstance.GetDevice(), inInstance.GetDescriptorPool(), inCache.GetVertexFragmentDescriptorSetLayout()),
    mDescriptorUpdateHandler(mInstance.GetDevice())
{
}

Jkr::GUIPainter::GUIPainter(const Instance& inInstance, const GUIWindow& inWindow, const GUIPainterCache& inCache, uint32_t inNoOfVariableDescriptorCount)
	: mInstance(inInstance),
	mWindow(inWindow),
	mGUIPainterCache(inCache),
	mVulkanPipeline(
		inInstance.GetDevice(),
		inCache.GetPipelineCache(),
		inCache.GetPipelineContext(),
		inWindow.GetRenderPass(),
		inCache.GetVertexFragmentPipelineLayout(),
		inCache.GetVertexFragmentShaderModulesArray()
	),
	mVulkanComputePipeline(
		inInstance.GetDevice(),
		inCache.GetPipelineCache(),
		inCache.GetPipelineContext(),
		inWindow.GetRenderPass(),
		inCache.GetComputePipelineLayout(),
		inCache.GetComputePipelineModule()
	),
	mComputeDescriptorSet(inInstance.GetDevice(), inInstance.GetDescriptorPool(), inCache.GetComputePipelineDescriptorSetLayout(), inNoOfVariableDescriptorCount),
	mVertexFragmentDescriptorSet(inInstance.GetDevice(), inInstance.GetDescriptorPool(), inCache.GetVertexFragmentDescriptorSetLayout(), inNoOfVariableDescriptorCount),
	mDescriptorUpdateHandler(mInstance.GetDevice())
{
}

void Jkr::GUIPainter::OptimizeImageParameter(const VulkanCommandBuffer& inBuffer, const GUIPainterParameter<GUIPainterParameterContext::StorageImage>& inImage, const GUIWindow& inWindow)
{
	inImage.GetStorageImage().CmdTransitionImageLayout(
		inBuffer,
		vk::ImageLayout::eUndefined,
		vk::ImageLayout::eGeneral,
		vk::PipelineStageFlagBits::eVertexShader,
		vk::PipelineStageFlagBits::eFragmentShader,
		vk::AccessFlagBits::eNone,
		vk::AccessFlagBits::eNone
	);
}

using namespace Jkr;

template<>
void GUIPainter::RegisterPainterParameter<GUIPainterParameterContext::StorageBuffer>
(const GUIPainterParameter<GUIPainterParameterContext::StorageBuffer>& inPainterParameter,
	vk::DeviceSize inOffset, uint32_t inDstBinding, uint32_t inDstArrayElement)
{
	mDescriptorUpdateHandler.Write<BufferContext::Storage>(mVertexFragmentDescriptorSet, inPainterParameter.GetStorageBuffer(), inOffset, inDstBinding, inDstArrayElement);
	mDescriptorUpdateHandler.Write<BufferContext::Storage>(mComputeDescriptorSet, inPainterParameter.GetStorageBuffer(), inOffset, inDstBinding, inDstArrayElement);
}

template<>
void GUIPainter::RegisterPainterParameter<GUIPainterParameterContext::UniformBuffer>
(const GUIPainterParameter<GUIPainterParameterContext::UniformBuffer>& inPainterParameter,
	vk::DeviceSize inOffset, uint32_t inDstBinding, uint32_t inDstArrayElement)
{
	mDescriptorUpdateHandler.Write<BufferContext::Uniform>(mVertexFragmentDescriptorSet, inPainterParameter.GetUniformBuffer(), inOffset, inDstBinding, inDstArrayElement);
	mDescriptorUpdateHandler.Write<BufferContext::Uniform>(mComputeDescriptorSet, inPainterParameter.GetUniformBuffer(), inOffset, inDstBinding, inDstArrayElement);
}

template<>
void GUIPainter::RegisterPainterParameter<GUIPainterParameterContext::StorageImage>
(const GUIPainterParameter<GUIPainterParameterContext::StorageImage>& inPainterParameter,
	vk::DeviceSize inOffset, uint32_t inDstBinding, uint32_t inDstArrayElement)
{
	mDescriptorUpdateHandler.Write<ImageContext::Storage>(
		mVertexFragmentDescriptorSet,
		inPainterParameter.GetStorageImage(),
		inPainterParameter.GetStorageImageSampler(),
		inDstBinding,
		inDstArrayElement
	);
	mDescriptorUpdateHandler.Write<ImageContext::Storage>(
		mComputeDescriptorSet,
		inPainterParameter.GetStorageImage(),
		inPainterParameter.GetStorageImageSampler(),
		inDstBinding,
		inDstArrayElement
	);
}


template<>
void GUIPainter::RegisterPainterParameter<GUIPainterParameterContext::UniformImage>
(const GUIPainterParameter<GUIPainterParameterContext::UniformImage>& inPainterParameter,
	vk::DeviceSize inOffset, uint32_t inDstBinding, uint32_t inDstArrayElement)
{
	mDescriptorUpdateHandler.Write<ImageContext::Default>(
		mVertexFragmentDescriptorSet,
		inPainterParameter.GetStorageImage(),
		inPainterParameter.GetStorageImageSampler(),
		inDstBinding,
		inDstArrayElement
	);
	mDescriptorUpdateHandler.Write<ImageContext::Default>(
		mComputeDescriptorSet,
		inPainterParameter.GetStorageImage(),
		inPainterParameter.GetStorageImageSampler(),
		inDstBinding,
		inDstArrayElement
	);
}


template<>
void GUIPainter::RegisterPainterParameter<GUIPainterParameterContext::UniformSampler> (
	const GUIPainterParameter<GUIPainterParameterContext::UniformSampler>& inPainterParameter,
	vk::DeviceSize inOffset, uint32_t inDstBinding, uint32_t inDstArrayElement)
{
	mDescriptorUpdateHandler.Write(
		mVertexFragmentDescriptorSet,
		inPainterParameter.GetSampler(),
		inDstBinding,
		inDstArrayElement
	);
	mDescriptorUpdateHandler.Write(
		mComputeDescriptorSet,
		inPainterParameter.GetSampler(),
		inDstBinding,
		inDstArrayElement
	);
}


