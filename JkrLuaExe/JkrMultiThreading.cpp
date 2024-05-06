#include "JkrLuaExe.hpp"
#include "Misc/ThreeD/World3D.hpp"
#include "sol/sol.hpp"
#include <mutex>

namespace JkrEXE {
extern void CreateMainBindings(sol::state& s);

struct MultiThreading {
    MultiThreading(Jkr::Instance& inInstance);
    ~MultiThreading();
    void Inject(std::string_view inVariable, sol::object inValue);
    void InjectToGate(std::string_view inVariable, sol::object inValue);
    void AddJobF(sol::function inFunction);
    sol::object GetFromGate(std::string_view inVariable);

    void Wait();

    private:
    std::mutex mMutex;
    std::deque<std::mutex> mMutexes;
    std::condition_variable mConditionVariable;
    bool mIsInjecting = false;
    sol::object Copy(sol::object obj, sol::state& inTarget);
    ksai::ThreadPool mPool;
    std::vector<sol::state> mStates;
    sol::state mGateState;
};

inline MultiThreading::MultiThreading(Jkr::Instance& inInstance) { // TODO Get Thread Count
    mPool.SetThreadCount(inInstance.GetThreadPool()
                              .mThreads.size()); // TODO don't do this, just GetThreadPoolSize();
    mStates.resize(inInstance.GetThreadPool().mThreads.size());
    mMutexes.resize(inInstance.GetThreadPool().mThreads.size());
    int i = 0;
    for (auto& state : mStates) {
        CreateMainBindings(state);
        state["StateId"] = i++;
    }
    CreateMainBindings(mGateState);
}

inline MultiThreading::~MultiThreading() { Wait(); }

inline void MultiThreading::InjectToGate(std::string_view inVariable, sol::object inValue) {
    std::scoped_lock<std::mutex> Lock(mMutex);
    mGateState[inVariable] = Copy(inValue, mGateState);
}

inline void MultiThreading::Inject(std::string_view inVariable, sol::object inValue) {
    for (int i = 0; i < mStates.size(); ++i) {
        std::scoped_lock<std::mutex> Lock1(mMutexes[i]);
        mStates[i][inVariable] = Copy(inValue, mStates[i]);
    }
}

inline sol::object MultiThreading::GetFromGate(std::string_view inVariable) {
    std::scoped_lock<std::mutex> Lock(mMutex);
    auto obj = Copy(mGateState[inVariable], GetMainStateRef());
    return obj;
}

inline void MultiThreading::AddJobF(sol::function inFunction) {
    std::scoped_lock<std::mutex> Lock(mMutex);
    int ThreadIndex = mPool.GetThreadIndex();
    auto& state     = mStates[ThreadIndex];
    auto f          = inFunction.dump();
    mPool.Add_Job([=, &state, this]() {
        std::scoped_lock<std::mutex> Lock2(mMutexes[ThreadIndex]);
        auto result = state.safe_script(f.as_string_view(), &sol::script_pass_on_error);
        if (not result.valid()) {
            sol::error error = result;
            std::cout << error.what();
            ksai_print(error.what());
        }
    });
}

inline void MultiThreading::Wait() { mPool.Wait(); }

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
         "InjectToGate",
         &MultiThreading::InjectToGate,
         "GetFromGate",
         &MultiThreading::GetFromGate,
         "Wait",
         &MultiThreading::Wait,
         "AddJobF",
         &MultiThreading::AddJobF);
}

} // namespace JkrEXE
