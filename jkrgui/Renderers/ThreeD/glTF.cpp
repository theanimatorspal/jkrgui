#include "glTF.hpp"
#include "Painter.hpp"
#include <filesystem>

Jkr::Renderer::_3D::glTF::glTF(const Instance &inInstance,
                               Window &inCompatibleWindow,
                               PainterCache &inPainterCache)
    : mInstance(inInstance)
{
#ifndef JKR_NO_STAGING_BUFFERS
    rb::CreateStagingBuffers(inInstance,
                             gb::VertexCountToBytes(mTotalNoOfVerticesRendererCanHold),
                             gb::IndexCountToBytes(mTotalNoOfIndicesRendererCanHold));
#endif
    mPrimitive = MakeUp<Primitive>(inInstance,
                                   gb::VertexCountToBytes(mTotalNoOfVerticesRendererCanHold),
                                   gb::IndexCountToBytes(mTotalNoOfIndicesRendererCanHold));
}

void Jkr::Renderer::_3D::glTF::Add(const std::string_view inFileName, uint32_t &outId)
{
    mModels.emplace_back(MakeUp<glTF_Model>(mInstance, inFileName));
    CheckAndResize(*mModels.back());
    gb::Add(mModels.back()->GetVerticesRef(), mModels.back()->GetIndicesRef(), outId);

    const auto OffsetId = (uint32_t) outId;
    const auto ModelId = (uint32_t) outId;
#ifndef JKR_NO_STAGING_BUFFERS
    rb::CopyToStagingBuffers(gb::GetVertexBufferData(),
                             gb::GetIndexBufferData(),
                             gb::VertexCountToBytes(gb::GetVertexOffsetAbsolute(OffsetId)),
                             gb::IndexCountToBytes(gb::GetIndexOffsetAbsolute(OffsetId)),
                             gb::VertexCountToBytes(gb::GetVertexCount(ModelId)),
                             gb::IndexCountToBytes(gb::GetIndexCount(ModelId)));
    rb::RegisterBufferCopyRegionToPrimitiveFromStaging(
        gb::VertexCountToBytes(gb::GetVertexOffsetAbsolute(OffsetId)),
        gb::IndexCountToBytes(gb::GetIndexOffsetAbsolute(OffsetId)),
        gb::VertexCountToBytes(gb::GetVertexCount(ModelId)),
        gb::IndexCountToBytes(gb::GetIndexCount(ModelId)));
#else
    rb::CopyToPrimitive(*mPrimitive,
                        gb::GetVertexBufferData(),
                        gb::GetIndexBufferData(),
                        gb::VertexCountToBytes(gb::GetVertexOffsetAbsolute(OffsetId)),
                        gb::IndexCountToBytes(gb::GetIndexOffsetAbsolute(OffsetId)),
                        gb::VertexCountToBytes(gb::GetVertexCount(ModelId)),
                        gb::IndexCountToBytes(gb::GetIndexCount(ModelId)));
#endif
}

void Jkr::Renderer::_3D::glTF::AddPainter(const std::string_view inFileName,
                                          const std::string_view inVertexShader,
                                          const std::string_view inFragmentShader,
                                          const std::string_view inComputeShader,
                                          Window &inCompatibleWindow,
                                          uint32_t &outId)
{
    Up<PainterCache> painterCache = MakeUp<PainterCache>(mInstance);
    if (std::filesystem::exists(inFileName)) {
        painterCache->Load(std::string(inFileName));
    } else {
        painterCache->Store(std::string(inFileName),
                            std::string(inVertexShader),
                            std::string(inFragmentShader),
                            std::string(inComputeShader));
    }
    Up<Painter> painter = MakeUp<Painter>(mInstance, inCompatibleWindow, *painterCache);
    AddPainter(std::move(painter), outId);
}

void Jkr::Renderer::_3D::glTF::AddPainter(Up<Painter> inPainter, uint32_t &outId)
{
    outId = mPainters.size();
    mPainters.push_back(std::move(inPainter));
}