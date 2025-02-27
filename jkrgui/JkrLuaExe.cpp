#define TRACY_IMPORTS
#include "EventManager.hpp"
#include "JkrLuaExe.hpp"
#include <SDLWindow.hpp>
#include <CLI11/CLI11.hpp>
#include <Misc/JkrFile.hpp>

#ifdef _WIN32
    #include <TracyLua.hpp>
#endif

// For Command Line Stuff
bool FlagBuild                        = false;
bool FlagGenerate                     = false;
bool FlagGenerateRun                  = false;
bool FlagRepl                         = false;
bool FlagCreateAndroidEnvironment     = false;
std::string OptionAndroidAppName      = "JkrGUIv2";
std::string OptionAndroidDirName      = "android";
std::string OptionAndroidBuildDirName = "android-arm64-v8a";
std::string OptionRun                 = "";

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
#ifdef _WIN32
    tracy::LuaRegister(s);
#endif
}
} // namespace JkrEXE

using namespace JkrEXE;
static sol::state mainState;
sol::state &GetMainStateRef() { return mainState; }

void RunScript() {
    CreateMainBindings(mainState);
    auto Jkr = GetMainStateRef()["Jkr"].get_or_create<sol::table>();
    if (OptionRun != "")
    {
#ifndef ANDROID
        Jkr.set_function("CreateAndroidEnvironment", &BuildSystem::CreateAndroidEnvironment);
#endif
        if (OptionRun.find(".lua") == s::npos) {
            OptionRun = OptionRun + ".lua";
        }
        sol::protected_function_result result_ =
            mainState.safe_script_file(OptionRun, &sol::script_pass_on_error);
        if (not result_.valid()) {
            sol::error error = result_;
            Log(error.what(), "ERROR");
            LuaShowToastNotification(std::string(error.what()));
        }
    } else { 
        if(std::filesystem::exists("main.jkr"))
        {
            LuaShowToastNotification("JkrGUI Started");
            auto File = Jkr::Misc::File("main.jkr");
            int fcount = File.Read<int>("func_count");
            for(int i = 1; i <= fcount; ++i)
            {
                    sol::basic_bytecode<> code = File.Read<decltype(code)>(s("func") + std::to_string(i));
                    sol::protected_function_result result = 
                    GetMainStateRef().script(std::string(code.as_string_view()));
                    if (not result.valid()) {
                        sol::error error = result;
                        Log(error.what(), "ERROR");
                        LuaShowToastNotification(std::string(error.what()));
                    }
            }
        }
    }
}

#ifndef ANDROID
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
    ///
    app.add_option("--appname", OptionAndroidAppName, "Android App Name");
    app.add_option("--dirname", OptionAndroidDirName, "Android Dir Name");
    app.add_option("--builddir", OptionAndroidBuildDirName, "Android Build Directory Name");
    app.add_option("--run", OptionRun, "Run File, Defaults to app.lua");

    app.parse(ArgCount, ArgStrings);

    if (FlagCreateAndroidEnvironment) {
        /// @warning Architecture is not considered, arm64-v8a default
        BuildSystem::CreateAndroidEnvironment(
             OptionAndroidAppName, OptionAndroidDirName, OptionAndroidBuildDirName);
    }

    auto Update = []() {
        filesystem::path src = std::string(getenv("JKRGUI_DIR"));
        src /= "luaproject";
        filesystem::path dest = filesystem::current_path();
        Log("Current Directory: " + filesystem::current_path().string());
        Log("Copying from:" + src.string());
        Log("Copying to:" + dest.string());
        filesystem::copy(src / "bundle.lua", dest / "bundle.lua", filesystem::copy_options::update_existing);
        filesystem::copy(src / "app.lua", dest / "app.lua", filesystem::copy_options::skip_existing);
        filesystem::copy(src / "res", dest / "res", filesystem::copy_options::recursive | filesystem::copy_options::skip_existing);
        filesystem::copy(src / ".vscode", dest / ".vscode", filesystem::copy_options::recursive | filesystem::copy_options::skip_existing);
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
                        printf("\n");
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
                Log(e.what(), "ERROR");
                mainThreadStatements.clear();
            }
        }

        Thread.join();
    }
}
#endif

JKR_EXPORT int main(int ArgCount, char **ArgStrings) {
#ifndef ANDROID
    ProcessCmdLine(ArgCount, ArgStrings);
#endif
    RunScript();
    return 0;
}
