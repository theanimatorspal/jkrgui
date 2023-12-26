#pragma once
#include "Primitive.hpp"
#include "glTF_Model.hpp"
#include "glTF_base.hpp"
#include <Renderers/Renderer_base.hpp>
#include <string>

namespace Jkr::Renderer::_3D {

class glTF : public Renderer_base, public glTF_base {
    using gb = glTF_base;
    using rb = Renderer_base;

    /* TODO: to be removed, khai maile kina lekhe */
    struct MemoryArea {
        glm::uvec2 mVertexIdStartEnd;
        glm::uvec2 mIndexIdStartEnd;
    };

public:
    glTF(const Instance& inInstance, Window& inCompatibleWindow);
    void Add(const std::string_view inFileName, uint32_t& outId);
    void AddPainter(const std::string_view inFileName,
        const std::string_view inVertexShader,
        const std::string_view inFragmentShader,
        const std::string_view inComputeShader,
        Window& inCompatibleWindow,
        uint32_t& outId);

    void Bind(Window& inWindow)
    {
        Painter::BindDrawParamtersVertexAndIndexBuffersOnly_EXT(mInstance, *mPrimitive, inWindow);
    }

    void PainterBindDraw(uint32_t inPainterId, Window& inWindow)
    {
        mPainters[inPainterId]->BindDrawParamtersPipelineOnly_EXT(*mPrimitive, inWindow);
    }

    void PainterBindDispatch(uint32_t inId)
    {
    }

    template <typename T>
    void PainterDraw(uint32_t inId, T inPush, Window& inWindow)
    {
        mPainters[mBoundPainterVF]->Draw_EXT<T>(*mPrimitive, inPush, inWindow, gb::GetIndexCount(inId), 1, 0, 0);
    }

    void PainterDispatch(uint32_t inId)
    {
    }

    void Dispatch(Window& inWindow);

private:
    void AddPainter(Up<Painter> inPainter, Up<PainterCache> inPainterCaches, uint32_t& outId);
    void CheckAndResize(const glTF_Model& inModel)
    {
        bool ResizeRequired = false;
        while (true) {
            bool ResizeRequiredi = (inModel.GetIndices().size() + gb::GetCurrentIndexOffset() >= mTotalNoOfIndicesRendererCanHold) or (inModel.GetVertices().size() + gb::GetCurrentVertexOffset() >= mTotalNoOfVerticesRendererCanHold);
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
                mInstance,
                gb::VertexCountToBytes(mTotalNoOfVerticesRendererCanHold),
                gb::IndexCountToBytes(mTotalNoOfIndicesRendererCanHold));
            mPrimitive->SetIndexCount(mTotalNoOfIndicesRendererCanHold);

            gb::Resize(mTotalNoOfVerticesRendererCanHold, mTotalNoOfIndicesRendererCanHold);

#ifndef JKR_NO_STAGING_BUFFERS
            rb::ResizeStagingBuffer(
                mInstance,
                gb::VertexCountToBytes(mTotalNoOfVerticesRendererCanHold),
                gb::IndexCountToBytes(mTotalNoOfIndicesRendererCanHold));
            rb::CopyToStagingBuffers(
                gb::GetVertexBufferData(),
                gb::GetIndexBufferData(),
                gb::VertexCountToBytes(0),
                gb::IndexCountToBytes(0),
                gb::VertexCountToBytes(mTotalNoOfVerticesRendererCanHold),
                gb::IndexCountToBytes(mTotalNoOfIndicesRendererCanHold));
            rb::RegisterBufferCopyRegionToPrimitiveFromStaging(
                gb::VertexCountToBytes(0),
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
    std::vector<Up<glTF_Model>> mModels;
    std::vector<Up<Painter>> mPainters;
    std::vector<Up<PainterCache>> mPainterCaches;
    uint32_t mBoundPainterVF = 0;
    uint32_t mBoundPainterC = 0; // Compute
    Up<Primitive> mPrimitive;
    const Instance& mInstance;

private:
    uint32_t mTotalNoOfVerticesRendererCanHold = rb::InitialRendererElementArraySize;
    uint32_t mTotalNoOfIndicesRendererCanHold = rb::InitialRendererElementArraySize;
};
} // namespace Jkr::Renderer::_3D
