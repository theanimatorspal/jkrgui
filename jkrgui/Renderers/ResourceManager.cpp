#include "ResourceManager.hpp"
using namespace Jkr;

void ResourceManager::Make(const Jkr::Instance& inInstance)
{
#ifndef ANDROID
    mLineRendererCache = MakeUp<Jkr::PainterCache>(inInstance,
        ksai::PipelinePropertiesContext::Line);
#else
    mLineRendererCache = MakeUp<Jkr::PainterCache>(inInstance);
#endif

    mFastTextRendererCache = MakeUp<Jkr::PainterCache>(inInstance);
    mShapePainterCaches[FillType::Fill] = MakeUp<Jkr::PainterCache>(inInstance);
    mShapePainterCaches[FillType::Image] = MakeUp<Jkr::PainterCache>(inInstance);

#ifndef ANDROID
    mShapePainterCaches[FillType::ContinousLine] = MakeUp<Jkr::PainterCache>(inInstance, ksai::PipelinePropertiesContext::LineStrip);
#else
    mShapePainterCaches[FillType::ContinousLine] = MakeUp<Jkr::PainterCache>(inInstance);
#endif

    mBestTextRendererCache = MakeUp<Jkr::PainterCache>(inInstance);
}

void ResourceManager::Folderize(const std::string_view inFolderPath)
{
    if (inFolderPath != "" and not std::filesystem::exists(inFolderPath))
        std::filesystem::create_directories(inFolderPath);

    auto Path = std::string(inFolderPath);
    LineRendererCacheFileName = Path + "LineRendererCache.bin";
    FastTextRendererCacheFileName = Path + "FastTextRendererCache.bin";
    ShapeRendererCacheFileName_Fill = Path + "ShapeRendererCache_Fill.bin";

#ifndef JKR_USE_VARIABLE_DES_INDEXING
    ShapeRendererCacheFileName_Image = Path + "ShapeRendererCache_Image.bin";
#else
    ShapeRendererCacheFileName_Image = Path + "ShapeRendererCache_ImageVarDes.bin";
    BestTextRendererCacheFilename = Path + "BestTextRendererCache.bin";
#endif
    ShapeRendererCacheFileName_ContinousLine = Path + "ShapeRendererCache_ContinousLine.bin";
}

ResourceManager& ResourceManager::Load(const Jkr::Instance& inInstance, uint32_t inVarDesCount)
{
    Make(inInstance);
    ksai_print("line Renderer Load Start");
    mLineRendererCache->Load(LineRendererCacheFileName);
    ksai_print("Fastext Renderer Load Start");
    mFastTextRendererCache->Load(FastTextRendererCacheFileName);
    ksai_print("ShapeFIll Renderer Load Start ");
    mShapePainterCaches[FillType::Fill]->Load(ShapeRendererCacheFileName_Fill);

    ksai_print("ShapeFillImage Load Start ");
#ifndef JKR_USE_VARIABLE_DES_INDEXING
    mShapePainterCaches[FillType::Image]->Load(ShapeRendererCacheFileName_Image);
#else
    mShapePainterCaches[FillType::Image]->__var_des_index_Load_EXT(ShapeRendererCacheFileName_Image,
        inVarDesCount);
    mBestTextRendererCache->__var_des_index_Load_EXT(BestTextRendererCacheFilename, inVarDesCount);
#endif
    ksai_print("ShapeFillContinousline Load Start ");
    mShapePainterCaches[FillType::ContinousLine]->Load(ShapeRendererCacheFileName_ContinousLine);
    ksai_print("Loaded All Painter Caches");
    return *this;
}

ResourceManager& ResourceManager::Store(const Jkr::Instance& inInstance, uint32_t inVarDesCount)
{
    Make(inInstance);
    ksai::Shader Line(LineRendererShaders::VertexShader, LineRendererShaders::FragmentShader);
    ksai::Shader LineCompute(8, LineRendererShaders::ComputeShader);
    ksai::Shader FastText(FontShader::VertexShader, FontShader::FragmentShader);
    ksai::Shader FastTextCompute(8, FontShader::ComputeShader);
    ksai::Shader Shape_Fill(ShapeRenderers_Fill::VertexShader, ShapeRenderers_Fill::FragmentShader);
    ksai::Shader Shape_FillCompute(8, ShapeRenderers_Fill::ComputeShader);

    auto mLineRendererCacheF = [&]() { mLineRendererCache->Store(LineRendererCacheFileName,
                                           Line.GetVertexShader().str(),
                                           Line.GetFragmentShader().str(),
                                           LineCompute.GetComputeShader().str()); };
    auto mFastTextRendererCacheF = [&]() { mFastTextRendererCache->Store(FastTextRendererCacheFileName,
                                               FastText.GetVertexShader().str(),
                                               FastText.GetFragmentShader().str(),
                                               FastTextCompute.GetComputeShader().str()); };
    auto mShapePainterCaches_fillF = [&]() { mShapePainterCaches[FillType::Fill]->Store(ShapeRendererCacheFileName_Fill,
                                                 Shape_Fill.GetVertexShader().str(),
                                                 Shape_Fill.GetFragmentShader().str(),
                                                 Shape_FillCompute.GetComputeShader().str()); };

    ksai::Shader Shape_Image(ShapeRenderers_Image::VertexShader,
        ShapeRenderers_Image::FragmentShader);
    ksai::Shader Shape_ImageCompute(8, ShapeRenderers_Image::ComputeShader);

#ifndef JKR_USE_VARIABLE_DES_INDEXING
    auto mShapePainterChaches_imgfillF = [&]() { mShapePainterCaches[FillType::Image]->Store(ShapeRendererCacheFileName_Image,
                                                     Shape_Image.GetVertexShader().str(),
                                                     Shape_Image.GetFragmentShader().str(),
                                                     Shape_ImageCompute.GetComputeShader().str()); };

#else

    ksai::Shader BestTextS(BestText_shader::VertexShader, BestText_shader::FragmentShader);
    ksai::Shader BestText_Compute(8, BestText_shader::ComputeShader);

    auto mShapePainterChaches_imgfillF = [&]() { mShapePainterCaches[FillType::Image]
                                                ->__var_des_index_Store_EXT(ShapeRendererCacheFileName_Image,
                                                    Shape_Image.GetVertexShader().str(),
                                                    Shape_Image.GetFragmentShader().str(),
                                                    Shape_ImageCompute.GetComputeShader().str(),
                                                    inVarDesCount); };
    auto mBestTextRendererCacheF = [&]() { mBestTextRendererCache->__var_des_index_Store_EXT(BestTextRendererCacheFilename,
                                               BestTextS.GetVertexShader().str(),
                                               BestTextS.GetFragmentShader().str(),
                                               BestText_Compute.GetComputeShader().str(),
                                               inVarDesCount); };
#endif

    auto mShapePainterCaches_ContiF = [&]() { mShapePainterCaches[FillType::ContinousLine]->Store(ShapeRendererCacheFileName_ContinousLine,
                                                  Shape_Fill.GetVertexShader().str(),
                                                  Shape_Fill.GetFragmentShader().str(),
                                                  Shape_FillCompute.GetComputeShader().str()); };

    if (mThreadPool.has_value()) {
        auto& T = mThreadPool.value().get();
        std::vector<std::function<void()>> vec = {
            mLineRendererCacheF,
            mFastTextRendererCacheF,
            mShapePainterCaches_fillF,
            mShapePainterChaches_imgfillF,
#ifdef JKR_USE_VARIABLE_DES_INDEXING
            mBestTextRendererCacheF,
#endif
            mShapePainterCaches_ContiF
        };
        T.Add_Job(vec);
        T.Wait();
    } else {
        mLineRendererCacheF();
        mFastTextRendererCacheF();
        mShapePainterCaches_fillF();
        mShapePainterChaches_imgfillF();
#ifdef JKR_USE_VARIABLE_DES_INDEXING
        mBestTextRendererCacheF();
#endif
        mShapePainterCaches_ContiF();
    }
    return *this;
}
