#include "Shape3D.hpp"
#include "Global/Standards.hpp"
#include "Painter.hpp"
#include "glTF_Model.hpp"
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

void Shape::CopyToPrimitive(ui inOffsetId, ui inModelId) {
#ifndef JKR_NO_STAGING_BUFFERS
          rb::CopyToStagingBuffers(gb::GetVertexBufferData(),
                                   gb::GetIndexBufferData(),
                                   gb::VertexCountToBytes(gb::GetVertexOffsetAbsolute(inOffsetId)),
                                   gb::IndexCountToBytes(gb::GetIndexOffsetAbsolute(inOffsetId)),
                                   gb::VertexCountToBytes(gb::GetVertexCount(inModelId)),
                                   gb::IndexCountToBytes(gb::GetIndexCount(inModelId)));
          rb::RegisterBufferCopyRegionToPrimitiveFromStaging(gb::VertexCountToBytes(gb::GetVertexOffsetAbsolute(inOffsetId)),
                                                             gb::IndexCountToBytes(gb::GetIndexOffsetAbsolute(inOffsetId)),
                                                             gb::VertexCountToBytes(gb::GetVertexCount(inModelId)),
                                                             gb::IndexCountToBytes(gb::GetIndexCount(inModelId)));
#else
          rb::CopyToPrimitive(*mPrimitive,
                              gb::GetVertexBufferData(),
                              gb::GetIndexBufferData(),
                              gb::VertexCountToBytes(gb::GetVertexOffsetAbsolute(inOffsetId)),
                              gb::IndexCountToBytes(gb::GetIndexOffsetAbsolute(inOffsetId)),
                              gb::VertexCountToBytes(gb::GetVertexCount(inModelId)),
                              gb::IndexCountToBytes(gb::GetIndexCount(inModelId)));
#endif
}

void Shape::Add(const std::string_view inFileName, uint32_t& outId) {
          mModels.emplace_back(MakeUp<glTF_Model>(inFileName, gb::GetCurrentVertexOffset()));
          CheckAndResize(*mModels.back());
          gb::Add(mModels.back()->GetVerticesRef(), mModels.back()->GetIndicesRef(), outId);
          const auto OffsetId = (uint32_t)outId;
          const auto ModelId = (uint32_t)outId;
          CopyToPrimitive(OffsetId, ModelId);
}

void Shape::Add(Generator& inGenerator, glm::vec3 inPosition, ui& outId) {
          v<ui> Indices;
          v<Vertex3D> Vertices;
          inGenerator(inPosition.x, inPosition.y, inPosition.z, gb::GetCurrentVertexOffset(), 0, Vertices, Indices);
          CheckAndResize(Vertices.size(), Indices.size());
          gb::Add(Vertices, Indices, outId);
          const auto OffsetId = (uint32_t)outId;
          const auto ModelId = (uint32_t)outId;
          CopyToPrimitive(OffsetId, ModelId);
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

void Shape::CheckAndResize(size_t inIndicesSize, size_t inVerticesSize) {
          bool ResizeRequired = false;
          while (true) {
                    bool ResizeRequiredi = (inIndicesSize + gb::GetCurrentIndexOffset() >= mTotalNoOfIndicesRendererCanHold) or
                                           (inVerticesSize + gb::GetCurrentVertexOffset() >= mTotalNoOfVerticesRendererCanHold);
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
                    mPrimitive = MakeUp<Primitive>(mInstance,
                                                   gb::VertexCountToBytes(mTotalNoOfVerticesRendererCanHold),
                                                   gb::IndexCountToBytes(mTotalNoOfIndicesRendererCanHold));
                    mPrimitive->SetIndexCount(mTotalNoOfIndicesRendererCanHold);

                    gb::Resize(mTotalNoOfVerticesRendererCanHold, mTotalNoOfIndicesRendererCanHold);

#ifndef JKR_NO_STAGING_BUFFERS
                    rb::ResizeStagingBuffer(mInstance,
                                            gb::VertexCountToBytes(mTotalNoOfVerticesRendererCanHold),
                                            gb::IndexCountToBytes(mTotalNoOfIndicesRendererCanHold));
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
void Shape::Update(ui inId, Generator& inGenerator, glm::vec3 inPosition) {
          // TODO To be Implemented
}

void Shape::CheckAndResize(const glTF_Model& inModel) { CheckAndResize(inModel.GetIndices().size(), inModel.GetVertices().size()); }