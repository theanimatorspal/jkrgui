#include "Instance.hpp"
#include "ksai_thread.hpp"
#include <Misc/RecycleBin.hpp>
#include <Renderers/TwoD/Shape.hpp>
#include <sol/sol.hpp>
#include <vector>

namespace JkrEXE {
using namespace ksai;
extern void CreateMainBindings(sol::state& s);
struct MultiThreading {
          MultiThreading(Jkr::Instance& inInstance);
          void Inject(std::string_view inVariable, sol::object inValue);
          void InjectScript(std::string_view inView);
          void AddJob(std::string inView);
          sol::object Get(std::string_view inVariable);

      private:
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
                                           &MultiThreading::AddJob);

          Jkr.new_usertype<Jkr::Misc::Recycle<int>>("RecycleBin",
                                                    sol::call_constructor,
                                                    sol::factories([]() { return Jkr::Misc::Recycle<int>(); }),
                                                    "Add",
                                                    &Jkr::Misc::Recycle<int>::Add,
                                                    "Remove",
                                                    &Jkr::Misc::Recycle<int>::Remove,
                                                    "Exists",
                                                    &Jkr::Misc::Recycle<int>::Exists,
                                                    "IsEmpty",
                                                    &Jkr::Misc::Recycle<int>::IsEmpty,
                                                    "Get",
                                                    &Jkr::Misc::Recycle<int>::Get);

          Jkr.new_usertype<Jkr::Renderer::CustomPainterImage>(
           "CustomPainterImage",
           sol::call_constructor,
           sol::factories([](Jkr::Instance& inInstance, Jkr::Window& inWindow, int inWidth, int inHeight) {
                     return mu<Jkr::Renderer::CustomPainterImage>(inInstance, inWindow, inWidth, inHeight);
           }),
           "GetImageToVector",
           &Jkr::Renderer::CustomPainterImage::GetImageToVector);

          struct DefaultCustomImagePushConstant {
                    glm::vec4 x, y, z;
          };

          // Jkr.new_usertype<DefaultCustomImagePushConstant>(
          //  "DefaultCustomImagePushConstant", "x", &DefaultCustomImagePushConstant::x, "y",
          //  &DefaultCustomImagePushConstant::y, "z", &DefaultCustomImagePushConstant::z);

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
                                                              &Jkr::Renderer::CustomImagePainter::Draw<DefaultCustomImagePushConstant>);
}

inline MultiThreading::MultiThreading(Jkr::Instance& inInstance) : mPool(inInstance.GetThreadPool()) {
          mStates.resize(inInstance.GetThreadPool().mThreads.size());
          for (auto& state : mStates) {
                    CreateMainBindings(state);
          }
}

inline void MultiThreading::Inject(std::string_view inVariable, sol::object inValue) {
          for (auto& state : mStates) {
                    state[inVariable] = inValue;
          }
}

inline sol::object MultiThreading::Get(std::string_view inVariable) { return mStates.front()[inVariable]; }

inline void MultiThreading::InjectScript(std::string_view inView) {
          for (auto& state : mStates) {
                    state.script(inView);
          }
}

inline void MultiThreading::AddJob(std::string inView) {
          auto& state = mStates[mPool.GetThreadIndex()];
          mPool.Add_Job([=, &state]() {
                    auto out = state.script(inView);
                    if (not out.valid()) {
                              std::cout << "Not VALID";
                    }
                    std::cout << "Invoked";
          });
}

} // namespace JkrEXE