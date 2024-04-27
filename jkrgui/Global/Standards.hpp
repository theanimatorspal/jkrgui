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
    glm::vec3 mPosition;
    glm::vec3 mNormal;
    glm::vec2 mUV;
    glm::vec3 mColor;
};

struct Vertex3DExt {
    glm::vec3 mPosition;
    glm::vec3 mNormal;
    glm::vec2 mUV;
    glm::vec3 mColor;
    glm::vec4 mJointIndices;
    glm::vec4 mJointWeights;
};

struct JointInfluence {
    alignas(16) glm::vec4 mJointIndices;
    alignas(16) glm::vec4 mJointWeights;
};

using ui32 = uint32_t;
using f32  = float_t;

namespace BindingIndex {
enum Storage : int { JointMatrix = 1, JointInfluence = 2 };
enum Uniform : int {
    WorldInfo     = 0,
    DiffuseImage  = 3,
    SpecularImage = 4,
    NormalImage   = 5,
    CubeMapImage  = 8
};
}; // namespace BindingIndex
} // namespace ksai::kstd