#include "JkrLuaExe.hpp"
#include <algorithm>
#include <filesystem>
#include <format>
#include <fstream>
#include <functional>
#include <sol/sol.hpp>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>

const string_view DefaultCMakeListsFile = R"CmakeListsFile(cmake_minimum_required(VERSION 3.27)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
include("{0}/CMakeConfig.cmake")
include("{0}/CMakeCommon.cmake")
include("{0}/out/build/{1}/luaExport.cmake")
include("{0}/out/build/{1}/ksaivulkanExport.cmake")
include("{0}/out/build/{1}/jkrguiExport.cmake")


project({2})
add_library({2} SHARED {2}.cpp)
target_link_libraries({2} lua ksaivulkan jkrgui)
configure_target({2})

add_custom_command(
    TARGET {2} POST_BUILD
    COMMAND ${{CMAKE_COMMAND}} -E copy
            $<TARGET_FILE:{2}>
            ${{CMAKE_CURRENT_SOURCE_DIR}}/../
)

)CmakeListsFile";

const string_view DefaultLuaLibraryFile = R"LuaLibraryString(#include <sol/sol.hpp>
#ifdef _WIN32
    #define DLLEXPORT __declspec(dllexport)
#else
    #define DLLEXPORT
#endif

extern "C" DLLEXPORT int luaopen_{0}(lua_State *L) {{
        sol::state_view s(L);
        auto jkrguiApp = s["jkrguiApp"].get_or_create<sol::table>();
        jkrguiApp.set_function("hello", []() {{ std::cout << "Hello World from jkrguiApp\n"; }});
        return 1;
}}
)LuaLibraryString";

// TODO For android disable this subsystem entirely

string GetLuaLibraryDefaultString(const string_view inLibraryName) {
#ifdef ANDROID
          return " ";
#else
          return std::vformat(DefaultLuaLibraryFile, std::make_format_args(inLibraryName));
#endif
}

string GetLuaCMakeListsDefaultString(const string_view inJkrguiPath, const string_view inBuildType, const string_view inLibraryName) {
#ifdef ANDROID
          return " ";
#else
          return std::vformat(DefaultCMakeListsFile, std::make_format_args(inJkrguiPath, inBuildType, inLibraryName));
#endif
}

namespace JkrEXE {
namespace fs = std::filesystem;
namespace BuildSystem {

void CreateLuaLibraryEnvironment(sv inLibraryName, sv inJkrGUIRepoDirectory, sv inNativeDestinationDirectory, sv inBuildType, bool inOverride) {
          fs::path CurrentPath = fs::current_path();
          fs::path JkrGuiRepoPath = s(inJkrGUIRepoDirectory);
          fs::path NativeDestinationDirectory = s(inNativeDestinationDirectory);

          {
                    fs::path mainCppFilePath = NativeDestinationDirectory / fs::path(string(inLibraryName) + ".cpp");
                    fs::path cmakeListsFilePath = NativeDestinationDirectory / fs::path("CMakeLists.txt");
                    fs::path cmakePresetsFilePath = NativeDestinationDirectory / fs::path("CMakePresets.json");
                    fs::path cmakePresetsJkrGuiLibraryPath = JkrGuiRepoPath / fs::path("CMakePresets.json");

                    if (not fs::exists(NativeDestinationDirectory)) fs::create_directory(inNativeDestinationDirectory);

                    if (not fs::exists(cmakeListsFilePath) or inOverride) {
                              std::ofstream cmakeListsFile(cmakeListsFilePath, std::fstream::out);
                              if (cmakeListsFile.is_open()) {
                                        cmakeListsFile << GetLuaCMakeListsDefaultString(inJkrGUIRepoDirectory, inBuildType, inLibraryName);
                              } else {
                                        std::cout << "Fuck, Files are not open";
                              }
                              if (not fs::exists(cmakePresetsFilePath)) fs::copy_file(cmakePresetsJkrGuiLibraryPath, cmakePresetsFilePath);
                    }

                    if (not fs::exists(mainCppFilePath) or inOverride) {
                              std::ofstream mainCppFile(mainCppFilePath, std::fstream::out);
                              if (mainCppFile.is_open()) {
                                        mainCppFile << GetLuaLibraryDefaultString(inLibraryName);
                              } else {
                                        std::cout << "Fuck, File is not open";
                              }
                    }
          }
}

}; // namespace BuildSystem

static umap<s, int> CommandLineCommandsArgumentCount = {{"--build", 0}};
int GetArgumentCount(sv inString) { return CommandLineCommandsArgumentCount[s(inString)]; }

umap<s, v<s>> CommandLine(int ArgCount, char** ArgStrings) {
          umap<s, v<s>> CommandLineStuff;
          int FirstArgIndex = 1;
          for (int i = FirstArgIndex; i < ArgCount; i++) {
                    const s arg(ArgStrings[i]);
                    const int ArgumentCount = GetArgumentCount(arg);
                    if (ArgumentCount == 0) {
                              CommandLineStuff[arg] = v<s>{""};
                    } else if (i + 1 < ArgCount) {
                              v<s> arguments;
                              for (int j = i; j < i + ArgumentCount; j++) {
                                        const s new_arg(ArgStrings[i]);
                                        i++;
                              }
                    } else {
                              std::cout << "Error:\n";
                    }
          }

          return CommandLineStuff;
}

void CreateBuildSystemBindings(sol::state& inS) {
          auto Jkr = inS["Jkr"].get_or_create<sol::table>();
          auto Build = Jkr["BuildSystem"].get_or_create<sol::table>();
          Build.set_function("CreateLuaLibraryEnvironment", &BuildSystem::CreateLuaLibraryEnvironment);
}

} // namespace JkrEXE