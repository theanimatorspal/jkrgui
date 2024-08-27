#include "JkrLuaExe.hpp"
#include <SDLWindow.hpp>
#include <CLI11/CLI11.hpp>
// #include <Tracy.hpp>
// #include <TracyLua.hpp>

extern void LuaShowToastNotification(const std::string_view inMessage);

namespace JkrEXE {
extern void CreateBasicBindings(sol::state &inState);
extern void CreateGLMBindings(sol::state &inState);
extern void CreateKeyBindings(sol::state &inState);
extern void CreateMiscBindings(sol::state &inState);
extern void CreateRendererBindings(sol::state &inState);
extern void CreateTextRendererBindings(sol::state &inState);
extern void CreateBuildSystemBindings(sol::state &inS);
extern void CreateRenderer3DBindings(sol::state &s);
extern void CreateAudioBindings(sol::state &inState);
extern void CreatePlatformBindings(sol::state &inS);
extern umap<s, v<s>> CommandLine(int ArgCount, char **ArgStrings); // TODO Complete This
extern void CreateMultiThreadingBindings(sol::state &inState);
extern void CreateNetworkBindings(sol::state &s);

void CreateMainBindings(sol::state &s) {
    s.set_exception_handler(&my_exception_handler);
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
    CreateAudioBindings(s);
    CreatePlatformBindings(s);
    CreateNetworkBindings(s);
    // tracy::LuaRegister(s);
}
} // namespace JkrEXE

using namespace JkrEXE;
static sol::state mainState;
sol::state &GetMainStateRef() { return mainState; }

void RunScript() {
    CreateMainBindings(mainState);

#ifdef ANDROID
    ksai_print("Main Function Entered");
    std::filesystem::current_path("/data/data/com.SampraharReturns/");
    std::filesystem::current_path("/storage/emulated/0/Download/jkrgui");

#endif

    sol::protected_function_result result =
         mainState.safe_script_file("app.lua", &sol::script_pass_on_error);
    if (not result.valid()) {
        sol::error error = result;
        std::cout << error.what();
        ksai_print(error.what());
        LuaShowToastNotification(error.what());
    }
}

void ProcessCmdLine(int ArgCount, char **ArgStrings) {
    CLI::App app;
    bool FlagBuild       = false;
    bool FlagGenerate    = false;
    bool FlagGenerateRun = false;
    bool FlagRepl        = false;
    app.add_flag("--b,--build", FlagBuild);
    app.add_flag("--g,--generate", FlagGenerate);
    app.add_flag("--gr,--generate-run", FlagGenerateRun);
    app.add_flag("--r, --repl", FlagRepl);

    app.parse(ArgCount, ArgStrings);

    if (FlagBuild) {
        sol::state s;
        s.open_libraries();
        CreateBuildSystemBindings(s);
        sol::protected_function_result result =
             s.safe_script_file("build.lua", &sol::script_pass_on_error);
        if (not result.valid()) {
            sol::error error = result;
            std::cout << error.what();
            ksai_print(error.what());
        }
        exit(0);
    }
    auto Update = []() {
        filesystem::path src = std::string(getenv("JKRGUI_DIR"));
        src /= "luaproject";
        filesystem::path dest = filesystem::current_path();
        std::cout << "Current Directory:" << filesystem::current_path() << "\n";
        std::cout << "SRC:" << src << "\n";
        std::cout << "DEST:" << dest << "\n";
        filesystem::copy(src,
                         dest,
                         filesystem::copy_options::recursive |
                              filesystem::copy_options::update_existing);
    };
    if (FlagGenerate) {
        Update();
        exit(0);
    }
    if (FlagGenerateRun) {
        Update();
    }
    if (FlagRepl) {
        CreateMainBindings(mainState);

        auto OldCurrentPath = std::filesystem::current_path();
        std::filesystem::current_path(std::filesystem::path(getenv("JKRGUI_DIR")) / "luaproject");
        mainState.safe_script(R"""(
        require("JkrGUIv2.repl")
)""");
        std::system("cls");
        auto CurrentPath = std::filesystem::current_path();
        std::filesystem::current_path(OldCurrentPath);

        using namespace std;
        while (true) {
            try {
                vector<bool> scope;
                string line;
                string input;
                cout << "[JKRGUI v2.0a]>> ";

                while (getline(cin, line)) {
                    if (line.find("function") != string::npos or line.find("if") != string::npos or
                        line.find("for") != string::npos or line.find("while") != string::npos or
                        line.find("(") != string::npos or line.find("{") != string::npos) {
                        //
                        scope.push_back(true);
                        //
                    }
                    if (line.find("end") != string::npos or line.find(")") != string::npos or
                        line.find("}") != string::npos) {
                        scope.pop_back();
                    }
                    input += line + '\n';

                    // ALL CUSTOM COMMANDS
                    if (line == "EXIT()" or line == "exit") {
                        exit(0);
                    } else if (line == "CLEAR()" or line == "cls" or line == "clear") {
                        // TODO Make this cross platform
                        std::system("cls");
                        break;
                    }

                    if (scope.empty()) {
                        mainState.safe_script(input);
                        std::cout << "\n";
                        break;
                    }
                }
            } catch (const std::exception &e) {
                std::cout << "ERROR:: " << e.what() << "\n";
            }
        }
    }
}

JKR_EXPORT int main(int ArgCount, char **ArgStrings) {
#ifndef ANDROID
    ProcessCmdLine(ArgCount, ArgStrings);
#endif
    try {
        RunScript();
    } catch (const std::exception &e) {
        std::cout << "ERROR:: " << e.what() << "\n";
    }
    return 0;
}
