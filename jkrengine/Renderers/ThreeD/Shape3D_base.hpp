#pragma once
#include <Global/Standards.hpp>
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

namespace Jkr::Renderer::_3D {
using namespace ksai::kstd;
using namespace ksai;
class Shape;

class Shape_base {
    public:
    struct VertexIndexMemOffset {
        uint32_t mVertexOffset;
        uint32_t mIndexOffset;
        uint32_t mVertexSize;
        uint32_t mIndexSize;
    };
    Shape_base() {}
    void Add(const std::vector<Vertex3D> &inVertices,
             const std::vector<uint32_t> inIndices,
             uint32_t &outId);
    void Update(uint32_t &inId,
                const std::vector<Vertex3D> &inVertices,
                const std::vector<uint32_t> inIndices);
    void Resize(size_t inNewVertexSize, size_t inNewIndexSize);

    public:
    GETTER GetVertexCount(uint32_t inId) const { return mIds[inId].mVertexSize; }
    GETTER GetIndexCount(uint32_t inId) const { return mIds[inId].mIndexSize; }
    GETTER GetVertexOffsetAbsolute(uint32_t inId) const { return mIds[inId].mVertexOffset; }
    GETTER GetIndexOffsetAbsolute(uint32_t inId) const { return mIds[inId].mIndexOffset; }

    GETTER GetSerializablesRefs() {
        return std::make_tuple(
             &mIds, &mVertices, &mIndices, &mCurrentVertexPointer, &mCurrentIndexPointer);
    }

    protected:
    GETTER GetVertexBufferData() { return mVertices.data(); }
    GETTER GetIndexBufferData() { return mIndices.data(); }
    GETTER VertexCountToBytes(uint32_t inCount) const { return inCount * sizeof(Vertex3D); }
    GETTER IndexCountToBytes(uint32_t inCount) const { return inCount * sizeof(uint32_t); }
    GETTER GetCurrentIndexOffset() const { return mCurrentIndexPointer; }
    GETTER GetCurrentVertexOffset() const { return mCurrentVertexPointer; }

    private:
    std::vector<VertexIndexMemOffset> mIds;

    private:
    std::mutex mMutex;
    std::vector<Vertex3D> mVertices;
    std::vector<uint32_t> mIndices;
    uint32_t mCurrentVertexPointer = 0;
    uint32_t mCurrentIndexPointer  = 0;
};

} // namespace Jkr::Renderer::_3D
