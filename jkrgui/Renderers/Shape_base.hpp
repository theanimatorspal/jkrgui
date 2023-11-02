#pragma once
#include <glm/glm.hpp>
#include <stdint.h>
#include <vector>
#include <string>
#include <variant>
#include <Global/Standards.hpp>

namespace Jkr
{
	enum class Shapes { Circle, Rectangle };
	using namespace Jkr;
	using namespace ksai;
	class Generator
	{
	public:
		using Arguments = std::variant<
			glm::uvec2,
			std::vector<glm::uvec2>,
			uint32_t
		>;
		Generator(Shapes inShape, Arguments inArgs) : mArgs(inArgs), mShape(inShape)
		{ }
		void operator()(uint32_t inX,
			uint32_t inY,
			uint32_t inZ,
			uint32_t inStartVertexIndex,
			uint32_t inStartIndexIndex,
			std::vector<kstd::Vertex>& modVertices,
			std::vector<uint32_t>& modIndices,
			uint32_t& outVertexCount,
			uint32_t& outIndexCount
			);
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
		void Add(Jkr::Generator& inShape, uint32_t inX, uint32_t inY, uint32_t inZ, uint32_t& outId);
		void Update(Jkr::Generator& inShape, uint32_t inId, uint32_t inX, uint32_t inY, uint32_t inZ);
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

