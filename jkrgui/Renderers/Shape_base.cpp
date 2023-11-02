#include "Shape_base.hpp"

using namespace Jkr;
using namespace Jkr::Renderer;


void Jkr::Generator::operator()(uint32_t inX, uint32_t inY, uint32_t inZ, uint32_t inStartVertexIndex, uint32_t inStartIndexIndex, std::vector<kstd::Vertex>& modVertices, std::vector<uint32_t>& modIndices, uint32_t& outVertexCount, uint32_t& outIndexCount)
{
	if (mShape == Shapes::Rectangle)
	{
		auto x = inX;
		auto dx = std::get<glm::uvec2>(mArgs).x;
		auto y = inY;
		auto dy = std::get<glm::uvec2>(mArgs).y;
		modVertices.resize(4 + modVertices.size());
		modIndices.resize(6 + modIndices.size());
		const auto v_index = inStartVertexIndex;
		const auto i_index = inStartIndexIndex;
		modVertices[v_index + 0] = kstd::Vertex{
			.mPosition = {x, y + dy, inZ},
			.mTextureCoordinates = {0, 1}
		};

		modVertices[v_index + 1] = kstd::Vertex{
			.mPosition = {x, y, inZ},
			.mTextureCoordinates = {0, 0}
		};

		modVertices[v_index + 2] = kstd::Vertex{
			.mPosition = {x + dx, y, inZ},
			.mTextureCoordinates = {1, 0}
		};

		modVertices[v_index + 3] = kstd::Vertex{
			.mPosition = {x + dx, y + dy, inZ},
			.mTextureCoordinates = {1, 1}
		};
		modIndices[i_index + 0] = v_index + 0;
		modIndices[i_index + 1] = v_index + 1;
		modIndices[i_index + 2] = v_index + 2;
		modIndices[i_index + 3] = v_index + 0;
		modIndices[i_index + 4] = v_index + 2;
		modIndices[i_index + 5] = v_index + 3;
		outVertexCount = 4;
		outIndexCount = 6;
		mVertexCount = 4;
		mIndexCount = 6;
	}
}

void Jkr::Renderer::Shape_base::Add(Jkr::Generator &inShape, uint32_t inX, uint32_t inY, uint32_t inZ, uint32_t& outId)
{
	uint32_t vcount = 0;
	uint32_t icount = 0;
	inShape(inX, inY, inZ, mOffsetVertex, mOffsetIndices, mVertices, mIndices, vcount, icount);
	mOffsetVertexOfShape.push_back(mOffsetVertex);
	mOffsetIndexOfShape.push_back(mOffsetIndices);
	mGenerators.push_back(inShape);
	mOffsetVertex += vcount;
	mOffsetIndices += icount;
	mEndOffsetVertexOfShape.push_back(mOffsetVertex);
	mEndOffsetIndexOfShape.push_back(mOffsetIndices);
	outId = mNoOfShapes;
	mNoOfShapes++;
}

/*
	WARNING: The Shape Generator Here SHOULD NOT modify the number of Vertices / Indices
*/
void Jkr::Renderer::Shape_base::Update(Jkr::Generator& inShape, uint32_t inId, uint32_t inX, uint32_t inY, uint32_t inZ)
{
	uint32_t vcount = 0;
	uint32_t icount = 0;
	inShape(inX, inY, inZ, mOffsetVertexOfShape[inId], mOffsetIndexOfShape[inId], mVertices, mIndices, vcount, icount);
}
