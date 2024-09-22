#define TRACY_IMPORTS
#include "EventManager.hpp"
#include "JkrLuaExe.hpp"
#include <SDLWindow.hpp>
#include <CLI11/CLI11.hpp>
#ifndef ANDROID
#ifndef __APPLE__
#include <TracyLua.hpp>
#endif
#else
#include "LuaBundleAndroid.hpp"
#endif

#include "LuaBundleAndroid.hpp"

// For Command Line Stuff
bool FlagBuild                        = false;
bool FlagGenerate                     = false;
bool FlagGenerateRun                  = false;
bool FlagRepl                         = false;
bool FlagCreateAndroidEnvironment     = false;
std::string OptionAndroidAppName      = "JkrGUIv2";
std::string OptionAndroidDirName      = "android";
std::string OptionAndroidBuildDirName = "android-arm64-v8a";
std::string OptionRun                 = "app.lua";

extern void LuaShowToastNotification(const std::string inMessage);

namespace JkrEXE {
extern void CreateBasicBindings(sol::state &inState);
extern void CreateGLMBindings(sol::state &inState);
extern void CreateKeyBindings(sol::state &inState);
extern void CreateMiscBindings(sol::state &inState);
extern void CreateRendererBindings(sol::state &inState);
extern void CreateTextRendererBindings(sol::state &inState);
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
#ifndef ANDROID
#ifndef __APPLE__
    tracy::LuaRegister(s);
#endif
#endif
}
} // namespace JkrEXE

using namespace JkrEXE;
static sol::state mainState;
sol::state &GetMainStateRef() { return mainState; }

void RunScript() {
    CreateMainBindings(mainState);

#ifdef ANDROID
    ksai_print("========================Main Function Entered=============================");
    sol::protected_function_result result =
         mainState.safe_script(LuaBundleScript, &sol::script_pass_on_error);
    if (not result.valid()) {
        sol::error error = result;
        std::cout << error.what();
        ksai_print(std::string_view("KSAI::ERROR:%s"), error.what());
        LuaShowToastNotification(std::string(error.what()));
    }
#endif
    if (OptionRun.find(".lua") == s::npos) {
        OptionRun = OptionRun + ".lua";
    }
    sol::protected_function_result result_ =
         mainState.safe_script_file(OptionRun, &sol::script_pass_on_error);
    if (not result_.valid()) {
        sol::error error = result_;
        std::cout << error.what();
        ksai_print(std::string_view("KSAI::ERROR:%s"), error.what());
        LuaShowToastNotification(std::string(error.what()));
    }
}

static bool IsBoundary(char c) { return std::isspace(c) or std::ispunct(c) or c == '\0'; }

int c(const std::string &str, const std::string &sub) {

    if (sub.length() == 0) return 0;
    int count = 0;

    for (int offset = str.find(sub); offset != std::string::npos;) {
        if (isalpha(sub[0])) {
            int left = (offset - 1 <= 0) ? 0 : (offset - 1);
            int right =
                 ((offset + sub.length()) >= str.length()) ? str.length() : (offset + sub.length());

            bool left_alright  = (left == 0) or IsBoundary(str[left]);
            bool right_alright = (right == str.length()) or IsBoundary(str[right]);
            count              = count + (left_alright and right_alright);
        } else {
            ++count;
        }

        offset = str.find(sub, offset + sub.length());
    }
    return count;
}

void ProcessCmdLine(int ArgCount, char **ArgStrings) {
    CLI::App app;
    app.add_flag("--g,--generate", FlagGenerate);
    app.add_flag("--gr,--generate-run", FlagGenerateRun);
    app.add_flag("--r, --repl", FlagRepl);
    app.add_flag("--android-environment,--and-env", FlagCreateAndroidEnvironment);
    ///
    /// @brief These options are string options
    /// @warning Use "" for giving them in the command line,
    /// otherwise it won't recognize the name
    ///
    app.add_option("--appname", OptionAndroidAppName, "Android App Name");
    app.add_option("--dirname", OptionAndroidDirName, "Android Dir Name");
    app.add_option("--builddir", OptionAndroidBuildDirName, "Android Build Directory Name");
    app.add_option("--run", OptionRun, "Run File, Defaults to app.lua");

    app.parse(ArgCount, ArgStrings);

    if (FlagCreateAndroidEnvironment) {
        BuildSystem::CreateAndroidEnvironment(
             OptionAndroidAppName, OptionAndroidDirName, OptionAndroidBuildDirName);
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

        std::deque<std::string> mainThreadStatements;
        std::recursive_mutex mutex;
        using namespace std;

        sol::object e_obj = mainState["e"];
        auto e            = e_obj.as<Jkr::EventManager *>();

        bool shouldQuit   = false;

        auto ReplLoop     = [&]() {
            while (true) {
                vector<bool> scope;
                string line;
                string input;

                cout << "[JKRGUI v2.0a]>> ";

                int scope_start = 0;
                int scope_end   = 0;

                while (getline(cin, line)) {
                    scope_start += c(line, "function") + c(line, "if") + c(line, "for") +
                                   c(line, "while") + c(line, "(") + c(line, "{") +
                                   (line.find("[[") != string::npos);
                    scope_end += c(line, "end") + c(line, ")") + c(line, "}") + c(line, "]]");

                    for (int i = 0; i < (scope_start - scope_end); ++i) {
                        scope.push_back(true);
                    }

                    if (scope_start == scope_end) {
                        scope.clear();
                        scope_start = 0;
                        scope_end   = 0;
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
                        auto Lock = std::scoped_lock(mutex);
                        mainThreadStatements.push_back(input);
                        std::cout << "\n";
                        break;
                    }
                }
            }
        };

        std::thread Thread(ReplLoop);

        while (not shouldQuit) {
            try {
                // shouldQuit = e->ShouldQuit();
                e->ProcessEvents();
                auto Lock = std::scoped_lock(mutex);
                if (not mainThreadStatements.empty()) {
                    mainState.safe_script(mainThreadStatements.front());
                    mainThreadStatements.clear();
                }
            } catch (const std::exception &e) {
                std::cout << "ERROR:: " << e.what() << "\n";
                mainThreadStatements.clear();
            }
        }

        Thread.join();
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
