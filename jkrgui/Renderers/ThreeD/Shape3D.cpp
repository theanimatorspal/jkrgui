#include "Shape3D.hpp"
#include "Painter.hpp"
#include <filesystem>

using namespace Jkr::Renderer::_3D;

Shape::Shape(const Instance& inInstance, Window& inCompatibleWindow) : mInstance(inInstance) {
#ifndef JKR_NO_STAGING_BUFFERS
          rb::CreateStagingBuffers(
           inInstance, gb::VertexCountToBytes(mTotalNoOfVerticesRendererCanHold), gb::IndexCountToBytes(mTotalNoOfIndicesRendererCanHold));
#endif
          mPrimitive = MakeUp<Primitive>(
           inInstance, gb::VertexCountToBytes(mTotalNoOfVerticesRendererCanHold), gb::IndexCountToBytes(mTotalNoOfIndicesRendererCanHold));
          mPrimitive->SetIndexCount(mTotalNoOfIndicesRendererCanHold);

#ifndef JKR_NO_STAGING_BUFFERS
//	mGlobalkj
#endif
}

void Shape::Add(const std::string_view inFileName, uint32_t& outId) {
          mModels.emplace_back(MakeUp<glTF_Model>(inFileName));
          CheckAndResize(*mModels.back());
          gb::Add(mModels.back()->GetVerticesRef(), mModels.back()->GetIndicesRef(), outId);

          const auto OffsetId = (uint32_t)outId;
          const auto ModelId = (uint32_t)outId;

#ifndef JKR_NO_STAGING_BUFFERS
          rb::CopyToStagingBuffers(gb::GetVertexBufferData(),
                                   gb::GetIndexBufferData(),
                                   gb::VertexCountToBytes(gb::GetVertexOffsetAbsolute(OffsetId)),
                                   gb::IndexCountToBytes(gb::GetIndexOffsetAbsolute(OffsetId)),
                                   gb::VertexCountToBytes(gb::GetVertexCount(ModelId)),
                                   gb::IndexCountToBytes(gb::GetIndexCount(ModelId)));
          rb::RegisterBufferCopyRegionToPrimitiveFromStaging(gb::VertexCountToBytes(gb::GetVertexOffsetAbsolute(OffsetId)),
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

void Shape::Dispatch(Window& inWindow) {
#ifndef JKR_NO_STAGING_BUFFERS
          if (not rb::IsCopyRegionsEmpty()) {
                    rb::CmdCopyToPrimitiveFromStagingBuffer(mInstance,
                                                            *mPrimitive,
                                                            inWindow,
                                                            gb::VertexCountToBytes(mTotalNoOfVerticesRendererCanHold),
                                                            gb::IndexCountToBytes(mTotalNoOfIndicesRendererCanHold));
          }
#endif
}

void Shape::CheckAndResize(const glTF_Model& inModel) {
          bool ResizeRequired = false;
          while (true) {
                    bool ResizeRequiredi = (inModel.GetIndices().size() + gb::GetCurrentIndexOffset() >= mTotalNoOfIndicesRendererCanHold) or
                                           (inModel.GetVertices().size() + gb::GetCurrentVertexOffset() >= mTotalNoOfVerticesRendererCanHold);
                    if (ResizeRequiredi) {
                              mTotalNoOfVerticesRendererCanHold *= rb::RendererCapacityResizeFactor;
                              mTotalNoOfIndicesRendererCanHold *= rb::RendererCapacityResizeFactor;
                              ResizeRequired = true;
                    } else {
                              break;
                    }
          }

          if (ResizeRequired) {
                    mPrimitive.reset();
                    mPrimitive = MakeUp<Primitive>(
                     mInstance, gb::VertexCountToBytes(mTotalNoOfVerticesRendererCanHold), gb::IndexCountToBytes(mTotalNoOfIndicesRendererCanHold));
                    mPrimitive->SetIndexCount(mTotalNoOfIndicesRendererCanHold);

                    gb::Resize(mTotalNoOfVerticesRendererCanHold, mTotalNoOfIndicesRendererCanHold);

#ifndef JKR_NO_STAGING_BUFFERS
                    rb::ResizeStagingBuffer(
                     mInstance, gb::VertexCountToBytes(mTotalNoOfVerticesRendererCanHold), gb::IndexCountToBytes(mTotalNoOfIndicesRendererCanHold));
                    rb::CopyToStagingBuffers(gb::GetVertexBufferData(),
                                             gb::GetIndexBufferData(),
                                             gb::VertexCountToBytes(0),
                                             gb::IndexCountToBytes(0),
                                             gb::VertexCountToBytes(mTotalNoOfVerticesRendererCanHold),
                                             gb::IndexCountToBytes(mTotalNoOfIndicesRendererCanHold));
                    rb::RegisterBufferCopyRegionToPrimitiveFromStaging(gb::VertexCountToBytes(0),
                                                                       gb::IndexCountToBytes(0),
                                                                       gb::VertexCountToBytes(mTotalNoOfVerticesRendererCanHold),
                                                                       gb::IndexCountToBytes(mTotalNoOfIndicesRendererCanHold));
#else
                    rb::CopyToPrimitive(*mPrimitive,
                                        gb::GetVertexBufferData(),
                                        gb::GetIndexBufferData(),
                                        gb::VertexCountToBytes(0),
                                        gb::IndexCountToBytes(0),
                                        gb::VertexCountToBytes(mTotalNoOfVerticesRendererCanHold),
                                        gb::IndexCountToBytes(mTotalNoOfIndicesRendererCanHold));
#endif
          }
}
