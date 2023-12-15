#pragma once
#include <Global/Standards.hpp>
#include <algorithm>
#include <vector>

namespace Jkr::Renderer::_3D {
using namespace ksai::kstd;
class glTF;
class glTF_base
{
private:
    struct VertexIndexMemOffset
    {
        uint32_t mVertexOffset;
        uint32_t mIndexOffset;
        size_t mVertexSize;
        size_t mIndexSize;
    };
public:
    glTF_base() {}
    void Add(const std::vector<Vertex3D> &inVertices,
             const std::vector<uint32_t> inIndices,
             uint32_t &outId)
    {
        mVertices.reserve(mVertices.size() + inVertices.size());
        mIndices.reserve(mIndices.size() + inIndices.size());
        std::copy(inVertices.begin(), inVertices.end(), mVertices.begin() + mCurrentVertexPointer);
        std::copy(inIndices.begin(), inIndices.end(), mIndices.begin() + mCurrentIndexPointer);
        VertexIndexMemOffset Offset = {.mVertexOffset = mCurrentVertexPointer,
                                       .mIndexOffset = mCurrentIndexPointer,
                                       .mVertexSize = inVertices.size(),
                                       .mIndexSize = inIndices.size()};
        outId = mIds.size();
        mIds.push_back(Offset);
    }

    void Update(uint32_t &inId,
                const std::vector<Vertex3D> &inVertices,
                const std::vector<uint32_t> inIndices)
    {
        auto MemOffset = mIds[inId];
        std::copy(inVertices.begin(), inVertices.end(), mVertices.begin() + MemOffset.mVertexOffset);
        std::copy(inIndices.begin(), inIndices.end(), mIndices.begin() + MemOffset.mIndexOffset);
    }

protected:
    GETTER GetVertexCount(uint32_t inId) { return mIds[inId].mVertexSize; }
    GETTER GetIndexCount(uint32_t inId) { return mIds[inId].mIndexSize; }
    GETTER GetVertexBufferData() { return mVertices.data(); }
    GETTER GetIndexBufferData() { return mIndices.data(); }
    GETTER VertexCountToBytes(uint32_t inCount) { return inCount * sizeof(Vertex3D); }
    GETTER IndexCountToBytes(uint32_t inCount) { return inCount * sizeof(uint32_t); }
    GETTER GetVertexOffsetAbsolute(uint32_t inId) { return mIds[inId].mVertexOffset; }
    GETTER GetIndexOffsetAbsolute(uint32_t inId) { return mIds[inId].mIndexOffset; }

private:
    std::vector<VertexIndexMemOffset> mIds;

private:
    std::vector<Vertex3D> mVertices;
    std::vector<uint32_t> mIndices;
    uint32_t mCurrentVertexPointer = 0;
    uint32_t mCurrentIndexPointer = 0;
};

} // namespace Jkr::Renderer::_3D
