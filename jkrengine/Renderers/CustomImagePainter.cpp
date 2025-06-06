#include "Renderers/TwoD/Shape.hpp"
using namespace Jkr::Renderer;
using namespace ksai;

const sv gbefore_xyz                  = R"""(
#version 450
layout(set = 0, binding = 0, rgba8) uniform image2D storageImage;
                                   
)""";

const sv gmain_function_null          = R"""(
void GlslMain()
{
}
)""";

const sv gmain_function_null_fragment = R"""(
void GlslMain()
{
    
}
)""";

const sv gafter_xyz                   = R"""(
void GlslMain()
{
	uvec3 gID = gl_GlobalInvocationID;
	ivec2 image_size = ivec2(imageSize(storageImage));
	ivec2 to_draw_at = ivec2(gl_GlobalInvocationID.x, gl_GlobalInvocationID.y);
	
	float x_cart = (float(gl_GlobalInvocationID.x) - float(image_size.x) / float(2)) / (float((image_size.x) / float(2)));
	float y_cart = (float(image_size.y) / float(2) - float(gl_GlobalInvocationID.y)) / (float(image_size.y) / float(2));
	
	float ColorValueX = x_cart;
	float ColorValueY = y_cart;
	vec2 xy = vec2(x_cart, y_cart);
)""";

const sv gend                         = R"""(
}
)""";

// TODO Remove  this
CustomImagePainter::CustomImagePainter(
     sv inName, sv inComputeShaderFunction, sv inPushConstantSignature, ui inX, ui inY, ui inZ)
    : mCustomPainterFileName(std::string(inName)), mThreads(inX, inY, inZ) {
    // TODO THESE Are to be removed
    mComputeStream << gbefore_xyz;
    mComputeStream << "layout(local_size_x = " << inX << ", local_size_y = " << inY << ","
                   << " local_size_z = " << inZ << ") in;";
    mComputeStream << inPushConstantSignature;
    mComputeStream << gafter_xyz;
    mComputeStream << inComputeShaderFunction;
    mComputeStream << gend;

    mVertexStream << gbefore_xyz;
    mVertexStream << inPushConstantSignature;
    mVertexStream << gmain_function_null;

    mFragmentStream << gbefore_xyz;
    mFragmentStream << inPushConstantSignature;
    mFragmentStream << gmain_function_null;
}

void CustomImagePainter::Store(Instance &inInstance, Window_base &inWindow, Jkr::Misc::File& inFile) {
    mCustomPainterCache = MakeUp<PainterCache>(inInstance);
    mCustomPainterCache->Store(inFile,
         mCustomPainterFileName, mVertexStream.str(), mFragmentStream.str(), mComputeStream.str());
    mPainter = MakeUp<Painter>(inInstance, inWindow, *mCustomPainterCache);
}

CustomImagePainter::CustomImagePainter(sv inName, sv inComputeShader)
    : mCustomPainterFileName(std::string(inName)) {
    mComputeStream << inComputeShader;

    mVertexStream << gbefore_xyz;
    mVertexStream << gmain_function_null;

    mFragmentStream << R"""(
#version 450
layout(location = 0) out vec4 outColor;
void GlslMain()
{
    outColor = vec4(0, 0, 0, 0);
}

               )""";
}

void Jkr::Renderer::CustomPainterImage::Register(Instance &inInstance,
                                                 CustomImagePainter &inPainter,
                                                 int inIndex) {
    if (not mVulkanDescriptorSet) {
        mVulkanDescriptorSet = MakeUp<VulkanDescriptorSet>(
             inInstance.GetDevice(),
             inInstance.GetDescriptorPool(),
             inPainter.GetPainterCache().GetComputePipelineDescriptorSetLayout());
    }
    mPainterParam->Register(0, inIndex, 0, *mVulkanDescriptorSet);
}

/// @todo Make this better, this is basically stopping parallelization
/// This is done to basically prevent the HAZARD READ_AFTER_WRITE
/// Since, there are images that is being written on and being loaded from
/// both.
void Jkr::Renderer::CustomPainterImage::SyncBefore(Window_base &inWindow, ComPar inPar) {

    auto &Cmd   = inWindow.GetCommandBuffers(inPar)[inWindow.GetCurrentFrame()];
    auto &Image = mPainterParam->GetStorageImage();
    Image.CmdTransitionImageLayout(
         Cmd,
         Image.GetCurrentImageLayout(),
         vk::ImageLayout::eGeneral,
         vk::PipelineStageFlagBits::eAllCommands,
         vk::PipelineStageFlagBits::eComputeShader,
         vk::AccessFlagBits::eShaderRead | vk::AccessFlagBits::eShaderWrite,
         vk::AccessFlagBits::eShaderRead | vk::AccessFlagBits::eShaderWrite);
}

void Jkr::Renderer::CustomPainterImage::SyncAfter(Window_base &inWindow, ComPar inPar) {

    auto &Cmd   = inWindow.GetCommandBuffers(inPar)[inWindow.GetCurrentFrame()];
    auto &Image = mPainterParam->GetStorageImage();
    Image.CmdTransitionImageLayout(
         Cmd,
         Image.GetCurrentImageLayout(),
         vk::ImageLayout::eGeneral,
         vk::PipelineStageFlagBits::eComputeShader,
         vk::PipelineStageFlagBits::eAllCommands,
         vk::AccessFlagBits::eShaderRead | vk::AccessFlagBits::eShaderWrite,
         vk::AccessFlagBits::eShaderRead | vk::AccessFlagBits::eShaderWrite);
}

CustomPainterImage::CustomPainterImage(Instance &inInstance,
                                       Window_base &inWindow,
                                       ui inWidth,
                                       ui inHeight) {
    mPainterParam = mu<Image>(inInstance);
    mPainterParam->Setup(inWidth, inHeight);
    Painter::OptimizeParameter(inInstance, *mPainterParam, inWindow);
}

void CustomImagePainter::BindImageFromImage(Window_base &inWindow,
                                            CustomPainterImage &inImage,
                                            ComPar inPar) {
    auto &Cmd   = inWindow.GetCommandBuffers(inPar)[inWindow.GetCurrentFrame()];
    auto &Image = inImage.GetPainterParam().GetStorageImage();

    Cmd.GetCommandBufferHandle().bindDescriptorSets(
         vk::PipelineBindPoint::eCompute,
         mCustomPainterCache->GetComputePipelineLayout().GetPipelineLayoutHandle(),
         0,
         inImage.mVulkanDescriptorSet->GetDescriptorSetHandle(),
         {});
}

v<char> CustomPainterImage::GetVector(Instance &inInstance) {
    auto size = mPainterParam->GetStorageImage().GetImageExtent().height *
                mPainterParam->GetStorageImage().GetImageExtent().width * 4;
    return mPainterParam->GetStorageImage().SubmitImmediateCmdGetImageToVector(
         inInstance.GetGraphicsQueue(),
         inInstance.GetStagingBuffer(size),
         inInstance.GetUtilCommandBuffer(),
         inInstance.GetUtilCommandBufferFence());
}