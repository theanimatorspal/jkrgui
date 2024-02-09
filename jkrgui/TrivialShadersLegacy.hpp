#pragma once
#include <ksaiSandBox/glslShader.hpp>
#include <string_view>

namespace BestText_shader {

	const std::string_view VertexShader = R"""(
#version 450
#extension GL_EXT_debug_printf : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in ivec3 inTextureIndices;

layout(location = 0) out vec2 outTexCoord;
layout(location = 1) out ivec3 outTextureIndices;

layout(push_constant, std430) uniform pc
{
	mat4 Matrix;
	vec4 Window;
}
push;

void GlslMain()
{
	vec4 dx = vec4(inPosition.x, inPosition.y, inPosition.z, 1.0);
	gl_Position = push.Matrix * dx;
	outTexCoord = inTexCoord;
	outTextureIndices = inTextureIndices;
}

)""";

	const std::string_view FragmentShader = R"""(
#version 450
#extension GL_EXT_debug_printf : enable
#extension GL_EXT_nonuniform_qualifier : enable

layout(location = 0) out vec4 outColor;
layout(set = 0, binding = 0) uniform sampler2D u_image[];
layout(location = 0) in vec2 inTextCoord;
layout(location = 1) flat in ivec3 inTextureIndices;

layout(push_constant, std430) uniform pc
{
	mat4 Matrix;
	vec4 Color;
}
push;

const float smoothing = 1.0 / 32.0;

void GlslMain()
{
	vec4 color = texture(u_image[nonuniformEXT(inTextureIndices[0])], inTextCoord);
	float distance = color.a;
	float alpha = distance;
	outColor = vec4(push.Color.x, push.Color.y, push.Color.z, alpha * push.Color.w);
}
)""";

	const std::string_view ComputeShader = R"""(
#version 450
#extension GL_EXT_debug_printf : enable
#extension GL_EXT_nonuniform_qualifier : enable

layout(set = 0, binding = 0) uniform sampler2D storageImage[];
layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

layout(push_constant, std430) uniform pc
{
	mat4 Matrix;
	vec4 Window;
}
push;

void GlslMain() {}

)""";
}

#ifndef JKR_USE_VARIABLE_DES_INDEXING

namespace ShapeRenderers_Image {
	const std::string_view VertexShader = R"""(
#version 450
#extension GL_EXT_debug_printf : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;
layout(location = 0) out vec2 outTexCoord;

layout(push_constant, std430) uniform pc {
	mat4 Matrix;
	vec4 Color;
	vec4 mParams;
} push;


void GlslMain() {
	vec4 dx = vec4(inPosition.x, inPosition.y, inPosition.z, 1.0);
	gl_Position = push.Matrix * dx;
	outTexCoord = inTexCoord;
}
)""";

	const std::string_view FragmentShader = R"""(
#version 450
#extension GL_EXT_debug_printf : enable

layout(location = 0) out vec4 outColor;
layout(set = 0, binding = 0) uniform sampler2D image;
layout(location = 0) in vec2 inTexCoord;

layout(push_constant, std430) uniform pc {
	mat4 Matrix;
	vec4 Color;
	vec4 mParams;
} push;

void GlslMain()
{
	vec4 color = texture(image, inTexCoord);
	outColor = vec4(color.r * push.Color.r, color.g * push.Color.g, color.b * push.Color.b, color.a * push.Color.a);
}

)""";

	const std::string_view ComputeShader = R"""(
#version 450
#extension GL_EXT_debug_printf : enable

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

layout(push_constant, std430) uniform pc {
	mat4 Matrix;
	vec4 Color;
	vec4 mParams;
} push;

layout(set = 0, binding = 0) uniform sampler2D u_atlas8;

void GlslMain()
{
	uint gID = gl_GlobalInvocationID.x;
}

)""";
}

#else

namespace ShapeRenderers_Image
{
	const std::string_view VertexShader = R"""(
#version 450
#extension GL_EXT_debug_printf : enable
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;

layout(location = 0) out vec2 outTexCoord;

layout(push_constant, std430) uniform pc {
	mat4 Matrix;
	vec4 Window;
	vec4 mParams;
} push;


void GlslMain() {
	vec4 dx = vec4(inPosition.x, inPosition.y, inPosition.z, 1.0);
	gl_Position = push.Matrix * dx;
	outTexCoord = inTexCoord;
}
)""";

	const std::string_view FragmentShader = R"""(
#version 450
#extension GL_EXT_debug_printf : enable
#extension GL_EXT_nonuniform_qualifier : enable

layout(location = 0) out vec4 outColor;
layout(set = 0, binding = 0) uniform sampler2D u_image[];
layout(location = 0) in vec2 inTextCoord;

layout(push_constant, std430) uniform pc {
	mat4 Matrix;
	vec4 Color;
	vec4 mParams;
} push;

void GlslMain()
{
	vec4 color = texture(u_image[int(push.mParams.x)], inTextCoord);
	outColor = vec4(color.r * push.Color.r, color.g * push.Color.g, color.b * push.Color.b, color.a * push.Color.a);
}
)""";

	const std::string_view ComputeShader = R"""(
#version 450

#extension GL_EXT_debug_printf : enable
#extension GL_EXT_nonuniform_qualifier : enable

layout(set = 0, binding = 0) uniform sampler2D storageImage[];

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

layout(push_constant, std430) uniform pc {
	mat4 Matrix;
	vec4 Window;
	vec4 mParams;
} push;

void GlslMain()
{
}
)""";
}
#endif


namespace ShapeRenderers_Fill
{
	const std::string_view VertexShader = R"""(
#version 450
#extension GL_EXT_debug_printf : enable
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;
layout(location = 0) out vec2 outTexCoord;

layout(push_constant, std430) uniform pc {
	mat4 Matrix;
	vec4 Color;
	vec4 mParams;
} push;


void GlslMain() {
	vec4 dx = vec4(inPosition.x, inPosition.y, inPosition.z, 1.0);
	gl_Position = push.Matrix * dx;
}
)""";

	const std::string_view FragmentShader = R"""(
#version 450
#extension GL_EXT_debug_printf : enable
layout(location = 0) out vec4 outColor;
layout(location = 0) in vec2 inTexCoord;

layout(push_constant, std430) uniform pc {
	mat4 Matrix;
	vec4 Color;
	vec4 mParams;
} push;

void GlslMain()
{
	outColor = push.Color;
}

)""";

	const std::string_view ComputeShader = R"""(
#version 450
#extension GL_EXT_debug_printf : enable

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

layout(push_constant, std430) uniform pc {
	mat4 Matrix;
	vec4 Color;
	vec4 mParams;
} push;

void GlslMain()
{
}
)""";
}

namespace LineRendererShaders
{
	const std::string_view VertexShader = R"""(
#version 450
#extension GL_EXT_debug_printf : enable

layout(location = 0) in vec3 inPosition;

layout(push_constant, std430) uniform pc {
	mat4 Matrix;
	vec4 Color;
} push;


void GlslMain() {
	vec4 dx = vec4(inPosition.x, inPosition.y, inPosition.z, 1.0);
	gl_Position = push.Matrix * dx;
}

)""";

	const std::string_view FragmentShader = R"""(
#version 450
#extension GL_EXT_debug_printf : enable
layout(location = 0) out vec4 outColor;

layout(push_constant, std430) uniform pc {
	mat4 Matrix;
	vec4 Color;
} push;

void GlslMain()
{
	outColor = push.Color;
}

)""";


	const std::string_view ComputeShader = R"""(
#version 450
#extension GL_EXT_debug_printf : enable
layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

layout(push_constant, std430) uniform pc {
	mat4 Matrix;
	vec4 Color;
} push;

layout(set = 0, binding = 0) uniform sampler2D u_atlas8;

void GlslMain()
{
	uint gID = gl_GlobalInvocationID.x;
}

)""";

}


namespace FontShader {
	const std::string_view VertexShader = R"""(
#version 450
#extension GL_EXT_debug_printf : enable
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;

layout(location = 0) out vec3 outColor;
layout(location = 1) out vec2 outTexCoord;

layout(push_constant, std430) uniform pc {
	mat4 Matrix;
	vec4 Color;
} push;

void GlslMain() {
	vec4 dx = vec4(inPosition.x, inPosition.y, inPosition.z, 1.0);
	gl_Position = push.Matrix * dx;
	outTexCoord = inTexCoord;
}
)""";

	const std::string_view FragmentShader = R"""(
#version 450
#extension GL_EXT_debug_printf : enable

layout(location = 0) in vec3 inColor;
layout(location = 1) in vec2 inTextCoord;
layout(location = 0) out vec4 outColor;

const float smoothing = 1.0 / 2.0;

layout(push_constant, std430) uniform pc {
	mat4 mvp;
	vec4 color;
} push;

layout(set = 0, binding = 0) uniform sampler2D u_atlas;

void GlslMain()
{
	vec4 color = texture(u_atlas, inTextCoord);
	float distance = color.a;
	float alpha = smoothstep(0.8 - smoothing, 0.8 + smoothing, distance * 2);
	outColor = vec4(push.color.x, push.color.y, push.color.z, alpha);
}

)""";

	const std::string_view ComputeShader = R"""(
#version 450
#extension GL_EXT_debug_printf : enable

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

layout(push_constant, std430) uniform pc {
	vec3 vt;
} push;

layout(set = 0, binding = 0) uniform sampler2D u_atlas8;

void GlslMain()
{
	uint gID = gl_GlobalInvocationID.x;
}

)""";
}

