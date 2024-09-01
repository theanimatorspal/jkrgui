﻿#pragma once
#define SOL_PRINT_ERRORS 1
#define SOL_ALL_SAFETIES_ON 1
#include "glm/glm.hpp"
#include "ksai_config.hpp"
#include <sol/sol.hpp>
#include <Instance.hpp>

#ifdef ANDROID
#define JKR_EXPORT __attribute__((visibility("default")))
#else
#define JKR_EXPORT __declspec(dllexport)
#endif

sol::state &GetMainStateRef();
struct DefaultCustomImagePainterPushConstant {
    glm::vec4 x, y, z;
};

using namespace std;
using namespace ksai;

static auto my_exception_handler(lua_State *L,
                                 sol::optional<const std::exception &> maybe_execption,
                                 sol::string_view description) -> int {
    std::cout << "An Exception "
                 "has occured in "
                 "the function "
                 "here is what "
                 "it says: ";
    if (maybe_execption) {
        std::cout << "(Straight from the exception) ";
        const std::exception &ex = *maybe_execption;
        std::cout << ex.what() << std::endl;
    } else {
        std::cout << "(From the Descriptor Parameter) ";
        std::cout.write(description.data(), static_cast<std::streamsize>(description.size()));
        std::cout << std::endl;
    }
    return sol::stack::push(L, description);
};

namespace JkrEXE {
struct MultiThreading {
    MultiThreading(Jkr::Instance &inInstance);
    ~MultiThreading();
    /*
    If mainState is being accessed otherwise (which is almost always the case, lock the following
    mutex)
     */
    GETTER &GetMainAccessMutex() { return mMainMutex; }
    void Inject(std::string_view inVariable, sol::object inValue);
    void InjectToGate(std::string_view inVariable, sol::object inValue);
    void AddJobF(sol::function inFunction);
    void AddJobFIndex(sol::function inFunction, int inIndex);
    sol::object GetFromGate(std::string_view inVariable);
    sol::object GetFromGateToThread(std::string_view inVanriable, int inThreadId);

    void Wait();

    private:
    std::recursive_mutex mMainMutex;
    std::recursive_mutex mGateMutex;
    std::deque<std::recursive_mutex> mMutexes;
    std::condition_variable mConditionVariable;
    bool mIsInjecting = false;
    sol::object Copy(sol::object obj, sol::state &inTarget);
    ksai::ThreadPool mPool;
    std::vector<sol::state> mStates;
    sol::state mGateState;
    sol::state mGateStateTarget;
};
} // namespace JkrEXE