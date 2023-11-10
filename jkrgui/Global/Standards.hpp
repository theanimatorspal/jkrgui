#pragma once
#include <glm/glm.hpp>
#include "Config.hpp"
#define GETTER inline auto
/*
CONFIGURATION
*/

namespace ksai::kstd
{
	struct Vertex
	{
		glm::vec3 mPosition = { 0, 0, 0 };
		glm::vec2 mTextureCoordinates = { 0, 0 };
	};

	struct VertexEXT
	{
		glm::vec3 mPosition = { 0, 0, 0 };
		glm::vec2 mTextureCoordinates = { 0, 0 };
		glm::ivec3 mIvec3 = {0, 0, 0};
	};

	struct LineVertex
	{
		glm::vec3 mPosition = { 0, 0, 0 };
	};

	using ui32 = uint32_t;
	using f32 = float_t;
}