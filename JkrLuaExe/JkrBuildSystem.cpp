#define FMT_HEADER_ONLY
#include "JkrLuaExe.hpp"
#include <filesystem>
#include <format>
#include <fstream>
#include <functional>
#include <sol/sol.hpp>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>

const string_view DefaultCMakeListsFile = R"CmakeListsFile(
cmake_minimum_required(VERSION 3.27)
)CmakeListsFile";

const string_view DefaultLuaLibraryFile = R"LuaLibraryString(
#include <lua.h>
#include <lauxlib.h>

int luaopen_{0}(lua_State *L) {{
    return 1;
}}
)LuaLibraryString";

string GetLuaLibraryDefaultString(const string_view inLibraryName) {
#ifdef ANDROID
          return " ";
#else
          return std::vformat(DefaultLuaLibraryFile, std::make_format_args(inLibraryName));
#endif
}

namespace JkrEXE {
namespace fs = std::filesystem;
namespace BuildSystem {

void CreateLuaLibraryEnvironment(
 sv inLibraryName, sv inJkrGUIRepoDirectory, sv inNativeDestinationDirectory, sv inBuildType, bool inManualLibLinking) {
          const v<sv> BuildType = {"x64-release", "x64-debug", "x86-relase", "x86-debug"};
          fs::path CurrentPath = fs::current_path();
          fs::path src = s(inJkrGUIRepoDirectory);
          fs::path NativeDestinationDirectory = s(inNativeDestinationDirectory);

          {
                    fs::path jkrgui_path = s(inJkrGUIRepoDirectory) + "/out/build/" + s(inBuildType) + "/jkrgui/jkrgui.lib";
                    fs::path jkrgui_dst_path = s(inNativeDestinationDirectory) + "jkrgui.lib";
                    fs::path mainCppFilePath = NativeDestinationDirectory / fs::path(string(inLibraryName) + ".cpp");
                    fs::path cmakeListsFilePath = NativeDestinationDirectory / fs::path("CMakeLists.txt");
                    std::ofstream mainCppFile(mainCppFilePath.string(), std::fstream::out);
                    std::ofstream cmakeListsFile(cmakeListsFilePath.string(), std::fstream::out);

                    if (not fs::exists(NativeDestinationDirectory)) fs::create_directory(inNativeDestinationDirectory);
                    if (not fs::exists(mainCppFilePath)) {
                              if (mainCppFile.is_open() and cmakeListsFile.is_open()) {
                                        mainCppFile << GetLuaLibraryDefaultString(inLibraryName);
                                        cmakeListsFile << "cmake_minimum";
                              } else {
                                        std::cout << "Fuck, Files are not open";
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