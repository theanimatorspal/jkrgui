#pragma once
#include "../Generator.hpp"
#include "Primitive.hpp"
#include <Global/Standards.hpp>
#include <Vulkan/Config.hpp>
#include <glm/glm.hpp>
#include <span>
#include <stdint.h>
#include <variant>
#include <vector>

namespace Jkr::Renderer {
using namespace Jkr;
using namespace ksai;
class Shape_base {
       protected:
               void Add(Jkr::Generator& inShape, int inX, int inY, uint32_t inZ, uint32_t& outId);
               void Update(Jkr::Generator& inShape, uint32_t inId, int inX, int inY, uint32_t inZ);
               GETTER GetVertexBufferData() { return reinterpret_cast<void*>(mVertices.data()); }
               GETTER GetIndexBufferData() { return reinterpret_cast<void*>(mIndices.data()); }
               GETTER VertexCountToBytes(uint32_t inVertexCount) { return sizeof(kstd::Vertex) * inVertexCount; }
               GETTER IndexCountToBytes(uint32_t inIndexCount) { return sizeof(uint32_t) * inIndexCount; }
               GETTER GetVertexOffsetAbsolute(uint32_t inId) const { return mOffsetVertexOfShape[inId]; }
               GETTER GetIndexOffsetAbsolute(uint32_t inId) const { return mOffsetIndexOfShape[inId]; }
               GETTER GetVertexCount(uint32_t inId) const { return mGenerators[inId].GetVertexCount(); }
               GETTER GetIndexCount(uint32_t inId) const { return mGenerators[inId].GetIndexCount(); }
               GETTER GetEndVertexOffsetAbsolute(uint32_t inId) const { return mEndOffsetVertexOfShape[inId]; }
               GETTER GetEndIndexOffsetAbsolute(uint32_t inId) const { return mEndOffsetIndexOfShape[inId]; }
               GETTER GetCurrentVertexOffset() const { return mOffsetVertex; }
               GETTER GetCurrentIndexOffset() const { return mOffsetIndices; }
               void Resize(uint32_t inNewSizeVertex, uint32_t inNewSizeIndex) {
                              mVertices.reserve(inNewSizeVertex);
                              mIndices.reserve(inNewSizeIndex);
               }

       private:
               uint32_t mNoOfShapes = 0;
               std::vector<kstd::Vertex> mVertices;
               std::vector<uint32_t> mIndices;

       private:
               uint32_t mOffsetVertex  = 0;
               uint32_t mOffsetIndices = 0;
               std::vector<Jkr::Generator> mGenerators;
               std::vector<uint32_t> mOffsetVertexOfShape;
               std::vector<uint32_t> mOffsetIndexOfShape;
               std::vector<uint32_t> mEndOffsetVertexOfShape;
               std::vector<uint32_t> mEndOffsetIndexOfShape;
};
} // namespace Jkr::Renderer
