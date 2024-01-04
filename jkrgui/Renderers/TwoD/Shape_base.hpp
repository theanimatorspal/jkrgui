#pragma once
#include <glm/glm.hpp>
#include <stdint.h>
#include <vector>
#include <variant>
#include <Global/Standards.hpp>
#include <span>
#include <Vulkan/Config.hpp>

namespace Jkr
{
enum class Shapes { Circle, Rectangle, Bezier2_8 };
class Generator
{
public:
    using Arguments = std::variant<glm::uvec2, std::span<glm::uvec2>, std::span<glm::vec2>, uint32_t>;
    Generator() = default;
    Generator(Shapes inShape, Arguments inArgs);
    void operator()(int inX,
                    int inY,
                    uint32_t inZ,
                    uint32_t inStartVertexIndex,
                    uint32_t inStartIndexIndex,
                    ksai::v<ksai::kstd::Vertex> &modVertices,
                    ksai::v<uint32_t> &modIndices);
    Shapes GetShape() const { return mShape; }
    uint32_t GetVertexCount() const { return mVertexCount; }
    uint32_t GetIndexCount() const { return mIndexCount; }

private:
    Arguments mArgs;
    Shapes mShape;
    uint32_t mVertexCount = 0;
    uint32_t mIndexCount = 0;
};
}



namespace Jkr::Renderer
{
	enum class Shapes { Circle, Rectangle };
	using namespace Jkr;
	using namespace ksai;
	class Shape_base
	{
	protected:
		void Add(Jkr::Generator& inShape, int inX, int inY, uint32_t inZ, uint32_t& outId);
		void Update(Jkr::Generator& inShape, uint32_t inId, int inX, int inY, uint32_t inZ);
		GETTER GetVertexBufferData() { return reinterpret_cast<void*>(mVertices.data()); }
		GETTER GetIndexBufferData() { return  reinterpret_cast<void*>(mIndices.data()); }
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
		void Resize(uint32_t inNewSizeVertex, uint32_t inNewSizeIndex)
		{
			mVertices.reserve(inNewSizeVertex);
			mIndices.reserve(inNewSizeIndex);
		}
	private:
		uint32_t mNoOfShapes = 0;
		std::vector<kstd::Vertex> mVertices;
		std::vector<uint32_t> mIndices;
	private:
		uint32_t mOffsetVertex = 0;
		uint32_t mOffsetIndices = 0;
		std::vector<Jkr::Generator> mGenerators;
		std::vector<uint32_t> mOffsetVertexOfShape;
		std::vector<uint32_t> mOffsetIndexOfShape;
		std::vector<uint32_t> mEndOffsetVertexOfShape;
		std::vector<uint32_t> mEndOffsetIndexOfShape;
	};
}

