#include "JkrLuaExe.hpp"

namespace JkrEXE {

void CreateGLMBindings(sol::state& lua) {
          auto vec2_multiply_overloads = sol::overload([](const glm::vec2& v1, const glm::vec2& v2) -> glm::vec2 { return v1 * v2; },
                                                       [](const glm::vec2& v1, float value) -> glm::vec2 { return v1 * value; },
                                                       [](float value, const glm::vec2& v1) -> glm::vec2 { return v1 * value; });

          auto vec2_divide_overloads = sol::overload([](const glm::vec2& v1, const glm::vec2& v2) -> glm::vec2 { return v1 / v2; },
                                                     [](const glm::vec2& v1, float value) -> glm::vec2 { return v1 / value; },
                                                     [](float value, const glm::vec2& v1) -> glm::vec2 { return v1 / value; });

          auto vec2_addition_overloads = sol::overload([](const glm::vec2& v1, const glm::vec2& v2) -> glm::vec2 { return v1 + v2; },
                                                       [](const glm::vec2& v1, float value) -> glm::vec2 { return v1 + value; },
                                                       [](float value, const glm::vec2& v1) -> glm::vec2 { return v1 + value; });

          auto vec2_subtraction_overloads = sol::overload([](const glm::vec2& v1, const glm::vec2& v2) -> glm::vec2 { return v1 - v2; },
                                                          [](const glm::vec2& v1, float value) -> glm::vec2 { return v1 - value; },
                                                          [](float value, const glm::vec2& v1) -> glm::vec2 { return v1 - value; });

          lua.new_usertype<glm::vec2>("vec2",
                                      sol::call_constructor,
                                      sol::constructors<glm::vec2(float), glm::vec2(float, float)>(),
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
                                      [](const glm::vec2& v) { return glm::length(v); });

          auto vec3_multiply_overloads = sol::overload([](const glm::vec3& v1, const glm::vec3& v2) { return v1 * v2; },
                                                       [](const glm::vec3& v1, float value) { return v1 * value; },
                                                       [](float value, const glm::vec3& v1) { return v1 * value; });

          auto vec3_divide_overloads = sol::overload([](const glm::vec3& v1, const glm::vec3& v2) { return v1 / v2; },
                                                     [](const glm::vec3& v1, float value) { return v1 / value; },
                                                     [](float value, const glm::vec3& v1) { return v1 / value; });

          auto vec3_addition_overloads = sol::overload([](const glm::vec3& v1, const glm::vec3& v2) { return v1 + v2; },
                                                       [](const glm::vec3& v1, float value) { return v1 + value; },
                                                       [](float value, const glm::vec3& v1) { return v1 + value; });

          auto vec3_subtraction_overloads = sol::overload([](const glm::vec3& v1, const glm::vec3& v2) { return v1 - v2; },
                                                          [](const glm::vec3& v1, float value) { return v1 - value; },
                                                          [](float value, const glm::vec3& v1) { return v1 - value; });

          lua.new_usertype<glm::vec3>("vec3",
                                      sol::call_constructor,
                                      sol::constructors<glm::vec3(float), glm::vec3(float, float, float)>(),
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
                                      vec3_subtraction_overloads);

          auto vec4_multiply_overloads = sol::overload([](const glm::vec4& v1, const glm::vec4& v2) { return v1 * v2; },
                                                       [](const glm::vec4& v1, float value) { return v1 * value; },
                                                       [](float value, const glm::vec4& v1) { return v1 * value; });

          auto vec4_divide_overloads = sol::overload([](const glm::vec4& v1, const glm::vec4& v2) { return v1 / v2; },
                                                     [](const glm::vec4& v1, float value) { return v1 / value; },
                                                     [](float value, const glm::vec4& v1) { return v1 / value; });

          auto vec4_addition_overloads = sol::overload([](const glm::vec4& v1, const glm::vec4& v2) { return v1 + v2; },
                                                       [](const glm::vec4& v1, float value) { return v1 + value; },
                                                       [](float value, const glm::vec4& v1) { return v1 + value; });

          auto vec4_subtraction_overloads = sol::overload([](const glm::vec4& v1, const glm::vec4& v2) { return v1 - v2; },
                                                          [](const glm::vec4& v1, float value) { return v1 - value; },
                                                          [](float value, const glm::vec4& v1) { return v1 - value; });

          lua.new_usertype<glm::vec4>("vec4",
                                      sol::call_constructor,
                                      sol::constructors<glm::vec4(float), glm::vec4(float, float, float, float)>(),
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
                                      vec4_subtraction_overloads

          );

          auto uvec2_multiply_overloads = sol::overload([](const glm::uvec2& v1, const glm::uvec2& v2) { return v1 * v2; },
                                                        [](const glm::uvec2& v1, uint32_t value) { return v1 * value; },
                                                        [](uint32_t value, const glm::uvec2& v1) { return v1 * value; });

          auto uvec2_divide_overloads = sol::overload([](const glm::uvec2& v1, const glm::uvec2& v2) { return v1 / v2; },
                                                      [](const glm::uvec2& v1, uint32_t value) { return v1 / value; },
                                                      [](uint32_t value, const glm::uvec2& v1) { return v1 / value; });

          auto uvec2_addition_overloads = sol::overload([](const glm::uvec2& v1, const glm::uvec2& v2) { return v1 + v2; },
                                                        [](const glm::uvec2& v1, uint32_t value) { return v1 + value; },
                                                        [](uint32_t value, const glm::uvec2& v1) { return v1 + value; });

          auto uvec2_subtraction_overloads = sol::overload([](const glm::uvec2& v1, const glm::uvec2& v2) { return v1 - v2; },
                                                           [](const glm::uvec2& v1, uint32_t value) { return v1 - value; },
                                                           [](uint32_t value, const glm::uvec2& v1) { return v1 - value; });

          lua.new_usertype<glm::uvec2>("uvec2",
                                       sol::call_constructor,
                                       sol::constructors<glm::uvec2(uint32_t), glm::uvec2(uint32_t, uint32_t), glm::uvec2(float, float)>(),
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
                                       uvec2_subtraction_overloads

          );

          auto mat4_multiply_overloads = sol::overload([](const glm::mat4& m1, const glm::vec4& v2) { return m1 * v2; },
                                                       [](const glm::mat4& m1, const glm::mat4& m2) { return m1 * m2; });

          lua.new_usertype<glm::mat4>(
           "mat4", sol::call_constructor, sol::constructors<glm::mat4(float)>(), sol::meta_function::multiplication, mat4_multiply_overloads);

          lua.set_function("std_vector_vec3", []() -> std::vector<glm::vec3> { return {}; });
          lua.set_function("std_vector_vec4", []() -> std::vector<glm::vec4> { return {}; });
          lua.set_function("std_vector_mat4", []() -> std::vector<glm::mat4> { return {}; });
          lua.set_function("std_vector_vec2", []() -> std::vector<glm::vec2> { return {}; });
          lua.set_function("std_vector_float", []() -> std::vector<float> { return {}; });
          lua.set_function("std_vector_int", []() -> std::vector<int> { return {}; });
          lua.set_function("std_vector_uint", []() -> std::vector<uint32_t> { return {}; });
          //	lua.set_function("std_vector_vertex3d", []() -> std::vector<ksai::std::Vertex3D> { return {}; });

          lua.set_function("Distance",
                           sol::overload([](glm::vec2& a, glm::vec2& b) { return glm::distance(a, b); },
                                         [](glm::vec3& a, glm::vec3& b) { return glm::distance(a, b); },
                                         [](glm::vec4& a, glm::vec4& b) { return glm::distance(a, b); }));
          lua.set_function("Lerp", [](float a, float b, float t) { return std::lerp(a, b, t); });
          lua.set_function("Clamp",
                           sol::overload([](float value, float min, float max) { return std::clamp(value, min, max); },
                                         [](double value, double min, double max) { return std::clamp(value, min, max); },
                                         [](int value, int min, int max) { return std::clamp(value, min, max); }));

          lua.create_named_table(
           "Jmath",
           "GetIdentityMatrix4x4",
           []() -> glm::mat4 { return glm::identity<glm::mat4>(); },

           "LookAt",
           [](glm::vec3 eye, glm::vec3 center, glm::vec3 up) -> glm::mat4 { return glm::lookAt(eye, center, up); },
           "Perspective",
           [](float fov, float aspect, float nearz, float farz) -> glm::mat4 { return glm::perspective(fov, aspect, nearz, farz); },
           "Ortho",
           [](float left, float right, float bottom, float top, float nearz, float farz) -> glm::mat4 {
                     return glm::ortho(left, right, bottom, top, nearz, farz);
           },

           "Translate",
           [](glm::mat4 inmatrix, glm::vec3 invector) -> glm::mat4 { return glm::translate(inmatrix, invector); },

           "Scale",
           [](glm::mat4 inmatrix, glm::vec3 invector) -> glm::mat4 { return glm::scale(inmatrix, invector); },
           "Rotate",
           [](glm::mat4 inmatrix, float angle_rad, glm::vec3 invector) -> glm::mat4 { return glm::rotate(inmatrix, angle_rad, invector); },
           "Rotate_deg",
           [](glm::mat4 inmatrix, float angle_deg, glm::vec3 invector) -> glm::mat4 {
                     return glm::rotate(inmatrix, glm::radians(angle_deg), invector);
           },
           "Normalize",
           [](glm::vec3 invec) { return glm::normalize(invec); },
           "Magnitude",
           [](glm::vec3 invec) { return glm::length(invec); });
}

} // namespace JkrEXE