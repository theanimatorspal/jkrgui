#include "glTF.hpp"

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
