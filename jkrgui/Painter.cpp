#include "Painter.hpp"

using namespace Jkr;

Jkr::Painter::Painter(const Instance& inInstance, const Window& inWindow, const PainterCache& inCache)
    : mInstance(inInstance)
    , mWindow(inWindow)
    , mGUIPainterCache(inCache)
    , mVulkanPipeline(
          inInstance.GetDevice(),
          inCache.GetPipelineCache(),
          inCache.GetPipelineContext(),
          inWindow.GetRenderPass(),
          inCache.GetVertexFragmentPipelineLayout(),
          inCache.GetVertexFragmentShaderModulesArray())
    , mVulkanComputePipeline(
          inInstance.GetDevice(),
          inCache.GetPipelineCache(),
          inCache.GetPipelineContext(),
          inWindow.GetRenderPass(),
          inCache.GetComputePipelineLayout(),
          inCache.GetComputePipelineModule())
    , mComputeDescriptorSet(inInstance.GetDevice(), inInstance.GetDescriptorPool(), inCache.GetComputePipelineDescriptorSetLayout())
    , mVertexFragmentDescriptorSet(inInstance.GetDevice(), inInstance.GetDescriptorPool(), inCache.GetVertexFragmentDescriptorSetLayout())
    , mDescriptorUpdateHandler(mInstance.GetDevice())
{
}

Jkr::Painter::Painter(const Instance& inInstance, const Window& inWindow, const PainterCache& inCache, uint32_t inNoOfVariableDescriptorCount)
    : mInstance(inInstance)
    , mWindow(inWindow)
    , mGUIPainterCache(inCache)
    , mVulkanPipeline(
          inInstance.GetDevice(),
          inCache.GetPipelineCache(),
          inCache.GetPipelineContext(),
          inWindow.GetRenderPass(),
          inCache.GetVertexFragmentPipelineLayout(),
          inCache.GetVertexFragmentShaderModulesArray())
    , mVulkanComputePipeline(
          inInstance.GetDevice(),
          inCache.GetPipelineCache(),
          inCache.GetPipelineContext(),
          inWindow.GetRenderPass(),
          inCache.GetComputePipelineLayout(),
          inCache.GetComputePipelineModule())
    , mComputeDescriptorSet(inInstance.GetDevice(), inInstance.GetDescriptorPool(), inCache.GetComputePipelineDescriptorSetLayout(), inNoOfVariableDescriptorCount)
    , mVertexFragmentDescriptorSet(inInstance.GetDevice(), inInstance.GetDescriptorPool(), inCache.GetVertexFragmentDescriptorSetLayout(), inNoOfVariableDescriptorCount)
    , mDescriptorUpdateHandler(mInstance.GetDevice())
{
}

void Painter::OptimizeParameter( const Instance& inInstance,
    const PainterParameter<PainterParameterContext::StorageImage>& inImage, const Window& inWindow)
{
    auto& Cmd = inInstance.GetUtilCommandBuffer().GetCommandBufferHandle();
    Cmd.begin(vk::CommandBufferBeginInfo());
    OptimizeImageParameter(inInstance, inInstance.GetUtilCommandBuffer(), inImage, inWindow);
    Cmd.end();
    inInstance.GetGraphicsQueue().Submit<SubmitContext::SingleTime>(inInstance.GetUtilCommandBuffer());
}

void Jkr::Painter::OptimizeImageParameter(const Instance& inInstance, const VulkanCommandBuffer& inBuffer, const PainterParameter<PainterParameterContext::StorageImage>& inImage, const Window& inWindow)
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

template <>
void Painter::RegisterPainterParameter<PainterParameterContext::StorageBuffer>(const PainterParameter<PainterParameterContext::StorageBuffer>& inPainterParameter,
    vk::DeviceSize inOffset, uint32_t inDstBinding, uint32_t inDstArrayElement, RegisterMode inRegisterMode)
{
    if (inRegisterMode != RegisterMode::ComputeOnly)
        mDescriptorUpdateHandler.Write<BufferContext::Storage>(mVertexFragmentDescriptorSet, inPainterParameter.GetStorageBuffer(), inOffset, inDstBinding, inDstArrayElement);

    if (inRegisterMode == RegisterMode::AllShaders or inRegisterMode == RegisterMode::ComputeOnly)
        mDescriptorUpdateHandler.Write<BufferContext::Storage>(mComputeDescriptorSet, inPainterParameter.GetStorageBuffer(), inOffset, inDstBinding, inDstArrayElement);
}

template <>
void Painter::RegisterPainterParameter<PainterParameterContext::UniformBuffer>(const PainterParameter<PainterParameterContext::UniformBuffer>& inPainterParameter,
    vk::DeviceSize inOffset, uint32_t inDstBinding, uint32_t inDstArrayElement, RegisterMode inRegisterMode)
{
    if (inRegisterMode != RegisterMode::ComputeOnly)
        mDescriptorUpdateHandler.Write<BufferContext::Uniform>(mVertexFragmentDescriptorSet, inPainterParameter.GetUniformBuffer(), inOffset, inDstBinding, inDstArrayElement);
    if (inRegisterMode == RegisterMode::AllShaders or inRegisterMode == RegisterMode::ComputeOnly)
        mDescriptorUpdateHandler.Write<BufferContext::Uniform>(mComputeDescriptorSet, inPainterParameter.GetUniformBuffer(), inOffset, inDstBinding, inDstArrayElement);
}

template <>
void Painter::RegisterPainterParameter<PainterParameterContext::StorageImage>(const PainterParameter<PainterParameterContext::StorageImage>& inPainterParameter,
    vk::DeviceSize inOffset, uint32_t inDstBinding, uint32_t inDstArrayElement, RegisterMode inRegisterMode)
{
    if (inRegisterMode != RegisterMode::ComputeOnly)
        mDescriptorUpdateHandler.Write<ImageContext::Storage>(
            mVertexFragmentDescriptorSet,
            inPainterParameter.GetStorageImage(),
            inPainterParameter.GetStorageImageSampler(),
            inDstBinding,
            inDstArrayElement);
    if (inRegisterMode == RegisterMode::AllShaders or inRegisterMode == RegisterMode::ComputeOnly)
        mDescriptorUpdateHandler.Write<ImageContext::Storage>(
            mComputeDescriptorSet,
            inPainterParameter.GetStorageImage(),
            inPainterParameter.GetStorageImageSampler(),
            inDstBinding,
            inDstArrayElement);
}

template <>
void Painter::RegisterPainterParameter<PainterParameterContext::UniformImage>(const PainterParameter<PainterParameterContext::UniformImage>& inPainterParameter,
    vk::DeviceSize inOffset, uint32_t inDstBinding, uint32_t inDstArrayElement, RegisterMode inRegisterMode)
{
    if (inRegisterMode != RegisterMode::ComputeOnly)
        mDescriptorUpdateHandler.Write<ImageContext::Default>(
            mVertexFragmentDescriptorSet,
            inPainterParameter.GetUniformImage(),
            inPainterParameter.GetUniformImageSampler(),
            inDstBinding,
            inDstArrayElement);
    if (inRegisterMode == RegisterMode::AllShaders or inRegisterMode == RegisterMode::ComputeOnly)
        mDescriptorUpdateHandler.Write<ImageContext::Default>(
            mComputeDescriptorSet,
            inPainterParameter.GetUniformImage(),
            inPainterParameter.GetUniformImageSampler(),
            inDstBinding,
            inDstArrayElement);
}

template <>
void Painter::RegisterPainterParameter<PainterParameterContext::SkyboxImage>(const PainterParameter<PainterParameterContext::SkyboxImage>& inPainterParameter,
    vk::DeviceSize inOffset, uint32_t inDstBinding, uint32_t inDstArrayElement, RegisterMode inRegisterMode)
{
    if (inRegisterMode != RegisterMode::ComputeOnly)
        mDescriptorUpdateHandler.Write<ImageContext::Default>(
            mVertexFragmentDescriptorSet,
            inPainterParameter.GetUniformImage(),
            inPainterParameter.GetUniformImageSampler(),
            inDstBinding,
            inDstArrayElement);
    if (inRegisterMode == RegisterMode::AllShaders or inRegisterMode == RegisterMode::ComputeOnly)
        mDescriptorUpdateHandler.Write<ImageContext::Default>(
            mComputeDescriptorSet,
            inPainterParameter.GetUniformImage(),
            inPainterParameter.GetUniformImageSampler(),
            inDstBinding,
            inDstArrayElement);
}

template <>
void Painter::RegisterPainterParameter<PainterParameterContext::UniformSampler>(
    const PainterParameter<PainterParameterContext::UniformSampler>& inPainterParameter,
    vk::DeviceSize inOffset, uint32_t inDstBinding, uint32_t inDstArrayElement, RegisterMode inRegisterMode)
{
    if (inRegisterMode != RegisterMode::ComputeOnly)
        mDescriptorUpdateHandler.Write(
            mVertexFragmentDescriptorSet,
            inPainterParameter.GetSampler(),
            inDstBinding,
            inDstArrayElement);
    if (inRegisterMode == RegisterMode::AllShaders or inRegisterMode == RegisterMode::ComputeOnly)
        mDescriptorUpdateHandler.Write(
            mComputeDescriptorSet,
            inPainterParameter.GetSampler(),
            inDstBinding,
            inDstArrayElement);
}


void Painter::BindDispatchParameters_EXT(const Window& inWindow, CmdParam inParam)
{
    auto& Cmd = inWindow.GetCommandBuffers(inParam)[inWindow.GetCurrentFrame()];
    Cmd.GetCommandBufferHandle()
        .bindDescriptorSets(vk::PipelineBindPoint::eCompute,
            mGUIPainterCache.GetComputePipelineLayout().GetPipelineLayoutHandle(),
            0,
            mComputeDescriptorSet.GetDescriptorSetHandle(),
            {});
    mVulkanComputePipeline.Bind<PipelineContext::Compute>(Cmd);
}

void Jkr::Painter::BindDispatchParametersDescriptorsOnly_EXT(const Window& inWindow, CmdParam inParam)
{
    auto& Cmd = inWindow.GetCommandBuffers(inParam)[inWindow.GetCurrentFrame()];
    Cmd.GetCommandBufferHandle()
        .bindDescriptorSets(vk::PipelineBindPoint::eCompute,
            mGUIPainterCache.GetComputePipelineLayout().GetPipelineLayoutHandle(),
            0,
            mComputeDescriptorSet.GetDescriptorSetHandle(),
            {});
}

void Jkr::Painter::BindDispatchParametersPipelineOnly_EXT(const Window& inWindow, CmdParam inParam)
{
    auto index = inWindow.GetCurrentFrame();
    auto &Cmd = inWindow.GetCommandBuffers(inParam)[index];
    mVulkanComputePipeline.Bind<PipelineContext::Compute>(Cmd);
}


void Painter::BindDrawParamters_EXT(const Primitive& inPrimitive, const Window& inWindow, CmdParam inParam)
{
    auto& Cmd = mWindow.GetCommandBuffers(inParam)[inWindow.GetCurrentFrame()];
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
