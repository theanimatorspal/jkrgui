#include "CustomImagePainter.hpp"

const std::string_view gbefore_xyz = R"""(
			
#version 450
layout(set = 0, binding = 0, rgba8) uniform image2D storageImage;
                                   
)""";

const std::string_view gmain_function_null = R"""(
void GlslMain()
{

}
)""";

const std::string_view gafter_xyz = R"""(
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

const std::string_view gend = R"""(
}
)""";

Jkr::Renderer::CustomImagePainter::CustomImagePainter(CustomImagePainter &inPainter,
                                                      std::string_view inName,
                                                      std::string_view inComputeShaderFunction,
                                                      std::string_view inPushConstantSignature)
    : mInstance(inPainter.mInstance)
    , mCustomPainterFileName(std::string(inName))
    , mThreads(inPainter.mThreads)
{
    mComputeStream << gbefore_xyz;
    mComputeStream << "layout(local_size_x = " << 16 << ", local_size_y = " << 16 << ","
                   << " local_size_z = " << 1 << ") in;";
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

    mCustomPainterCache = MakeUp<PainterCache>(mInstance);
}

Jkr::Renderer::CustomImagePainter::CustomImagePainter(const Instance& inInstance, sv inName, sv inComputeShaderFunction, sv inPushConstantSignature, ui inX, ui inY, ui inZ)
    : mInstance(mInstance)
    , mCustomPainterFileName(std::string(inName))
    , mThreads(inX, inY, inZ)
{
    mComputeStream << gbefore_xyz;
    mComputeStream << "layout(local_size_x = " << 16 << ", local_size_y = " << 16 << ","
                   << " local_size_z = " << 1 << ") in;";
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

    mCustomPainterCache = MakeUp<PainterCache>(mInstance);
}

void Jkr::Renderer::CustomImagePainter::Load(Window& inWindow)
{
    Make();
    mCustomPainterCache->Load(mCustomPainterFileName);
    mPainter = MakeUp<Painter>(mInstance, inWindow, *mCustomPainterCache);
}

void Jkr::Renderer::CustomImagePainter::Store(Window &inWindow)
{
    Make();
    std::cout << mComputeStream.str() << std::endl;
    mCustomPainterCache->Store(mCustomPainterFileName,
                               mVertexStream.str(),
                               mFragmentStream.str(),
                               mComputeStream.str());
    mPainter = MakeUp<Painter>(mInstance, inWindow, *mCustomPainterCache);
}

void Jkr::Renderer::CustomImagePainter::RegisterImageToBeDrawnTo(Window &inWindow,
                                                                 uint32_t inWidth,
                                                                 uint32_t inHeight)
{
    mPainterParam = MakeUp<Image>(mInstance);
    mPainterParam->Setup(inWidth, inHeight);
    mPainter->RegisterPainterParameter(*mPainterParam, 0, 0, 0);
    mPainter->OptimizeParameter(*mPainterParam, inWindow);
}

void Jkr::Renderer::CustomImagePainter::RegisterImageToBeDrawnTo(Window &inWindow,
                                                                 CustomImagePainter &inExisting)
{
    mPainter->RegisterPainterParameter(*inExisting.GetImagePainterParameter(), 0, 0, 0);
}
