#pragma once
#include "Config.hpp"
#include <glm/glm.hpp>
#define GETTER inline auto
#define SETTER inline void
/*
CONFIGURATION
*/

template <class T> using Up = std::unique_ptr<T>;
template <class T> using Sp = std::shared_ptr<T>;

namespace ksai::kstd {
struct Vertex {
    glm::vec3 mPosition           = {0, 0, 0};
    glm::vec2 mTextureCoordinates = {0, 0};
};

struct VertexEXT {
    glm::vec3 mPosition           = {0, 0, 0};
    glm::vec2 mTextureCoordinates = {0, 0};
    glm::ivec3 mIvec3             = {0, 0, 0};
};

struct LineVertex {
    glm::vec3 mPosition = {0, 0, 0};
};

struct Vertex3D {
    alignas(16) glm::vec3 mPosition;
    alignas(16) glm::vec3 mNormal;
    alignas(8) glm::vec2 mUV;
    alignas(16) glm::vec3 mColor;
};

struct Vertex3DExt {
    glm::vec3 mPosition;
    glm::vec3 mNormal;
    glm::vec2 mUV;
    glm::vec3 mColor;
    glm::vec4 mTangent;
    glm::vec4 mJointIndices;
    glm::vec4 mJointWeights;
};

struct JointInfluence {
    alignas(16) glm::vec4 mJointIndices;
    alignas(16) glm::vec4 mJointWeights;
};

struct Tangent {
    alignas(16) glm::vec4 mTangent;
};

using ui32 = uint32_t;
using f32  = float_t;

namespace BindingIndex {
enum Storage : int {
    JointMatrix       = 1,
    JointInfluence    = 2,
    WorldVertexBuffer = 12,
    WorldIndexBuffer  = 13,
    VertexInfo        = 14
};
enum Uniform : int {
    WorldInfo    = 0,
    Images       = 3, // Upto 8 or After 8
    CubeMapImage = 20
};
}; // namespace BindingIndex

/* ============================================================

    World Info Uniform

============================================================== */
constexpr int LightCount = 8;
struct WorldInfoUniform {
    alignas(16) glm::mat4 mView;
    alignas(16) glm::mat4 mProjection;
    glm::vec3 mCameraPosition;
    alignas(16) glm::vec4 mNearFar;
    alignas(16) glm::vec4 mLights[LightCount];
    alignas(16) glm::mat4 mShadowMatrix;
    alignas(16) glm::vec4 mLightsDirections[LightCount];
};
constexpr int GeneralUsageDescriptorSetIndex3D = 1;

} // namespace ksai::kstd