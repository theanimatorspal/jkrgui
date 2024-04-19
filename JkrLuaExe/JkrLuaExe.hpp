// JkrLuaExe.h : Header file for your target.

#pragma once
#include "Pipeline/VulkanPipelineContext.hpp"
#include "Pipeline/VulkanShaderModule.hpp"
#include "Renderers/Renderer_base.hpp"
#include "Renderers/ThreeD/Shape3D.hpp"
#include "Renderers/TwoD/Line.hpp"
#include "Renderers/TwoD/Shape.hpp"
#include "Window.hpp"
#include "glm/fwd.hpp"
#include "ksai_config.hpp"
#include "ksai_thread.hpp"
#include <EventManager.hpp>
#include <Instance.hpp>
#include <Misc/RecycleBin.hpp>
#include <Misc/ThreeD/Skybox.hpp>
#include <Misc/ThreeD/Uniform3D.hpp>
#include <Renderers/BestText_Alt.hpp>
#include <Renderers/ThreeD/Simple3D.hpp>
#include <Renderers/ThreeD/glTF_Model.hpp>
#include <Renderers/TwoD/Shape.hpp>
#include <WindowMulT.hpp>
#include <filesystem>
#include <functional>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <sol/sol.hpp>
#include <vector>

#ifdef ANDROID
#define JKR_EXPORT __attribute__((visibility("default")))
#else
#define JKR_EXPORT __declspec(dllexport)
#endif

sol::state& GetMainStateRef();

using namespace std;
using namespace ksai;

static auto my_exception_handler(lua_State* L, sol::optional<const std::exception&> maybe_execption, sol::string_view description) -> int {
               std::cout << "An Exception "
                            "has occured in "
                            "the function "
                            "here is what "
                            "it says: ";
               if (maybe_execption) {
                              std::cout << "(Straight from the exception) ";
                              const std::exception& ex = *maybe_execption;
                              std::cout << ex.what() << std::endl;
               } else {
                              std::cout << "(From the Descriptor Parameter) ";
                              std::cout.write(description.data(), static_cast<std::streamsize>(description.size()));
                              std::cout << std::endl;
               }
               return sol::stack::push(L, description);
};

struct DefaultCustomImagePainterPushConstant {
               glm::vec4 x, y, z;
};
