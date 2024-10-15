#include "Shape3D_base.hpp"
using namespace Jkr;
void Renderer::_3D::Shape_base::Add(const std::vector<Vertex3D> &inVertices,
                                    const std::vector<uint32_t> inIndices,
                                    uint32_t &outId) {
    std::scoped_lock<std::mutex> Lock(mMutex);
    std::copy(inVertices.begin(), inVertices.end(), mVertices.begin() + mCurrentVertexPointer);
    std::copy(inIndices.begin(), inIndices.end(), mIndices.begin() + mCurrentIndexPointer);
    uint32_t vsize              = inVertices.size();
    uint32_t isize              = inIndices.size();
    VertexIndexMemOffset Offset = {.mVertexOffset = mCurrentVertexPointer,
                                   .mIndexOffset  = mCurrentIndexPointer,
                                   .mVertexSize   = vsize,
                                   .mIndexSize    = isize};
    outId                       = mIds.size();
    mIds.push_back(Offset);
    mCurrentVertexPointer += inVertices.size();
    mCurrentIndexPointer += inIndices.size();
}

void Renderer::_3D::Shape_base::Update(uint32_t &inId,
                                       const std::vector<Vertex3D> &inVertices,
                                       const std::vector<uint32_t> inIndices) {
    std::scoped_lock<std::mutex> Lock(mMutex);
    auto MemOffset = mIds[inId];
    std::copy(inVertices.begin(), inVertices.end(), mVertices.begin() + MemOffset.mVertexOffset);
    std::copy(inIndices.begin(), inIndices.end(), mIndices.begin() + MemOffset.mIndexOffset);
}

void Renderer::_3D::Shape_base::Resize(size_t inNewVertexSize, size_t inNewIndexSize) {
    std::scoped_lock<std::mutex> Lock(mMutex);
    mVertices.resize(inNewVertexSize);
    mIndices.resize(inNewIndexSize);
}