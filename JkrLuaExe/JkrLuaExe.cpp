﻿#include "ksai_config.hpp"
#include <filesystem>
#define SOL_PRINT_ERRORS 1
#define SOL_ALL_SAFETIES_ON 1
#include "JkrLuaExe.hpp"
#include "sol/sol.hpp"

namespace JkrEXE {
extern void CreateBasicBindings(sol::state& inState);
extern void CreateGLMBindings(sol::state& inState);
extern void CreateKeyBindings(sol::state& inState);
extern void CreateMiscBindings(sol::state& inState);
extern void CreateRendererBindings(sol::state& inState);
extern void CreateTextRendererBindings(sol::state& inState);
extern void CreateBuildSystemBindings(sol::state& inS);
extern void CreateRenderer3DBindings(sol::state& s);
extern umap<s, v<s>> CommandLine(int ArgCount, char** ArgStrings); // TODO Complete This
extern void CreateMultiThreadingBindings(sol::state& inState);

void CreateMainBindings(sol::state& s) {
          s.open_libraries();
#ifdef _WIN32
          s["_WIN32"] = true;
#elif APPLE
          s["APPLE"] = true;
#elif ANDROID
          s["ANDROID"] = true;
#endif
          CreateGLMBindings(s);
          CreateKeyBindings(s);
          CreateBasicBindings(s);
          CreateRendererBindings(s);
          CreateTextRendererBindings(s);
          CreateMiscBindings(s);
          CreateMultiThreadingBindings(s);
          CreateRenderer3DBindings(s);
}
} // namespace JkrEXE

using namespace JkrEXE;
static sol::state mainState;
sol::state& GetMainStateRef() { return mainState; }

void RunScript() {
          CreateMainBindings(mainState);
          mainState.set_exception_handler(&my_exception_handler);
          sol::protected_function_result result = mainState.safe_script_file("app.lua", &sol::script_pass_on_error);
          if (not result.valid()) {
                    sol::error error = result;
                    std::cout << error.what();
                    ksai_print(error.what());
          }
}

void ProcessCmdLine(auto& inCmdLineArg_Map) {
          if (inCmdLineArg_Map.contains("--build")) {
                    sol::state s;
                    s.open_libraries();
                    CreateBuildSystemBindings(s);
                    sol::protected_function_result result = s.safe_script_file("build.lua", &sol::script_pass_on_error);
                    if (not result.valid()) {
                              sol::error error = result;
                              std::cout << error.what();
                              ksai_print(error.what());
                    }
          }
}

JKR_EXPORT int main(int ArgCount, char** ArgStrings) {
#ifdef ANDROID
          ksai_print("Main Function Entered");
          std::filesystem::current_path("/data/data/com.AndroidApp/"); // TODO Do this in Lua, not in C++
#endif

#ifndef ANDROID
          auto CmdArg_Map = CommandLine(ArgCount, ArgStrings);
          if (not CmdArg_Map.empty())
                    ProcessCmdLine(CmdArg_Map);
          else
#endif
                    RunScript();
          return 0;
}
