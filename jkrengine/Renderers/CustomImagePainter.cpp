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

void CustomImagePainter::Load(Instance &inInstance, Window_base &inWindow) {
    mCustomPainterCache = MakeUp<PainterCache>(inInstance);
    mCustomPainterCache->Load(mCustomPainterFileName);
    mPainter = MakeUp<Painter>(inInstance, inWindow, *mCustomPainterCache);
}

void CustomImagePainter::Store(Instance &inInstance, Window_base &inWindow) {
    mCustomPainterCache = MakeUp<PainterCache>(inInstance);
    mCustomPainterCache->Store(
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

std::vector<int> CustomPainterImage::GetImageToVector(Instance &inInstance,
                                                      const Window_base &inWindow) {
    ui ImageChannels = 4;
    auto ImageExtent = mPainterParam->GetStorageImage().GetImageExtent();
    auto Size        = ImageChannels * ImageExtent.width * ImageExtent.height;
    uint32_t size    = ImageChannels * ImageExtent.width * ImageExtent.height;
    VulkanBufferVMA Buffer(inInstance.GetVMA(),
                           inInstance.GetDevice(),
                           size,
                           BufferContext::Storage,
                           MemoryType::HostVisibleAndCoherenet);
    Buffer.SubmitImmediateCmdCopyFromImage(
         inInstance.GetGraphicsQueue(),
         inWindow.GetCommandBuffers(Jkr::Window_base::None)[inWindow.GetCurrentFrame()],
         mPainterParam->GetStorageImage(),
         vk::ImageLayout::eGeneral);
    void *MemoryRegion;
    Buffer.MapMemoryRegion(&MemoryRegion);
    std::vector<uint8_t> OutImage;
    std::vector<int> OutImage_i;
    OutImage.resize(size);
    OutImage_i.reserve(size);
    std::memcpy(OutImage.data(), MemoryRegion, size);

    for (int i = 0; i < OutImage.size(); i++) {
        OutImage_i.push_back(OutImage[i]);
    }
    return OutImage_i;
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
                                       const Window_base &inWindow,
                                       ui inWidth,
                                       ui inHeight) {
    mPainterParam = mu<Image>(inInstance);
    mPainterParam->Setup(inWidth, inHeight);
    Painter::OptimizeParameter(inInstance, *mPainterParam, inWindow);
}

void CustomImagePainter::BindImageFromImage(const Window_base &inWindow,
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
