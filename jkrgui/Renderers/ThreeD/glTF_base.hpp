#pragma once
#include <Global/Standards.hpp>
#include <algorithm>
#include <string>
#include <vector>
#include <iostream>

namespace Jkr::Renderer::_3D {
using namespace ksai::kstd;
class glTF;

class glTF_base {
private:
    struct VertexIndexMemOffset {
        uint32_t mVertexOffset;
        uint32_t mIndexOffset;
        size_t mVertexSize;
        size_t mIndexSize;
    };

public:
    glTF_base() { }
    void Add(const std::vector<Vertex3D>& inVertices,
        const std::vector<uint32_t> inIndices,
        uint32_t& outId)
    {
        mVertices.resize(mVertices.size() + inVertices.size());
        mIndices.resize(mIndices.size() + inIndices.size());
        std::copy(inVertices.begin(), inVertices.end(), mVertices.begin() + mCurrentVertexPointer);
        std::copy(inIndices.begin(), inIndices.end(), mIndices.begin() + mCurrentIndexPointer);
        VertexIndexMemOffset Offset = { .mVertexOffset = mCurrentVertexPointer,
            .mIndexOffset = mCurrentIndexPointer,
            .mVertexSize = inVertices.size(),
            .mIndexSize = inIndices.size() };
        outId = mIds.size();
        mIds.push_back(Offset);
        mCurrentVertexPointer += inVertices.size();
        mCurrentIndexPointer += inIndices.size();
    }

    void Update(uint32_t& inId,
        const std::vector<Vertex3D>& inVertices,
        const std::vector<uint32_t> inIndices)
    {
        auto MemOffset = mIds[inId];
        std::copy(inVertices.begin(), inVertices.end(), mVertices.begin() + MemOffset.mVertexOffset);
        std::copy(inIndices.begin(), inIndices.end(), mIndices.begin() + MemOffset.mIndexOffset);
    }

    void Resize(size_t inNewVertexSize, size_t inNewIndexSize)
    {
        mVertices.resize(inNewVertexSize);
        mIndices.resize(inNewIndexSize);
    }

protected:
    GETTER GetVertexCount(uint32_t inId) const { return mIds[inId].mVertexSize; }
    GETTER GetIndexCount(uint32_t inId) const { return mIds[inId].mIndexSize; }
    GETTER GetVertexBufferData() { return mVertices.data(); }
    GETTER GetIndexBufferData() { return mIndices.data(); }
    GETTER VertexCountToBytes(uint32_t inCount) const { return inCount * sizeof(Vertex3D); }
    GETTER IndexCountToBytes(uint32_t inCount) const { return inCount * sizeof(uint32_t); }
    GETTER GetVertexOffsetAbsolute(uint32_t inId) const { return mIds[inId].mVertexOffset; }
    GETTER GetIndexOffsetAbsolute(uint32_t inId) const { return mIds[inId].mIndexOffset; }
    GETTER GetCurrentIndexOffset() const { return mIds.empty() ? 0 : mIds.end()->mIndexOffset; }
    GETTER GetCurrentVertexOffset() const { return mIds.empty() ? 0 : mIds.end()->mVertexOffset; }

private:
    std::vector<VertexIndexMemOffset> mIds;

private:
    std::vector<Vertex3D> mVertices;
    std::vector<uint32_t> mIndices;
    uint32_t mCurrentVertexPointer = 0;
    uint32_t mCurrentIndexPointer = 0;

private:
    const std::string mGlslVertexShaderHeader = R"""(
#version 450
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec3 inColor;

)""";

    const std::string mGLslFragmentShaderHeader = R"""(
#version 450
)""";

public:
    GETTER& GetGlslVertexShaderHeaderString() const { return mGlslVertexShaderHeader; }
    GETTER& GetGlslFragmentShaderHeaderString() const { return mGLslFragmentShaderHeader; }
    GETTER& GetGlslComputeShaderHeaderString() const { return mGLslFragmentShaderHeader; }
};

} // namespace Jkr::Renderer::_3D
