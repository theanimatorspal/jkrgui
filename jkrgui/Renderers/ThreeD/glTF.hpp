#pragma once
#include "Primitive.hpp"
#include "glTF_Model.hpp"
#include "glTF_base.hpp"
#include <Renderers/Renderer_base.hpp>

namespace Jkr::Renderer::_3D {

class glTF : public Renderer_base, public glTF_base
{
    using gb = glTF_base;
    using rb = Renderer_base;

    /* TODO: to be removed, khai maile kina lekhe */
    struct MemoryArea
    {
        glm::uvec2 mVertexIdStartEnd;
        glm::uvec2 mIndexIdStartEnd;
    };

public:
    glTF(const Instance &inInstance, Window &inCompatibleWindow, PainterCache &inPainterCache);
    void Add(const std::string_view inFileName, uint32_t &outId);
    void AddPainter(Up<Painter> &inPainter, uint32_t &outId)
    {
        outId = mPainters.size();
        mPainters.push_back(std::move(inPainter));
    }

    void BindPainter(uint32_t inId) {}

private:
    void CheckAndResize(const glTF_Model &inModel) {}
    std::vector<Up<glTF_Model>> mModels;
    std::vector<Up<Painter>> mPainters;
    uint32_t mBoundPainter = 0;
    Up<Primitive> mPrimitive;
    const Instance &mInstance;

private:
    uint32_t mTotalNoOfVerticesRendererCanHold = rb::InitialRendererElementArraySize;
    uint32_t mTotalNoOfIndicesRendererCanHold = rb::InitialRendererElementArraySize;
};
} // namespace Jkr::Renderer::_3D
