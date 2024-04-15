#include "JkrLuaExe.hpp"

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
          /* TODO Remove these functions */
          void InjectScript(std::string inView);
          void AddJob(std::string inView);

          void InjectScriptF(sol::function inFunction);
          void AddJobF(sol::function inFunction);
          // TODO Make Lua Binding of this function
          void AddJobToThreadF(sol::function inFunction, int inThreadIndex);
          sol::object Get(std::string_view inVariable);

          void Wait();

      private:
          sol::object Copy(sol::object obj, sol::state& inTarget);
          ksai::ThreadPool mPool;
          std::vector<sol::state> mStates;
};

inline MultiThreading::MultiThreading(Jkr::Instance& inInstance) {
          mPool.SetThreadCount(inInstance.GetThreadPool().mThreads.size()); // TODO don't do this, just GetThreadPoolSize();
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
                    state.safe_script(inView);
          }
}

inline void MultiThreading::AddJob(std::string inView) {
          auto& state = mStates[mPool.GetThreadIndex()];
          int ThreadIndex = mPool.GetThreadIndex();
          mPool.Add_Job([=, &state]() {
                    state["ThreadingId"] = ThreadIndex;
                    state.safe_script(inView);
          });
}

inline void MultiThreading::AddJobF(sol::function inFunction) {
          auto& state = mStates[mPool.GetThreadIndex()];
          auto f = inFunction.dump();
          mPool.Add_Job([=, &state]() { state.safe_script(f.as_string_view()); });
}

inline void MultiThreading::InjectScriptF(sol::function inFunction) {
          auto f = inFunction.dump();
          for (auto& state : mStates) {
                    state.safe_script(f.as_string_view());
          }
}

inline static sol::object GetObjectByType(sol::object obj, sol::state& target, AllTypes inType) noexcept {
          if (inType == AllTypes::Window) {
                    return sol::make_object(target, std::ref(obj.as<Jkr::Window>()));
          } else if (inType == AllTypes::DefaultCustomImagePainterPushConstant) {
                    return sol::make_object(target, std::ref(obj.as<DefaultCustomImagePainterPushConstant>()));
          } else if (inType == AllTypes::Misc_RecycleBin) {
                    return sol::make_object(target, std::ref(obj.as<Jkr::Misc::RecycleBin<int>>()));
          } else if (inType == AllTypes::Renderer_Line) {
                    return sol::make_object(target, std::ref(obj.as<Jkr::Renderer::Renderer::Line>()));
          } else if (inType == AllTypes::MultiThreading) {
                    return sol::make_object(target, std::ref(obj.as<MultiThreading>()));
          } else {
                    return {};
          }
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
                              return getObjectByType<Jkr::Instance,
                                                     Jkr::WindowMulT,
                                                     MultiThreading,

                                                     DefaultCustomImagePainterPushConstant,
                                                     Jkr::Misc::RecycleBin<int>,

                                                     Jkr::Renderer::Line,
                                                     Jkr::Renderer::_3D::Shape,
                                                     Jkr::Renderer::_3D::glTF_Model,
                                                     Jkr::Renderer::_3D::Simple3D,

                                                     Jkr::Misc::_3D::Uniform3D,
                                                     ui>(obj, target); // the last arg doesn't work
                    } break;
                    default:
                              break;
          }
          return {};
}

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
                                           "Wait",
                                           &MultiThreading::Wait);
}

inline void MultiThreading::Wait() { mPool.Wait(); }

inline void MultiThreading::AddJobToThreadF(sol::function inFunction, int inThreadIndex) {
          auto& state = mStates[inThreadIndex];
          auto f = inFunction.dump();
          mPool.Add_JobToThread([=, &state]() { state.script(f.as_string_view()); }, inThreadIndex);
}
} // namespace JkrEXE
