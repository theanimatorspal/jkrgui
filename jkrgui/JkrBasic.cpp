#include "EventManager.hpp"
#include "JkrLuaExe.hpp"
#include "Painter.hpp"
#include "Renderers/ThreeD/Uniform3D.hpp"
#include "Renderers/ThreeD/World3D.hpp"
#include "Window.hpp"
#include "TinyGLTF/json.hpp"

namespace JkrEXE {
using namespace std;
using namespace Jkr;
using json = nlohmann::json;

template <typename T> std::string dump_value(const T &value) {
      if constexpr (std::is_same_v<T, std::string>) {
            return value;
      } else if constexpr (std::is_same_v<T, bool>) {
            return value ? "true" : "false";
      } else if constexpr (std::is_arithmetic_v<T>) {
            return std::to_string(value);
      } else {
            return "<unsupported-type>";
      }
}

template <> std::string dump_value<glm::vec4>(const glm::vec4 &vec) {
      return std::format("{:.3f}, {:.3f}, {:.3f}, {:.3f}", vec.x, vec.y, vec.z, vec.w);
}

template <> std::string dump_value<glm::vec2>(const glm::vec2 &vec) {
      return std::format("{:.3f}, {:.3f}", vec.x, vec.y);
}

template <> std::string dump_value<glm::vec3>(const glm::vec3 &vec) {
      return std::format("{:.3f}, {:.3f}, {:.3f}", vec.x, vec.y, vec.z);
}

template <> std::string dump_value<glm::mat4>(const glm::mat4 &mat) {
      std::string s;
      for (int row = 0; row < 4; ++row) {
            s += std::format("[{:.3f}, {:.3f}, {:.3f}, {:.3f}]",
                             mat[row][0],
                             mat[row][1],
                             mat[row][2],
                             mat[row][3]);
            if (row < 3) s += ", ";
      }
      return s;
}

template <typename... Ts> std::string try_sol_cast(sol::object obj) {
      std::string result = "<unknown>";
      (..., (obj.is<Ts>() ? result = dump_value(obj.as<Ts>()) : void()));
      return result;
}

json dump_table(const sol::table &table);

json dump_object(const sol::object &obj) {
      if (obj.is<sol::table>()) {
            return dump_table(obj.as<sol::table>());
      } else if (obj.is<std::string>()) {
            return obj.as<std::string>();
      } else if (obj.is<double>()) {
            return obj.as<double>();
      } else if (obj.is<bool>()) {
            return obj.as<bool>();
      } else if (obj.is<glm::vec2>()) {
            return dump_value(obj.as<glm::vec2>());
      } else if (obj.is<glm::vec3>()) {
            return dump_value(obj.as<glm::vec3>());
      } else if (obj.is<glm::vec4>()) {
            return dump_value(obj.as<glm::vec4>());
      } else if (obj.is<glm::mat4>()) {
            return dump_value(obj.as<glm::mat4>());
      }
      return "<unknown>";
}

json dump_table(const sol::table &table) {
      json j;

      for (auto &kv : table) {
            sol::object key   = kv.first;
            sol::object value = kv.second;

            std::string k;
            if (key.is<std::string>()) {
                  k = key.as<std::string>();
            } else if (key.is<double>()) {
                  k = std::to_string(key.as<double>());
            } else {
                  k = "<non-printable-key>";
            }

            j[k] = dump_object(value);
      }

      return j;
}

static std::mutex log_mut;
void CreateBasicBindings(sol::state &s) {
      auto Jkr = s["Jkr"].get_or_create<sol::table>();

      Jkr.set_function("SetLogCallBack", [](sol::function inFunction) {
            std::cout << "Log Call back set" << std::endl;
            SetLogCallBack([=](sv inColor, sv inMessage, sv inType) {
                  std::lock_guard<std::mutex> guard(log_mut);
                  inFunction(inColor, inMessage, inType);
                  return 22;
            });
      });
      Jkr.new_usertype<Jkr::Instance>(
           "Instance",
           sol::call_constructor,
           sol::factories([](int inDescriptorSize, int inPoolSize, bool inEnableValidation) {
                 return std::make_unique<Jkr::Instance>(
                      inDescriptorSize, inPoolSize, inEnableValidation);
           }));
      Jkr.new_usertype<Jkr::Window_base>("WindowBase");
      Jkr.new_usertype<Jkr::Window>(
           "Window",
           sol::base_classes,
           sol::bases<Jkr::Window_base>(),
           sol::call_constructor,
           sol::factories(
                [](Jkr::Instance &inInstance,
                   std::string_view inTitle,
                   int inWidth,
                   int inHeight,
                   int inThreadsCount) {
                      int NoOfCmdBufferPerThread = 2; // TODO Don't hardcode
                      std::vector<ui> CmdBufferCountPerThreadVec;
                      CmdBufferCountPerThreadVec.resize(inThreadsCount, NoOfCmdBufferPerThread);
                      return std::make_unique<Jkr::Window>(inInstance,
                                                           inTitle,
                                                           inHeight,
                                                           inWidth,
                                                           1080,
                                                           1920,
                                                           inThreadsCount,
                                                           CmdBufferCountPerThreadVec,
                                                           inInstance.GetThreadPool());
                },
                [](Jkr::Instance &inInstance,
                   std::string_view inTitle,
                   int inWidth,
                   int inHeight,
                   int inOffscreenFrameWidth,
                   int inOffscreenFrameHeight,
                   int inThreadsCount) {
                      int NoOfCmdBufferPerThread = 2; // TODO Don't hardcode
                      std::vector<ui> CmdBufferCountPerThreadVec;
                      CmdBufferCountPerThreadVec.resize(inThreadsCount, NoOfCmdBufferPerThread);
                      return std::make_unique<Jkr::Window>(inInstance,
                                                           inTitle,
                                                           inHeight,
                                                           inWidth,
                                                           inOffscreenFrameHeight,
                                                           inOffscreenFrameWidth,
                                                           inThreadsCount,
                                                           CmdBufferCountPerThreadVec,
                                                           inInstance.GetThreadPool());
                },
                [](Jkr::Instance &inInstance,
                   int inOffscreenFrameWidth,
                   int inOffscreenFrameHeight,
                   int inThreadsCount) {
                      int NoOfCmdBufferPerThread = 2;
                      std::vector<ui> CmdBufferCountPerThreadVec;
                      CmdBufferCountPerThreadVec.resize(inThreadsCount, NoOfCmdBufferPerThread);
                      return std::make_unique<Jkr::Window>(inInstance,
                                                           CmdBufferCountPerThreadVec,
                                                           inOffscreenFrameHeight,
                                                           inOffscreenFrameWidth,
                                                           inThreadsCount);
                }),

           "Show",
           &Jkr::Window::Show,
           "Hide",
           &Jkr::Window::Hide,
           "Destroy",
           &Jkr::Window::Destroy,
           "Minimize",
           &Jkr::Window::Minimize,
           "Maximize",
           &Jkr::Window::Maximize,
           "ToggleWindowFullScreen",
           &Jkr::Window::ToggleWindowFullScreen,

           "BuildShadowPass",
           &Jkr::Window::BuildShadowPass,
           "BeginShadowPass",
           &Jkr::Window::BeginShadowPass,
           "EndShadowPass",
           &Jkr::Window::EndShadowPass,

           "BuildDeferredPass",
           &Jkr::Window::BuildDeferredPass,
           "PrepareDeferredPass",
           &Jkr::Window::PrepareDeferredPass,
           "BeginDeferredDraws",
           &Jkr::Window::BeginDeferredDraws,
           "EndDeferredDraws",
           &Jkr::Window::EndDeferredDraws,
           "ExecuteDeferredComposition",
           &Jkr::Window::ExecuteDeferredComposition,
           "PresentDeferred",
           &Jkr::Window::PresentDeferred,

           "SetTitle",
           &Jkr::Window::SetTitle,
           "Wait",
           &Jkr::Window::Wait,
           "AcquireImage",
           &Jkr::Window::AcquireImage,
           "BeginRecording",
           &Jkr::Window::BeginRecording,
           "EndRecording",
           &Jkr::Window::EndRecording,
           "BeginDraws",
           &Jkr::Window::BeginDraws,
           "EndDraws",
           &Jkr::Window::EndDraws,
           "Present",
           &Jkr::Window::Present,
           "Submit",
           &Jkr::Window::Submit,
           "BlitImage",
           &Jkr::Window::BlitImage,

           "BeginUIs",
           &Jkr::Window::BeginUIs,
           "EndUIs",
           &Jkr::Window::EndUIs,
           "ExecuteUIs",
           &Jkr::Window::ExecuteUIs,

           "BeginImmediate",
           &Jkr::Window::BeginImmediate,
           "EndImmediate",
           &Jkr::Window::EndImmediate,

           "BeginThreadCommandBuffer",
           &Jkr::Window::BeginThreadCommandBuffer,
           "EndThreadCommandBuffer",
           &Jkr::Window::EndThreadCommandBuffer,
           "ExecuteThreadCommandBuffer",
           &Jkr::Window::ExecuteThreadCommandBuffer,

           "GetWindowCurrentTime",
           &Jkr::Window::GetWindowCurrentTime,
           "GetWindowDimension",
           &Jkr::Window::GetWindowDimension,
           "GetVulkanDrawableSize",
           &Jkr::Window::GetVulkanDrawableSize,
           "GetOffscreenFrameDimension",
           &Jkr::Window::GetOffscreenFrameDimension,

           "SetScissor",
           [](Window &inWindow,
              glm::vec3 inVec3,
              glm::vec3 inDimesnion,
              Jkr::Window::ParameterContext incontext) {
                 inWindow.SetScissor(inVec3.x, inVec3.y, inDimesnion.x, inDimesnion.y, incontext);
           },
           "SetDefaultScissor",
           &Jkr::Window::SetDefaultScissor,
           "SetViewport",
           [](Window &inWindow,
              glm::vec3 inVec3,
              glm::vec3 inDimesnion,
              Jkr::Window::ParameterContext incontext) {
                 inWindow.SetViewport(
                      inVec3.x, inVec3.y, inDimesnion.x, inDimesnion.y, 0, 1, incontext);
           },
           "SetDefaultViewport",
           &Jkr::Window::SetDefaultViewport);

      using namespace Jkr;
      Jkr.new_usertype<Jkr::EventManager>(
           "EventManager",
           sol::call_constructor,
           sol::factories([]() { return mu<Jkr::EventManager>(); }),
           "ShouldQuit",
           &Jkr::EventManager::ShouldQuit,
           "ProcessEvents",
           &EventManager::ProcessEvents,
           "ProcessEventsEXT",
           &EventManager::ProcessEventsEXT,
           "ShouldQuit",
           &EventManager::ShouldQuit,
           "GetEventHandle",
           &EventManager::GetEventHandle,
           "GetMousePos",
           &EventManager::GetMousePos,
           "GetRelativeMousePos",
           &EventManager::GetRelativeMousePos,
           "GetMouseButtonValue",
           &EventManager::GetMouseButtonValue,
           "IsKeyPressedContinous",
           &EventManager::IsKeyPressedContinous,
           "IsLeftButtonPressedContinous",
           &EventManager::IsLeftButtonPressedContinous,
           "IsRightButtonPressedContinous",
           &EventManager::IsRightButtonPressedContinous,
           "IsLeftButtonPressed",
           &EventManager::IsLeftButtonPressed,
           "IsRightButtonPressed",
           &EventManager::IsRightButtonPressed,
           "IsKeyReleased",
           &EventManager::IsKeyReleased,
           "IsKeyPressed",
           &EventManager::IsKeyPressed,
           "IsCloseWindowEvent",
           &EventManager::IsCloseWindowEvent,
           "StartTextInput",
           &EventManager::StartTextInput,
           "StopTextInput",
           &EventManager::StopTextInput,
           "IsMouseWithinAtTopOfStack",
           &EventManager::IsMouseWithinAtTopOfStack,
           "IsMouseWithin",
           &EventManager::IsMouseWithin,
           "SetBoundedRect",
           &EventManager::SetBoundedRect,
           "UpdateBoundedRect",
           &EventManager::UpdateBoundedRect,
           "RemoveBoundedRect",
           &EventManager::RemoveBoundedRect,
           "SetEventCallBack",
           [](Jkr::EventManager &inManager, sol::function inFunction) {
                 inManager.SetEventCallBack([=]() {
                       auto result = inFunction();
                       if (not result.valid()) {
                             sol::error error = result;
                             Log(error.what(), "ERROR");
                       }
                 });
           },
           "RemoveEventCallBacks",
           &EventManager::RemoveEventCallBacks);

      auto PainterCacheTypeEnum = Jkr.new_enum<false>("PainterType",
                                                      "Default",
                                                      PipelinePropertiesContext::Default,
                                                      "Line",
                                                      PipelinePropertiesContext::Line,
                                                      "LineStrip",
                                                      PipelinePropertiesContext::LineStrip);
      Jkr.new_usertype<Jkr::PainterCache>(
           "PainterCache",
           sol::call_constructor,
           [](Jkr::Instance &inInstance, PipelinePropertiesContext inContext) {
                 return mu<Jkr::PainterCache>(inInstance, inContext);
           },
           "Store",
           [](Jkr::PainterCache &inCache,
              Jkr::Misc::File &inFile,
              string_view inFilename,
              string_view inVertexShader,
              string_view inFragmentShader,
              string_view inComputeShader) {
                 inCache.Store(inFile,
                               string(inFilename),
                               string(inVertexShader),
                               string(inFragmentShader),
                               string(inComputeShader));
           });

      Jkr.set_function("__breakpoint", [](sol::this_state s, int bp_id) {
            sol::state_view lua(s);
            sol::table debug = lua["debug"];

            json dump;
            dump["breakpoint"] = bp_id;
            dump["stack"]      = json::array();

            int level          = 1;
            while (true) {
                  sol::object info_obj = debug["getinfo"](level, "nSl");
                  if (!info_obj.valid()) break;

                  sol::table info = info_obj;
                  json frame;

                  frame["level"]  = level;
                  frame["func"]   = info.get_or("name", std::string("unknown"));
                  frame["source"] = info.get_or("short_src", std::string("??"));
                  frame["line"]   = info.get_or("currentline", -1);
                  frame["locals"] = json::object();

                  int i           = 1;
                  while (true) {
                        sol::object name_obj = debug["getlocal"](info, i);
                        if (!name_obj.valid()) break;

                        std::string var_name      = name_obj.as<std::string>();
                        sol::object var_value     = debug["getlocal"](info, i + 1);

                        frame["locals"][var_name] = dump_object(var_value);

                        i += 2;
                  }

                  dump["stack"].push_back(frame);
                  ++level;
            }

            std::cout << std::setw(4) << dump << "\n";
      });
}
} // namespace JkrEXE