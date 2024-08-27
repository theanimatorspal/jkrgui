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

        while (true) {
            try {
                std::string line;
                std::string input;
                while (std::getline(std::cin, line)) {
                    input += line + '\n';
                }

                mainState.safe_script(input);

                if (std::cin.eof()) {
                    std::cin.clear();
                } else {
                    throw std::runtime_error("Input Error Occured");
                }
            } catch (const std::exception &e) {
                std::cout << "ERROR:: " << e.what() << "\n";
            }
        }
    }
}

JKR_EXPORT int main(int ArgCount, char **ArgStrings) {
#ifndef ANDROID
    auto CmdArg_Map = CommandLine(ArgCount, ArgStrings);
    if (not CmdArg_Map.empty()) ProcessCmdLine(ArgCount, ArgStrings);
#endif
    try {
        RunScript();
    } catch (const std::exception &e) {
        std::cout << "ERROR:: " << e.what() << "\n";
    }
    return 0;
}
