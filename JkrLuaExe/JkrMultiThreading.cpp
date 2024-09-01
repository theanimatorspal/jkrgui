#include "JkrLuaExe.hpp"
#include "Misc/RecycleBin.hpp"
#include "Renderers/ThreeD/World3D.hpp"
#include "Renderers/Generator.hpp"
#include "Renderers/Renderer_base.hpp"
#include "Renderers/TwoD/Line.hpp"
#include "Renderers/TwoD/Shape.hpp"
#include "Renderers/BestText_Alt.hpp"
#include "Window.hpp"
#include "sol/sol.hpp"
#include <Misc/RecycleBin.hpp>
#include <Renderers/ThreeD/World3D.hpp>
#include <Renderers/TwoD/Shape.hpp>
#include <Window.hpp>
#include <mutex>

extern sol::state &GetMainStateRef();
namespace JkrEXE {
using namespace Jkr;
extern void CreateMainBindings(sol::state &s);

MultiThreading::MultiThreading(Jkr::Instance &inInstance) { // TODO Get Thread Count
    mPool.SetThreadCount(inInstance.GetThreadPool()
                              .mThreads.size()); // TODO don't do this, just GetThreadPoolSize();
    mStates.resize(inInstance.GetThreadPool().mThreads.size());
    mMutexes.resize(inInstance.GetThreadPool().mThreads.size());
    int i = 0;
    for (auto &state : mStates) {
        CreateMainBindings(state);
        state["StateId"] = i++;
    }
    GetMainStateRef()["StateId"] = -1;
    CreateMainBindings(mGateState);
    CreateMainBindings(mGateStateTarget);
}

MultiThreading::~MultiThreading() { Wait(); }

void MultiThreading::Inject(std::string_view inVariable, sol::object inValue) {
    std::scoped_lock<std::recursive_mutex> gatelock(mGateMutex);
    mGateState[inVariable] = Copy(inValue, mGateState);
}

sol::object MultiThreading::Get(std::string_view inVariable, int inThreadId) {
    std::scoped_lock<std::recursive_mutex> Lock(mGateMutex);
    sol::object obj;
    if (inThreadId != -1) {
        std::scoped_lock<std::recursive_mutex> mThreadMutex(mMutexes[inThreadId]);
        obj = Copy(mGateState[inVariable], mStates[inThreadId]);
    } else {
        obj = Copy(mGateState[inVariable], GetMainStateRef());
    }
    return obj;
}

void MultiThreading::AddJobF(sol::function inFunction) {
    int ThreadIndex = mPool.GetThreadIndex();
    auto f          = inFunction.dump();
    mPool.Add_Job([=, this]() {
        std::scoped_lock<std::recursive_mutex> Lock2(mMutexes[ThreadIndex]);
        auto &state = mStates[ThreadIndex];
        auto result = state.safe_script(f.as_string_view(), &sol::script_pass_on_error);
        if (not result.valid()) {
            sol::error error = result;
            std::cout << error.what();
            ksai_print(error.what());
        }
    });
}

void MultiThreading::AddJobFIndex(sol::function inFunction, int inIndex) {
    int ThreadIndex = inIndex;
    auto f          = inFunction.dump();
    mPool.Add_JobToThread(
         [=, this]() {
             std::scoped_lock<std::recursive_mutex> Lock2(mMutexes[ThreadIndex]);
             auto &state = mStates[ThreadIndex];
             auto result = state.safe_script(f.as_string_view(), &sol::script_pass_on_error);
             if (not result.valid()) {
                 sol::error error = result;
                 std::cout << error.what();
                 ksai_print(error.what());
             }
         },
         ThreadIndex);
}

void MultiThreading::Wait() { mPool.Wait(); }

template <typename T, typename... Ts>
static sol::object getObjectByType(sol::object &obj, sol::state &target) noexcept {
    if constexpr (not(sizeof...(Ts) == 0)) {
        if (obj.is<T>()) {
            return sol::make_object(target, std::ref(obj.as<T>()));
        } else {
            return getObjectByType<Ts...>(obj,
                                          target); // TODO FIX last argument doesn't work template
        }
    }
    return {};
}

sol::object MultiThreading::Copy(sol::object obj, sol::state &target) {
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
            sol::table t     = obj.as<sol::table>();
            sol::table tcopy = target.create_table();
            for (auto it = t.begin(); it != t.end(); ++it) {
                auto [key, val] = *it;
                auto tableKey   = Copy(key, target);
                auto tableValue = Copy(val, target);
                tcopy[tableKey] = tableValue;
            }
            return tcopy;
        } break;
        case sol::type::function: {
            sol::function t     = obj.as<sol::function>();
            sol::object DumpedF = target.load(t.dump().as_string_view());
            return sol::make_object(target, DumpedF);
        } break;
        case sol::type::userdata: {
            return getObjectByType<glm::vec2,
                                   glm::vec3,
                                   glm::vec4,
                                   glm::uvec2,
                                   glm::mat4,
                                   Jkr::Instance,
                                   Jkr::Window,
                                   Jkr::EventManager,
                                   MultiThreading,

                                   DefaultCustomImagePainterPushConstant,
                                   Jkr::Misc::RecycleBin<int>,
                                   Jkr::Renderer::_3D::Camera3D,
                                   Jkr::Renderer::_3D::World3D,
                                   Jkr::Renderer::_3D::World3D::Object3D,
                                   std::vector<Jkr::Renderer::_3D::World3D::Object3D> &,
                                   Jkr::Renderer::_3D::Uniform3D,

                                   Jkr::Renderer::Shape,
                                   Jkr::Renderer::Line,
                                   Jkr::Renderer::BestText_Alt,
                                   Jkr::Renderer::CustomImagePainter,

                                   Jkr::Renderer::Line,
                                   Jkr::Renderer::_3D::Shape,
                                   Jkr::Renderer::_3D::glTF_Model,
                                   Jkr::Renderer::_3D::Simple3D,

                                   ui>(obj, target); // the last arg doesn't work
        } break;
        default:
            break;
    }
    return {};
}

void CreateMultiThreadingBindings(sol::state &inState) {
    using namespace JkrEXE;
    auto Jkr = inState["Jkr"].get_or_create<sol::table>();

    Jkr.new_usertype<MultiThreading>(
         "MultiThreading",
         sol::call_constructor,
         sol::factories([](Jkr::Instance &inInstance) { return mu<MultiThreading>(inInstance); }),
         "Inject",
         &MultiThreading::Inject,
         "Get",
         &MultiThreading::Get,
         "Wait",
         &MultiThreading::Wait,
         "AddJobF",
         &MultiThreading::AddJobF,
         "AddJobFIndex",
         &MultiThreading::AddJobFIndex);
}

} // namespace JkrEXE
