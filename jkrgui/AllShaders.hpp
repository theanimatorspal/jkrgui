#pragma once
#include <glm/glm.hpp>
#include <Global/Config.hpp>
#include <ksaiSandBox/glslShader.hpp>
#include <ksaiSandBox/DefGlsl.hpp>	
#pragma warning(disable : 4101 )

inline glm::vec4 gl_Position;
inline glm::vec4 gl_GlobalInvocationID;
inline glm::vec4 gl_FragCoord;

const std::string ShaderGlobals = R"""(

float sdf_circle(vec2 xy, vec2 center, float radius)
{
	return distance(xy, center) - radius;
}

float sdf_circle_border(float center, float half_width, float distance)
{
	return abs(distance - center) - half_width;
}

float sdf_rectangle(vec2 xy, vec2 hw, vec2 center, float radius)
{
	vec2 Q = abs(xy - center) - hw;
	return distance(max(Q, vec2(0.0)), vec2(0.0)) + min(max(Q.x, Q.y), 0.0) - radius;
}

float sdf_line(in vec2 p, in vec2 a, in vec2 b) {
	vec2 ba = b - a;
	vec2 pa = p - a;
	float h = clamp(dot(pa, ba) / dot(ba, ba), 0., 1.);
	return length(pa - h * ba);
}


)""";

#define sdf_circle(vec2_xy, vec2_center, float_radius) 0.0
#define sdf_circle_border(float_center, float_half_width, float_distance) 0.0
#define sdf_rectangle(vec2_xy, vec2_hw, vec2_center, float_radius) 0.0
#define sdf_line(vec2_xy, vec2_a, vec2_b)

#ifdef JKR_USE_VARIABLE_DES_INDEXING
namespace BestText
{
	using namespace glm;
	const auto VertexShader = [](ksai::Shader& inShader) {
		GlslShaderStart(vertex);

		GlslVersion(450);
		GlslShader(vertex);
		GlslExtension(GL_EXT_debug_printf);

		GlslCodeStart();

		layout(location = 0) in vec3 inPosition;
		layout(location = 1) in vec2 inTexCoord;
		layout(location = 2) in ivec3 inTextureIndices;

		layout(location = 0) out vec2 outTexCoord;
		layout(location = 1) out ivec3 outTextureIndices;

		layout(push_constant, std430) uniform pc {
			mat4 Matrix;
			vec4 Window;
		} push;


		void GlslMain() {
			vec4 dx = vec4(inPosition.x, inPosition.y, inPosition.z, 1.0);
			gl_Position = push.Matrix * dx;
			outTexCoord = inTexCoord;
			outTextureIndices = inTextureIndices;
		}

		GlslCodeFinish();

		};

	const auto FragmentShader = [](ksai::Shader& inShader)
		{
			GlslShaderStart(fragment);

			GlslVersion(450);
			GlslExtension(GL_EXT_debug_printf);
			GlslExtension(GL_EXT_nonuniform_qualifier);

			GlslBindOut(vec4, 0, outColor);

			ShaderRawAdd("layout(set = 0, binding = 0) uniform sampler2D u_image[];");

			GlslCodeStart();
			layout(location = 0) in vec2 inTextCoord;
			layout(location = 1) flat in ivec3 inTextureIndices;

			layout(push_constant, std430) uniform pc {
				mat4 Matrix;
				vec4 Color;
			} push;

			const float smoothing = 1.0 / 32.0;

			void GlslMain()
			{
				vec4 color = texture(u_image[nonuniformEXT(inTextureIndices[0])], inTextCoord);
				float distance = color.a;
				float alpha = distance;
				outColor = vec4(push.Color.x, push.Color.y, push.Color.z, alpha * push.Color.w);
			}

			GlslCodeFinish();
		};

	/* Not Used */
	const auto ComputeShader = [](ksai::Shader& inShader)
		{
			GlslShaderStart(compute);

			GlslVersion(450);

			GlslExtension(GL_EXT_debug_printf);
			GlslExtension(GL_EXT_nonuniform_qualifier);

			ShaderRawAdd(ShaderGlobals);
			ShaderRawAdd("layout(set = 0, binding = 0) uniform sampler2D storageImage[];");

			GlslCodeStart();

			layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

			layout(push_constant, std430) uniform pc {
				mat4 Matrix;
				vec4 Window;
			} push;

			void GlslMain()
			{
			}

			GlslCodeFinish();

		};

}

#endif


#ifndef JKR_USE_VARIABLE_DES_INDEXING
namespace ShapeRenderers_Image
{
	using namespace glm;
	const auto VertexShader = [](ksai::Shader& inShader) {
		GlslShaderStart(vertex);

		GlslVersion(450);
		GlslShader(vertex);
		GlslExtension(GL_EXT_debug_printf);

		GlslCodeStart();

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

		GlslCodeFinish();

		};

	const auto FragmentShader = [](ksai::Shader& inShader)
		{
			GlslShaderStart(fragment);

			GlslVersion(450);
			GlslExtension(GL_EXT_debug_printf);

			GlslBindOut(vec4, 0, outColor);

			GlslCodeStart();
			layout(set = 0, binding = 0) uniform sampler2D image;
			layout(location = 0) in vec2 inTexCoord;

			layout(push_constant, std430) uniform pc {
				mat4 Matrix;
				vec4 Color;
				vec4 mParams;
			} push;

			void GlslMain()
			{
				outColor = texture(image, inTexCoord);
			}

			GlslCodeFinish();
		};

	const auto ComputeShader = [](ksai::Shader& inShader)
		{
			GlslShaderStart(compute);

			GlslVersion(450);

			GlslExtension(GL_EXT_debug_printf);

			GlslCodeStart();

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

			GlslCodeFinish();

		};
}
#else
namespace ShapeRenderers_Image
{
	using namespace glm;
	const auto VertexShader = [](ksai::Shader& inShader) {
		GlslShaderStart(vertex);

		GlslVersion(450);
		GlslShader(vertex);
		GlslExtension(GL_EXT_debug_printf);

		GlslCodeStart();

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

		GlslCodeFinish();

		};

	const auto FragmentShader = [](ksai::Shader& inShader)
		{
			GlslShaderStart(fragment);

			GlslVersion(450);
			GlslExtension(GL_EXT_debug_printf);
			GlslExtension(GL_EXT_nonuniform_qualifier);

			GlslBindOut(vec4, 0, outColor);

			ShaderRawAdd("layout(set = 0, binding = 0) uniform sampler2D u_image[];");

			GlslCodeStart();
			layout(location = 0) in vec2 inTextCoord;

			layout(push_constant, std430) uniform pc {
				mat4 Matrix;
				vec4 Color;
				vec4 mParams;
			} push;

			void GlslMain()
			{
				outColor = texture(u_image[int(push.mParams.x)], inTextCoord);
			}

			GlslCodeFinish();
		};

	/* Not Used */
	const auto ComputeShader = [](ksai::Shader& inShader)
		{
			GlslShaderStart(compute);

			GlslVersion(450);

			GlslExtension(GL_EXT_debug_printf);
			GlslExtension(GL_EXT_nonuniform_qualifier);

			ShaderRawAdd(ShaderGlobals);
			ShaderRawAdd("layout(set = 0, binding = 0) uniform sampler2D storageImage[];");

			GlslCodeStart();

			layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

			layout(push_constant, std430) uniform pc {
				mat4 Matrix;
				vec4 Window;
				vec4 mParams;
			} push;

			void GlslMain()
			{
			}

			GlslCodeFinish();

		};

}

#endif



namespace ShapeRenderers_Fill
{
	using namespace glm;
	const auto VertexShader = [](ksai::Shader& inShader) {
		GlslShaderStart(vertex);

		GlslVersion(450);
		GlslShader(vertex);
		GlslExtension(GL_EXT_debug_printf);

		GlslCodeStart();

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

		GlslCodeFinish();

		};

	const auto FragmentShader = [](ksai::Shader& inShader)
		{
			GlslShaderStart(fragment);

			GlslVersion(450);
			GlslExtension(GL_EXT_debug_printf);

			GlslBindOut(vec4, 0, outColor);
			layout(location = 0) out vec2 inTexCoord;

			GlslCodeStart();

			layout(push_constant, std430) uniform pc {
				mat4 Matrix;
				vec4 Color;
				vec4 mParams;
			} push;

			void GlslMain()
			{
				outColor = push.Color;
			}

			GlslCodeFinish();
		};

	const auto ComputeShader = [](ksai::Shader& inShader)
		{
			GlslShaderStart(compute);

			GlslVersion(450);

			GlslExtension(GL_EXT_debug_printf);

			GlslCodeStart();

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

			GlslCodeFinish();

		};
}







namespace LineRendererShaders
{
	using namespace glm;
	const auto VertexShader = [](ksai::Shader& inShader) {
		GlslShaderStart(vertex);

		GlslVersion(450);
		GlslShader(vertex);
		GlslExtension(GL_EXT_debug_printf);

		GlslCodeStart();

		layout(location = 0) in vec3 inPosition;

		layout(push_constant, std430) uniform pc {
			mat4 Matrix;
			vec4 Color;
		} push;


		void GlslMain() {
			vec4 dx = vec4(inPosition.x, inPosition.y, inPosition.z, 1.0);
			gl_Position = push.Matrix * dx;
		}

		GlslCodeFinish();

		};

	const auto FragmentShader = [](ksai::Shader& inShader)
		{
			GlslShaderStart(fragment);

			GlslVersion(450);
			GlslExtension(GL_EXT_debug_printf);

			GlslBindOut(vec4, 0, outColor);

			GlslCodeStart();

			layout(push_constant, std430) uniform pc {
				mat4 Matrix;
				vec4 Color;
			} push;

			void GlslMain()
			{
				outColor = push.Color;
			}

			GlslCodeFinish();
		};

	const auto ComputeShader = [](ksai::Shader& inShader)
		{
			GlslShaderStart(compute);

			GlslVersion(450);

			GlslExtension(GL_EXT_debug_printf);

			GlslCodeStart();

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

			GlslCodeFinish();

		};
}









namespace GUI2DShaders
{
	using namespace glm;
	const auto VertexShader = [](ksai::Shader& inShader) {
		GlslShaderStart(vertex);

		GlslVersion(450);
		GlslShader(vertex);
		GlslExtension(GL_EXT_debug_printf);

		ShaderRawAdd("layout(set = 0, binding = 0, rgba8) uniform readonly image2D u_atlas[];");
		GlslCodeStart();

		layout(location = 0) in vec3 inPosition;
		layout(location = 1) in vec2 inTexCoord;
		layout(location = 2) in ivec3 inIvec3;

		layout(location = 0) out vec3 outColor;
		layout(location = 1) out vec2 outTexCoord;
		layout(location = 2) out ivec3 outTextureIndex;

		layout(push_constant, std430) uniform pc {
			mat4 Matrix;
			vec2 Window;
			vec4 Color;
		} push;


		void GlslMain() {
			vec4 dx = vec4(inPosition.x, inPosition.y, inPosition.z, 1.0);
			gl_Position = push.Matrix * dx;
			outTexCoord = inTexCoord;
			outTextureIndex = inIvec3;
		}

		GlslCodeFinish();

		};

	const auto FragmentShader = [](ksai::Shader& inShader)
		{
			GlslShaderStart(fragment);

			GlslVersion(450);
			GlslExtension(GL_EXT_debug_printf);
			GlslExtension(GL_EXT_nonuniform_qualifier);

			GlslBindOut(vec4, 0, outColor);

			ShaderRawAdd("layout(set = 0, binding = 0, rgba8) uniform readonly image2D u_atlas[];");

			GlslCodeStart();
			layout(location = 0) in vec3 inColor;
			layout(location = 1) in vec2 inTextCoord;
			layout(location = 2) flat in ivec3 inTextureIndex;

			layout(push_constant, std430) uniform pc {
				mat4 Matrix;
				vec4 Color;
				int inIndex;
			} push;

			void GlslMain()
			{
				int index = push.inIndex;
				ivec2 imageSize = imageSize(u_atlas[index]);
				ivec2 imageCoords = ivec2(inTextCoord.x * imageSize.x, inTextCoord.y * imageSize.y);
				vec4 ColorValue = imageLoad(u_atlas[index], imageCoords);

				outColor = vec4(ColorValue.x * push.Color.x, ColorValue.y * push.Color.y, ColorValue.z * push.Color.z, ColorValue.x + ColorValue.y + ColorValue.z);
				//			debugPrintfEXT("Index Texture : %d, size: %d, %d", index, imageSize.x, imageSize.y);
			}

			GlslCodeFinish();
		};

	const auto ComputeShader = [](ksai::Shader& inShader)
		{
			GlslShaderStart(compute);

			GlslVersion(450);

			GlslExtension(GL_EXT_debug_printf);
			GlslExtension(GL_EXT_nonuniform_qualifier);

			ShaderRawAdd(ShaderGlobals);
			ShaderRawAdd("layout(set = 0, binding = 0, rgba8) uniform image2D storageImage[];");

			GlslCodeStart();

			layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

			layout(push_constant, std430) uniform pc {
				vec4 mPosDimen;
				vec4 mColor;
				vec4 mParam;
				int mTextureCoordinateIndex;
				int mShapeIndex;
			} push;

			void GlslMain()
			{
				uint idx = push.mTextureCoordinateIndex;
				uvec3 gID = gl_GlobalInvocationID;
				ivec2 image_size = ivec2(imageSize(storageImage[idx]));
				ivec2 to_draw_at = ivec2(gl_GlobalInvocationID.x, gl_GlobalInvocationID.y);

				float x_cart = (float(gl_GlobalInvocationID.x) - float(image_size.x) / float(2)) / (float((image_size.x) / float(2)));
				float y_cart = (float(image_size.y) / float(2) - float(gl_GlobalInvocationID.y)) / (float(image_size.y) / float(2));

				float ColorValueX = x_cart;
				float ColorValueY = y_cart;
				vec2 xy = vec2(x_cart, y_cart);

				float final_color = 0.0;

				switch (push.mShapeIndex)
				{
				case 0:
					final_color = smoothstep(-0.05, 0.05, -sdf_circle(xy, vec2(push.mPosDimen.r, push.mPosDimen.g), push.mParam.x));
					break;
				case 1:
					final_color = smoothstep(-0.05, 0.05, -sdf_rectangle(xy, vec2(push.mPosDimen.b, push.mPosDimen.a), vec2(push.mPosDimen.r, push.mPosDimen.g), push.mParam.x));
					break;
				case 2:
					final_color = smoothstep(-0.05, 0.05, -sdf_line(xy, vec2(push.mPosDimen.r, push.mPosDimen.g), vec2(push.mPosDimen.b, push.mPosDimen.a)));
					break;
				case 3:
					final_color = 1.0;
					break;
				case 4:
					float a = abs(sin(gID.x * 10 + push.mPosDimen.r));
					float b = abs(sin(gID.y + push.mPosDimen.g));
					final_color = mix(a, b, 0.5);
					break;
				}

				if (final_color > 0.0)
				{
					imageStore(storageImage[idx], to_draw_at, vec4(push.mColor.x * final_color, push.mColor.y * final_color, push.mColor.z * final_color, final_color));
				}
				//vec3 ExistingColor = vec3(0, 0, 0);
				//vec3 ColorToBeGiven = vec3(push.mColor.x * final_color, push.mColor.y * final_color, push.mColor.z * final_color);
				//vec4 ExistingColor4 = imageLoad(storageImage[idx], to_draw_at);

				//ExistingColor = vec3(ExistingColor4.x, ExistingColor4.y, ExistingColor4.z);
				//ColorToBeGiven = mix(ExistingColor, ColorToBeGiven, final_color);
				//imageStore(storageImage[idx], to_draw_at, vec4(ColorToBeGiven, final_color));

			}

			GlslCodeFinish();

		};

}









namespace FontShader
{
	using namespace glm;

	const auto VertexShader = [](ksai::Shader& inShader) {
		GlslShaderStart(vertex);

		GlslVersion(450);
		GlslShader(vertex);
		GlslExtension(GL_EXT_debug_printf);

		GlslCodeStart();

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

		GlslCodeFinish();

		};

	const auto FragmentShader = [](ksai::Shader& inShader) {
		GlslShaderStart(fragment);

		GlslVersion(450);
		GlslExtension(GL_EXT_debug_printf);

		GlslBindIn(vec3, 0, inColor);
		GlslBindIn(vec2, 1, inTextCoord);
		GlslBindOut(vec4, 0, outColor);


		GlslCodeStart();

		GlslCodeFinish();

		GlslCodeStart();

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

		GlslCodeFinish();

		};

	const auto ComputeShader = [](ksai::Shader& inShader)
		{
			GlslShaderStart(compute);

			GlslVersion(450);

			GlslExtension(GL_EXT_debug_printf);

			GlslCodeStart();

			layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

			layout(push_constant, std430) uniform pc {
				vec3 vt;
			} push;

			layout(set = 0, binding = 0) uniform sampler2D u_atlas8;

			void GlslMain()
			{
				uint gID = gl_GlobalInvocationID.x;
			}

			GlslCodeFinish();

		};
}












#include <ksaiSandBox/UndefGlsl.hpp>
