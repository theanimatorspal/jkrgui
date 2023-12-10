#include "Painter.hpp"

using namespace Jkr;

Jkr::Painter::Painter(const Instance& inInstance, const Window& inWindow, const PainterCache& inCache) :
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

Jkr::Painter::Painter(const Instance& inInstance, const Window& inWindow, const PainterCache& inCache, uint32_t inNoOfVariableDescriptorCount)
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
{}

void Painter::OptimizeParameter(
    const PainterParameter<PainterParameterContext::StorageImage> &inImage, const Window &inWindow)
{
    auto &Cmd = mInstance.GetUtilCommandBuffer().GetCommandBufferHandle();
    Cmd.begin(vk::CommandBufferBeginInfo());
    OptimizeImageParameter(mInstance.GetUtilCommandBuffer(), inImage, inWindow);
    Cmd.end();
    mInstance.GetGraphicsQueue().Submit<SubmitContext::SingleTime>(mInstance.GetUtilCommandBuffer());
}

void Jkr::Painter::OptimizeImageParameter(const VulkanCommandBuffer& inBuffer, const PainterParameter<PainterParameterContext::StorageImage>& inImage, const Window& inWindow)
{
    inImage.GetStorageImage().CmdTransitionImageLayout(inBuffer,
                                                       vk::ImageLayout::eUndefined,
                                                       vk::ImageLayout::eGeneral,
                                                       vk::PipelineStageFlagBits::eVertexShader,
                                                       vk::PipelineStageFlagBits::eFragmentShader,
                                                       vk::AccessFlagBits::eNone,
                                                       vk::AccessFlagBits::eNone);
}

using namespace Jkr;

template<>
void Painter::RegisterPainterParameter<PainterParameterContext::StorageBuffer>
(const PainterParameter<PainterParameterContext::StorageBuffer>& inPainterParameter,
	vk::DeviceSize inOffset, uint32_t inDstBinding, uint32_t inDstArrayElement)
{
	mDescriptorUpdateHandler.Write<BufferContext::Storage>(mVertexFragmentDescriptorSet, inPainterParameter.GetStorageBuffer(), inOffset, inDstBinding, inDstArrayElement);
	mDescriptorUpdateHandler.Write<BufferContext::Storage>(mComputeDescriptorSet, inPainterParameter.GetStorageBuffer(), inOffset, inDstBinding, inDstArrayElement);
}

template<>
void Painter::RegisterPainterParameter<PainterParameterContext::UniformBuffer>
(const PainterParameter<PainterParameterContext::UniformBuffer>& inPainterParameter,
	vk::DeviceSize inOffset, uint32_t inDstBinding, uint32_t inDstArrayElement)
{
	mDescriptorUpdateHandler.Write<BufferContext::Uniform>(mVertexFragmentDescriptorSet, inPainterParameter.GetUniformBuffer(), inOffset, inDstBinding, inDstArrayElement);
	mDescriptorUpdateHandler.Write<BufferContext::Uniform>(mComputeDescriptorSet, inPainterParameter.GetUniformBuffer(), inOffset, inDstBinding, inDstArrayElement);
}

template<>
void Painter::RegisterPainterParameter<PainterParameterContext::StorageImage>
(const PainterParameter<PainterParameterContext::StorageImage>& inPainterParameter,
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
void Painter::RegisterPainterParameter<PainterParameterContext::UniformImage>
(const PainterParameter<PainterParameterContext::UniformImage>& inPainterParameter,
	vk::DeviceSize inOffset, uint32_t inDstBinding, uint32_t inDstArrayElement)
{
	mDescriptorUpdateHandler.Write<ImageContext::Default>(
		mVertexFragmentDescriptorSet,
		inPainterParameter.GetUniformImage(),
		inPainterParameter.GetUniformImageSampler(),
		inDstBinding,
		inDstArrayElement
	);
	mDescriptorUpdateHandler.Write<ImageContext::Default>(
		mComputeDescriptorSet,
		inPainterParameter.GetUniformImage(),
		inPainterParameter.GetUniformImageSampler(),
		inDstBinding,
		inDstArrayElement
	);
}


template<>
void Painter::RegisterPainterParameter<PainterParameterContext::UniformSampler> (
	const PainterParameter<PainterParameterContext::UniformSampler>& inPainterParameter,
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


