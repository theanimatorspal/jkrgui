#pragma once
#define SOL_PRINT_ERRORS 1
#define SOL_ALL_SAFETIES_ON 1
#include "glm/glm.hpp"
#include "ksai_config.hpp"
#include <sol/sol.hpp>
#include <Instance.hpp>

#ifdef ANDROID
#define JKR_EXPORT __attribute__((visibility("default")))
#elif __APPLE__
#define JKR_EXPORT __attribute__((visibility("default")))
#else
#define JKR_EXPORT __declspec(dllexport)
#endif

sol::state &GetMainStateRef();
struct DefaultCustomImagePainterPushConstant {
      glm::vec4 x, y, z;
};
struct Matrix2CustomImagePainterPushConstant {
      glm::mat4 a, b;
};

using namespace std;
using namespace ksai;

static auto my_exception_handler(lua_State *L,
                                 sol::optional<const std::exception &> maybe_execption,
                                 sol::string_view description) -> int {
      if (maybe_execption) {
            const std::exception &ex = *maybe_execption;
            Log(ex.what(), "ERROR");
      } else {
            Log(description, "ERROR");
      }
      return sol::stack::push(L, description);
};

namespace JkrEXE {
struct MultiThreading {
      MultiThreading(Jkr::Instance &inInstance);
      ~MultiThreading();

      /// @brief Injects to gate only
      /// in all threads, all multithreaded objects should be returned by
      /// Get function

      void Inject(std::string_view inVariable, sol::object inValue);
      sol::object Get(std::string_view inVanriable, int inThreadId);
      void AddJobF(sol::function inFunction);
      void AddJobFIndex(sol::function inFunction, int inIndex);
      void ExecuteAll(sol::function inFunction);

      void Wait();

      private:
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

namespace BuildSystem {
void CreateAndroidEnvironment(const sv inAndroidAppName      = "JkrGUIv2",
                              const sv inAndroidAppDirectory = "android",
                              const sv inBuild               = "android-arm64-v8a",
                              const sv inArchitecture        = "arm64-v8a");

void CreateLuaLibraryEnvironment(sv inLibraryName,
                                 sv inNativeDestinationDirectory,
                                 bool inOverride);

void Bundle(const std::string &outFileName);
} // namespace BuildSystem