#include "JkrLuaExe.hpp"
#include "Renderers/TwoD/Line.hpp"
#include "Window.hpp"
#include "sol/sol.hpp"
#include "vulkan/vulkan_enums.hpp"

namespace JkrEXE {
extern void CreateMainBindings(sol::state& s);
enum class AllTypes {
          Window,
          DefaultCustomImagePainterPushConstant,
          Misc_RecycleBin,
          Renderer_Line,
          MultiThreading,
};
struct MultiThreading {
          MultiThreading(Jkr::Instance& inInstance);
          void Inject(std::string_view inVariable, sol::object inValue);
          void InjectScript(std::string inView);
          void AddJob(std::string inView);
          void InjectScriptF(sol::function inFunction);
          void AddJobF(sol::function inFunction);
          sol::object Get(std::string_view inVariable);
          sol::object GetFull(sol::object inObject);
          sol::object CastToType(sol::object inObject, AllTypes inTypes);

      private:
          sol::object Copy(sol::object obj, sol::state& inTarget);
          ksai::ThreadPool& mPool;
          std::vector<sol::state> mStates;
};

inline MultiThreading::MultiThreading(Jkr::Instance& inInstance) : mPool(inInstance.GetThreadPool()) {
          mStates.resize(inInstance.GetThreadPool().mThreads.size());
          int i = 0;
          for (auto& state : mStates) {
                    CreateMainBindings(state);
                    state["StateId"] = i++;
          }
}

inline void MultiThreading::Inject(std::string_view inVariable, sol::object inValue) {
          for (auto& state : mStates) {
                    state[inVariable] = Copy(inValue, state);
          }
}

inline sol::object MultiThreading::Get(std::string_view inVariable) { return mStates.front()[inVariable]; }

inline void MultiThreading::InjectScript(std::string inView) {
          for (auto& state : mStates) {
                    state.script(inView);
          }
}

inline void MultiThreading::AddJob(std::string inView) {
          auto& state = mStates[mPool.GetThreadIndex()];
          int ThreadIndex = mPool.GetThreadIndex();
          mPool.Add_Job([=, &state]() {
                    state["ThreadingId"] = ThreadIndex;
                    state.script(inView);
          });
}

inline void MultiThreading::AddJobF(sol::function inFunction) {
          auto& state = mStates[mPool.GetThreadIndex()];
          auto f = inFunction.dump();
          mPool.Add_Job([=, &state]() { state.script(f.as_string_view()); });
}

inline void MultiThreading::InjectScriptF(sol::function inFunction) {
          auto f = inFunction.dump();
          for (auto& state : mStates) {
                    state.script(f.as_string_view());
          }
}

inline static sol::object GetObjectByType(sol::object obj, sol::state& target, AllTypes inType) noexcept {
          sol::object Object = {};
          if (inType == AllTypes::Window) {
                    Object = sol::make_object(target, std::ref(obj.as<Jkr::Window>()));
          } else if (inType == AllTypes::DefaultCustomImagePainterPushConstant) {
                    Object = sol::make_object(target, std::ref(obj.as<DefaultCustomImagePainterPushConstant>()));
          } else if (inType == AllTypes::Misc_RecycleBin) {
                    Object = sol::make_object(target, std::ref(obj.as<Jkr::Misc::RecycleBin<int>>()));
          } else if (inType == AllTypes::Renderer_Line) {
                    Object = sol::make_object(target, std::ref(obj.as<Jkr::Renderer::Renderer::Line>()));
          } else if (inType == AllTypes::MultiThreading) {
                    Object = sol::make_object(target, std::ref(obj.as<MultiThreading>()));
          } else {
                    {};
          }
          return Object;
}

template <typename T, typename... Ts> inline static sol::object getObjectByType(sol::object& obj, sol::state& target) noexcept {

          if constexpr (not(sizeof...(Ts) == 0)) {
                    if (obj.is<T>()) {
                              return sol::make_object(target, std::ref(obj.as<T>()));
                    } else {
                              return getObjectByType<Ts...>(obj, target); // TODO FIX last argument doesn't work template
                    }
          }
          return {};
}

sol::object MultiThreading::CastToType(sol::object inObject, AllTypes inType) { return GetObjectByType(inObject, mStates.front(), inType); }

inline sol::object MultiThreading::Copy(sol::object obj, sol::state& target) {
          sol::type tp = obj.get_type();
          switch (tp) {
                    case sol::type::number: {
                              const double d = obj.as<double>();
                              return sol::make_object(target, d);
                    } break;
                    case sol::type::boolean: {
                              const bool b = obj.as<bool>();
                              return sol::make_object(target, b);
                    } break;
                    case sol::type::string: {
                              const std::string s = obj.as<std::string>();
                              return sol::make_object(target, s);
                    } break;
                    case sol::type::table: {
                              sol::table t = obj.as<sol::table>();
                              sol::table tcopy = target.create_table();
                              for (auto it = t.begin(); it != t.end(); ++it) {
                                        auto [key, val] = *it;
                                        auto tableKey = Copy(key, target);
                                        auto tableValue = Copy(val, target);
                                        if (tableValue.get_type() != sol::type::function) {
                                                  tcopy[tableKey] = tableValue;
                                        } else {
                                                  tcopy[tableKey].set_function(tableValue);
                                        }
                              }
                              return tcopy;
                    } break;
                    case sol::type::function: {
                              sol::function t = obj.as<sol::function>();
                              return sol::make_object(target, t);
                    } break;
                    case sol::type::userdata: {
                              //   if (obj.is<Jkr::Window>())
                              //             return sol::make_object(target, std::ref(obj.as<Jkr::Window>()));
                              //   else if (obj.is<DefaultCustomImagePainterPushConstant>())
                              //             return sol::make_object(target, std::ref(obj.as<DefaultCustomImagePainterPushConstant>()));
                              //   else if (obj.is<Jkr::Misc::RecycleBin<int>>())
                              //             return sol::make_object(target, std::ref(obj.as<Jkr::Misc::RecycleBin<int>>()));
                              //   else if (obj.is<Jkr::Renderer::Line>())
                              //             return sol::make_object(target, std::ref(obj.as<Jkr::Renderer::Line>()));
                              //   else if (obj.is<MultiThreading>())
                              //             return sol::make_object(target, std::ref(obj.as<MultiThreading>()));

                              return getObjectByType<Jkr::Window,
                                                     DefaultCustomImagePainterPushConstant,
                                                     Jkr::Misc::RecycleBin<int>,
                                                     Jkr::Renderer::Line,
                                                     MultiThreading,
                                                     ui>(obj, target); // the last arg doesn't work
                    } break;
                    default:
                              break;
          }
          return {};
}

inline sol::object MultiThreading::GetFull(sol::object inObject) { return Copy(inObject, mStates.front()); }

void CreateMultiThreadingBindings(sol::state& inState) {
          using namespace JkrEXE;
          auto Jkr = inState["Jkr"].get_or_create<sol::table>();

          auto TypeEnum = Jkr.new_enum<false>("AllTypes");
          TypeEnum["Window"] = AllTypes::Window;
          TypeEnum["DefaultCustomImagePainterPushConstant"] = AllTypes::DefaultCustomImagePainterPushConstant;
          TypeEnum["Misc_RecycleBin"] = AllTypes::Misc_RecycleBin;
          TypeEnum["Renderer_Line"] = AllTypes::Renderer_Line;
          TypeEnum["MultiThreading"] = AllTypes::MultiThreading;

          Jkr.new_usertype<MultiThreading>("MultiThreading",
                                           sol::call_constructor,
                                           sol::factories([](Jkr::Instance& inInstance) { return mu<MultiThreading>(inInstance); }),
                                           "Inject",
                                           &MultiThreading::Inject,
                                           "Get",
                                           &MultiThreading::Get,
                                           "InjectScript",
                                           &MultiThreading::InjectScript,
                                           "AddJob",
                                           &MultiThreading::AddJob,
                                           "AddJobF",
                                           &MultiThreading::AddJobF,
                                           "InjectScriptF",
                                           &MultiThreading::InjectScriptF,
                                           "GetFull", // TODO Remove this
                                           &MultiThreading::GetFull,
                                           "CastToType",
                                           &MultiThreading::CastToType);
}

} // namespace JkrEXE
