#include "JkrLuaExe.hpp"
#include "glm/fwd.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>

namespace JkrEXE {

void CreateGLMBindings(sol::state &lua) {
    auto vec2_multiply_overloads = sol::overload(
         [](const glm::vec2 &v1, const glm::vec2 &v2) -> glm::vec2 { return v1 * v2; },
         [](const glm::vec2 &v1, float value) -> glm::vec2 { return v1 * value; },
         [](float value, const glm::vec2 &v1) -> glm::vec2 { return v1 * value; });

    auto vec2_divide_overloads = sol::overload(
         [](const glm::vec2 &v1, const glm::vec2 &v2) -> glm::vec2 { return v1 / v2; },
         [](const glm::vec2 &v1, float value) -> glm::vec2 { return v1 / value; },
         [](float value, const glm::vec2 &v1) -> glm::vec2 { return v1 / value; });

    auto vec2_addition_overloads = sol::overload(
         [](const glm::vec2 &v1, const glm::vec2 &v2) -> glm::vec2 { return v1 + v2; },
         [](const glm::vec2 &v1, float value) -> glm::vec2 { return v1 + value; },
         [](float value, const glm::vec2 &v1) -> glm::vec2 { return v1 + value; });

    auto vec2_subtraction_overloads = sol::overload(
         [](const glm::vec2 &v1, const glm::vec2 &v2) -> glm::vec2 { return v1 - v2; },
         [](const glm::vec2 &v1, float value) -> glm::vec2 { return v1 - value; },
         [](float value, const glm::vec2 &v1) -> glm::vec2 { return v1 - value; });

    lua.new_usertype<glm::vec2>(
         "vec2",
         sol::call_constructor,
         sol::constructors<glm::vec2(float),
                           glm::vec2(float, float),
                           glm::vec2(glm::vec2),
                           glm::vec2(glm::vec3)>(),
         "x",
         &glm::vec2::x,
         "y",
         &glm::vec2::y,
         sol::meta_function::multiplication,
         vec2_multiply_overloads,
         sol::meta_function::division,
         vec2_divide_overloads,
         sol::meta_function::addition,
         vec2_addition_overloads,
         sol::meta_function::subtraction,
         vec2_subtraction_overloads,
         "length",
         [](const glm::vec2 &v) { return glm::length(v); },
         sol::meta_function::equal_to,
         [](const glm::vec2 &ina, const glm::vec2 &inb) { return ina == inb; }

    );

    auto vec3_multiply_overloads =
         sol::overload([](const glm::vec3 &v1, const glm::vec3 &v2) { return v1 * v2; },
                       [](const glm::vec3 &v1, float value) { return v1 * value; },
                       [](float value, const glm::vec3 &v1) { return v1 * value; });

    auto vec3_divide_overloads =
         sol::overload([](const glm::vec3 &v1, const glm::vec3 &v2) { return v1 / v2; },
                       [](const glm::vec3 &v1, float value) { return v1 / value; },
                       [](float value, const glm::vec3 &v1) { return v1 / value; });

    auto vec3_addition_overloads =
         sol::overload([](const glm::vec3 &v1, const glm::vec3 &v2) { return v1 + v2; },
                       [](const glm::vec3 &v1, float value) { return v1 + value; },
                       [](float value, const glm::vec3 &v1) { return v1 + value; });

    auto vec3_subtraction_overloads =
         sol::overload([](const glm::vec3 &v1, const glm::vec3 &v2) { return v1 - v2; },
                       [](const glm::vec3 &v1, float value) { return v1 - value; },
                       [](float value, const glm::vec3 &v1) { return v1 - value; });

    lua.new_usertype<glm::vec3>(
         "vec3",
         sol::call_constructor,
         sol::constructors<glm::vec3(float),
                           glm::vec3(float, float, float),
                           glm::vec3(glm::vec3),
                           glm::vec3(glm::vec2, float)>(),
         "x",
         &glm::vec3::x,
         "y",
         &glm::vec3::y,
         "z",
         &glm::vec3::z,
         sol::meta_function::multiplication,
         vec3_multiply_overloads,
         sol::meta_function::division,
         vec3_divide_overloads,
         sol::meta_function::addition,
         vec3_addition_overloads,
         sol::meta_function::subtraction,
         vec3_subtraction_overloads,
         sol::meta_function::equal_to,
         [](const glm::vec3 &ina, const glm::vec3 &inb) { return ina == inb; });

    auto vec4_multiply_overloads =
         sol::overload([](const glm::vec4 &v1, const glm::vec4 &v2) { return v1 * v2; },
                       [](const glm::vec4 &v1, float value) { return v1 * value; },
                       [](float value, const glm::vec4 &v1) { return v1 * value; });

    auto vec4_divide_overloads =
         sol::overload([](const glm::vec4 &v1, const glm::vec4 &v2) { return v1 / v2; },
                       [](const glm::vec4 &v1, float value) { return v1 / value; },
                       [](float value, const glm::vec4 &v1) { return v1 / value; });

    auto vec4_addition_overloads =
         sol::overload([](const glm::vec4 &v1, const glm::vec4 &v2) { return v1 + v2; },
                       [](const glm::vec4 &v1, float value) { return v1 + value; },
                       [](float value, const glm::vec4 &v1) { return v1 + value; });

    auto vec4_subtraction_overloads =
         sol::overload([](const glm::vec4 &v1, const glm::vec4 &v2) { return v1 - v2; },
                       [](const glm::vec4 &v1, float value) { return v1 - value; },
                       [](float value, const glm::vec4 &v1) { return v1 - value; });

    lua.new_usertype<glm::vec4>(
         "vec4",
         sol::call_constructor,
         sol::constructors<glm::vec4(float),
                           glm::vec4(float, float, float, float),
                           glm::vec4(glm::vec4),
                           glm::vec4(glm::vec3, float)>(),
         "x",
         &glm::vec4::x,
         "y",
         &glm::vec4::y,
         "z",
         &glm::vec4::z,
         "w",
         &glm::vec4::w,
         sol::meta_function::multiplication,
         vec4_multiply_overloads,
         sol::meta_function::division,
         vec4_divide_overloads,
         sol::meta_function::addition,
         vec4_addition_overloads,
         sol::meta_function::subtraction,
         vec4_subtraction_overloads,
         sol::meta_function::equal_to,
         [](const glm::vec4 &ina, const glm::vec4 &inb) { return ina == inb; });

    auto uvec2_multiply_overloads =
         sol::overload([](const glm::uvec2 &v1, const glm::uvec2 &v2) { return v1 * v2; },
                       [](const glm::uvec2 &v1, uint32_t value) { return v1 * value; },
                       [](uint32_t value, const glm::uvec2 &v1) { return v1 * value; });

    auto uvec2_divide_overloads =
         sol::overload([](const glm::uvec2 &v1, const glm::uvec2 &v2) { return v1 / v2; },
                       [](const glm::uvec2 &v1, uint32_t value) { return v1 / value; },
                       [](uint32_t value, const glm::uvec2 &v1) { return v1 / value; });

    auto uvec2_addition_overloads =
         sol::overload([](const glm::uvec2 &v1, const glm::uvec2 &v2) { return v1 + v2; },
                       [](const glm::uvec2 &v1, uint32_t value) { return v1 + value; },
                       [](uint32_t value, const glm::uvec2 &v1) { return v1 + value; });

    auto uvec2_subtraction_overloads =
         sol::overload([](const glm::uvec2 &v1, const glm::uvec2 &v2) { return v1 - v2; },
                       [](const glm::uvec2 &v1, uint32_t value) { return v1 - value; },
                       [](uint32_t value, const glm::uvec2 &v1) { return v1 - value; });

    lua.new_usertype<glm::uvec2>(
         "uvec2",
         sol::call_constructor,
         sol::constructors<glm::uvec2(uint32_t),
                           glm::uvec2(uint32_t, uint32_t),
                           glm::uvec2(float, float),
                           glm::uvec2(glm::uvec2)>(),
         "x",
         &glm::uvec2::x,
         "y",
         &glm::uvec2::y,
         sol::meta_function::multiplication,
         uvec2_multiply_overloads,
         sol::meta_function::division,
         uvec2_divide_overloads,
         sol::meta_function::addition,
         uvec2_addition_overloads,
         sol::meta_function::subtraction,
         uvec2_subtraction_overloads,
         sol::meta_function::equal_to,
         [](const glm::uvec2 &ina, const glm::uvec2 &inb) { return ina == inb; }

    );

    auto quat_multiply_overloads = sol::overload(
         [](const glm::quat &q1, const glm::quat &q2) { return q1 * q2; },
         [](const glm::quat &q, float scalar) { return q * scalar; },
         [](float scalar, const glm::quat &q) { return scalar * q; },
         [](const glm::vec3 &inVec, const glm::quat &q) -> glm::quat { return inVec * q; });

    auto quat_divide_overloads =
         sol::overload([](const glm::quat &q, float scalar) { return q / scalar; });

    lua.new_usertype<glm::quat>(
         "quat",
         sol::call_constructor,
         sol::constructors<glm::quat(),
                           glm::quat(float, float, float, float),
                           glm::quat(glm::vec3),
                           glm::quat(glm::quat)>(),
         "x",
         &glm::quat::x,
         "y",
         &glm::quat::y,
         "z",
         &glm::quat::z,
         "w",
         &glm::quat::w,
         sol::meta_function::multiplication,
         quat_multiply_overloads,
         sol::meta_function::division,
         quat_divide_overloads,
         "Rotate",
         [](glm::quat &inQuat, float inValue, glm::vec3 inAxis) {
             float factor  = sin(inValue / 2.0f);
             float x       = factor * inAxis.x;
             float y       = factor * inAxis.y;
             float z       = factor * inAxis.z;
             float w       = cos(inValue / 2.0f);
             glm::quat out = glm::normalize(glm::quat(w, x, y, z)) * inQuat;
             return out;
         },
         "Rotate_deg",
         [](glm::quat &inQuat, float inValue, glm::vec3 inAxis) {
             float value   = glm::radians(inValue);
             float factor  = sin(value / 2.0f);
             float x       = factor * inAxis.x;
             float y       = factor * inAxis.y;
             float z       = factor * inAxis.z;
             float w       = cos(value / 2.0f);
             glm::quat out = glm::normalize(glm::quat(w, x, y, z)) * inQuat;
             return out;
         },
         "GetMatrix3x3",
         [](glm::quat &inQuat) { return glm::mat3_cast(inQuat); },
         sol::meta_function::equal_to,
         [](const glm::quat &ina, const glm::quat &inb) { return ina == inb; }

    );

    auto mat4_multiply_overloads =
         sol::overload([](const glm::mat4 &m1, const glm::vec4 &v2) { return m1 * v2; },
                       [](const glm::mat4 &m1, const glm::mat4 &m2) { return m1 * m2; });

    using namespace glm;
    lua.new_usertype<glm::mat4>(
         "mat4",
         sol::call_constructor,
         sol::constructors<glm::mat4(float),
                           glm::mat4(const vec4 &, const vec4 &, const vec4 &, const vec4 &),
                           glm::mat4(glm::mat4)>(),
         sol::meta_function::multiplication,
         mat4_multiply_overloads,
         "GetTranslationComponent",
         [](glm::mat4 &inMatrix) {
             glm::vec3 scale;
             glm::quat rotation;
             glm::vec3 translation;
             glm::vec3 skew;
             glm::vec4 perspective;
             glm::decompose(inMatrix, scale, rotation, translation, skew, perspective);
             return translation;
         },
         "GetRotationComponent",
         [](glm::mat4 &inMatrix) {
             glm::vec3 scale;
             glm::quat rotation;
             glm::vec3 translation;
             glm::vec3 skew;
             glm::vec4 perspective;
             glm::decompose(inMatrix, scale, rotation, translation, skew, perspective);
             return rotation; // Check if the quaternions are correct
         },
         "GetScaleComponent",
         [](glm::mat4 &inMatrix) {
             glm::vec3 scale;
             glm::quat rotation;
             glm::vec3 translation;
             glm::vec3 skew;
             glm::vec4 perspective;
             glm::decompose(inMatrix, scale, rotation, translation, skew, perspective);
             return scale;
         });

    auto mat3_multiply_overloads = sol::overload(
         [](const glm::mat3 &m1, const glm::vec3 &v2) -> glm::vec3 { return m1 * v2; },
         [](const glm::mat3 &m1, const glm::mat3 &m2) { return m1 * m2; });

    lua.new_usertype<glm::mat3>(
         "mat3",
         sol::call_constructor,
         sol::constructors<glm::mat3(float),
                           glm::mat3(const vec3 &, const vec3 &, const vec3 &),
                           glm::mat3(glm::mat3),
                           glm::mat3(glm::mat4)>(),
         sol::meta_function::multiplication,
         mat3_multiply_overloads);

    lua.set_function("std_vector_vec3", []() -> std::vector<glm::vec3> { return {}; });
    lua.set_function("std_vector_vec4", []() -> std::vector<glm::vec4> { return {}; });
    lua.set_function("std_vector_mat4", []() -> std::vector<glm::mat4> { return {}; });
    lua.set_function("std_vector_vec2", []() -> std::vector<glm::vec2> { return {}; });
    lua.set_function("std_vector_float", []() -> std::vector<float> { return {}; });
    lua.set_function("std_vector_int", []() -> std::vector<int> { return {}; });
    lua.set_function("std_vector_uint", []() -> std::vector<uint32_t> { return {}; });
    lua.set_function("std_vector_char", [](int inSize) -> std::vector<char> {
        std::vector<char> c;
        c.resize(inSize, 0);
        return c;
    });
    //	lua.set_function("std_vector_vertex3d", []() -> std::vector<ksai::std::Vertex3D> { return {};
    //});

    lua.set_function("Distance",
                     sol::overload([](glm::vec2 &a, glm::vec2 &b) { return glm::distance(a, b); },
                                   [](glm::vec3 &a, glm::vec3 &b) { return glm::distance(a, b); },
                                   [](glm::vec4 &a, glm::vec4 &b) { return glm::distance(a, b); }));
    lua.create_named_table(
         "Jmath",
         "GetIdentityMatrix4x4",
         []() -> glm::mat4 { return glm::identity<glm::mat4>(); },
         "LookAt",
         [](glm::vec3 eye, glm::vec3 center, glm::vec3 up) -> glm::mat4 {
             return glm::lookAt(eye, center, up);
         },
         "Perspective",
         [](float fov, float aspect, float nearz, float farz) -> glm::mat4 {
             return glm::perspective(fov, aspect, nearz, farz);
         },
         "Ortho",
         [](float left, float right, float bottom, float top, float nearz, float farz)
              -> glm::mat4 { return glm::ortho(left, right, bottom, top, nearz, farz); },
         "Translate",
         [](glm::mat4 inmatrix, glm::vec3 invector) -> glm::mat4 {
             return glm::translate(inmatrix, invector);
         },
         "Scale",
         [](glm::mat4 inmatrix, glm::vec3 invector) -> glm::mat4 {
             return glm::scale(inmatrix, invector);
         },
         "Rotate",
         sol::overload([](glm::mat4 inmatrix, float angle_rad, glm::vec3 invector) -> glm::mat4 {
             return glm::rotate(inmatrix, angle_rad, invector);
         }),
         "Rotate_deg",
         [](glm::mat4 inmatrix, float angle_deg, glm::vec3 invector) -> glm::mat4 {
             return glm::rotate(inmatrix, glm::radians(angle_deg), invector);
         },
         "Normalize",
         sol::overload([](glm::vec3 invec) { return glm::normalize(invec); },
                       [](glm::vec4 invec) { return glm::normalize(invec); }),
         "Magnitude",
         [](glm::vec3 invec) { return glm::length(invec); },
         "Cross",
         [](glm::vec3 inA, glm::vec3 inB) { return glm::cross(inA, inB); },
         "PrintMatrix",
         [](glm::mat4 inMatrix) {
             for (int y = 0; y < 4; ++y) {
                 for (int x = 0; x < 4; ++x) {
                     std::cout << inMatrix[x][y] << "\t";
                 }
                 std::cout << "\n";
             }
         }

    );

    using namespace glm;
    auto Jmath = lua["Jmath"].get_or_create<sol::table>();
    Jmath.set_function("Lerp",
                       sol::overload(sol::resolve<float(float, float, float)>(&mix),
                                     sol::resolve<vec2(const vec2 &, const vec2 &, float)>(&mix),
                                     sol::resolve<vec3(const vec3 &, const vec3 &, float)>(&mix),
                                     sol::resolve<quat(const quat &, const quat &, float)>(&slerp),
                                     sol::resolve<vec4(const vec4 &, const vec4 &, float)>(&mix)));
    Jmath.set_function(
         "Clamp",
         sol::overload(
              [](float value, float min, float max) { return std::clamp(value, min, max); },
              [](int value, int min, int max) { return std::clamp(value, min, max); }));

    Jmath.set_function("Inverse",
                       sol::overload([](const glm::mat4 &inMat) { return glm::inverse(inMat); },
                                     [](const glm::mat3 &inMat) { return glm::inverse(inMat); }));
    Jmath.set_function("Transpose",
                       sol::overload([](const glm::mat4 &inMat) { return glm::transpose(inMat); },
                                     [](const glm::mat3 &inMat) { return glm::transpose(inMat); }));
}

} // namespace JkrEXE