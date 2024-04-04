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

static sol::state mainState;
sol::state& GetMainStateRef() { return mainState; }

int main(int ArgCount, char** ArgStrings) {
          using namespace JkrEXE;
          auto CmdArg_Map = CommandLine(ArgCount, ArgStrings);
          if (not CmdArg_Map.empty()) {
                    if (CmdArg_Map.contains("--build")) { // C++20 ma matrai
                              sol::state s;
                              s.open_libraries();
                              CreateBuildSystemBindings(s);
                              sol::protected_function_result result = s.safe_script_file("build.lua", &sol::script_pass_on_error);
                              if (not result.valid()) {
                                        sol::error error = result;
                                        std::cout << error.what();
                                        exit(EXIT_FAILURE);
                              }
                    }
          } else {
                    CreateMainBindings(mainState);
                    const vector<string_view> CommandLineArgs(ArgStrings + 1, ArgStrings + ArgCount);
                    mainState.set_exception_handler(&my_exception_handler);
                    sol::protected_function_result result = mainState.safe_script_file("app.lua", &sol::script_pass_on_error);
                    if (not result.valid()) {
                              sol::error error = result;
                              std::cout << error.what();
                              exit(EXIT_FAILURE);
                    }
          }
          return 0;
}
