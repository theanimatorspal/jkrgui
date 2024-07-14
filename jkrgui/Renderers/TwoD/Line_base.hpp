#pragma once
#include <Global/Standards.hpp>
#include "Window_base.hpp"
#include "Painter.hpp"
#include "PainterParameter.hpp"

namespace Jkr::Renderer
{
	class Line_base
	{
	public:
		using LineFeatures = void;
		GETTER GetCurrentLineOffset() const { return mLineCount - 1; }
		GETTER GetCurrentNewLineId() const { return mLineCount; }
	protected:
		void AddLine(glm::vec2 inFirstPoint, glm::vec2 inSecondPoint, float inDepthValue);
		void UpdateLine(uint32_t inLineIndex, glm::vec2 inFirstPoint, glm::vec2 inSecondPoint, float inDepthValue);
	protected:
		GETTER LineCountToVertexBytes(size_t inLineCount) { return 2 * sizeof(kstd::LineVertex) * inLineCount; }
		GETTER LineCountToIndexBytes(size_t inLineCount) { return 2 * sizeof(uint32_t) * inLineCount; }
		GETTER GetCurrentLineOffsetAbsolute() const { return mLineCount; }
		GETTER GetVertexBufferData() { return reinterpret_cast<void*>(mLineVertices.data()); }
		GETTER GetIndexBufferData() { return  reinterpret_cast<void*>(mLineIndices.data()); }
		void Resize(uint32_t inNewSize) {
			mLineVertices.reserve(LineCountToVertexBytes(inNewSize)); 
			mLineIndices.reserve(LineCountToIndexBytes(inNewSize));
		}
	private:
		std::vector<kstd::LineVertex> mLineVertices;
		std::vector<uint32_t> mLineIndices;
		uint32_t mCurrentLineVertexIndex = 0;
		uint32_t mLineCount = 0;
	};
}

