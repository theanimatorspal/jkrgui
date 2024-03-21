#include "Instance.hpp"
#include "JkrLuaExe.hpp"
#include "Renderers/TwoD/Line.hpp"
#include "Window.hpp"
#include "ksai_thread.hpp"
#include <Misc/RecycleBin.hpp>
#include <Renderers/TwoD/Shape.hpp>
#include <functional>
#include <memory>
#include <sol/sol.hpp>
#include <vector>

namespace JkrEXE {
extern void CreateMainBindings(sol::state& s);

struct DefaultCustomImagePainterPushConstant {
          glm::vec4 x, y, z;
};

struct MultiThreading {
          MultiThreading(Jkr::Instance& inInstance);
          void Inject(std::string_view inVariable, sol::object inValue);
          void InjectScript(std::string inView);
          void AddJob(std::string inView);
          void InjectScriptF(sol::function inFunction);
          void AddJobF(sol::function inFunction);
          sol::object Get(std::string_view inVariable);

      private:
          sol::object Copy(sol::object obj, sol::state& inTarget);
          ksai::ThreadPool& mPool;
          std::vector<sol::state> mStates;
};

void CreateMiscBindings(sol::state& inState) {
          auto Jkr = inState["Jkr"].get_or_create<sol::table>();

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
                                           &MultiThreading::InjectScriptF);

          Jkr.new_usertype<Jkr::Misc::RecycleBin<int>>("RecycleBin",
                                                       sol::call_constructor,
                                                       sol::factories([]() { return Jkr::Misc::RecycleBin<int>(); }),
                                                       "Add",
                                                       &Jkr::Misc::RecycleBin<int>::Add,
                                                       "Remove",
                                                       &Jkr::Misc::RecycleBin<int>::Remove,
                                                       "Exists",
                                                       &Jkr::Misc::RecycleBin<int>::Exists,
                                                       "IsEmpty",
                                                       &Jkr::Misc::RecycleBin<int>::IsEmpty,
                                                       "Get",
                                                       &Jkr::Misc::RecycleBin<int>::Get);

          Jkr.new_usertype<Jkr::Renderer::CustomPainterImage>(
           "CustomPainterImage",
           sol::call_constructor,
           sol::factories([](Jkr::Instance& inInstance, Jkr::Window& inWindow, int inWidth, int inHeight) {
                     return mu<Jkr::Renderer::CustomPainterImage>(inInstance, inWindow, inWidth, inHeight);
           }),
           "GetImageToVector",
           &Jkr::Renderer::CustomPainterImage::GetImageToVector);

          Jkr.new_usertype<DefaultCustomImagePainterPushConstant>("DefaultCustomImagePainterPushConstant",
                                                                  sol::call_constructor,
                                                                  sol::factories([]() { return DefaultCustomImagePainterPushConstant(); }),
                                                                  "x",
                                                                  &DefaultCustomImagePainterPushConstant::x,
                                                                  "y",
                                                                  &DefaultCustomImagePainterPushConstant::y,
                                                                  "z",
                                                                  &DefaultCustomImagePainterPushConstant::z);

          Jkr.new_usertype<Jkr::Renderer::CustomImagePainter>("CustomImagePainter",
                                                              sol::call_constructor,
                                                              sol::factories([](sv inCacheFileName, sv inComputeShader) {
                                                                        return mu<Jkr::Renderer::CustomImagePainter>(inCacheFileName,
                                                                                                                     inComputeShader);
                                                              }),
                                                              "Load",
                                                              &Jkr::Renderer::CustomImagePainter::Load,
                                                              "Store",
                                                              &Jkr::Renderer::CustomImagePainter::Store,
                                                              "Bind",
                                                              &Jkr::Renderer::CustomImagePainter::Bind,
                                                              "BindImageFromImage",
                                                              &Jkr::Renderer::CustomImagePainter::BindImageFromImage,
                                                              "Draw",
                                                              &Jkr::Renderer::CustomImagePainter::Draw<DefaultCustomImagePainterPushConstant>);
}

inline MultiThreading::MultiThreading(Jkr::Instance& inInstance) : mPool(inInstance.GetThreadPool()) {
          mStates.resize(inInstance.GetThreadPool().mThreads.size());
          int i = 0;
          for (auto& state : mStates) {
                    CreateMainBindings(state);
                    state["ThreadingId"] = i++;
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
                                        tcopy.set(Copy(key, target), Copy(val, target));
                              }
                              return tcopy;
                    } break;
                    case sol::type::function: {
                              sol::function t = obj.as<sol::function>();
                              return sol::make_object(target, t);
                    } break;
                    case sol::type::userdata: {
                              if (obj.is<Jkr::Window>()) {
                                        return sol::make_object(target, std::ref(obj.as<Jkr::Window>()));
                              } else if (obj.is<DefaultCustomImagePainterPushConstant>()) {
                                        return sol::make_object(target, std::ref(obj.as<DefaultCustomImagePainterPushConstant>()));
                              } else if (obj.is<Jkr::Misc::RecycleBin<int>>()) {
                                        return sol::make_object(target, std::ref(obj.as<Jkr::Misc::RecycleBin<int>>()));
                              } else if (obj.is<Jkr::Renderer::Line>()) {
                                        return sol::make_object(target, std::ref(obj.as<Jkr::Renderer::Line>()));
                              }
                    } break;
                    default:
                              break;
          }
          return {};
}

} // namespace JkrEXE