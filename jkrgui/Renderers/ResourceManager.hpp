#pragma once
#include "AllShaders.hpp"
#include "Painter.hpp"
#include "PainterCache.hpp"

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

namespace Jkr::Renderer {

class CustomImagePainter
{
    using Image = PainterParameter<Jkr::PainterParameterContext::StorageImage>;

public:
    CustomImagePainter(const Instance &inInstance,
                       std::string_view inName,
                       std::string_view inComputeShaderFunction,
                       std::string_view inPushConstantSignature,
                       uint32_t inX,
                       uint32_t inY,
                       uint32_t inZ);

    explicit CustomImagePainter(CustomImagePainter &inPainter,
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

    void Load(Window &inWindow);

    void Store(Window &inWindow);

    void RegisterImageToBeDrawnTo(Window &inWindow, uint32_t inWidth, uint32_t inHeight);

    template<class T>
    void Draw(Window &inWindow, T inPushConstant, uint32_t inX, uint32_t inY, uint32_t inZ)
    {
        mPainter->Dispatch<T>(inPushConstant, inX, inY, inZ);
    }

    template<class T>
    void Draw(Window &inWindow, T inPushConstant)
    {
        Draw<T>(inWindow, inPushConstant, mThreads.x, mThreads.y, mThreads.z);
    }

    GETTER &GetImage() { return mPainterParam->GetStorageImage(); }
    GETTER GetImagePtr() { return mPainterParam->GetStorageImagePtr(); }

private:
    void Make() { mCustomPainterCache = MakeUp<PainterCache>(mInstance); }
    const Instance &mInstance;
    std::string mCustomPainterFileName = "customPainter.bin";
    std::ostringstream mComputeStream;
    std::ostringstream mVertexStream;
    std::ostringstream mFragmentStream;
    glm::vec3 mThreads;
    Up<PainterCache> mCustomPainterCache;
    Up<Painter> mPainter;
    Up<Image> mPainterParam;
};

enum class FillType {
    Fill,
    Image,
    ContinousLine
};
class ResourceManager {
public:

private:
    std::string LineRendererCacheFileName = "LineRendererCache.bin";
    std::string FastTextRendererCacheFileName = "FastTextRendererCache.bin";
    std::string ShapeRendererCacheFileName_Fill = "ShapeRendererCache_Fill.bin";

#ifndef JKR_USE_VARIABLE_DES_INDEXING
    std::string ShapeRendererCacheFileName_Image = "ShapeRendererCache_Image.bin";
#else
    std::string ShapeRendererCacheFileName_Image = "ShapeRendererCache_ImageVarDes.bin";
    std::string BestTextRendererCacheFilename = "BestTextRendererCache.bin";
#endif
    std::string ShapeRendererCacheFileName_ContinousLine = "ShapeRendererCache_ContinousLine.bin";

    void Make(const Jkr::Instance& inInstance)
    {
        mLineRendererCache = MakeUp<Jkr::PainterCache>(inInstance, ksai::PipelinePropertiesContext::Line);
        mFastTextRendererCache = MakeUp<Jkr::PainterCache>(inInstance);
        mShapePainterCaches[FillType::Fill] = MakeUp<Jkr::PainterCache>(inInstance);
        mShapePainterCaches[FillType::Image] = MakeUp<Jkr::PainterCache>(inInstance);
        mShapePainterCaches[FillType::ContinousLine] = MakeUp<Jkr::PainterCache>(inInstance, ksai::PipelinePropertiesContext::LineStrip);
        mBestTextRendererCache = MakeUp<Jkr::PainterCache>(inInstance);
    }

public:
    ResourceManager &Load(const Jkr::Instance &inInstance, uint32_t inVarDesCount = 5000)
    {
        Make(inInstance);
        mLineRendererCache->Load(LineRendererCacheFileName);
        mFastTextRendererCache->Load(FastTextRendererCacheFileName);
        mShapePainterCaches[FillType::Fill]->Load(ShapeRendererCacheFileName_Fill);

#ifndef JKR_USE_VARIABLE_DES_INDEXING
        mShapePainterCaches[FillType::Image]->Load(ShapeRendererCacheFileName_Image);
#else
        mShapePainterCaches[FillType::Image]->__var_des_index_Load_EXT(ShapeRendererCacheFileName_Image);
        mBestTextRendererCache->__var_des_index_Load_EXT(BestTextRendererCacheFilename,
                                                         inVarDesCount);
#endif

        mShapePainterCaches[FillType::ContinousLine]->Load(ShapeRendererCacheFileName_ContinousLine);
        return *this;
    }

    ResourceManager &Store(const Jkr::Instance &inInstance, uint32_t inVarDesCount = 5000)
    {
        Make(inInstance);
        ksai::Shader Line(LineRendererShaders::VertexShader, LineRendererShaders::FragmentShader);
        ksai::Shader LineCompute(8, LineRendererShaders::ComputeShader);
        ksai::Shader FastText(FontShader::VertexShader, FontShader::FragmentShader);
        ksai::Shader FastTextCompute(8, FontShader::ComputeShader);
        ksai::Shader Shape_Fill(ShapeRenderers_Fill::VertexShader, ShapeRenderers_Fill::FragmentShader);
        ksai::Shader Shape_FillCompute(8, ShapeRenderers_Fill::ComputeShader);
        ksai::Shader BestTextS(BestText_shader::VertexShader, BestText_shader::FragmentShader);
        ksai::Shader BestText_Compute(8, BestText_shader::ComputeShader);

        mLineRendererCache->Store(LineRendererCacheFileName, Line.GetVertexShader().str(), Line.GetFragmentShader().str(), LineCompute.GetComputeShader().str());
        mFastTextRendererCache->Store(FastTextRendererCacheFileName, FastText.GetVertexShader().str(), FastText.GetFragmentShader().str(), FastTextCompute.GetComputeShader().str());
        mShapePainterCaches[FillType::Fill]->Store(ShapeRendererCacheFileName_Fill, Shape_Fill.GetVertexShader().str(), Shape_Fill.GetFragmentShader().str(), Shape_FillCompute.GetComputeShader().str());

        ksai::Shader Shape_Image(ShapeRenderers_Image::VertexShader, ShapeRenderers_Image::FragmentShader);
        ksai::Shader Shape_ImageCompute(8, ShapeRenderers_Image::ComputeShader);
#ifndef JKR_USE_VARIABLE_DES_INDEXING
        mShapePainterCaches[FillType::Image]->Store(ShapeRendererCacheFileName_Image, Shape_Image.GetVertexShader().str(), Shape_Image.GetFragmentShader().str(), Shape_ImageCompute.GetComputeShader().str());
#else
        mShapePainterCaches[FillType::Image]->__var_des_index_Store_EXT(ShapeRendererCacheFileName_Image, Shape_Image.GetVertexShader().str(), Shape_Image.GetFragmentShader().str(), Shape_ImageCompute.GetComputeShader().str());
        mBestTextRendererCache->__var_des_index_Store_EXT(BestTextRendererCacheFilename,
                                                          BestTextS.GetVertexShader().str(),
                                                          BestTextS.GetFragmentShader().str(),
                                                          BestText_Compute.GetComputeShader().str(),
                                                          inVarDesCount);
#endif

        mShapePainterCaches[FillType::ContinousLine]->Store(ShapeRendererCacheFileName_ContinousLine, Shape_Fill.GetVertexShader().str(), Shape_Fill.GetFragmentShader().str(), Shape_FillCompute.GetComputeShader().str());

        return *this;
    }

    GETTER& GetLineRendererCache() { return *mLineRendererCache; }
    GETTER& GetFastTextRendererCache() { return *mFastTextRendererCache; }
    GETTER& GetShapePainterCaches() { return mShapePainterCaches; }
    GETTER& GetBestTextRendererCache() { return *mBestTextRendererCache; }

private:
    Up<Jkr::PainterCache> mLineRendererCache;
    Up<Jkr::PainterCache> mFastTextRendererCache;
    Up<Jkr::PainterCache> mBestTextRendererCache;
    std::unordered_map<FillType, Up<Jkr::PainterCache>> mShapePainterCaches;
};

inline CustomImagePainter::CustomImagePainter(const Instance &inInstance,
                                              std::string_view inName,
                                              std::string_view inComputeShaderFunction,
                                              std::string_view inPushConstantSignature,
                                              uint32_t inX,
                                              uint32_t inY,
                                              uint32_t inZ)
    : mInstance(inInstance)
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

    mCustomPainterCache = MakeUp<PainterCache>(inInstance);
}

inline void CustomImagePainter::Load(Window &inWindow)
{
    Make();
    mCustomPainterCache->Load(mCustomPainterFileName);
    mPainter = MakeUp<Painter>(mInstance, inWindow, *mCustomPainterCache);
}

inline void CustomImagePainter::Store(Window &inWindow)
{
    Make();
    std::cout << mComputeStream.str() << std::endl;
    mCustomPainterCache->Store(mCustomPainterFileName,
                               mVertexStream.str(),
                               mFragmentStream.str(),
                               mComputeStream.str());
    mPainter = MakeUp<Painter>(mInstance, inWindow, *mCustomPainterCache);
}

inline void CustomImagePainter::RegisterImageToBeDrawnTo(Window &inWindow,
                                                         uint32_t inWidth,
                                                         uint32_t inHeight)
{
    mPainterParam = MakeUp<Image>(mInstance);
    mPainterParam->Setup(inWidth, inHeight);
    mPainter->RegisterPainterParameter(*mPainterParam, 0, 0, 0);
    mPainter->OptimizeParameter(*mPainterParam, inWindow);
}
} // namespace Jkr::Renderer
