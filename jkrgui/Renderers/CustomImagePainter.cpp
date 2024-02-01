#include "CustomImagePainter.hpp"
#include "AllShaders.hpp"

using namespace Jkr::Renderer;
using namespace ksai;

const sv gbefore_xyz = R"""(
#version 450
layout(set = 0, binding = 0, rgba8) uniform image2D storageImage;
                                   
)""";

const sv gmain_function_null = R"""(
void GlslMain()
{
}
)""";

const sv gmain_function_null_fragment = R"""(
void GlslMain()
{
	
}
)""";

const sv gafter_xyz = R"""(
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

const sv gend = R"""(
}
)""";

CustomImagePainter::CustomImagePainter(sv inName, sv inComputeShaderFunction, sv inPushConstantSignature, ui inX, ui inY, ui inZ)
    : mCustomPainterFileName(std::string(inName))
    , mThreads(inX, inY, inZ)
{
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

void CustomImagePainter::Load(const Instance& inInstance, Window& inWindow)
{
    mCustomPainterCache = MakeUp<PainterCache>(inInstance);
    mCustomPainterCache->Load(mCustomPainterFileName);
    mPainter = MakeUp<Painter>(inInstance, inWindow, *mCustomPainterCache);
}

void CustomImagePainter::Store(const Instance& inInstance, Window& inWindow)
{
    //ksai::Shader Shape_Fill(ShapeRenderers_Fill::VertexShader, ShapeRenderers_Fill::FragmentShader);
    //ksai::Shader Shape_FillCompute(8, ShapeRenderers_Fill::ComputeShader);
    //SpirvHelper::Init();
    //auto xxxx = MakeUp<Jkr::PainterCache>(inInstance);
    //xxxx->Store(mCustomPainterFileName,
    //    Shape_Fill.GetVertexShader().str(),
    //    Shape_Fill.GetFragmentShader().str(),
    //    Shape_FillCompute.GetComputeShader().str());

    mCustomPainterCache = MakeUp<PainterCache>(inInstance);
    mCustomPainterCache->Store(mCustomPainterFileName,
        mVertexStream.str(),
        mFragmentStream.str(),
        mComputeStream.str());
    mPainter = MakeUp<Painter>(inInstance, inWindow, *mCustomPainterCache);
}

