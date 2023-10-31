#include "Line.hpp"

void Jkr::Renderer::Line_base::AddLine(glm::vec2 inFirstPoint, glm::vec2 inSecondPoint, float inDepthValue)
{
    mLineVertices.push_back(kstd::LineVertex{ .mPosition = glm::vec3(inFirstPoint, inDepthValue) });
    mLineIndices.push_back(mCurrentLineVertexIndex++);
    mLineVertices.push_back(kstd::LineVertex{ .mPosition = glm::vec3(inSecondPoint, inDepthValue) });
    mLineIndices.push_back(mCurrentLineVertexIndex++);
	 mLineCount++;
}

void Jkr::Renderer::Line_base::UpdateLine(uint32_t inLineIndex, glm::vec2 inFirstPoint, glm::vec2 inSecondPoint, float inDepthValue)
{
	mLineVertices[inLineIndex * 2].mPosition = glm::vec3(inFirstPoint, inDepthValue);
	mLineVertices[inLineIndex * 2 + 1].mPosition = glm::vec3(inSecondPoint, inDepthValue);
}
