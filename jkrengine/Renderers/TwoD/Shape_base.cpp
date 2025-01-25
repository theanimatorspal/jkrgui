#include "Shape_base.hpp"
#include "Global/Standards.hpp"

using namespace Jkr;
using namespace Jkr::Renderer;

void Jkr::Renderer::Shape_base::Add(
     Jkr::Generator &inShape, int inX, int inY, uint32_t inZ, uint32_t &outId) {
    inShape(inX, inY, inZ, mOffsetVertex, mOffsetIndices, mVertices, mIndices);
    mOffsetVertexOfShape.push_back(mOffsetVertex);
    mOffsetIndexOfShape.push_back(mOffsetIndices);
    mGenerators.push_back(inShape);
    mOffsetVertex += inShape.GetVertexCount();
    mOffsetIndices += inShape.GetIndexCount();
    mEndOffsetVertexOfShape.push_back(mOffsetVertex);
    mEndOffsetIndexOfShape.push_back(mOffsetIndices);
    outId = mNoOfShapes;
    mNoOfShapes++;
}

/*
        WARNING: The Shape Generator Here SHOULD NOT modify the number of Vertices / Indices
*/
void Jkr::Renderer::Shape_base::Update(
     Jkr::Generator &inShape, uint32_t inId, int inX, int inY, int inZ) {
    uint32_t vcount = 0;
    uint32_t icount = 0;
    inShape(
         inX, inY, inZ, mOffsetVertexOfShape[inId], mOffsetIndexOfShape[inId], mVertices, mIndices);
}
