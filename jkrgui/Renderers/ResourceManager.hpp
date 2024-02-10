#pragma once
#include "Painter.hpp"
#include "PainterCache.hpp"
#include <filesystem>
// #include "TrivialShadersLegacy.hpp"

enum class FillType {
    Fill,
    Image,
    ContinousLine
};

namespace Jkr {

class ResourceManager {
public:
    ResourceManager() { Folderize(""); };
    ResourceManager(const sv inFolderPath) { Folderize(inFolderPath); }
    ResourceManager& Load(const Jkr::Instance& inInstance, uint32_t inVarDesCount = 5000);
    ResourceManager& Store(const Jkr::Instance& inInstance, uint32_t inVarDesCount = 5000);
    GETTER& GetLineRendererCache() { return *mLineRendererCache; }
    GETTER& GetFastTextRendererCache() { return *mFastTextRendererCache; }
    GETTER& GetShapePainterCaches() { return mShapePainterCaches; }
    GETTER& GetBestTextRendererCache() { return *mBestTextRendererCache; }
    SETTER SetThreadPool(ksai::ThreadPool& inThreadPool) { mThreadPool = inThreadPool; }

private:
    Up<Jkr::PainterCache> mLineRendererCache;
    Up<Jkr::PainterCache> mFastTextRendererCache;
    Up<Jkr::PainterCache> mBestTextRendererCache;
    umap<FillType, Up<Jkr::PainterCache>> mShapePainterCaches;
    optref<ThreadPool> mThreadPool;

private:
    s LineRendererCacheFileName = "LineRendererCache.bin";
    s FastTextRendererCacheFileName = "FastTextRendererCache.bin";
    s ShapeRendererCacheFileName_Fill = "ShapeRendererCache_Fill.bin";

#ifndef JKR_USE_VARIABLE_DES_INDEXING
    s ShapeRendererCacheFileName_Image = "ShapeRendererCache_Image.bin";
#else
    s ShapeRendererCacheFileName_Image = "ShapeRendererCache_ImageVarDes.bin";
    s BestTextRendererCacheFilename = "BestTextRendererCache.bin";
#endif
    s ShapeRendererCacheFileName_ContinousLine = "ShapeRendererCache_ContinousLine.bin";

    void Make(const Jkr::Instance& inInstance);
    void Folderize(const sv inFolderPath);
};

} // namespace Jkr
