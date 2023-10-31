#pragma once
#include "../Global/Standards.hpp"
#include "Window.hpp"
#include "Painter.hpp"
#include "PainterParameter.hpp"

namespace Jkr::Renderer
{
	class Line_base
	{
	protected:
		void AddLine(glm::vec2 inFirstPoint, glm::vec2 inSecondPoint, float inDepthValue);
		void UpdateLine(uint32_t inLineIndex, glm::vec2 inFirstPoint, glm::vec2 inSecondPoint, float inDepthValue);
	protected:
		constexpr size_t LineCountToVertexBytes(size_t inLineCount) { return 2 * sizeof(kstd::LineVertex) * inLineCount; }
		constexpr size_t LineCountToIndexBytes(size_t inLineCount) { return 2 * sizeof(uint32_t) * inLineCount; }
		GETTER GetCurrentLineOffset() const { return mLineCount - 1; }
		inline void* GetVertexBufferData() { return reinterpret_cast<void*>(mLineVertices.data()); }
		inline void* GetIndexBufferData() { return  reinterpret_cast<void*>(mLineIndices.data()); }
	private:
		std::vector<kstd::LineVertex> mLineVertices;
		std::vector<uint32_t> mLineIndices;
		uint32_t mCurrentLineVertexIndex = 0;
		uint32_t mLineCount = 0;
	};
}

