#include "JkrLuaExe.hpp"
#include "Misc/ThreeD/World3D.hpp"
#include "sol/sol.hpp"
#include <mutex>

namespace JkrEXE {
extern void CreateMainBindings(sol::state& s);

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
    std::mutex mMutex;
    std::condition_variable mConditionVariable;
    bool mIsInjecting = false;
    sol::object Copy(sol::object obj, sol::state& inTarget);
    ksai::ThreadPool mPool;
    std::vector<sol::state> mStates;
};

inline MultiThreading::MultiThreading(Jkr::Instance& inInstance) { // TODO Get Thread Count
    mPool.SetThreadCount(inInstance.GetThreadPool()
                              .mThreads.size()); // TODO don't do this, just GetThreadPoolSize();
    mStates.resize(inInstance.GetThreadPool().mThreads.size());
    int i = 0;
    for (auto& state : mStates) {
        CreateMainBindings(state);
        state["StateId"] = i++;
    }
}

inline void MultiThreading::Inject(std::string_view inVariable, sol::object inValue) {
    std::unique_lock<std::mutex> Lock(mMutex);
    mConditionVariable.wait(Lock, [this]() { return (not mIsInjecting); });
    mIsInjecting = true;
    for (auto& state : mStates) {
        state[inVariable] = Copy(inValue, state);
    }
    mIsInjecting = false;
    mConditionVariable.notify_all();
}

inline sol::object MultiThreading::Get(std::string_view inVariable) {
    return Copy(mStates.front()[inVariable], GetMainStateRef());
}

inline void MultiThreading::InjectScript(std::string inView) {
    std::unique_lock<std::mutex> Lock(mMutex);
    mConditionVariable.wait(Lock, [this]() { return (not mIsInjecting); });
    mIsInjecting = true;
    for (auto& state : mStates) {
        auto result = state.safe_script(inView);
        if (not result.valid()) {
            sol::error error = result;
            std::cout << error.what();
            ksai_print(error.what());
        }
    }
    mIsInjecting = false;
    mConditionVariable.notify_all();
}

inline void MultiThreading::AddJob(std::string inView) {
    std::unique_lock<std::mutex> Lock(mMutex);
    mConditionVariable.wait(Lock, [this]() { return (not mIsInjecting); });
    mIsInjecting    = true;
    auto& state     = mStates[mPool.GetThreadIndex()];
    int ThreadIndex = mPool.GetThreadIndex();
    mPool.Add_Job([=, &state]() {
        state["ThreadingId"] = ThreadIndex;
        auto result          = state.safe_script(inView, &sol::script_pass_on_error);
        if (not result.valid()) {
            sol::error error = result;
            std::cout << error.what();
            ksai_print(error.what());
        }
    });
    mIsInjecting = false;
    mConditionVariable.notify_all();
}

inline void MultiThreading::AddJobF(sol::function inFunction) {
    std::unique_lock<std::mutex> Lock(mMutex);
    mConditionVariable.wait(Lock, [this]() { return (not mIsInjecting); });
    mIsInjecting = true;
    auto& state  = mStates[mPool.GetThreadIndex()];
    auto f       = inFunction.dump();
    mPool.Add_Job([=, &state]() {
        auto result = state.safe_script(f.as_string_view(), &sol::script_pass_on_error);
        if (not result.valid()) {
            sol::error error = result;
            std::cout << error.what();
            ksai_print(error.what());
        }
    });
    mIsInjecting = false;
    mConditionVariable.notify_all();
}

inline void MultiThreading::InjectScriptF(sol::function inFunction) {
    std::unique_lock<std::mutex> Lock(mMutex);
    mConditionVariable.wait(Lock, [this]() { return (not mIsInjecting); });
    mIsInjecting = true;
    auto f       = inFunction.dump();
    for (auto& state : mStates) {
        auto result = state.safe_script(f.as_string_view(), &sol::script_pass_on_error);
        if (not result.valid()) {
            sol::error error = result;
            std::cout << error.what();
            ksai_print(error.what());
        }
    }
    mIsInjecting = false;
    mConditionVariable.notify_all();
}

template <typename T, typename... Ts>
inline static sol::object getObjectByType(sol::object& obj, sol::state& target) noexcept {

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
            sol::table t     = obj.as<sol::table>();
            sol::table tcopy = target.create_table();
            for (auto it = t.begin(); it != t.end(); ++it) {
                auto [key, val] = *it;
                auto tableKey   = Copy(key, target);
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
                                   Jkr::WindowMulT,
                                   MultiThreading,

                                   DefaultCustomImagePainterPushConstant,
                                   Jkr::Misc::RecycleBin<int>,
                                   Jkr::Misc::_3D::Camera3D,
                                   Jkr::Misc::_3D::World3D,
                                   Jkr::Misc::_3D::World3D::Object3D,
                                   std::vector<Jkr::Misc::_3D::World3D::Object3D>&,
                                   Jkr::Misc::_3D::Uniform3D,

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

void CreateMultiThreadingBindings(sol::state& inState) {
    using namespace JkrEXE;
    auto Jkr = inState["Jkr"].get_or_create<sol::table>();

    Jkr.new_usertype<MultiThreading>(
         "MultiThreading",
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
    auto f      = inFunction.dump();
    mPool.Add_JobToThread([=, &state]() { state.script(f.as_string_view()); }, inThreadIndex);
}
} // namespace JkrEXE
