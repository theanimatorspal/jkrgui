#include "Line.hpp"
#include <glm/ext/matrix_clip_space.hpp>

Jkr::Renderer::Line::Line(const Instance& inInstance,
                          Window& inCompatibleWindow,
                          PainterCache& inPainterCache)
    : mInstance(inInstance) {
    mPainter = MakeUp<Painter>(inInstance, inCompatibleWindow, inPainterCache);
#ifndef JKR_NO_STAGING_BUFFERS
    rb::CreateStagingBuffers(inInstance,
                             lb::LineCountToVertexBytes(mTotalNoOfLinesRendererCanHold),
                             lb::LineCountToIndexBytes(mTotalNoOfLinesRendererCanHold));
#endif
    mPrimitive = MakeUp<Primitive>(inInstance,
                                   lb::LineCountToVertexBytes(mTotalNoOfLinesRendererCanHold),
                                   lb::LineCountToIndexBytes(mTotalNoOfLinesRendererCanHold));
}

void Jkr::Renderer::Line::AddLine(glm::vec2 inFirstPoint,
                                  glm::vec2 inSecondPoint,
                                  float inDepthValue,
                                  uint32_t& outId) {
    CheckAndResize(mInstance, 1);
    lb::AddLine(inFirstPoint, inSecondPoint, inDepthValue);
#ifndef JKR_NO_STAGING_BUFFERS
    rb::CopyToStagingBuffers(lb::GetVertexBufferData(),
                             lb::GetIndexBufferData(),
                             lb::LineCountToVertexBytes(lb::GetCurrentLineOffset()),
                             lb::LineCountToIndexBytes(lb::GetCurrentLineOffset()),
                             lb::LineCountToVertexBytes(1),
                             lb::LineCountToIndexBytes(1));
    rb::RegisterBufferCopyRegionToPrimitiveFromStaging(
         lb::LineCountToVertexBytes(lb::GetCurrentLineOffset()),
         lb::LineCountToIndexBytes(lb::GetCurrentLineOffset()),
         lb::LineCountToVertexBytes(1),
         lb::LineCountToIndexBytes(1));
#else
    rb::CopyToPrimitive(*mPrimitive,
                        lb::GetVertexBufferData(),
                        lb::GetIndexBufferData(),
                        lb::LineCountToVertexBytes(lb::GetCurrentLineOffset()),
                        lb::LineCountToIndexBytes(lb::GetCurrentLineOffset()),
                        lb::LineCountToVertexBytes(1),
                        lb::LineCountToIndexBytes(1));

#endif
    outId = lb::GetCurrentLineOffset();
}

void Jkr::Renderer::Line::UpdateLine(uint32_t inId,
                                     glm::vec2 inFirstPoint,
                                     glm::vec2 inSecondPoint,
                                     float inDepthValue) {
    auto LineOffset = inId;
    lb::UpdateLine(inId, inFirstPoint, inSecondPoint, inDepthValue);
#ifndef JKR_NO_STAGING_BUFFERS
    rb::CopyToStagingBuffers(lb::GetVertexBufferData(),
                             lb::GetIndexBufferData(),
                             lb::LineCountToVertexBytes(LineOffset),
                             lb::LineCountToIndexBytes(LineOffset),
                             lb::LineCountToVertexBytes(1),
                             lb::LineCountToIndexBytes(1));
    rb::RegisterBufferCopyRegionToPrimitiveFromStaging(lb::LineCountToVertexBytes(LineOffset),
                                                       lb::LineCountToIndexBytes(LineOffset),
                                                       lb::LineCountToVertexBytes(1),
                                                       lb::LineCountToIndexBytes(1));
#else
    rb::CopyToPrimitive(*mPrimitive,
                        lb::GetVertexBufferData(),
                        lb::GetIndexBufferData(),
                        lb::LineCountToVertexBytes(LineOffset),
                        lb::LineCountToIndexBytes(LineOffset),
                        lb::LineCountToVertexBytes(1),
                        lb::LineCountToIndexBytes(1));
#endif
}

void Jkr::Renderer::Line::Dispatch(Window& inWindow) {
#ifndef JKR_NO_STAGING_BUFFERS
    if (!rb::IsCopyRegionsEmpty()) {
        rb::CmdCopyToPrimitiveFromStagingBuffer(
             mInstance,
             *mPrimitive,
             inWindow,
             lb::LineCountToVertexBytes(mTotalNoOfLinesRendererCanHold),
             lb::LineCountToIndexBytes(mTotalNoOfLinesRendererCanHold));
    }
#else

#endif
}

void Jkr::Renderer::Line::Bind(Window& inWindow) {
    // TODO Improve this
    // mPainter->BindDrawParamters_EXT(*mPrimitive, inWindow);
}

void Jkr::Renderer::Line::Draw(Window& inWindow,
                               glm::vec4 inColor,
                               uint32_t inWindowW,
                               uint32_t inWindowH,
                               uint32_t inStartLineId,
                               uint32_t inEndLineId,
                               glm::mat4 inMatrix) {
    glm::mat4 Matrix = glm::ortho(0.0f,
                                  static_cast<float>(inWindowW) * mSizeFactor.x,
                                  0.0f,
                                  static_cast<float>(inWindowH) * mSizeFactor.y,
                                  100.0f,
                                  -100.0f) *
                       inMatrix;
    PushConstant Push;
    Push.mColor  = inColor;
    Push.mMatrix = Matrix;

    mPainter->Draw_EXT<PushConstant>(
         Push, inWindow, 2 * (inEndLineId - inStartLineId + 1), 1, inStartLineId * 2, 0);
}

void Jkr::Renderer::Line::CheckAndResize(const Instance& inInstance, uint32_t inNewSizeNeeded) {
    bool ResizeRequired = false;
    while (true) {
        bool ResizeRequiredi =
             (inNewSizeNeeded + lb::GetCurrentLineOffset() >= mTotalNoOfLinesRendererCanHold);
        if (ResizeRequiredi) {
            mTotalNoOfLinesRendererCanHold *= rb::RendererCapacityResizeFactor;
            ResizeRequired = true;
        } else {
            break;
        }
    }

    if (ResizeRequired) {
        lb::Resize(mTotalNoOfLinesRendererCanHold);
        mPrimitive.reset();
        mPrimitive = MakeUp<Primitive>(inInstance,
                                       lb::LineCountToVertexBytes(mTotalNoOfLinesRendererCanHold),
                                       lb::LineCountToIndexBytes(mTotalNoOfLinesRendererCanHold));
#ifndef JKR_NO_STAGING_BUFFERS
        rb::ResizeStagingBuffer(inInstance,
                                lb::LineCountToVertexBytes(mTotalNoOfLinesRendererCanHold),
                                lb::LineCountToIndexBytes(mTotalNoOfLinesRendererCanHold));
        rb::CopyToStagingBuffers(lb::GetVertexBufferData(),
                                 lb::GetIndexBufferData(),
                                 lb::LineCountToVertexBytes(0),
                                 lb::LineCountToIndexBytes(0),
                                 lb::LineCountToVertexBytes(mTotalNoOfLinesRendererCanHold),
                                 lb::LineCountToIndexBytes(mTotalNoOfLinesRendererCanHold));
        rb::RegisterBufferCopyRegionToPrimitiveFromStaging(
             lb::LineCountToVertexBytes(0),
             lb::LineCountToIndexBytes(0),
             lb::LineCountToVertexBytes(mTotalNoOfLinesRendererCanHold),
             lb::LineCountToIndexBytes(mTotalNoOfLinesRendererCanHold));
#else
        rb::CopyToPrimitive(*mPrimitive,
                            lb::GetVertexBufferData(),
                            lb::GetIndexBufferData(),
                            lb::LineCountToVertexBytes(0),
                            lb::LineCountToIndexBytes(0),
                            lb::LineCountToVertexBytes(mTotalNoOfLinesRendererCanHold),
                            lb::LineCountToIndexBytes(mTotalNoOfLinesRendererCanHold));
#endif
    }
}
